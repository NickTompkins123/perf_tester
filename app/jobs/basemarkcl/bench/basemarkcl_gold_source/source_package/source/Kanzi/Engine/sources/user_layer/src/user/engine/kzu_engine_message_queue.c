/**
* \file
* Graphical user interface screen functionality.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_engine_message_queue.h"

#include <user/engine/kzu_engine_message.h>

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/debug/kzc_log.h>


/* TODO: Replace the implementation with kzc_queue */
/* TODO: Above TODO comment is not valid since this queues the events, but they can come out of the queue at different times due to delayed events. */
struct KzuEngineMessageQueue
{
    struct KzcDynamicArray* messages;
};


kzsError kzuEngineMessageQueueCreate(const struct KzcMemoryManager* memoryManager, struct KzuEngineMessageQueue** out_queue)
{
    kzsError result;
    struct KzuEngineMessageQueue* queue;
 
    result = kzcMemoryAllocVariable(memoryManager, queue, "UI Message Queue");
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &queue->messages);
    kzsErrorForward(result);

    *out_queue = queue;
    kzsSuccess();
}

kzsError kzuEngineMessageQueueDelete(struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzcDynamicArrayMutableIterator it;
    kzsAssert(kzcIsValidPointer(queue));

    it = kzuEngineMessageQueueGetMutableIterator(queue);
    while(kzcDynamicArrayMutableIterate(it))
    {
        struct KzuEngineMessage* message = kzcDynamicArrayMutableIteratorGetValue(it);
        result = kzuEngineMessageDelete(message);
        kzsErrorForward(result);                
        result = kzcDynamicArrayMutableIteratorRemove(it);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayDelete(queue->messages);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(queue);
    kzsErrorForward(result);   

    kzsSuccess();
}

kzsError kzuEngineMessageQueueAddItem(const struct KzuEngineMessageQueue* queue, struct KzuEngineMessage* message)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(queue));

    result = kzcDynamicArrayAdd(queue->messages, message);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcDynamicArrayIterator kzuEngineMessageQueueGetIterator(const struct KzuEngineMessageQueue* queue)
{
    kzsAssert(kzcIsValidPointer(queue));
    return kzcDynamicArrayGetIterator(queue->messages);
}

struct KzcDynamicArrayMutableIterator kzuEngineMessageQueueGetMutableIterator(const struct KzuEngineMessageQueue* queue)
{
    kzsAssert(kzcIsValidPointer(queue));
    return kzcDynamicArrayGetMutableIterator(queue->messages);
}

struct KzcDynamicArray* kzuEngineMessageQueueGetArray(const struct KzuEngineMessageQueue* queue)
{
    kzsAssert(kzcIsValidPointer(queue));
    return queue->messages;
}

kzUint kzuEngineMessageQueueGetSize(const struct KzuEngineMessageQueue* queue)
{
    kzsAssert(kzcIsValidPointer(queue));
    return kzcDynamicArrayGetSize(queue->messages);
}

kzsError kzuEngineMessageQueueClear(const struct KzuEngineMessageQueue* queue)
{
    kzsAssert(kzcIsValidPointer(queue));

    kzcDynamicArrayClear(queue->messages);
    
    kzsSuccess();
}
