/**
* \file
* Property collection.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_collection.h"

#include "kzu_property_private.h"
#include "kzu_property.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/string/kzc_string.h>


struct KzuPropertyCollection
{
    struct KzcDynamicArray* properties;         /**< List of properties. */
};

struct KzuPropertyTypeCollection
{
    struct KzcDynamicArray* propertyTypes;      /**< List of property types. */
};


kzsError kzuPropertyCollectionCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyCollection** out_collection)
{
    kzsError result;
    struct KzuPropertyCollection* collection;

    result = kzcMemoryAllocVariable(memoryManager, collection, "Property collection");
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &collection->properties);
    kzsErrorForward(result);

    *out_collection = collection;
    kzsSuccess();
}

kzsError kzuPropertyCollectionCreateCopy(const struct KzcMemoryManager* memoryManager, const struct KzuPropertyCollection* original, 
                                         struct KzuPropertyCollection** out_collection)
{
    kzsError result;
    struct KzuPropertyCollection* collection;

    result = kzuPropertyCollectionCreate(memoryManager, &collection);
    kzsErrorForward(result);

    result = kzuPropertyCollectionCopy(memoryManager, original, collection);
    kzsErrorForward(result);

    *out_collection = collection;
    kzsSuccess();
}

kzsError kzuPropertyCollectionCreateShallowCopy(const struct KzcMemoryManager* memoryManager, const struct KzuPropertyCollection* original,
                                                struct KzuPropertyCollection** out_copy)
{
    kzsError result;
    struct KzuPropertyCollection* copy;

    kzsAssert(kzcIsValidPointer(original));

    result = kzcMemoryAllocVariable(memoryManager, copy, "Property collection");
    kzsErrorForward(result);

    result = kzcDynamicArrayCopy(memoryManager, original->properties, &copy->properties);
    kzsErrorForward(result);

    *out_copy = copy;
    kzsSuccess();
}

kzsError kzuPropertyCollectionDelete(struct KzuPropertyCollection* collection)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzcIsValidPointer(collection));

    it = kzcDynamicArrayGetIterator(collection->properties);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuProperty* property = (struct KzuProperty*)kzcDynamicArrayIteratorGetValue(it);
        
        result = kzuPropertyDelete(property);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayDelete(collection->properties);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(collection);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuPropertyCollectionCopy(const struct KzcMemoryManager* memoryManager, const struct KzuPropertyCollection* sourceCollection,
                                   const struct KzuPropertyCollection* targetCollection)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzcIsValidPointer(sourceCollection));

    it = kzcDynamicArrayGetIterator(sourceCollection->properties);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuProperty* existingProperty;
        struct KzuProperty* property = (struct KzuProperty*)kzcDynamicArrayIteratorGetValue(it);
        struct KzuProperty* copyProperty;

        result = kzuPropertyCopy(memoryManager, property, &copyProperty);
        kzsErrorForward(result);

        /* Remove the property from the target if it already exists. */
        existingProperty = kzuPropertyCollectionGetPropertyByType(targetCollection, kzuPropertyGetPropertyType(property));
        if (existingProperty != KZ_NULL)
        {
            result = kzuPropertyDelete(existingProperty);
            kzsErrorForward(result);

            result = kzuPropertyCollectionRemoveProperty(targetCollection, existingProperty);
            kzsErrorForward(result);
        }

        result = kzcDynamicArrayAdd(targetCollection->properties, copyProperty);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuPropertyCollectionCopyCustom(const struct KzcMemoryManager* memoryManager, const struct KzuPropertyCollection* sourceCollection, 
                                         const struct KzuPropertyCollection* targetCollection)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzcIsValidPointer(sourceCollection));
    kzsAssert(kzcIsValidPointer(targetCollection));

    it = kzcDynamicArrayGetIterator(sourceCollection->properties);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuProperty* copyProperty;
        struct KzuProperty* property = (struct KzuProperty*)kzcDynamicArrayIteratorGetValue(it);
        if(!kzuPropertyGetFromKzb(property))
        {
            result = kzuPropertyCopy(memoryManager, property, &copyProperty);
            kzsErrorForward(result);
            result = kzcDynamicArrayAdd(targetCollection->properties, copyProperty);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzuPropertyCollectionCopyValues(const struct KzuPropertyCollection* sourceCollection,
                                         const struct KzuPropertyCollection* targetCollection)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzcIsValidPointer(sourceCollection));

    it = kzcDynamicArrayGetIterator(sourceCollection->properties);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuProperty* sourceProperty = (struct KzuProperty*)kzcDynamicArrayIteratorGetValue(it);
        struct KzuProperty* targetProperty = kzuPropertyCollectionGetPropertyByType(targetCollection, sourceProperty->propertyType);

        result = kzuPropertyCopyValue(targetProperty, sourceProperty);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzUint kzuPropertyCollectionGetSize(const struct KzuPropertyCollection* collection)
{
    kzsAssert(kzcIsValidPointer(collection));
    return kzcDynamicArrayGetSize(collection->properties);
}

/* TODO: Think about not supporting duplicate elements */
kzsError kzuPropertyCollectionAddProperty(const struct KzuPropertyCollection* collection, struct KzuProperty* property)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(collection));
    kzsAssert(kzcIsValidPointer(property));
    kzsAssert(kzuPropertyGetPropertyType(property) != KZ_NULL);

    result = kzcDynamicArrayAdd(collection->properties, property);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuPropertyCollectionRemoveProperty(const struct KzuPropertyCollection* collection, const struct KzuProperty* property)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(collection));

    result = kzcDynamicArrayRemove(collection->properties, property);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcDynamicArrayIterator kzuPropertyCollectionGetIterator(const struct KzuPropertyCollection* collection)
{
    kzsAssert(kzcIsValidPointer(collection));

    return kzcDynamicArrayGetIterator(collection->properties);
}

struct KzcDynamicArrayMutableIterator kzuPropertyCollectionGetMutableIterator(const struct KzuPropertyCollection* collection)
{
    kzsAssert(kzcIsValidPointer(collection));

    return kzcDynamicArrayGetMutableIterator(collection->properties);
}

struct KzuProperty* kzuPropertyCollectionGetProperty(const struct KzuPropertyCollection* collection, kzString propertyName)
{
    struct KzcDynamicArrayIterator it;
    struct KzuProperty* foundProperty = KZ_NULL;

    kzsAssert(kzcIsValidPointer(collection));
    
    it = kzcDynamicArrayGetIterator(collection->properties);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuProperty* property = (struct KzuProperty*)kzcDynamicArrayIteratorGetValue(it);
        struct KzuPropertyType* propertyType = property->propertyType;

        kzsAssert(kzcIsValidPointer(propertyType));

        /* Check if the names equal. */
        if (kzcStringIsEqual(propertyType->data->name, propertyName))
        {
            kzsAssert(kzcIsValidPointer(property));
            foundProperty = property;
            break;
        }
    }
    return foundProperty;
}

struct KzuProperty* kzuPropertyCollectionGetPropertyByType(const struct KzuPropertyCollection* collection,
                                                           const struct KzuPropertyType* propertyType)
{
    struct KzcDynamicArrayIterator it;
    struct KzuProperty* foundProperty = KZ_NULL;

    kzsAssert(kzcIsValidPointer(collection));

    it = kzcDynamicArrayGetIterator(collection->properties);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuProperty* property = (struct KzuProperty*)kzcDynamicArrayIteratorGetValue(it);
        if (property->propertyType == propertyType)
        {
            foundProperty = property;
            break;
        }
    }
    return foundProperty;
}

struct KzuProperty* kzuPropertyCollectionGetPropertyAtIndex(const struct KzuPropertyCollection* collection, kzUint index)
{
    struct KzuProperty* foundProperty;

    kzsAssert(kzcIsValidPointer(collection));

    foundProperty = (struct KzuProperty*)kzcDynamicArrayGet(collection->properties, index);

    return foundProperty;
}

kzsError kzuPropertyTypeCollectionCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyTypeCollection** out_collection)
{
    kzsError result;
    struct KzuPropertyTypeCollection* collection;

    result = kzcMemoryAllocVariable(memoryManager, collection, "Property type collection");
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &collection->propertyTypes);
    kzsErrorForward(result);

    *out_collection = collection;
    kzsSuccess();
}

kzsError kzuPropertyTypeCollectionDelete(struct KzuPropertyTypeCollection* collection)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(collection));

    result = kzcDynamicArrayDelete(collection->propertyTypes);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(collection);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuPropertyTypeCollectionAddPropertyType(const struct KzuPropertyTypeCollection* collection, struct KzuPropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(collection));

    result = kzcDynamicArrayAdd(collection->propertyTypes, propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuPropertyTypeCollectionAddCollection(const struct KzuPropertyTypeCollection* collection, const struct KzuPropertyTypeCollection* collectionToAdd)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzcIsValidPointer(collectionToAdd));

    it = kzcDynamicArrayGetIterator(collectionToAdd->propertyTypes);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuPropertyType* propertyType = (struct KzuPropertyType*)kzcDynamicArrayIteratorGetValue(it);

        result = kzcDynamicArrayAdd(collection->propertyTypes, propertyType);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

struct KzcDynamicArrayIterator kzuPropertyTypeCollectionGetIterator(const struct KzuPropertyTypeCollection* collection)
{
    kzsAssert(kzcIsValidPointer(collection));

    return kzcDynamicArrayGetIterator(collection->propertyTypes);
}

/* TODO: Remove */
struct KzcDynamicArray* kzuPropertyTypeCollectionGetPropertyTypes(const struct KzuPropertyTypeCollection* collection)
{
    kzsAssert(kzcIsValidPointer(collection));

    return collection->propertyTypes;
}

struct KzuPropertyType* kzuPropertyTypeCollectionGetPropertyTypeByName(const struct KzuPropertyTypeCollection* collection, kzString propertyTypeName)
{
    struct KzcDynamicArrayIterator it;
    struct KzuPropertyType* foundPropertyType = KZ_NULL;

    kzsAssert(kzcIsValidPointer(collection));
    
    it = kzcDynamicArrayGetIterator(collection->propertyTypes);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuPropertyType* propertyType = (struct KzuPropertyType*)kzcDynamicArrayIteratorGetValue(it);
        /* Check if the names equal. */
        if (kzcStringIsEqual(propertyType->data->name, propertyTypeName))
        {
            foundPropertyType = propertyType;
            break;
        }
    }
    return foundPropertyType;
}

kzUint kzuPropertyTypeCollectionGetSize(const struct KzuPropertyTypeCollection* collection)
{
    kzsAssert(kzcIsValidPointer(collection));

    return kzcDynamicArrayGetSize(collection->propertyTypes);
}
