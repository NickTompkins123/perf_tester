/**
* \file
* System memory manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_MEMORY_H
#define KZS_MEMORY_H


#include <system/kzs_types.h>
#include <system/wrappers/kzs_wrapper.h>
#include <system/debug/kzs_counter.h>

#include <stdlib.h>
#include <string.h>


/** Enable debug output for every allocation and deallocation of system memory. */
#define DEBUG_ALLOCATIONS 0

#if DEBUG_ALLOCATIONS
#include <stdio.h>
#endif


/** Keeps track of number of memory allocations. */
extern kzInt g_allocationCount_private;
extern kzInt g_allocatedMemory_private;


/** System level malloc. */
KZ_INLINE void* kzsMalloc(kzUint size)
{
    void* pointer;
    ++g_allocationCount_private;
    pointer = malloc(size);
#if DEBUG_ALLOCATIONS
    g_allocatedMemory_private += size;
    printf("Allocated %p, total memory %d\n", pointer, g_allocatedMemory_private);
#endif
    return KZS_WRAP_NULL(pointer);
}

/** System level malloc. */
KZ_INLINE void* kzsRealloc(void* pointer, kzUint newSize)
{
    return realloc(pointer, newSize);
}

/** System level free. */
KZ_INLINE void kzsFree(void* pointer)
{
    --g_allocationCount_private;
#if DEBUG_ALLOCATIONS
    printf("Deallocating %p\n", pointer);
#endif
    free(pointer);
}

/** System level memcpy. */
KZ_INLINE void kzsMemcpy(void* destinationPointer, const void* sourcePointer, kzUint size)
{
    memcpy(destinationPointer, sourcePointer, size);
    kzsCounterIncrease("kzsMemcpy");
}

/** System level memset. */
KZ_INLINE void kzsMemset(void* pointer, kzInt valueToSet, kzUint size)
{
    memset(pointer, valueToSet, size);
    kzsCounterIncrease("kzsMemset");
}


/** Gets the number of active memory allocations. */
kzInt kzsMemoryGetAllocationCount(void);

/** Resets the number of active memory allocations. */
void kzsMemoryResetAllocationCount(void);


#endif
