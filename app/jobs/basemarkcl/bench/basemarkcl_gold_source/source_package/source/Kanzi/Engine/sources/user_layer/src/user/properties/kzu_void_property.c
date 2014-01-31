/**
* \file
* Specifies property that holds a pointer to some data.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_void_property.h"

#include "kzu_property_private.h"

#include <core/memory/kzc_memory_manager.h>


struct KzuVoidPropertyType
{
    struct KzuPropertyType propertyType;    /**< Parent property type. */
};

struct KzuVoidPropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuVoidPropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    void* defaultValue; /**< Default value. */
};

struct KzuVoidProperty
{
    struct KzuProperty property;    /**< Property. Used for inheritance. */
    void* value;          /**< Value. */
};


kzsError kzuVoidPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, void* defaultValue,
                                   struct KzuVoidPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuVoidPropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Void property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_VOID, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuVoidPropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuVoidPropertyTypeDelete(struct KzuVoidPropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuVoidPropertyType* kzuVoidPropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuVoidPropertyType*)propertyType;
}

struct KzuPropertyType* kzuVoidPropertyTypeToPropertyType(struct KzuVoidPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

void* kzuVoidPropertyTypeGetDefaultValue(const struct KzuVoidPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuVoidPropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuVoidPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuVoidPropertyType* propertyType, 
                                 void* value, struct KzuVoidProperty** out_property)
{
    kzsError result;
    struct KzuVoidProperty* voidProperty;

    result = kzcMemoryAllocVariable(memoryManager, voidProperty, "Void property");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&voidProperty->property, (struct KzuPropertyType*)propertyType);

    voidProperty->value = value;

    *out_property = voidProperty;
    kzsSuccess();
}

kzsError kzuVoidPropertyDelete(struct KzuVoidProperty* property)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(property));

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuVoidProperty* kzuVoidPropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuVoidProperty*)property;
}

struct KzuProperty* kzuVoidPropertyToProperty(struct KzuVoidProperty* property)
{
    return &property->property;
}

void* kzuVoidPropertyGetValue(const struct KzuVoidProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return property->value;
}

void kzuVoidPropertySetValue(struct KzuVoidProperty* property, void* value)
{
    kzsAssert(kzcIsValidPointer(property));
    property->value = value;
}
