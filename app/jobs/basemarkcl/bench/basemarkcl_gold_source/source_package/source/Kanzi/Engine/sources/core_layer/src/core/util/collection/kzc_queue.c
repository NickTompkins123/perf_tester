/**
 * \file
 * Queue (First-In-First-Out).
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_queue.h"

#include "kzc_linked_list.h"
#include "kzc_linked_list_private.h"

#include <core/memory/kzc_memory_manager.h>


struct KzcQueue
{
    struct KzcLinkedList linkedList; /**< Linked list holding the data of the queue. */
};


kzsError kzcQueueCreate(const struct KzcMemoryManager* memoryManager, struct KzcQueue** out_queue)
{
    kzsError result;
    struct KzcQueue* queue;

    result = kzcLinkedListCreate(memoryManager, (struct KzcLinkedList**)&queue);
    kzsErrorForward(result);

    *out_queue = queue;
    kzsSuccess();
}

kzsError kzcQueueDelete(struct KzcQueue* queue)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(queue));

    result = kzcLinkedListDelete(&queue->linkedList);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcQueueAdd(struct KzcQueue* queue, void* element)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(queue));

    result = kzcLinkedListAdd(&queue->linkedList, element);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcQueueReceive(struct KzcQueue* queue, void** out_element)
{
    kzsError result;
    void* element;

    kzsAssert(kzcIsValidPointer(queue));

    result = kzcLinkedListGetFirst(&queue->linkedList, &element);
    kzsErrorForward(result);

    result = kzcLinkedListRemoveFirst(&queue->linkedList);
    kzsErrorForward(result);

    *out_element = element;
    kzsSuccess();
}

kzsError kzcQueuePeek(const struct KzcQueue* queue, void** out_element)
{
    kzsError result;
    void* element;

    kzsAssert(kzcIsValidPointer(queue));

    result = kzcLinkedListGetFirst(&queue->linkedList, &element);
    kzsErrorForward(result);

    *out_element = element;
    kzsSuccess();
}

kzUint kzcQueueGetSize(const struct KzcQueue* queue)
{
    kzsAssert(kzcIsValidPointer(queue));

    return kzcLinkedListGetSize(&queue->linkedList);
}

kzBool kzcQueueIsEmpty(const struct KzcQueue* queue)
{
    kzsAssert(kzcIsValidPointer(queue));

    return kzcLinkedListIsEmpty(&queue->linkedList);
}

kzsError kzcQueueClear(struct KzcQueue* queue)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(queue));

    result = kzcLinkedListClear(&queue->linkedList);
    kzsErrorForward(result);

    kzsSuccess();
}
