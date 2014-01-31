/**
 * \file
 * Queue (First-In-First-Out).
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_QUEUE_H
#define KZC_QUEUE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;


/**
 * \struct KzcQueue
 * Structure of a queue.
 */
struct KzcQueue;


/** Creates a new initially empty queue. */
kzsError kzcQueueCreate(const struct KzcMemoryManager* memoryManager, struct KzcQueue** out_queue);

/** Frees the memory allocated for the queue. */
kzsError kzcQueueDelete(struct KzcQueue* queue);


/** Add the specified element to the queue. */
kzsError kzcQueueAdd(struct KzcQueue* queue, void* element);

/** Gets and removes the first element from the queue. Returns error if the queue is empty. */
kzsError kzcQueueReceive(struct KzcQueue* queue, void** out_element);

/** Gets the first element from the queue without removing it. Returns error if the queue is empty. */
kzsError kzcQueuePeek(const struct KzcQueue* queue, void** out_element);


/** Returns the number of elements in the queue. */
kzUint kzcQueueGetSize(const struct KzcQueue* queue);

/** Checks if the queue is empty or not. */
kzBool kzcQueueIsEmpty(const struct KzcQueue* queue);


/** Clears the queue by removing all items from it. */
kzsError kzcQueueClear(struct KzcQueue* queue);


#endif
