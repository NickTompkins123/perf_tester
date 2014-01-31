/**
* \file
* Scene element contains root node of scene graph, composer, camera and set of properties.
* Element that constructs the scene visually.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_scene.h"

#include "kzu_object.h"
#include "kzu_mesh.h"
#include "kzu_instanciator.h"
#include "bounding_volume/kzu_bounding_volume.h"
#include "kzu_transformed_object.h"

#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/ui/kzu_ui_screen.h>
#include <user/ui/kzu_ui_component.h>
#include <user/animation/kzu_animation_player.h>
#include <user/renderer/kzu_composer.h>
#include <user/renderer/kzu_render_pass.h>
#include <user/filter/kzu_object_source.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_queue.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>
#include <system/time/kzs_tick.h>


/* Forward declarations */
struct KzuComposer;



/** Defines a scene structure. */
struct KzuScene
{
    struct KzuSceneData* data;
};

struct KzuSceneMeasurement
{
    kzUint layoutingTime; /**< Time for layouting. */
    kzUint extractTime; /**< Time for extracting. */
    kzUint transformTime; /**< Time for transform. */
    kzUint arrangeTime; /**< Time for arrange. */
    kzUint constraintTime; /**< Time for constraints. */
    kzUint animationTime; /**< Time for animation. */
};

struct KzuSceneObjectIterator
{
    struct KzcQueue* objectQueue;
    struct KzuObjectNode* currentObject;
};

struct KzuSceneData
{
    kzBool selfOwned;
    struct KzuPropertyManager* propertyManager; /**< Scene property manager. */
    struct KzuObjectNode* rootNode;           /**< Direct root node of the Scene. */
    kzBool customRoot;
    struct KzuComposer* currentComposer;      /**< Current composer for the scene.  */    
    
    struct KzuCameraNode* viewCamera;         /**< Scene's view camera. */
    struct KzuCameraNode* viewCamera2D;         /**< Scene's view camera. */
    kzMutableString viewCameraPath;           /**< Scene's view camera path. */
    struct KzuAnimationPlayer* animationPlayer; /**< Scene's animation player. */

    struct KzuUiScreen* screen; /**< Scene's ui screen. */

    struct KzuSceneMeasurement measurement; /**< Scene measurement. */

    kzMutableString name; /**< Name of the scene. */
};


/** Helper function. Traverses scene graph object iterator recursively. */
static kzsError kzuSceneObjectsGetIteratorTraverse_internal(struct KzuSceneObjectIterator* it, struct KzuObjectNode* objectNode);
/** Gets object iterator from scene. */
static kzsError kzuSceneObjectsGetIterator_internal(const struct KzuScene* scene, const struct KzcMemoryManager* memoryManager,
                                                    struct KzuSceneObjectIterator* it);
/** Iterates scene objects. */
static kzBool kzuSceneObjectsIterate_internal(struct KzuSceneObjectIterator* it);
/** Gets iterator value from scene object iterator. */
static struct KzuObjectNode* kzuSceneObjectsIteratorGetValue_internal(const struct KzuSceneObjectIterator* it);


kzsError kzuSceneCreateWithRoot(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                kzString name, struct KzuObjectNode* rootNode, struct KzuScene** out_scene)
{
    kzsError result;
    struct KzuScene* scene;
    struct KzuSceneData* sceneData;

    result = kzcMemoryAllocVariable(memoryManager, scene, "Scene");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, sceneData, "Scene data");
    kzsErrorForward(result);

    scene->data = sceneData;
    sceneData->selfOwned = KZ_TRUE;
    sceneData->propertyManager = propertyManager;

    result = kzuAnimationPlayerCreate(memoryManager, &sceneData->animationPlayer);
    kzsErrorForward(result);

    result = kzuUiScreenCreate(memoryManager, &sceneData->screen);
    kzsErrorForward(result);

    result = kzcStringCopy(memoryManager, name, &sceneData->name);
    kzsErrorForward(result);

    /* Define composer as null. */
    sceneData->currentComposer = KZ_NULL;
    /* View camera is null as default. */
    sceneData->viewCamera = KZ_NULL;
    sceneData->viewCameraPath = KZ_NULL;
    sceneData->rootNode = rootNode;
    sceneData->customRoot = KZ_TRUE;
    scene->data->measurement.extractTime = 0;
    scene->data->measurement.transformTime = 0;
    scene->data->measurement.layoutingTime = 0;
    scene->data->measurement.arrangeTime = 0;
    scene->data->measurement.constraintTime = 0;
    scene->data->measurement.animationTime = 0;


    *out_scene = scene;
    kzsSuccess();
}

kzsError kzuSceneCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                        kzString name, struct KzuScene** out_scene)
{
    kzsError result;
    struct KzuObjectNode* rootNode;
    struct KzuScene* scene;

    result = kzuEmptyNodeCreate(memoryManager, propertyManager, messageDispatcher, "Root", &rootNode);
    kzsErrorForward(result);

    result = kzuSceneCreateWithRoot(memoryManager, propertyManager, name, rootNode, &scene);
    kzsErrorForward(result);

    scene->data->customRoot = KZ_FALSE;

    *out_scene = scene;
    kzsSuccess();
}

kzsError kzuSceneDelete(struct KzuScene* scene)
{
    kzsError result;
    struct KzuSceneData* sceneData;
    kzBool selfOwned;

    kzsAssert(kzuSceneIsValid(scene));

    sceneData = scene->data;
    selfOwned = sceneData->selfOwned;

    result = kzuUiScreenDelete(sceneData->screen);
    kzsErrorForward(result);

    result = kzuAnimationPlayerDelete(sceneData->animationPlayer);
    kzsErrorForward(result);

    result = kzuPropertyManagerRemoveProperties(sceneData->propertyManager, scene);
    kzsErrorForward(result);

    if(sceneData->name != KZ_NULL)
    {
        result = kzcStringDelete(sceneData->name);
        kzsErrorForward(result);
    }

    if (!sceneData->customRoot)
    {
        result = kzuObjectNodeDelete(sceneData->rootNode);
        kzsErrorForward(result);
    }

    if (sceneData->viewCameraPath != KZ_NULL)
    {
        result = kzcStringDelete(sceneData->viewCameraPath);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(sceneData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(scene);
        kzsErrorForward(result);
    }
    else
    {
        scene->data = KZ_NULL;
    }

    kzsSuccess();
}

/* TODO: Consider renaming. */
void kzuSceneTakeOwnership(const struct KzuScene* scene)
{
    kzsAssert(kzuSceneIsValid(scene));

    scene->data->selfOwned = KZ_FALSE;
}

kzBool kzuSceneIsValid(const struct KzuScene* scene)
{
    return scene != KZ_NULL && scene->data != KZ_NULL;
}

void kzuSceneTransferData(struct KzuScene* targetScene, struct KzuScene* sourceScene)
{
    kzsAssert(kzcIsValidPointer(targetScene));
    kzsAssert(targetScene->data == KZ_NULL);
    kzsAssert(kzuSceneIsValid(sourceScene));

    targetScene->data = sourceScene->data;
    sourceScene->data = KZ_NULL;
}

static kzsError kzuSceneObjectsGetIteratorTraverse_internal(struct KzuSceneObjectIterator* it, struct KzuObjectNode* objectNode)
{
    kzsError result;
    struct KzcDynamicArrayIterator childIterator;
    
    result = kzcQueueAdd(it->objectQueue, objectNode);
    kzsErrorForward(result);

    childIterator = kzuObjectNodeGetChildren(objectNode);
    while(kzcDynamicArrayIterate(childIterator))
    {
        struct KzuObjectNode* childNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(childIterator);
        result = kzuSceneObjectsGetIteratorTraverse_internal(it, childNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

static kzsError kzuSceneObjectsGetIterator_internal(const struct KzuScene* scene, const struct KzcMemoryManager* memoryManager,
                                                    struct KzuSceneObjectIterator* it)
{
    kzsError result;
    
    result = kzcQueueCreate(memoryManager, &it->objectQueue);
    kzsErrorForward(result);

    result = kzcQueueAdd(it->objectQueue, kzuSceneGetRootNode(scene));
    kzsErrorForward(result);
    result = kzuSceneObjectsGetIteratorTraverse_internal(it, kzuSceneGetRootNode(scene));
    kzsErrorForward(result);

    kzsSuccess();
}

static kzBool kzuSceneObjectsIterate_internal(struct KzuSceneObjectIterator* it)
{
    kzsError result;
    kzBool objectFound = KZ_FALSE;
    it->currentObject = KZ_NULL;
    if(!kzcQueueIsEmpty(it->objectQueue))
    {
        result = kzcQueueReceive(it->objectQueue, (void**)&it->currentObject);
        kzsAssert(result == KZS_SUCCESS);
        objectFound = KZ_TRUE;
    }
    else
    {
        result = kzcQueueDelete(it->objectQueue);
        kzsAssert(result == KZS_SUCCESS);
    }

    return objectFound;
}

static struct KzuObjectNode* kzuSceneObjectsIteratorGetValue_internal(const struct KzuSceneObjectIterator* it)
{
    return it->currentObject;
}

kzUint kzuSceneGetPerformanceMeasurement(const struct KzuScene* scene, enum KzuScenePerformanceMeasurement measurement)
{
    kzUint time = 0;
    switch(measurement)
    {
        case KZU_SCENE_PERFORMANCE_MEASUREMENT_LAYOUT: time = scene->data->measurement.layoutingTime; break;
        case KZU_SCENE_PERFORMANCE_MEASUREMENT_EXTRACT: time = scene->data->measurement.extractTime; break;
        case KZU_SCENE_PERFORMANCE_MEASUREMENT_TRANSFORM: time = scene->data->measurement.transformTime; break;
        case KZU_SCENE_PERFORMANCE_MEASUREMENT_ARRANGE: time = scene->data->measurement.arrangeTime; break;
        case KZU_SCENE_PERFORMANCE_MEASUREMENT_CONSTRAINTS: time = scene->data->measurement.constraintTime; break;
        case KZU_SCENE_PERFORMANCE_MEASUREMENT_ANIMATION: time = scene->data->measurement.animationTime; break;
        default: kzsAssertText(KZ_FALSE, "Invalid scene measurement enum");
    }
    return time;
}

kzsError kzuSceneApplyConstraints(const struct KzuScene* scene, const struct KzcMemoryManager* quickMemoryManager)
{
    kzsError result;
    kzUint stamp;
    kzsAssert(kzuSceneIsValid(scene));

    stamp = kzsTimeGetCurrentTimestamp();

    /* LookAt. */
    {
        struct KzuSceneObjectIterator it;
        kzBool objectFound;

        result = kzuSceneObjectsGetIterator_internal(scene, quickMemoryManager, &it);
        kzsErrorForward(result);

        while(kzuSceneObjectsIterate_internal(&it))
        {
            struct KzuObjectNode* objectNode = (struct KzuObjectNode*)kzuSceneObjectsIteratorGetValue_internal(&it);
            struct KzuObjectNode* targetObjectNode = kzuPropertyManagerGetVoidDefault(kzuObjectNodeGetPropertyManager(objectNode), objectNode, KZU_PROPERTY_TYPE_LOOK_AT);
            if(targetObjectNode != KZ_NULL)
            {
                struct KzcVector3 startPoint;
                struct KzcVector3 endPoint;
                struct KzcVector3 lookAtVector;
                struct KzcVector3 upVector = KZC_VECTOR3_Y_AXIS;
                struct KzcVector3 right;
                struct KzcVector3 up;
                struct KzcMatrix4x4 currentMatrix;
                struct KzcMatrix4x4 targetMatrix;
                kzFloat scaleX, scaleY, scaleZ;

                objectFound = kzuObjectNodeCalculateRelativeTransformation(kzuSceneGetRootNode(scene), objectNode, &currentMatrix);
                if(!objectFound)
                {
                    continue;
                }
                objectFound = kzuObjectNodeCalculateRelativeTransformation(kzuSceneGetRootNode(scene), targetObjectNode, &targetMatrix);
                if(!objectFound)
                {
                    continue;
                }
                scaleX = kzcMatrix4x4GetScaleX(&currentMatrix);
                scaleY = kzcMatrix4x4GetScaleY(&currentMatrix);
                scaleZ = kzcMatrix4x4GetScaleZ(&currentMatrix);

                kzcMatrix4x4GetPosition(&currentMatrix, &startPoint);
                kzcMatrix4x4GetPosition(&targetMatrix, &endPoint);

                kzcVector3Subtract(&endPoint, &startPoint, &lookAtVector);

                /* Calculate the forward vector Z */
                kzcVector3Normalize(&lookAtVector);
                kzcVector3Normalize(&upVector);

                /* Right vector X = Y x Z. */
                if(!kzcVector3IsEqual(&lookAtVector, &upVector))
                {
                    kzcVector3Cross(&lookAtVector, &upVector, &right);
                }
                else
                {
                    right = KZC_VECTOR3_X_AXIS;
                }
                kzcVector3Normalize(&right);

                /* Up vector Y = Z x X */
                kzcVector3Cross(&right, &lookAtVector, &up);
                kzcVector3Normalize(&up);

                currentMatrix.data[0] = kzcVector3GetX(&right) * scaleX; currentMatrix.data[1] = kzcVector3GetY(&right) * scaleX; currentMatrix.data[2] = kzcVector3GetZ(&right) * scaleX;
                currentMatrix.data[4] = kzcVector3GetX(&up) * scaleY; currentMatrix.data[5] = kzcVector3GetY(&up) * scaleY; currentMatrix.data[6] = kzcVector3GetZ(&up) * scaleY;
                currentMatrix.data[8] = -kzcVector3GetX(&lookAtVector) * scaleZ; currentMatrix.data[9] = -kzcVector3GetY(&lookAtVector) * scaleZ; currentMatrix.data[10] = -kzcVector3GetZ(&lookAtVector) * scaleZ;

                {
                    struct KzuObjectNode* parentNode = kzuObjectNodeGetParent(objectNode);
                    if(parentNode != KZ_NULL)
                    {
                        struct KzcMatrix4x4 inverseParent;
                        struct KzcMatrix4x4 parentTransformation = kzuObjectNodeGetTransformation(parentNode);
                        objectFound = kzuObjectNodeCalculateRelativeTransformation(kzuSceneGetRootNode(scene), parentNode, &parentTransformation);
                        if(!objectFound)
                        {
                            continue;
                        }

                        kzcMatrix4x4Inverse(&parentTransformation, &inverseParent);
                        kzcMatrix4x4Multiply(&currentMatrix, &inverseParent, &parentTransformation);
                        currentMatrix = parentTransformation;
                    }
                }

                result = kzuObjectNodeSetTransformation(objectNode, &currentMatrix);
                kzsErrorForward(result);
            }
        }
    }

    scene->data->measurement.constraintTime = (kzsTimeGetCurrentTimestamp() - stamp);

    kzsSuccess();
}

kzsError kzuSceneAnimate(const struct KzuScene* scene, kzUint deltaTime)
{
    kzsError result;
    kzUint stamp = kzsTimeGetCurrentTimestamp();
    kzsAssert(kzuSceneIsValid(scene));

    result = kzuAnimationPlayerUpdate(scene->data->animationPlayer, (kzFloat)deltaTime / 1000.0f);
    kzsErrorForward(result);

    scene->data->measurement.animationTime = (kzsTimeGetCurrentTimestamp() - stamp);
    kzsSuccess();
}

kzsError kzuSceneExtractNode(const struct KzuScene* scene, struct KzcMemoryManager* memoryManager, struct KzcDynamicArray* extractedObjectList,
                             struct KzuObjectNode* objectNode, struct KzuTransformedObjectNode** out_transformedNode)
{
    kzsError result;
    struct KzuTransformedObjectNode* transformedNode = KZ_NULL;

    if (kzuObjectNodeIsLayout(objectNode))
    {
        /* Initiate full measure pass because the node wants to arrange children later. */        
        result = kzuSceneMeasureNode(scene, memoryManager, extractedObjectList, objectNode, &transformedNode);
        kzsErrorForward(result);
    }
    else
    {
        struct KzcDynamicArrayIterator it;
        kzUint stamp = kzsTimeGetCurrentTimestamp();

        /* Create transformed object node from object. */
        result = kzuTransformedObjectNodeCreateFromObjectNode(memoryManager, objectNode, &transformedNode);
        kzsErrorForward(result);

        /* Add transformed node to extracted object list. */
        result = kzcDynamicArrayAdd(extractedObjectList, transformedNode);
        kzsErrorForward(result);

        scene->data->measurement.extractTime += (kzsTimeGetCurrentTimestamp() - stamp);

        /* Go through children and extract them. */
        it = kzuObjectNodeGetChildren(objectNode);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuObjectNode* childNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);

            /* Get transformed node from the child node. */
            struct KzuTransformedObjectNode* transformedChildNode;
            result = kzuSceneExtractNode(scene, memoryManager, extractedObjectList, childNode, &transformedChildNode);
            kzsErrorForward(result);

            /* Attach the child. */
            result = kzuTransformedObjectNodeAddChildNode(transformedNode, transformedChildNode);
            kzsErrorForward(result);
        }

        
        if (kzuObjectNodeGetType(objectNode) == KZU_OBJECT_TYPE_INSTANCIATOR)
        {
            struct KzuInstanciatorNode* instanciatorNode = kzuInstanciatorNodeFromObjectNode(objectNode);
            struct KzuObjectNode* targetNode = kzuInstanciatorNodeGetTarget(instanciatorNode);

            if (targetNode != KZ_NULL)
            {
                struct KzuTransformedObjectNode* transformedTargetNode;

                /* Get transformed node from the target node. */
                result = kzuSceneExtractNode(scene, memoryManager, extractedObjectList, targetNode, &transformedTargetNode);
                kzsErrorForward(result);

                stamp = kzsTimeGetCurrentTimestamp();
                /* Attach the target. */
                result = kzuTransformedObjectNodeAddChildNode(transformedNode, transformedTargetNode);
                kzsErrorForward(result);
                scene->data->measurement.extractTime += (kzsTimeGetCurrentTimestamp() - stamp);
            }
        }
    }

    *out_transformedNode = transformedNode;
    kzsSuccess();
}

kzsError kzuSceneMeasureNode(const struct KzuScene* scene, struct KzcMemoryManager* memoryManager, struct KzcDynamicArray* extractedObjectList,
                             struct KzuObjectNode* objectNode, struct KzuTransformedObjectNode** out_transformedNode)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    kzBool measured = KZ_FALSE;
    struct KzuTransformedObjectNode* transformedNode;
    kzUint stamp;

    stamp = kzsTimeGetCurrentTimestamp();

    /* Create transformed object node from object. */
    result = kzuTransformedObjectNodeCreateFromObjectNode(memoryManager, objectNode, &transformedNode);
    kzsErrorForward(result);

    /* Allocate UI data. */
    result = kzuTransformedObjectNodeAllocateUiData(memoryManager, transformedNode);
    kzsErrorForward(result);

    /* Add transformed node to extracted object list. */
    result = kzcDynamicArrayAdd(extractedObjectList, transformedNode);
    kzsErrorForward(result);

    scene->data->measurement.layoutingTime += (kzsTimeGetCurrentTimestamp() - stamp);

    /* Go through children and measure them. */
    it = kzuObjectNodeGetChildren(objectNode);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuObjectNode* childNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);

        /* Get transformed node from the child node. */
        struct KzuTransformedObjectNode* transformedChildNode;
        result = kzuSceneMeasureNode(scene, memoryManager, extractedObjectList, childNode, &transformedChildNode);
        kzsErrorForward(result);

        /* Attach the child. */
        stamp = kzsTimeGetCurrentTimestamp();
        result = kzuTransformedObjectNodeAddChildNode(transformedNode, transformedChildNode);
        kzsErrorForward(result);
        scene->data->measurement.layoutingTime += (kzsTimeGetCurrentTimestamp() - stamp);
    }

    stamp = kzsTimeGetCurrentTimestamp();
    /* Measure self after all children. */
    if (kzuObjectNodeIsLayout(objectNode))
    {
        struct KzuUiComponentNode* componentNode = kzuUiComponentNodeFromObjectNode(objectNode);
        KzuUiComponentMeasureFunction measureFunction = kzuUiComponentNodeGetMeasureFunction(componentNode);

        kzsAssert(measureFunction != KZ_NULL);
        
        result = measureFunction(transformedNode);
        kzsErrorForward(result);
        measured = KZ_TRUE;
    }

    /* The node is not a component or doesn't have a measure function. */
    if (!measured)
    {
        /* Measure the union of all children. */        
        struct KzcVector3 aabbMax = kzcVector3(-KZ_FLOAT_MAXIMUM, -KZ_FLOAT_MAXIMUM, -KZ_FLOAT_MAXIMUM);
        struct KzcVector3 aabbMin = kzcVector3(KZ_FLOAT_MAXIMUM, KZ_FLOAT_MAXIMUM, KZ_FLOAT_MAXIMUM);
        struct KzuBoundingVolume* boundingVolume = kzuObjectNodeGetBoundingVolume(objectNode);
        struct KzcVector3 minimumCorner;
        struct KzcVector3 maximumCorner;

        it = kzuTransformedObjectNodeGetChildren(transformedNode);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzcVector3 childMin, childMax;
            struct KzuTransformedObjectNode* transformedChildNode = kzcDynamicArrayIteratorGetValue(it);
            struct KzuObjectNode* childObjectNode = kzuTransformedObjectNodeGetObjectNode(transformedChildNode);
            struct KzcVector3 childPosition = kzuObjectNodeGetPositionVector(childObjectNode);

            kzuTransformedObjectNodeGetDesiredSize(transformedChildNode, &childMin, &childMax);
            kzcVector3Add(&childMin, &childPosition, &childMin);
            kzcVector3Add(&childMax, &childPosition, &childMax);

            aabbMax = kzcVector3ComponentMax(&aabbMax, &childMax);
            aabbMin = kzcVector3ComponentMin(&aabbMin, &childMin);
        }

        if (boundingVolume != KZ_NULL)
        {
            /* If the node has a bounding volume, add its own aabb as well. */
            struct KzuBoundingVolumeAxisAlignedBox* boundingBox = kzuBoundingVolumeGetAxisAlignedBox(boundingVolume);
            minimumCorner = kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(boundingBox);
            maximumCorner = kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(boundingBox);

        }
        else
        {
            /* All other node types are measured as a point with size zero. */
            minimumCorner = KZC_VECTOR3_ZERO;
            maximumCorner = KZC_VECTOR3_ZERO;
        }
        aabbMax = kzcVector3ComponentMax(&aabbMax, &maximumCorner);
        aabbMin = kzcVector3ComponentMin(&aabbMin, &minimumCorner);

        /* Set the desired size. */
        {
            struct KzcVector3 userMin, userMax;
            struct KzcVector3 contentMin, contentMax;
            struct KzcVector3 desiredMin, desiredMax;

            /* Final content size is content size restricted by user defined dimensions. */
            kzuObjectNodeCalculateUserRestrictedSize(objectNode, &aabbMin, &aabbMax, &userMin, &userMax);

            {
                struct KzcMatrix4x4 transform;
                transform = kzuObjectNodeGetTransformation(objectNode);

                /* Transform the bounding box and calculate new enclosing aabb from that. */
                kzuAxisAlignedBoundingBoxFromTransformedAABB(&userMin, &userMax, &transform, &contentMin, &contentMax);
            }

            kzuTransformedObjectNodeSetContentSize(transformedNode, &contentMin, &contentMax);

            /* Calculate desired size. */
            kzuTransformedObjectNodeCalculateDesiredSize(transformedNode, &userMin, &userMax, &desiredMin, &desiredMax);
            kzuTransformedObjectNodeSetDesiredSize(transformedNode, &desiredMin, &desiredMax);
        }
    }
    scene->data->measurement.layoutingTime += (kzsTimeGetCurrentTimestamp() - stamp);

    *out_transformedNode = transformedNode;
    kzsSuccess();
}

kzsError kzuSceneTransformNode(const struct KzuScene* scene, struct KzuTransformedObjectNode* transformedNode, struct KzuTransformedObjectNode* transformedParentNode)
{
    kzsError result;

    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedNode);
    if (kzuObjectNodeGetType(objectNode) == KZU_OBJECT_TYPE_UI_COMPONENT &&
        /* TODO: Add kzuUiComponentIsLayoutRoot() */
        kzuUiComponentNodeGetArrangeFunction(kzuUiComponentNodeFromObjectNode(objectNode)) != KZ_NULL)
    {
        /* This is a top-level component node -> initiate arrange pass. Copy desired size to layout size. */
        struct KzcVector3 desiredMin, desiredMax;
        kzuTransformedObjectNodeGetDesiredSize(transformedNode, &desiredMin, &desiredMax);
        kzuTransformedObjectNodeSetLayoutSize(transformedNode, &desiredMin, &desiredMax);

        result = kzuSceneArrangeNode(scene, transformedNode, transformedParentNode);
        kzsErrorForward(result);
    }
    else
    {
        struct KzcDynamicArrayIterator it = kzuTransformedObjectNodeGetChildren(transformedNode);
        kzUint stamp = kzsTimeGetCurrentTimestamp();

        /* Apply parent transform if this is not root. */
        if (transformedParentNode != KZ_NULL)
        {
            struct KzcMatrix4x4 newTransform;
            struct KzcMatrix4x4 parentTransform = kzuTransformedObjectNodeGetMatrix(transformedParentNode);
            struct KzcMatrix4x4 transform = kzuTransformedObjectNodeGetMatrix(transformedNode);

            kzcMatrix4x4MultiplyAffine(&transform, &parentTransform, &newTransform);
            kzuTransformedObjectNodeSetMatrix(transformedNode, &newTransform);
        }
        scene->data->measurement.transformTime = (kzsTimeGetCurrentTimestamp() - stamp);

        /* Transform children. */
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuTransformedObjectNode* transformedChildNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            result = kzuSceneTransformNode(scene, transformedChildNode, transformedNode);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzuSceneArrangeNode(const struct KzuScene* scene, struct KzuTransformedObjectNode* transformedNode, struct KzuTransformedObjectNode* transformedParentNode)
{
    kzsError result;
    kzBool layoutHandled = KZ_FALSE;
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedNode);

    if (kzuObjectNodeGetType(objectNode) == KZU_OBJECT_TYPE_UI_COMPONENT)
    {
        struct KzuUiComponentNode* componentNode = kzuUiComponentNodeFromObjectNode(objectNode);
        KzuUiComponentArrangeFunction arrangeFunction = kzuUiComponentNodeGetArrangeFunction(componentNode);

        if (arrangeFunction != KZ_NULL)
        {
            result = arrangeFunction(scene, transformedNode, transformedParentNode);
            kzsErrorForward(result);
            layoutHandled = KZ_TRUE;
        }
    }

    if (!layoutHandled)
    {
        /* TODO: move initialization near the usage. */
        struct KzcDynamicArrayIterator it = kzuTransformedObjectNodeGetChildren(transformedNode);
        kzuTransformedObjectNodeDefaultArrange(transformedNode);

        /* Transform children. */
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuTransformedObjectNode* transformedChildNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            /* Full arrange pass not needed for descendants anymore -> transform. */
            result = kzuSceneTransformNode(scene, transformedChildNode, transformedNode);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzuSceneExtract(struct KzcMemoryManager* memoryManager, const struct KzuScene* scene,
                         struct KzcDynamicArray* extractedObjectList, struct KzuTransformedObjectNode** out_extractedRootNode)
{
    kzsError result;
    struct KzuTransformedObjectNode* extractedRootNode;
    kzUint stamp;
    kzUint totalTransformTime;
    
    kzsAssert(kzuSceneIsValid(scene));

    scene->data->measurement.layoutingTime = 0;
    scene->data->measurement.extractTime = 0;
    scene->data->measurement.transformTime = 0;
    result = kzuSceneExtractNode(scene, memoryManager, extractedObjectList, scene->data->rootNode, &extractedRootNode);
    kzsErrorForward(result);

    stamp = kzsTimeGetCurrentTimestamp();
    result = kzuSceneTransformNode(scene, extractedRootNode, KZ_NULL);
    kzsErrorForward(result);
    totalTransformTime = (kzsTimeGetCurrentTimestamp() - stamp);
    scene->data->measurement.arrangeTime = (totalTransformTime - scene->data->measurement.transformTime);

    *out_extractedRootNode = extractedRootNode;
    kzsSuccess();
}

void kzuSceneSetComposer(const struct KzuScene* scene, struct KzuComposer* composer)
{
    kzsAssert(kzuSceneIsValid(scene));
    kzsAssert(kzcIsValidPointer(composer));

    scene->data->currentComposer = composer;
}

struct KzuComposer* kzuSceneGetComposer(const struct KzuScene* scene)
{
    kzsAssert(kzuSceneIsValid(scene));

    return scene->data->currentComposer;
}

kzsError kzuSceneAddObject(const struct KzuScene* scene, struct KzuObjectNode* objectNode)
{
    kzsError result;
    kzsAssert(kzuSceneIsValid(scene));
    kzsAssert(kzcIsValidPointer(objectNode));

    result = kzuObjectNodeAddChild(scene->data->rootNode, objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuSceneRemoveObject(const struct KzuScene* scene, const struct KzuObjectNode* objectNode)
{
    kzsError result;
    kzsAssert(kzuSceneIsValid(scene));
    kzsAssert(kzcIsValidPointer(objectNode));
 
    result = kzuObjectNodeRemoveChild(scene->data->rootNode, objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuSceneSetRootNode(const struct KzuScene* scene, struct KzuObjectNode* rootNode)
{
    kzsError result;

    kzsAssert(kzuSceneIsValid(scene));

    if (!scene->data->customRoot && scene->data->rootNode != KZ_NULL)
    {
        result = kzuObjectNodeDelete(scene->data->rootNode);
        kzsErrorForward(result);
    }

    scene->data->rootNode = rootNode;
    scene->data->customRoot = KZ_TRUE;

    kzsSuccess();
}

struct KzuObjectNode* kzuSceneGetRootNode(const struct KzuScene* scene)
{
    kzsAssert(kzuSceneIsValid(scene));

    return scene->data->rootNode;
}

void kzuSceneSetViewCamera(const struct KzuScene* scene, struct KzuCameraNode* camera)
{
    kzsAssert(kzuSceneIsValid(scene));
    scene->data->viewCamera = camera;
}

void kzuSceneSetViewCamera2D(const struct KzuScene* scene, struct KzuCameraNode* camera2D)
{
    kzsAssert(kzuSceneIsValid(scene));
    scene->data->viewCamera2D = camera2D;
}

struct KzuCameraNode* kzuSceneGetViewCamera(const struct KzuScene* scene)
{
    kzsAssert(kzuSceneIsValid(scene));

    return scene->data->viewCamera;
}

struct KzuCameraNode* kzuSceneGetViewCamera2D(const struct KzuScene* scene)
{
    kzsAssert(kzuSceneIsValid(scene));

    return scene->data->viewCamera2D;
}

kzBool kzuSceneFindObjectNode(const struct KzuScene* scene, const struct KzuObjectNode* objectToFind)
{
    kzsAssert(kzuSceneIsValid(scene));
    kzsAssert(kzcIsValidPointer(objectToFind));

    return kzuObjectNodeFindObjectNode(kzuSceneGetRootNode(scene), objectToFind);
}

static struct KzuObjectSource* kzuSceneGetRootObjectSource_internal(const struct KzuComposer* composer)
{
    struct KzuObjectSource* rootObjectSource;

    kzsAssert(kzuComposerIsValid(composer));

    if (kzuComposerGetType(composer) == KZU_COMPOSER_TYPE_RENDER_PASS)
    {
        rootObjectSource =  kzuObjectSourceGetRoot(kzuRenderPassGetObjectSource(kzuRenderPassFromComposer(composer)));
    }
    else
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(composer->data->children);

        rootObjectSource = KZ_NULL;
        while (rootObjectSource == KZ_NULL && kzcDynamicArrayIterate(it))
        {
            struct KzuComposer* childComposer = (struct KzuComposer*)kzcDynamicArrayIteratorGetValue(it);

            rootObjectSource = kzuSceneGetRootObjectSource_internal(childComposer);
        }
    }

    return rootObjectSource;
}

struct KzuObjectSource* kzuSceneGetRootObjectSource(const struct KzuScene* scene)
{
    struct KzuObjectSource* rootObjectSource;
    struct KzuComposer* composer;

    kzsAssert(kzuSceneIsValid(scene));

    composer = scene->data->currentComposer;

    if (composer != KZ_NULL)
    {
        rootObjectSource = kzuSceneGetRootObjectSource_internal(composer);
    }
    else
    {
        rootObjectSource = KZ_NULL;
    }

    return rootObjectSource;
}

kzBool kzuSceneCalculateWorldTransformation(const struct KzuScene* scene,
                                            const struct KzuObjectNode* objectNode,
                                            struct KzcMatrix4x4* out_worldTransformation)
{
    kzBool result;

    kzsAssert(kzuSceneIsValid(scene));

    result = kzuObjectNodeCalculateRelativeTransformation(scene->data->rootNode, objectNode, out_worldTransformation);

    return result;
}

struct KzuAnimationPlayer* kzuSceneGetAnimationPlayer(const struct KzuScene* scene)
{
    kzsAssert(kzuSceneIsValid(scene));
    return scene->data->animationPlayer;
}

struct KzuUiScreen* kzuSceneGetUiScreen(const struct KzuScene* scene)
{
    kzsAssert(kzuSceneIsValid(scene));
    return scene->data->screen;
}

void kzuSceneHandleKeyEvents(const struct KzuScene* scene, enum KzsInputKey keyCode, enum KzsInputKeyDeviceState keyState)
{
    if(scene != KZ_NULL)
    {
        kzuUiScreenHandleKeyEvents(scene->data->screen, keyCode, keyState);
    }
}

void kzuSceneHandlePointerEvents(const struct KzuScene* scene, kzU32 buttons, enum KzsInputPointingDeviceState state, kzInt mouseX, kzInt mouseY)
{
    if(scene != KZ_NULL)
    {
        kzuUiScreenHandlePointerEvents(scene->data->screen, buttons, state, mouseX, mouseY);
    }
}

kzsError kzuSceneSetName(const struct KzuScene* scene, kzString name)
{
    kzsError result;
    kzsAssert(kzuSceneIsValid(scene));

    /* Delete previous name if possible. */
    if(scene->data->name != KZ_NULL)
    {
        result = kzcStringDelete(scene->data->name);
        kzsErrorForward(result);
    }

    result = kzcStringCopy(kzcMemoryGetManager(scene), name, &scene->data->name);
    kzsErrorForward(result);

    kzsSuccess();
}

kzString kzuSceneGetName(const struct KzuScene* scene)
{
    kzsAssert(kzuSceneIsValid(scene));

    return scene->data->name;
}
