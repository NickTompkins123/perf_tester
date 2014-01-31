/**
 * \file
 * Hash set. Hash set stores unique values, whose existence in the set can be quickly queried.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_HASH_SET_H
#define KZC_HASH_SET_H


#include "kzc_hash_code.h"
#include "kzc_comparator.h"
#include "kzc_hash_map.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;


/**
 * \struct KzcHashSet
 * Internal structure of a HashSet.
 */
struct KzcHashSet;

/**
 * Accessor to the contents of a HashSet
 *
 * Usage: 
 *     struct KzcHashSetIterator it = KzcHashSetGetIterator(set);
 *     while (KzcHashSetIterate(it))
 *     {
 *         struct SomeStruct* value = (struct SomeStruct*)kzcHashSetIteratorGetValue(it);
 *         somestruct_some_operation(value);
 *         ... etc ...
 *     }
 */
struct KzcHashSetIterator
{
    struct KzcHashMapIterator mapIterator_private; /**< Hash map iterator corresponding the hash set iterator. */
};


/** Creates a new initially empty hash set. Initial capacity is given as a parameter. */
kzsError kzcHashSetCreateWithCapacity(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                      kzUint capacity, struct KzcHashSet** out_hashSet);

/** Creates a new initially empty hash set. */
kzsError kzcHashSetCreate(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                          struct KzcHashSet** out_hashSet);

/** Creates a new initially empty hash set, which ensures that the iteration order is same as the order of adding elements. Initial capacity is given as a parameter. */
kzsError kzcHashSetCreateOrderedWithCapacity(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                             kzUint capacity, struct KzcHashSet** out_hashSet);

/** Creates a new initially empty hash set, which ensures that the iteration order is same as the order of adding elements. */
kzsError kzcHashSetCreateOrdered(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                 struct KzcHashSet** out_hashSet);

/** Frees the memory allocated for the hash set. */
kzsError kzcHashSetDelete(struct KzcHashSet* hashSet);

/** Clears the content of the hash set. */
void kzcHashSetClear(const struct KzcHashSet* hashSet);

/** Get configuration. */
struct KzcHashMapConfiguration kzcHashSetGetConfiguration(const struct KzcHashSet* hashSet);

/** Add value to a hash set. In case of collision the existing entry is replaced. */
kzsError kzcHashSetAdd(const struct KzcHashSet* hashSet, void* value);

/** Remove given value from hash set. If the value does not exist, an error is returned. */
kzsError kzcHashSetRemove(const struct KzcHashSet* hashSet, const void* value);

/**
 * Get the value stored in the hash set corresponding the value given as parameter.
 * Return KZ_NULL if an entry with the given value doesn't exist in the hash map.
 */
void* kzcHashSetGetStoredValue(const struct KzcHashSet* hashSet, const void* value);

/** Check if the specified value exists in the hash set. Return true if the value is found and false otherwise. */
kzBool kzcHashSetContains(const struct KzcHashSet* hashSet, const void* value);

/** Returns the number of entries stored in the hash set. */
kzUint kzcHashSetGetSize(const struct KzcHashSet* hashSet);

/** Checks if the hash set is empty or not. */
kzBool kzcHashSetIsEmpty(const struct KzcHashSet* hashSet);


/** Returns an iterator to the hash set. Call kzcHashSetIterate before accsessing the first and any subsequent entries. */
struct KzcHashSetIterator kzcHashSetGetIterator(const struct KzcHashSet* hashSet);

/** Finds the next entry in the attached hash set. Returns KZ_TRUE if next entry is found, otherwise KZ_FALSE. */
#define kzcHashSetIterate(iterator_param) kzcHashMapIterate(iterator_param.mapIterator_private)
/** Returns the value of the hash set entry pointed by the iterator. */
#define kzcHashSetIteratorGetValue(iterator_param) kzcHashMapIteratorGetValue(iterator_param.mapIterator_private)


#endif

