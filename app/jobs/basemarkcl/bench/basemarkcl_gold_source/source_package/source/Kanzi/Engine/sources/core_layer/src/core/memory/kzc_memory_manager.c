/**
* \file
* Core memory manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_memory_manager.h"

#include "kzc_memory_private.h"
#include "kzc_byte_buffer.h"

#include <system/debug/kzs_log.h>
#include <system/debug/kzs_counter.h>
#include <system/wrappers/kzs_math.h>


/* TODO: Make KZC_MEMORY_DEBUG to depend on KZ_DEBUG or similar instead of _DEBUG. */
#ifdef KZC_MEMORY_DEBUG
/** Gets the pointer description, which was given while allocating the pointer. */
static kzString kzcMemoryGetDebugDescription_internal(const void* pointer);

/** Calculates a memory guard value for the given pointer location. */
static kzByte kzcMemoryGetGuardByte_internal(const kzByte* pointer);

/** Checks if the given pointer location contains a proper memory guard value. */
static kzBool kzcMemoryCheckGuardByte_internal(const kzByte* pointer);

/** Checks that the given pointer contains valid guards. */
#ifndef kzcIsValidPointer
static kzBool kzcMemoryManagerCheckGuards_internal(const kzByte* pointer);
#endif
#endif

#define KZC_MEMORY_ALIGNMENT_SIZE sizeof(void*)


void kzcMemoryManagerInitialize_private(struct KzcMemoryManager* memoryManager, enum KzcMemoryManagerType type, KzcMemoryManagerDeletor deletor,
                                        KzcMemoryAllocator allocator, KzcMemoryDeallocator deallocator)
{
    memoryManager->type = type;
    memoryManager->deletor = deletor;
    memoryManager->allocator = allocator;
    memoryManager->deallocator = deallocator;
}

kzsError kzcMemoryManagerDelete(struct KzcMemoryManager* memoryManager)
{
    kzsError result;

    result = memoryManager->deletor(memoryManager);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Generic memory allocator, which forwards the allocation to the specified manager. */
static kzsError kzcMemoryAlloc_internal(const struct KzcMemoryManager* memoryManager, kzUint size,
                                        void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsError result;
    void* pointer;

    result = memoryManager->allocator(memoryManager, size, &pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(description));
    kzsErrorForward(result);

    kzsCounterIncrease("kzcMemoryAlloc_internal");

    *out_pointer = pointer;
    kzsSuccess();
}

kzsError kzcMemoryAllocPointer_private(const struct KzcMemoryManager* memoryManager, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsError result;
    kzByte* pointer;
    void* resultPointer;
    struct KzcByteWriteBuffer memoryBuffer;
    kzUint align = KZC_MEMORY_ALIGNMENT_SIZE - 1 - (size + KZC_MEMORY_ALIGNMENT_SIZE - 1) % KZC_MEMORY_ALIGNMENT_SIZE;
    kzUint alignedSize = size + align;
    kzUint memorySize = alignedSize + sizeof(struct KzcMemoryManager*);
#ifdef KZC_MEMORY_DEBUG
    memorySize += sizeof(description);
    memorySize += sizeof(memorySize);
    memorySize += KZC_MEMORY_MANAGER_GUARD_PRE_SIZE;
    memorySize += KZC_MEMORY_MANAGER_GUARD_POST_SIZE;
#endif

    result = kzcMemoryAlloc_internal(memoryManager, memorySize, (void**)&pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(description));
    kzsErrorForward(result);

    kzcByteBufferInitializeWrite(&memoryBuffer, pointer, memorySize);

    /* Store manager */
    kzcByteBufferWrite(&memoryBuffer, memoryManager);

#ifdef KZC_MEMORY_DEBUG
    /* Store description */
    kzcByteBufferWrite(&memoryBuffer, description);

    /* Store size */
    kzcByteBufferWrite(&memoryBuffer, size);

    {
        kzUint i;
        for (i = 0; i < KZC_MEMORY_MANAGER_GUARD_PRE_SIZE; ++i)
        {
            kzByte byte = kzcMemoryGetGuardByte_internal(pointer + kzcByteBufferGetOffset(&memoryBuffer));
            kzcByteBufferWrite(&memoryBuffer, byte);
        }
    }
#endif

    /* Actual data */
    kzcByteBufferAllocateWritePointer(&memoryBuffer, size, &resultPointer);

#ifdef KZC_MEMORY_DEBUG
    {
        kzUint i;
        for (i = 0; i < KZC_MEMORY_MANAGER_GUARD_POST_SIZE; ++i)
        {
            kzByte byte = kzcMemoryGetGuardByte_internal(pointer + kzcByteBufferGetOffset(&memoryBuffer));
            kzcByteBufferWrite(&memoryBuffer, byte);
        }
    }
#endif

    kzcByteBufferSkip(&memoryBuffer, align);

    kzsAssert(kzcByteBufferIsFinished(&memoryBuffer));

    /*kzsAssert(*(kzUint*)(void*)&resultPointer % KZC_MEMORY_ALIGNMENT_SIZE == 0);*/

    *out_pointer = resultPointer;
    kzsSuccess();
}

kzsError kzcMemoryAllocArray_private(const struct KzcMemoryManager* memoryManager, kzUint length, kzUint elementSize, void** out_array MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsError result;
    kzByte* pointer;
    void* array;
    struct KzcByteWriteBuffer memoryBuffer;
    kzUint size = length * elementSize;
    kzUint align = KZC_MEMORY_ALIGNMENT_SIZE - 1 - (size + KZC_MEMORY_ALIGNMENT_SIZE - 1) % KZC_MEMORY_ALIGNMENT_SIZE;
    kzUint alignedSize = size + align;
    kzUint memorySize = alignedSize + sizeof(struct KzcMemoryManager*) + sizeof(kzUint);
#ifdef KZC_MEMORY_DEBUG
    memorySize += sizeof(description);
    memorySize += sizeof(memorySize);
    memorySize += KZC_MEMORY_MANAGER_GUARD_PRE_SIZE;
    memorySize += KZC_MEMORY_MANAGER_GUARD_POST_SIZE;
#endif

    result = kzcMemoryAlloc_internal(memoryManager, memorySize, (void**)&pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(description));
    kzsErrorForward(result);

    kzcByteBufferInitializeWrite(&memoryBuffer, pointer, memorySize);

    /* Store size */
    kzcByteBufferWrite(&memoryBuffer, length);

    /* Store manager */
    kzcByteBufferWrite(&memoryBuffer, memoryManager);

#ifdef KZC_MEMORY_DEBUG
    /* Store description */
    kzcByteBufferWrite(&memoryBuffer, description);

    /* Store size */
    kzcByteBufferWrite(&memoryBuffer, size);

    {
        kzUint i;
        for (i = 0; i < KZC_MEMORY_MANAGER_GUARD_PRE_SIZE; ++i)
        {
            kzByte byte = kzcMemoryGetGuardByte_internal(pointer + kzcByteBufferGetOffset(&memoryBuffer));
            kzcByteBufferWrite(&memoryBuffer, byte);
        }
    }
#endif

    /* Actual data */
    kzcByteBufferAllocateWritePointer(&memoryBuffer, size, &array);

#ifdef KZC_MEMORY_DEBUG
    {
        kzUint i;
        for (i = 0; i < KZC_MEMORY_MANAGER_GUARD_POST_SIZE; ++i)
        {
            kzByte byte = kzcMemoryGetGuardByte_internal(pointer + kzcByteBufferGetOffset(&memoryBuffer));
            kzcByteBufferWrite(&memoryBuffer, byte);
        }
    }
#endif

    kzcByteBufferSkip(&memoryBuffer, align);

    kzsAssert(kzcByteBufferIsFinished(&memoryBuffer));

    /*kzsAssert(*(kzUint*)(void*)&array % KZC_MEMORY_ALIGNMENT_SIZE == 0);*/

    *out_array = array;
    kzsSuccess();
}

/** Generic memory deallocator, which forwards the deallocation to the specified manager. */
static kzsError kzcMemoryFree_internal(const struct KzcMemoryManager* memoryManager, void* pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsError result;

    result = memoryManager->deallocator(memoryManager, pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(description));
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzsError kzcMemoryFreePointer(void* pointer)
{
    kzsError result;

    struct KzcMemoryManager* manager;
#ifdef KZC_MEMORY_DEBUG
    kzString description;
#endif
    kzByte* offsetedPointer = pointer;

    kzsAssert(kzcIsValidPointer(pointer));

    manager = kzcMemoryGetManager(pointer);

#ifdef KZC_MEMORY_DEBUG
    offsetedPointer -= KZC_MEMORY_MANAGER_GUARD_PRE_SIZE;
    offsetedPointer -= sizeof(kzUint);
    description = kzcMemoryGetDebugDescription_internal(pointer);
    offsetedPointer -= sizeof(description);
#endif

    offsetedPointer -= sizeof(manager);

    result = kzcMemoryFree_internal(manager, offsetedPointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(description));
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcMemoryFreeArray(void* array)
{
    kzsError result;

    struct KzcMemoryManager* manager;
#ifdef KZC_MEMORY_DEBUG
    kzString description;
#endif
    kzByte* offsetedPointer = array;

    kzsAssert(kzcIsValidPointer(array));

    manager = kzcMemoryGetManager(array);

#ifdef KZC_MEMORY_DEBUG
    offsetedPointer -= KZC_MEMORY_MANAGER_GUARD_PRE_SIZE;
    offsetedPointer -= sizeof(kzUint);
    description = kzcMemoryGetDebugDescription_internal(array);
    offsetedPointer -= sizeof(description);
#endif

    offsetedPointer -= sizeof(manager);

    offsetedPointer -= sizeof(kzUint);

    result = kzcMemoryFree_internal(manager, offsetedPointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(description));
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcMemoryFreeVariable(void* variable)
{
    return kzcMemoryFreePointer(variable);
}

kzsError kzcMemoryReallocPointer(void* pointer, kzUint oldSize, kzUint newSize, void** out_newPointer)
{
    kzsError result;
    struct KzcMemoryManager* manager = kzcMemoryGetManager(pointer);
    void* newPointer;
#ifdef KZC_MEMORY_DEBUG
    kzString description = kzcMemoryGetDebugDescription_internal(pointer);
#endif

    result = kzcMemoryAllocPointer_private(manager, newSize, &newPointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(description));
    kzsErrorForward(result);

    kzsMemcpy(newPointer, pointer, kzsMinU(oldSize, newSize));

    result = kzcMemoryFreePointer(pointer);
    kzsErrorForward(result);

    *out_newPointer = newPointer;
    kzsSuccess();
}

#ifndef kzcIsValidPointer
kzBool kzcIsValidPointer(const void* pointer)
{
#ifdef KZC_MEMORY_DEBUG
    return pointer != KZ_NULL && kzcMemoryManagerCheckGuards_internal(pointer);
#else
    return pointer != KZ_NULL;
#endif
}
#endif

kzBool kzcIsValidArrayIndex(const void* array, kzUint index)
{
    return index < kzcArrayLength(array);
}

#ifdef KZC_MEMORY_DEBUG

static kzUint kzcMemoryGetSize_internal(const void* pointer)
{
    struct KzcByteReadBuffer memoryBuffer;
    kzUint size;

    kzcByteBufferInitializeReversedRead(&memoryBuffer, pointer, KZC_MEMORY_MANAGER_GUARD_PRE_SIZE + sizeof(size));
    kzcByteBufferSkip(&memoryBuffer, KZC_MEMORY_MANAGER_GUARD_PRE_SIZE); /* Pre-guards */

    kzcByteBufferRead(&memoryBuffer, size);

    kzsAssert(kzcByteBufferIsFinished(&memoryBuffer));

    return size;
}

static kzString kzcMemoryGetDebugDescription_internal(const void* pointer)
{
    struct KzcByteReadBuffer memoryBuffer;
    kzString description;

    kzcByteBufferInitializeReversedRead(&memoryBuffer, pointer, KZC_MEMORY_MANAGER_GUARD_PRE_SIZE + sizeof(kzUint) + sizeof(description));
    kzcByteBufferSkip(&memoryBuffer, KZC_MEMORY_MANAGER_GUARD_PRE_SIZE); /* Pre-guards */

    kzcByteBufferSkip(&memoryBuffer, sizeof(kzUint)); /* Memory size */

    kzcByteBufferRead(&memoryBuffer, description);

    kzsAssert(kzcByteBufferIsFinished(&memoryBuffer));

    return description;
}

static kzByte kzcMemoryGetGuardByte_internal(const kzByte* pointer)
{
    return *(kzByte*)&pointer;
}

static kzBool kzcMemoryCheckGuardByte_internal(const kzByte* pointer)
{
    return *pointer == kzcMemoryGetGuardByte_internal(pointer);
}

#if defined KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
static kzByte kzcMemoryGetUnallocatedByte_internal(const kzByte* pointer)
{
    return *(kzByte*)&pointer ^ 0xFF;
}

void kzcMemoryFillUnallocated(kzByte* pointer, kzUint size)
{
    struct KzcByteWriteBuffer buffer;
    kzUint i;

    kzcByteBufferInitializeWrite(&buffer, pointer, size);
    for (i = 0; i < size; ++i)
    {
        kzByte byte = kzcMemoryGetUnallocatedByte_internal((kzByte*)pointer + kzcByteBufferGetOffset(&buffer));
        kzcByteBufferWrite(&buffer, byte);
    }
    kzsAssert(kzcByteBufferIsFinished(&buffer));
}

kzBool kzcMemoryCheckUnallocated(const kzByte* pointer, kzUint size)
{
    struct KzcByteReadBuffer buffer;
    kzUint i;
    kzBool result = KZ_TRUE;

    kzcByteBufferInitializeRead(&buffer, pointer, size);
    for (i = 0; i < size; ++i)
    {
        kzByte guardByte = kzcMemoryGetUnallocatedByte_internal((kzByte*)pointer + kzcByteBufferGetOffset(&buffer));
        kzByte byte;
        kzBool check;
        kzcByteBufferRead(&buffer, byte);
        check = (byte == guardByte);
        result &= check;
    }
    kzsAssert(kzcByteBufferIsFinished(&buffer));

    return result;
}
#endif

#ifndef kzcIsValidPointer
static kzBool kzcMemoryManagerCheckGuards_internal(const kzByte* pointer)
{
    kzBool result = KZ_TRUE;
    kzUint memorySize;

    { /* Pre-guards and memory size */
        struct KzcByteReadBuffer memoryBuffer;
        kzUint i;

        kzcByteBufferInitializeReversedRead(&memoryBuffer, pointer, KZC_MEMORY_MANAGER_GUARD_PRE_SIZE + sizeof(kzUint));

        for (i = 0; i < KZC_MEMORY_MANAGER_GUARD_PRE_SIZE; ++i)
        {
            kzcByteBufferSkip(&memoryBuffer, sizeof(kzByte));
            result &= kzcMemoryCheckGuardByte_internal(pointer - kzcByteBufferGetOffset(&memoryBuffer));
        }

        kzcByteBufferRead(&memoryBuffer, memorySize);

        kzsAssert(kzcByteBufferIsFinished(&memoryBuffer));
    }

    { /* Post-guards */
        struct KzcByteReadBuffer memoryBuffer;
        kzUint i;

        kzcByteBufferInitializeRead(&memoryBuffer, pointer, memorySize + KZC_MEMORY_MANAGER_GUARD_POST_SIZE);

        kzcByteBufferSkip(&memoryBuffer, memorySize);

        for (i = 0; i < KZC_MEMORY_MANAGER_GUARD_POST_SIZE; ++i)
        {
            result &= kzcMemoryCheckGuardByte_internal(pointer + kzcByteBufferGetOffset(&memoryBuffer));
            kzcByteBufferSkip(&memoryBuffer, sizeof(kzByte));
        }

        kzsAssert(kzcByteBufferIsFinished(&memoryBuffer));
    }

    return result;
}
#endif
#endif

kzU32 kzcMemoryCalculateChecksum(const void* pointer)
{
#ifdef KZC_MEMORY_DEBUG
    /* Calculates the checksum with Adler-32 for speed and simplicity reasons. */
    const kzU32 MOD_ADLER = 65521;
    kzUint length = kzcMemoryGetSize_internal(pointer);
    kzByte* data = (kzByte*)pointer;
    kzU32 a = 1;
    kzU32 b = 0;
 
    while (length != 0)
    {
        a = (a + *data++) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
 
        --length;
    }
 
    return (b << 16) | a;
#else
    KZ_UNUSED_PARAMETER(pointer);
    return 0;
#endif
}

struct KzcMemoryManager* kzcMemoryGetManager(const void* pointer)
{
    struct KzcByteReadBuffer memoryBuffer;
    struct KzcMemoryManager* manager;
    kzUint bufferSize;

    kzsAssert(kzcIsValidPointer(pointer));

    bufferSize = sizeof(manager);
#ifdef KZC_MEMORY_DEBUG
    bufferSize += KZC_MEMORY_MANAGER_GUARD_PRE_SIZE; /* Pre-guards */
    bufferSize += sizeof(kzUint); /* Memory size */
    bufferSize += sizeof(kzString); /* Description */
#endif

    kzcByteBufferInitializeReversedRead(&memoryBuffer, pointer, bufferSize);

#ifdef KZC_MEMORY_DEBUG
    kzcByteBufferSkip(&memoryBuffer, KZC_MEMORY_MANAGER_GUARD_PRE_SIZE + sizeof(kzUint) + sizeof(kzString));
#endif
    kzcByteBufferRead(&memoryBuffer, manager);

    kzsAssert(kzcByteBufferIsFinished(&memoryBuffer));

    return manager;
}

kzUint kzcArrayLength(const void* array)
{
    struct KzcByteReadBuffer memoryBuffer;
    kzUint length;

    kzUint bufferSize = sizeof(struct KzcMemoryManager*) + sizeof(kzUint);
#ifdef KZC_MEMORY_DEBUG
    bufferSize += KZC_MEMORY_MANAGER_GUARD_PRE_SIZE; /* Pre-guards */
    bufferSize += sizeof(kzUint); /* Memory size */
    bufferSize += sizeof(kzString); /* Description */
#endif

    kzcByteBufferInitializeReversedRead(&memoryBuffer, array, bufferSize);

#ifdef KZC_MEMORY_DEBUG
    kzcByteBufferSkip(&memoryBuffer, KZC_MEMORY_MANAGER_GUARD_PRE_SIZE + sizeof(kzUint) + sizeof(kzString));
#endif
    kzcByteBufferSkip(&memoryBuffer, sizeof(struct KzcMemoryManager*));
    kzcByteBufferRead(&memoryBuffer, length);

    kzsAssert(kzcByteBufferIsFinished(&memoryBuffer));

    return length;
}

enum KzcMemoryManagerType kzcMemoryManagerGetType(const struct KzcMemoryManager* memoryManager)
{
    return memoryManager->type;
}
