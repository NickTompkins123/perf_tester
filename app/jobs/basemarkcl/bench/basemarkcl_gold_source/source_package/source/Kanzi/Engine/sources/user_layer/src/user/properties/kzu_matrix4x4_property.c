/**
* \file
* Specifies matrix property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_matrix4x4_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>


/** Matrix4x4 property type. */
struct KzuMatrix4x4PropertyType
{
    struct KzuPropertyType propertyType;        /**< Parent property type. */
};

struct KzuMatrix4x4PropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuMatrix4x4PropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    struct KzcMatrix4x4 defaultValue; /**< Default value. */
};

/** Matrix 4x4 property. */
struct KzuMatrix4x4Property
{
    struct KzuProperty property;    /**< Property. Used for inheritance. */
    struct KzcMatrix4x4 matrix;        /**< Matrix. */

    kzBool isRelative; /**< Is matrix property relative (for animations). */
    struct KzcMatrix4x4 relativeBaseMatrix; /**< Object base matrix. */
};


kzsError kzuMatrix4x4PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcMatrix4x4 defaultValue,
                                        struct KzuMatrix4x4PropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuMatrix4x4PropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Matrix 4x4 property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_MATRIX4X4, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuMatrix4x4PropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuMatrix4x4PropertyTypeDelete(struct KzuMatrix4x4PropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuMatrix4x4PropertyType* kzuMatrix4x4PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuMatrix4x4PropertyType*)propertyType;
}

struct KzuPropertyType* kzuMatrix4x4PropertyTypeToPropertyType(struct KzuMatrix4x4PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

struct KzcMatrix4x4 kzuMatrix4x4PropertyTypeGetDefaultValue(const struct KzuMatrix4x4PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuMatrix4x4PropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuMatrix4x4PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuMatrix4x4PropertyType* propertyType, 
                                    const struct KzcMatrix4x4* matrix, struct KzuMatrix4x4Property** out_property)
{
    kzsError result;
    struct KzuMatrix4x4Property* matrixProperty;

    result = kzcMemoryAllocVariable(memoryManager, matrixProperty, "Matrix4x4Property");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&matrixProperty->property, (struct KzuPropertyType*)propertyType);
    matrixProperty->matrix = *matrix;
    matrixProperty->relativeBaseMatrix = KZC_MATRIX4X4_IDENTITY;
    matrixProperty->isRelative = KZ_FALSE;

    *out_property = matrixProperty;
    kzsSuccess();
}

kzsError kzuMatrix4x4PropertyDelete(struct KzuMatrix4x4Property* property)
{
    kzsError result;

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuProperty* kzuMatrix4x4PropertyToProperty(struct KzuMatrix4x4Property* matrix)
{
    return &matrix->property;
}

struct KzcMatrix4x4 kzuMatrix4x4PropertyGetValue(const struct KzuMatrix4x4Property* matrix)
{
    kzsAssert(kzcIsValidPointer(matrix));
 
    return matrix->matrix;
}

struct KzcMatrix4x4 kzuMatrix4x4PropertyGetRelativeValue(const struct KzuMatrix4x4Property* matrix)
{
    struct KzcMatrix4x4* relativeMatrix;

    kzsAssert(kzcIsValidPointer(matrix));

    if(!matrix->isRelative)
    {
        relativeMatrix = (struct KzcMatrix4x4*)&matrix->matrix;
    }
    else
    {
        relativeMatrix = (struct KzcMatrix4x4*)&matrix->relativeBaseMatrix;
    }

    return *relativeMatrix;
}

void kzuMatrix4x4PropertySetValue(struct KzuMatrix4x4Property* matrixProperty, const struct KzcMatrix4x4* matrix)
{
    kzsAssert(kzcIsValidPointer(matrixProperty));
    matrixProperty->matrix = *matrix;
}

void kzuMatrix4x4PropertySetRelativeValue(struct KzuMatrix4x4Property* matrixProperty, const struct KzcMatrix4x4* matrix)
{
    kzsAssert(kzcIsValidPointer(matrixProperty));
    matrixProperty->relativeBaseMatrix = *matrix;
    if(!matrixProperty->isRelative)
    {
        kzuMatrix4x4PropertySetValue(matrixProperty, matrix);
    }
}

void kzuMatrix4x4PropertySetRelative(struct KzuMatrix4x4Property* matrixProperty, kzBool relative)
{
    kzsAssert(kzcIsValidPointer(matrixProperty));
    matrixProperty->isRelative = relative;
}

kzBool kzuMatrix4x4PropertyIsRelative(const struct KzuMatrix4x4Property* matrixProperty)
{
    kzsAssert(kzcIsValidPointer(matrixProperty));
    return matrixProperty->isRelative;
}

struct KzuMatrix4x4Property* kzuMatrix4x4PropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuMatrix4x4Property*)property;
}
