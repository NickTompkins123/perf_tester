/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_bool.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_bool_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyBoolStorage* kzuPropertyManagerFindBoolStorage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_BOOL);

    return (struct KzuPropertyBoolStorage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyBoolStorage* kzuPropertyManagerFindBoolStorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_BOOL);

    return (struct KzuPropertyBoolStorage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireBoolStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyBoolStorage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyBoolStorage* propertyStorage = kzuPropertyManagerFindBoolStorage_internal(object, typeStorage);

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_BOOL);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Bool property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        propertyStorage->baseValue = KZ_FALSE;
        propertyStorage->relativeValue = KZ_FALSE;

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetBool(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzBool value )
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyBoolStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireBoolStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeBool(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzBool value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyBoolStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireBoolStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetBool(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzBool* out_value)
{
    kzBool found = KZ_FALSE;
    kzBool value = KZ_FALSE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyBoolStorage* propertyStorage = kzuPropertyManagerFindBoolStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue + propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseBool(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzBool* out_value)
{
    kzBool found = KZ_FALSE;
    kzBool value = KZ_FALSE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyBoolStorage* propertyStorage = kzuPropertyManagerFindBoolStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeBool(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzBool* out_value)
{
    kzBool found = KZ_FALSE;
    kzBool value = KZ_FALSE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyBoolStorage* propertyStorage = kzuPropertyManagerFindBoolStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBoolDefault(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    kzBool value;

    if (!kzuPropertyManagerGetBool(propertyManager, object, propertyType, &value))
    {
        const struct KzuBoolPropertyType* actualType = kzuBoolPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuBoolPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetBoolProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzBool value)
{
    kzsError result;

    result = kzuPropertyManagerSetBool(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetBoolProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzBool* out_value)
{
    return kzuObjectNodeGetBaseBoolProperty(objectNode, propertyType, out_value);
}

kzBool kzuObjectNodeGetBaseBoolProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzBool* out_value)
{
    kzBool found;
    kzBool value = KZ_FALSE;
    struct KzuPropertyBoolStorage* propertyStorage;

    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (propertyStorage != KZ_NULL)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

kzBool kzuObjectNodeGetBoolPropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    kzBool value;
    if (!kzuObjectNodeGetBoolProperty(objectNode, propertyType, &value))
    {
        const struct KzuBoolPropertyType* actualType = kzuBoolPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuBoolPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
