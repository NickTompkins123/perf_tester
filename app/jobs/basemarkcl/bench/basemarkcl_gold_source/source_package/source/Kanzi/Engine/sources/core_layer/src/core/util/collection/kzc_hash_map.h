/**
 * \file
 * Hash map. Hash map stores arbitrary key -> value mappings.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_HASH_MAP_H
#define KZC_HASH_MAP_H


#include "kzc_hash_code.h"
#include "kzc_comparator.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzcHashMapOrderedEntry;


/**
 * \struct KzcHashMap
 * Structure of a HashMap.
 */
struct KzcHashMap;

/**
 * Accessor to the contents of a HashMap
 *
 * Usage: 
 *     struct KzcHashMapIterator it = KzcHashMapGetIterator(map);
 *     while (KzcHashMapIterate(it))
 *     {
 *         struct SomeKey* key = (struct SomeKey*)kzcHashMapIteratorGetKey(it); (Optional)
 *         struct SomeStruct* value = (struct SomeStruct*)kzcHashMapIteratorGetValue(it);
 *         somestruct_some_operation(value);
 *         ... etc ...
 *     }
 */
struct KzcHashMapIterator
{
    const struct KzcHashMap* map_private; /**< Reference to the hash map accessed by this iterator. */
    union
    {
        kzInt tableIndex_private;
        struct KzcHashMapOrderedEntry* entry_private;
    } data; /**< Used for tracking position of the iterator. */
};

/**
 * Configuration parameters for a HashMap. The configuration specifies what type of keys are used in the hash map.
 * The most common configurations are KZC_HASH_MAP_CONFIGURATION_POINTER, if the keys are arbitrary pointers and
 * KZC_HASH_MAP_CONFIGURATION_STRING, if the keys are strings.
 * Custom configurations can also be created for other key types such as structs by providing required callbacks.
 *
 * \see KZC_HASH_MAP_CONFIGURATION_INT
 * \see KZC_HASH_MAP_CONFIGURATION_U32
 * \see KZC_HASH_MAP_CONFIGURATION_POINTER
 * \see KZC_HASH_MAP_CONFIGURATION_STRING
 */
struct KzcHashMapConfiguration
{
    KzcHashFunction hashFunction; /**< Returns a hash code for the specified key. */
    KzcComparatorFunction keyComparator; /**< Comparator for the key. */
};


/** Configuration for hash maps where key type is integer. */
extern const struct KzcHashMapConfiguration KZC_HASH_MAP_CONFIGURATION_INT;
/** Configuration for hash maps where key type is a pointer to unsigned 32 bit integer. */
extern const struct KzcHashMapConfiguration KZC_HASH_MAP_CONFIGURATION_U32;
/** Configuration for hash maps where key type is arbitrary pointer. */
extern const struct KzcHashMapConfiguration KZC_HASH_MAP_CONFIGURATION_POINTER;
/** Configuration for hash maps where key type is a pointer to string. */
extern const struct KzcHashMapConfiguration KZC_HASH_MAP_CONFIGURATION_STRING;


/** Creates a new initially empty hash map. Stores key-value pairs. Initial capacity is given as a parameter. configuration specifies what type of keys are used in the map. */
kzsError kzcHashMapCreateWithCapacity(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                      kzUint capacity, struct KzcHashMap** out_hashMap);

/** Creates a new initially empty hash map. Stores key-value pairs. configuration specifies what type of keys are used in the map. */
kzsError kzcHashMapCreate(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                          struct KzcHashMap** out_hashMap);

/** Creates a new initially empty hash map, which ensures that the iteration order is same as the order of adding elements. Stores key-value pairs. Initial capacity is given as a parameter. configuration specifies what type of keys are used in the map. */
kzsError kzcHashMapCreateOrderedWithCapacity(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                             kzUint capacity, struct KzcHashMap** out_hashMap);

/** Creates a new initially empty hash map, which ensures that the iteration order is same as the order of adding elements. configuration specifies what type of keys are used in the map. */
kzsError kzcHashMapCreateOrdered(const struct KzcMemoryManager* memoryManager, struct KzcHashMapConfiguration configuration,
                                 struct KzcHashMap** out_hashMap);

/** Frees the memory allocated for the hash map. */
kzsError kzcHashMapDelete(struct KzcHashMap* hashMap);


/** Clears the content of the hash map. */
void kzcHashMapClear(struct KzcHashMap* hashMap);

/** Get hash map configuration. */
struct KzcHashMapConfiguration kzcHashMapGetConfiguration(const struct KzcHashMap* hashMap);

/** Add entry to a hash map. In case of key collisions the existing entry is replaced. */
kzsError kzcHashMapPut(struct KzcHashMap* hashMap, const void* key, void* value);

/** Remove entry with the given key from hash map. If key does not exist, an error is returned. */
kzsError kzcHashMapRemove(struct KzcHashMap* hashMap, const void* key);

/** Get the value associated with the specified key from the hash map. If the key is not found, KZ_FALSE is returned and out_value is set to KZ_NULL. */
kzBool kzcHashMapGet(const struct KzcHashMap* hashMap, const void* key, void** out_value);

/**
 * Get the key stored in the hash map corresponding the key given as parameter.
 * Return KZ_NULL if an entry with the given key doesn't exist in the hash map.
 */
const void* kzcHashMapGetStoredKey(const struct KzcHashMap* hashMap, const void* key);

/** Check if the specified key exists in the hash map. Return true if the key is found and false otherwise. */
kzBool kzcHashMapContains(const struct KzcHashMap* hashMap, const void* key);

/** Get the specified value from the hash map. Return error if the key is not found. */
kzsError kzcHashMapGetOrThrowError(const struct KzcHashMap* hashMap, const void* key, void** out_value);

/** Returns the number of entries stored in the hash map. */
kzUint kzcHashMapGetSize(const struct KzcHashMap* HashMap);

/** Checks if the hash map is empty or not. */
kzBool kzcHashMapIsEmpty(const struct KzcHashMap* hashMap);


/** Returns an iterator to the hash map. Call kzcHashMapIterate before accsessing the first and any subsequent entries. */
struct KzcHashMapIterator kzcHashMapGetIterator(const struct KzcHashMap* hashMap);

/** Finds the next entry in the attached hash map. Returns KZ_TRUE if next entry is found, otherwise KZ_FALSE. */
#define kzcHashMapIterate(iterator_param) kzcHashMapIterate_private(&iterator_param)
/** Returns the key of the hash map entry pointed by the iterator. */
#define kzcHashMapIteratorGetKey(iterator_param) kzcHashMapIteratorGetKey_private(&iterator_param)
/** Returns the value of the hash map entry pointed by the iterator. */
#define kzcHashMapIteratorGetValue(iterator_param) kzcHashMapIteratorGetValue_private(&iterator_param)

/** \see kzcHashMapIterate */
kzBool kzcHashMapIterate_private(struct KzcHashMapIterator* iterator);
/** \see kzcHashMapIteratorGetKey */
const void* kzcHashMapIteratorGetKey_private(const struct KzcHashMapIterator* iterator);
/** \see kzcHashMapIteratorGetValue */
void* kzcHashMapIteratorGetValue_private(const struct KzcHashMapIterator* iterator);


#endif
