/**
 * \file
 * Self-balancing tree.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_balanced_tree.h"

#include "kzc_balanced_tree_node_manager.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/kzc_error_codes.h>


/** Structure of a BalancedTree. */
struct KzcBalancedTree
{
    kzUint elementCount; /**< Number of elements in the balanced tree. */
    KzcComparatorFunction comparator; /**< Comparator for the elements. */
    struct KzcBalancedTreeNode* root; /**< Root node of the tree. */
    kzBool removeFlipFlop; /**< Flip-flop variable to determine which subtree of a node should be used for swapping when removing the node. */

    /* Node manager */
    KzcNodeSwapFunction swapHandler; /**< Handler function for swapping elements. */
};

kzsError kzcBalancedTreeCreateManaged(const struct KzcMemoryManager* memoryManager, KzcComparatorFunction comparator, KzcNodeSwapFunction swapHandler, struct KzcBalancedTree** out_balancedTree)
{
    kzsError result;
    struct KzcBalancedTree* balancedTree;

    result = kzcMemoryAllocVariable(memoryManager, balancedTree, "BalancedTree");
    kzsErrorForward(result);

    balancedTree->elementCount = 0;
    balancedTree->comparator = comparator;
    balancedTree->root = KZ_NULL;
    balancedTree->removeFlipFlop = KZ_FALSE;

    /* Node manager */
    balancedTree->swapHandler = swapHandler;

    *out_balancedTree = balancedTree;
    kzsSuccess();
}

static void kzcDefaultSwapHandler_internal(struct KzcBalancedTreeNode* first, struct KzcBalancedTreeNode* second)
{
    void* tempElement = first->element;
    first->element = second->element;
    second->element = tempElement;
}

kzsError kzcBalancedTreeCreate(const struct KzcMemoryManager* memoryManager, KzcComparatorFunction comparator, struct KzcBalancedTree** out_balancedTree)
{
    kzsError result;
    
    result = kzcBalancedTreeCreateManaged(memoryManager, comparator, kzcDefaultSwapHandler_internal, out_balancedTree);
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzsError kzcBalancedTreeDeleteManaged(struct KzcBalancedTree* balancedTree)
{
    kzsError result;

    result = kzcMemoryFreeVariable(balancedTree);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcBalancedTreeDelete(struct KzcBalancedTree* balancedTree)
{
    kzsError result;
    
    result = kzcBalancedTreeClear(balancedTree);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(balancedTree);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcBalancedTreeNode* kzcBalancedTreeGetRoot(const struct KzcBalancedTree* balancedTree)
{
    return balancedTree->root;
}

void kzcBalancedTreeSetRoot(struct KzcBalancedTree* balancedTree, struct KzcBalancedTreeNode* node)
{
    balancedTree->root = node;
}

static kzsError kzcBalancedTreeCreateNode_internal(const struct KzcMemoryManager* memoryManager, void* element, struct KzcBalancedTreeNode** out_node)
{
    kzsError result;

    struct KzcBalancedTreeNode* node;
    result = kzcMemoryAllocVariable(memoryManager, node, "BalancedTreeNode");
    kzsErrorForward(result);

    node->element = element;
    node->left = KZ_NULL;
    node->right = KZ_NULL;
    
    *out_node = node;
    kzsSuccess();
}

kzsError kzcBalancedTreeAddNode(struct KzcBalancedTree* balancedTree, struct KzcBalancedTreeNode* node)
{
    struct KzcBalancedTreeNode* parent = KZ_NULL;
    struct KzcBalancedTreeNode** nodePointer = &(balancedTree->root);
    kzBool found = KZ_FALSE;
    do 
    {
        struct KzcBalancedTreeNode* currentNode = *nodePointer;

        if (currentNode == KZ_NULL)
        {
            node->parent = parent;
            *nodePointer = node;
            ++balancedTree->elementCount;
            found = KZ_TRUE;
        }
        else
        {
            kzInt comparison = (*balancedTree->comparator)(currentNode->element, node->element);
            if (comparison > 0)
            {
                nodePointer = &(currentNode->left);
            }
            else if (comparison < 0)
            {
                nodePointer = &(currentNode->right);
            }
            else
            {
                kzsErrorThrow(KZC_ERROR_DUPLICATE_ELEMENT, "Given element already exist in the tree (with the given comparator).");
            }
            parent = currentNode;
        }
    } while (!found);

    kzsSuccess();
}

kzsError kzcBalancedTreeAdd(struct KzcBalancedTree* balancedTree, void* element)
{
    kzsError result;

    struct KzcBalancedTreeNode* node;
    result = kzcBalancedTreeCreateNode_internal(kzcMemoryGetManager(balancedTree), element, &node);
    kzsErrorForward(result);

    result = kzcBalancedTreeAddNode(balancedTree, node);
    kzsErrorForward(result);

    kzsSuccess();
}

typedef kzUint KzcBalancedTreeFindOperation; /**< Bit-mask for find operation. */
/* Basic operations */
#define KZC_FIND_OPERATION_EQUALS           (KzcBalancedTreeFindOperation)0x01
#define KZC_FIND_OPERATION_LOWER            (KzcBalancedTreeFindOperation)0x02
#define KZC_FIND_OPERATION_HIGHER           (KzcBalancedTreeFindOperation)0x04
/* Combined operations */
#define KZC_FIND_OPERATION_LOWER_OR_EQUALS  (KZC_FIND_OPERATION_LOWER | KZC_FIND_OPERATION_EQUALS)
#define KZC_FIND_OPERATION_HIGHER_OR_EQUALS (KZC_FIND_OPERATION_HIGHER | KZC_FIND_OPERATION_EQUALS)

static struct KzcBalancedTreeNode* kzcBalancedTreeFindNode_internal(const struct KzcBalancedTree* balancedTree, KzcBalancedTreeFindOperation operation, const void* element)
{
    struct KzcBalancedTreeNode* node = balancedTree->root;
    struct KzcBalancedTreeNode* resultNode = KZ_NULL;
    struct KzcBalancedTreeNode* lastNode = KZ_NULL;
    struct KzcBalancedTreeNode* leastLowerNode = KZ_NULL;
    struct KzcBalancedTreeNode* leastHigherNode = KZ_NULL;
    
    kzsAssert((operation & (KZC_FIND_OPERATION_EQUALS | KZC_FIND_OPERATION_LOWER | KZC_FIND_OPERATION_HIGHER)) != 0); /* At least one operation required. */
    kzsAssert((operation & KZC_FIND_OPERATION_LOWER) == 0 || (operation & KZC_FIND_OPERATION_HIGHER) == 0); /* Both lower and higher is disallowed. */

    /* Find the exact match */
    while (node != KZ_NULL && resultNode == KZ_NULL)
    {
        kzInt comparison = (*balancedTree->comparator)(node->element, element);
        lastNode = node;
        if (comparison > 0)
        {
            leastHigherNode = node;
            node = node->left;
        }
        else if (comparison < 0)
        {
            leastLowerNode = node;
            node = node->right;
        }
        else
        {
            resultNode = node;
        }
    }

    /* If the operation is exactly equals or the operation accepts equals and result has been found, stop the search. If not, search further. */
    if (!(operation == KZC_FIND_OPERATION_EQUALS ||
          ((operation & KZC_FIND_OPERATION_EQUALS) != 0 && resultNode != KZ_NULL) ||
          lastNode == KZ_NULL))
    {
        /* If exact match is not an accepted operation */
        if ((operation & KZC_FIND_OPERATION_EQUALS) == 0)
        {
            node = lastNode;
            resultNode = KZ_NULL;

            /* Find highest from the left sub-tree */
            if ((operation & KZC_FIND_OPERATION_LOWER) != 0)
            {
                node = node->left;
                while (node != KZ_NULL)
                {
                    resultNode = node;
                    node = node->right;
                }
            }
            /* Find lowest from the right sub-tree */
            else if ((operation & KZC_FIND_OPERATION_HIGHER) != 0)
            {
                node = node->right;
                while (node != KZ_NULL)
                {
                    resultNode = node;
                    node = node->left;
                }
            }
            else
            {
                kzsAssert(KZ_FALSE);
            }
        }

        /* If the result has yet to be found. */
        if (resultNode == KZ_NULL)
        {
            if ((operation & KZC_FIND_OPERATION_LOWER) != 0)
            {
                resultNode = leastLowerNode;
            }
            else if ((operation & KZC_FIND_OPERATION_HIGHER) != 0)
            {
                resultNode = leastHigherNode;
            }
            else
            {
                kzsAssert(KZ_FALSE);
            }
        }
    }

    return resultNode;
}

/** Find the variable that is holding a pointer for the given node. */
static struct KzcBalancedTreeNode** kzcBalancedTreeGetNodeHolder_internal(struct KzcBalancedTree* balancedTree, const struct KzcBalancedTreeNode* node)
{
    struct KzcBalancedTreeNode** nodeHolder;

    if (node == balancedTree->root)
    {
        nodeHolder = &balancedTree->root;
    }
    else
    {
        kzsAssert(node->parent != KZ_NULL);

        if (node->parent->left == node)
        {
            nodeHolder = &node->parent->left;
        }
        else if (node->parent->right == node)
        {
            nodeHolder = &node->parent->right;
        }
        else
        {
            nodeHolder = KZ_NULL;
            kzsAssert(KZ_FALSE);
        }
    }

    return nodeHolder;
}

struct KzcBalancedTreeNode* kzcBalancedTreeRemoveNode(struct KzcBalancedTree* balancedTree, struct KzcBalancedTreeNode* node)
{
    struct KzcBalancedTreeNode** nodeHolder;
    struct KzcBalancedTreeNode* removeNode;

    if (node->left != KZ_NULL && node->right != KZ_NULL)
    {
        /* Swap the node with either successor or predecessor. Alternate which one with a flip-flop variable.
           This avoids unbalacing the tree. */
        struct KzcBalancedTreeNode* swapNode;

        if (balancedTree->removeFlipFlop)
        {
            /* Find the next node. */
            swapNode = node->right;
            while (swapNode->left != KZ_NULL)
            {
                swapNode = swapNode->left;
            }
        }
        else
        {
            /* Find the previous node. */
            swapNode = node->left;
            while (swapNode->right != KZ_NULL)
            {
                swapNode = swapNode->right;
            }
        }

        /* Swap the element data of 'node' and 'swapNode'. */
        (*balancedTree->swapHandler)(node, swapNode);

        removeNode = swapNode;

        balancedTree->removeFlipFlop = !balancedTree->removeFlipFlop;
    }
    else
    {
        removeNode = node;
    }

    nodeHolder = kzcBalancedTreeGetNodeHolder_internal(balancedTree, removeNode);

    kzsAssert(nodeHolder != KZ_NULL);

    /* At this point the node has at most one child. */
    kzsAssert(removeNode->left == KZ_NULL || removeNode->right == KZ_NULL);

    /* Remove the node. */
    if (removeNode->left != KZ_NULL)
    {
        removeNode->left->parent = removeNode->parent;
        *nodeHolder = removeNode->left;
    }
    else if (removeNode->right != KZ_NULL)
    {
        removeNode->right->parent = removeNode->parent;
        *nodeHolder = removeNode->right;
    }
    else
    {
        *nodeHolder = KZ_NULL;
    }

    return removeNode;
}

kzsError kzcBalancedTreeRemove(struct KzcBalancedTree* balancedTree, const void* element)
{
    kzsError result;

    struct KzcBalancedTreeNode* removeNode;
    struct KzcBalancedTreeNode* node = kzcBalancedTreeFindNode_internal(balancedTree, KZC_FIND_OPERATION_EQUALS, element);
    kzsErrorTest(node != KZ_NULL, KZC_ERROR_ELEMENT_NOT_FOUND, "Element not found from the tree");

    removeNode = kzcBalancedTreeRemoveNode(balancedTree, node);

    result = kzcMemoryFreeVariable(removeNode);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzcBalancedTreeGetSize(const struct KzcBalancedTree* balancedTree)
{
    return balancedTree->elementCount;
}

struct KzcBalancedTreeNode* kzcBalancedTreeGetFirstNode(const struct KzcBalancedTree* balancedTree)
{
    struct KzcBalancedTreeNode* node = balancedTree->root;
    while (node->left != KZ_NULL)
    {
        node = node->left;
    }

    return node;
}

void* kzcBalancedTreeGetFirst(const struct KzcBalancedTree* balancedTree)
{
    struct KzcBalancedTreeNode* node = kzcBalancedTreeGetFirstNode(balancedTree);
    return node == KZ_NULL ? KZ_NULL : node->element;
}

struct KzcBalancedTreeNode* kzcBalancedTreeGetLastNode(const struct KzcBalancedTree* balancedTree)
{
    struct KzcBalancedTreeNode* node = balancedTree->root;
    while (node->right != KZ_NULL)
    {
        node = node->right;
    }

    return node;
}

void* kzcBalancedTreeGetLast(const struct KzcBalancedTree* balancedTree)
{
    struct KzcBalancedTreeNode* node = kzcBalancedTreeGetLastNode(balancedTree);
    return node == KZ_NULL ? KZ_NULL : node->element;
}


struct KzcBalancedTreeIterator kzcBalancedTreeGetIterator(const struct KzcBalancedTree* balancedTree)
{
    struct KzcBalancedTreeIterator iterator;
    struct KzcBalancedTreeNode* next = balancedTree->root;
    while (next->left != KZ_NULL)
    {
        next = next->left;
    }
    iterator.tree_private = balancedTree;
    iterator.next_private = next;
    iterator.node_private = KZ_NULL;
    return iterator;
}

static void kzcBalancedTreeIteratorAdvance_internal(struct KzcBalancedTreeIterator* iterator)
{
    const struct KzcBalancedTreeNode* node = iterator->next_private;
    const struct KzcBalancedTreeNode* next;

    iterator->node_private = node;

    if (node->right == KZ_NULL)
    {
        /* Go up until node == parent.left */
        next = node->parent;
        while (next != KZ_NULL && next->right == node)
        {
            node = next;
            next = node->parent;
        }
    }
    else
    {
        /* Go to the leftmost child of node */
        next = node->right;
        while (next->left != KZ_NULL)
        {
            next = next->left;
        }
    }

    iterator->next_private = next;
}

kzBool kzcBalancedTreeIterate_private(struct KzcBalancedTreeIterator* iterator)
{
    kzcBalancedTreeIteratorAdvance_internal(iterator);
    return iterator->next_private != KZ_NULL;
}

void* kzcBalancedTreeIteratorGetValue_private(const struct KzcBalancedTreeIterator* iterator)
{
    return iterator->node_private->element;
}


kzsError kzcBalancedTreeClear(struct KzcBalancedTree* balancedTree)
{
    kzsError result;

    struct KzcBalancedTreeNode* node = balancedTree->root;

    while (node != KZ_NULL)
    {
        struct KzcBalancedTreeNode* parent;

        while (node->left != KZ_NULL)
        {
            node = node->left;
        }

        while (node->right != KZ_NULL)
        {
            node = node->right;
        }

        parent = node->parent;

        if (parent != KZ_NULL)
        {
            if (parent->left == node)
            {
                parent->left = KZ_NULL;
            }
            if (parent->right == node)
            {
                parent->right = KZ_NULL;
            }
        }

        result = kzcMemoryFreeVariable(node);
        kzsErrorForward(result);

        node = parent;
    }

    balancedTree->root = KZ_NULL;

    kzsSuccess();
}


struct KzcBalancedTreeNode* kzcBalancedTreeGetEqualNode(const struct KzcBalancedTree* balancedTree, const void* element)
{
    return kzcBalancedTreeFindNode_internal(balancedTree, KZC_FIND_OPERATION_EQUALS, element);
}

struct KzcBalancedTreeNode* kzcBalancedTreeGetLowerNode(const struct KzcBalancedTree* balancedTree, const void* element)
{
    return kzcBalancedTreeFindNode_internal(balancedTree, KZC_FIND_OPERATION_LOWER, element);
}

struct KzcBalancedTreeNode* kzcBalancedTreeGetLowerOrEqualNode(const struct KzcBalancedTree* balancedTree, const void* element)
{
    return kzcBalancedTreeFindNode_internal(balancedTree, KZC_FIND_OPERATION_LOWER_OR_EQUALS, element);
}

struct KzcBalancedTreeNode* kzcBalancedTreeGetHigherNode(const struct KzcBalancedTree* balancedTree, const void* element)
{
    return kzcBalancedTreeFindNode_internal(balancedTree, KZC_FIND_OPERATION_HIGHER, element);
}

struct KzcBalancedTreeNode* kzcBalancedTreeGetHigherOrEqualNode(const struct KzcBalancedTree* balancedTree, const void* element)
{
    return kzcBalancedTreeFindNode_internal(balancedTree, KZC_FIND_OPERATION_HIGHER_OR_EQUALS, element);
}

void* kzcBalancedTreeGetLower(const struct KzcBalancedTree* balancedTree, const void* element)
{
    struct KzcBalancedTreeNode* node = kzcBalancedTreeGetLowerNode(balancedTree, element);
    return node != KZ_NULL ? node->element : KZ_NULL;
}

void* kzcBalancedTreeGetLowerOrEqual(const struct KzcBalancedTree* balancedTree, const void* element)
{
    struct KzcBalancedTreeNode* node = kzcBalancedTreeGetLowerOrEqualNode(balancedTree, element);
    return node != KZ_NULL ? node->element : KZ_NULL;
}

void* kzcBalancedTreeGetHigher(const struct KzcBalancedTree* balancedTree, const void* element)
{
    struct KzcBalancedTreeNode* node = kzcBalancedTreeGetHigherNode(balancedTree, element);
    return node != KZ_NULL ? node->element : KZ_NULL;
}

void* kzcBalancedTreeGetHigherOrEqual(const struct KzcBalancedTree* balancedTree, const void* element)
{
    struct KzcBalancedTreeNode* node = kzcBalancedTreeGetHigherOrEqualNode(balancedTree, element);
    return node != KZ_NULL ? node->element : KZ_NULL;
}
