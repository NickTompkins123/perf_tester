/**
 * \file
 * Hash set.
 * 
 * Hash set implemented using hash map.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_hash_set.h"

#include <core/memory/kzc_memory_manager.h>


struct KzcHashSet
{
    struct KzcHashMap* hashMap;
};


kzsError kzcHashSetCreateWithCapacity(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                      kzUint capacity, struct KzcHashSet** out_hashSet)
{
    kzsError result;
    struct KzcHashSet* hashSet;

    /* Allocate memory. */
    result = kzcMemoryAllocVariable(memoryManager, hashSet, "HashSet");
    kzsErrorForward(result);

    /* Create hash map. */
    result = kzcHashMapCreateWithCapacity(memoryManager, configuration, capacity, &hashSet->hashMap);
    kzsErrorForward(result);

    *out_hashSet = hashSet;
    kzsSuccess();
}

kzsError kzcHashSetCreate(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                          struct KzcHashSet** out_hashSet)
{
    kzsError result;
    struct KzcHashSet* hashSet;

    /* Allocate memory. */
    result = kzcMemoryAllocVariable(memoryManager, hashSet, "HashSet");
    kzsErrorForward(result);

    /* Create hash map. */
    result = kzcHashMapCreate(memoryManager, configuration, &hashSet->hashMap);
    kzsErrorForward(result);

    *out_hashSet = hashSet;
    kzsSuccess();
}

kzsError kzcHashSetCreateOrderedWithCapacity(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                             kzUint capacity, struct KzcHashSet** out_hashSet)
{
    kzsError result;
    struct KzcHashSet* hashSet;

    /* Allocate memory. */
    result = kzcMemoryAllocVariable(memoryManager, hashSet, "HashSet");
    kzsErrorForward(result);

    /* Create hash map. */
    result = kzcHashMapCreateOrderedWithCapacity(memoryManager, configuration, capacity, &hashSet->hashMap);
    kzsErrorForward(result);

    *out_hashSet = hashSet;
    kzsSuccess();
}

kzsError kzcHashSetCreateOrdered(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                 struct KzcHashSet** out_hashSet)
{
    kzsError result;
    struct KzcHashSet* hashSet;

    /* Allocate memory. */
    result = kzcMemoryAllocVariable(memoryManager, hashSet, "HashSet");
    kzsErrorForward(result);

    /* Create hash map. */
    result = kzcHashMapCreateOrdered(memoryManager, configuration, &hashSet->hashMap);
    kzsErrorForward(result);

    *out_hashSet = hashSet;
    kzsSuccess();
}


kzsError kzcHashSetDelete(struct KzcHashSet* hashSet)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(hashSet));

    /* Free memory used by hash set. */

    result = kzcHashMapDelete(hashSet->hashMap);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(hashSet);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcHashSetClear(const struct KzcHashSet* hashSet)
{
    kzcHashMapClear(hashSet->hashMap);
}

struct KzcHashMapConfiguration kzcHashSetGetConfiguration(const struct KzcHashSet* hashSet)
{
    return kzcHashMapGetConfiguration(hashSet->hashMap);
}

kzsError kzcHashSetAdd(const struct KzcHashSet* hashSet, void* value)
{
    kzsError result;

    result = kzcHashMapPut(hashSet->hashMap, value, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcHashSetRemove(const struct KzcHashSet* hashSet, const void* value)
{
    kzsError result;

    result = kzcHashMapRemove(hashSet->hashMap, value);
    kzsErrorForward(result);

    kzsSuccess();
}

void* kzcHashSetGetStoredValue(const struct KzcHashSet* hashSet, const void* value)
{
    return (void*)kzcHashMapGetStoredKey(hashSet->hashMap, value);
}

kzBool kzcHashSetContains(const struct KzcHashSet* hashSet, const void* value)
{
    return kzcHashMapContains(hashSet->hashMap, value);
}

kzUint kzcHashSetGetSize(const struct KzcHashSet* hashSet)
{
    return kzcHashMapGetSize(hashSet->hashMap);
}

kzBool kzcHashSetIsEmpty(const struct KzcHashSet* hashSet)
{
    return kzcHashMapIsEmpty(hashSet->hashMap);
}

struct KzcHashSetIterator kzcHashSetGetIterator(const struct KzcHashSet* hashSet)
{
    struct KzcHashSetIterator iterator;

    iterator.mapIterator_private = kzcHashMapGetIterator(hashSet->hashMap);

    return iterator;
}
