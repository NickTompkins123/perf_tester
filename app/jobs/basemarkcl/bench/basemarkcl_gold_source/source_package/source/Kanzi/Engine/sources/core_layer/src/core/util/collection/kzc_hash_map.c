/**
 * \file
 * Hash map.
 * 
 * Hash map implemented using open addressing for dealing with hash collisions.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_hash_map.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/kzc_error_codes.h>

#include <system/wrappers/kzs_math.h>
#include <system/debug/kzs_counter.h>


const struct KzcHashMapConfiguration KZC_HASH_MAP_CONFIGURATION_INT = {kzcHashCodeFromInt, kzcCompareInts};
const struct KzcHashMapConfiguration KZC_HASH_MAP_CONFIGURATION_U32 = {kzcHashCodeFromU32, kzcCompareU32s};
const struct KzcHashMapConfiguration KZC_HASH_MAP_CONFIGURATION_POINTER = {kzcHashCodeFromPointer, kzcComparePointers};
const struct KzcHashMapConfiguration KZC_HASH_MAP_CONFIGURATION_STRING = {kzcHashCodeFromString, kzcCompareStrings};


/**
 * Hash map entry of one key -> value pair.
 */
struct KzcHashMapEntry
{
    kzBool isUsed; /**< Is the entry used. */
    const void* key; /**< Pointer to the key for the entry. */
    void* value; /**< Pointer to the stored value. */
};

/**
 * Hash map entry of one key -> value pair for ordered hash map.
 */
struct KzcHashMapOrderedEntry
{
    struct KzcHashMapEntry parent; /**< For inheritance. */
    struct KzcHashMapOrderedEntry* previous; /**< Previous hash map entry. */
    struct KzcHashMapOrderedEntry* next; /**< Next hash map entry. */
};

typedef union
{
    struct KzcHashMapEntry* unordered;   /**< The internal table of normal hash map. */
    struct KzcHashMapOrderedEntry* ordered;   /**< The internal table of ordered hash map. */
} KzcHashMapTable; /**< Hash map table definiotion. */

struct KzcHashMap
{
    struct KzcHashMapConfiguration configuration; /**< Hash map configuration. */
    kzBool isOrdered; /**< Is this hash map ordered or not. */

    KzcHashMapTable table; /**< Hash map entries. */
    kzUint tableSize;                /**< The length of 'table'. */
    kzUint maxSize;                  /**< Maximum number of entries storable in 'table'. */
    kzUint size;                     /**< Current number of used entries stored in the map. */

    /**
     * Store the maximum number of hash collisions in this table since the last time internal table size was changed.
     * Used to speed up negative results when searching for keys.
     * Otherwise the entire table would be scanned for each negative search result.
     */
    kzUint maxNumberOfOverloads;

    struct KzcHashMapOrderedEntry* first; /**< First entry of ordered hash map. */
    struct KzcHashMapOrderedEntry* last; /**< Last entry of ordered hash map. */
};

/**
 * This is the maximum load capacity of the hash map allocated to hold size_ entries. 
 * Ideally, when using open addressing maximum capacity should be between 50% and 75%, but never more as the performance 
 * of open addressing degrades quickly with larger capacities than this.
 */
#define KZC_HASH_MAP_MAXIMUM_LOAD_FACTOR 0.75f


/** Return actual entry from hash map corresponging for key. */
static struct KzcHashMapEntry* kzcHashMapGetEntryForKey_internal(const struct KzcHashMap* HashMap, const void* key);


/**
 * Return the size of the array required to hold numEntries number of entries.
 * \param[in] numEntries   Minimum number of entries storable in the hash map
 * \param[out] out_size    Size of the table to allocate that will hold at least numEntries number of entries
 * \param[out] out_maxSize Maximum number of entries storable. See declaration of KZC_HASH_MAP_MAXIMUM_LOAD_FACTOR for further clarification
 */
static kzsError kzcHashMapGetRequiredSize_internal(const kzUint entryCount, kzUint* out_size, kzUint* out_maxSize)
{
    /* A list of fairly good primes for table sizes. 
       Choosing a good prime for the table size decreases the chances of collisions.
       See: http://planetmath.org/encyclopedia/GoodHashTablePrimes.html */
    const kzUint hashPrimes[] = {11, 23, 53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 
                                 49157, 98317, 196613, 393241, 786433, 1572869, 3145739, 
                                 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 
                                 402653189, 805306457, 1610612741, 0};

    kzUint size;
    kzUint i = 0;
    kzUint maxSize;

    /* Find the size, or return error. */
    do 
    {
        size = hashPrimes[i];

        kzsErrorTest(size > 0, KZS_ERROR_ILLEGAL_ARGUMENT, "Hash map maximum possible capacity exceeded");

        maxSize = kzsFloatToUint(size * KZC_HASH_MAP_MAXIMUM_LOAD_FACTOR);
        ++i;
    } while (maxSize < entryCount);

    *out_size = size;
    *out_maxSize = maxSize;
    kzsSuccess();
}

static kzsError kzcHashMapCreateWithCapacity_internal(const struct KzcMemoryManager* memoryManager,
                                                      struct KzcHashMapConfiguration configuration,
                                                      kzUint capacity, kzBool ordered, struct KzcHashMap** out_hashMap)
{
    kzsError result;
    struct KzcHashMap* hashMap;

    /* Check that all function pointers are passed */
    kzsAssert(configuration.hashFunction != KZ_NULL);
    kzsAssert(configuration.keyComparator != KZ_NULL);

    /* Allocate memory. */
    result = kzcMemoryAllocVariable(memoryManager, hashMap, "HashMap");
    kzsErrorForward(result);

    hashMap->isOrdered = ordered;
    hashMap->configuration = configuration;

    /* Get size of internal array that can store sizeHint number of entries and allocate memory for internal table.*/
    result = kzcHashMapGetRequiredSize_internal(capacity, &(hashMap->tableSize), &(hashMap->maxSize));
    kzsErrorForward(result);

    /* Allocate the internal array. */
    if (ordered)
    {
        result = kzcMemoryAllocArray(memoryManager, hashMap->table.ordered, hashMap->tableSize, "HashMap table");
        kzsErrorForward(result);
    }
    else
    {
        result = kzcMemoryAllocArray(memoryManager, hashMap->table.unordered, hashMap->tableSize, "HashMap table");
        kzsErrorForward(result);
    }

    kzcHashMapClear(hashMap);

    *out_hashMap = hashMap;
    kzsSuccess();
}

kzsError kzcHashMapCreateWithCapacity(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                      kzUint capacity, struct KzcHashMap** out_hashMap)
{
    kzsError result;
    struct KzcHashMap* hashMap;

    result = kzcHashMapCreateWithCapacity_internal(memoryManager, configuration, capacity, KZ_FALSE, &hashMap);
    kzsErrorForward(result);

    *out_hashMap = hashMap;
    kzsSuccess();
}

kzsError kzcHashMapCreate(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                          struct KzcHashMap** out_hashMap)
{
    kzsError result;
    struct KzcHashMap* hashMap;

    result = kzcHashMapCreateWithCapacity_internal(memoryManager, configuration, 0, KZ_FALSE, &hashMap);
    kzsErrorForward(result);

    *out_hashMap = hashMap;
    kzsSuccess();
}

kzsError kzcHashMapCreateOrderedWithCapacity(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                             kzUint capacity, struct KzcHashMap** out_hashMap)
{
    kzsError result;
    struct KzcHashMap* hashMap;

    result = kzcHashMapCreateWithCapacity_internal(memoryManager, configuration, capacity, KZ_TRUE, &hashMap);
    kzsErrorForward(result);

    *out_hashMap = hashMap;
    kzsSuccess();
}

kzsError kzcHashMapCreateOrdered(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                 struct KzcHashMap** out_hashMap)
{
    kzsError result;
    struct KzcHashMap* hashMap;

    result = kzcHashMapCreateWithCapacity_internal(memoryManager, configuration, 0, KZ_TRUE, &hashMap);
    kzsErrorForward(result);

    *out_hashMap = hashMap;
    kzsSuccess();
}

kzsError kzcHashMapDelete(struct KzcHashMap* hashMap)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(hashMap));

    /* Free memory used by hash map. */

    if (hashMap->isOrdered)
    {
        result = kzcMemoryFreeArray(hashMap->table.ordered);
        kzsErrorForward(result);
    }
    else
    {
        result = kzcMemoryFreeArray(hashMap->table.unordered);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(hashMap);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcHashMapClear(struct KzcHashMap* hashMap)
{
    kzUint i;

    for (i = 0; i < hashMap->tableSize; ++i)
    {
        struct KzcHashMapEntry* entry;

        if (hashMap->isOrdered)
        {
            entry = &hashMap->table.ordered[i].parent;
        }
        else
        {
            entry = &hashMap->table.unordered[i];
        }
        entry->key = KZ_NULL;
        entry->value = KZ_NULL;
        entry->isUsed = KZ_FALSE;
    }

    hashMap->maxNumberOfOverloads = 0;
    hashMap->size = 0;
    hashMap->first = KZ_NULL;
    hashMap->last = KZ_NULL;
}

struct KzcHashMapConfiguration kzcHashMapGetConfiguration(const struct KzcHashMap* hashMap)
{
    return hashMap->configuration;
}

/** Utility function to insert entries into the map. 
 *  Used only when the sizes, values and pointers passed to this function are known to be within valid range.  
 * \param[in] hashMap Hash map to manipulate
 * \param[in] key Key of the entry
 * \param[in] value Value of the entry
 */ 
static void kzcHashMapInsert_internal(struct KzcHashMap* hashMap, const void* key, void* value)
{
    struct KzcHashMapEntry* entry;
    struct KzcHashMapOrderedEntry* orderedEntry = KZ_NULL; /* Prevent warnings of uninitialized variable, which can never happen. */
    kzUint tableSize;                   /* Length of the internal table */
    kzUint index;
    kzU32 hashValueOfKey;
    kzUint numberOfOverloadsForKey = 0;

    kzsAssert(hashMap != KZ_NULL);

    /* Init local references. */
    tableSize = hashMap->tableSize;

    kzsAssert(tableSize > 0);

    /* Init iteration. */
    hashValueOfKey = hashMap->configuration.hashFunction(key);
    index = (kzUint)hashValueOfKey % tableSize;

    do 
    {
        if (hashMap->isOrdered)
        {
            orderedEntry = &hashMap->table.ordered[index];
            entry = &orderedEntry->parent;
        }
        else
        {
            entry = &hashMap->table.unordered[index];
        }
        if (entry->isUsed)
        {
            index = (index + 1) % tableSize;
            ++numberOfOverloadsForKey;
            /* Duplicate keys should be handled prior to entering this function.
               We will never wrap around as there are never as many filled entries 
               as there are position in table - no need to store the number of searches */
        }
    } while (entry->isUsed);

    /* Update maximum number of overloads */
    hashMap->maxNumberOfOverloads = kzsMaxU(hashMap->maxNumberOfOverloads, numberOfOverloadsForKey);

    /* Free position found, store key and value */
    entry->key = key;
    entry->value = value;
    entry->isUsed = KZ_TRUE;
    ++hashMap->size;

    if (hashMap->isOrdered)
    {
        kzsAssert(orderedEntry != KZ_NULL);

        orderedEntry->previous = hashMap->last;
        orderedEntry->next = KZ_NULL;

        if (hashMap->first == KZ_NULL)
        {
            hashMap->first = orderedEntry;
            hashMap->last = orderedEntry;
        }
        else
        {
            hashMap->last->next = orderedEntry;
            hashMap->last = orderedEntry;
        }
    }
}

/** Helper function to manage table growth. */
static kzsError kzcHashMapGrowIfFull_internal(struct KzcHashMap* hashMap)
{
    kzsError result;

    kzsAssert(hashMap != KZ_NULL);

    /* If we reach maximum capacity for the map, we need to resize the internal table and copy the entries stored to their
       proper locations. */
    if (hashMap->size == hashMap->maxSize)
    {
        kzUint oldTableSize = hashMap->tableSize; /* Current size of the hash map table (!= currentSize) */
        KzcHashMapTable oldTable = hashMap->table;
        struct KzcHashMapOrderedEntry* oldFirst = hashMap->first;

        /* Get new size for table and allocate memory. */
        result = kzcHashMapGetRequiredSize_internal(hashMap->size + 1, &hashMap->tableSize, &hashMap->maxSize);
        kzsErrorForward(result);

        /* Allocate the internal array. */
        if (hashMap->isOrdered)
        {
            result = kzcMemoryAllocArray(kzcMemoryGetManager(hashMap), hashMap->table.ordered, hashMap->tableSize, "HashMap table");
            kzsErrorForward(result);
        }
        else
        {
            result = kzcMemoryAllocArray(kzcMemoryGetManager(hashMap), hashMap->table.unordered, hashMap->tableSize, "HashMap table");
            kzsErrorForward(result);
        }

        kzcHashMapClear(hashMap);

        /* Insert entries from the old table to the new. */
        if (hashMap->isOrdered)
        {
            struct KzcHashMapOrderedEntry* entry = oldFirst;
            while (entry != KZ_NULL)
            {
                kzcHashMapInsert_internal(hashMap, entry->parent.key, entry->parent.value);
                entry = entry->next;
            }
        }
        else
        {
            kzUint i;
            for (i = 0; i < oldTableSize; ++i)
            {
                struct KzcHashMapEntry* entry = &oldTable.unordered[i];
                if (entry->isUsed)
                {
                    kzcHashMapInsert_internal(hashMap, entry->key, entry->value);
                }
            }
        }

        /* Free memory used by the old table. */
        if (hashMap->isOrdered)
        {
            result = kzcMemoryFreeArray(oldTable.ordered);
            kzsErrorForward(result);
        }
        else
        {
            result = kzcMemoryFreeArray(oldTable.unordered);
            kzsErrorForward(result);
        }
    }

    kzsCounterIncrease("kzcHashMapGrowIfFull_internal");
    kzsSuccess();
}

kzsError kzcHashMapPut(struct KzcHashMap* hashMap, const void* key, void* value)
{
    kzsError result;

    struct KzcHashMapEntry* entry;        /* Iteration reference to internal table */

    kzsAssert(hashMap != KZ_NULL);

    entry = kzcHashMapGetEntryForKey_internal(hashMap, key);

    if (entry != KZ_NULL)
    {
        entry->key = key;
        entry->value = value;
    }
    else
    {
        /* Key was not found in table, now we need to ensure that we have space left for one more entry. */
        result = kzcHashMapGrowIfFull_internal(hashMap);
        kzsErrorForward(result);
        kzcHashMapInsert_internal(hashMap, key, value);
    }

    kzsSuccess();
}

kzsError kzcHashMapRemove(struct KzcHashMap* hashMap, const void* key)
{
    struct KzcHashMapEntry* entry;

    kzsAssert(hashMap != KZ_NULL);

    /* Try to find the specified entry. */
    entry = kzcHashMapGetEntryForKey_internal(hashMap, key);

    kzsErrorTest(entry != KZ_NULL, KZS_ERROR_ILLEGAL_ARGUMENT, "Hash map did no contain given key to remove");

    entry->isUsed = KZ_FALSE;
    --hashMap->size;

    if (hashMap->isOrdered)
    {
        struct KzcHashMapOrderedEntry* orderedEntry = (struct KzcHashMapOrderedEntry*)entry;
        if (orderedEntry->previous == KZ_NULL)
        {
            hashMap->first = orderedEntry->next;
        }
        else
        {
            orderedEntry->previous->next = orderedEntry->next;
        }

        if (orderedEntry->next == KZ_NULL)
        {
            hashMap->last = orderedEntry->previous;
        }
        else
        {
            orderedEntry->next->previous = orderedEntry->previous;
        }
    }

    kzsSuccess();
}

kzBool kzcHashMapGet(const struct KzcHashMap* hashMap, const void* key, void** out_value)
{
    struct KzcHashMapEntry* entry;
    kzBool keyFound;
    void* value;

    kzsAssert(kzcIsValidPointer(hashMap));

    /* Try to find the specified entry. kzcHashMapGetEntryForKey checks for pointer consistency.*/
    entry = kzcHashMapGetEntryForKey_internal(hashMap, key);

    if (entry != KZ_NULL)
    {
        value = entry->value; /* Note that value might still be null, since null values are allowed. */
        keyFound = KZ_TRUE;
    }
    else
    {
        value = KZ_NULL;
        keyFound = KZ_FALSE;
    }

    kzsCounterIncrease("kzcHashMapGet");
    *out_value = value;
    return keyFound;
}

const void* kzcHashMapGetStoredKey(const struct KzcHashMap* hashMap, const void* key)
{
    struct KzcHashMapEntry* entry;
    const void* storedKey;

    kzsAssert(kzcIsValidPointer(hashMap));

    /* Try to find the specified entry. kzcHashMapGetEntryForKey checks for pointer consistency.*/
    entry = kzcHashMapGetEntryForKey_internal(hashMap, key);

    if (entry != KZ_NULL)
    {
        storedKey = entry->key;
    }
    else
    {
        storedKey = KZ_NULL;
    }

    return storedKey;
}

kzBool kzcHashMapContains(const struct KzcHashMap* hashMap, const void* key)
{
    void* valueToDiscard;
    return kzcHashMapGet(hashMap, key, &valueToDiscard);
}

kzsError kzcHashMapGetOrThrowError(const struct KzcHashMap* hashMap, const void* key, void** out_value)
{
    struct KzcHashMapEntry* entry;

    /* Try to find the specified entry. kzcHashMapGetEntryForKey checks for pointer consistency.*/
    entry = kzcHashMapGetEntryForKey_internal(hashMap, key);

    kzsErrorTest(entry != KZ_NULL, KZC_ERROR_ELEMENT_NOT_FOUND, "Key not found in HashMap");

    *out_value = entry->value;
    kzsSuccess();
}

kzUint kzcHashMapGetSize(const struct KzcHashMap* hashMap)
{
    kzsAssert(hashMap != KZ_NULL);
    return hashMap->size;
}

kzBool kzcHashMapIsEmpty(const struct KzcHashMap* hashMap)
{
    return (hashMap->size == 0);
}

static struct KzcHashMapEntry* kzcHashMapGetEntryForKey_internal(const struct KzcHashMap* hashMap, const void* key)
{
    struct KzcHashMapEntry* result = KZ_NULL;
    kzUint tableSize;                          /* size of internal table */
    struct KzcHashMapEntry* entry;
    KzcComparatorFunction comparator;
    kzU32 keyHashValue;
    kzUint numberOfOverloads;                  /* Maximum number of key overloads in table */
    kzUint i;

    kzsAssert(hashMap != KZ_NULL);

    /* Localize variables */
    tableSize           = hashMap->tableSize;
    numberOfOverloads   = hashMap->maxNumberOfOverloads;
    comparator          = hashMap->configuration.keyComparator;

    /* Calculate initial position in table. */
    keyHashValue = hashMap->configuration.hashFunction(key);

    for (i = 0; i <= numberOfOverloads; ++i)
    {
        kzUint index = ((kzUint)keyHashValue + i) % tableSize;
        if (hashMap->isOrdered)
        {
            entry = &hashMap->table.ordered[index].parent;
        }
        else
        {
            entry = &hashMap->table.unordered[index];
        }
        if (entry->isUsed && comparator(entry->key, key) == 0)
        {
            result = entry;
            break;
        }
    }

    return result;
}

struct KzcHashMapIterator kzcHashMapGetIterator(const struct KzcHashMap* hashMap)
{
    struct KzcHashMapIterator iterator;

    /* Init iterator to start iterarion (does not refer to any entry yet) */
    iterator.map_private = hashMap;
    if (hashMap->isOrdered)
    {
        iterator.data.entry_private = KZ_NULL;
    }
    else
    {
        iterator.data.tableIndex_private = -1;
    }

    return iterator;
}

kzBool kzcHashMapIterate_private(struct KzcHashMapIterator* iterator)
{
    kzBool result;

    kzsAssert(iterator->map_private != KZ_NULL);

    /* Initially, iterator->tableIndex_private is set to -1. first call of this function starts the iteration at the beginning of the internal table.*/

    if (iterator->map_private->isOrdered)
    {
        iterator->data.entry_private = (iterator->data.entry_private == KZ_NULL) ?
                                       iterator->map_private->first :
                                       iterator->data.entry_private->next;
        result = iterator->data.entry_private != KZ_NULL;
    }
    else
    {
        kzUint i;

        result = KZ_FALSE;
        /* Find next entry in use and pass reference to it. */
        for (i = (kzUint)(iterator->data.tableIndex_private + 1); i < iterator->map_private->tableSize; ++i)
        {
            if (iterator->map_private->table.unordered[i].isUsed)
            {
                iterator->data.tableIndex_private = (kzInt)i;
                result = KZ_TRUE;
                break;
            }
        }
    }

    /* No more used entries found before the end of the table.*/
    return result;
}

const void* kzcHashMapIteratorGetKey_private(const struct KzcHashMapIterator* iterator)
{
    const void* result;

    if (iterator->map_private->isOrdered)
    {
        result = iterator->data.entry_private->parent.key;
    }
    else
    {
        result = iterator->map_private->table.unordered[iterator->data.tableIndex_private].key;
    }

    return result;
}

void* kzcHashMapIteratorGetValue_private(const struct KzcHashMapIterator* iterator)
{
    void* result;

    if (iterator->map_private->isOrdered)
    {
        result = iterator->data.entry_private->parent.value;
    }
    else
    {
        result = iterator->map_private->table.unordered[iterator->data.tableIndex_private].value;
    }

    return result;
}
