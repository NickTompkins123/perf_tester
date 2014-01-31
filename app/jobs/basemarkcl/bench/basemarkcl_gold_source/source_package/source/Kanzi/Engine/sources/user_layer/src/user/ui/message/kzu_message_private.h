/**
* \file
* Message object private functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_MESSAGE_PRIVATE_H
#define KZU_MESSAGE_PRIVATE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzuMessageType;
struct KzcMemoryManager;


/** Message object. */
struct KzuMessage
{
    kzBool valid;
    kzBool handled;
    struct KzuMessageDispatcher* dispatcher;
    const struct KzuMessageType* messageType;
    struct KzuObjectNode* source;
    struct KzcDynamicArray* arguments;
};


/** Create a message object. */
kzsError kzuMessageCreate_private(const struct KzcMemoryManager* memoryManager, struct KzuMessageDispatcher* messageDispatcher,
                                  const struct KzuMessageType* messageType, struct KzuObjectNode* source, struct KzuMessage** out_message);


#endif
