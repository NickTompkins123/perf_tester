/**
* \file
* Standard message types.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_message_type.h"
#include "kzu_message.h"
#include "kzu_message_dispatcher.h"
#include "kzu_general_messages.h"

#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/memory/kzc_memory_manager.h>


const struct KzuMessageType* KZU_MESSAGE_DELETE_OBJECT;                            /*lint !e956*/
const struct KzuMessageType* KZU_MESSAGE_POINTER_DOWN;                             /*lint !e956*/
const struct KzuMessageType* KZU_MESSAGE_POINTER_UP;                               /*lint !e956*/


kzsError kzuGeneralMessagesPostDeleteObject(struct KzuObjectNode* object)
{
    kzsError result;
    struct KzuMessageDispatcher* dispatcher = kzuObjectNodeGetMessageDispatcher(object);
    struct KzuMessage* message;

    result = kzuMessageDispatcherCreateMessage(dispatcher, KZU_MESSAGE_DELETE_OBJECT, object, &message);
    kzsErrorForward(result);

    result = kzuMessageSetVoidArgument(message, KZU_PROPERTY_TYPE_MESSAGE_ARGUMENT_TARGET_OBJECT, object);
    kzsErrorForward(result);

    result = kzuMessageDispatcherPostMessage(dispatcher, message);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuMessageCreateGeneralTypes(const struct KzcMemoryManager* memoryManager, struct KzcDynamicArray** out_messageArray)
{
    kzsError result;
    struct KzcDynamicArray* array;

    result = kzcDynamicArrayCreate(memoryManager, &array);
    kzsErrorForward(result);

    /* Destroy object. */
    {
        struct KzuMessageType* messageType;
        result = kzuMessageTypeCreate(memoryManager, KZU_MESSAGE_ROUTING_TUNNELLING_BUBBLING, &messageType);
        kzsErrorForward(result);
        result = kzcDynamicArrayAdd(array, messageType);
        kzsErrorForward(result);

        KZU_MESSAGE_DELETE_OBJECT = messageType;
    }

    /* Pointer down. */
    {
        struct KzuMessageType* messageType;
        result = kzuMessageTypeCreate(memoryManager, KZU_MESSAGE_ROUTING_TUNNELLING_BUBBLING, &messageType);
        kzsErrorForward(result);
        result = kzcDynamicArrayAdd(array, messageType);
        kzsErrorForward(result);

        KZU_MESSAGE_POINTER_DOWN = messageType;
    }

    /* Pointer up. */
    {
        struct KzuMessageType* messageType;
        result = kzuMessageTypeCreate(memoryManager, KZU_MESSAGE_ROUTING_TUNNELLING_BUBBLING, &messageType);
        kzsErrorForward(result);
        result = kzcDynamicArrayAdd(array, messageType);
        kzsErrorForward(result);

        KZU_MESSAGE_POINTER_UP = messageType;
    }

    *out_messageArray = array;
    kzsSuccess();
}

kzsError kzuMessageDeleteGeneralTypes(struct KzcDynamicArray* messageArray)
{
    kzsError result;
    struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(messageArray);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuMessageType* messageType = kzcDynamicArrayIteratorGetValue(it);
        result = kzuMessageTypeDelete(messageType);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayDelete(messageArray);
    kzsErrorForward(result);

    kzsSuccess();
}
