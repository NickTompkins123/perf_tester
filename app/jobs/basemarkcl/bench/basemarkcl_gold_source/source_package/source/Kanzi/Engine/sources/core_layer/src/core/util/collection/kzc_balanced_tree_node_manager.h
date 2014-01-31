/**
 * \file
 * Node manager for balanced tree.
 * This file provides possibility to create balanced tree with more control on the internal node handling.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_BALANCED_TREE_NODE_MANAGER_H
#define KZC_BALANCED_TREE_NODE_MANAGER_H


#include <core/util/collection/kzc_comparator.h>

#include <system/debug/kzs_error.h>


/** Structure of a BalancedTreeNode. */
struct KzcBalancedTreeNode
{
    struct KzcBalancedTreeNode* parent; /**< Parent node of the node. */
    struct KzcBalancedTreeNode* left; /**< Left child node of the node. */
    struct KzcBalancedTreeNode* right; /**< Right child node of the node. */
    void* element; /**< Data of the node. */
};


/* Forward declarations */
struct KzcBalancedTree;


/** Swaps the content of the two nodes. */
typedef void (*KzcNodeSwapFunction)(struct KzcBalancedTreeNode* first, struct KzcBalancedTreeNode* second);


/** Creates a new initially empty balanced tree with given node management functions. */
kzsError kzcBalancedTreeCreateManaged(const struct KzcMemoryManager* manager, KzcComparatorFunction comparator,
                                      KzcNodeSwapFunction swapHandler, struct KzcBalancedTree** out_balancedTree);

/** Deletes a balanced tree without removing the nodes in it. */
kzsError kzcBalancedTreeDeleteManaged(struct KzcBalancedTree* balancedTree);

/** Gets the root node of the tree. */
struct KzcBalancedTreeNode* kzcBalancedTreeGetRoot(const struct KzcBalancedTree* balancedTree);

/** Sets the root node of the tree. */
void kzcBalancedTreeSetRoot(struct KzcBalancedTree* balancedTree, struct KzcBalancedTreeNode* node);

/** Adds the specified node to the balanced tree. */
kzsError kzcBalancedTreeAddNode(struct KzcBalancedTree* balancedTree, struct KzcBalancedTreeNode* node);

/**
 * Removes the specified node from the balanced tree.
 * To simplify the removal process, the removed node might not actually be the given one due to node swapping in the tree.
 * The actual removed node is the return value.
 */
struct KzcBalancedTreeNode* kzcBalancedTreeRemoveNode(struct KzcBalancedTree* balancedTree, struct KzcBalancedTreeNode* node);

/** Returns the first node in the tree. */
struct KzcBalancedTreeNode* kzcBalancedTreeGetFirstNode(const struct KzcBalancedTree* balancedTree);
/** Returns the last node in the tree. */
struct KzcBalancedTreeNode* kzcBalancedTreeGetLastNode(const struct KzcBalancedTree* balancedTree);

/** Gets the node with equal value with the given one according to the ordering of the tree. */
struct KzcBalancedTreeNode* kzcBalancedTreeGetEqualNode(const struct KzcBalancedTree* balancedTree, const void* element);
/** Gets the node with the highest element lower than the given one according to the ordering of the tree. */
struct KzcBalancedTreeNode* kzcBalancedTreeGetLowerNode(const struct KzcBalancedTree* balancedTree, const void* element);
/** Gets the node with the highest element lower or equal than the given one according to the ordering of the tree. */
struct KzcBalancedTreeNode* kzcBalancedTreeGetLowerOrEqualNode(const struct KzcBalancedTree* balancedTree, const void* element);
/** Gets the node with the highest element higher than the given one according to the ordering of the tree. */
struct KzcBalancedTreeNode* kzcBalancedTreeGetHigherNode(const struct KzcBalancedTree* balancedTree, const void* element);
/** Gets the node with the highest element higher or equal than the given one according to the ordering of the tree. */
struct KzcBalancedTreeNode* kzcBalancedTreeGetHigherOrEqualNode(const struct KzcBalancedTree* balancedTree, const void* element);


#endif
