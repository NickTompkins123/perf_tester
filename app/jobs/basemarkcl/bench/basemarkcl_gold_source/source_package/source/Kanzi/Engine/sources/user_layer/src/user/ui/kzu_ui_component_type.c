/**
* \file
* User interface component type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_component_type.h"

#include <user/ui/kzu_ui_component_base.h>
#include <user/properties/kzu_property_collection.h>
#include <user/common/kzu_globals.h>
#include <user/scripting/lua/kzu_lua.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/string/kzc_string.h>


/**
* \struct KzuUiComponentTypeData
* Structure for component type data.
*/
struct KzuUiComponentTypeData
{
    kzBool selfOwned;                                   /**< Data is owned by the component type. */
    struct KzuUiComponentType* parentComponentType;     /**< Parent component type. Used for inheritance. */
    struct KzuPropertyTypeCollection* propertyTypes;    /**< List of property types defining this component type. */
    KzuComponentTypeIdentifier componentTypeIdentifier; /**< Type identifier for the component type. */

    struct KzcHashMap* actions; /**< Actions linked to this component type. */

    enum KzuUiComponentCallbackType initializeType;
    union 
    {
        kzMutableString script;
        KzuUiComponentInitializeCallback callback; 
    } initialize; /**< Actions for component logic. */

    enum KzuUiComponentCallbackType uninitializeType;
    union
    {
        kzMutableString script;
        KzuUiComponentUninitializeCallback callback;
    } uninitialize; /**< Action called when object is uninitialized. */

    enum KzuUiComponentCallbackType logicUpdateType;
    union
    {
        kzMutableString script;
        KzuUiComponentUpdateCallback callback;
    } logicUpdate; /**< Action called during the application logic update. */

    enum KzuUiComponentCallbackType handleEventsType;
    union
    {
        kzMutableString script;
        KzuUiComponentHandleEventsCallback callback;
    } handleEvents; /**< Action called when new events are received. */
};


struct KzuUiComponentType
{
    struct KzuUiComponentTypeData* data; /**< Data container for the component type. */
};


kzsError kzuUiComponentTypeCreate(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentType** out_componentType)
{
    kzsError result;
    struct KzuUiComponentType* componentType;
    struct KzuUiComponentTypeData* componentTypeData;

    result = kzcMemoryAllocVariable(memoryManager, componentType, "Ui Component Type");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, componentTypeData, "Ui Component Type data");
    kzsErrorForward(result);
    componentType->data = componentTypeData;

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &componentTypeData->actions);
    kzsErrorForward(result);

    componentTypeData->selfOwned = KZ_TRUE;
    componentTypeData->parentComponentType = KZ_NULL;
    componentTypeData->handleEventsType = KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED;
    componentTypeData->logicUpdateType = KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED;
    componentTypeData->initializeType = KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED;
    componentTypeData->uninitializeType = KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED;

    result = kzuPropertyTypeCollectionCreate(memoryManager, &componentTypeData->propertyTypes);
    kzsErrorForward(result);

    *out_componentType = componentType;
    kzsSuccess();
}

kzsError kzuUiComponentTypeDelete(struct KzuUiComponentType* componentType)
{
    kzsError result;
    kzBool selfOwned;

    kzsAssert(kzuUiComponentTypeIsValid(componentType));

    selfOwned = componentType->data->selfOwned;

    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(componentType->data->actions);
        while(kzcHashMapIterate(it))
        {
            kzMutableString key = (kzMutableString)kzcHashMapIteratorGetKey(it);
            result = kzcStringDelete(key);
            kzsErrorForward(result);
        }
    }

    if(componentType->data->initializeType == KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT)
    {
        result = kzcStringDelete(componentType->data->initialize.script);
        kzsErrorForward(result);
    }
    if(componentType->data->uninitializeType == KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT)
    {
        result = kzcStringDelete(componentType->data->uninitialize.script);
        kzsErrorForward(result);
    }
    if(componentType->data->logicUpdateType == KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT)
    {
        result = kzcStringDelete(componentType->data->logicUpdate.script);
        kzsErrorForward(result);
    }
    if(componentType->data->handleEventsType == KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT)
    {
        result = kzcStringDelete(componentType->data->handleEvents.script);
        kzsErrorForward(result);
    }

    result = kzcHashMapDelete(componentType->data->actions);
    kzsErrorForward(result);
    result = kzuPropertyTypeCollectionDelete(componentType->data->propertyTypes);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(componentType->data);
    kzsErrorForward(result);

    if(selfOwned)
    {
        result = kzcMemoryFreeVariable(componentType);
        kzsErrorForward(result);
    }
    else
    {
        componentType->data = KZ_NULL;
    }

    kzsSuccess();
}

void kzuUiComponentTypeSetParent(const struct KzuUiComponentType* componentType, struct KzuUiComponentType* parent)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    componentType->data->parentComponentType = parent;
}

kzsError kzuUiComponentTypeAddAction(const struct KzuUiComponentType* componentType, kzString name, struct KzuUiAction* action)
{
    kzsError result;
    kzMutableString key;
    
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    
    result = kzcStringCopy(kzcMemoryGetManager(componentType), name, &key);
    kzsErrorForward(result);

    result = kzcHashMapPut(componentType->data->actions, key, action);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuUiComponentTypeTakeOwnership(const struct KzuUiComponentType* componentType)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    componentType->data->selfOwned = KZ_FALSE;
}

kzBool kzuUiComponentTypeIsValid(const struct KzuUiComponentType* componentType)
{
    return componentType != KZ_NULL && componentType->data != KZ_NULL;
}

void kzuUiComponentTypeTransferData(struct KzuUiComponentType* targetComponentType, struct KzuUiComponentType* sourceComponentType)
{
    kzsAssert(kzcIsValidPointer(targetComponentType));
    kzsAssert(targetComponentType->data == KZ_NULL);
    kzsAssert(kzuUiComponentTypeIsValid(sourceComponentType));

    targetComponentType->data = sourceComponentType->data;
    sourceComponentType->data = KZ_NULL;
}

kzsError kzuUiComponentTypeAddPropertyType(const struct KzuUiComponentType* componentType, struct KzuPropertyType* propertyType)
{
    kzsError result;
    kzsAssert(kzuUiComponentTypeIsValid(componentType));

    result = kzuPropertyTypeCollectionAddPropertyType(componentType->data->propertyTypes, propertyType);
    kzsErrorForward(result);
    
    kzsSuccess();
}

struct KzuPropertyTypeCollection* kzuUiComponentTypeGetPropertyTypeCollection(const struct KzuUiComponentType* componentType)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    return componentType->data->propertyTypes;
}

struct KzcDynamicArray* kzuUiComponentTypeGetPropertyTypes(const struct KzuUiComponentType* componentType)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    return kzuPropertyTypeCollectionGetPropertyTypes(componentType->data->propertyTypes);
}

void kzuUiComponentTypeSetInitializeCallback(const struct KzuUiComponentType* componentType,
                                             KzuUiComponentInitializeCallback function)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    componentType->data->initializeType = KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK;
    componentType->data->initialize.callback = function;
}

/* TODO: Consider allowing KZ_NULL as callback for cmoponent type, when no functionality is required. */
void kzuUiComponentTypeSetUninitializeCallback(const struct KzuUiComponentType* componentType,
                                               KzuUiComponentUninitializeCallback function)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    componentType->data->uninitializeType = KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK;
    componentType->data->uninitialize.callback = function;
}

void kzuUiComponentTypeSetLogicUpdateCallback(const struct KzuUiComponentType* componentType,
                                              KzuUiComponentUpdateCallback function)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    componentType->data->logicUpdateType = KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK;
    componentType->data->logicUpdate.callback = function;
}

void kzuUiComponentTypeSetHandleEventsCallback(const struct KzuUiComponentType* componentType,
                                               KzuUiComponentHandleEventsCallback function)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    componentType->data->handleEventsType = KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK;
    componentType->data->handleEvents.callback = function;
}


kzsError kzuUiComponentTypeSetCallbackScript(const struct KzcMemoryManager* memoryManager, 
                                             const struct KzuUiComponentType* componentType,
                                             enum KzuUiComponentCallback callback,
                                             kzString script)
{
    kzsError result;
    kzsAssert(kzuUiComponentTypeIsValid(componentType));

    switch(callback)
    {
        case KZU_UI_COMPONENT_CALLBACK_INITIALIZE:
        {
            componentType->data->initializeType = KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT; 
            result = kzcStringCopy(memoryManager, script, &componentType->data->initialize.script);
            kzsErrorForward(result);
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_UNINITIALIZE:
        {
            componentType->data->uninitializeType = KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT; 
            result = kzcStringCopy(memoryManager, script, &componentType->data->uninitialize.script);
            kzsErrorForward(result);
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_LOGIC_UPDATE:
        {
            componentType->data->logicUpdateType = KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT; 
            result = kzcStringCopy(memoryManager, script, &componentType->data->logicUpdate.script);
            kzsErrorForward(result);
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_HANDLE_EVENTS:
        {
            componentType->data->handleEventsType = KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT; 
            result = kzcStringCopy(memoryManager, script, &componentType->data->handleEvents.script);
            kzsErrorForward(result);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown enumeration value");
        }
    }
    kzsSuccess();
}

kzsError kzuUiComponentTypeSetCallbackInherited(const struct KzuUiComponentType* componentType, 
                                                enum KzuUiComponentCallback callback)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));

    switch(callback)
    {
        case KZU_UI_COMPONENT_CALLBACK_INITIALIZE:
        {
            componentType->data->initializeType = KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED;
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_UNINITIALIZE:
        {
            componentType->data->uninitializeType = KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED;
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_LOGIC_UPDATE:
        {
            componentType->data->logicUpdateType = KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED;
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_HANDLE_EVENTS:
        {
            componentType->data->handleEventsType = KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED;
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown enumeration value");
        }
    }
    kzsSuccess();
}

struct KzuUiComponentType* kzuUiComponentTypeGetInheritedCallback(struct KzuUiComponentType* componentType,
                                                                  enum KzuUiComponentCallback callback)
{
    struct KzuUiComponentType* returnValue;
    enum KzuUiComponentCallbackType callbackType = KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED;

    kzsAssert(kzuUiComponentTypeIsValid(componentType));

    switch(callback)
    {
        case KZU_UI_COMPONENT_CALLBACK_INITIALIZE:
        {
            callbackType = componentType->data->initializeType;
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_UNINITIALIZE:
        {
            callbackType = componentType->data->uninitializeType;
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_LOGIC_UPDATE:
        {
            callbackType = componentType->data->logicUpdateType;
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_HANDLE_EVENTS:
        {
            callbackType = componentType->data->handleEventsType;
            break;
        }
        default:
        {
            /* Error not thrown to keep interface simple. */
            kzsAssertText(KZ_FALSE, "Unknown enumeration value");
        }
    }

    if(callbackType == KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED && componentType->data->parentComponentType != KZ_NULL)
    {
        struct KzuUiComponentType* parentType = componentType->data->parentComponentType;
        kzsAssert(kzuUiComponentTypeIsValid(parentType));        
        returnValue = kzuUiComponentTypeGetInheritedCallback(parentType, callback);
    }
    else
    {
        returnValue = componentType;
    }
    return returnValue;
}

KzuComponentTypeIdentifier kzuUiComponentTypeGetTypeIdentifier(const struct KzuUiComponentType* componentType)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    return componentType->data->componentTypeIdentifier;
}

void kzuUiComponentTypeSetTypeIdentifier(const struct KzuUiComponentType* componentType, KzuComponentTypeIdentifier type)
{
    kzsAssert(kzuUiComponentTypeIsValid(componentType));
    componentType->data->componentTypeIdentifier = type;
}

kzsError kzuUiComponentTypeCallCallbackInitialize(const struct KzuUiComponentType* componentType, 
                                                  struct KzuUiComponentNode* componentNode, 
                                                  const struct KzcMemoryManager* memoryManager)
{
    kzsError result;
    
    kzsAssert(kzuUiComponentTypeIsValid(componentType));

    switch(componentType->data->initializeType)
    {
        case KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK:
        {
            if(componentType->data->initialize.callback != KZ_NULL)
            {
                result = componentType->data->initialize.callback(memoryManager, componentNode); 
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED:
        {
            if(componentType->data->parentComponentType != KZ_NULL)
            {
                result = kzuUiComponentTypeCallCallbackInitialize(componentType->data->parentComponentType, componentNode, memoryManager);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT:
        {
#undef ENABLE_LUA
#ifdef ENABLE_LUA
            kzMutableString functionName = componentType->data->initialize.script;
            const struct KzuLua* lua = kzuGlobalsGet(KZU_GLOBALS_LUA_STATE);

            result = kzuLuaCallFunction(lua, functionName, "pPpP", 
                "KzcMemoryManager", memoryManager,
                "KzuUiComponent");
            kzsErrorForward(result);
#endif
            break;
        }
    }
    kzsSuccess();
}

kzsError kzuUiComponentTypeCallCallbackUninitialize(const struct KzuUiComponentType* componentType, struct KzuUiComponentNode* componentNode)
{
    kzsError result;

    kzsAssert(kzuUiComponentTypeIsValid(componentType));

    switch(componentType->data->uninitializeType)
    {
        case KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK:
        {
            if(componentType->data->uninitialize.callback != KZ_NULL)
            {
                result = componentType->data->uninitialize.callback(componentNode);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED:
        {
            if(componentType->data->parentComponentType != KZ_NULL)
            {
                result = kzuUiComponentTypeCallCallbackUninitialize(componentType->data->parentComponentType, componentNode);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT:
        {
#undef ENABLE_LUA
#ifdef ENABLE_LUA
            kzMutableString functionName = componentType->data->uninitialize.script;
            const struct KzuLua* lua = kzuGlobalsGet(KZU_GLOBALS_LUA_STATE);
            result = kzuLuaCallFunction(lua, functionName, "pP", 
                "KzuUiComponent");
            kzsErrorForward(result);
#endif
            break;
        }
    }
    kzsSuccess();
}

kzsError kzuUiComponentTypeCallCallbackLogicUpdate(const struct KzuUiComponentType* componentType, 
                                                   const struct KzuUiComponentNode* componentNode, kzFloat deltaTime, struct KzuEngineMessageQueue* queue)
{
    kzsError result;

    kzsAssert(kzuUiComponentTypeIsValid(componentType));

    switch(componentType->data->logicUpdateType)
    {
        case KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK:
        {
            if(componentType->data->logicUpdate.callback != KZ_NULL)
            {
                result = componentType->data->logicUpdate.callback(componentNode, deltaTime, queue);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED:
        {
            if(componentType->data->parentComponentType != KZ_NULL)
            {
                result = kzuUiComponentTypeCallCallbackLogicUpdate(componentType->data->parentComponentType, componentNode, deltaTime, queue);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT:
        {
#undef ENABLE_LUA
#ifdef ENABLE_LUA
            kzMutableString functionName = componentType->data->logicUpdate.script;
            const struct KzuLua* lua = kzuGlobalsGet(KZU_GLOBALS_LUA_STATE);
            result = kzuLuaCallFunction(lua, functionName, "pPf", 
                "KzuUiComponentNode", componentNode,
                deltaTime);
            kzsErrorForward(result);
#endif
            break;
        }
    }
    kzsSuccess();
}

kzsError kzuUiComponentTypeCallCallbackHandleEvents(const struct KzuUiComponentType* componentType, 
                                                    const struct KzuUiComponentNode* componentNode,
                                                    kzString eventType,
                                                    struct KzuUiEvent* event,
                                                    struct KzuEngineMessageQueue* queue)
{
    kzsError result;

    kzsAssert(kzuUiComponentTypeIsValid(componentType));

    switch(componentType->data->handleEventsType)
    {
        case KZU_UI_COMPONENT_CALLBACK_TYPE_CALLBACK:
        {
            if(componentType->data->handleEvents.callback != KZ_NULL)
            {
                result = componentType->data->handleEvents.callback(componentNode, eventType, event, KZ_NULL, queue);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_TYPE_INHERITED:
        {
            if(componentType->data->parentComponentType != KZ_NULL)
            {
                result = kzuUiComponentTypeCallCallbackHandleEvents(componentType->data->parentComponentType, componentNode, eventType, event, queue);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_UI_COMPONENT_CALLBACK_TYPE_SCRIPT:
        {
#undef ENABLE_LUA
#ifdef ENABLE_LUA
            kzMutableString functionName = componentType->data->handleEvents.script;
            const struct KzuLua* lua = kzuGlobalsGet(KZU_GLOBALS_LUA_STATE);
            result = kzuLuaCallFunction(lua, functionName, "pPspPpP", 
                "KzuUiComponentNode", componentNode,
                eventType,
                "KzuUiEvent", event,
                "KzuEngineMessageQueue", queue);
            kzsErrorForward(result);
#endif
            break;
        }
    }
    kzsSuccess();
}
