/**
* \file
* Memory manager pool.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_memory_pool.h"

#include "kzc_memory_private.h"
#include "kzc_memory_manager.h"
#include "kzc_memory_system.h"
#include "kzc_byte_buffer.h"

#include <core/util/collection/kzc_balanced_tree_node_manager.h>
#include <core/util/collection/kzc_balanced_tree.h>
#include <core/util/string/kzc_string.h>
#include <core/util/string/kzc_string_buffer.h>
#include <core/kzc_error_codes.h>

#include <system/wrappers/kzs_math.h>
#include <system/debug/kzs_log.h>
#include <system/kzs_error_codes.h>
#include <system/kzs_system.h>

/* TODO: Track and report memory fragmentation index */

/** If defined, this flag enables internal debug prints. */
#define KZC_MEMORY_POOL_DEBUG_INTERNAL 0

#if KZC_MEMORY_POOL_DEBUG_INTERNAL == 1
/* TODO: Remove printf function calls when system logging has been improved */
#include <stdio.h>
#endif

#ifdef KZC_MEMORY_DEBUG
/** This variable gives unique allocation id for each allocation. It can be tracked with KZC_MEMORY_DEBUG_BREAK_ON_ALLOCATION macro. */
/*lint -e{956} This is only for debugging purposes. */
static kzUint allocationId = 1;
/** Set the value of this macro to any positive number to break the debugger when the given allocation is allocated. Set to 0 to disable. */
#define KZC_MEMORY_DEBUG_BREAK_ON_ALLOCATION 0
#endif

/**
 * Descriptor structure for a chunk of memory.
 * A block can represent either allocated or unallocated chunk of memory.
 */
struct KzcMemoryPoolBlock
{
    struct KzcMemoryPoolBlock* previousBlock; /**< Pointer to previous block. */
    struct KzcMemoryPoolBlock* nextBlock; /**< Pointer to next block. */
    kzUint offset;  /**< Offset of this block in the memory pool data. TODO: Not really needed, could use block pointer instead. */
    kzUint size; /**< Size of this memory block (in bytes). */
    struct KzcBalancedTreeNode* treeNode; /**< Pointer to available tree node if this block is unallocated, NULL otherwise. */
#ifdef KZC_MEMORY_DEBUG
    kzString debugDescription; /**< Debug description of an allocated memory block. */
    kzUint debugAllocationId; /**< Debug allocation id of the block. */
#endif
};

struct KzcMemoryPool
{
    kzUint poolIndex; /**< Index of this pool in the containing memory manager. */
    kzUint dataSize; /**< Size of the pool data. */
    struct KzcBalancedTree* availabilityTree; /**< Available memory chunks ordered by size. */
    struct KzcMemoryPoolBlock* lastBlock; /**< Pointer to the last memory block. */
    kzByte* data; /**< Pool data. */
    kzUint treeNodeCount; /**< Number of tree nodes in use. */
    kzUint reservedTreeNodeCount; /**< Number of reserved tree nodes. */
    kzUint minimumEmptySize; /**< Minimum size of a continuous available memory block. Blocks smaller than this will be merged to the previous block. */
};


static const kzUint NODE_SIZE = sizeof(struct KzcBalancedTreeNode); /**< Size of a single availability tree node. */
static const kzUint MINIMUM_EXTRA_NODE_COUNT = 10; /**< At least this many additional availability tree nodes must always fit in the pool. */
static const kzUint INITIAL_NODE_COUNT = 16; /**< Initial number of reserved availability tree nodes. Must be greater than MINIMUM_EXTRA_NODE_COUNT. */


/**
 * Comparator function which orders instances of KzcMemoryPoolBlock primarily by their size (smallest first) and
 * secondarily by their offset (smallest first).
 */
static kzInt kzcAvailabilityTreeComparator(const void* first, const void* second)
{
    struct KzcMemoryPoolBlock* firstBlock = (struct KzcMemoryPoolBlock*)first;
    struct KzcMemoryPoolBlock* secondBlock = (struct KzcMemoryPoolBlock*)second;

    kzInt difference;
    if (firstBlock->size == secondBlock->size)
    {
        difference = (kzInt)(firstBlock->offset - secondBlock->offset);
    }
    else
    {
        difference = (kzInt)(firstBlock->size - secondBlock->size);
    }

    return difference;
}

/**
 * Swap function for balanced tree.
 * In addition to swapping the element data this also updates the backreferences (KzcMemoryPoolBlock->treeNode) accordingly.
 */
static void kzcMemoryPoolSwapHandler(struct KzcBalancedTreeNode* first, struct KzcBalancedTreeNode* second)
{
    struct KzcMemoryPoolBlock* firstBlock = (struct KzcMemoryPoolBlock*)first->element;
    struct KzcMemoryPoolBlock* secondBlock = (struct KzcMemoryPoolBlock*)second->element;

    first->element = secondBlock;
    secondBlock->treeNode = first;

    second->element = firstBlock;
    firstBlock->treeNode = second;
}


#if KZC_MEMORY_POOL_DEBUG_INTERNAL == 1

static void kzcMemoryTreePrintSubTree_internal(const struct KzcBalancedTreeNode* node)
{
    if (node != KZ_NULL)
    {
        const struct KzcBalancedTreeNode* left = node->left;
        const struct KzcBalancedTreeNode* right = node->right;

        kzsAssert(((struct KzcMemoryPoolBlock*)node->element)->treeNode == node);

        printf("\t           %u-%u\t[", ((struct KzcMemoryPoolBlock*)node->element)->offset, ((struct KzcMemoryPoolBlock*)node->element)->offset + ((struct KzcMemoryPoolBlock*)node->element)->size);

        if (left == KZ_NULL)
        {
            printf("-");
        }
        else
        {
            printf("%u", ((struct KzcMemoryPoolBlock*)left->element)->offset);
        }

        printf(",");

        if (right == KZ_NULL)
        {
            printf("-");
        }
        else
        {
            printf("%u", ((struct KzcMemoryPoolBlock*)right->element)->offset);
        }

        printf("]\n");

        kzcMemoryTreePrintSubTree_internal(left);
        kzcMemoryTreePrintSubTree_internal(right);
    }
}

static void kzcMemoryTreePrint_internal(const struct KzcMemoryPool* pool)
{
    kzsAssert(pool->lastBlock->treeNode != KZ_NULL);

    printf("Allocation tree:\n");
    kzcMemoryTreePrintSubTree_internal(kzcBalancedTreeGetRoot(pool->availabilityTree));
}

static void kzcMemoryListPrintNode(const struct KzcMemoryPoolBlock* block)
{
    if (block != KZ_NULL)
    {
        if (block->previousBlock != KZ_NULL)
        {
            kzcMemoryListPrintNode(block->previousBlock);
            printf(", ");
        }
        if (block->treeNode == KZ_NULL)
        {
            printf("[%u-%u]", block->offset, block->offset + block->size);
        }
        else
        {
            printf("<%u-%u>", block->offset, block->offset + block->size);
        }
    }
}

static void kzcMemoryListPrint(const struct KzcMemoryPool* pool)
{
    printf("Memory block list: ");
    kzcMemoryListPrintNode(pool->lastBlock);
    printf("\n");
}

static void kzcMemoryPrint(const struct KzcMemoryPool* pool)
{
    printf("------BEGIN--------\n");
    kzcMemoryTreePrint_internal(pool);
    kzcMemoryListPrint(pool);
    printf("-------END---------\n\n");
}
#endif


/** Initializes a memory pool block with the given parameters. */
static void kzcMemoryPoolBlockInitialize_internal(struct KzcMemoryPoolBlock* block, struct KzcMemoryPoolBlock* previousBlock, struct KzcMemoryPoolBlock* nextBlock,
                                                  kzUint offset, kzUint size, struct KzcBalancedTreeNode* treeNode)
{
    block->previousBlock = previousBlock;
    block->nextBlock = nextBlock;
    block->offset = offset;
    block->size = size;
    block->treeNode = treeNode;
}

/** Updates the size of an availability tree node. */
static void kzcUpdateNodeSize_internal(struct KzcBalancedTree* tree, struct KzcBalancedTreeNode* node, kzUint newSize)
{
    kzsError result;
    kzUint oldSize;
    struct KzcBalancedTreeNode* testNode;
    struct KzcBalancedTreeNode* removedNode;
    struct KzcMemoryPoolBlock* block = (struct KzcMemoryPoolBlock*)node->element;
    struct KzcMemoryPoolBlock* removedBlock;

    kzsAssert(block->treeNode == node);

    oldSize = block->size;
    block->size = newSize;

    /* Check if the node will be found with the new size. If yes, then it doesn't need to be moved. */
    testNode = kzcBalancedTreeGetEqualNode(tree, block);
    if (testNode != node)
    {
        block->size = oldSize;
        /* Remove the node from the tree. */
        removedNode = kzcBalancedTreeRemoveNode(tree, node);
        kzsAssert(removedNode != KZ_NULL);

        /* Update tree node pointers in case the elements were swapped. */
        ((struct KzcMemoryPoolBlock*)removedNode->element)->treeNode = removedNode;
        ((struct KzcMemoryPoolBlock*)node->element)->treeNode = node;

        /* Update the size of the block pointed by the node. */
        removedBlock = (struct KzcMemoryPoolBlock*)removedNode->element;
        removedBlock->size = newSize;

        /* Re-initialize necessary fields of the node. */
        removedNode->left = KZ_NULL;
        removedNode->right = KZ_NULL;

        /* Re-add the node back to the tree. */
        result = kzcBalancedTreeAddNode(tree, removedNode);
        kzsAssert(result == KZS_SUCCESS);
    }
}

/**
 * Removes a given node from the availability tree.
 * Special handling is required here, since only the last node can be deleted
 * without breaking the linear allocation strategy of the tree nodes.
 */
static void kzcMemoryPoolRemoveTreeNode_internal(struct KzcMemoryPool* pool, struct KzcBalancedTreeNode* node)
{
    struct KzcBalancedTreeNode* removedNode;
    struct KzcByteWriteBuffer poolBuffer;
    struct KzcBalancedTreeNode* lastNode;

    /* First remove the node. */
    removedNode = kzcBalancedTreeRemoveNode(pool->availabilityTree, node);
    kzsAssert(removedNode != KZ_NULL);

    /* Get the last node. This will be removed instead of removeNode. */
    kzcByteBufferInitializeReversedWrite(&poolBuffer, pool->data + pool->dataSize, pool->treeNodeCount * NODE_SIZE);

    kzcByteBufferSkip(&poolBuffer, (pool->treeNodeCount - 1) * NODE_SIZE);
    kzcByteBufferAllocateWriteVariable(&poolBuffer, lastNode);

    kzsAssert(kzcByteBufferIsFinished(&poolBuffer));

    if (removedNode != lastNode)
    {
        /* Copy node data */
        removedNode->parent = lastNode->parent;
        removedNode->left = lastNode->left;
        removedNode->right= lastNode->right;
        removedNode->element= lastNode->element;

        /* Fix backreferences */
        if (removedNode->parent == KZ_NULL)
        {
            kzsAssert(kzcBalancedTreeGetRoot(pool->availabilityTree) == lastNode);
            kzcBalancedTreeSetRoot(pool->availabilityTree, removedNode);
        }
        else if (removedNode->parent->left == lastNode)
        {
            removedNode->parent->left = removedNode;
        }
        else if (removedNode->parent->right == lastNode)
        {
            removedNode->parent->right = removedNode;
        }
        else
        {
            kzsAssert(KZ_FALSE);
        }

        if (removedNode->left != KZ_NULL)
        {
            removedNode->left->parent = removedNode;
        }
        if (removedNode->right != KZ_NULL)
        {
            removedNode->right->parent = removedNode;
        }

        ((struct KzcMemoryPoolBlock*)removedNode->element)->treeNode = removedNode;
    }

    --pool->treeNodeCount;

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
    kzcMemoryFillUnallocated((kzByte*)lastNode, sizeof(*lastNode));
#endif
}

/** Allocates an availability tree node from the end of the pool data. */
static kzsError kzcMemoryPoolAllocateTreeNode_internal(struct KzcMemoryPool* pool, struct KzcMemoryPoolBlock* block, struct KzcBalancedTreeNode** out_node)
{
    struct KzcBalancedTreeNode* node;
    struct KzcByteWriteBuffer poolBuffer;
    kzUint treeNodeIndex;
    
    kzsAssert(pool->treeNodeCount + MINIMUM_EXTRA_NODE_COUNT <= pool->reservedTreeNodeCount);

    /* If the number of nodes approaches too close to the number of reserved nodes, reserve more. */
    if (pool->treeNodeCount + MINIMUM_EXTRA_NODE_COUNT == pool->reservedTreeNodeCount)
    {
        kzUint numAddedNodes = pool->reservedTreeNodeCount;
        kzUint remainingSize;
        struct KzcMemoryPoolBlock* lastBlock = pool->lastBlock;

        kzsAssert(lastBlock->treeNode != KZ_NULL);

        /* Ensure that the pool has enough space for the new nodes. */
        kzsErrorTest(lastBlock->size >= NODE_SIZE * numAddedNodes,
                     KZS_ERROR_OUT_OF_MEMORY, "Not enough space for bookkeeping of memory pool reserved space.");


        remainingSize = lastBlock->size - NODE_SIZE * numAddedNodes;
        kzcUpdateNodeSize_internal(pool->availabilityTree, lastBlock->treeNode, remainingSize);

        pool->reservedTreeNodeCount += numAddedNodes;

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
        kzcMemoryFillUnallocated(pool->data + pool->dataSize - pool->reservedTreeNodeCount * NODE_SIZE, numAddedNodes * NODE_SIZE);
#endif
    }

    treeNodeIndex = pool->treeNodeCount++;

    kzcByteBufferInitializeReversedWrite(&poolBuffer, pool->data + pool->dataSize, pool->treeNodeCount * NODE_SIZE);

    kzcByteBufferSkip(&poolBuffer, treeNodeIndex * NODE_SIZE);
    kzcByteBufferAllocateWriteVariable(&poolBuffer, node);

    kzsAssert(kzcByteBufferIsFinished(&poolBuffer));

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
    kzsAssert(kzcMemoryCheckUnallocated((kzByte*)node, sizeof(*node)));
#endif

    node->left = KZ_NULL;
    node->right = KZ_NULL;
    node->element = block;

    *out_node = node;
    kzsSuccess();
}

kzsError kzcMemoryManagerCreatePool(const struct KzcMemoryManager* parentManager, kzUint poolIndex, kzUint poolSize,
                                    kzUint minimumEmptySize, struct KzcMemoryPool** out_pool)
{
    kzsError result;
    struct KzcMemoryPool* pool;
    kzUint dataSize = poolSize - sizeof(*pool);
    kzByte* data;
    struct KzcBalancedTree* availabilityTree;
    struct KzcMemoryPoolBlock* initialBlock;
    struct KzcBalancedTreeNode* initialNode;
    struct KzcByteWriteBuffer poolBuffer;

    /* Requirements */
    kzsAssert(INITIAL_NODE_COUNT > MINIMUM_EXTRA_NODE_COUNT);
    kzsErrorTest(poolSize > sizeof(*pool), KZC_ERROR_MEMORY_INVALID_POOL_SIZE, "Memory manager pool size is too small.");

    /* Allocate memory for the whole pool from the parent manager. */
    result = kzcMemoryAllocPointer(parentManager, &pool, poolSize, "Memory pool");
    kzsErrorForward(result);

    /* Data follows after the pool header. */
    data = (kzByte*)pool + sizeof(*pool);

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
    kzcMemoryFillUnallocated(data, dataSize);
#endif

    /* Create the availability tree. */
    result = kzcBalancedTreeCreateManaged(parentManager, kzcAvailabilityTreeComparator, kzcMemoryPoolSwapHandler, &availabilityTree);
    kzsErrorForward(result);


    /* Create byte buffer for the data. */
    kzcByteBufferInitializeWrite(&poolBuffer, data, sizeof(*initialBlock));

    /* Allocate initial empty block from the data. */
    kzcByteBufferAllocateWriteVariable(&poolBuffer, initialBlock);

    kzsAssert(kzcByteBufferIsFinished(&poolBuffer));


    /* Initialize the initial empty block with the remaining size of the data minus reserved size for the availability tree nodes. */
    kzsErrorTest(dataSize > NODE_SIZE * INITIAL_NODE_COUNT, KZC_ERROR_MEMORY_INVALID_POOL_SIZE, "Memory manager pool size is too small to hold initial bookkeeping.");
    kzcMemoryPoolBlockInitialize_internal(initialBlock, KZ_NULL, KZ_NULL, 0, dataSize - NODE_SIZE * INITIAL_NODE_COUNT, KZ_NULL);


    /* Initialize the memory pool. */
    pool->poolIndex = poolIndex;
    pool->dataSize = dataSize;
    pool->availabilityTree = availabilityTree;
    pool->lastBlock = initialBlock;
    pool->data = data;
    pool->treeNodeCount = 0;
    pool->reservedTreeNodeCount = INITIAL_NODE_COUNT;
    pool->minimumEmptySize = minimumEmptySize + sizeof(struct KzcMemoryPoolBlock);


    /* Allocate initial node for the availability tree pointing to the initial empty block. */
    result = kzcMemoryPoolAllocateTreeNode_internal(pool, initialBlock, &initialNode);
    kzsErrorForward(result);

    initialBlock->treeNode = initialNode;

    /* Add the initial node to the tree. */
    result = kzcBalancedTreeAddNode(availabilityTree, initialNode);
    kzsErrorForward(result);


#if KZC_MEMORY_POOL_DEBUG_INTERNAL == 1
    kzcMemoryPrint(pool);
#endif

    *out_pool = pool;
    kzsSuccess();
}

kzsError kzcMemoryManagerDeletePool(struct KzcMemoryPool* pool)
{
    kzsError result;
    struct KzcBalancedTreeNode* removeNode;

    kzsAssert(pool->lastBlock->treeNode != KZ_NULL);

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
    {
        struct KzcMemoryPoolBlock* block = pool->lastBlock;
        while (block != KZ_NULL)
        {
            if (block->treeNode != KZ_NULL)
            {
                kzsAssert(kzcMemoryCheckUnallocated((kzByte*)block + sizeof(*block), block->size - sizeof(*block)));
            }
            block = block->previousBlock;
        }
    }
#endif

    /* Check if the memory pool contains any allocated memory. */
    if (pool->lastBlock->previousBlock != KZ_NULL)
    {
        if (kzsSystemIsMemoryLeakReportEnabled())
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Some memory pool memory was not freed properly:");
#ifdef KZC_MEMORY_DEBUG
            kzcMemoryPoolDump(pool);
            kzsLog(KZS_LOG_LEVEL_INFO, "Use KZC_MEMORY_DEBUG_BREAK_ON_ALLOCATION in " __FILE__ " to track down the allocation point.");
#endif
        }
    }
    else
    {
        /* The return value will be discarded. */
        removeNode = kzcBalancedTreeRemoveNode(pool->availabilityTree, pool->lastBlock->treeNode);
        kzsAssert(removeNode != KZ_NULL);
        kzsAssert(pool->treeNodeCount == 1);
    }

    pool->lastBlock = KZ_NULL;
    pool->treeNodeCount = 0;

    result = kzcBalancedTreeDeleteManaged(pool->availabilityTree);
    kzsErrorForward(result);

    result = kzcMemoryFreePointer(pool);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcMemoryPoolAlloc(struct KzcMemoryPool* pool, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsError result;
    void* pointer;
    struct KzcMemoryPoolBlock searchBlock;
    struct KzcBalancedTreeNode* foundNode;
    struct KzcMemoryPoolBlock* foundBlock;

    /* Find the smallest available block with at least given size. */
    searchBlock.size = size + sizeof(*foundBlock);
    searchBlock.offset = 0;
    foundNode = kzcBalancedTreeGetHigherOrEqualNode(pool->availabilityTree, &searchBlock);

    /* If the found block was the last block and there is another suitable block available, use it instead. */
    /*if (foundNode != KZ_NULL && foundNode->element == pool->lastBlock && foundNode != kzcBalancedTreeGetLastNode(pool->availabilityTree))
    {
        foundNode = kzcBalancedTreeGetHigherNode(pool->availabilityTree, foundNode->element);
    }*/

    if (foundNode == KZ_NULL)
    {
        struct KzcMemoryManager* systemMemoryManager;
        kzMutableString errorMessage;

        result = kzcMemoryManagerCreateSystemManager(&systemMemoryManager);
        kzsErrorForward(result);

#ifdef KZC_MEMORY_DEBUG
        result = kzcStringFormat(systemMemoryManager, "Out of memory while trying to allocate %i bytes (%s)", &errorMessage, size, description);
        kzsErrorForward(result);
#else
        result = kzcStringFormat(systemMemoryManager, "Out of memory while trying to allocate %i bytes", &errorMessage, size);
        kzsErrorForward(result);
#endif

        kzsErrorThrow(KZS_ERROR_OUT_OF_MEMORY, errorMessage);
    }

    foundBlock = (struct KzcMemoryPoolBlock*)foundNode->element;

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
    kzsAssert(kzcMemoryCheckUnallocated((kzByte*)foundBlock + sizeof(*foundBlock), size));
#endif

    kzsAssert(foundBlock->treeNode == foundNode);

#if KZC_MEMORY_POOL_DEBUG_INTERNAL == 1
    printf("Found the following block for size %u: <%u-%u>\n", searchBlock.size, foundBlock->offset, foundBlock->offset + foundBlock->size);
#endif

    /* Check if the empty block is too small to be splitted into two blocks. */
    if (foundBlock->size < pool->minimumEmptySize + searchBlock.size)
    {
        /* Use the block completely. */
        struct KzcByteWriteBuffer foundBuffer;

            /* TODO: Proper error message. */
        /* Don't allow full allocation of the last block */
        kzsErrorTest(foundNode != pool->lastBlock->treeNode, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while trying to allocate %i bytes (%s)");

        kzcMemoryPoolRemoveTreeNode_internal(pool, foundNode);
        foundBlock->treeNode = KZ_NULL;

        /* Create a byte buffer to the memory of the found block. */
        kzcByteBufferInitializeWrite(&foundBuffer, foundBlock, foundBlock->size);

        /* Leave the current block alone */
        kzcByteBufferSkip(&foundBuffer, sizeof(*foundBlock));

        /* Allocate the actual memory pointer. */
        kzcByteBufferAllocateWritePointer(&foundBuffer, size, &pointer);
    }
    else
    {
        /* Split the block by creating a new empty space block after the found one. */
        struct KzcMemoryPoolBlock* newBlock;
        struct KzcByteWriteBuffer foundBuffer;

        /* Create a byte buffer to the memory of the found block. */
        kzcByteBufferInitializeWrite(&foundBuffer, foundBlock, foundBlock->size);

        /* Leave the current block alone */
        kzcByteBufferSkip(&foundBuffer, sizeof(*foundBlock));

        /* Allocate the actual memory pointer. */
        kzcByteBufferAllocateWritePointer(&foundBuffer, size, &pointer);

        /* Allocate the second block. */
        kzcByteBufferAllocateWriteVariable(&foundBuffer, newBlock);


        /* Found block is now ready to be used for allocated memory. */

        /* Fix references to the available block (foundBlock) to point to the new block. */
        foundNode->element = newBlock;

        if (foundBlock->nextBlock != KZ_NULL)
        {
            foundBlock->nextBlock->previousBlock = newBlock;
        }
        else
        {
            pool->lastBlock = newBlock;
        }


        /* Initialize the new block as available. */
        kzcMemoryPoolBlockInitialize_internal(newBlock, foundBlock, foundBlock->nextBlock, foundBlock->offset + searchBlock.size,
                                             foundBlock->size, foundNode); /* foundBlock->size is needed here. It will be replaced with the real lower value in the updateNodeSize call. */
        kzcUpdateNodeSize_internal(pool->availabilityTree, foundNode, newBlock->size - searchBlock.size);

        /* Re-initialize the found block as reserved. */
        kzcMemoryPoolBlockInitialize_internal(foundBlock, foundBlock->previousBlock, newBlock, foundBlock->offset,
                                             searchBlock.size, KZ_NULL);

        kzsAssert((kzByte*)foundBlock->nextBlock == (kzByte*)foundBlock + foundBlock->size);
    }

#ifdef KZC_MEMORY_DEBUG
    {
        foundBlock->debugDescription = description;
        foundBlock->debugAllocationId = allocationId++;
#ifdef KZC_MEMORY_DEBUG_BREAK_ON_ALLOCATION
        if (foundBlock->debugAllocationId == KZC_MEMORY_DEBUG_BREAK_ON_ALLOCATION)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Breaking for memory debug allocation.");
            kzsDebugBreak();
        }
#endif
    }
#endif

#if KZC_MEMORY_POOL_DEBUG_INTERNAL == 1
    kzcMemoryPrint(pool);
#endif

    *out_pointer = pointer;
    kzsSuccess();
}

/* Merges the two given blocks together. Either left or right block must be allocated and the other one unallocated. */
static void kzcMemoryPoolMergeTwo_internal(struct KzcMemoryPool* pool, struct KzcMemoryPoolBlock* left, const struct KzcMemoryPoolBlock* right)
{
    kzsAssert((left->treeNode != KZ_NULL) != (right->treeNode != KZ_NULL));

    /* Update references pointing to right node to point to the left node instead. */
    if (right->nextBlock != KZ_NULL)
    {
        right->nextBlock->previousBlock = left;
    }
    else
    {
        pool->lastBlock = left;
    }

    left->nextBlock = right->nextBlock;


    /* Check if left is allocated. */
    if (left->treeNode == KZ_NULL)
    {
        /* Move right node to left and grow it. */
        kzcUpdateNodeSize_internal(pool->availabilityTree, right->treeNode, left->size + right->size);
        left->treeNode = right->treeNode;
        left->size = right->size;
        left->treeNode->element = left;
#ifdef KZC_MEMORY_DEBUG
        left->debugDescription = KZ_NULL;
        left->debugAllocationId = 0;
#endif
    }
    else
    {
        kzsAssert(right->treeNode == KZ_NULL);

        /* Grow left node and discard right node. */
        kzcUpdateNodeSize_internal(pool->availabilityTree, left->treeNode, left->size + right->size);
    }

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
    kzcMemoryFillUnallocated((kzByte*)right, sizeof(*right));
#endif

    kzsAssert(left->nextBlock == KZ_NULL || (kzByte*)left->nextBlock == (kzByte*)left + left->size);
}

/* Merges the three given blocks together. Middle must be allocated whereas both left and right must be unallocated. */
static void kzcMemoryPoolMergeThree_internal(struct KzcMemoryPool* pool, struct KzcMemoryPoolBlock* left, const struct KzcMemoryPoolBlock* middle, const struct KzcMemoryPoolBlock* right)
{
    kzsAssert(left->treeNode != KZ_NULL && middle->treeNode == KZ_NULL && right->treeNode != KZ_NULL);

    /* Update references pointing to right node to point to the left node instead. */
    if (right->nextBlock != KZ_NULL)
    {
        right->nextBlock->previousBlock = left;
    }
    else
    {
        pool->lastBlock = left;
    }

    left->nextBlock = right->nextBlock;

    /* Grow left node, discard middle node and remove right node. */
    kzcUpdateNodeSize_internal(pool->availabilityTree, left->treeNode, left->size + middle->size + right->size);
    kzcMemoryPoolRemoveTreeNode_internal(pool, right->treeNode);

    kzsAssert(left->nextBlock == KZ_NULL || (kzByte*)left->nextBlock == (kzByte*)left + left->size);
#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
    kzcMemoryFillUnallocated((kzByte*)middle, sizeof(*middle));
    kzcMemoryFillUnallocated((kzByte*)right, sizeof(*right));
#endif
}

kzsError kzcMemoryPoolFree(struct KzcMemoryPool* pool, void* pointer)
{
    kzsError result;
    struct KzcMemoryPoolBlock* block;
    struct KzcByteWriteBuffer blockBuffer;
    struct KzcMemoryPoolBlock* previousBlock;
    struct KzcMemoryPoolBlock* nextBlock;
    kzBool previousFree;
    kzBool nextFree;

    /* Create a byte buffer for reading backwards in the memory to find the block of the given pointer. */
    kzcByteBufferInitializeReversedWrite(&blockBuffer, pointer, sizeof(*block));

    kzcByteBufferAllocateWriteVariable(&blockBuffer, block);

    kzsAssert(kzcByteBufferIsFinished(&blockBuffer));

    kzsErrorTest(block->treeNode == KZ_NULL, KZC_ERROR_MEMORY_ALLOCATION_MISMATCH, "Given pointer was already freed.");

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
    kzcMemoryFillUnallocated((kzByte*)block + sizeof(*block), block->size - sizeof(*block));
#endif

    /*
    Following table shows the different possible cases what the linked list can
    contain surrounding the pointer.
    Cases:
    1) [     NULL, pointer,      NULL]  -> create new empty node
    2) [     NULL, pointer, allocated]  -> create new empty node
    3) [     NULL, pointer,      free]  -> merge(1,2)
    4) [allocated, pointer,      NULL]  -> create new empty node
    5) [allocated, pointer, allocated]  -> create new empty node
    6) [allocated, pointer,      free]  -> merge(1,2)
    7) [     free, pointer,      NULL]  -> merge(0,1)
    8) [     free, pointer, allocated]  -> merge(0,1)
    9) [     free, pointer,      free]  -> merge(0,1,2)
    Cases 1==2==4==5, 3==6, 7==8

    Cases 1,4 and 7 should not be possible due to constraint that last block in the pool should always be available.
    */
    kzsAssert(block->nextBlock != KZ_NULL);

    previousBlock = block->previousBlock;
    nextBlock = block->nextBlock;
    previousFree = previousBlock != KZ_NULL && previousBlock->treeNode != KZ_NULL;
    nextFree = nextBlock != KZ_NULL && nextBlock->treeNode != KZ_NULL;

    if (previousFree && nextFree)
    {
        /* Case 9: Merge all three. */
        kzcMemoryPoolMergeThree_internal(pool, previousBlock, block, nextBlock);
    }
    else if (previousFree)
    {
        /* Cases (7),8: Merge previous and middle. */
        kzcMemoryPoolMergeTwo_internal(pool, previousBlock, block);
    }
    else if (nextFree)
    {
        /* Cases 3,6: Merge middle and next. */
        kzcMemoryPoolMergeTwo_internal(pool, block, nextBlock);
    }
    else
    {
        /* Cases (1),2,(4),5: Create a new empty node for middle. */
        if (block->size < pool->minimumEmptySize && block->previousBlock != KZ_NULL)
        {
            kzsAssert(block->nextBlock != KZ_NULL);

            block->previousBlock->size += block->size;
            block->nextBlock->previousBlock = block->previousBlock;
            block->previousBlock->nextBlock = block->nextBlock;
#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
            kzcMemoryFillUnallocated((kzByte*)block, block->size);
#endif
        }
        else
        {
            struct KzcBalancedTreeNode* newNode;

            result = kzcMemoryPoolAllocateTreeNode_internal(pool, block, &newNode);
            kzsErrorForward(result);

            block->treeNode = newNode;
#ifdef KZC_MEMORY_DEBUG
            block->debugDescription = KZ_NULL;
            block->debugAllocationId = 0;
#endif

            result = kzcBalancedTreeAddNode(pool->availabilityTree, newNode);
            kzsErrorForward(result);

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
            kzcMemoryFillUnallocated((kzByte*)block + sizeof(*block), block->size - sizeof(*block));
#endif
        }
    }

#if KZC_MEMORY_POOL_DEBUG_INTERNAL == 1
    kzcMemoryPrint(pool);
#endif

    kzsSuccess();
}

kzUint kzcMemoryPoolGetMaximumAvailableSize(const struct KzcMemoryPool* pool)
{
    struct KzcMemoryPoolBlock* block;
    struct KzcMemoryPoolBlock* nextBlock;
    kzUint result;

    block = (struct KzcMemoryPoolBlock*)kzcBalancedTreeGetLast(pool->availabilityTree);
    kzsAssert(block != KZ_NULL);
    result = block->size - pool->minimumEmptySize; /* Last block is special, since it cannot be completely allocated. */

    nextBlock = (struct KzcMemoryPoolBlock*)kzcBalancedTreeGetHigher(pool->availabilityTree, block);
    if (nextBlock != KZ_NULL)
    {
        result = kzsMaxU(result, nextBlock->size);
    }

    return result - sizeof(*block);
}

kzBool kzcMemoryPoolIsPointerInRange(const struct KzcMemoryPool* pool, const void* pointer)
{
    return ((kzByte*)pointer >= pool->data) &&
           ((kzByte*)pointer < pool->data + pool->dataSize);
}

/** Prints the memory usage as an ASCII-bar. */
static void kzcMemoryPoolDumpBlocks_internal(struct KzcStringBuffer* stringBuffer, struct KzcMemoryPoolBlock* firstBlock, kzUint characterSize)
{
    kzsError result;
    struct KzcMemoryPoolBlock* currentBlock = firstBlock;
    kzUint offsetLeftOver = 0;
    kzUint usedLeftOver = 0;

    while (currentBlock != KZ_NULL)
    {
        kzBool reserved = currentBlock->treeNode == KZ_NULL;
        kzUint delta = currentBlock->size;
        kzUint remainder = characterSize - offsetLeftOver;

        while (delta >= remainder)
        {
            kzChar ch;
            delta -= remainder;
            usedLeftOver += reserved ? remainder : 0;

            if (usedLeftOver == characterSize)
            {
                ch = '#';
            }
            else if (usedLeftOver == 0)
            {
                ch = ' ';
            }
            else
            {
                ch = '=';
            }
            result = kzcStringBufferAppendCharacter(stringBuffer, ch);
            KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */

            offsetLeftOver = 0;
            usedLeftOver = 0;
            remainder = characterSize;
        }
        offsetLeftOver += delta;
        usedLeftOver += reserved ? delta : 0;

        currentBlock = currentBlock->nextBlock;
    }

    if (usedLeftOver > 0) /* Output the remainder */
    {
        kzChar ch;
        if (usedLeftOver == characterSize)
        {
            ch = '#';
        }
        else
        {
            ch = '=';
        }
        result = kzcStringBufferAppendCharacter(stringBuffer, ch);
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */
    }
}

/** Prints the memory usage of the availability tree as an ASCII-bar. */
static void kzcMemoryPoolDumpTree_internal(struct KzcStringBuffer* stringBuffer, kzUint treeNodeCount, kzUint numAllocatedTreeNodes, kzUint treeNodeSize, kzUint characterSize)
{
    kzsError result;
    kzUint currentTreeNode = 0;
    kzUint offsetLeftOver = 0;
    kzUint usedLeftOver = 0;

    while (currentTreeNode < numAllocatedTreeNodes)
    {
        kzBool reserved = currentTreeNode >= numAllocatedTreeNodes - treeNodeCount;
        kzUint delta = treeNodeSize;
        kzUint remainder = characterSize - offsetLeftOver;

        while (delta >= remainder)
        {
            kzChar ch;
            delta -= remainder;
            usedLeftOver += reserved ? remainder : 0;

            if (usedLeftOver == characterSize)
            {
                ch = '#';
            }
            else if (usedLeftOver == 0)
            {
                ch = ' ';
            }
            else
            {
                ch = '=';
            }
            result = kzcStringBufferAppendCharacter(stringBuffer, ch);
            KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */

            offsetLeftOver = 0;
            usedLeftOver = 0;
            remainder = characterSize;
        }
        offsetLeftOver += delta;
        usedLeftOver += reserved ? delta : 0;

        ++currentTreeNode;
    }

    if (usedLeftOver > 0) /* Output the remainder */
    {
        kzChar ch;
        if (usedLeftOver == offsetLeftOver)
        {
            ch = '#';
        }
        else
        {
            ch = '=';
        }
        result = kzcStringBufferAppendCharacter(stringBuffer, ch);
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */
    }
}

void kzcMemoryPoolDump(const struct KzcMemoryPool* pool)
{
    kzsError result;
    struct KzcMemoryPoolBlock* firstBlock;

    kzsAssert(kzcIsValidPointer(pool));

    firstBlock = pool->lastBlock;;

    kzsLog(KZS_LOG_LEVEL_INFO, "Memory dump of pool %u:");

    while (firstBlock->previousBlock != KZ_NULL) 
    {
        kzsAssert((kzByte*)firstBlock == (kzByte*)firstBlock->previousBlock + firstBlock->previousBlock->size);
        firstBlock = firstBlock->previousBlock;
    }


    { /* Memory overview (ASCII-bar) */
        kzUint numCharacters = 80 - 10; /* 10 = | */
        kzUint characterSize = pool->dataSize / numCharacters;
        struct KzcStringBuffer* stringBuffer;

        result = kzcStringBufferCreateDefault(kzcMemoryGetManager(pool), &stringBuffer);
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */

        result = kzcStringBufferAppend(stringBuffer, "Usage: |");
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */

        kzcMemoryPoolDumpBlocks_internal(stringBuffer, firstBlock, characterSize);

        result = kzcStringBufferAppend(stringBuffer, "|");
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */

        kzcMemoryPoolDumpTree_internal(stringBuffer, pool->treeNodeCount, pool->reservedTreeNodeCount, sizeof(struct KzcBalancedTreeNode), characterSize);

        result = kzcStringBufferAppend(stringBuffer, "|");
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */
        kzsLog(KZS_LOG_LEVEL_INFO, kzcStringBufferGetString(stringBuffer));

        result = kzcStringBufferDelete(stringBuffer);
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */
    }

    { /* Details */
        struct KzcMemoryPoolBlock* currentBlock = firstBlock;
        struct KzcStringBuffer* stringBuffer;

        result = kzcStringBufferCreateDefault(kzcMemoryGetManager(pool), &stringBuffer);
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */

        result = kzcStringBufferAppend(stringBuffer, "Details: |");
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */

        while (currentBlock != KZ_NULL)
        {
            if (currentBlock->treeNode != KZ_NULL)
            {
                result = kzcStringBufferAppend(stringBuffer, "        ");
                KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */
            }
            else
            {
                result = kzcStringBufferAppendFormat(stringBuffer, "%08X", currentBlock->offset);
                KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */
            }

            result = kzcStringBufferAppendCharacter(stringBuffer, '|');
            KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */

            currentBlock = currentBlock->nextBlock;
        }
        result = kzcStringBufferAppendCharacter(stringBuffer, '\n');
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */

        currentBlock = firstBlock;
        while (currentBlock != KZ_NULL)
        {
            if (currentBlock->treeNode == KZ_NULL)
            {
                result = kzcStringBufferAppendFormat(stringBuffer, "%08X: %u bytes", currentBlock->offset, currentBlock->size);
                KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */
#ifdef KZC_MEMORY_DEBUG
                result = kzcStringBufferAppendFormat(stringBuffer, " (%u. %s)", currentBlock->debugAllocationId, currentBlock->debugDescription);
                KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */
#endif
                result = kzcStringBufferAppendCharacter(stringBuffer, '\n');
                KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */
            }

            currentBlock = currentBlock->nextBlock;
        }

        kzsLog(KZS_LOG_LEVEL_INFO, kzcStringBufferGetString(stringBuffer));

        result = kzcStringBufferDelete(stringBuffer);
        KZ_UNUSED_RETURN_VALUE(result); /* Ignore all errors as this is a debugging function. */
    }
}
