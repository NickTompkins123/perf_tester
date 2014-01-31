/**
 * \file
 * Stack (Last-In-First-Out).
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_stack.h"

#include "kzc_linked_list.h"
#include "kzc_linked_list_private.h"

#include <core/memory/kzc_memory_manager.h>


struct KzcStack
{
    struct KzcLinkedList linkedList; /**< Linked list holding the data of the stack. */
};


kzsError kzcStackCreate(const struct KzcMemoryManager* memoryManager, struct KzcStack** out_stack)
{
    kzsError result;
    struct KzcStack* stack;

    result = kzcLinkedListCreate(memoryManager, (struct KzcLinkedList**)&stack);
    kzsErrorForward(result);

    *out_stack = stack;
    kzsSuccess();
}

kzsError kzcStackDelete(struct KzcStack* stack)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(stack));

    result = kzcLinkedListDelete(&stack->linkedList);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcStackPush(struct KzcStack* stack, void* element)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(stack));

    result = kzcLinkedListAdd(&stack->linkedList, element);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcStackPop(struct KzcStack* stack, void** out_element)
{
    kzsError result;
    void* element;

    kzsAssert(kzcIsValidPointer(stack));

    result = kzcLinkedListGetLast(&stack->linkedList, &element);
    kzsErrorForward(result);

    result = kzcLinkedListRemoveLast(&stack->linkedList);
    kzsErrorForward(result);

    *out_element = element;
    kzsSuccess();
}

kzsError kzcStackPeek(const struct KzcStack* stack, void** out_element)
{
    kzsError result;
    void* element;

    kzsAssert(kzcIsValidPointer(stack));

    result = kzcLinkedListGetLast(&stack->linkedList, &element);
    kzsErrorForward(result);

    *out_element = element;
    kzsSuccess();
}

kzUint kzcStackGetSize(const struct KzcStack* stack)
{
    kzsAssert(kzcIsValidPointer(stack));

    return kzcLinkedListGetSize(&stack->linkedList);
}

kzBool kzcStackIsEmpty(const struct KzcStack* stack)
{
    kzsAssert(kzcIsValidPointer(stack));

    return kzcLinkedListIsEmpty(&stack->linkedList);
}

kzsError kzcStackClear(struct KzcStack* stack)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(stack));

    result = kzcLinkedListClear(&stack->linkedList);
    kzsErrorForward(result);

    kzsSuccess();
}
