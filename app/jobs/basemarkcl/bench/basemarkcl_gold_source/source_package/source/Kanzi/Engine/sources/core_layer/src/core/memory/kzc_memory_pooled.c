/**
* \file
* Pooled memory manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/

#include "kzc_memory_pooled.h"

#include "kzc_memory_private.h"

#include "kzc_memory_pool.h"
#include "kzc_memory_manager.h"
#include "kzc_memory_system.h"
#include "kzc_byte_buffer.h"

#include <core/debug/kzc_log.h>
#include <core/kzc_error_codes.h>
#include <core/util/string/kzc_string.h>

#include <system/wrappers/kzs_memory.h>
#include <system/debug/kzs_log.h>
#include <system/kzs_error_codes.h>
#include <system/wrappers/kzs_math.h>


#ifdef KZC_MEMORY_DEBUG
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/string/kzc_string.h>

#include <system/wrappers/kzs_math.h>

/** Entry for debug allocation hash map. */
struct KzcDebugAllocationEntry
{
    kzString description; /**< Description of the allocations. */
    kzUint allocationCount; /**< Current number of allocations for given description. */
    kzUint peakAllocationCount; /**< Peak number of allocations for given description. */
    kzUint cumulativeAllocationCount; /**< Cumulative number of allocations for given description. */
};
#endif

struct KzcPooledMemoryManager
{
    struct KzcMemoryManager base; /**< Memory manager base. */
    kzUint poolCount; /**< Number of memory pools */
    struct KzcMemoryPool** pools; /**< Array of memory pools */
#ifdef KZC_MEMORY_MEASURE
    kzUint peakMaximumAvailableSize; /**< Peak lowest value for the maximum available block. */
#endif

#ifdef KZC_MEMORY_DEBUG
    const struct KzcMemoryManager* parent; /**< Parent memory manager. Needed for memory dumping. */
    struct KzcHashMap* debugAllocationMap; /**< Debug allocation entries based on allocation description. <kzString, DebugallocationEntry>. */
#endif
};


static kzsError kzcMemoryPooledDelete_internal(struct KzcMemoryManager* memoryManager);
static kzsError kzcMemoryPooledAlloc_internal(const struct KzcMemoryManager* memoryManager, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));
static kzsError kzcMemoryPooledFree_internal(const struct KzcMemoryManager* memoryManager, void* pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description));


kzsError kzcMemoryManagerCreatePooledManager(const struct KzcMemoryManager* parentManager, kzUint poolCount,
                                             kzUint poolSize, struct KzcMemoryManager** out_memoryManager)
{
    kzsError result;
    struct KzcPooledMemoryManager* pooledMemoryManager;
    kzUint allocSize;
    void* managerPointer;
    struct KzcByteWriteBuffer managerBuffer;
    kzUint i;
    kzUint minimumEmptySize = sizeof(struct KzcMemoryManager*);
    struct KzcMemoryManager* memoryManager;


#ifdef KZC_MEMORY_DEBUG
    minimumEmptySize += sizeof(kzString);
    minimumEmptySize += sizeof(kzUint);
    minimumEmptySize += KZC_MEMORY_MANAGER_GUARD_PRE_SIZE;
    minimumEmptySize += KZC_MEMORY_MANAGER_GUARD_POST_SIZE;
#endif
   
    /*
     * Allocate memory for the manager, manager data and memory pools.
     * Required space is size of the MemoryManager struct + size of the PooledMemoryManager struct + array of memory pool pointers.
     */
    allocSize = sizeof(*pooledMemoryManager) + sizeof(struct KzcMemoryPool*) * poolCount;
    result = kzcMemoryAllocPointer(parentManager, &managerPointer, allocSize, "Pooled memory manager");
    kzsErrorForward(result);

    kzcByteBufferInitializeWrite(&managerBuffer, managerPointer, allocSize);

    /* PooledMemoryManager data follows right after MemoryManager struct */
    kzcByteBufferAllocateWriteVariable(&managerBuffer, pooledMemoryManager);

    pooledMemoryManager->poolCount = poolCount;

    /* Pool array data follows right after the PooledMemoryManager struct */
    kzcByteBufferAllocateWriteArray(&managerBuffer, poolCount, pooledMemoryManager->pools);

    kzsAssert(kzcByteBufferIsFinished(&managerBuffer));

    for (i = 0; i < poolCount; ++i)
    {
        result = kzcMemoryManagerCreatePool(parentManager, i, poolSize, minimumEmptySize, &pooledMemoryManager->pools[i]);
        kzsErrorForward(result);
    }

#ifdef KZC_MEMORY_DEBUG
    {
        pooledMemoryManager->parent = parentManager;
    
        result = kzcHashMapCreate(parentManager, KZC_HASH_MAP_CONFIGURATION_STRING, &pooledMemoryManager->debugAllocationMap);
        kzsErrorForward(result);
    }
#endif

    kzcMemoryManagerInitialize_private(&pooledMemoryManager->base, KZC_MEMORY_MANAGER_TYPE_POOLED, kzcMemoryPooledDelete_internal, kzcMemoryPooledAlloc_internal, kzcMemoryPooledFree_internal);

    memoryManager = (struct KzcMemoryManager*)pooledMemoryManager;

#ifdef KZC_MEMORY_MEASURE
    pooledMemoryManager->peakMaximumAvailableSize = kzcMemoryPooledGetMaximumAvailableSize(memoryManager);
#endif

    *out_memoryManager = memoryManager;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemoryPooledDelete_internal(struct KzcMemoryManager* memoryManager)
{
    kzsError result;
    struct KzcPooledMemoryManager* pooledMemoryManager = (struct KzcPooledMemoryManager*)memoryManager;
    kzUint i;

    for (i = 0; i < pooledMemoryManager->poolCount; ++i)
    {
        result = kzcMemoryManagerDeletePool(pooledMemoryManager->pools[i]);
        kzsErrorForward(result);
    }

#ifdef KZC_MEMORY_DEBUG
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(pooledMemoryManager->debugAllocationMap);

        while (kzcHashMapIterate(it))
        {
            struct KzcDebugAllocationEntry* entry = (struct KzcDebugAllocationEntry*)kzcHashMapIteratorGetValue(it);
            result = kzcMemoryFreeVariable(entry);
            kzsErrorForward(result);
        }
    }

    result = kzcHashMapDelete(pooledMemoryManager->debugAllocationMap);
    kzsErrorForward(result);
#endif

    result = kzcMemoryFreePointer(memoryManager);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzcMemoryPooledGetMaximumAvailableSize(const struct KzcMemoryManager* memoryManager)
{
    kzUint largestSize = 0;
    struct KzcPooledMemoryManager* pooledMemoryManager = (struct KzcPooledMemoryManager*)memoryManager;
    kzUint i;

    for (i = 0; i < pooledMemoryManager->poolCount; ++i) 
    {
        struct KzcMemoryPool* pool = pooledMemoryManager->pools[i];
        kzUint poolSize = kzcMemoryPoolGetMaximumAvailableSize(pool);
        if (poolSize >= largestSize)
        {
            largestSize = poolSize;
        }
    }

    return largestSize;
}

#ifdef KZC_MEMORY_MEASURE
kzUint kzcMemoryPooledGetPeakMaximumAvailableSize(const struct KzcMemoryManager* memoryManager)
{
    struct KzcPooledMemoryManager* pooledMemoryManager = (struct KzcPooledMemoryManager*)memoryManager;

    kzsAssert(kzcIsValidPointer(pooledMemoryManager));

    return pooledMemoryManager->peakMaximumAvailableSize;
}
#endif

KZ_CALLBACK static kzsError kzcMemoryPooledAlloc_internal(const struct KzcMemoryManager* memoryManager, kzUint size, void** out_pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsError result;
    struct KzcPooledMemoryManager* pooledMemoryManager = (struct KzcPooledMemoryManager*)memoryManager;
    void* pointer = KZ_NULL;
    kzUint i;

    for (i = 0; i < pooledMemoryManager->poolCount && pointer == KZ_NULL; ++i) 
    {
        struct KzcMemoryPool* pool = pooledMemoryManager->pools[i];
        /* See if there is enough space available in some pool if there are several pools */
        if (pooledMemoryManager->poolCount == 1 || kzcMemoryPoolGetMaximumAvailableSize(pool) >= size)
        {
            result = kzcMemoryPoolAlloc(pool, size, &pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(description));
            kzsErrorForward(result);
        }
    }
    if (pointer == KZ_NULL)
    {
        struct KzcMemoryManager* systemMemoryManager;
        kzMutableString errorMessage;

        result = kzcMemoryManagerCreateSystemManager(&systemMemoryManager);
        kzsErrorForward(result);

#ifdef KZC_MEMORY_DEBUG
        result = kzcStringFormat(systemMemoryManager, "Out of memory while trying to allocate %i bytes (%s)", &errorMessage, size, description);
        kzsErrorForward(result);
#else
        result = kzcStringFormat(systemMemoryManager, "Out of memory while trying to allocate %i bytes", &errorMessage, size);
        kzsErrorForward(result);
#endif

        kzsErrorThrow(KZS_ERROR_OUT_OF_MEMORY, errorMessage);
    }

#ifdef KZC_MEMORY_DEBUG
    {
        struct KzcHashMap* map = pooledMemoryManager->debugAllocationMap;
        struct KzcDebugAllocationEntry* entry;
        if (!kzcHashMapGet(pooledMemoryManager->debugAllocationMap, description, (void**)&entry))
        {
            result = kzcMemoryAllocVariable(kzcMemoryGetManager(map), entry, "DebugAllocationEntry");
            kzsErrorForward(result);

            entry->description = description;
            entry->allocationCount = 0;
            entry->peakAllocationCount = 0;
            entry->cumulativeAllocationCount = 0;

            result = kzcHashMapPut(map, description, entry);
            kzsErrorForward(result);
        }
        ++entry->allocationCount;
        entry->peakAllocationCount = kzsMaxU(entry->peakAllocationCount, entry->allocationCount);
        ++entry->cumulativeAllocationCount;
    }
#endif

#ifdef KZC_MEMORY_MEASURE
    pooledMemoryManager->peakMaximumAvailableSize = kzsMinU(pooledMemoryManager->peakMaximumAvailableSize,
                                                            kzcMemoryPooledGetMaximumAvailableSize(memoryManager));
#endif

    *out_pointer = pointer;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzcMemoryPooledFree_internal(const struct KzcMemoryManager* memoryManager, void* pointer MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(kzString description))
{
    kzsError result;
    struct KzcPooledMemoryManager* pooledMemoryManager = (struct KzcPooledMemoryManager*)memoryManager;
    kzUint i;
    kzBool poolFound = KZ_FALSE;

#ifdef KZC_MEMORY_DEBUG
    {
        struct KzcDebugAllocationEntry* entry;
        if (kzcHashMapGet(pooledMemoryManager->debugAllocationMap, description, (void**)&entry))
        {
            --entry->allocationCount;
        }
        else
        {
            kzsErrorThrow(KZC_ERROR_MEMORY_ALLOCATION_MISMATCH, "Tried to free unallocated memory.");
        }
    }
#endif

    for (i = 0; i < pooledMemoryManager->poolCount && !poolFound; ++i)
    {
        struct KzcMemoryPool* pool = pooledMemoryManager->pools[i];
        poolFound = kzcMemoryPoolIsPointerInRange(pool, pointer);
        if (poolFound)
        {
            result = kzcMemoryPoolFree(pool, pointer);
            kzsErrorForward(result);
        }
    }
    kzsErrorTest(poolFound, KZC_ERROR_MEMORY_POOL_NOT_FOUND, "None of the memory pools manages the memory range of the given pointer.");

    kzsSuccess();
}

kzsError kzcMemoryPrintDebugAllocations(const struct KzcMemoryManager* memoryManager)
{
    kzsErrorTest(memoryManager->type == KZC_MEMORY_MANAGER_TYPE_POOLED,
                 KZS_ERROR_ILLEGAL_ARGUMENT, "Printing debug allocations is only available for pooled memory managers");

#ifdef KZC_MEMORY_DEBUG
    {
        kzsError result;
        struct KzcPooledMemoryManager* pooledMemoryManager = (struct KzcPooledMemoryManager*)memoryManager;
        struct KzcHashMapIterator it = kzcHashMapGetIterator(pooledMemoryManager->debugAllocationMap);
        
        kzsLog(KZS_LOG_LEVEL_DETAIL, "--- Memory allocation status ---");

        while (kzcHashMapIterate(it))
        {
            struct KzcDebugAllocationEntry* entry = (struct KzcDebugAllocationEntry*)kzcHashMapIteratorGetValue(it);

            kzsAssert(kzcIsValidPointer(entry));

            result = kzcLog(pooledMemoryManager->parent, KZS_LOG_LEVEL_DETAIL, "%s: %i allocated (peak %i, cumulative %i)",
                            entry->description, entry->allocationCount, entry->peakAllocationCount, entry->cumulativeAllocationCount);
            kzsErrorForward(result);
        }

        kzsLog(KZS_LOG_LEVEL_DETAIL, "--------------------------------");
    }
#else
    KZ_UNUSED_PARAMETER(memoryManager);
    kzsLog(KZS_LOG_LEVEL_INFO, "Memory debugging disabled. Unable to print allocation information.");
#endif

    kzsSuccess();
}

kzsError kzcMemoryDump(const struct KzcMemoryManager* memoryManager)
{
    struct KzcPooledMemoryManager* pooledMemoryManager;
    kzUint i;

    kzsErrorTest(memoryManager->type == KZC_MEMORY_MANAGER_TYPE_POOLED,
                 KZS_ERROR_ILLEGAL_ARGUMENT, "Dumping memory content is only available for pooled memory managers");

    pooledMemoryManager = (struct KzcPooledMemoryManager*)memoryManager;

    kzsLog(KZS_LOG_LEVEL_INFO, "Dumping content of memory pools");
    for (i = 0; i < pooledMemoryManager->poolCount; ++i)
    {
        kzcMemoryPoolDump(pooledMemoryManager->pools[i]);
    }
    kzsLog(KZS_LOG_LEVEL_INFO, "End of memory dump\n");

    kzsSuccess();
}
