/**
 * \file
 * Structure definitions for doubly-linked list.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_LINKED_LIST_PRIVATE_H
#define KZC_LINKED_LIST_PRIVATE_H


#include <system/kzs_types.h>


/** Linked list node. */
struct KzcLinkedListNode
{
    struct KzcLinkedListNode* previous; /**< Previous node. */
    struct KzcLinkedListNode* next; /**< Next node. */
    void* data; /**< Data of the node. */
};

struct KzcLinkedList
{
    struct KzcLinkedListNode* first; /**< First node. */
    struct KzcLinkedListNode* last; /**< Last node. */
    kzUint size; /**< Number of nodes in the list. */
};


#endif
