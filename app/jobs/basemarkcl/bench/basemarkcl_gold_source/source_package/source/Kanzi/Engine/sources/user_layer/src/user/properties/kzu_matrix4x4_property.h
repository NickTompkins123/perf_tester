/**
* \file
* Specifies matrix4x4 property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_MATRIX4X4_PROPERTY_H
#define KZU_MATRIX4X4_PROPERTY_H


#include <core/util/math/kzc_matrix4x4.h>

#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuMatrix4x4Property
* Structure for matrix4x4 property.
*/
struct KzuMatrix4x4Property;
/** 
* \struct KzuMatrix4x4PropertyType
* Structure for matrix4x4 property type.
*/
struct KzuMatrix4x4PropertyType;


/** Creates a new matrix property type. */
kzsError kzuMatrix4x4PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcMatrix4x4 defaultValue,
                                        struct KzuMatrix4x4PropertyType** out_propertyType);
/** Deletes matrix property type. */
kzsError kzuMatrix4x4PropertyTypeDelete(struct KzuMatrix4x4PropertyType* propertyType);


/** Returns matrix 4x4 property type from property type. */
struct KzuMatrix4x4PropertyType* kzuMatrix4x4PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Returns property type from matrix property type. */
struct KzuPropertyType* kzuMatrix4x4PropertyTypeToPropertyType(struct KzuMatrix4x4PropertyType* propertyType);

/** Gets the default value of matrix property type. */
struct KzcMatrix4x4 kzuMatrix4x4PropertyTypeGetDefaultValue(const struct KzuMatrix4x4PropertyType* propertyType);


/** Creates a new matrix property. */
kzsError kzuMatrix4x4PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuMatrix4x4PropertyType* propertyType, 
                                    const struct KzcMatrix4x4* matrix, struct KzuMatrix4x4Property** out_property);
/** Deletes matrix property. */
kzsError kzuMatrix4x4PropertyDelete(struct KzuMatrix4x4Property* property);


/** Sets matrix property relative. */
void kzuMatrix4x4PropertySetRelative( struct KzuMatrix4x4Property* matrixProperty, kzBool relative);
/** Is matrix property relative. */
kzBool kzuMatrix4x4PropertyIsRelative(const struct KzuMatrix4x4Property* matrix);
/** Returns a value of matrix property. */
struct KzcMatrix4x4 kzuMatrix4x4PropertyGetValue(const struct KzuMatrix4x4Property* matrix);
/** Returns a relative value of matrix property. */
struct KzcMatrix4x4 kzuMatrix4x4PropertyGetRelativeValue(const struct KzuMatrix4x4Property* matrix);
/** Convers matrix property to property. */
struct KzuProperty* kzuMatrix4x4PropertyToProperty(struct KzuMatrix4x4Property* property);
/** Returns matrix property from property. */
struct KzuMatrix4x4Property* kzuMatrix4x4PropertyFromProperty(const struct KzuProperty* property);
/** Sets value for matrix property. */
void kzuMatrix4x4PropertySetValue(struct KzuMatrix4x4Property* matrixProperty, const struct KzcMatrix4x4* matrix);
/** Sets relative value for matrix property. */
void kzuMatrix4x4PropertySetRelativeValue(struct KzuMatrix4x4Property* matrixProperty, const struct KzcMatrix4x4* matrix);


#endif
