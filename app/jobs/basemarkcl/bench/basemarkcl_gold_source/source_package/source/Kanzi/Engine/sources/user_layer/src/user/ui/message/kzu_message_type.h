/**
* \file
* Message type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_MESSAGE_TYPE_H
#define KZU_MESSAGE_TYPE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;


/** Message type descriptor. */
struct KzuMessageType;


/** Message routing enumeration. */
enum KzuMessageRouting
{
    KZU_MESSAGE_ROUTING_TUNNELLING_BUBBLING     = 0,
    KZU_MESSAGE_ROUTING_TUNNELLING              = 1,
    KZU_MESSAGE_ROUTING_BUBBLING                = 2,
    KZU_MESSAGE_ROUTING_DIRECT                  = 3
};


/** Create a message type. */
kzsError kzuMessageTypeCreate(const struct KzcMemoryManager* memoryManager, enum KzuMessageRouting routing, struct KzuMessageType** out_messageType);
/** Delete a message type. */
kzsError kzuMessageTypeDelete(struct KzuMessageType* messageType);

/** Gets the routing method of a message type. */
enum KzuMessageRouting kzuMessageTypeGetRouting(const struct KzuMessageType* messageType);


#endif
