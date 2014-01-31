/**
* \file
* Specifies vector3 property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_VECTOR3_PROPERTY_H
#define KZU_VECTOR3_PROPERTY_H


#include <core/util/math/kzc_vector3.h>

#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuVector3Property
* Structure for vector3 property.
*/
struct KzuVector3Property;
/** 
* \struct KzuVector3PropertyType
* Structure for vector3 property type.
*/
struct KzuVector3PropertyType;


/** Type of vector3 property. */
enum KzuVector3Type
{
    KZU_VECTOR3_TYPE_NORMAL,                    /**< Normal vector3 property. */
    KZU_VECTOR3_TYPE_POSITION_FROM_MATRIX4X4,   /**< Vector3 property value fetch from pipeline 4x4 world matrix position. */
    KZU_VECTOR3_TYPE_DIRECTION_FROM_MATRIX4X4   /**< Vector3 property value fetch from pipeline 4x4 world matrix direction. */
};


/** Creates a new vector property type. */
kzsError kzuVector3PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name,
                                      enum KzuVector3Type vectorType, struct KzcVector3 defaultValue,
                                      struct KzuVector3PropertyType** out_propertyType);
/** Deletes vector property type. */
kzsError kzuVector3PropertyTypeDelete(struct KzuVector3PropertyType* propertyType);


/** Returns vector3 property type from property type. */
struct KzuVector3PropertyType* kzuVector3PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Returns property type from vector3 property type. */
struct KzuPropertyType* kzuVector3PropertyTypeToPropertyType(struct KzuVector3PropertyType* propertyType);

/** Gets the default value of vector property type. */
struct KzcVector3 kzuVector3PropertyTypeGetDefaultValue(const struct KzuVector3PropertyType* propertyType);

/** Gets vector type from vector property. */
enum KzuVector3Type kzuVector3PropertyTypeGetVectorType(const struct KzuVector3PropertyType* vector);


/** Creates a new vector property. */
kzsError kzuVector3PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuVector3PropertyType* propertyType, 
                                  struct KzcVector3 vector, struct KzuVector3Property** out_property);
/** Deletes vector property. */
kzsError kzuVector3PropertyDelete(struct KzuVector3Property* property);


/** Returns vector3 property from generic property. */
struct KzuVector3Property* kzuVector3PropertyFromProperty(const struct KzuProperty* property);
/** Returns property from vector3 property. */
struct KzuProperty* kzuVector3PropertyToProperty(struct KzuVector3Property* vector);

/** Sets property relative. */
void kzuVector3PropertySetRelative(struct KzuVector3Property* property, kzBool relative);
/** Is property relative. */
kzBool kzuVector3PropertyIsRelative(const struct KzuVector3Property* property);

/** Returns a vector value of vector property. */
struct KzcVector3 kzuVector3PropertyGetValue(const struct KzuVector3Property* vector);
/** Returns relative value of vector property. */
struct KzcVector3 kzuVector3PropertyGetRelativeValue(const struct KzuVector3Property* vector);

/** Sets value for vector property. */
void kzuVector3PropertySetValue(struct KzuVector3Property* vectorProperty, struct KzcVector3 vector);
/** Sets relative value for vector property. */
void kzuVector3PropertySetRelativeValue(struct KzuVector3Property* vectorProperty, struct KzcVector3 vector);

/** Sets value for given component of the vector property. */
void kzuVector3PropertySetComponentValue(struct KzuVector3Property* vectorProperty, kzUint component, kzFloat value);
/** Sets value for given component of the vector property. */
void kzuVector3PropertySetRelativeComponentValue(struct KzuVector3Property* vectorProperty, kzUint component, kzFloat value);


#endif
