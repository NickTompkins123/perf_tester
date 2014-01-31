/**
* \file
* Level of detail selector node and data. Node inherited from KzuObjectNode.
* LOD selector node is pipeline element for selecting level of detail depending on camera distance / perspective size.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_lod_selector.h"

#include "kzu_object.h"
#include "kzu_object_private.h"

#include <user/kzu_error_codes.h>

#include <core/util/collection/kzc_sort.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/wrappers/kzs_memory.h>


/** Object node class for LOD selector. */
extern const struct KzuObjectNodeClass KZU_LOD_SELECTOR_NODE_CLASS;

const KzuObjectType KZU_OBJECT_TYPE_LOD_SELECTOR = (void*)&KZU_LOD_SELECTOR_NODE_CLASS;


/** Internal structure for LOD level. */
struct KzuLODLevel
{
    union
    {
        kzFloat zDistance; /**< Distance. */
        kzFloat screenArea; /**< Screen area. */
    } constraint;   /**< Constraint for this LOD level that has to be fulfilled in order for LOD to appear. */
    struct KzuObjectNode* objectNode;   /**< Used object node for this LOD level. */
};

struct KzuLODSelectorNode
{
    struct KzuObjectNode objectNode; /**< Inherited from object node, that contains transformation and child data. */
};

struct KzuLODSelectorNodeData
{
    struct KzuObjectNodeData objectNodeData; /**< Inherited from object node, that contains transformation and child data. */ /*lint -esym(754, KzuLODNodeData::objectNodeData) Suppressed as this member is required but not referenced. */ 
    enum KzuLODType type; /**< LOD type, either perspective or distance. */
    struct KzuLODLevel* lodLevels; /**< Array of LOD levels. Sorted by constraint. */
};


/** Internal function for comparing LOD levels, when sorting them after assigning data. */
static kzInt kzuCompareLODDistances_internal(const void* first, const void* second);


static struct KzuLODSelectorNodeData* kzuLODSelectorNodeGetData_internal(const struct KzuLODSelectorNode* lodSelectorNode)
{
    kzsAssert(kzuObjectNodeIsValid(&lodSelectorNode->objectNode));

    return (struct KzuLODSelectorNodeData*)lodSelectorNode->objectNode.data;
}

static kzsError kzuLODSelectorCreateObjectNodeData_internal(const struct KzcMemoryManager* memoryManager,
                                                            struct KzuObjectNodeData** out_lodSelectorNodeData)
{
    kzsError result;
    struct KzuLODSelectorNodeData* lodSelectorNodeData;

    result = kzcMemoryAllocVariable(memoryManager, lodSelectorNodeData, "LOD selector node data");
    kzsErrorForward(result);

    lodSelectorNodeData->type = KZU_LOD_TYPE_INVALID;
    lodSelectorNodeData->lodLevels = KZ_NULL;

    *out_lodSelectorNodeData = &lodSelectorNodeData->objectNodeData;
    kzsSuccess();
}

kzsError kzuLODSelectorNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                  kzString name, struct KzuLODSelectorNode** out_lodNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;
    struct KzuLODSelectorNode* lodNode;

    result = kzuLODSelectorCreateObjectNodeData_internal(memoryManager, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCreate_private(memoryManager, propertyManager, messageDispatcher, &KZU_LOD_SELECTOR_NODE_CLASS, name, objectNodeData, &objectNode);
    kzsErrorForward(result);

    lodNode = (struct KzuLODSelectorNode*)objectNode;

    *out_lodNode = lodNode;
    kzsSuccess();
}

kzsError kzuLODSelectorNodeDelete(struct KzuLODSelectorNode* lodSelectorNode)
{
    kzsError result;
    struct KzuLODSelectorNodeData* lodSelectorNodeData = kzuLODSelectorNodeGetData_internal(lodSelectorNode);

    kzsAssert(kzcIsValidPointer(lodSelectorNodeData));

    if (lodSelectorNodeData->lodLevels != KZ_NULL)
    {
        result = kzcMemoryFreeArray(lodSelectorNodeData->lodLevels);
        kzsErrorForward(result);
    }

    result = kzuObjectNodeDeleteBase_private(&lodSelectorNode->objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuLODSelectorNode* kzuLODSelectorNodeFromObjectNode(const struct KzuObjectNode* objectNode)
{
    struct KzuLODSelectorNode* lodNode;

    kzsAssert(kzuObjectNodeIsValid(objectNode));
    kzsAssert(objectNode->data->objectNodeClass == &KZU_LOD_SELECTOR_NODE_CLASS);

    lodNode = (struct KzuLODSelectorNode*)objectNode;
    return lodNode;
}

struct KzuObjectNode* kzuLODSelectorNodeToObjectNode(struct KzuLODSelectorNode* lodNode)
{
    return (struct KzuObjectNode*)lodNode;
}

static kzInt kzuCompareLODDistances_internal(const void* first, const void* second)
{
    struct KzuLODLevel* level1 = (struct KzuLODLevel*)first;
    struct KzuLODLevel* level2 = (struct KzuLODLevel*)second;
    kzFloat value1 = level1->constraint.zDistance;
    kzFloat value2 = level2->constraint.zDistance;

    /* Sort by having smallest constraint values first. */
    return (value1 < value2) ? -1 : ((value1 > value2) ? 1 : 0);
}

static kzInt kzuCompareLODScreenAreas_internal(const void* first, const void* second)
{
    struct KzuLODLevel* level1 = (struct KzuLODLevel*)first;
    struct KzuLODLevel* level2 = (struct KzuLODLevel*)second;
    kzFloat value1 = level1->constraint.screenArea;
    kzFloat value2 = level2->constraint.screenArea;

    /* Sort by having smallest constraint values first. */
    return (value1 < value2) ? -1 : ((value1 > value2) ? 1 : 0);
}

static kzsError kzuLODSelectorNodeInitialize_internal(struct KzuLODSelectorNodeData* lodSelectorNodeData, kzUint lodLevelCount,
                                                      struct KzuObjectNode* const* objectNodes)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(lodSelectorNodeData);
    kzUint i;

    kzsAssert(kzcIsValidPointer(lodSelectorNodeData));

    kzsErrorTest(lodSelectorNodeData->type == KZU_LOD_TYPE_INVALID, KZU_ERROR_INVALID_LOD_DATA, "Trying to initialize a LOD selector which is already initialized");

    /* Allocate memory for LOD levels. */
    result = kzcMemoryAllocArray(memoryManager, lodSelectorNodeData->lodLevels, lodLevelCount, "LOD levels");
    kzsErrorForward(result);

    /* Assign data from parameter to LOD. */
    for (i = 0; i < lodLevelCount; ++i)
    {
        lodSelectorNodeData->lodLevels[i].objectNode = objectNodes[i];
    }

    kzsSuccess();
}

kzsError kzuLODSelectorNodeInitializeDistance(const struct KzuLODSelectorNode* lodSelectorNode, kzUint lodLevelCount,
                                              const kzFloat* constraints, struct KzuObjectNode* const* objectNodes)
{
    kzsError result;
    struct KzuLODSelectorNodeData* lodSelectorNodeData = kzuLODSelectorNodeGetData_internal(lodSelectorNode);
    kzUint i;

    kzsAssert(kzcIsValidPointer(lodSelectorNodeData));

    result = kzuLODSelectorNodeInitialize_internal(lodSelectorNodeData, lodLevelCount, objectNodes);
    kzsErrorForward(result);

    lodSelectorNodeData->type = KZU_LOD_TYPE_DISTANCE;

    /* Assign data from parameter to LOD. */
    for (i = 0; i < lodLevelCount; ++i)
    {
        lodSelectorNodeData->lodLevels[i].constraint.zDistance = constraints[i];
    }

    /* Sort LOD levels by constraint values. */
    kzcSortArray(lodSelectorNodeData->lodLevels, lodLevelCount, kzuCompareLODDistances_internal);

    kzsSuccess();
}

kzsError kzuLODSelectorNodeInitializeScreenArea(const struct KzuLODSelectorNode* lodSelectorNode, kzUint lodLevelCount,
                                                const kzFloat* constraints, struct KzuObjectNode* const* objectNodes)
{
    kzsError result;
    struct KzuLODSelectorNodeData* lodSelectorNodeData = kzuLODSelectorNodeGetData_internal(lodSelectorNode);
    kzUint i;

    kzsAssert(kzcIsValidPointer(lodSelectorNodeData));

    result = kzuLODSelectorNodeInitialize_internal(lodSelectorNodeData, lodLevelCount, objectNodes);
    kzsErrorForward(result);

    lodSelectorNodeData->type = KZU_LOD_TYPE_SCREEN_AREA;

    /* Assign data from parameter to LOD. */
    for (i = 0; i < lodLevelCount; ++i)
    {
        lodSelectorNodeData->lodLevels[i].constraint.screenArea = constraints[i];
    }

    /* Sort LOD levels by constraint values. */
    kzcSortArray(lodSelectorNodeData->lodLevels, lodLevelCount, kzuCompareLODScreenAreas_internal);

    kzsSuccess();
}

enum KzuLODType kzuLODSelectorGetLODType(const struct KzuLODSelectorNode* lodSelectorNode)
{
    struct KzuLODSelectorNodeData* lodSelectorNodeData = kzuLODSelectorNodeGetData_internal(lodSelectorNode);

    kzsAssert(kzcIsValidPointer(lodSelectorNodeData));

    return lodSelectorNodeData->type;
}

kzUint kzuLODSelectorNodeGetLevelCount(const struct KzuLODSelectorNode* lodSelectorNode)
{
    struct KzuLODSelectorNodeData* lodSelectorNodeData = kzuLODSelectorNodeGetData_internal(lodSelectorNode);

    kzsAssert(kzcIsValidPointer(lodSelectorNodeData));

    return kzcArrayLength(lodSelectorNodeData->lodLevels);
}

struct KzuObjectNode* kzuLODSelectorNodeGetObjectAtConstraint(const struct KzuLODSelectorNode* lodSelectorNode, kzFloat constraint)
{
    kzInt i;
    struct KzuLODSelectorNodeData* lodSelectorNodeData = kzuLODSelectorNodeGetData_internal(lodSelectorNode);
    struct KzuObjectNode* resultObject = KZ_NULL;
    kzUint lodLevelCount;

    kzsAssert(kzcIsValidPointer(lodSelectorNodeData));

    lodLevelCount = kzcArrayLength(lodSelectorNodeData->lodLevels);

    /* Assumes that items are sorted. */
    for (i = (kzInt)lodLevelCount - 1; i >= 0; --i)
    {
        struct KzuLODLevel* lodLevel = &lodSelectorNodeData->lodLevels[i];
        kzBool foundObject;

        switch (lodSelectorNodeData->type)
        {
            /* Get LOD level according to distance. */
            case KZU_LOD_TYPE_DISTANCE:
            {
                foundObject = (constraint >= lodLevel->constraint.zDistance);
                break;
            }
            /* Get LOD level according to perspective size. */
            case KZU_LOD_TYPE_SCREEN_AREA:
            {
                foundObject = (constraint >= lodLevel->constraint.screenArea);
                break;
            }
            case KZU_LOD_TYPE_INVALID:
            default:
            {
                kzsAssertText(KZ_FALSE, "Invalid type for LOD level!");
                foundObject = KZ_FALSE;
                break;
            }
        }

        if (foundObject)
        {
            resultObject = lodSelectorNodeData->lodLevels[i].objectNode;
            break;
        }
    }

    return resultObject;
}

struct KzuObjectNode* kzuLODSelectorGetObjectNodeAtLevel(const struct KzuLODSelectorNode* lodSelectorNode, kzUint index)
{
    struct KzuLODSelectorNodeData* lodSelectorNodeData = kzuLODSelectorNodeGetData_internal(lodSelectorNode);

    kzsAssert(kzcIsValidPointer(lodSelectorNodeData));
    kzsAssert(kzcIsValidArrayIndex(lodSelectorNodeData->lodLevels, index));

    return lodSelectorNodeData->lodLevels[index].objectNode;
}

KZ_CALLBACK static kzsError kzuLODSelectorNodeDeleteImplementation_internal(struct KzuObjectNode* objectNode)
{
    kzsError result = kzuLODSelectorNodeDelete(kzuLODSelectorNodeFromObjectNode(objectNode));
    kzsErrorForward(result);
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuLODSelectorNodeCopyImplementation_internal(const struct KzcMemoryManager* memoryManager,
                                                                          const struct KzuObjectNode* sourceObjectNode,
                                                                          struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;

    result = kzuLODSelectorCreateObjectNodeData_internal(memoryManager, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCopy_private(memoryManager, sourceObjectNode, objectNodeData, &objectNode);
    kzsErrorForward(result);

    /* Copy data */
    {
        struct KzuLODSelectorNodeData* sourceLODSelectorNodeData = ((struct KzuLODSelectorNodeData*)sourceObjectNode->data);
        struct KzuLODSelectorNodeData* lodSelectorNodeData = (struct KzuLODSelectorNodeData*)objectNodeData;
        kzUint lodLevelCount = kzcArrayLength(sourceLODSelectorNodeData->lodLevels);

        result = kzcMemoryAllocArray(memoryManager, lodSelectorNodeData->lodLevels, lodLevelCount, "LOD levels");
        kzsErrorForward(result);

        kzsMemcpy(lodSelectorNodeData->lodLevels, sourceLODSelectorNodeData->lodLevels, lodLevelCount * sizeof(struct KzuLODLevel));
    }

    *out_objectNode = objectNode;
    kzsSuccess();
}

const struct KzuObjectNodeClass KZU_LOD_SELECTOR_NODE_CLASS =
{
    kzuLODSelectorNodeDeleteImplementation_internal,
    kzuLODSelectorNodeCopyImplementation_internal,
    KZ_NULL,
    KZ_NULL
};
