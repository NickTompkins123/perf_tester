/**
* \file
* Specifies string property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_string_property.h"

#include "kzu_property_private.h"

#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>


/** String property type. */
struct KzuStringPropertyType
{
    struct KzuPropertyType propertyType;    /**< Parent property type. */
};

struct KzuStringPropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuStringPropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    kzMutableString defaultValue; /**< Default value. */
};

/* String property structure. */
struct KzuStringProperty
{
    struct KzuProperty property;    /**< Property. Used for inheritance. */
    kzMutableString value;          /**< Value. */
};


kzsError kzuStringPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, kzString defaultValue,
                                     struct KzuStringPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuStringPropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "String property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_STRING, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    if (defaultValue != KZ_NULL)
    {
        result = kzcStringCopy(memoryManager, defaultValue, &propertyTypeData->defaultValue);
        kzsErrorForward(result);
    }
    else
    {
        propertyTypeData->defaultValue = KZ_NULL;
    }

    *out_propertyType = (struct KzuStringPropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuStringPropertyTypeDelete(struct KzuStringPropertyType* propertyType)
{
    kzsError result;
    kzMutableString defaultValue;

    kzsAssert(kzcIsValidPointer(propertyType));

    defaultValue = ((struct KzuStringPropertyTypeData*)propertyType->propertyType.data)->defaultValue;

    if (defaultValue != KZ_NULL)
    {
        result = kzcStringDelete(defaultValue);
        kzsErrorForward(result);
    }

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuStringPropertyType* kzuStringPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuStringPropertyType*)propertyType;
}

struct KzuPropertyType* kzuStringPropertyTypeToPropertyType(struct KzuStringPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

kzString kzuStringPropertyTypeGetDefaultValue(const struct KzuStringPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuStringPropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuStringPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuStringPropertyType* propertyType, 
                                 kzString value, struct KzuStringProperty** out_property)
{
    kzsError result;
    struct KzuStringProperty* stringProperty;

    result = kzcMemoryAllocVariable(memoryManager, stringProperty, "StringProperty");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&stringProperty->property, (struct KzuPropertyType*)propertyType);

    result = kzcStringCopy(memoryManager, value, &stringProperty->value);
    kzsErrorForward(result);

    *out_property = stringProperty;
    kzsSuccess();
}

kzsError kzuStringPropertyDelete(struct KzuStringProperty* property)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(property));

    result = kzcStringDelete(property->value);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuStringProperty* kzuStringPropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuStringProperty*)property;
}

struct KzuProperty* kzuStringPropertyToProperty(struct KzuStringProperty* stringProperty)
{
    return &stringProperty->property;
}

kzMutableString kzuStringPropertyGetValue(const struct KzuStringProperty* stringProperty)
{
    kzsAssert(kzcIsValidPointer(stringProperty));
    return stringProperty->value;
}

kzsError kzuStringPropertySetValue(struct KzuStringProperty* stringProperty, kzString value)
{
    kzsError result;
    kzMutableString newValue = KZ_NULL;

    kzsAssert(kzcIsValidPointer(stringProperty));

    result = kzcStringCopy(kzcMemoryGetManager(stringProperty->value), value, &newValue);
    kzsErrorForward(result);

    result = kzcStringDelete(stringProperty->value);
    kzsErrorForward(result);

    stringProperty->value = newValue;

    kzsSuccess();
}
