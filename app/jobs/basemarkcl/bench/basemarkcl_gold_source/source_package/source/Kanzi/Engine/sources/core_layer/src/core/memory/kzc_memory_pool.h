/**
* \file
* Memory pool for pooled memory manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_MEMORY_POOL_H
#define KZC_MEMORY_POOL_H


#include "kzc_memory_base.h" /* Required for MEMORY_MANAGER_DEBUG_PARAM */

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;


/**
 * \struct KzcMemoryPool
 * Internal structure of a MemoryPool.
 */
struct KzcMemoryPool;


/** Creates a new memory pool. */
kzsError kzcMemoryManagerCreatePool(const struct KzcMemoryManager* parentManager, kzUint poolIndex,
                                    kzUint poolSize, kzUint minimumEmptySize, struct KzcMemoryPool** out_pool);
/** Deletes a memory pool. */
kzsError kzcMemoryManagerDeletePool(struct KzcMemoryPool* pool);

/** Allocates given amount of memory from the pool. */
kzsError kzcMemoryPoolAlloc(struct KzcMemoryPool* pool, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));
/** Frees the memory of the given pointer. */
kzsError kzcMemoryPoolFree(struct KzcMemoryPool* pool, void* pointer);

/** Returns the maximum continuous size of memory that can be allocated. */
kzUint kzcMemoryPoolGetMaximumAvailableSize(const struct KzcMemoryPool* pool);

/** Checks if the given pointer is located within the memory range of this pool. */
kzBool kzcMemoryPoolIsPointerInRange(const struct KzcMemoryPool* pool, const void* pointer);

/** Dumps the internal state of the memory pool to log. */
void kzcMemoryPoolDump(const struct KzcMemoryPool* pool);


#endif
