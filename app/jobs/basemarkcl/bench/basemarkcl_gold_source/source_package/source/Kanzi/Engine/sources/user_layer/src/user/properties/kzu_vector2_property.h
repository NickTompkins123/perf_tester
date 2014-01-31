/**
* \file
* Specifies vector2 property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_VECTOR2_PROPERTY_H
#define KZU_VECTOR2_PROPERTY_H


#include <core/util/math/kzc_vector2.h>

#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuVector2Property
* Structure for vector2 property.
*/
struct KzuVector2Property;
/** 
* \struct KzuVector2PropertyType
* Structure for vector2 property type.
*/
struct KzuVector2PropertyType;


/** Creates a new vector property type. */
kzsError kzuVector2PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcVector2 defaultValue,
                                      struct KzuVector2PropertyType** out_propertyType);
/** Deletes vector property type. */
kzsError kzuVector2PropertyTypeDelete(struct KzuVector2PropertyType* property);


/** Returns vector2 property type from property type. */
struct KzuVector2PropertyType* kzuVector2PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Returns property type from vector3 property type. */
struct KzuPropertyType* kzuVector2PropertyTypeToPropertyType(struct KzuVector2PropertyType* propertyType);

/** Gets the default value of vector property type. */
struct KzcVector2 kzuVector2PropertyTypeGetDefaultValue(const struct KzuVector2PropertyType* propertyType);


/** Creates a new vector property. */
kzsError kzuVector2PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuVector2PropertyType* propertyType, 
                                  struct KzcVector2 vector, struct KzuVector2Property** out_property);
/** Deletes vector property. */
kzsError kzuVector2PropertyDelete(struct KzuVector2Property* property);


/** Sets property relative. */
void kzuVector2PropertySetRelative(struct KzuVector2Property* property, kzBool relative);
/** Is property relative. */
kzBool kzuVector2PropertyIsRelative(const struct KzuVector2Property* property);

/** Casts generic property to vector2 property. */
struct KzuVector2Property* kzuVector2PropertyFromProperty(const struct KzuProperty* property);
/** Convert vector property to property. */
struct KzuProperty* kzuVector2PropertyToProperty(struct KzuVector2Property* vector);

/** Returns a vector value of vector property. */
struct KzcVector2 kzuVector2PropertyGetValue(const struct KzuVector2Property* vector);
/** Returns a vector value of vector property. */
struct KzcVector2 kzuVector2PropertyGetRelativeValue(const struct KzuVector2Property* vector);

/** Sets value for vector property. */
void kzuVector2PropertySetValue(struct KzuVector2Property* vectorProperty, struct KzcVector2 vector);
/** Sets relative value for vector property. */
void kzuVector2PropertySetRelativeValue(struct KzuVector2Property* vectorProperty, struct KzcVector2 vector);

/** Sets value for given component of the vector property. */
void kzuVector2PropertySetComponentValue(struct KzuVector2Property* vectorProperty, kzUint component, kzFloat value);
/** Sets value for given component of the vector property. */
void kzuVector2PropertySetRelativeComponentValue(struct KzuVector2Property* vectorProperty, kzUint component, kzFloat value);


#endif
