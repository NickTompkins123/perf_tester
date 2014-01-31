/**
* \file
* Dynamic mesh for Kanzi Engine.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "dynamic_mesh.h"

#include <user/scene_graph/kzu_object_private.h>
#include <user/renderer/kzu_renderer_util.h>
#include <user/renderer/kzu_renderer.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_object.h>

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/color/kzc_color.h>
#include <core/util/curve/kzc_interpolation.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/renderer/kzc_renderer.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/debug/kzs_log.h>


struct DynamicMeshNode
{
    struct KzuObjectNode objectNode;            /**< Object node data. */
};

struct DynamicMeshNodeData
{
    struct KzuObjectNodeData    objectNodeData; /**< Inherited from object node, that contains transformation and child data. */ /*lint -esym(754, KzuMeshNodeData::objectNodeData) Suppressed as this member is required but not referenced. */ 
    kzBool selfOwned;
    DynamicMeshRenderCallback renderCallback;
    void* userData;
    kzFloat t;
};


extern const struct KzuObjectNodeClass DYNAMIC_MESH_CLASS;


kzsError dynamicMeshNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                               struct DynamicMeshNode** out_dynamicMeshNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode;
    struct DynamicMeshNodeData* meshNodeData;

    result = kzcMemoryAllocVariable(memoryManager, meshNodeData, "Dynamic mesh node");
    kzsErrorForward(result);

    meshNodeData->renderCallback = KZ_NULL;
    meshNodeData->userData = KZ_NULL;

    result = kzuObjectNodeCreate_private(memoryManager, propertyManager, messageDispatcher, &DYNAMIC_MESH_CLASS, "Dynamic_mesh", 
        (struct KzuObjectNodeData*)meshNodeData, &objectNode);
    kzsErrorForward(result);

    *out_dynamicMeshNode = (struct DynamicMeshNode*)objectNode;
    kzsSuccess();
}

kzsError dynamicMeshNodeDelete(struct DynamicMeshNode* dynamicMeshNode)
{
    kzsError result;

    result = kzuObjectNodeDeleteBase_private(&dynamicMeshNode->objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

struct DynamicMeshNodeData* dynamicMeshNodeGetDynamicMeshNodeData(const struct KzuObjectNode* objectNode)
{
   kzsAssert(kzcIsValidPointer(objectNode));

   return (struct DynamicMeshNodeData*)objectNode->data;
}


kzsError dynamicMeshRender(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode)
{
    kzsError result;
    struct KzuObjectNode* dynamicMeshNode = kzuTransformedObjectNodeGetObjectNode(transformedObjectNode);
    struct DynamicMeshNodeData* meshNodeData;

    kzsAssert(kzcIsValidPointer(dynamicMeshNode));

    meshNodeData = (struct DynamicMeshNodeData*)dynamicMeshNode->data;
    if(meshNodeData->renderCallback != KZ_NULL)
    {
        result = meshNodeData->renderCallback(renderer, transformedObjectNode, meshNodeData->userData);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

void dynamicMeshSetTime(const struct DynamicMeshNode* dynamicMesh, const kzFloat time)
{
    struct DynamicMeshNodeData* data;
    kzsAssert(kzcIsValidPointer(dynamicMesh));
    data = dynamicMeshNodeGetDynamicMeshNodeData(&dynamicMesh->objectNode);
    kzsAssert(kzcIsValidPointer(data));
    data->t = time;
}

void dynamicMeshSetCallback(const struct DynamicMeshNode* dynamicMeshNode, DynamicMeshRenderCallback callback, void* userData)
{
    struct DynamicMeshNodeData* dynamicMeshNodeData = dynamicMeshNodeGetDynamicMeshNodeData((struct KzuObjectNode*)dynamicMeshNode);
    kzsAssert(kzcIsValidPointer(dynamicMeshNodeData));
    dynamicMeshNodeData->renderCallback = callback;
    dynamicMeshNodeData->userData = userData;
}

kzsError dynamicMeshDelete(const struct KzuObjectNode* objectNode)
{
    kzsError result;
    struct DynamicMeshNodeData* dynamicMeshNodeData = dynamicMeshNodeGetDynamicMeshNodeData(objectNode);

    result = kzcMemoryFreeVariable(dynamicMeshNodeData);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError dynamicMeshCopier(const struct KzcMemoryManager* memoryManager, const struct KzuObjectNode* sourceObjectNode,
    struct KzuObjectNode** out_objectNode)
{
    KZ_UNUSED_PARAMETER(memoryManager);
    KZ_UNUSED_PARAMETER(sourceObjectNode);
    KZ_UNUSED_PARAMETER(out_objectNode);
    /* Not needed as object is not copied in this app. */
    kzsSuccess();
}

const struct KzuObjectNodeClass DYNAMIC_MESH_CLASS =
{
    dynamicMeshDelete,
    dynamicMeshCopier,
    dynamicMeshRender,
    KZ_NULL
};
