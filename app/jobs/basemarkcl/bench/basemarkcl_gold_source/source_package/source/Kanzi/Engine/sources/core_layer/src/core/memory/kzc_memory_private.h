/**
* \file
* Memory manager private members.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_MEMORY_PRIVATE_H
#define KZC_MEMORY_PRIVATE_H


#include "kzc_memory_base.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;


typedef kzsError (*KzcMemoryManagerDeletor)(struct KzcMemoryManager* memoryManager);
typedef kzsError (*KzcMemoryAllocator)(const struct KzcMemoryManager* memoryManager, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));
typedef kzsError (*KzcMemoryDeallocator)(const struct KzcMemoryManager* memoryManager, void* pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));


/** Memory manager. */
struct KzcMemoryManager
{
    enum KzcMemoryManagerType type; /**< Type of the memory manager. */
    KzcMemoryManagerDeletor deletor; /**< Memory manager deletor function. */
    KzcMemoryAllocator allocator; /**< Memory allocator function. */
    KzcMemoryDeallocator deallocator; /**< Memory deallocator function. */
};


#ifdef KZC_MEMORY_DEBUG
static const kzUint KZC_MEMORY_MANAGER_GUARD_PRE_SIZE = 4;
static const kzUint KZC_MEMORY_MANAGER_GUARD_POST_SIZE = 4;
#endif


/** Initializes a memory manager. */
void kzcMemoryManagerInitialize_private(struct KzcMemoryManager* manager, enum KzcMemoryManagerType type, KzcMemoryManagerDeletor deletor,
                                        KzcMemoryAllocator allocator, KzcMemoryDeallocator deallocator);

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
/** Fills the given pointer with guard data. */
void kzcMemoryFillUnallocated(kzByte* pointer, kzUint size);

/** Checks if the given pointer contains proper memory guards. */
kzBool kzcMemoryCheckUnallocated(const kzByte* pointer, kzUint size);
#endif


#endif
