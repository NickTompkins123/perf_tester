/**
* \file
* Specifies vector4 property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_VECTOR4_PROPERTY_H
#define KZU_VECTOR4_PROPERTY_H


#include <core/util/math/kzc_vector4.h>

#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuVector4Property
* Structure for vector4 property.
*/
struct KzuVector4Property;
/** 
* \struct KzuVector4PropertyType
* Structure for vector4 property type.
*/
struct KzuVector4PropertyType;


/** Creates a new vector property type. */
kzsError kzuVector4PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name,
                                      struct KzcVector4 defaultValue, struct KzuVector4PropertyType** out_propertyType);
/** Deletes vector property type. */
kzsError kzuVector4PropertyTypeDelete(struct KzuVector4PropertyType* property);


/** Returns vector4 property type from property type. */
struct KzuVector4PropertyType* kzuVector4PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Returns property type from vector4 property type. */
struct KzuPropertyType* kzuVector4PropertyTypeToPropertyType(struct KzuVector4PropertyType* propertyType);

/** Gets the default value of vector property type. */
struct KzcVector4 kzuVector4PropertyTypeGetDefaultValue(const struct KzuVector4PropertyType* propertyType);


/** Creates a new vector property. */
kzsError kzuVector4PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuVector4PropertyType* propertyType, 
                                  struct KzcVector4 vector, struct KzuVector4Property** out_property);
/** Deletes vector property. */
kzsError kzuVector4PropertyDelete(struct KzuVector4Property* property);


/** Sets property relative. */
void kzuVector4PropertySetRelative(struct KzuVector4Property* property, kzBool relative);
/** Is property relative. */
kzBool kzuVector4PropertyIsRelative(const struct KzuVector4Property* property);

/** Returns a value of vector property. */
struct KzcVector4 kzuVector4PropertyGetValue(const struct KzuVector4Property* vector);
/** Returns relative value of vector property. */
struct KzcVector4 kzuVector4PropertyGetRelativeValue(const struct KzuVector4Property* vector);

/** Sets value for vector property. */
void kzuVector4PropertySetValue(struct KzuVector4Property* vectorProperty, struct KzcVector4 vector);
/** Sets relative value for vector property. */
void kzuVector4PropertySetRelativeValue(struct KzuVector4Property* vectorProperty, struct KzcVector4 vector);

/** Casts generic property to vector4 property. */
struct KzuVector4Property* kzuVector4PropertyFromProperty(const struct KzuProperty* property);
/** Return property from vector property. */
struct KzuProperty* kzuVector4PropertyToProperty(struct KzuVector4Property* vector);
/** Sets value for given component of the vector property. */
void kzuVector4PropertySetComponentValue(struct KzuVector4Property* vectorProperty, kzUint component, kzFloat value);
/** Sets value for given component of the vector property. */
void kzuVector4PropertySetRelativeComponentValue(struct KzuVector4Property* vectorProperty, kzUint component, kzFloat value);


#endif
