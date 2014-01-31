/**
* \file
* Specifies vector3 property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_vector3_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>


/** Vector3 property type. */
struct KzuVector3PropertyType
{
    struct KzuPropertyType propertyType; /**< Parent property type. */
};

struct KzuVector3PropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuVector3PropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    enum KzuVector3Type vectorType; /**< Type of vector, i.e. is it fetch from matrix. */
    struct KzcVector3 defaultValue; /**< Default value. */
};

/** Vector3 property structure. */
struct KzuVector3Property
{
    struct KzuProperty property;   /**< Property. Used for inheritance. */
    struct KzcVector3 vector;      /**< Vector. */

    kzBool isRelative; /**< Is property relative (for animations). */
    struct KzcVector3 relativeBase; /**< Object base value. */
};


kzsError kzuVector3PropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name,
                                      enum KzuVector3Type vectorType, struct KzcVector3 defaultValue,
                                      struct KzuVector3PropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuVector3PropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Vector3 property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_VECTOR3, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->vectorType = vectorType;
    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuVector3PropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuVector3PropertyTypeDelete(struct KzuVector3PropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuVector3PropertyType* kzuVector3PropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuVector3PropertyType*)propertyType;
}

struct KzuPropertyType* kzuVector3PropertyTypeToPropertyType(struct KzuVector3PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

struct KzcVector3 kzuVector3PropertyTypeGetDefaultValue(const struct KzuVector3PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuVector3PropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

enum KzuVector3Type kzuVector3PropertyTypeGetVectorType(const struct KzuVector3PropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuVector3PropertyTypeData*)propertyType->propertyType.data)->vectorType;
}

kzsError kzuVector3PropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuVector3PropertyType* propertyType, 
                                  struct KzcVector3 vector, struct KzuVector3Property** out_property)
{
    kzsError result;
    struct KzuVector3Property* vectorProperty;

    result = kzcMemoryAllocVariable(memoryManager, vectorProperty, "Vector3Property");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&vectorProperty->property, (struct KzuPropertyType*)propertyType);
    vectorProperty->vector = vector;
    vectorProperty->relativeBase = kzcVector3(0.0f, 0.0f, 0.0f);
    vectorProperty->isRelative = KZ_FALSE;

    *out_property = vectorProperty;
    kzsSuccess();
}

kzsError kzuVector3PropertyDelete(struct KzuVector3Property* property)
{
    kzsError result;

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuProperty* kzuVector3PropertyToProperty(struct KzuVector3Property* property)
{
    return &property->property;
}

void kzuVector3PropertySetRelative(struct KzuVector3Property* property, kzBool relative)
{
    kzsAssert(kzcIsValidPointer(property));
    property->isRelative = relative;
}

kzBool kzuVector3PropertyIsRelative(const struct KzuVector3Property* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return property->isRelative;
}

struct KzcVector3 kzuVector3PropertyGetValue(const struct KzuVector3Property* property)
{
    struct KzcVector3 value;
    kzsAssert(kzcIsValidPointer(property));
    kzcVector3Add(&property->vector, &property->relativeBase, &value);
    return value;
}

struct KzcVector3 kzuVector3PropertyGetRelativeValue(const struct KzuVector3Property* vector)
{
    kzsAssert(kzcIsValidPointer(vector));
    return vector->relativeBase;
}

void kzuVector3PropertySetValue(struct KzuVector3Property* vectorProperty, struct KzcVector3 property)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    vectorProperty->vector = property;
}

void kzuVector3PropertySetRelativeValue(struct KzuVector3Property* vectorProperty, struct KzcVector3 vector)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    vectorProperty->relativeBase = vector;
}

void kzuVector3PropertySetComponentValue(struct KzuVector3Property* vectorProperty, kzUint component, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    kzsAssertText(component < 3, "Component out of valid range");

    vectorProperty->vector.data[component] = value;
}

void kzuVector3PropertySetRelativeComponentValue(struct KzuVector3Property* vectorProperty, kzUint component, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(vectorProperty));
    kzsAssertText(component < 3, "Component out of valid range");

    vectorProperty->relativeBase.data[component] = value;
}

struct KzuVector3Property* kzuVector3PropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuVector3Property*)property;
}
