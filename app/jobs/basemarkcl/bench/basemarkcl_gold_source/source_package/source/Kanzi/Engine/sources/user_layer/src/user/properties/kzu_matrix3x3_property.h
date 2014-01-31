/**
* \file
* Specifies matrix3x3 property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_MATRIX3X3_PROPERTY_H
#define KZU_MATRIX3X3_PROPERTY_H


#include <core/util/math/kzc_matrix3x3.h>

#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuMatrix3x3Property
* Structure for matrix3x3 property.
*/
struct KzuMatrix3x3Property;
/** 
* \struct KzuMatrix3x3PropertyType
* Structure for matrix3x3 property type.
*/
struct KzuMatrix3x3PropertyType;


/** Creates a new matrix property type. */
kzsError kzuMatrix3x3PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcMatrix3x3 defaultValue,
                                        struct KzuMatrix3x3PropertyType** out_propertyType);
/** Deletes matrix property type. */
kzsError kzuMatrix3x3PropertyTypeDelete(struct KzuMatrix3x3PropertyType* propertyType);


/** Returns matrix 3x3 property type from property type. */
struct KzuMatrix3x3PropertyType* kzuMatrix3x3PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Returns property type from matrix property type. */
struct KzuPropertyType* kzuMatrix3x3PropertyTypeToPropertyType(struct KzuMatrix3x3PropertyType* propertyType);

/** Gets the default value of matrix property type. */
struct KzcMatrix3x3 kzuMatrix3x3PropertyTypeGetDefaultValue(const struct KzuMatrix3x3PropertyType* propertyType);


/** Creates a new matrix property. */
kzsError kzuMatrix3x3PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuMatrix3x3PropertyType* propertyType, 
                                    const struct KzcMatrix3x3* matrix, struct KzuMatrix3x3Property** out_property);
/** Deletes matrix property. */
kzsError kzuMatrix3x3PropertyDelete(struct KzuMatrix3x3Property* property);


/** Converts matrix property to property. */
struct KzuProperty* kzuMatrix3x3PropertyToProperty(struct KzuMatrix3x3Property* matrix);
/** Returns matrix property from property. */
struct KzuMatrix3x3Property* kzuMatrix3x3PropertyFromProperty(const struct KzuProperty* property);

/** Returns a value of matrix property. */
struct KzcMatrix3x3 kzuMatrix3x3PropertyGetValue(const struct KzuMatrix3x3Property* matrix);
/** Sets value for matrix property. */
void kzuMatrix3x3PropertySetValue(struct KzuMatrix3x3Property* matrixProperty, const struct KzcMatrix3x3* matrix);


#endif
