/**
* \file
* Specifies property that holds a pointer to arbitrary data.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_VOID_PROPERTY_H
#define KZU_VOID_PROPERTY_H


#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuVoidProperty
* Structure for void property.
*/
struct KzuVoidProperty;
/** 
* \struct KzuVoidPropertyType
* Structure for void property type.
*/
struct KzuVoidPropertyType;


/** Creates a new void property type. */
kzsError kzuVoidPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, void* defaultValue,
                                   struct KzuVoidPropertyType** out_propertyType);
/** Deletes void property type. */
kzsError kzuVoidPropertyTypeDelete(struct KzuVoidPropertyType* propertyType);


/** Returns void property type from property type. */
struct KzuVoidPropertyType* kzuVoidPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Converts void property type to property type. */
struct KzuPropertyType* kzuVoidPropertyTypeToPropertyType(struct KzuVoidPropertyType* propertyType);

/** Gets the default value of void property type. */
void* kzuVoidPropertyTypeGetDefaultValue(const struct KzuVoidPropertyType* propertyType);


/** Creates a new void property. */
kzsError kzuVoidPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuVoidPropertyType* propertyType, 
                               void* value, struct KzuVoidProperty** out_property);
/** Deletes void property. */
kzsError kzuVoidPropertyDelete(struct KzuVoidProperty* property);


/** Converts property to void property. */
struct KzuVoidProperty* kzuVoidPropertyFromProperty(const struct KzuProperty* property);
/** Converts void property to property. */
struct KzuProperty* kzuVoidPropertyToProperty(struct KzuVoidProperty* property);

/** Returns the data of void property. */
void* kzuVoidPropertyGetValue(const struct KzuVoidProperty* property);
/** Sets value for property. The data pointer is stored, but the ownership is not changed. */
void kzuVoidPropertySetValue(struct KzuVoidProperty* property, void* data);


#endif
