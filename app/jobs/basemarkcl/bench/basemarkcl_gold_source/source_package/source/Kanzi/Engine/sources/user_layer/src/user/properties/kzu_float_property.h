/**
* \file
* Specifies float property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_FLOAT_PROPERTY_H
#define KZU_FLOAT_PROPERTY_H


#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuFloatProperty
* Structure for float property.
*/
struct KzuFloatProperty;
/** 
* \struct KzuFloatPropertyType
* Structure for float property type. 
*/
struct KzuFloatPropertyType;


/** Creates a new float property type. */
kzsError kzuFloatPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, 
                                    kzFloat defaultValue, struct KzuFloatPropertyType** out_propertyType);
/** Deletes float property type. */
kzsError kzuFloatPropertyTypeDelete(struct KzuFloatPropertyType* propertyType);


/** Returns float property type from property type. */
struct KzuFloatPropertyType* kzuFloatPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Returns property type from float property type. */
struct KzuPropertyType* kzuFloatPropertyTypeToPropertyType(struct KzuFloatPropertyType* propertyType);

/** Gets the default value of float property type. */
kzFloat kzuFloatPropertyTypeGetDefaultValue(const struct KzuFloatPropertyType* propertyType);


/** Creates a new float property. */
kzsError kzuFloatPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuFloatPropertyType* propertyType, 
                               kzFloat value, struct KzuFloatProperty** out_property);
/** Deletes float property. */
kzsError kzuFloatPropertyDelete(struct KzuFloatProperty* property);


/** Sets float property relative. */
void kzuFloatPropertySetRelative(struct KzuFloatProperty* property, kzBool relative);
/** Is float property relative. */
kzBool kzuFloatPropertyIsRelative(const struct KzuFloatProperty* property);

/** Returns a value of float property. */
kzFloat kzuFloatPropertyGetValue(const struct KzuFloatProperty* floatProperty);
/** Returns relative a value of float property. */
kzFloat kzuFloatPropertyGetRelativeValue(const struct KzuFloatProperty* floatProperty);

/** Sets value for float property. */
void kzuFloatPropertySetValue(struct KzuFloatProperty* floatProperty, kzFloat value);
/** Sets relative base value for float property. */
void kzuFloatPropertySetRelativeValue(struct KzuFloatProperty* floatProperty, kzFloat value);

/** Returns property from float property. */
struct KzuProperty* kzuFloatPropertyToProperty(struct KzuFloatProperty* property);
/** Returns float property from property. */
struct KzuFloatProperty* kzuFloatPropertyFromProperty(const struct KzuProperty* property);


#endif
