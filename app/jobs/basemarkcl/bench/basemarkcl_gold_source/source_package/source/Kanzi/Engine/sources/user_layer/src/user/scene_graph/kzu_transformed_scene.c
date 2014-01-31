/**
* \file
* Transformed scene object. Transformed scene is extracted from a scene object.
* After extracting it holds all the data needed to perform updates and to render the scene.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_transformed_scene.h"

#include <user/filter/kzu_object_source.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_hash_set.h>


struct KzuTransformedScene
{
    struct KzcMemoryManager* quickMemoryManager; /**< Quick memory manager for internal data. This is created only once for the whole transformed scene. */

    /* The following members are created in every extract call. */
    struct KzuScene* originalScene; /**< Original scene used to create this transformed scene. */
    struct KzcDynamicArray* transformedObjects; /**< Extracted object list. */
    struct KzuTransformedObjectNode* extractedRootNode; /**< Extracted (transformed) root node. */
    struct KzuObjectSource* rootObjectSource; /**< Root object source. */
    struct KzuObjectSourceRuntimeData* objectSourceRuntimeData; /**< Runtime data for object sources. */
};


kzsError kzuTransformedSceneCreate(const struct KzcMemoryManager* memoryManager, struct KzuTransformedScene** out_scene)
{
    kzUint quickMemorySize = 1 << 20;
    kzsError result;
    struct KzuTransformedScene* transformedScene;

    result = kzcMemoryAllocVariable(memoryManager, transformedScene, "TransformedScene");
    kzsErrorForward(result);

    /* Create quick memory manager. */
    result = kzcMemoryManagerCreateQuickManager(memoryManager, quickMemorySize, &transformedScene->quickMemoryManager);
    kzsErrorForward(result);

    transformedScene->originalScene = KZ_NULL;
    transformedScene->transformedObjects = KZ_NULL;
    transformedScene->extractedRootNode = KZ_NULL;
    transformedScene->rootObjectSource = KZ_NULL;
    transformedScene->objectSourceRuntimeData = KZ_NULL;

    *out_scene = transformedScene;
    kzsSuccess();
}

kzsError kzuTransformedSceneDelete(struct KzuTransformedScene* transformedScene)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(transformedScene));

    result = kzcMemoryManagerDelete(transformedScene->quickMemoryManager);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(transformedScene);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuTransformedSceneExtract(struct KzuTransformedScene* transformedScene, struct KzuScene* scene)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(transformedScene));

    /* Save the scene. */
    transformedScene->originalScene = scene;

    /* Reset quick memory. */
    result = kzcMemoryManagerResetQuickManager(transformedScene->quickMemoryManager);
    kzsErrorForward(result);

    /* Create list of extracted object nodes. */
    result = kzcDynamicArrayCreate(transformedScene->quickMemoryManager, &transformedScene->transformedObjects);
    kzsErrorForward(result);

    transformedScene->rootObjectSource = kzuSceneGetRootObjectSource(scene);
    kzuObjectSourceResetMeasurementInfo(transformedScene->rootObjectSource);

    /* Create runtime data for object sources. */
    result = kzuObjectSourceRuntimeDataCreate(transformedScene->quickMemoryManager, kzuSceneGetRootObjectSource(scene),
                                              transformedScene->transformedObjects,
                                              &transformedScene->objectSourceRuntimeData);
    kzsErrorForward(result);

    result = kzuSceneApplyConstraints(scene, transformedScene->quickMemoryManager);
    kzsErrorForward(result);

    result = kzuSceneExtract(transformedScene->quickMemoryManager, scene, transformedScene->transformedObjects, &transformedScene->extractedRootNode);
    kzsErrorForward(result);

    kzuTransformedSceneApplyPreRenderingProperties(transformedScene);

    kzsSuccess();
}

void kzuTransformedSceneApplyPreRenderingProperties(const struct KzuTransformedScene* transformedScene)
{
    struct KzcDynamicArrayIterator it;
    struct KzuTransformedObjectNode* cameraNode;
    struct KzuScene* scene;

    scene = kzuTransformedSceneGetScene(transformedScene);
    /* Face to camera. */
    if(kzuSceneGetViewCamera(scene) != KZ_NULL)
    {
        struct KzcDynamicArray* objectList = kzuTransformedSceneGetObjects(transformedScene);
        cameraNode = kzuTransformedObjectListFindObject(objectList, kzuCameraNodeToObjectNode(kzuSceneGetViewCamera(scene)));

        if(cameraNode != KZ_NULL)
        {
            struct KzcMatrix4x4 cameraMatrix;

            cameraMatrix = kzuTransformedObjectNodeGetMatrix(cameraNode);
            cameraMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] = 0.0f;
            cameraMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] = 0.0f;
            cameraMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] = 0.0f;

            it = kzcDynamicArrayGetIterator(objectList);

            /* Apply object face-to-camera properties. */
            while(kzcDynamicArrayIterate(it))
            {
                struct KzuTransformedObjectNode* transformedObjectNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
                kzBool faceToCamera = kzuObjectNodeGetBoolPropertyDefault(kzuTransformedObjectNodeGetObjectNode(transformedObjectNode), KZU_PROPERTY_TYPE_FACE_TO_CAMERA);

                /* Face to camera was enabled, adjust object world matrix according to camera orientation: */
                if (faceToCamera)
                {
                    kzFloat scaleX, scaleY, scaleZ;
                    struct KzcMatrix4x4 worldBillboardMatrix;
                    struct KzcMatrix4x4 worldMatrix;

                    worldMatrix = kzuTransformedObjectNodeGetMatrix(transformedObjectNode);

                    scaleX = kzcMatrix4x4GetScaleX(&worldMatrix);
                    scaleY = kzcMatrix4x4GetScaleY(&worldMatrix);
                    scaleZ = kzcMatrix4x4GetScaleZ(&worldMatrix);

                    worldBillboardMatrix = worldMatrix;
                    {
                        kzUint i, j;
                        for (i = 0; i < 3; ++i)
                        {
                            for (j = 0; j < 3; ++j)
                            {
                                worldBillboardMatrix.data[j * 4 + i] = cameraMatrix.data[j * 4 + i];
                            }
                        }
                    }
                    kzcMatrix4x4ScaleX(&worldBillboardMatrix, scaleX);
                    kzcMatrix4x4ScaleY(&worldBillboardMatrix, scaleY);
                    kzcMatrix4x4ScaleZ(&worldBillboardMatrix, scaleZ);
                    kzuTransformedObjectNodeSetMatrix(transformedObjectNode, &worldBillboardMatrix);
                }
            }
        }
    }
}

struct KzuScene* kzuTransformedSceneGetScene(const struct KzuTransformedScene* transformedScene)
{
    kzsAssert(kzcIsValidPointer(transformedScene));
    return transformedScene->originalScene;
}

struct KzuCameraNode* kzuTransformedSceneGetViewCamera(const struct KzuTransformedScene* transformedScene)
{
    kzsAssert(kzcIsValidPointer(transformedScene));
    return kzuSceneGetViewCamera(transformedScene->originalScene);
}

struct KzcDynamicArray* kzuTransformedSceneGetObjects(const struct KzuTransformedScene* transformedScene)
{
    kzsAssert(kzcIsValidPointer(transformedScene));
    return transformedScene->transformedObjects;
}

struct KzuObjectSource* kzuTransformedSceneGetRootObjectSource(const struct KzuTransformedScene* transformedScene)
{
    kzsAssert(kzcIsValidPointer(transformedScene));
    return transformedScene->rootObjectSource;
}

struct KzuObjectSourceRuntimeData* kzuTransformedSceneGetObjectSourceRuntimeData(const struct KzuTransformedScene* transformedScene)
{
    kzsAssert(kzcIsValidPointer(transformedScene));

    return transformedScene->objectSourceRuntimeData;
}
