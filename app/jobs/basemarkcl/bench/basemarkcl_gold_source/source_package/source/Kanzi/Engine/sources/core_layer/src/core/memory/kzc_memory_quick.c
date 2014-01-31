/**
* \file
* Quick memory manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/

#include "kzc_memory_quick.h"

#include "kzc_memory_private.h"

#include "kzc_memory_manager.h"
#include "kzc_byte_buffer.h"

#include <system/kzs_error_codes.h>


/** Internal structure of a quick memory manager. */
struct KzcQuickMemoryManager
{
    struct KzcMemoryManager memoryManager; /**< Memory manager base. */
    kzUint dataSize; /**< Size of the data */
    kzUint usageOffset; /**< Current offset for the beginning of the unallocated memory */
    void* data; /**< Memory data */
};


static kzsError kzcMemoryQuickDelete_internal(struct KzcMemoryManager* memoryManager);
static kzsError kzcMemoryQuickAlloc_internal(const struct KzcMemoryManager* memoryManager, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));
static kzsError kzcMemoryQuickFree_internal(const struct KzcMemoryManager* memoryManager, void* pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));

static void kzcMemoryQuickReset_internal(struct KzcQuickMemoryManager* quickMemoryManager);


kzsError kzcMemoryManagerCreateQuickManager(const struct KzcMemoryManager* parentManager, kzUint size, struct KzcMemoryManager** out_memoryManager)
{
    kzsError result;
    struct KzcQuickMemoryManager* quickMemoryManager;
    kzUint allocSize;
    void* managerPointer;
    struct KzcByteWriteBuffer managerBuffer;

    /* Allocate memory for the manager and manager data. */
    allocSize = sizeof(*quickMemoryManager);
    result = kzcMemoryAllocPointer(parentManager, &managerPointer, size, "Quick memory manager");
    kzsErrorForward(result);

    kzcByteBufferInitializeWrite(&managerBuffer, managerPointer, size);

    kzcByteBufferAllocateWriteVariable(&managerBuffer, quickMemoryManager);

    quickMemoryManager->dataSize = size - allocSize;

    kzcByteBufferAllocateWritePointer(&managerBuffer, quickMemoryManager->dataSize, &quickMemoryManager->data);

    kzsAssert(kzcByteBufferIsFinished(&managerBuffer));

    quickMemoryManager->usageOffset = quickMemoryManager->dataSize;
    kzcMemoryQuickReset_internal(quickMemoryManager);

    kzcMemoryManagerInitialize_private(&quickMemoryManager->memoryManager, KZC_MEMORY_MANAGER_TYPE_QUICK, kzcMemoryQuickDelete_internal, kzcMemoryQuickAlloc_internal, kzcMemoryQuickFree_internal);

    *out_memoryManager = &quickMemoryManager->memoryManager;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemoryQuickDelete_internal(struct KzcMemoryManager* memoryManager)
{
    kzsError result;

    result = kzcMemoryFreePointer(memoryManager);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemoryQuickAlloc_internal(const struct KzcMemoryManager* memoryManager, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    struct KzcQuickMemoryManager* quickMemoryManager = (struct KzcQuickMemoryManager*)memoryManager;
    void* pointer;

    /* TODO: Make the error message display how many bytes were requested. */
    kzsErrorTest(size <= quickMemoryManager->dataSize - quickMemoryManager->usageOffset, KZS_ERROR_OUT_OF_MEMORY, "Out of memory");

    pointer = (kzByte*)quickMemoryManager->data + quickMemoryManager->usageOffset;
    quickMemoryManager->usageOffset += size;

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
    kzsAssert(kzcMemoryCheckUnallocated(pointer, size));
#endif

    *out_pointer = pointer;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemoryQuickFree_internal(const struct KzcMemoryManager* memoryManager,
                                                        void* pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsSuccess();
}

static void kzcMemoryQuickReset_internal(struct KzcQuickMemoryManager* quickMemoryManager)
{
    kzsAssert(kzcIsValidPointer(quickMemoryManager));

#if defined KZC_MEMORY_DEBUG && defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
    kzcMemoryFillUnallocated(quickMemoryManager->data, quickMemoryManager->usageOffset);
#endif
    quickMemoryManager->usageOffset = 0;
}

kzBool kzcMemoryIsQuickManager(const struct KzcMemoryManager* memoryManager)
{
    return memoryManager->type == KZC_MEMORY_MANAGER_TYPE_QUICK;
}

kzsError kzcMemoryManagerResetQuickManager(const struct KzcMemoryManager* memoryManager)
{
    kzsErrorTest(memoryManager->type == KZC_MEMORY_MANAGER_TYPE_QUICK, KZS_ERROR_ILLEGAL_OPERATION,
                 "Tried to reset non-quick memory manager");

    kzcMemoryQuickReset_internal((struct KzcQuickMemoryManager*)memoryManager);

    kzsSuccess();
}
