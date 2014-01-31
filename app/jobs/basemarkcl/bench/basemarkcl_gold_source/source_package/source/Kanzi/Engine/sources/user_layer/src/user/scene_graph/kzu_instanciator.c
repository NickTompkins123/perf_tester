/**
* \file
* Level of detail selector node and data. Node inherited from KzuObjectNode.
* Instanciator selector node is pipeline element for selecting level of detail depending on camera distance / perspective size.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_instanciator.h"

#include "kzu_object.h"
#include "kzu_object_private.h"

#include <core/memory/kzc_memory_manager.h>


/** Object node class for Instanciator selector. */
extern const struct KzuObjectNodeClass KZU_INSTANCIATOR_NODE_CLASS;

const KzuObjectType KZU_OBJECT_TYPE_INSTANCIATOR = (void*)&KZU_INSTANCIATOR_NODE_CLASS;


struct KzuInstanciatorNode
{
    struct KzuObjectNode objectNode; /**< Inherited from object node, that contains transformation and child data. */
};

struct KzuInstanciatorNodeData
{
    struct KzuObjectNodeData objectNodeData; /**< Inherited from object node, that contains transformation and child data. */ /*lint -esym(754, KzuInstanciatorNodeData::objectNodeData) Suppressed as this member is required but not referenced. */ 
    struct KzuObjectNode* targetObjectNode; /**< Target object node for instanciation. */
};


static struct KzuInstanciatorNodeData* kzuInstanciatorNodeGetData_internal(const struct KzuInstanciatorNode* instanciatorNode)
{
    kzsAssert(kzuObjectNodeIsValid(&instanciatorNode->objectNode));

    return (struct KzuInstanciatorNodeData*)instanciatorNode->objectNode.data;
}

static kzsError kzuInstanciatorCreateObjectNodeData_internal(const struct KzcMemoryManager* memoryManager,
                                                             struct KzuObjectNodeData** out_instanciatorNodeData)
{
    kzsError result;
    struct KzuInstanciatorNodeData* instanciatorNodeData;

    result = kzcMemoryAllocVariable(memoryManager, instanciatorNodeData, "Instanciator selector node data");
    kzsErrorForward(result);

    instanciatorNodeData->targetObjectNode = KZ_NULL;

    *out_instanciatorNodeData = &instanciatorNodeData->objectNodeData;
    kzsSuccess();
}

kzsError kzuInstanciatorNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                   kzString name, struct KzuInstanciatorNode** out_instanciatorNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;
    struct KzuInstanciatorNode* instanciatorNode;

    result = kzuInstanciatorCreateObjectNodeData_internal(memoryManager, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCreate_private(memoryManager, propertyManager, messageDispatcher, &KZU_INSTANCIATOR_NODE_CLASS, name, objectNodeData, &objectNode);
    kzsErrorForward(result);

    instanciatorNode = (struct KzuInstanciatorNode*)objectNode;

    *out_instanciatorNode = instanciatorNode;
    kzsSuccess();
}

kzsError kzuInstanciatorNodeDelete(struct KzuInstanciatorNode* instanciatorNode)
{
    kzsError result;

    result = kzuObjectNodeDeleteBase_private(&instanciatorNode->objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuInstanciatorNode* kzuInstanciatorNodeFromObjectNode(const struct KzuObjectNode* objectNode)
{
    struct KzuInstanciatorNode* instanciatorNode;

    kzsAssert(kzuObjectNodeIsValid(objectNode));
    kzsAssert(objectNode->data->objectNodeClass == &KZU_INSTANCIATOR_NODE_CLASS);

    instanciatorNode = (struct KzuInstanciatorNode*)objectNode;
    return instanciatorNode;
}

struct KzuObjectNode* kzuInstanciatorNodeToObjectNode(struct KzuInstanciatorNode* instanciatorNode)
{
    return (struct KzuObjectNode*)instanciatorNode;
}

kzsError kzuInstanciatorNodeInitialize(const struct KzuInstanciatorNode* instanciatorNode, struct KzuObjectNode* targetObjectNode)
{
    struct KzuInstanciatorNodeData* instanciatorNodeData = kzuInstanciatorNodeGetData_internal(instanciatorNode);

    kzsAssert(kzcIsValidPointer(instanciatorNodeData));

    instanciatorNodeData->targetObjectNode = targetObjectNode;

    kzsSuccess();
}

struct KzuObjectNode* kzuInstanciatorNodeGetTarget(const struct KzuInstanciatorNode* instanciatorNode)
{
    struct KzuInstanciatorNodeData* instanciatorNodeData = kzuInstanciatorNodeGetData_internal(instanciatorNode);

    kzsAssert(kzcIsValidPointer(instanciatorNodeData));


    return instanciatorNodeData->targetObjectNode;
}

KZ_CALLBACK static kzsError kzuInstanciatorNodeDeleteImplementation_internal(struct KzuObjectNode* objectNode)
{
    kzsError result = kzuInstanciatorNodeDelete(kzuInstanciatorNodeFromObjectNode(objectNode));
    kzsErrorForward(result);
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuInstanciatorNodeCopyImplementation_internal(const struct KzcMemoryManager* memoryManager,
                                                                           const struct KzuObjectNode* sourceObjectNode,
                                                                           struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;

    result = kzuInstanciatorCreateObjectNodeData_internal(memoryManager, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCopy_private(memoryManager, sourceObjectNode, objectNodeData, &objectNode);
    kzsErrorForward(result);

    /* Copy data */
    {
        struct KzuInstanciatorNodeData* sourceInstanciatorNodeData = ((struct KzuInstanciatorNodeData*)sourceObjectNode->data);
        struct KzuInstanciatorNodeData* instanciatorNodeData = (struct KzuInstanciatorNodeData*)objectNodeData;

        instanciatorNodeData->targetObjectNode = sourceInstanciatorNodeData->targetObjectNode;
    }

    *out_objectNode = objectNode;
    kzsSuccess();
}

const struct KzuObjectNodeClass KZU_INSTANCIATOR_NODE_CLASS =
{
    kzuInstanciatorNodeDeleteImplementation_internal,
    kzuInstanciatorNodeCopyImplementation_internal,
    KZ_NULL,
    KZ_NULL
};
