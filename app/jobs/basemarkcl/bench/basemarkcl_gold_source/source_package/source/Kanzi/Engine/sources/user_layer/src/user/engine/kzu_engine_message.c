/**
* \file
* Graphical user interface screen functionality.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_engine_message.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_string_property.h>

#include <core/util/collection/kzc_hash_map.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/debug/kzc_log.h>


/* TODO: document KzuEngineMessageType members. */

struct KzuEngineMessage
{
    enum KzuEngineMessageType type; /**< Type of the message. */
    struct KzcHashMap* arguments;   /**< Arguments <kzString, kzuProperty> */
    kzInt delay;                    /**< Delay left. Can be negative when all the delay is used. */
    void* userData;                 /**< Pointer to user set data. */
};


kzsError kzuEngineMessageCreate(const struct KzcMemoryManager* memoryManager, enum KzuEngineMessageType type, 
                                kzUint delayDuration, void* userData, struct KzuEngineMessage** out_message)
{
    kzsError result;
    struct KzuEngineMessage* message;
 
    result = kzcMemoryAllocVariable(memoryManager, message, "Engine Message");
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &message->arguments);
    kzsErrorForward(result);

    message->type = type;
    message->delay = (kzInt)delayDuration;
    message->userData = userData;

    *out_message = message;
    kzsSuccess();
}

kzsError kzuEngineMessageDelete(struct KzuEngineMessage* message)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(message));

    /* Delete properties which are not manually added. */
    {
        struct KzcHashMapIterator it;
        it = kzcHashMapGetIterator(message->arguments);
        while(kzcHashMapIterate(it))
        {
            struct KzuProperty* property = kzcHashMapIteratorGetValue(it);
            kzsAssert(kzcIsValidPointer(property));
            
            if(!kzuPropertyGetFromKzb(property))
            {
                result = kzuPropertyDelete(property);
                kzsErrorForward(result);
            }
        }
    }

    result = kzcHashMapDelete(message->arguments);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(message);
    kzsErrorForward(result);   

    kzsSuccess();
}

enum KzuEngineMessageType kzuEngineMessageGetType(const struct KzuEngineMessage* message)
{
    kzsAssert(kzcIsValidPointer(message));
    return message->type;
}

kzsError kzuEngineMessageAddArgument(const struct KzuEngineMessage* message, kzString argumentName, void* argument)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(message));

    result = kzcHashMapPut(message->arguments, argumentName, argument);
    kzsErrorForward(result);

    kzsSuccess();
}

void* kzuEngineMessageGetArgument(const struct KzuEngineMessage* message, kzString argumentName)
{
    void* value;
    kzsAssert(kzcIsValidPointer(message));

    if(!kzcHashMapGet(message->arguments, argumentName, &value))
    {
        value = KZ_NULL;
    }

    return value;
}

void* kzuEngineMessageGetUserData(const struct KzuEngineMessage* message)
{    
    kzsAssert(kzcIsValidPointer(message));
    return message->userData;
}

kzBool kzuEngineMessageUpdate(struct KzuEngineMessage* message, kzUint deltaTime)
{
    kzBool returnValue;
    kzsAssert(kzcIsValidPointer(message));
    
    message->delay -= (kzInt)deltaTime;
    returnValue = ((message->delay > 0) ? KZ_FALSE : KZ_TRUE);
    return returnValue;
}
