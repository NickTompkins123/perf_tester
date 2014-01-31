/**
* \file
* User interface event listener.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_EVENT_LISTENER_H
#define KZU_UI_EVENT_LISTENER_H

#include <user/ui/kzu_ui_event.h>

#include <core/util/collection/kzc_hash_map.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/** Types of possible argument types. */
enum KzuUiArgumentType
{
    KZU_UI_ARGUMENT_TYPE_FIXED,
    KZU_UI_ARGUMENT_TYPE_FORWARD_ARGUMENT,
    KZU_UI_ARGUMENT_TYPE_FORWARD_PROPERTY
};

/** Possible location for user interface action. */
enum KzuUiActionLocation
{
    KZU_UI_ACTION_LOCATION_ENGINE,
    KZU_UI_ACTION_LOCATION_APPLICATION,
    KZU_UI_ACTION_LOCATION_COMPONENT
};


/* Forward declarations. */
struct KzcMemoryManager;
struct KzuUiEvent;
struct KzuUiAction;
struct KzuEngineMessageQueue;
struct KzuUiComponentNode;
struct KzuProperty;

/**
* \struct KzuUiArgument
* Structure containing data for user interface event listener arguments.
*/
struct KzuUiEventListenerArgument;

/**
* \struct KzuUiArgument
* Structure containing data for user interface components event listener.
*/
struct KzuUiEventListener;


/** Creates new user interface event listener. */
kzsError kzuUiEventListenerCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzuUiEventListener** out_eventListener);
/** Deletes event listener. */
kzsError kzuUiEventListenerDelete(struct KzuUiEventListener* eventListener);

/** Creates a copy of ui event listener. */
kzsError kzuUiEventListenerCopy(const struct KzcMemoryManager* memoryManager, const struct KzuUiEventListener* sourceEventListener, struct KzuUiEventListener** out_eventListenerCopy);

/** Adds given argument to given event listener. */
kzsError kzuUiEventListenerAddArgument(const struct KzuUiEventListener *eventListener, struct KzuUiEventListenerArgument* argument);
/** Sets event listener to target a component. */
void kzuUiEventListenerSetTargetComponent(struct KzuUiEventListener* eventListener, struct KzuUiComponentNode* targetComponentNode);
/** Sets event listener path target a component. The component node is loaded from this path after the object itself is loaded. */
kzsError kzuUiEventListenerSetTargetComponentPath(struct KzuUiEventListener* eventListener, kzString path);
/** Sets event listener path target a component. The component node is loaded from this path after the object itself is loaded. */
kzString kzuUiEventListenerGetTargetComponentPath(const struct KzuUiEventListener* eventListener);
/** Sets event listener to target a component. */
void kzuUiEventListenerSetAction(struct KzuUiEventListener* eventListener, struct KzuUiAction* action);
/** Gets event listener target action. */
struct KzuUiAction* kzuUiEventListenerGetAction(const struct KzuUiEventListener* eventListener);
/** Gets location of the action attached to listener. */
enum KzuUiActionLocation kzuUiEventListenerGetTargetLocation(const struct KzuUiEventListener* eventListener);

/** Sets delay time that must be elapsed before the action in the listener is fired. */
void kzuUiEventListenerSetDelay(struct KzuUiEventListener* eventListener, kzUint delay);
/** Gets delay from the listener. */
kzUint kzuUiEventListenerGetDelay(const struct KzuUiEventListener* eventListener);

/** Creates new user interface action with callback. */
kzsError kzuUiEventListenerArgumentCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzuUiEventListenerArgument** out_argument);
/** Deletes action. */
kzsError kzuUiEventListenerArgumentDelete(struct KzuUiEventListenerArgument* argument);

/** Creates a copy of event listener argument. */
kzsError kzuUiEventListenerArgumentCopy(const struct KzcMemoryManager* memoryManager, const struct KzuUiEventListenerArgument* sourceArgument, 
    struct KzuUiEventListenerArgument** out_argument);


/** Gets name from given argument. */
kzString kzuUiEventListenerArgumentGetName(const struct KzuUiEventListenerArgument* argument);

/** Sets given argument to use fixed property as its value. */
void kzuUiEventListenerArgumentSetFixedValue(struct KzuUiEventListenerArgument* argument, struct KzuProperty* property);
/** Sets given argument to forward a event parameter of given name as its value. */
kzsError kzuUiEventListenerArgumentSetForwardEventValue(struct KzuUiEventListenerArgument* argument, kzString argumentName);
/** Sets given argument to forward a property parameter of given name as its value. */
kzsError kzuUiEventListenerArgumentSetForwardValue(struct KzuUiEventListenerArgument* argument, kzString argumentName);

/** Calls action attached to this event listener. */
kzsError kzuUiEventListenerCallCallback(const struct KzuUiEventListener* eventListener, const struct KzuUiComponentNode* component, 
                                        struct KzuUiEvent* event, kzString eventType, struct KzuEngineMessageQueue* queue);


#endif
