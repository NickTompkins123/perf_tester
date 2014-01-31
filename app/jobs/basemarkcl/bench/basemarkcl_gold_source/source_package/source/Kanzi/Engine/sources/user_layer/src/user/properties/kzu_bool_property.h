/**
* \file
* Specifies boolean property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_BOOL_PROPERTY_H
#define KZU_BOOL_PROPERTY_H


#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
 * \struct KzuBoolProperty
 * Structure for bool property. 
 */
struct KzuBoolProperty;
/** 
* \struct KzuBoolPropertyType
* Structure for bool property type. 
*/
struct KzuBoolPropertyType;


/** Creates a property type bool. */
kzsError kzuBoolPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, kzBool defaultValue,
                                   struct KzuBoolPropertyType** out_propertyType);
/** Deletes bool property type. */
kzsError kzuBoolPropertyTypeDelete(struct KzuBoolPropertyType* propertyType);


/** Converts property type to bool property type. */
struct KzuBoolPropertyType* kzuBoolPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Converts bool property type to property type. */
struct KzuPropertyType* kzuBoolPropertyTypeToPropertyType(struct KzuBoolPropertyType* propertyType);

/** Gets the default value of bool property type. */
kzBool kzuBoolPropertyTypeGetDefaultValue(const struct KzuBoolPropertyType* propertyType);


/** Creates a new bool property. */
kzsError kzuBoolPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuBoolPropertyType* propertyType, 
                               kzBool value, struct KzuBoolProperty** out_property);
/** Deletes bool property. */
kzsError kzuBoolPropertyDelete(struct KzuBoolProperty* property);


/** Sets value for bool property. */
void kzuBoolPropertySetValue(struct KzuBoolProperty* boolProperty, kzBool value);
/** Returns a value of bool property. */
kzBool kzuBoolPropertyGetValue(const struct KzuBoolProperty* boolProperty);
/** Converts bool property to property. */
struct KzuProperty* kzuBoolPropertyToProperty(struct KzuBoolProperty* property);
/** Converts property to bool property. */
struct KzuBoolProperty* kzuBoolPropertyFromProperty(const struct KzuProperty* property);


#endif
