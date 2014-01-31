/**
* \file
* User interface component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_component.h"

#include <user/ui/kzu_ui_event.h>
#include <user/ui/kzu_ui_event_listener.h>
#include <user/ui/kzu_ui_action.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/components/kzu_ui_label.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_object_base.h>
#include <user/scene_graph/kzu_object_private.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_property_collection.h>

#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>


struct KzuUiComponentNode
{
    struct KzuObjectNode objectNode; /**< Inherited from object node, that contains transformation and child data. */
};

struct KzuUiComponentNodeData
{
    struct KzuObjectNodeData objectNodeData; /**< Inherited from object node, that contains transformation and child data. */ /*lint -esym(754, KzuUiComponentNodeData::objectNodeData) Suppressed as this member is required but not referenced. */ 

    struct KzuUiComponentType* componentType; /**< Component type reference. Component type contains the logic for the component. */
    struct KzcHashMap* eventListeners; /**< Hash map containing pair <event id, dynamic array> for mapping component events to event listeners. */
    KzuUiComponentMeasureFunction measureFunction;  /**< Measure function. */
    KzuUiComponentArrangeFunction arrangeFunction;  /**< Arrange function. */
    KzuUiComponentSpecificCopyFunction specificCopyFunction;  /**< UI component type specific copy function. */
};


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_EMPTY = (void*)&KZU_COMPONENT_TYPE_EMPTY;

/** Object node class for ui component. */
extern const struct KzuObjectNodeClass KZU_UI_COMPONENT_NODE_CLASS;

/** Object type identifier for component objects. */
const KzuObjectType KZU_OBJECT_TYPE_UI_COMPONENT = (void*)&KZU_UI_COMPONENT_NODE_CLASS;


/** Internal helper for adding & removing event listeners to component data. */
static kzsError kzuUiComponentNodeDataAddEventListener_internal(const struct KzuUiComponentNodeData* uiComponentNodeData, kzString event, struct KzuUiEventListener* eventListener);
static kzsError kzuUiComponentNodeDataRemoveEventListener_internal(const struct KzuUiComponentNodeData* uiComponentNodeData, kzString event, const struct KzuUiEventListener* eventListener);

static struct KzuUiComponentNodeData* kzuUiComponentNodeGetData_internal(const struct KzuUiComponentNode* uiComponentNode)
{
    kzsAssert(kzuObjectNodeIsValid(&uiComponentNode->objectNode));

    return (struct KzuUiComponentNodeData*)uiComponentNode->objectNode.data;
}

static kzsError kzuUiComponentCreateObjectNodeData_internal(const struct KzcMemoryManager* memoryManager,
                                                            struct KzuUiComponentType* componentType,
                                                            struct KzuObjectNodeData** out_uiComponentNodeData)
{
    kzsError result;
    struct KzuUiComponentNodeData* uiComponentNodeData;

    result = kzcMemoryAllocVariable(memoryManager, uiComponentNodeData, "UI component node data");
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &uiComponentNodeData->eventListeners);
    kzsErrorForward(result);

    uiComponentNodeData->componentType = componentType;
    uiComponentNodeData->arrangeFunction = KZ_NULL;
    uiComponentNodeData->measureFunction = KZ_NULL;
    uiComponentNodeData->specificCopyFunction = KZ_NULL;

    *out_uiComponentNodeData = &uiComponentNodeData->objectNodeData;
    kzsSuccess();
}

kzsError kzuUiComponentNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                  kzString path, struct KzuUiComponentType* componentType, struct KzuUiComponentNode** out_componentNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;
    struct KzuUiComponentNode* uiComponentNode;

    result = kzuUiComponentCreateObjectNodeData_internal(memoryManager, componentType, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCreate_private(memoryManager, propertyManager, messageDispatcher, &KZU_UI_COMPONENT_NODE_CLASS, path, objectNodeData, &objectNode);
    kzsErrorForward(result);

    uiComponentNode = (struct KzuUiComponentNode*)objectNode;

    *out_componentNode = uiComponentNode;
    kzsSuccess();
}

kzsError kzuUiComponentNodeDelete(struct KzuUiComponentNode* uiComponentNode)
{
    kzsError result;
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);
    struct KzcHashMapIterator it;

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    it = kzcHashMapGetIterator(uiComponentNodeData->eventListeners);
    while (kzcHashMapIterate(it))
    {
        kzMutableString key = (kzMutableString)kzcHashMapIteratorGetKey(it);
        struct KzcDynamicArray* listeners = (struct KzcDynamicArray*)kzcHashMapIteratorGetValue(it);
        struct KzcDynamicArrayIterator it2 = kzcDynamicArrayGetIterator(listeners);
        
        while (kzcDynamicArrayIterate(it2))
        {
            struct KzuUiEventListener* eventListener = (struct KzuUiEventListener*)kzcDynamicArrayIteratorGetValue(it2);
            result = kzuUiEventListenerDelete(eventListener);
            kzsErrorForward(result);
        }

        result = kzcStringDelete(key);
        kzsErrorForward(result);
        result = kzcDynamicArrayDelete(listeners);
        kzsErrorForward(result);
    }
    result = kzcHashMapDelete(uiComponentNodeData->eventListeners);
    kzsErrorForward(result);

    result = kzuObjectNodeDeleteBase_private(&uiComponentNode->objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuUiComponentNodeSetComponentType(const struct KzuUiComponentNode* uiComponentNode, struct KzuUiComponentType* componentType)
{
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    uiComponentNodeData->componentType = componentType;
}

kzsError kzuUiComponentNodeInitialize(struct KzuUiComponentNode* uiComponentNode, const struct KzcMemoryManager* memoryManager)
{
    kzsError result;
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);
    struct KzuUiComponentType* componentType;

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    componentType = kzuUiComponentTypeGetInheritedCallback(uiComponentNodeData->componentType, KZU_UI_COMPONENT_CALLBACK_INITIALIZE);
    result = kzuUiComponentTypeCallCallbackInitialize(componentType, uiComponentNode, memoryManager);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiComponentNodeUninitialize(struct KzuUiComponentNode* uiComponentNode)
{
    kzsError result;
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);
    struct KzuUiComponentType* componentType;

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    componentType = kzuUiComponentTypeGetInheritedCallback(uiComponentNodeData->componentType, KZU_UI_COMPONENT_CALLBACK_UNINITIALIZE);
    result = kzuUiComponentTypeCallCallbackUninitialize(componentType, uiComponentNode);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiComponentNodeLogicUpdate(const struct KzuUiComponentNode* uiComponentNode, kzFloat deltaTime, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);
    struct KzuUiComponentType* componentType;

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    componentType = kzuUiComponentTypeGetInheritedCallback(uiComponentNodeData->componentType, KZU_UI_COMPONENT_CALLBACK_LOGIC_UPDATE);
    result = kzuUiComponentTypeCallCallbackLogicUpdate(componentType, uiComponentNode, deltaTime, queue);
    kzsErrorForward(result);
    
    kzsSuccess();
}


/* TODO: Enable when needed and add to header. */
/*
static kzsError kzuUiComponentEventBubble_internal(const struct KzuObjectNode* objectNode,
                                                   const struct KzuUiComponentNode* uiComponentNode,
                                                   struct KzuUiEvent* event, kzString eventType,
                                                   struct KzuEngineMessageQueue* queue)
{
    kzsError result;

    struct KzuObjectNode* parentObjectNode = kzuObjectNodeGetParent(objectNode);
    
    if(parentObjectNode != KZ_NULL)
    {
        if(kzuObjectNodeGetType(parentObjectNode) == KZU_OBJECT_TYPE_COMPONENT)
        {
            struct KzuUiComponentNode* node = kzuUiComponentNodeFromObjectNode(parentObjectNode);
            result = kzuUiComponentHandleEvents(node, event, eventType, queue);
            kzsErrorForward(result);
        }
        else
        {
            result = kzuUiComponentEventBubble_internal(parentObjectNode, uiComponentNode, event, eventType, queue);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}*/

kzsError kzuUiComponentNodeHandleEvents(const struct KzuUiComponentNode* uiComponentNode, struct KzuUiEvent* event, 
                                        kzString eventType, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);
    struct KzuUiComponentType* componentType;

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    if(kzcHashMapContains(uiComponentNodeData->eventListeners, eventType))
    {
        struct KzcDynamicArray* array;
        if(kzcHashMapGet(uiComponentNodeData->eventListeners, eventType, (void**)&array))
        {
            struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(array);
            while(kzcDynamicArrayIterate(it))
            {
                struct KzuUiEventListener* eventListener = (struct KzuUiEventListener*)kzcDynamicArrayIteratorGetValue(it);
                result = kzuUiEventListenerCallCallback(eventListener, uiComponentNode, event, eventType, queue);
                kzsErrorForward(result);
            }
        }
    }
    else
    {
        /* 
        TODO: Re-enable if required.
        struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(uiComponentNode);
        result = kzuUiComponentEventBubble_internal(objectNode, uiComponentNode, event, eventType, queue);
        kzsErrorForward(result);
        */
    }

    componentType = kzuUiComponentTypeGetInheritedCallback(uiComponentNodeData->componentType, KZU_UI_COMPONENT_CALLBACK_HANDLE_EVENTS);
    result = kzuUiComponentTypeCallCallbackHandleEvents(componentType, uiComponentNode, eventType, event, queue);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiComponentNodeAddEventListener(const struct KzuUiComponentNode* uiComponentNode, kzString event, struct KzuUiEventListener* eventListener)
{
    kzsError result;
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);

    result = kzuUiComponentNodeDataAddEventListener_internal(uiComponentNodeData, event, eventListener);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiComponentNodeRemoveEventListener(const struct KzuUiComponentNode* uiComponentNode, kzString event, const struct KzuUiEventListener* eventListener)
{
    kzsError result;
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);

    result = kzuUiComponentNodeDataRemoveEventListener_internal(uiComponentNodeData, event, eventListener);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuUiComponentNodeDataAddEventListener_internal(const struct KzuUiComponentNodeData* uiComponentNodeData, kzString event, struct KzuUiEventListener* eventListener)
{
    kzsError result;
    kzBool found;
    struct KzcDynamicArray* array;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));
    kzsAssert(kzcIsValidPointer(eventListener));

    memoryManager = kzcMemoryGetManager(uiComponentNodeData);

    found = kzcHashMapGet(uiComponentNodeData->eventListeners, event, (void**)&array);

    if(found)
    {
        result = kzcDynamicArrayAdd(array, eventListener);
        kzsErrorForward(result);
    }
    else
    {
        kzMutableString key;
        struct KzcDynamicArray* newArray;
        result = kzcDynamicArrayCreate(memoryManager, &newArray);
        kzsErrorForward(result);
        result = kzcDynamicArrayAdd(newArray, eventListener);
        kzsErrorForward(result);
        result = kzcStringCopy(memoryManager, event, &key);
        kzsErrorForward(result);
        result = kzcHashMapPut(uiComponentNodeData->eventListeners, key, newArray);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

static kzsError kzuUiComponentNodeDataRemoveEventListener_internal(const struct KzuUiComponentNodeData* uiComponentNodeData, kzString event, const struct KzuUiEventListener* eventListener)
{
    kzsError result;
    kzBool found;
    struct KzcDynamicArray* array;

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));
    kzsAssert(kzcIsValidPointer(eventListener));

    found = kzcHashMapGet(uiComponentNodeData->eventListeners, event, (void**)&array);

    if(found)
    {
        result = kzcDynamicArrayRemove(array, eventListener);
        kzsErrorForward(result);
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Event listener does not exist.");
    }

    kzsSuccess();
}

kzsError kzuUiComponentNodeGetEventListeners(const struct KzcMemoryManager* memoryManager,
                                             const struct KzuUiComponentNode* uiComponentNode, 
                                             struct KzcDynamicArray** out_array)
{
    kzsError result;
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);
    struct KzcDynamicArray* array;
    struct KzcHashMapIterator it;

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    result = kzcDynamicArrayCreate(memoryManager, &array);
    kzsErrorForward(result);

    it = kzcHashMapGetIterator(uiComponentNodeData->eventListeners);
    while(kzcHashMapIterate(it))
    {
        struct KzcDynamicArray* listeners = (struct KzcDynamicArray*)kzcHashMapIteratorGetValue(it);
        struct KzcDynamicArrayIterator it2 = kzcDynamicArrayGetIterator(listeners);

        while(kzcDynamicArrayIterate(it2))
        {
            struct KzuUiEventListener* eventListener = (struct KzuUiEventListener*)kzcDynamicArrayIteratorGetValue(it2);
            if(kzuUiEventListenerGetTargetLocation(eventListener) == KZU_UI_ACTION_LOCATION_COMPONENT)
            {
                result = kzcDynamicArrayAdd(array, eventListener);
                kzsErrorForward(result);
            }
        }
    }

    *out_array = array;
    kzsSuccess();
}

struct KzuUiComponentType* kzuUiComponentNodeGetType(const struct KzuUiComponentNode* uiComponentNode)
{
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    return uiComponentNodeData->componentType;
}

struct KzuUiComponentNode* kzuUiComponentNodeFromObjectNode(const struct KzuObjectNode* objectNode)
{
    kzsAssert(kzcIsValidPointer(objectNode));
    kzsAssert(objectNode->data->objectNodeClass == &KZU_UI_COMPONENT_NODE_CLASS);

    return (struct KzuUiComponentNode*)objectNode;
}

struct KzuObjectNode* kzuUiComponentNodeToObjectNode(const struct KzuUiComponentNode* uiComponentNode)
{
    return (struct KzuObjectNode*)uiComponentNode;
}

void kzuUiComponentNodeSetMeasureFunction(const struct KzuUiComponentNode* uiComponentNode, KzuUiComponentMeasureFunction function)
{
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    uiComponentNodeData->measureFunction = function;
}

void kzuUiComponentNodeSetArrangeFunction(const struct KzuUiComponentNode* uiComponentNode, KzuUiComponentArrangeFunction function)
{
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    uiComponentNodeData->arrangeFunction = function;
}

void kzuUiComponentNodeSetSpecificCopyFunction(const struct KzuUiComponentNode* uiComponentNode, KzuUiComponentSpecificCopyFunction function)
{
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    uiComponentNodeData->specificCopyFunction = function;
}

KzuUiComponentMeasureFunction kzuUiComponentNodeGetMeasureFunction(const struct KzuUiComponentNode* uiComponentNode)
{
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    return uiComponentNodeData->measureFunction;
}

KzuUiComponentArrangeFunction kzuUiComponentNodeGetArrangeFunction(const struct KzuUiComponentNode* uiComponentNode)
{
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    return uiComponentNodeData->arrangeFunction;
}

KzuUiComponentSpecificCopyFunction kzuUiComponentNodeGetSpecificCopyFunction(const struct KzuUiComponentNode* uiComponentNode)
{
    struct KzuUiComponentNodeData* uiComponentNodeData = kzuUiComponentNodeGetData_internal(uiComponentNode);

    kzsAssert(kzcIsValidPointer(uiComponentNodeData));

    return uiComponentNodeData->specificCopyFunction;
}

kzBool kzuUiFindChildNode(const struct KzuObjectNode* parentObject, const struct KzuObjectNode* objectToFind)
{
    kzBool found = KZ_FALSE;
    struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(parentObject);

    while (kzcDynamicArrayIterate(it) && !found)
    {
        struct KzuObjectNode* childObjectNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        KzuObjectType childType = kzuObjectNodeGetType(childObjectNode);
        kzBool valid = KZ_FALSE;

        /* Accept all nodes non UI components and also UI Label components.
           This assumes that all other UI components will handle their children except UI Label. */
        if (childType == KZU_OBJECT_TYPE_UI_COMPONENT)
        {
            struct KzuUiComponentType* type = kzuUiComponentNodeGetType(kzuUiComponentNodeFromObjectNode(childObjectNode));
            valid = kzuUiComponentTypeGetTypeIdentifier(type) == KZU_COMPONENT_TYPE_LABEL;
        }
        else
        {
            valid = KZ_TRUE;
        }

        if (valid)
        {
            if (childObjectNode == objectToFind)
            {
                found = KZ_TRUE;
            }
            else
            {
                found = kzuUiFindChildNode(childObjectNode, objectToFind);
            }
        }
    }

    return found;
}

KZ_CALLBACK static kzsError kzuUiComponentNodeDeleteImplementation_internal(struct KzuObjectNode* objectNode)
{
    kzsError result = kzuUiComponentNodeDelete(kzuUiComponentNodeFromObjectNode(objectNode));
    kzsErrorForward(result);
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuUiComponentNodeCopyImplementation_internal(const struct KzcMemoryManager* memoryManager,
                                                                          const struct KzuObjectNode* sourceObjectNode,
                                                                          struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuUiComponentNodeData* sourceUiComponentNodeData = ((struct KzuUiComponentNodeData*)sourceObjectNode->data);
    struct KzuUiComponentNodeData* outUiComponentNodeData; /* TODO: out -> target */
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode; /* TODO: Consider renaming, e.g. targetObjectNode */
    KzuUiComponentSpecificCopyFunction specificCopyFunction;

    result = kzuUiComponentCreateObjectNodeData_internal(memoryManager, sourceUiComponentNodeData->componentType, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCopy_private(memoryManager, sourceObjectNode, objectNodeData, &objectNode);
    kzsErrorForward(result);

    outUiComponentNodeData = ((struct KzuUiComponentNodeData*)objectNode->data);
    outUiComponentNodeData->arrangeFunction = sourceUiComponentNodeData->arrangeFunction;
    outUiComponentNodeData->measureFunction = sourceUiComponentNodeData->measureFunction;
    specificCopyFunction = sourceUiComponentNodeData->specificCopyFunction;
    outUiComponentNodeData->specificCopyFunction = specificCopyFunction;
    if(specificCopyFunction != KZ_NULL)
    {
        result = specificCopyFunction(sourceObjectNode, objectNode);
        kzsErrorForward(result);
    }

    {
        struct KzcHashMapIterator it;
        it = kzcHashMapGetIterator(sourceUiComponentNodeData->eventListeners);
        while (kzcHashMapIterate(it))
        {
            kzMutableString key = (kzMutableString)kzcHashMapIteratorGetKey(it);
            struct KzcDynamicArray* listeners = (struct KzcDynamicArray*)kzcHashMapIteratorGetValue(it);
            struct KzcDynamicArrayIterator it2 = kzcDynamicArrayGetIterator(listeners);

            while (kzcDynamicArrayIterate(it2))
            {
                struct KzuUiEventListener* newEventListener;
                struct KzuUiEventListener* eventListener = (struct KzuUiEventListener*)kzcDynamicArrayIteratorGetValue(it2);
                result = kzuUiEventListenerCopy(memoryManager, eventListener, &newEventListener);
                kzsErrorForward(result);
                result = kzuUiComponentNodeDataAddEventListener_internal((struct KzuUiComponentNodeData*)objectNodeData, key, newEventListener);
                kzsErrorForward(result);
            }
        }
    }

    *out_objectNode = objectNode;
    kzsSuccess();
}

const struct KzuObjectNodeClass KZU_UI_COMPONENT_NODE_CLASS =
{
    kzuUiComponentNodeDeleteImplementation_internal,
    kzuUiComponentNodeCopyImplementation_internal,
    KZ_NULL,
    KZ_NULL
};
