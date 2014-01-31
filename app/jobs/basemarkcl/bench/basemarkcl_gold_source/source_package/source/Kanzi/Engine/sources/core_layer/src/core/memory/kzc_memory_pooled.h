/**
* \file
* Pooled memory manager.
* Pooled memory manager allocates memory from constant sized preallocated memory pools.
* This manager type tries to minimize memory fragmentation and is suitable for most common uses.
* It is not intended for real-time memory allocation however.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_MEMORY_POOLED_H
#define KZC_MEMORY_POOLED_H


#include "kzc_memory_base.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcPooledMemoryManager;


/** Creates a new memory manager, which allocates the memory from pre-allocated memory pools. */
kzsError kzcMemoryManagerCreatePooledManager(const struct KzcMemoryManager* parentManager, kzUint poolCount,
                                             kzUint poolSize, struct KzcMemoryManager** out_manager);

/** Dumps the memory content to log. This function is only available for pooled memory manager. */
kzsError kzcMemoryDump(const struct KzcMemoryManager* memoryManager);

/** Prints all active memory allocations and their descriptions allocated with the given memory manager. */
kzsError kzcMemoryPrintDebugAllocations(const struct KzcMemoryManager* memoryManager);

/** Gets maximum available block size from pooled memory manager. */
kzUint kzcMemoryPooledGetMaximumAvailableSize(const struct KzcMemoryManager* memoryManager);

#ifdef KZC_MEMORY_MEASURE
/** Gets peak (lowest among the lifetime) maximum available block size from pooled memory manager. */
kzUint kzcMemoryPooledGetPeakMaximumAvailableSize(const struct KzcMemoryManager* memoryManager);
#endif


#endif
