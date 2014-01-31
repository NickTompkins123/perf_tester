/**
* \file
* Specifies level of detail filter.
* Level of detail filter filters any undesired level of detail object nodes that occur.
*
* Basic functionality for LOD filter:
*
* 1) Loop through scene graph, search for LOD nodes.
* 2) Add child nodes recursively to hash map, <KzuTransformedObjectNode, KzuLODFilterNode_internal>
* 3) When applying filter
*   - If node was not found from hash map, nothing is filtered.
*   - If node was found, see the hash map value and if LOD constraint is fulfilled, nothing is filtered,
*      otherwise node is selected away.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_lod_filter.h"

#include "kzu_object_source.h"
#include "kzu_object_source_private.h"

#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_lod_selector.h>

#include <core/util/collection/kzc_hash_map.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>
#include <system/time/kzs_tick.h>


/** Runtime data for level of detail filter. */
struct KzuLODFilterRuntimeData
{
    /** List of objects that are influenced by LOD. <KzuTransformedObjectNode, KzuLODFilterNode_internal>. */
    struct KzcHashMap* includedObjects;

    struct KzcMatrix4x4 cameraMatrix; /**< Calculated camera matrix. */
};

/** Structure for level of detail filter. Node's child object with ancestors is selected if it meets constraint the best. */
struct KzuLODFilter
{
    struct KzuFilterObjectSource filterObjectSource; /**< Inheritance. */
};

/** LOD filter level, specifies single LOD level data. */
struct KzuLODObjectInfo
{
    struct KzuLODSelectorNode* lodSelectorNode; /**< Level of detail reference. */
    struct KzuTransformedObjectNode* lodLevelNode; /**< Parent object. */
    struct KzuTransformedObjectNode* lodParentNode; /**< Parent object. */
};


/** Call-back function for per fetch initialization.of level of detail filter. */
static kzsError kzuLODFilterStart_internal(struct KzuFilterObjectSource* filterObjectSource,
                                                     const struct KzuObjectSourceRuntimeData* runtimeData,
                                                     const struct KzuTransformedObjectNode* camera,
                                                     const struct KzcDynamicArray* inputObjects);
/** Call-back function for applying level of detail filter. */
static kzBool kzuLODFilterApply_internal(const struct KzuFilterObjectSource* filterObjectSource,
                                                   const struct KzuObjectSourceRuntimeData* runtimeData,
                                                   const struct KzuTransformedObjectNode* camera,
                                                   const struct KzuTransformedObjectNode* transformedObject);


/** Filter configuration for level of detail filters. */
static const struct KzuFilterConfiguration KZU_LEVEL_OF_DETAIL_FILTER_CONFIGURATION =
{
    kzuLODFilterStart_internal, /* Start function */
    KZ_NULL, /* Delete function */
    kzuLODFilterApply_internal /* Apply function */
};


kzsError kzuLODFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                      struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;
    struct KzuLODFilter* lodFilter;

    result = kzcMemoryAllocVariable(memoryManager, lodFilter, "Level of detail filter");
    kzsErrorForward(result);

    result = kzuFilterObjectSourceInitialize_private(memoryManager, &lodFilter->filterObjectSource, input, KZ_FALSE,
                                                     &KZU_LEVEL_OF_DETAIL_FILTER_CONFIGURATION, &objectSource);
    kzsErrorForward(result);

    *out_objectSource = objectSource;
    kzsSuccess();
}

/** Adds object and its ancestors to hash map, which has value for parent object and LOD data. */
static kzsError kzuLODFilterAddAncestors_internal(const struct KzcMemoryManager* memoryManager,
                                                  struct KzcHashMap* includedObjects,
                                                  const struct KzuTransformedObjectNode* transformedObject,
                                                  struct KzuTransformedObjectNode* parentObject,
                                                  struct KzuTransformedObjectNode* lodParentNode,
                                                  struct KzuLODSelectorNode* lodSelectorNode)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    struct KzuLODObjectInfo* lodInfo;

    kzsAssert(kzcIsValidPointer(transformedObject));
    kzsAssert(kzcIsValidPointer(includedObjects));

    result = kzcMemoryAllocVariable(memoryManager, lodInfo, "LODConstraint");
    kzsErrorForward(result);
    /* Assign hash map value data, referencing for LOD data, which is used when applying filter. */
    lodInfo->lodLevelNode = parentObject;
    lodInfo->lodSelectorNode = lodSelectorNode;
    lodInfo->lodParentNode = lodParentNode;

    result = kzcHashMapPut(includedObjects, transformedObject, lodInfo);
    kzsErrorForward(result);

    it = kzuTransformedObjectNodeGetChildren(transformedObject);
    /* Add objects recursively. */
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* childTransformedObject = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        result = kzuLODFilterAddAncestors_internal(memoryManager, includedObjects, childTransformedObject,
                                                   parentObject, lodParentNode, lodSelectorNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuLODFilterStart_internal(struct KzuFilterObjectSource* filterObjectSource,
                                                       const struct KzuObjectSourceRuntimeData* runtimeData,
                                                       const struct KzuTransformedObjectNode* camera,
                                                       const struct KzcDynamicArray* inputObjects)
{
    kzsError result;
    struct KzuLODFilter* lodFilter = (struct KzuLODFilter*)filterObjectSource;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(runtimeData);
    struct KzuLODFilterRuntimeData* lodRuntimeData;
    struct KzcMatrix4x4 cameraMatrix = kzuTransformedObjectNodeGetMatrix(camera);
    struct KzcDynamicArrayIterator it;
    kzUint stamp = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(lodFilter));

    result = kzcMemoryAllocVariable(memoryManager, lodRuntimeData, "LOD filter runtime data");
    kzsErrorForward(result);

    result = kzuObjectSourceSetRuntimeCacheData(&lodFilter->filterObjectSource.objectSource, runtimeData, lodRuntimeData);
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &lodRuntimeData->includedObjects);
    kzsErrorForward(result);

    it = kzcDynamicArrayGetIterator(runtimeData->allTransformedObjects);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* transformedObject = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        struct KzuObjectNode* object = kzuTransformedObjectNodeGetObjectNode(transformedObject);

        /* LOD object was found from scene graph. */
        if (kzuObjectNodeGetType(object) == KZU_OBJECT_TYPE_LOD_SELECTOR)
        {
            struct KzuLODSelectorNode* lodSelectorNode = kzuLODSelectorNodeFromObjectNode(object);
            kzUint i;
            
            /* Loop through LOD node's LOD levels. */
            for (i = 0; i < kzuLODSelectorNodeGetLevelCount(lodSelectorNode); ++i)
            {
                struct KzuObjectNode* lodLevelObject = kzuLODSelectorGetObjectNodeAtLevel(lodSelectorNode, i);
                struct KzuTransformedObjectNode* childTransformedObject;

                childTransformedObject = kzuTransformedObjectNodeFindChildObjectNode(transformedObject, lodLevelObject);
                if (childTransformedObject != KZ_NULL)
                {
                    result = kzuLODFilterAddAncestors_internal(memoryManager, lodRuntimeData->includedObjects,
                                                               childTransformedObject, childTransformedObject, 
                                                               transformedObject, lodSelectorNode);

                    kzsErrorForward(result);
                }
            }
        }
    }
    
    kzcMatrix4x4SetToViewCoordinates(&cameraMatrix, &lodRuntimeData->cameraMatrix);

    kzuObjectSourceAddCumulativeTime_private(runtimeData->rootObjectSource, KZU_OBJECT_SOURCE_MEASUREMENT_LOD_START, kzsTimeGetCurrentTimestamp() - stamp);

    kzsSuccess();
}

KZ_CALLBACK static kzBool kzuLODFilterApply_internal(const struct KzuFilterObjectSource* filterObjectSource,
                                                     const struct KzuObjectSourceRuntimeData* runtimeData,
                                                     const struct KzuTransformedObjectNode* camera,
                                                     const struct KzuTransformedObjectNode* transformedObject)
{
    kzBool result;
    struct KzuLODObjectInfo* value;
    struct KzuLODFilterRuntimeData* lodRuntimeData;
    kzUint stamp = kzsTimeGetCurrentTimestamp();
    
    kzsAssert(kzcIsValidPointer(filterObjectSource));

    lodRuntimeData = kzuObjectSourceGetRuntimeCacheData(&filterObjectSource->objectSource, runtimeData);
    
    kzsAssert(kzcIsValidPointer(lodRuntimeData));

    /* This transformed node was under LOD node, see if it fulfills the LOD constraint. */
    if (kzcHashMapGet(lodRuntimeData->includedObjects, transformedObject, (void**)&value))
    {
        switch (kzuLODSelectorGetLODType(value->lodSelectorNode))
        {
            case KZU_LOD_TYPE_DISTANCE:
            {
                struct KzcMatrix4x4 objectTransformation = kzuTransformedObjectNodeGetMatrix(value->lodParentNode);
                kzFloat distance;

                /* Get distance. Negative Z due to */
                distance = -kzcMatrix4x4MultiplyAffineGetTranslationZ(&objectTransformation, &lodRuntimeData->cameraMatrix);

                /* Object node was found from given constraint. */
                if (kzuLODSelectorNodeGetObjectAtConstraint(value->lodSelectorNode, distance) ==
                    kzuTransformedObjectNodeGetObjectNode(value->lodLevelNode))
                {
                    result = KZ_TRUE;
                }
                /* Object node was not found, filter it away. */
                else
                {
                    result = KZ_FALSE;
                }
                break;
            }
            case KZU_LOD_TYPE_SCREEN_AREA:
            {
                result = KZ_FALSE;
                break;
            }
            case KZU_LOD_TYPE_INVALID:
            default:
            {
                kzsAssert(KZ_FALSE);
                result = KZ_FALSE;
                break;
            }
        }
    }
    /* This node was not under LOD node, so filter accepts it as normal. */
    else
    {
        result = KZ_TRUE;
    }

    kzuObjectSourceAddCumulativeTime_private(runtimeData->rootObjectSource, KZU_OBJECT_SOURCE_MEASUREMENT_LOD_APPLY, kzsTimeGetCurrentTimestamp() - stamp);

    return result;
}
