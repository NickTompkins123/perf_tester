/**
 * \file
 * Doubly-linked list.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_linked_list.h"

#include "kzc_linked_list_private.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/kzc_error_codes.h>


kzsError kzcLinkedListCreate(const struct KzcMemoryManager* memoryManager, struct KzcLinkedList** out_linkedList)
{
    kzsError result;
    struct KzcLinkedList* linkedList;

    result = kzcMemoryAllocVariable(memoryManager, linkedList, "Linked list");
    kzsErrorForward(result);

    linkedList->first = KZ_NULL;
    linkedList->last = KZ_NULL;
    linkedList->size = 0;

    *out_linkedList = linkedList;
    kzsSuccess();
}

kzsError kzcLinkedListDelete(struct KzcLinkedList* linkedList)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(linkedList));

    /* Delete all nodes. */
    if (linkedList->size > 0)
    {
        result = kzcLinkedListClear(linkedList);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(linkedList);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzcLinkedListCreateNode_internal(const struct KzcMemoryManager* memoryManager,
                                                 struct KzcLinkedListNode* previous, struct KzcLinkedListNode* next,
                                                 void* element, struct KzcLinkedListNode** out_node)
{
    kzsError result;
    struct KzcLinkedListNode* node;

    result = kzcMemoryAllocVariable(memoryManager, node, "Linked list node");
    kzsErrorForward(result);

    node->previous = previous;
    node->next = next;
    node->data = element;

    *out_node = node;
    kzsSuccess();
}

static kzsError kzcLinkedListCreateFirstNode_internal(struct KzcLinkedList* linkedList, void* element)
{
    kzsError result;
    struct KzcLinkedListNode* node;

    kzsAssert(kzcIsValidPointer(linkedList));
    kzsAssert(linkedList->size == 0);
    kzsAssert(linkedList->first == KZ_NULL);
    kzsAssert(linkedList->last == KZ_NULL);

    result = kzcLinkedListCreateNode_internal(kzcMemoryGetManager(linkedList), KZ_NULL, KZ_NULL, element, &node);
    kzsErrorForward(result);

    linkedList->first = node;
    linkedList->last = node;
    ++linkedList->size;

    kzsSuccess();
}

static kzsError kzcLinkedListAddAfterNode_internal(struct KzcLinkedList* linkedList, struct KzcLinkedListNode* previous, void* element)
{
    kzsError result;
    struct KzcLinkedListNode* node;

    kzsAssert(kzcIsValidPointer(linkedList));
    kzsAssert(kzcIsValidPointer(previous));
    kzsAssert(linkedList->first != KZ_NULL);
    kzsAssert(linkedList->last != KZ_NULL);

    result = kzcLinkedListCreateNode_internal(kzcMemoryGetManager(linkedList), previous, previous->next, element, &node);
    kzsErrorForward(result);

    /* Fix next link from previous node. */
    previous->next = node;
    /* Fix previous link from next node. */
    if (node->next != KZ_NULL)
    {
        node->next->previous = node;
    }
    /* Fix last node. */
    else
    {
        linkedList->last = node;
    }

    ++linkedList->size;

    kzsSuccess();
}

static kzsError kzcLinkedListAddBeforeNode_internal(struct KzcLinkedList* linkedList, struct KzcLinkedListNode* next, void* element)
{
    kzsError result;
    struct KzcLinkedListNode* node;

    kzsAssert(kzcIsValidPointer(linkedList));
    kzsAssert(kzcIsValidPointer(next));
    kzsAssert(linkedList->first != KZ_NULL);
    kzsAssert(linkedList->last != KZ_NULL);

    result = kzcLinkedListCreateNode_internal(kzcMemoryGetManager(linkedList), next->previous, next, element, &node);
    kzsErrorForward(result);

    /* Fix previous link from next node. */
    next->previous = node;
    /* Fix next link from previous node. */
    if (node->previous != KZ_NULL)
    {
        node->previous->next = node;
    }
    /* Fix first node. */
    else
    {
        linkedList->first = node;
    }

    ++linkedList->size;

    kzsSuccess();
}

kzsError kzcLinkedListAdd(struct KzcLinkedList* linkedList, void* element)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(linkedList));

    if (linkedList->size == 0)
    {
        result = kzcLinkedListCreateFirstNode_internal(linkedList, element);
        kzsErrorForward(result);
    }
    else
    {
        result = kzcLinkedListAddAfterNode_internal(linkedList, linkedList->last, element);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzcLinkedListAddToBeginning(struct KzcLinkedList* linkedList, void* element)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(linkedList));

    if (linkedList->size == 0)
    {
        result = kzcLinkedListAdd(linkedList, element);
        kzsErrorForward(result);
    }
    else
    {
        result = kzcLinkedListAddBeforeNode_internal(linkedList, linkedList->first, element);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

static struct KzcLinkedListNode* kzcLinkedListFindNode_internal(const struct KzcLinkedList* linkedList, const void* element)
{
    struct KzcLinkedListNode* node = linkedList->first;

    while (node != KZ_NULL && node->data != element)
    {
        node = node->next;
    }

    return node;
}

kzsError kzcLinkedListAddAfter(struct KzcLinkedList* linkedList, const void* previousElement, void* element)
{
    kzsError result;
    struct KzcLinkedListNode* previousNode;

    kzsAssert(kzcIsValidPointer(linkedList));

    previousNode = kzcLinkedListFindNode_internal(linkedList, previousElement);
    kzsErrorTest(previousNode != KZ_NULL, KZC_ERROR_ELEMENT_NOT_FOUND, "Given previous element was not found from linked list");

    result = kzcLinkedListAddAfterNode_internal(linkedList, previousNode, element);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcLinkedListAddBefore(struct KzcLinkedList* linkedList, const void* nextElement, void* element)
{
    kzsError result;
    struct KzcLinkedListNode* nextNode;

    kzsAssert(kzcIsValidPointer(linkedList));

    nextNode = kzcLinkedListFindNode_internal(linkedList, nextElement);
    kzsErrorTest(nextNode != KZ_NULL, KZC_ERROR_ELEMENT_NOT_FOUND, "Given next element was not found from linked list");

    result = kzcLinkedListAddBeforeNode_internal(linkedList, nextNode, element);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzcLinkedListRemoveNode_internal(struct KzcLinkedList* linkedList, struct KzcLinkedListNode* node)
{
    kzsError result;

    if (node->previous == KZ_NULL)
    {
        linkedList->first = node->next;
    }
    else
    {
        node->previous->next = node->next;
    }

    if (node->next == KZ_NULL)
    {
        linkedList->last = node->previous;
    }
    else
    {
        node->next->previous = node->previous;
    }

    result = kzcMemoryFreeVariable(node);
    kzsErrorForward(result);

    --linkedList->size;

    kzsSuccess();
}

kzsError kzcLinkedListRemove(struct KzcLinkedList* linkedList, const void* element)
{
    kzsError result;
    struct KzcLinkedListNode* node;

    node = kzcLinkedListFindNode_internal(linkedList, element);
    kzsErrorTest(node != KZ_NULL, KZC_ERROR_ELEMENT_NOT_FOUND, "Given next element was not found from linked list");

    result = kzcLinkedListRemoveNode_internal(linkedList, node);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcLinkedListRemoveFirst(struct KzcLinkedList* linkedList)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(linkedList));

    kzsErrorTest(linkedList->size > 0, KZC_ERROR_ELEMENT_NOT_FOUND, "Trying to remove element from empty linked list");

    result = kzcLinkedListRemoveNode_internal(linkedList, linkedList->first);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcLinkedListRemoveLast(struct KzcLinkedList* linkedList)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(linkedList));

    kzsErrorTest(linkedList->size > 0, KZC_ERROR_ELEMENT_NOT_FOUND, "Trying to remove element from empty linked list");

    result = kzcLinkedListRemoveNode_internal(linkedList, linkedList->last);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzcLinkedListGetSize(const struct KzcLinkedList* linkedList)
{
    kzsAssert(kzcIsValidPointer(linkedList));

    return linkedList->size;
}

kzBool kzcLinkedListIsEmpty(const struct KzcLinkedList* linkedList)
{
    kzsAssert(kzcIsValidPointer(linkedList));

    return (linkedList->size == 0);
}

kzsError kzcLinkedListGetFirst(const struct KzcLinkedList* linkedList, void** out_element)
{
    kzsAssert(kzcIsValidPointer(linkedList));

    kzsErrorTest(linkedList->size > 0, KZC_ERROR_ELEMENT_NOT_FOUND, "Trying to get first element of empty linked list");

    *out_element = linkedList->first->data;
    kzsSuccess();
}

kzsError kzcLinkedListGetLast(const struct KzcLinkedList* linkedList, void** out_element)
{
    kzsAssert(kzcIsValidPointer(linkedList));

    kzsErrorTest(linkedList->size > 0, KZC_ERROR_ELEMENT_NOT_FOUND, "Trying to get last element of empty linked list");

    *out_element = linkedList->last->data;
    kzsSuccess();
}

kzsError kzcLinkedListClear(struct KzcLinkedList* linkedList)
{
    kzsError result;
    struct KzcLinkedListNode* node;

    kzsAssert(kzcIsValidPointer(linkedList));

    node = linkedList->first;

    while (node != KZ_NULL)
    {
        struct KzcLinkedListNode* next = node->next;

        result = kzcMemoryFreeVariable(node);
        kzsErrorForward(result);

        node = next;
    }

    linkedList->first = KZ_NULL;
    linkedList->last = KZ_NULL;

    linkedList->size = 0;

    kzsSuccess();
}

struct KzcLinkedListIterator kzcLinkedListGetIterator(const struct KzcLinkedList* linkedList)
{
    struct KzcLinkedListIterator iterator;

    iterator.reversed = KZ_FALSE;
    iterator.linkedList_private = linkedList;
    iterator.node_private = KZ_NULL;

    return iterator;
}

struct KzcLinkedListIterator kzcLinkedListGetReverseIterator(const struct KzcLinkedList* linkedList)
{
    struct KzcLinkedListIterator iterator;

    iterator.reversed = KZ_TRUE;
    iterator.linkedList_private = linkedList;
    iterator.node_private = KZ_NULL;

    return iterator;
}

kzBool kzcLinkedListIterate_private(struct KzcLinkedListIterator* iterator)
{
    if (iterator->reversed)
    {
        if (iterator->node_private == KZ_NULL)
        {
            iterator->node_private = iterator->linkedList_private->last;
        }
        else
        {
            iterator->node_private = iterator->node_private->previous;
        }
    }
    else
    {
        if (iterator->node_private == KZ_NULL)
        {
            iterator->node_private = iterator->linkedList_private->first;
        }
        else
        {
            iterator->node_private = iterator->node_private->next;
        }
    }
    return iterator->node_private != KZ_NULL;
}

void* kzcLinkedListIteratorGetValue_private(const struct KzcLinkedListIterator* iterator)
{
    return iterator->node_private->data;
}

struct KzcLinkedListMutableIterator kzcLinkedListGetMutableIterator(struct KzcLinkedList* linkedList)
{
    struct KzcLinkedListMutableIterator iterator;

    iterator.reversed = KZ_FALSE;
    iterator.linkedList_private = linkedList;
    iterator.node_private = KZ_NULL;

    return iterator;
}

struct KzcLinkedListMutableIterator kzcLinkedListGetReverseMutableIterator(struct KzcLinkedList* linkedList)
{
    struct KzcLinkedListMutableIterator iterator;

    iterator.reversed = KZ_TRUE;
    iterator.linkedList_private = linkedList;
    iterator.node_private = KZ_NULL;

    return iterator;
}

kzBool kzcLinkedListMutableIterate_private(struct KzcLinkedListMutableIterator* iterator)
{
    if (iterator->reversed)
    {
        if (iterator->node_private == KZ_NULL)
        {
            iterator->node_private = iterator->linkedList_private->last;
        }
        else
        {
            iterator->node_private = iterator->node_private->previous;
        }
    }
    else
    {
        if (iterator->node_private == KZ_NULL)
        {
            iterator->node_private = iterator->linkedList_private->first;
        }
        else
        {
            iterator->node_private = iterator->node_private->next;
        }
    }

    return iterator->node_private != KZ_NULL;
}

void* kzcLinkedListMutableIteratorGetValue_private(const struct KzcLinkedListMutableIterator* iterator)
{
    return iterator->node_private->data;
}

kzsError kzcLinkedListMutableIteratorRemove_private(struct KzcLinkedListMutableIterator* iterator)
{
    kzsError result;
    struct KzcLinkedListNode* continueNode;

    kzsAssert(kzcIsValidPointer(iterator->node_private));

    if (iterator->reversed)
    {
        continueNode = iterator->node_private->next;
    }
    else
    {
        continueNode = iterator->node_private->previous;
    }

    result = kzcLinkedListRemoveNode_internal(iterator->linkedList_private, iterator->node_private);
    kzsErrorForward(result);

    iterator->node_private = continueNode;

    kzsSuccess();
}
