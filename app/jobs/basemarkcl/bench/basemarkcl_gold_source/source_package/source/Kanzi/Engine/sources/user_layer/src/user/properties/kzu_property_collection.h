/**
* \file
* Property collection.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROPERTY_COLLECTION_H
#define KZU_PROPERTY_COLLECTION_H


#include <core/util/collection/kzc_dynamic_array.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;
struct KzcDynamicArray;


/**
 * \struct KzuPropertyCollection
 * Contains list of properties.
 */
struct KzuPropertyCollection;

/**
 * \struct KzuPropertyTypeCollection
 * Contains list of property types.
 */
struct KzuPropertyTypeCollection;


/** Creates new property collection. */
kzsError kzuPropertyCollectionCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyCollection** out_collection);
/** Creates a new property collection by copying the properties from given collection.. */
kzsError kzuPropertyCollectionCreateCopy(const struct KzcMemoryManager* memoryManager, const struct KzuPropertyCollection* original,
                                         struct KzuPropertyCollection** out_collection);
/** Copies property collection to another. The new collection will hold references to the original properties. */
kzsError kzuPropertyCollectionCreateShallowCopy(const struct KzcMemoryManager* memoryManager, const struct KzuPropertyCollection* original,
                                                struct KzuPropertyCollection** out_copy);

/** Deletes property collection and properties associated to it. */
kzsError kzuPropertyCollectionDelete(struct KzuPropertyCollection* collection);


/** Copies property collection to another. */
kzsError kzuPropertyCollectionCopy(const struct KzcMemoryManager* memoryManager, const struct KzuPropertyCollection* sourceCollection,
                                   const struct KzuPropertyCollection* targetCollection);
/** Copies part of property collection to another. Copies only properties which are set are not loaded from KZB. */
kzsError kzuPropertyCollectionCopyCustom(const struct KzcMemoryManager* memoryManager, const struct KzuPropertyCollection* sourceCollection,
                                         const struct KzuPropertyCollection* targetCollection);
/**
 * Copies the property values from the source collection to the target collection if the same property type exists in the target collection.
 * This function may cause memory allocations in the case of string properties.
 */
kzsError kzuPropertyCollectionCopyValues(const struct KzuPropertyCollection* sourceCollection, const struct KzuPropertyCollection* targetCollection);


/** Gets amount of stored properties from the property collection. */
kzUint kzuPropertyCollectionGetSize(const struct KzuPropertyCollection* collection);

/** Adds property to property collection. */
kzsError kzuPropertyCollectionAddProperty(const struct KzuPropertyCollection* collection, struct KzuProperty* property);
/** Removes property from property collection. */
kzsError kzuPropertyCollectionRemoveProperty(const struct KzuPropertyCollection* collection, const struct KzuProperty* property);
/** Gets iterator for property collection. */
struct KzcDynamicArrayIterator kzuPropertyCollectionGetIterator(const struct KzuPropertyCollection* collection);
/** Gets mutable iterator for property collection. */
struct KzcDynamicArrayMutableIterator kzuPropertyCollectionGetMutableIterator(const struct KzuPropertyCollection* collection);
/** Finds a property by its type's name. KZ_NULL, if no property was found. */
struct KzuProperty* kzuPropertyCollectionGetProperty(const struct KzuPropertyCollection* collection, kzString propertyName);
/** Find a property by its property type. KZ_NULL if no property was found. */
struct KzuProperty* kzuPropertyCollectionGetPropertyByType(const struct KzuPropertyCollection* collection,
                                                              const struct KzuPropertyType* propertyTypeToBeFound);
/** Gets property from given index. KZ_NULL, if no property was found. */
struct KzuProperty* kzuPropertyCollectionGetPropertyAtIndex(const struct KzuPropertyCollection* collection, kzUint index);

/** Creates new property type collection. */
kzsError kzuPropertyTypeCollectionCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyTypeCollection** out_collection);
/** Deletes property type collection and properties associated to it. */
kzsError kzuPropertyTypeCollectionDelete(struct KzuPropertyTypeCollection* collection);
/** Adds property type to property type collection. */
kzsError kzuPropertyTypeCollectionAddPropertyType(const struct KzuPropertyTypeCollection* collection, struct KzuPropertyType* propertyType);
/** Adds another property type collection property types to destionation property collection. */
kzsError kzuPropertyTypeCollectionAddCollection(const struct KzuPropertyTypeCollection* collection, const struct KzuPropertyTypeCollection* collectionToAdd);
/** Returns iterator of property type collection elements. */
struct KzcDynamicArrayIterator kzuPropertyTypeCollectionGetIterator(const struct KzuPropertyTypeCollection* collection);
/** Returns list of property types from property type collection. */
struct KzcDynamicArray* kzuPropertyTypeCollectionGetPropertyTypes(const struct KzuPropertyTypeCollection* collection);
/** Gets property type from property type collection by name. KZ_NULL if not found. */
struct KzuPropertyType* kzuPropertyTypeCollectionGetPropertyTypeByName(const struct KzuPropertyTypeCollection* collection, kzString propertyTypeName);

/** Gets the number of stored property types from the property type collection. */
kzUint kzuPropertyTypeCollectionGetSize(const struct KzuPropertyTypeCollection* collection);


#endif
