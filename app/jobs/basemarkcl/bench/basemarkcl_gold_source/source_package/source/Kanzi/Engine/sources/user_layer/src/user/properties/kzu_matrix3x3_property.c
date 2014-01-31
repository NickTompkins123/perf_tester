/**
* \file
* Specifies matrix property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_matrix3x3_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>


/** Matrix3x3 property type. */
struct KzuMatrix3x3PropertyType
{
    struct KzuPropertyType propertyType;        /**< Parent property type. */
};

struct KzuMatrix3x3PropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuMatrix3x3PropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    struct KzcMatrix3x3 defaultValue; /**< Default value. */
};

/** Matrix3x3 property structure. */
struct KzuMatrix3x3Property
{
    struct KzuProperty property;    /**< Property. Used for inheritance. */
    struct KzcMatrix3x3 matrix;      /**< Matrix. */
};


kzsError kzuMatrix3x3PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcMatrix3x3 defaultValue,
                                        struct KzuMatrix3x3PropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuMatrix3x3PropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Matrix 3x3 property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_MATRIX3X3, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuMatrix3x3PropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuMatrix3x3PropertyTypeDelete(struct KzuMatrix3x3PropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuMatrix3x3PropertyType* kzuMatrix3x3PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuMatrix3x3PropertyType*)propertyType;
}

struct KzuPropertyType* kzuMatrix3x3PropertyTypeToPropertyType(struct KzuMatrix3x3PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

struct KzcMatrix3x3 kzuMatrix3x3PropertyTypeGetDefaultValue(const struct KzuMatrix3x3PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuMatrix3x3PropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuMatrix3x3PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuMatrix3x3PropertyType* propertyType, 
                                    const struct KzcMatrix3x3* matrix, struct KzuMatrix3x3Property** out_property)
{
    kzsError result;
    struct KzuMatrix3x3Property* matrixProperty;

    result = kzcMemoryAllocVariable(memoryManager, matrixProperty, "Matrix3x3Property");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&matrixProperty->property, (struct KzuPropertyType*)propertyType);
    matrixProperty->matrix = *matrix;

    *out_property = matrixProperty;
    kzsSuccess();
}

kzsError kzuMatrix3x3PropertyDelete(struct KzuMatrix3x3Property* property)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(property));

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuProperty* kzuMatrix3x3PropertyToProperty(struct KzuMatrix3x3Property* matrix)
{
    return &matrix->property;
}

struct KzcMatrix3x3 kzuMatrix3x3PropertyGetValue(const struct KzuMatrix3x3Property* matrix)
{
    kzsAssert(kzcIsValidPointer(matrix));
    return matrix->matrix;
}

void kzuMatrix3x3PropertySetValue(struct KzuMatrix3x3Property* matrixProperty, const struct KzcMatrix3x3* matrix)
{
    kzsAssert(kzcIsValidPointer(matrixProperty));
    matrixProperty->matrix = *matrix;
}

struct KzuMatrix3x3Property* kzuMatrix3x3PropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuMatrix3x3Property*)property;
}
