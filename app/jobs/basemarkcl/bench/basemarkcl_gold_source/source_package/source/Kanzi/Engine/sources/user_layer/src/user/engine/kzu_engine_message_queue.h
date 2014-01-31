/**
* \file
* User interface message queue.
* Provides way to send messages from user interface to engine.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_MESSAGE_QUEUE_H
#define KZU_MESSAGE_QUEUE_H

#include <system/input/kzs_input.h>
#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuEngineMessage;
struct KzcMemoryManager;
struct KzcDynamicArray;


/**
 * \struct KzuUiMessageQueue
 * Structure for holding messages.
 */
struct KzuEngineMessageQueue;


/**
* \struct KzuEngineMessageQueueHelper
* Structure for passing helper queue and duration over void pointer.
*/
struct KzuEngineMessageQueueHelper
{
    struct KzuEngineMessageQueue* queue; /**< Reference to the queue. */
    kzUint delayDuration; /**< Delay duration. */
    void* userData; /**< Pointer to user data. */
};


/** Creates the message queue. */
kzsError kzuEngineMessageQueueCreate(const struct KzcMemoryManager* memoryManager, struct KzuEngineMessageQueue** out_queue);
/** Deletes message queue. */
kzsError kzuEngineMessageQueueDelete(struct KzuEngineMessageQueue* queue);

/** Adds message to message queue. */
kzsError kzuEngineMessageQueueAddItem(const struct KzuEngineMessageQueue* queue, struct KzuEngineMessage* message);
/** Gets iterator from message queue. */
struct KzcDynamicArrayIterator kzuEngineMessageQueueGetIterator(const struct KzuEngineMessageQueue* queue);
/** Gets mutable iterator from message queue. */
struct KzcDynamicArrayMutableIterator kzuEngineMessageQueueGetMutableIterator(const struct KzuEngineMessageQueue* queue);
/** Gets the array of messages from message queue. */
struct KzcDynamicArray* kzuEngineMessageQueueGetArray(const struct KzuEngineMessageQueue* queue);
/** Gets length of message queue. */
kzUint kzuEngineMessageQueueGetSize(const struct KzuEngineMessageQueue* queue);

/** Clears message queue. */
kzsError kzuEngineMessageQueueClear(const struct KzuEngineMessageQueue* queue);


#endif
