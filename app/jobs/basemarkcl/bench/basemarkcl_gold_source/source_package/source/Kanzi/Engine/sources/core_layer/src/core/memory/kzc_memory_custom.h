/**
* \file
* Custom memory manager.
* Custom memory manager provides a way to plug in arbitrary memory allocation mechanisms.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_MEMORY_CUSTOM_H
#define KZC_MEMORY_CUSTOM_H


#include "kzc_memory_base.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcCustomMemoryManager;


/** Memory allocation function type. Used for custom memory managers. */
typedef kzsError (*KzcMemoryAllocFunction)(void* customData, kzUint size, void** out_pointer);
/** Memory deallocation function type. Used for custom memory managers. */
typedef kzsError (*KzcMemoryFreeFunction)(void* customData, void* pointer);


/**
 * Creates a new memory manager, which allocates the memory with custom memory allocation functions.
 * \param customData Arbitrary custom data associated with the manager. This data is passed to allocator and deallocator functions.
 */
kzsError kzcMemoryManagerCreateCustomManager(KzcMemoryAllocFunction allocator, KzcMemoryFreeFunction deallocator,
                                             void* customData, struct KzcMemoryManager** out_memoryManager);


#endif
