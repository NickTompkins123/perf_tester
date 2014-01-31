/**
* \file
* Message type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_message_type.h"

#include <core/memory/kzc_memory_manager.h>


/** Message type. */
struct KzuMessageType
{
    /*kzString name;*/
    enum KzuMessageRouting routing;
};


kzsError kzuMessageTypeCreate(const struct KzcMemoryManager* memoryManager, enum KzuMessageRouting routing, struct KzuMessageType** out_messageType)
{
    kzsError result;
    struct KzuMessageType* messageType;

    result = kzcMemoryAllocVariable(memoryManager, messageType, "Message type");
    kzsErrorForward(result);

    messageType->routing = routing;

    *out_messageType = messageType;
    kzsSuccess();
}

kzsError kzuMessageTypeDelete(struct KzuMessageType* messageType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(messageType));
    
    result = kzcMemoryFreeVariable(messageType);
    kzsErrorForward(result);

    kzsSuccess();
}

enum KzuMessageRouting kzuMessageTypeGetRouting(const struct KzuMessageType* messageType)
{
    kzsAssert(kzcIsValidPointer(messageType));

    return messageType->routing;
}
