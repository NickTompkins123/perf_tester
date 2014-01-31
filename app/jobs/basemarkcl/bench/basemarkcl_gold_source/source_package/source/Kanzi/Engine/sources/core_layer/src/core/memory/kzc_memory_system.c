/**
* \file
* System memory manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/

#include "kzc_memory_system.h"

#include "kzc_memory_private.h"

#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>


static kzsError kzcMemorySystemDelete_internal(struct KzcMemoryManager* memoryManager);
static kzsError kzcMemorySystemAlloc_internal(const struct KzcMemoryManager* memoryManager, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));
static kzsError kzcMemorySystemFree_internal(const struct KzcMemoryManager* memoryManager, void* pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));


kzsError kzcMemoryManagerCreateSystemManager(struct KzcMemoryManager** out_memoryManager)
{
    struct KzcMemoryManager* memoryManager;
    
    memoryManager = (struct KzcMemoryManager*)kzsMalloc(sizeof(*memoryManager));
    kzsErrorTest(memoryManager != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while trying to allocate memory for system memory manager.");

    kzcMemoryManagerInitialize_private(memoryManager, KZC_MEMORY_MANAGER_TYPE_SYSTEM, kzcMemorySystemDelete_internal, kzcMemorySystemAlloc_internal, kzcMemorySystemFree_internal);

    *out_memoryManager = memoryManager;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemorySystemDelete_internal(struct KzcMemoryManager* memoryManager)
{
    kzsFree(memoryManager);

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemorySystemAlloc_internal(const struct KzcMemoryManager* memoryManager, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    void* pointer;

    pointer = kzsMalloc(size);
    /* TODO: Make the error message display how many bytes were requested. */
    kzsErrorTest(pointer != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory");

    *out_pointer = pointer;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemorySystemFree_internal(const struct KzcMemoryManager* memoryManager, void* pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsFree(pointer);

    kzsSuccess();
}
