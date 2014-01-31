/**
* \file
* Specifies matrix property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_matrix2x2_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>


/** Matrix2x2 property type. */
struct KzuMatrix2x2PropertyType
{
    struct KzuPropertyType propertyType; /**< Parent property type. */
};

struct KzuMatrix2x2PropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuMatrix2x2PropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    struct KzcMatrix2x2 defaultValue; /**< Default value. */
};

/** Matrix2z2 property structure. */
struct KzuMatrix2x2Property
{
    struct KzuProperty property; /**< Property. Used for inheritance. */
    struct KzcMatrix2x2 matrix; /**< Matrix. */
};


kzsError kzuMatrix2x2PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcMatrix2x2 defaultValue,
                                        struct KzuMatrix2x2PropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuMatrix2x2PropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Matrix 2x2 property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_MATRIX2X2, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuMatrix2x2PropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuMatrix2x2PropertyTypeDelete(struct KzuMatrix2x2PropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuMatrix2x2PropertyType* kzuMatrix2x2PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuMatrix2x2PropertyType*)propertyType;
}

struct KzuPropertyType* kzuMatrix2x2PropertyTypeToPropertyType(struct KzuMatrix2x2PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

struct KzcMatrix2x2 kzuMatrix2x2PropertyTypeGetDefaultValue(const struct KzuMatrix2x2PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuMatrix2x2PropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuMatrix2x2PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuMatrix2x2PropertyType* propertyType, 
                                    const struct KzcMatrix2x2* matrix, struct KzuMatrix2x2Property** out_property)
{
    kzsError result;
    struct KzuMatrix2x2Property* matrixProperty;

    result = kzcMemoryAllocVariable(memoryManager, matrixProperty, "Matrix2x2Property");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&matrixProperty->property, (struct KzuPropertyType*)propertyType);
    matrixProperty->matrix = *matrix;

    *out_property = matrixProperty;
    kzsSuccess();
}

kzsError kzuMatrix2x2PropertyDelete(struct KzuMatrix2x2Property* property)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(property));

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuProperty* kzuMatrix2x2PropertyToProperty(struct KzuMatrix2x2Property* matrix)
{
    return &matrix->property;
}

struct KzuMatrix2x2Property* kzuMatrix2x2PropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuMatrix2x2Property*)property;
}

struct KzcMatrix2x2 kzuMatrix2x2PropertyGetValue(const struct KzuMatrix2x2Property* matrix)
{
    kzsAssert(kzcIsValidPointer(matrix));
    return matrix->matrix;
}

void kzuMatrix2x2PropertySetValue(struct KzuMatrix2x2Property* matrixProperty, const struct KzcMatrix2x2* matrix)
{
    kzsAssert(kzcIsValidPointer(matrixProperty));
    matrixProperty->matrix = *matrix;
}
