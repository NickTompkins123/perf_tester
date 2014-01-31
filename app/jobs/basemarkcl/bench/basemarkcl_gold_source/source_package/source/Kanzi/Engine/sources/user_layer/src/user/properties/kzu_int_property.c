/**
* \file
* Specifies integer property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_int_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>


/** Int property type. */
struct KzuIntPropertyType
{
    struct KzuPropertyType propertyType; /**< Parent property type. */
};

struct KzuIntPropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuIntPropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    kzInt defaultValue; /**< Default value. */
};

/** Int property. */
struct KzuIntProperty
{
    struct KzuProperty property;    /**< Property. Used for inheritance. */
    kzInt value;                    /**< Value. */
};


kzsError kzuIntPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, kzInt defaultValue, 
                                  struct KzuIntPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuIntPropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Int property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_INT, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuIntPropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuIntPropertyTypeDelete(struct KzuIntPropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuIntPropertyType* kzuIntPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuIntPropertyType*)propertyType;
}

struct KzuPropertyType* kzuIntPropertyTypeToPropertyType(struct KzuIntPropertyType* intPropertyType)
{
    kzsAssert(kzcIsValidPointer(intPropertyType));

    return &intPropertyType->propertyType;
}

kzInt kzuIntPropertyTypeGetDefaultValue(const struct KzuIntPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuIntPropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuIntPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuIntPropertyType* propertyType, 
                              kzInt value, struct KzuIntProperty** out_property)
{
    kzsError result;
    struct KzuIntProperty* intProperty;

    result = kzcMemoryAllocVariable(memoryManager, intProperty, "IntProperty");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&intProperty->property, (struct KzuPropertyType*)propertyType);
    intProperty->value = value;

    *out_property = intProperty;
    kzsSuccess();
}

kzsError kzuIntPropertyDelete(struct KzuIntProperty* property)
{
    kzsError result;

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuProperty* kzuIntPropertyToProperty(struct KzuIntProperty* intProperty)
{
    kzsAssert(kzcIsValidPointer(intProperty));
    return &intProperty->property;
}

kzInt kzuIntPropertyGetValue(const struct KzuIntProperty* intProperty)
{
    kzsAssert(kzcIsValidPointer(intProperty));
    return intProperty->value;
}

void kzuIntPropertySetValue(struct KzuIntProperty* intProperty, kzInt value)
{
    kzsAssert(kzcIsValidPointer(intProperty));
    intProperty->value = value;
}

struct KzuIntProperty* kzuIntPropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuIntProperty*)property;
}
