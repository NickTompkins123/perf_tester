/**
* \file
* Specifies string property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_STRING_PROPERTY_H
#define KZU_STRING_PROPERTY_H


#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuStringProperty
* Structure for string property.
*/
struct KzuStringProperty;
/** 
* \struct KzuStringPropertyType
* Structure for string property type.
*/
struct KzuStringPropertyType;


/** Creates a new property type string. */
kzsError kzuStringPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, kzString defaultValue,
                                     struct KzuStringPropertyType** out_propertyType);
/** Deletes string property type. */
kzsError kzuStringPropertyTypeDelete(struct KzuStringPropertyType* propertyType);


/** Returns string property type from property type. */
struct KzuStringPropertyType* kzuStringPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Converts string property type to property type. */
struct KzuPropertyType* kzuStringPropertyTypeToPropertyType(struct KzuStringPropertyType* propertyType);

/** Gets the default value of string property type. */
kzString kzuStringPropertyTypeGetDefaultValue(const struct KzuStringPropertyType* propertyType);


/** Creates a new string property. */
kzsError kzuStringPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuStringPropertyType* propertyType, 
                                 kzString value, struct KzuStringProperty** out_property);
/** Deletes string property. */
kzsError kzuStringPropertyDelete(struct KzuStringProperty* property);


/** Converts property to string property. */
struct KzuStringProperty* kzuStringPropertyFromProperty(const struct KzuProperty* property);
/** Converts string property to property. */
struct KzuProperty* kzuStringPropertyToProperty(struct KzuStringProperty* stringProperty);

/** Returns a string value of string property. */
kzMutableString kzuStringPropertyGetValue(const struct KzuStringProperty* stringProperty);
/** Sets value for string property. */
kzsError kzuStringPropertySetValue(struct KzuStringProperty* stringProperty, kzString value);


#endif
