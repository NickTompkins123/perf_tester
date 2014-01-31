/**
* \file
* Specifies property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROPERTY_H
#define KZU_PROPERTY_H


#include <user/properties/kzu_property_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzcMemoryManager;
struct KzcDynamicArray;


/**
 * \struct KzuProperty
 * Structure for property.
 */
struct KzuProperty;

/**
 * \struct KzuPropertyType
 * Structure for property type. Contains ID, datatype and reference value.
 */
struct KzuPropertyType;


/** Deletes a property. */
kzsError kzuPropertyDelete(const struct KzuProperty* property);

/** Creates a copy of given property. */
kzsError kzuPropertyCopy(const struct KzcMemoryManager* memoryManager, const struct KzuProperty* sourceProperty, struct KzuProperty** out_property);
/** Copies all property data from given source property to the target property. Allocates memory if needed. Uses the memory manager used to create the target property. Throws error if the properties are not of same type. */
kzsError kzuPropertyCopyContent(struct KzuProperty* targetProperty, const struct KzuProperty* sourceProperty);
/** Copies property value from given source property to the target property. */
kzsError kzuPropertyCopyValue(const struct KzuProperty* targetProperty, const struct KzuProperty* sourceProperty);

/** Sets a priority for property. */
void kzuPropertySetPriority(struct KzuProperty* property, enum KzuPropertyPriority priority);
/** Gets a priority of a property. */
enum KzuPropertyPriority kzuPropertyGetPriority(const struct KzuProperty* property);
/** Returns property type from property. */
struct KzuPropertyType* kzuPropertyGetPropertyType(const struct KzuProperty* property);


/** Deletes a property type. */
kzsError kzuPropertyTypeDelete(struct KzuPropertyType* propertyType);
/** Returns property type's name. */
kzString kzuPropertyTypeGetName(const struct KzuPropertyType* propertyType);
/** Gets data type of property. */
enum KzuPropertyDataType kzuPropertyTypeGetDataType(const struct KzuPropertyType* propertyType);
/** Sets the inheritance behavior of the property type. */
void kzuPropertyTypeSetInherited(const struct KzuPropertyType* propertyType, kzBool inherited);
/** Gets the inheritance behavior of the property type. */
kzBool kzuPropertyTypeGetInherited(const struct KzuPropertyType* propertyType);

/** Returns true if property is added to object through KZB. */
kzBool kzuPropertyGetFromKzb(const struct KzuProperty* property);
/** Sets given property fromKzb state to true. */
void kzuPropertySetFromKzb(struct KzuProperty* property);
/** Sets given property fromKzb state to false. */
void kzuPropertyDisableFromKzb(struct KzuProperty* property);

/** Sets flags for the property. */
void kzuPropertySetFlags(struct KzuProperty* property, kzU32 flags);

/** Returns KZ_TRUE if the property is inherited. Note that this may return KZ_TRUE indirectly from the property type even if the property itself is not set to inherited. */
kzBool kzuPropertyIsInherited(const struct KzuProperty* property);
/** Overrides the inheritance behavior defined in the type for the property. */
void kzuPropertySetInherited(struct KzuProperty* property, kzBool inherited);


/**
 * Takes ownership of the property type container deletion. After this, when a property type is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuPropertyTypeTakeOwnership(const struct KzuPropertyType* propertyType);
/** Checks if the object node is valid (pointer and the data are non-null). */
kzBool kzuPropertyTypeIsValid(const struct KzuPropertyType* propertyType);
/** Moves the data from source object node to the target object node. */
void kzuPropertyTypeTransferData(struct KzuPropertyType* targetPropertyType, struct KzuPropertyType* sourcePropertyType);


#endif
