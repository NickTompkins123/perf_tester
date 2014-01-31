/**
* \file
* Message dispatcher.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_MESSAGE_DISPATCHER_H
#define KZU_MESSAGE_DISPATCHER_H


#include "kzu_message_type.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzuMessageType;
struct KzuMessage;
struct KzuObjectNode;


/** Message dispatcher object. */
struct KzuMessageDispatcher;


/** Message callback prototype. */
typedef kzsError (*KzuMessageHandlerFunction)(struct KzuMessage* message, void* userData);


/** Create a message dispatcher. */
kzsError kzuMessageDispatcherCreate(const struct KzcMemoryManager* memoryManager, struct KzuMessageDispatcher** out_messageDispatcher);
/** Destroy a message dispatcher. */
kzsError kzuMessageDispatcherDelete(struct KzuMessageDispatcher* messageDispatcher);


/** Creates a message object. */
kzsError kzuMessageDispatcherCreateMessage(struct KzuMessageDispatcher* messageDispatcher, const struct KzuMessageType* messageType, struct KzuObjectNode* source,
                                           struct KzuMessage** out_Message);

/** Adds a message handler. */
kzsError kzuMessageDispatcherAddHandler(const struct KzuMessageDispatcher* messageDispatcher, struct KzuObjectNode* subscriptionSiteNode,
                                        const struct KzuMessageType* messageType, struct KzuObjectNode* messageSource, enum KzuMessageRouting routing,
                                        KzuMessageHandlerFunction handlerFunction, void* userData);

/** Removes a message handler. */
kzsError kzuMessageDispatcherRemoveHandler(const struct KzuMessageDispatcher* messageDispatcher, const struct KzuObjectNode* subscriptionSiteNode,
                                           const struct KzuMessageType* messageType, KzuMessageHandlerFunction handlerFunction, const void* userData);

/** Posts a message. The associated handlers are called when kzuMessageDispatcherDispatch is called. */
kzsError kzuMessageDispatcherPostMessage(const struct KzuMessageDispatcher* messageDispatcher, struct KzuMessage* message);

/** Sends a message. The associated handlers are called immediately. */
kzsError kzuMessageDispatcherSendMessage(const struct KzuMessageDispatcher* messageDispatcher, struct KzuMessage* message);

/** Dispatches enqueued messages. Generally should be called once per frame. */
kzsError kzuMessageDispatcherDispatch(struct KzuMessageDispatcher* messageDispatcher);

/** Notifies the message dispatcher that an object node is being destroyed. This will cancel all messages where the specified node is the source of the message or the current site during routing. */
kzsError kzuMessageDispatcherNotifyObjectDeleted(const struct KzuMessageDispatcher* messageDispatcher, const struct KzuObjectNode* objectNode);


#endif
