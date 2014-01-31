/**
* \file
* Specifies boolean property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_bool_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>


/** Boolean property type. */
struct KzuBoolPropertyType
{
    struct KzuPropertyType propertyType;        /**< Parent property type. */
};

struct KzuBoolPropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuIntPropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    kzBool defaultValue; /**< Default value. */
};

/** Boolean property. */
struct KzuBoolProperty
{
    struct KzuProperty property;    /**< Property. Used for inheritance. */
    kzBool value;                    /**< Value. */
};


kzsError kzuBoolPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, kzBool defaultValue,
                                   struct KzuBoolPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuBoolPropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Bool property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_BOOL, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuBoolPropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuBoolPropertyTypeDelete(struct KzuBoolPropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuBoolPropertyType* kzuBoolPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuBoolPropertyType*)propertyType;
}

struct KzuPropertyType* kzuBoolPropertyTypeToPropertyType(struct KzuBoolPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

kzBool kzuBoolPropertyTypeGetDefaultValue(const struct KzuBoolPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuBoolPropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuBoolPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuBoolPropertyType* propertyType, 
                               kzBool value, struct KzuBoolProperty** out_property)
{
    kzsError result;
    struct KzuBoolProperty* boolProperty;

    result = kzcMemoryAllocVariable(memoryManager, boolProperty, "BoolProperty");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&boolProperty->property, (struct KzuPropertyType*)propertyType);
    boolProperty->value = value;

    *out_property = boolProperty;
    kzsSuccess();
}

kzsError kzuBoolPropertyDelete(struct KzuBoolProperty* property)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(property));

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuBoolPropertyGetValue(const struct KzuBoolProperty* boolProperty)
{
    kzsAssert(kzcIsValidPointer(boolProperty));
    return boolProperty->value;
}

struct KzuProperty* kzuBoolPropertyToProperty(struct KzuBoolProperty* property)
{
    return &property->property;
}

void kzuBoolPropertySetValue(struct KzuBoolProperty* boolProperty, kzBool value)
{
    kzsAssert(kzcIsValidPointer(boolProperty));
    boolProperty->value = value;
}

struct KzuBoolProperty* kzuBoolPropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuBoolProperty*)property;
}
