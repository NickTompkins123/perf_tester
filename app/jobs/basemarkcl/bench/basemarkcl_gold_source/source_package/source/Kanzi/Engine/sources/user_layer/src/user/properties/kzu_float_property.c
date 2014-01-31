/**
* \file
* Specifies float property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_float_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>


/** Float property type. */
struct KzuFloatPropertyType
{
    struct KzuPropertyType propertyType; /**< Parent property type. */
};

struct KzuFloatPropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuFloatPropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    kzFloat defaultValue; /**< Default value. */
};

/** Float property structure. */
struct KzuFloatProperty
{
    struct KzuProperty property;    /**< Property. Used for inheritance. */
    kzFloat value;                  /**< Value. */

    kzBool isRelative; /**< Is property relative (for animations). */
    kzFloat relativeBase; /**< Object base value. */
};


kzsError kzuFloatPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, 
                                    kzFloat defaultValue, struct KzuFloatPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuFloatPropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Float property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_FLOAT, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuFloatPropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuFloatPropertyTypeDelete(struct KzuFloatPropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuFloatPropertyType* kzuFloatPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuFloatPropertyType*)propertyType;
}

struct KzuPropertyType* kzuFloatPropertyTypeToPropertyType(struct KzuFloatPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

kzFloat kzuFloatPropertyTypeGetDefaultValue(const struct KzuFloatPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuFloatPropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuFloatPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuFloatPropertyType* propertyType, 
                                kzFloat value, struct KzuFloatProperty** out_property)
{
    kzsError result;
    struct KzuFloatProperty* floatProperty;

    result = kzcMemoryAllocVariable(memoryManager, floatProperty, "FloatProperty");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&floatProperty->property, (struct KzuPropertyType*)propertyType);
    floatProperty->value = value;
    floatProperty->relativeBase = 0.0f;
    floatProperty->isRelative = KZ_FALSE;

    *out_property = floatProperty;
    kzsSuccess();
}

kzsError kzuFloatPropertyDelete(struct KzuFloatProperty* property)
{
    kzsError result;

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuFloatPropertySetRelative(struct KzuFloatProperty* property, kzBool relative)
{
    kzsAssert(kzcIsValidPointer(property));
    property->isRelative = relative;
}

kzBool kzuFloatPropertyIsRelative(const struct KzuFloatProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return property->isRelative;
}

kzFloat kzuFloatPropertyGetValue(const struct KzuFloatProperty* floatProperty)
{
    kzsAssert(kzcIsValidPointer(floatProperty));
    return floatProperty->value + floatProperty->relativeBase;
}

kzFloat kzuFloatPropertyGetRelativeValue(const struct KzuFloatProperty* floatProperty)
{
    kzsAssert(kzcIsValidPointer(floatProperty));
    return floatProperty->relativeBase;
}

void kzuFloatPropertySetValue(struct KzuFloatProperty* floatProperty, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(floatProperty));
    floatProperty->value = value;
}

void kzuFloatPropertySetRelativeValue(struct KzuFloatProperty* floatProperty, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(floatProperty));
    floatProperty->relativeBase = value;
}

struct KzuProperty* kzuFloatPropertyToProperty(struct KzuFloatProperty* floatProperty)
{
    return &floatProperty->property;
}

struct KzuFloatProperty* kzuFloatPropertyFromProperty(const struct KzuProperty* property)
{
    return (struct KzuFloatProperty*)property;
}
