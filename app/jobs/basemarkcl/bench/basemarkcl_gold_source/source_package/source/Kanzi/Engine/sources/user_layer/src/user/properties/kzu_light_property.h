/**
* \file
* Specifies light property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_LIGHT_PROPERTY_H
#define KZU_LIGHT_PROPERTY_H


#include <core/util/math/kzc_matrix4x4.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzuPropertyCollection;
struct KzuGroupProperty;
struct KzuLightNode;
struct KzuObjectNode;
struct KzcMemoryManager;
struct KzcDynamicArray;


/** 
* \struct KzuLightProperty
* Structure for light property.
*/
struct KzuLightProperty;

/** 
* \struct KzuLightPropertyType
* Structure for light property type.
*/
struct KzuLightPropertyType;


/* Light property */

/** Creates a new group property */
kzsError kzuLightPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuLightPropertyType* propertyType,
                                struct KzuLightProperty** out_property);
/** Deletes a light property. */
kzsError kzuLightPropertyDelete(struct KzuLightProperty* property);


/** Converts light property to property. */
struct KzuProperty* kzuLightPropertyToProperty(struct KzuLightProperty* lightProperty);
/** Converts property to light property. */
struct KzuLightProperty* kzuLightPropertyFromProperty(const struct KzuProperty* property);
/** Gets the type of the light property. */
struct KzuLightPropertyType* kzuLightPropertyGetLightPropertyType(const struct KzuLightProperty* lightProperty);


/* Light property type */

/** Creates a new group property type. */
kzsError kzuLightPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, /*const struct KzcDynamicArray* group, */
                                    struct KzuLightPropertyType** out_propertyType);
/** Deletes a light property type. */
kzsError kzuLightPropertyTypeDelete(struct KzuLightPropertyType* propertyType);


/** Adds child property type for a light property type. */
kzsError kzuLightPropertyTypeAddProperty(const struct KzuLightPropertyType* property, const struct KzuPropertyType* childPropertyType);


/** Returns property type from group property type. */
struct KzuPropertyType* kzuLightPropertyTypeToPropertyType(struct KzuLightPropertyType* propertyType);
/** Returns group property type from property type. */
struct KzuLightPropertyType* kzuLightPropertyTypeFromPropertyType(const struct KzuPropertyType* propertyType);
/** Gets list of property types that light property type contains. */
struct KzcDynamicArrayIterator kzuLightPropertyTypeGetPropertyTypes(const struct KzuLightPropertyType* lightPropertyType);
/** Gets the number of property types. */
kzUint kzuLightPropertyTypeGetPropertyTypeCount(const struct KzuLightPropertyType* lightPropertyType);


#endif
