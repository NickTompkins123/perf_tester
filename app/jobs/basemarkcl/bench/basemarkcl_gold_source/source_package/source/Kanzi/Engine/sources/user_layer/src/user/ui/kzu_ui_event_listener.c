/**
* \file
* User event listener.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_event_listener.h"
#include "kzu_ui_action.h"
#include "kzu_ui_component.h"

#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/engine/kzu_engine_message_queue.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>


struct KzuUiEventListenerArgument
{
    enum KzuUiArgumentType type;    /**< Type of the argument. */
    kzMutableString name;           /**< Name of the argument. */
    union
    {
        struct KzuProperty* property; /**< Property to be forwarded as argument. */
        kzMutableString forwardName;  /**< Name of the property to be forwarded from the launching object node. */
    } argument;
};


struct KzuUiEventListener
{
    kzMutableString eventName;  /**< Event which fires the action. */
    struct KzuUiAction* action; /**< Action fired when this event is received. */
    kzUint delay; /**< Delay duration. */

    enum KzuUiActionLocation locationType;  /**< Location of the action to be launched. */
    struct KzuUiComponentNode* locationComponent; /**< Location of the action. KZ_NULL if object targets itself. Ignored if locationType is not component. */
    kzMutableString componentPath; /**< Path to the component target. Set to KZ_NULL after it is no longer needed. Stores temp data during loading. */

    struct KzcHashMap* arguments; /**< Arguments to be passed to action. <kzString, KzuUiEventListenerArgument*> */
};


kzsError kzuUiEventListenerCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzuUiEventListener** out_eventListener)
{
    kzsError result;
    struct KzuUiEventListener* eventListener;

    result = kzcMemoryAllocVariable(memoryManager, eventListener, "UI Event Listener");
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &eventListener->arguments);
    kzsErrorForward(result);

    result = kzcStringCopy(memoryManager, name, &eventListener->eventName);
    kzsErrorForward(result);
    
    eventListener->locationType = KZU_UI_ACTION_LOCATION_ENGINE;
    eventListener->componentPath = KZ_NULL;
    eventListener->locationComponent = KZ_NULL;
        
    *out_eventListener = eventListener;
    kzsSuccess();
}

kzsError kzuUiEventListenerDelete(struct KzuUiEventListener* eventListener)
{
    kzsError result;
    struct KzcHashMapIterator it;

    if(eventListener->componentPath != KZ_NULL)
    {
        result = kzcStringDelete(eventListener->componentPath);
        kzsErrorForward(result);
    }

    result = kzcStringDelete(eventListener->eventName);
    kzsErrorForward(result);

    it = kzcHashMapGetIterator(eventListener->arguments);
    while(kzcHashMapIterate(it))
    {
        struct KzuUiEventListenerArgument* argument = (struct KzuUiEventListenerArgument*)kzcHashMapIteratorGetValue(it);
        result = kzuUiEventListenerArgumentDelete(argument);
        kzsErrorForward(result);
    }
    result = kzcHashMapDelete(eventListener->arguments);
    kzsErrorForward(result);

    /* Action can be null, if it is introduced in tool, but not available in this preview. */
    if(eventListener->action != KZ_NULL)
    {
        result = kzuUiActionDelete(eventListener->action);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(eventListener);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiEventListenerCopy(const struct KzcMemoryManager* memoryManager, const struct KzuUiEventListener* sourceEventListener, struct KzuUiEventListener** out_eventListenerCopy)
{
    kzsError result;
    struct KzcHashMapIterator it;
    struct KzuUiEventListener* eventListener;

    result = kzcMemoryAllocVariable(memoryManager, eventListener, "UI Event Listener");
    kzsErrorForward(result);

    result = kzcStringCopy(memoryManager, sourceEventListener->eventName, &eventListener->eventName);
    kzsErrorForward(result);

    /* Action can be null, if it is introduced in tool, but not available in this preview. */
    result = kzuUiActionCopy(memoryManager, sourceEventListener->action, &eventListener->action);
    kzsErrorForward(result);

    eventListener->delay = sourceEventListener->delay;

    eventListener->locationType = sourceEventListener->locationType;
    eventListener->locationComponent = sourceEventListener->locationComponent;
    eventListener->componentPath = KZ_NULL;
    if(sourceEventListener->componentPath != KZ_NULL)
    {
        result = kzcStringCopy(memoryManager, sourceEventListener->componentPath, &eventListener->componentPath);
        kzsErrorForward(result);
    }

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &eventListener->arguments);
    kzsErrorForward(result);
    it = kzcHashMapGetIterator(sourceEventListener->arguments);
    while(kzcHashMapIterate(it))
    {
        struct KzuUiEventListenerArgument* newArgument;
        struct KzuUiEventListenerArgument* argument = (struct KzuUiEventListenerArgument*)kzcHashMapIteratorGetValue(it);
        result = kzuUiEventListenerArgumentCopy(memoryManager, argument, &newArgument);
        kzsErrorForward(result);
        result = kzuUiEventListenerAddArgument(eventListener, newArgument);
        kzsErrorForward(result);
    }

    *out_eventListenerCopy = eventListener;
    kzsSuccess();
}

kzsError kzuUiEventListenerAddArgument(const struct KzuUiEventListener *eventListener, struct KzuUiEventListenerArgument* argument)
{
    kzsError result;
    kzString name;

    kzsAssert(kzcIsValidPointer(eventListener));
    kzsAssert(kzcIsValidPointer(argument));

    name = kzuUiEventListenerArgumentGetName(argument);
    result = kzcHashMapPut(eventListener->arguments, name, argument);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuUiEventListenerSetTargetComponent(struct KzuUiEventListener* eventListener, struct KzuUiComponentNode* targetComponentNode)
{
    kzsAssert(kzcIsValidPointer(eventListener));
    /* Target component node can be KZ_NULL since it marks target component as caller component. */
    eventListener->locationType = KZU_UI_ACTION_LOCATION_COMPONENT;
    eventListener->locationComponent = targetComponentNode;
}

kzsError kzuUiEventListenerSetTargetComponentPath(struct KzuUiEventListener* eventListener, kzString path)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(eventListener));
    
    /* Target component node can be KZ_NULL since it marks target component as caller component. */
    eventListener->locationType = KZU_UI_ACTION_LOCATION_COMPONENT;

    /* Remove old path. */
    if(eventListener->componentPath != KZ_NULL)
    {
        result = kzcStringDelete(eventListener->componentPath);
        kzsErrorForward(result);
    }

    /* Save new path. */
    if(path == KZ_NULL)
    {
        eventListener->componentPath = KZ_NULL;
    }
    else
    {
        result = kzcStringCopy(kzcMemoryGetManager(eventListener), path, &eventListener->componentPath);
        kzsErrorForward(result);
    }
    
    kzsSuccess();
}

kzString kzuUiEventListenerGetTargetComponentPath(const struct KzuUiEventListener* eventListener)
{
    kzsAssert(kzcIsValidPointer(eventListener));
    return eventListener->componentPath;
}

void kzuUiEventListenerSetAction(struct KzuUiEventListener* eventListener, struct KzuUiAction* action)
{
    kzsAssert(kzcIsValidPointer(eventListener));
    eventListener->action = action;
}

struct KzuUiAction* kzuUiEventListenerGetAction(const struct KzuUiEventListener* eventListener)
{
    kzsAssert(kzcIsValidPointer(eventListener));
    return eventListener->action;
}

enum KzuUiActionLocation kzuUiEventListenerGetTargetLocation(const struct KzuUiEventListener* eventListener)
{
    kzsAssert(kzcIsValidPointer(eventListener));
    return eventListener->locationType;
}

void kzuUiEventListenerSetDelay(struct KzuUiEventListener* eventListener, kzUint delay)
{
    kzsAssert(kzcIsValidPointer(eventListener));
    eventListener->delay = delay;
}

kzUint kzuUiEventListenerGetDelay(const struct KzuUiEventListener* eventListener)
{
    kzsAssert(kzcIsValidPointer(eventListener));
    return eventListener->delay;
}

kzsError kzuUiEventListenerArgumentCreate(const struct KzcMemoryManager* memoryManager, kzString name, 
                                          struct KzuUiEventListenerArgument** out_argument)
{
    kzsError result;
    struct KzuUiEventListenerArgument* argument;

    result = kzcMemoryAllocVariable(memoryManager, argument, "Ui Event Listener Argument");
    kzsErrorForward(result);
    result = kzcStringCopy(memoryManager, name, &argument->name);
    kzsErrorForward(result);

    *out_argument = argument;
    kzsSuccess();
}

kzsError kzuUiEventListenerArgumentDelete(struct KzuUiEventListenerArgument* argument)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(argument));

    result = kzcStringDelete(argument->name);
    kzsErrorForward(result);

    switch(argument->type)
    {
        case KZU_UI_ARGUMENT_TYPE_FIXED:
        {
            result = kzuPropertyDelete(argument->argument.property);
            kzsErrorForward(result);
            break;
        }
        case KZU_UI_ARGUMENT_TYPE_FORWARD_ARGUMENT:
        {
            result = kzcStringDelete(argument->argument.forwardName);
            kzsErrorForward(result);
            break;
        }
        case KZU_UI_ARGUMENT_TYPE_FORWARD_PROPERTY:
        {
            result = kzcStringDelete(argument->argument.forwardName);
            kzsErrorForward(result);
            break;
        }
        default:
        {
            break;
        }
    }

    result = kzcMemoryFreeVariable(argument);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiEventListenerArgumentCopy(const struct KzcMemoryManager* memoryManager, const struct KzuUiEventListenerArgument* sourceArgument, 
    struct KzuUiEventListenerArgument** out_argument)
{
    kzsError result;
    struct KzuUiEventListenerArgument* argument;

    kzsAssert(kzcIsValidPointer(sourceArgument));

    result = kzcMemoryAllocVariable(memoryManager, argument, "Ui Event Listener Argument");
    kzsErrorForward(result);
    result = kzcStringCopy(memoryManager, sourceArgument->name, &argument->name);
    kzsErrorForward(result);

    argument->type = sourceArgument->type;

    switch(argument->type)
    {
        case KZU_UI_ARGUMENT_TYPE_FIXED:
        {
            result = kzuPropertyCopy(memoryManager, sourceArgument->argument.property, &argument->argument.property);
            kzsErrorForward(result);
            break;
        }
        case KZU_UI_ARGUMENT_TYPE_FORWARD_ARGUMENT:
        {
            result = kzcStringCopy(memoryManager, sourceArgument->argument.forwardName, &argument->argument.forwardName);
            kzsErrorForward(result);
            break;
        }
        case KZU_UI_ARGUMENT_TYPE_FORWARD_PROPERTY:
        {
            result = kzcStringCopy(memoryManager, sourceArgument->argument.forwardName, &argument->argument.forwardName);
            kzsErrorForward(result);
            break;
        }
        default:
        {
            break;
        }
    }

    *out_argument = argument;
    kzsSuccess();
}

kzString kzuUiEventListenerArgumentGetName(const struct KzuUiEventListenerArgument* argument)
{
    kzsAssert(kzcIsValidPointer(argument));
    return argument->name;
}

void kzuUiEventListenerArgumentSetFixedValue(struct KzuUiEventListenerArgument* argument, struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(argument));

    argument->type = KZU_UI_ARGUMENT_TYPE_FIXED;
    argument->argument.property = property;
}

kzsError kzuUiEventListenerArgumentSetForwardEventValue(struct KzuUiEventListenerArgument* argument, kzString argumentName)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(argument));

    argument->type = KZU_UI_ARGUMENT_TYPE_FORWARD_ARGUMENT;
    result = kzcStringCopy(kzcMemoryGetManager(argument), argumentName, &argument->argument.forwardName);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiEventListenerArgumentSetForwardValue(struct KzuUiEventListenerArgument* argument, kzString argumentName)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(argument));

    argument->type = KZU_UI_ARGUMENT_TYPE_FORWARD_PROPERTY;
    result = kzcStringCopy(kzcMemoryGetManager(argument), argumentName, &argument->argument.forwardName);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiEventListenerCallCallback(const struct KzuUiEventListener* eventListener, const struct KzuUiComponentNode* componentNode, 
                                        struct KzuUiEvent* event, kzString eventType, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    void* parameterData;
    struct KzcHashMap* parameters = KZ_NULL;
    struct KzuEngineMessageQueueHelper helper;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcIsValidPointer(eventListener));

    memoryManager = kzcMemoryGetManager(eventListener);

    /* TODO: Which memory manager to use for this? Quick - but should we have a quick memory manager for each event listener? */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(eventListener->arguments);

        /* Create hash map for parameters. */
        result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &parameters);
        kzsErrorForward(result);

        /* Iterate eventListener->arguments and fill parameters array. */
        while(kzcHashMapIterate(it))
        {
            struct KzuProperty* sourceProperty;
            struct KzuUiEventListenerArgument* argument = (struct KzuUiEventListenerArgument*)kzcHashMapIteratorGetValue(it);
            kzString name = (kzString)kzcHashMapIteratorGetKey(it);

            kzsAssert(kzcIsValidPointer(argument));
            
            switch(argument->type)
            {
                case KZU_UI_ARGUMENT_TYPE_FIXED:
                {
                    result = kzuPropertyCopy(memoryManager, argument->argument.property, &sourceProperty);
                    kzsErrorForward(result);
                    break;
                }
                case KZU_UI_ARGUMENT_TYPE_FORWARD_ARGUMENT:
                {
                    struct KzuPropertyCollection* propertyCollection = kzuUiEventGetEventPropertyCollection(event);
                    result = kzuPropertyCopy(memoryManager, kzuPropertyCollectionGetProperty(propertyCollection, argument->argument.forwardName), &sourceProperty);
                    kzsErrorForward(result);
                    break;
                }
                case KZU_UI_ARGUMENT_TYPE_FORWARD_PROPERTY:
                {
                    struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);
                    struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(objectNode);
                    result = kzuPropertyManagerGetPropertyByName_old(propertyManager, objectNode, argument->argument.forwardName, &sourceProperty);
                    kzsErrorForward(result);
                    break;
                }
                default:
                {
                    kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown action argument type.");
                }
            }

            /* Disable from KZB property, since we want to delete these properties. */
            kzuPropertyDisableFromKzb(sourceProperty);

            result = kzcHashMapPut(parameters, name, sourceProperty);
            kzsErrorForward(result);
        }
    }

    /* Custom callbacks which are not available current preview are ignored. */
    if(eventListener->action != KZ_NULL)
    {
        if(kzuUiActionGetRequiresMessageQueue(eventListener->action))
        {
            helper.queue = queue;
            helper.delayDuration = eventListener->delay;
            helper.userData = kzuUiActionGetUserData(eventListener->action);
            parameterData = (void*)&helper;
        }
        else
        {
            parameterData = kzuUiActionGetUserData(eventListener->action);
        }

        if(eventListener->locationType == KZU_UI_ACTION_LOCATION_COMPONENT)
        {
            struct KzuVoidProperty* property = KZ_NULL;
            const struct KzuUiComponentNode* targetComponentNode;

            targetComponentNode = (eventListener->locationComponent == KZ_NULL ? componentNode : eventListener->locationComponent);
            result = kzuVoidPropertyCreate(memoryManager, kzuVoidPropertyTypeFromPropertyType((struct KzuPropertyType*)KZU_PROPERTY_TYPE_TARGET_COMPONENT_LOCATION), (void*)targetComponentNode, &property);
            kzsErrorForward(result);
            result = kzcHashMapPut(parameters, kzuPropertyTypeGetName(KZU_PROPERTY_TYPE_TARGET_COMPONENT_LOCATION), property);
            kzsErrorForward(result);
            
            result = kzuUiActionCall(eventListener->action, targetComponentNode, parameterData, eventType, event, parameters);
            kzsErrorForward(result);
        }
        else
        {
            result = kzuUiActionCall(eventListener->action, componentNode, parameterData, eventType, event, parameters);
            kzsErrorForward(result);
        }
    }

    if(parameters != KZ_NULL)
    {
        result = kzcHashMapDelete(parameters);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
