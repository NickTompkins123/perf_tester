/**
* \file
* Specifies vector2 property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_vector2_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>


/** Vector2 property type. */
struct KzuVector2PropertyType
{
    struct KzuPropertyType propertyType;        /**< Parent property type. */
};

struct KzuVector2PropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuVector2PropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    struct KzcVector2 defaultValue; /**< Default value. */
};

/** Vector2 property structure. */
struct KzuVector2Property
{
    struct KzuProperty property;   /**< Property. Used for inheritance. */
    struct KzcVector2 vector;      /**< Vector. */

    kzBool isRelative; /**< Is property relative (for animations). */
    struct KzcVector2 relativeBase; /**< Object base value. */
};


kzsError kzuVector2PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, struct KzcVector2 defaultValue,
                                      struct KzuVector2PropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuVector2PropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Vector2 property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_VECTOR2, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuVector2PropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuVector2PropertyTypeDelete(struct KzuVector2PropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuVector2PropertyType* kzuVector2PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuVector2PropertyType*)propertyType;
}

struct KzuPropertyType* kzuVector2PropertyTypeToPropertyType(struct KzuVector2PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

struct KzcVector2 kzuVector2PropertyTypeGetDefaultValue(const struct KzuVector2PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuVector2PropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuVector2PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuVector2PropertyType* propertyType, 
                                  struct KzcVector2 vector, struct KzuVector2Property** out_property)
{
    kzsError result;
    struct KzuVector2Property* vectorProperty;

    result = kzcMemoryAllocVariable(memoryManager, vectorProperty, "Vector2Property");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&vectorProperty->property, (struct KzuPropertyType*)propertyType);
    vectorProperty->vector = vector;
    vectorProperty->relativeBase = kzcVector2(0.0f, 0.0f);
    vectorProperty->isRelative = KZ_FALSE;

    *out_property = vectorProperty;
    kzsSuccess();
}

kzsError kzuVector2PropertyDelete(struct KzuVector2Property* property)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(property));

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuVector2PropertySetRelative(struct KzuVector2Property* property, kzBool relative)
{
    kzsAssert(kzcIsValidPointer(property));
    property->isRelative = relative;
}

kzBool kzuVector2PropertyIsRelative(const struct KzuVector2Property* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return property->isRelative;
}

struct KzuProperty* kzuVector2PropertyToProperty(struct KzuVector2Property* vector)
{
    return &vector->property;
}

struct KzcVector2 kzuVector2PropertyGetValue(const struct KzuVector2Property* vector)
{
    struct KzcVector2 value;
    kzsAssert(kzcIsValidPointer(vector));
    kzcVector2Add(&vector->vector, &vector->relativeBase, &value);
    return value;
}

struct KzcVector2 kzuVector2PropertyGetRelativeValue(const struct KzuVector2Property* vector)
{
    kzsAssert(kzcIsValidPointer(vector));
    return vector->relativeBase;
}

void kzuVector2PropertySetValue(struct KzuVector2Property* vectorProperty, struct KzcVector2 vector)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    vectorProperty->vector = vector;
}

void kzuVector2PropertySetRelativeValue(struct KzuVector2Property* vectorProperty, struct KzcVector2 vector)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    vectorProperty->relativeBase = vector;
}

struct KzuVector2Property* kzuVector2PropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuVector2Property*)property;
}

void kzuVector2PropertySetComponentValue(struct KzuVector2Property* vectorProperty, kzUint component, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    kzsAssertText(component < 2, "Component out of valid range");

    vectorProperty->vector.data[component] = value;
}

void kzuVector2PropertySetRelativeComponentValue(struct KzuVector2Property* vectorProperty, kzUint component, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    kzsAssertText(component < 2, "Component out of valid range");

    vectorProperty->relativeBase.data[component] = value;
}
