/**
* \file
* Custom memory manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/

#include "kzc_memory_custom.h"

#include "kzc_memory_private.h"

#include "kzc_memory_manager.h"
#include "kzc_byte_buffer.h"


/** Internal structure of a custom memory manager. */
struct KzcCustomMemoryManager
{
    struct KzcMemoryManager base; /**< Memory manager base. */
    KzcMemoryAllocFunction allocator; /**< Memory allocation function. */
    KzcMemoryFreeFunction deallocator; /**< Memory deallocation function. */
    void* customData; /**< Arbitrary custom data associated with the manager. */
};


static kzsError kzcMemoryCustomDelete_internal(struct KzcMemoryManager* memoryManager);
static kzsError kzcMemoryCustomAlloc_internal(const struct KzcMemoryManager* memoryManager, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));
static kzsError kzcMemoryCustomFree_internal(const struct KzcMemoryManager* memoryManager, void* pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));


kzsError kzcMemoryManagerCreateCustomManager(KzcMemoryAllocFunction allocator, KzcMemoryFreeFunction deallocator, void* customData, struct KzcMemoryManager** out_memoryManager)
{
    kzsError result;
    struct KzcCustomMemoryManager* customMemoryManager;

    /* Allocate memory for the manager and manager data. */
    result = (*allocator)(customData, sizeof(*customMemoryManager), (void**)&customMemoryManager);
    kzsErrorForward(result);

    customMemoryManager->allocator = allocator;
    customMemoryManager->deallocator = deallocator;
    customMemoryManager->customData = customData;

    kzcMemoryManagerInitialize_private(&customMemoryManager->base, KZC_MEMORY_MANAGER_TYPE_CUSTOM, kzcMemoryCustomDelete_internal, kzcMemoryCustomAlloc_internal, kzcMemoryCustomFree_internal);

    *out_memoryManager = &customMemoryManager->base;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemoryCustomDelete_internal(struct KzcMemoryManager* memoryManager)
{
    kzsError result;

    result = kzcMemoryFreePointer(memoryManager);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemoryCustomAlloc_internal(const struct KzcMemoryManager* memoryManager, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsError result;
    const struct KzcCustomMemoryManager* customMemoryManager = (const struct KzcCustomMemoryManager*)memoryManager;
    void* pointer;

    result = (*customMemoryManager->allocator)(customMemoryManager->customData, size, &pointer);
    kzsErrorForward(result);

    *out_pointer = pointer;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemoryCustomFree_internal(const struct KzcMemoryManager* memoryManager, void* pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsError result;
    const struct KzcCustomMemoryManager* customMemoryManager = (const struct KzcCustomMemoryManager*)memoryManager;

    result = (*customMemoryManager->deallocator)(customMemoryManager->customData, pointer);
    kzsErrorForward(result);

    kzsSuccess();
}
