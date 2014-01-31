/**
* \file
* Specifies matrix2x2 property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_MATRIX2X2_PROPERTY_H
#define KZU_MATRIX2X2_PROPERTY_H


#include <core/util/math/kzc_matrix2x2.h>

#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzcMemoryManager;


/** 
* \struct KzuMatrix2x2Property
* Structure for matrix2x2 property.
*/
struct KzuMatrix2x2Property;
/** 
* \struct KzuMatrix2x2PropertyType
* Structure for matrix2x2 property type.
*/
struct KzuMatrix2x2PropertyType;


/** Creates a new matrix property type. */
kzsError kzuMatrix2x2PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcMatrix2x2 defaultValue,
                                        struct KzuMatrix2x2PropertyType** out_propertyType);
/** Deletes matrix property type. */
kzsError kzuMatrix2x2PropertyTypeDelete(struct KzuMatrix2x2PropertyType* propertyType);


/** Returns matrix property type from property type. */
struct KzuMatrix2x2PropertyType* kzuMatrix2x2PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Returns property type from matrix property type. */
struct KzuPropertyType* kzuMatrix2x2PropertyTypeToPropertyType(struct KzuMatrix2x2PropertyType* propertyType);

/** Gets the default value of matrix property type. */
struct KzcMatrix2x2 kzuMatrix2x2PropertyTypeGetDefaultValue(const struct KzuMatrix2x2PropertyType* propertyType);


/** Creates a new matrix property. */
kzsError kzuMatrix2x2PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuMatrix2x2PropertyType* propertyType, 
                                    const struct KzcMatrix2x2* matrix, struct KzuMatrix2x2Property** out_property);
/** Deletes matrix property. */
kzsError kzuMatrix2x2PropertyDelete(struct KzuMatrix2x2Property* property);


/** Returns a value of matrix property. */
struct KzcMatrix2x2 kzuMatrix2x2PropertyGetValue(const struct KzuMatrix2x2Property* matrix);
/** Sets value for matrix property. */
void kzuMatrix2x2PropertySetValue(struct KzuMatrix2x2Property* matrixProperty, const struct KzcMatrix2x2* matrix);

/** Converts matrix property to property. */
struct KzuProperty* kzuMatrix2x2PropertyToProperty(struct KzuMatrix2x2Property* matrix);
/** Returns matrix property from property. */
struct KzuMatrix2x2Property* kzuMatrix2x2PropertyFromProperty(const struct KzuProperty* property);


#endif
