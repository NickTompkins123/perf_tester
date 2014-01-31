/**
 * \file
 * Doubly-linked list.
 * 
 * For some operations such as remove and addAfter the target is located based on simple element address comparison.
 *
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_LINKED_LIST_H
#define KZC_LINKED_LIST_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzcLinkedListNode;


/**
 * \struct KzcLinkedList
 * Structure of a linked list.
 */
struct KzcLinkedList;


/** Read-only iterator for LinkedList. */
struct KzcLinkedListIterator
{
    kzBool reversed; /**< The iterator iterates from last to first. */
    const struct KzcLinkedList* linkedList_private; /**< Linked list associated with this iterator. */
    const struct KzcLinkedListNode* node_private; /**< Next node to iterate. */
};

/** Mutable iterator for LinkedList. */
struct KzcLinkedListMutableIterator
{
    kzBool reversed; /**< The iterator iterates from last to first. */
    struct KzcLinkedList* linkedList_private; /**< Linked list associated with this iterator. */
    struct KzcLinkedListNode* node_private; /**< Next node to iterate. */
};


/** Creates a new initially empty linked list. */
kzsError kzcLinkedListCreate(const struct KzcMemoryManager* memoryManager, struct KzcLinkedList** out_linkedList);

/** Frees the memory allocated for the linked list. */
kzsError kzcLinkedListDelete(struct KzcLinkedList* linkedList);


/** Adds the specified element to the linked list. */
kzsError kzcLinkedListAdd(struct KzcLinkedList* linkedList, void* element);

/** Adds the specified element to the beginning of linked list. */
kzsError kzcLinkedListAddToBeginning(struct KzcLinkedList* linkedList, void* element);

/** Adds the specified element after specified previousElement to the linked list. */
kzsError kzcLinkedListAddAfter(struct KzcLinkedList* linkedList, const void* previousElement, void* element);

/** Adds the specified element before specified nextElement to the linked list. */
kzsError kzcLinkedListAddBefore(struct KzcLinkedList* linkedList, const void* nextElement, void* element);


/** Removes the specified element from the linked list. Returns error if element is not found. */
kzsError kzcLinkedListRemove(struct KzcLinkedList* linkedList, const void* element);

/** Removes the first element from the linked list. Returns error if the linked list is empty. */
kzsError kzcLinkedListRemoveFirst(struct KzcLinkedList* linkedList);

/** Removes the last element from the linked list. Returns error if the linked list is empty. */
kzsError kzcLinkedListRemoveLast(struct KzcLinkedList* linkedList);


/** Returns the number of elements in the linked list. */
kzUint kzcLinkedListGetSize(const struct KzcLinkedList* linkedList);

/** Checks if the linked list is empty or not. */
kzBool kzcLinkedListIsEmpty(const struct KzcLinkedList* linkedList);


/** Returns the first element in the linked list. */
kzsError kzcLinkedListGetFirst(const struct KzcLinkedList* linkedList, void** out_element);
/** Returns the last element in the linked list. */
kzsError kzcLinkedListGetLast(const struct KzcLinkedList* linkedList, void** out_element);


/** Clears the linked list by removing all items from it. */
kzsError kzcLinkedListClear(struct KzcLinkedList* linkedList);


/** Returns a first-to-last iterator for all elements in the linked list. */
struct KzcLinkedListIterator kzcLinkedListGetIterator(const struct KzcLinkedList* linkedList);

/** Returns a last-to-first iterator for all elements in the linked list. */
struct KzcLinkedListIterator kzcLinkedListGetReverseIterator(const struct KzcLinkedList* linkedList);

/** Finds the next entry in the attached linked list. Returns KZ_TRUE if next entry is found, otherwise KZ_FALSE. */
#define kzcLinkedListIterate(iterator_param) kzcLinkedListIterate_private(&iterator_param)
/** Returns the value of the linked list entry pointed by the iterator. */
#define kzcLinkedListIteratorGetValue(iterator_param) kzcLinkedListIteratorGetValue_private(&iterator_param)

/** \see kzcLinkedListIterate */
kzBool kzcLinkedListIterate_private(struct KzcLinkedListIterator* iterator);
/** \see kzcLinkedListIteratorGetValue */
void* kzcLinkedListIteratorGetValue_private(const struct KzcLinkedListIterator* iterator);


/** Returns a mutable first-to-last iterator for all elements in the linked list. */
struct KzcLinkedListMutableIterator kzcLinkedListGetMutableIterator(struct KzcLinkedList* linkedList);

/** Returns a mutable last-to-first iterator for all elements in the linked list. */
struct KzcLinkedListMutableIterator kzcLinkedListGetReverseMutableIterator(struct KzcLinkedList* linkedList);

/** Finds the next entry in the attached linked list. Returns KZ_TRUE if next entry is found, otherwise KZ_FALSE. */
#define kzcLinkedListMutableIterate(iterator_param) kzcLinkedListMutableIterate_private(&iterator_param)
/** Returns the value of the linked list entry pointed by the iterator. */
#define kzcLinkedListMutableIteratorGetValue(iterator_param) kzcLinkedListMutableIteratorGetValue_private(&iterator_param)
/** Removes current entry of the linked list pointed by the iterator. */
#define kzcLinkedListMutableIteratorRemove(iterator_param) kzcLinkedListMutableIteratorRemove_private(&iterator_param)

/** \see kzcLinkedListIterate */
kzBool kzcLinkedListMutableIterate_private(struct KzcLinkedListMutableIterator* iterator);
/** \see kzcLinkedListIteratorGetValue */
void* kzcLinkedListMutableIteratorGetValue_private(const struct KzcLinkedListMutableIterator* iterator);
/** \see kzcLinkedListMutableIteratorRemove */
kzsError kzcLinkedListMutableIteratorRemove_private(struct KzcLinkedListMutableIterator* iterator);


#endif
