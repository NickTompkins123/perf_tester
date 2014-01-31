/**
* \file
* Specifies integer property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_INT_PROPERTY_H
#define KZU_INT_PROPERTY_H


#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuIntProperty
* Structure for int property.
*/
struct KzuIntProperty;
/** 
* \struct KzuIntPropertyType
* Structure for int property type.
*/
struct KzuIntPropertyType;


/** Creates a property type integer. */
kzsError kzuIntPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, kzInt defaultValue,
                                  struct KzuIntPropertyType** out_propertyType);
/** Deletes int property type. */
kzsError kzuIntPropertyTypeDelete(struct KzuIntPropertyType* propertyType);


/** Returns int property type from property type. */
struct KzuIntPropertyType* kzuIntPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Return property type from int property type. */
struct KzuPropertyType* kzuIntPropertyTypeToPropertyType(struct KzuIntPropertyType* intPropertyType);

/** Gets the default value of int property type. */
kzInt kzuIntPropertyTypeGetDefaultValue(const struct KzuIntPropertyType* propertyType);


/** Creates a new int property. */
kzsError kzuIntPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuIntPropertyType* propertyType, 
                              kzInt value, struct KzuIntProperty** out_property);
/** Deletes int property. */
kzsError kzuIntPropertyDelete(struct KzuIntProperty* property);

/** Converts int property to property. */
struct KzuProperty* kzuIntPropertyToProperty(struct KzuIntProperty* intProperty);
/** Returns int property from property. */
struct KzuIntProperty* kzuIntPropertyFromProperty(const struct KzuProperty* property);

/** Returns a value of int property. */
kzInt kzuIntPropertyGetValue(const struct KzuIntProperty* intProperty);
/** Sets value for int property. */
void kzuIntPropertySetValue(struct KzuIntProperty* intProperty, kzInt value);


#endif
