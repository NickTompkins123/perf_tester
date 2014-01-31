/**
 * \file
 * Dynamic array. Automatically growing array implementation of arbitrary data.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_DYNAMIC_ARRAY_H
#define KZC_DYNAMIC_ARRAY_H


#include "kzc_comparator.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;


/**
 * \struct KzcDynamicArray
 * Structure of a DynamicArray.
 */
struct KzcDynamicArray;


/** Read-only iterator for dynamic array. */
struct KzcDynamicArrayIterator
{
    void* const* elements_private; /**< Array of the elements. */
    kzUint elementCount_private; /**< Number of elements. */
    kzInt currentIndex_private; /**< Current index in the iterator. */
};

/** Mutable iterator for dynamic array. */
struct KzcDynamicArrayMutableIterator
{
    struct KzcDynamicArray* dynamicArray_private; /**< Reference to the dynamic array accessed by this iterator. */
    kzInt currentIndex_private; /**< Current index in the iterator. */
};


/** Creates a new initially empty dynamic array. */
kzsError kzcDynamicArrayCreate(const struct KzcMemoryManager* memoryManager, struct KzcDynamicArray** out_dynamicArray);

/** Creates a new initially empty dynamic array. Initial capacity is given as a parameter. */
kzsError kzcDynamicArrayCreateWithCapacity(const struct KzcMemoryManager* memoryManager, kzUint capacity,
                                           struct KzcDynamicArray** out_dynamicArray);

/** Frees the memory allocated for the dynamic array. */
kzsError kzcDynamicArrayDelete(struct KzcDynamicArray* dynamicArray);

/** Creates copy of a dynamic array */
kzsError kzcDynamicArrayCopy(const struct KzcMemoryManager* memoryManager, const struct KzcDynamicArray* original, struct KzcDynamicArray** out_copy);

/** Adds the specified element to the dynamic array. */
kzsError kzcDynamicArrayAdd(struct KzcDynamicArray* dynamicArray, void* element);

/** Removes all the elements from the dynamic array. */
void kzcDynamicArrayClear(struct KzcDynamicArray* dynamicArray);

/**
 * Removes the specified element from the dynamic array. Decides equality by simple address comparison.
 * Returns error if element is not found.
 */
kzsError kzcDynamicArrayRemove(struct KzcDynamicArray* dynamicArray, const void* element);

/** Removes the index'th element from the dynamic array. Returns error if index is out of bounds. */
kzsError kzcDynamicArrayRemoveFromIndex(struct KzcDynamicArray* dynamicArray, const kzUint index);

/** Replaces the element at the specified index in the dynamic array. */
void kzcDynamicArrayReplace(const struct KzcDynamicArray* dynamicArray, kzUint index, void* element);

/** Retrieves element from dynamic array at given index. It is illegal and thus undefined behaviour to pass index out of bounds. */
void* kzcDynamicArrayGet(const struct KzcDynamicArray* dynamicArray, kzUint index);

/** Sorts the given dynamic array according to the given comparator function. */
void kzcDynamicArraySort(const struct KzcDynamicArray* dynamicArray, KzcComparatorFunction comparator);

/** Sorts the given dynamic array according to the given comparator function. The given arbitrary context is passed to the comparator. */
void kzcDynamicArraySortWithContext(const struct KzcDynamicArray* dynamicArray, KzcComparatorWithContextFunction comparator,
                                    const void* context);

/** Switches the indices of dynamic array. */
void kzcDynamicArraySwitchIndices(const struct KzcDynamicArray* dynamicArray, kzUint indexA, kzUint indexB);

/**
 * Removes all the specified elements from the dynamic array that are found equal 
 * with elementToCompare by the comparator. Returns the number of elements removed.
 */
kzsError kzcDynamicArrayRemoveIfEqual(struct KzcDynamicArray* dynamicArray, KzcComparatorFunction comparator, const void* const elementToCompare, kzUint* out_removedCount);

/**
 * Finds the first element that that is found equal with elementToCompare by the comparator.
 * Returns a boolean value indicating whether the specified element was found.
 */
kzBool kzcDynamicArrayFindIfEqual(const struct KzcDynamicArray* dynamicArray, KzcComparatorFunction comparator, const void* const elementToCompare, void** out_element);

/** Checks if dynamic array contains specific element. */
kzBool kzcDynamicArrayContains(const struct KzcDynamicArray* dynamicArray, const void* elementToCompare);
/** Finds the first index of the elementToCompare. Returns a boolean value indicating wether the specified element was found. */
kzBool kzcDynamicArrayFindIndex(const struct KzcDynamicArray* dynamicArray, const void* elementToCompare, kzUint* out_index);

/**
 * Finds the index of the first element that is found equal with elementToCompare by the comparator.
 * Returns a boolean value indicating whether the specified element was found.
 */
kzBool kzcDynamicArrayFindIndexIfEqual(const struct KzcDynamicArray* dynamicArray, KzcComparatorFunction comparator, const void* elementToCompare, kzUint* out_index);

/** Inserts the element before index'th element, or, if index is out of bounds to the end of the array. */
kzsError kzcDynamicArrayInsert(struct KzcDynamicArray* dynamicArray, kzUint index, void* elementIn);

/** Returns the number of elements in the dynamic array. */
kzUint kzcDynamicArrayGetSize(const struct KzcDynamicArray* dynamicArray);

/** Checks if the dynamic array is empty or not. */
kzBool kzcDynamicArrayIsEmpty(const struct KzcDynamicArray* dynamicArray);

/** Creates a new array from the content of dynamic array. Returns an array of void pointers. */
kzsError kzcDynamicArrayToArray(const struct KzcMemoryManager* memoryManager, const struct KzcDynamicArray* dynamicArray,
                                void*** out_array);


/** Returns an iterator for the dynamic array. */
struct KzcDynamicArrayIterator kzcDynamicArrayGetIterator(const struct KzcDynamicArray* dynamicArray);

/** Finds the next entry in the attached dynamic array. Returns KZ_TRUE if next entry is found, otherwise KZ_FALSE. */
#define kzcDynamicArrayIterate(iterator_param) ((kzUint)(++(iterator_param).currentIndex_private) < (iterator_param).elementCount_private)
/** Returns the value of the dynamic array entry pointed by the iterator. */
#define kzcDynamicArrayIteratorGetValue(iterator_param) ((iterator_param).elements_private[(iterator_param).currentIndex_private])


/** Returns a mutable iterator for the dynamic array. */
struct KzcDynamicArrayMutableIterator kzcDynamicArrayGetMutableIterator(struct KzcDynamicArray* dynamicArray);

/** Finds the next entry in the attached dynamic array. Returns KZ_TRUE if next entry is found, otherwise KZ_FALSE. */
#define kzcDynamicArrayMutableIterate(iterator_param) kzcDynamicArrayMutableIterate_private(&iterator_param)
/** Returns the value of the dynamic array entry pointed by the iterator. */
#define kzcDynamicArrayMutableIteratorGetValue(iterator_param) kzcDynamicArrayMutableIteratorGetValue_private(&iterator_param)
/** Removes current entry of the dynamic array pointed by the iterator. */
#define kzcDynamicArrayMutableIteratorRemove(iterator_param) kzcDynamicArrayMutableIteratorRemove_private(&iterator_param)

/** \see kzcDynamicArrayMutableIterate */
kzBool kzcDynamicArrayMutableIterate_private(struct KzcDynamicArrayMutableIterator* iterator);
/** \see kzcDynamicArrayMutableIteratorGetValue */
void* kzcDynamicArrayMutableIteratorGetValue_private(const struct KzcDynamicArrayMutableIterator* iterator);
/** \see kzcDynamicArrayMutableIteratorRemove */
kzsError kzcDynamicArrayMutableIteratorRemove_private(struct KzcDynamicArrayMutableIterator* iterator);


#endif
