/**
 * \file
 * Dynamic array.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_dynamic_array.h"

#include "kzc_sort.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/kzc_error_codes.h>

#include <system/wrappers/kzs_memory.h>
#include <system/debug/kzs_counter.h>


/** Structure of a DynamicArray. */
struct KzcDynamicArray
{
    kzUint elementCount; /**< Number of elements in the dynamic array. */
    void** elements; /**< The elements of the dynamic array. */
};


/** Verifies that there is space left for one element and increases the size of the dynamic array if there is none left*/
static kzsError kzcDynamicArrayGrowIfFull_internal(struct KzcDynamicArray* dynamicArray)
{
    kzsError result;
    kzUint currentSize;

    /* no assert - this is called only from where the non-nullness of dynamicArray pointer has been verified */

    currentSize = kzcArrayLength(dynamicArray->elements);

    if (dynamicArray->elementCount >= currentSize)
    {
        void** currentElements = dynamicArray->elements;
        void** newElements;
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(currentElements);
        const kzUint GROWTH_SCALE = 2;
        const kzUint newSize = currentSize * GROWTH_SCALE;
        kzUint i;

        result = kzcMemoryAllocArray(memoryManager, newElements, newSize, "DynamicArray elements");
        kzsErrorForward(result);

        for (i = 0; i < currentSize; ++i)
        {
            newElements[i] = currentElements[i];
        }

        result = kzcMemoryFreeArray(currentElements);
        kzsErrorForward(result);

        dynamicArray->elements = newElements;
    }

    kzsSuccess();
}

/** Allocates memory for the dynamic array. */
static kzsError kzcDynamicArrayInitialize_internal(struct KzcDynamicArray* dynamicArray, kzUint size)
{
    kzsError result;

    result = kzcMemoryAllocArray(kzcMemoryGetManager(dynamicArray), dynamicArray->elements, size, "DynamicArray elements");
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcDynamicArrayCreate(const struct KzcMemoryManager* memoryManager, struct KzcDynamicArray** out_dynamicArray)
{
    kzsError result;
    struct KzcDynamicArray* dynamicArray;

    result = kzcDynamicArrayCreateWithCapacity(memoryManager, 10, &dynamicArray);
    kzsErrorForward(result);

    *out_dynamicArray = dynamicArray;
    kzsSuccess();
}

kzsError kzcDynamicArrayCreateWithCapacity(const struct KzcMemoryManager* memoryManager, kzUint capacity,
                                           struct KzcDynamicArray** out_dynamicArray)
{
    kzsError result;
    struct KzcDynamicArray* dynamicArray;

    result = kzcMemoryAllocVariable(memoryManager, dynamicArray, "Dynamic array");
    kzsErrorForward(result);

    dynamicArray->elementCount = 0;
    result = kzcDynamicArrayInitialize_internal(dynamicArray, capacity);
    kzsErrorForward(result);

    kzsCounterIncrease("kzcDynamicArrayCreateWithCapacity");
    *out_dynamicArray = dynamicArray;
    kzsSuccess();
}

kzsError kzcDynamicArrayDelete(struct KzcDynamicArray* dynamicArray)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    result = kzcMemoryFreeArray(dynamicArray->elements);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(dynamicArray);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcDynamicArrayCopy(const struct KzcMemoryManager* memoryManager, const struct KzcDynamicArray* original, struct KzcDynamicArray** out_copy)
{
    kzsError result;
    struct KzcDynamicArray* copy;
    kzUint i;

    kzsAssert(kzcIsValidPointer(original));
    
    result = kzcMemoryAllocVariable(memoryManager, copy, "DynamicArray");
    kzsErrorForward(result);

    copy->elementCount = original->elementCount;
    result = kzcDynamicArrayInitialize_internal(copy, kzcArrayLength(original->elements));
    kzsErrorForward(result);

    for (i = 0; i < original->elementCount; ++i)
    {
        copy->elements[i] = original->elements[i];
    }

    *out_copy = copy;
    kzsSuccess();
}

kzsError kzcDynamicArrayAdd(struct KzcDynamicArray* dynamicArray, void* element)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    result = kzcDynamicArrayGrowIfFull_internal(dynamicArray);
    kzsErrorForward(result);

    dynamicArray->elements[dynamicArray->elementCount++] = element;
    kzsCounterIncrease("kzcDynamicArrayAdd");

    kzsSuccess();
}

void kzcDynamicArrayClear(struct KzcDynamicArray* dynamicArray)
{
    kzUint i;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    for (i = 0; i < dynamicArray->elementCount; ++i)
    {
        dynamicArray->elements[i] = KZ_NULL;
    }
    dynamicArray->elementCount = 0;
}

kzsError kzcDynamicArrayRemove(struct KzcDynamicArray* dynamicArray, const void* element)
{
    kzsError result;
    kzUint index;
    kzBool indexFound;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    indexFound = kzcDynamicArrayFindIndex(dynamicArray, element, &index);
    kzsErrorTest(indexFound, KZC_ERROR_ELEMENT_NOT_FOUND, "Element to be removed not found in DynamicArray");

    result = kzcDynamicArrayRemoveFromIndex(dynamicArray, index);
    kzsErrorForward(result);

    kzsCounterIncrease("kzcDynamicArrayRemove");

    kzsSuccess();
}

kzsError kzcDynamicArrayRemoveFromIndex(struct KzcDynamicArray* dynamicArray, const kzUint index)
{
    kzUint i;
    void** elements;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    elements = dynamicArray->elements;

    kzsErrorTest(index < dynamicArray->elementCount, KZS_ERROR_ARRAY_OUT_OF_BOUNDS, "Index of element to be removed out of bounds");

    --dynamicArray->elementCount;

    /* Move all the elements after the index of removal one element towards the array head. */
    for (i = index; i < dynamicArray->elementCount; ++i)
    {
        elements[i] = elements[i + 1];
    }
    elements[i] = KZ_NULL;

    kzsCounterIncrease("kzcDynamicArrayRemoveFromIndex");
    kzsSuccess();
}

void kzcDynamicArrayReplace(const struct KzcDynamicArray* dynamicArray, kzUint index, void* element)
{
    kzsAssert(kzcIsValidPointer(dynamicArray));
    dynamicArray->elements[index] = element;
}

void* kzcDynamicArrayGet(const struct KzcDynamicArray* dynamicArray, kzUint index)
{
    void* result;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    if (index >= kzcArrayLength(dynamicArray->elements))
    {
        kzsAssertText(KZ_FALSE, "Dynamic array index out of bounds");
        result = KZ_NULL;
    }
    else
    {
        result = dynamicArray->elements[index];
    }

    return result;
}

void kzcDynamicArraySort(const struct KzcDynamicArray* dynamicArray, KzcComparatorFunction comparator)
{
    kzsAssert(kzcIsValidPointer(dynamicArray));

    kzcSortArray(dynamicArray->elements, dynamicArray->elementCount, comparator);
}

void kzcDynamicArraySortWithContext(const struct KzcDynamicArray* dynamicArray, KzcComparatorWithContextFunction comparator,
                                    const void* context)
{
    kzsAssert(kzcIsValidPointer(dynamicArray));

    kzcSortArrayWithContext(dynamicArray->elements, dynamicArray->elementCount, comparator, context);
}

void kzcDynamicArraySwitchIndices(const struct KzcDynamicArray* dynamicArray, kzUint indexA, kzUint indexB)
{
    void* tempElement;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    tempElement = dynamicArray->elements[indexA];
    dynamicArray->elements[indexA] = dynamicArray->elements[indexB];
    dynamicArray->elements[indexB] = tempElement;
}

kzsError kzcDynamicArrayRemoveIfEqual(struct KzcDynamicArray* dynamicArray, KzcComparatorFunction comparator,
                                      const void* const elementToCompare, kzUint* out_removedCount)
{
    kzsError result;
    kzUint arrayIndex;
    kzBool iterateOverElements;
    void** elements;
    kzUint numRemoved = 0;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    elements = dynamicArray->elements;

    arrayIndex = 0;
    iterateOverElements = (arrayIndex < dynamicArray->elementCount);

    while (iterateOverElements)
    {
        if (comparator(elementToCompare, elements[arrayIndex]) != 0)
        {
          /* If elements are not equal, just pass on to the next element */
            ++arrayIndex;
        }
        else
        {
            /* Remove matching element.*/
            result = kzcDynamicArrayRemoveFromIndex(dynamicArray, arrayIndex);
            kzsErrorForward(result);

            /* Increment removal counter and reassign elements-reference.*/
            ++numRemoved;
            elements = dynamicArray->elements;

            /* In case of removal, never move forward */
        }
        iterateOverElements = (arrayIndex < dynamicArray->elementCount);
    }

    *out_removedCount = numRemoved;
    kzsSuccess();
}

kzBool kzcDynamicArrayFindIfEqual(const struct KzcDynamicArray* dynamicArray,
                                  KzcComparatorFunction comparator,
                                  const void* const elementToCompare, 
                                  void** out_element)
{
    kzUint size;
    kzUint i;
    void** elements;
    kzBool elementFound = KZ_FALSE;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    size = dynamicArray->elementCount;
    elements = dynamicArray->elements;

    for (i = 0; i < size; ++i)
    {
        if (comparator(elementToCompare, elements[i]) == 0)
        {
            *out_element = elements[i];
            elementFound = KZ_TRUE;
            break;
        }
    }

    return elementFound;
}

kzBool kzcDynamicArrayContains(const struct KzcDynamicArray* dynamicArray, const void* elementToCompare)
{
    kzUint size;
    kzUint i;
    void** elements;
    kzBool indexFound = KZ_FALSE;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    size = dynamicArray->elementCount;
    elements = dynamicArray->elements;

    for (i = 0; i < size; ++i)
    {
        if (elementToCompare == elements[i])
        {
            indexFound = KZ_TRUE;
            break;
        }
    }

    return indexFound;
}

kzBool kzcDynamicArrayFindIndex(const struct KzcDynamicArray* dynamicArray, const void* elementToCompare, kzUint* out_index)
{
    kzUint size;
    kzUint i;
    void** elements;
    kzBool indexFound = KZ_FALSE;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    size = dynamicArray->elementCount;
    elements = dynamicArray->elements;

    for (i = 0; i < size; ++i)
    {
        if (elementToCompare == elements[i])
        {
            *out_index = i;
            indexFound = KZ_TRUE;
            break;
        }
    }

    return indexFound;
}

kzBool kzcDynamicArrayFindIndexIfEqual(const struct KzcDynamicArray* dynamicArray,
                                       KzcComparatorFunction comparator,
                                       const void* elementToCompare,
                                       kzUint* out_index)
{
    kzUint size;
    kzUint i;
    void** elements;
    kzBool foundIndex = KZ_FALSE;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    size = dynamicArray->elementCount;
    elements = dynamicArray->elements;

    for (i = 0; i < size; ++i)
    {
        if (comparator(elementToCompare, elements[i]) == 0)
        {
            *out_index = i;
            foundIndex = KZ_TRUE;
            break;
        }
    }

    return foundIndex;
}

kzsError kzcDynamicArrayInsert(struct KzcDynamicArray* dynamicArray, kzUint index, void* elementIn)
{
    kzUint i;
    kzUint end;
    void** elements;
    kzsError result;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    result = kzcDynamicArrayGrowIfFull_internal(dynamicArray);
    kzsErrorForward(result);    
    
    dynamicArray->elementCount++;

    index = (index < dynamicArray->elementCount) ? index : dynamicArray->elementCount - 1;
    end = dynamicArray->elementCount - 1;

    elements = dynamicArray->elements;
    /* Move array elements one index tailwards from the insertion point */
    for (i = end; i > index; --i)
    {
        elements[i] = elements[i - 1];
    }

    elements[index] = elementIn;

    kzsSuccess();
}

kzUint kzcDynamicArrayGetSize(const struct KzcDynamicArray* dynamicArray)
{
    kzsAssert(kzcIsValidPointer(dynamicArray));

    return dynamicArray->elementCount;
}

kzBool kzcDynamicArrayIsEmpty(const struct KzcDynamicArray* dynamicArray)
{
    kzsAssert(kzcIsValidPointer(dynamicArray));

    return (dynamicArray->elementCount == 0);
}

kzsError kzcDynamicArrayToArray(const struct KzcMemoryManager* memoryManager, const struct KzcDynamicArray* dynamicArray,
                                void*** out_array)
{
    kzsError result;
    void** array;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    result = kzcMemoryAllocArray(memoryManager, array, dynamicArray->elementCount, "Array from dynamic array");
    kzsErrorForward(result);

    kzsMemcpy(array, dynamicArray->elements, dynamicArray->elementCount * sizeof(*array));

    *out_array = array;
    kzsSuccess();
}

struct KzcDynamicArrayIterator kzcDynamicArrayGetIterator(const struct KzcDynamicArray* dynamicArray)
{
    struct KzcDynamicArrayIterator iterator;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    iterator.elements_private = dynamicArray->elements;
    iterator.elementCount_private = dynamicArray->elementCount;
    iterator.currentIndex_private = -1;

    return iterator;
}

struct KzcDynamicArrayMutableIterator kzcDynamicArrayGetMutableIterator(struct KzcDynamicArray* dynamicArray)
{
    struct KzcDynamicArrayMutableIterator iterator;

    kzsAssert(kzcIsValidPointer(dynamicArray));

    iterator.dynamicArray_private = dynamicArray;
    iterator.currentIndex_private = -1;

    return iterator;
}

kzBool kzcDynamicArrayMutableIterate_private(struct KzcDynamicArrayMutableIterator* iterator)
{
    return (kzUint)(++iterator->currentIndex_private) < iterator->dynamicArray_private->elementCount;
}

void* kzcDynamicArrayMutableIteratorGetValue_private(const struct KzcDynamicArrayMutableIterator* iterator)
{
    return iterator->dynamicArray_private->elements[iterator->currentIndex_private];
}

kzsError kzcDynamicArrayMutableIteratorRemove_private(struct KzcDynamicArrayMutableIterator* iterator)
{
    kzsError result;

    result = kzcDynamicArrayRemoveFromIndex(iterator->dynamicArray_private, (kzUint)iterator->currentIndex_private);
    kzsErrorForward(result);

    --iterator->currentIndex_private;

    kzsSuccess();
}
