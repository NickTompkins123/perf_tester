/**
 * \file
 * Self-balancing binary search tree.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_BALANCED_TREE_H
#define KZC_BALANCED_TREE_H


#include "kzc_comparator.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzcBalancedTreeNode;


/**
 * \struct KzcBalancedTree
 * Structure of a BalancedTree.
 */
struct KzcBalancedTree;


/** Read-only iterator for BalancedTree. */
struct KzcBalancedTreeIterator
{
    const struct KzcBalancedTree* tree_private; /**< Balanced tree associated with this iterator. */
    const struct KzcBalancedTreeNode* node_private; /**< Current node being iterated. */
    const struct KzcBalancedTreeNode* next_private; /**< Next node to iterate. */
};


/** Creates a new initially empty balanced tree. */
kzsError kzcBalancedTreeCreate(const struct KzcMemoryManager* memoryManager, KzcComparatorFunction comparator,
                               struct KzcBalancedTree** out_balancedTree);

/** Frees the memory allocated for the balanced tree. */
kzsError kzcBalancedTreeDelete(struct KzcBalancedTree* balancedTree);

/** Adds the specified element to the balanced tree. */
kzsError kzcBalancedTreeAdd(struct KzcBalancedTree* balancedTree, void* element);

/**
 * Removes the specified element from the balanced tree. Decides equality by simple address comparison.
 * Returns error if element is not found.
 */
kzsError kzcBalancedTreeRemove(struct KzcBalancedTree* balancedTree, const void* element);

/** Returns the number of elements in the balanced tree. */
kzUint kzcBalancedTreeGetSize(const struct KzcBalancedTree* balancedTree);

/** Returns the first element in the tree. */
void* kzcBalancedTreeGetFirst(const struct KzcBalancedTree* balancedTree);
/** Returns the last element in the tree. */
void* kzcBalancedTreeGetLast(const struct KzcBalancedTree* balancedTree);


/** Returns an iterator for all elements in the balanced tree. */
struct KzcBalancedTreeIterator kzcBalancedTreeGetIterator(const struct KzcBalancedTree* balancedTree);

/** Finds the next entry in the attached balanced tree. Returns KZ_TRUE if next entry is found, otherwise KZ_FALSE. */
#define kzcBalancedTreeIterate(iterator_param) kzcBalancedTreeIterate_private(&iterator_param)
/** Returns the value of the balanced tree entry pointed by the iterator. */
#define kzcBalancedTreeIteratorGetValue(iterator_param) kzcBalancedTreeIteratorGetValue_private(&iterator_param)

/** \see kzcBalancedTreeIterate */
kzBool kzcBalancedTreeIterate_private(struct KzcBalancedTreeIterator* iterator);
/** \see kzcBalancedTreeIteratorGetValue */
void* kzcBalancedTreeIteratorGetValue_private(const struct KzcBalancedTreeIterator* iterator);


/** Gets the highest element lower than given one according to the ordering of the tree. */
void* kzcBalancedTreeGetLower(const struct KzcBalancedTree* balancedTree, const void* element);
/** Gets the highest element lower or equal than given one according to the ordering of the tree. */
void* kzcBalancedTreeGetLowerOrEqual(const struct KzcBalancedTree* balancedTree, const void* element);
/** Gets the lowest element higher than given one according to the ordering of the tree. */
void* kzcBalancedTreeGetHigher(const struct KzcBalancedTree* balancedTree, const void* element);
/** Gets the lowest element higher or equal than given one according to the ordering of the tree. */
void* kzcBalancedTreeGetHigherOrEqual(const struct KzcBalancedTree* balancedTree, const void* element);

/** Clears the tree by removing all nodes from it. */
kzsError kzcBalancedTreeClear(struct KzcBalancedTree* balancedTree);


#endif
