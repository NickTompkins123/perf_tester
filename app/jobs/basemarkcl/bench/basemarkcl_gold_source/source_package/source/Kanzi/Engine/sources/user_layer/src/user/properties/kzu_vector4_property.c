/**
* \file
* Specifies vector4 property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_vector4_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>


/** Vector4 property type. */
struct KzuVector4PropertyType
{
    struct KzuPropertyType propertyType;        /**< Parent property type. */
};

struct KzuVector4PropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuVector4PropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    struct KzcVector4 defaultValue; /**< Default value. */
};

/** Vector4 property structure. */
struct KzuVector4Property
{
    struct KzuProperty property;   /**< Property. Used for inheritance. */
    struct KzcVector4 vector;      /**< Vector. */

    kzBool isRelative; /**< Is property relative (for animations). */
    struct KzcVector4 relativeBase; /**< Object base value. */
};


kzsError kzuVector4PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name,
                                      struct KzcVector4 defaultValue, struct KzuVector4PropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuVector4PropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Vector4 property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_VECTOR4, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuVector4PropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuVector4PropertyTypeDelete(struct KzuVector4PropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuVector4PropertyType* kzuVector4PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuVector4PropertyType*)propertyType;
}

struct KzuPropertyType* kzuVector4PropertyTypeToPropertyType(struct KzuVector4PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

struct KzcVector4 kzuVector4PropertyTypeGetDefaultValue(const struct KzuVector4PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuVector4PropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuVector4PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuVector4PropertyType* propertyType, 
                                  struct KzcVector4 vector, struct KzuVector4Property** out_property)
{
    kzsError result;
    struct KzuVector4Property* vectorProperty;

    result = kzcMemoryAllocVariable(memoryManager, vectorProperty, "Vector4 property");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&vectorProperty->property, (struct KzuPropertyType*)propertyType);
    vectorProperty->vector = vector;
    vectorProperty->relativeBase = kzcVector4(0.0f, 0.0f, 0.0f, 0.0f);
    vectorProperty->isRelative = KZ_FALSE;

    *out_property = vectorProperty;
    kzsSuccess();
}

kzsError kzuVector4PropertyDelete(struct KzuVector4Property* property)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(property));

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuProperty* kzuVector4PropertyToProperty(struct KzuVector4Property* vector)
{
    return &vector->property;
}

void kzuVector4PropertySetRelative(struct KzuVector4Property* property, kzBool relative)
{
    kzsAssert(kzcIsValidPointer(property));
    property->isRelative = relative;
}

kzBool kzuVector4PropertyIsRelative(const struct KzuVector4Property* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return property->isRelative;
}

struct KzcVector4 kzuVector4PropertyGetValue(const struct KzuVector4Property* vector)
{
    struct KzcVector4 value;
    kzsAssert(kzcIsValidPointer(vector));
    kzcVector4Add(&vector->vector, &vector->relativeBase, &value);
    return value;
}

struct KzcVector4 kzuVector4PropertyGetRelativeValue(const struct KzuVector4Property* vector)
{
    kzsAssert(kzcIsValidPointer(vector));
    return vector->relativeBase;
}

void kzuVector4PropertySetComponentValue(struct KzuVector4Property* vectorProperty, kzUint component, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    kzsAssertText(component < 4, "Component out of valid range");

    vectorProperty->vector.data[component] = value;
}

void kzuVector4PropertySetRelativeComponentValue(struct KzuVector4Property* vectorProperty, kzUint component, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    kzsAssertText(component < 4, "Component out of valid range");

    vectorProperty->relativeBase.data[component] = value;
}

void kzuVector4PropertySetValue(struct KzuVector4Property* vectorProperty, struct KzcVector4 vector)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    vectorProperty->vector = vector;
}

void kzuVector4PropertySetRelativeValue(struct KzuVector4Property* vectorProperty, struct KzcVector4 vector)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    vectorProperty->relativeBase = vector;
}

struct KzuVector4Property* kzuVector4PropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuVector4Property*)property;
}
