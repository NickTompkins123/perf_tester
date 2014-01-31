/**
* \file
* Quick memory manager.
* Quick memory manager allocates memory from constant sized preallocated memory chunk.
* This manager type is optimized for speed and is intended for real-time memory allocation.
* It does not support memory deallocating of individual pointers, but instead the whole
* chunk of memory must be deallocated at once.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_MEMORY_QUICK_H
#define KZC_MEMORY_QUICK_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcQuickMemoryManager;


/**
 * Creates a new memory manager, which allocates the memory linearly from a pre-allocated chunk of memory.
 * This type of manager has no support for memory freeing of individual pointers, but is able to
 * reset the memory chunk by discarding all allocations simultaneously.
 */
kzsError kzcMemoryManagerCreateQuickManager(const struct KzcMemoryManager* parentManager, kzUint size,
                                         struct KzcMemoryManager** out_memoryManager);

/** Checks whether the given memory manager is quick memory manager or not. */
kzBool kzcMemoryIsQuickManager(const struct KzcMemoryManager* memoryManager);

/**
 * Frees every pointer allocated with the given memory manager.
 * The given manager must be a quick memory manager.
 */
kzsError kzcMemoryManagerResetQuickManager(const struct KzcMemoryManager* memoryManager);


#endif
