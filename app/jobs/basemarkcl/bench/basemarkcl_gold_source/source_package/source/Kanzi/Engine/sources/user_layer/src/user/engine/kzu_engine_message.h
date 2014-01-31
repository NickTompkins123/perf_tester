/**
* \file
* Engine message queue.
* Provides way to send messages from user interface to engine.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_ENGINE_MESSAGE_H
#define KZU_ENGINE_MESSAGE_H

#include <system/input/kzs_input.h>
#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/** Enumeration of the available engine messages. */
enum KzuEngineMessageType
{
    KZU_ENGINE_MESSAGE_TYPE_CHANGE_SCENE,
    KZU_ENGINE_MESSAGE_START_ANIMATION,
    KZU_ENGINE_MESSAGE_SET_PROPERTY_VALUE,
    KZU_ENGINE_MESSAGE_SWITCH_BOOLEAN_PROPERTY,
    KZU_ENGINE_MESSAGE_SET_BOOLEAN_PROPERTY,
    KZU_ENGINE_MESSAGE_START_TRANSITION,
    KZU_ENGINE_MESSAGE_SET_SLIDER_VALUE,
    KZU_ENGINE_MESSAGE_INCREASE_SLIDER_VALUE,
    KZU_ENGINE_MESSAGE_DECREASE_SLIDER_VALUE,
    KZU_ENGINE_MESSAGE_CHANGE_RENDER_PASS_CAMERA,
    KZU_ENGINE_MESSAGE_CHANGE_2D_PICKING_CAMERA,
    KZU_ENGINE_MESSAGE_SET_TRANSFORMATION_TO_IDENTITY,
    KZU_ENGINE_MESSAGE_EXECUTE_SCRIPT,
    KZU_ENGINE_MESSAGE_CHANGE_SCENE_CAMERA,
    KZU_ENGINE_MESSAGE_SWITCH_TOGGLE_BUTTON_STATE,
    KZU_ENGINE_MESSAGE_DROPDOWN_OPEN,
    KZU_ENGINE_MESSAGE_DROPDOWN_CLOSE,
    KZU_ENGINE_MESSAGE_DROPDOWN_SWITCH_OPEN_STATE,
    KZU_ENGINE_MESSAGE_DROPDOWN_SET_SELECTED_ITEM,
    KZU_ENGINE_MESSAGE_RENDER_PASS_SET_ENABLED_STATE,
    KZU_ENGINE_MESSAGE_RENDER_PASS_TOGGLE_ENABLED_STATE,
    KZU_ENGINE_MESSAGE_RENDER_PASS_RENDER_FOR_ONE_FRAME_AND_DISABLE,
    KZU_ENGINE_MESSAGE_CHANGE_SCENE_COMPOSER,
    KZU_ENGINE_MESSAGE_SET_TEXTURE_PROPERTY
};


/* Forward declarations. */
struct KzcMemoryManager;


/**
* \struct KzuEngineMessage
* Structure for delivering information from user interface to engine.
* Can be used to deliver for example commands for starting animations and changing scenes.
*/
struct KzuEngineMessage;


/** Creates the message. */
kzsError kzuEngineMessageCreate(const struct KzcMemoryManager* memoryManager, enum KzuEngineMessageType type, 
                                kzUint delayDuration, void* userData, struct KzuEngineMessage** out_message);
/** Deletes the message. */
kzsError kzuEngineMessageDelete(struct KzuEngineMessage* message);


/** Gets message type from message. */
enum KzuEngineMessageType kzuEngineMessageGetType(const struct KzuEngineMessage* message);


/** Adds new argument to a message. */
kzsError kzuEngineMessageAddArgument(const struct KzuEngineMessage* message, kzString argumentName, void* argument);
/** Gets argument from message. */
void* kzuEngineMessageGetArgument(const struct KzuEngineMessage* message, kzString argumentName);
/** Gets user data from message. */
void* kzuEngineMessageGetUserData(const struct KzuEngineMessage* message);


/** Updates message. If message should be launched after the update return value is KZ_TRUE, KZ_FALSE otherwise. */
kzBool kzuEngineMessageUpdate(struct KzuEngineMessage* message, kzUint deltaTime);


#endif
