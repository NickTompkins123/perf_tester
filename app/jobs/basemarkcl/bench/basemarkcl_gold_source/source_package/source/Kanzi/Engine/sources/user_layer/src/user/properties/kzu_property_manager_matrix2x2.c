/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_matrix2x2.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_matrix2x2_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyMatrix2x2Storage* kzuPropertyManagerFindMatrix2x2Storage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_MATRIX2X2);

    return (struct KzuPropertyMatrix2x2Storage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyMatrix2x2Storage* kzuPropertyManagerFindMatrix2x2StorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_MATRIX2X2);

    return (struct KzuPropertyMatrix2x2Storage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireMatrix2x2Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyMatrix2x2Storage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyMatrix2x2Storage* propertyStorage = kzuPropertyManagerFindMatrix2x2Storage_internal(object, typeStorage);

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_MATRIX2X2);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Matrix2x2 property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        kzcMatrix2x2SetInvalid(&propertyStorage->baseValue);
        kzcMatrix2x2SetIdentity(&propertyStorage->relativeValue);

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetMatrix2x2(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, const struct KzcMatrix2x2* value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyMatrix2x2Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireMatrix2x2Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = *value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeMatrix2x2(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, const struct KzcMatrix2x2* value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyMatrix2x2Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireMatrix2x2Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = *value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetMatrix2x2(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix2x2* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcMatrix2x2 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    kzcMatrix2x2SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix2x2Storage* propertyStorage = kzuPropertyManagerFindMatrix2x2StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            kzcMatrix2x2Multiply(&propertyStorage->relativeValue, &propertyStorage->baseValue, &value);
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseMatrix2x2(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix2x2* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcMatrix2x2 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    kzcMatrix2x2SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix2x2Storage* propertyStorage = kzuPropertyManagerFindMatrix2x2StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeMatrix2x2(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix2x2* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcMatrix2x2 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    kzcMatrix2x2SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix2x2Storage* propertyStorage = kzuPropertyManagerFindMatrix2x2StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

struct KzcMatrix2x2 kzuPropertyManagerGetMatrix2x2Default(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    struct KzcMatrix2x2 value;

    if (!kzuPropertyManagerGetMatrix2x2(propertyManager, object, propertyType, &value))
    {
        const struct KzuMatrix2x2PropertyType* actualType = kzuMatrix2x2PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuMatrix2x2PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetMatrix2x2Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, const struct KzcMatrix2x2* value)
{
    kzsError result;

    result = kzuPropertyManagerSetMatrix2x2(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetMatrix2x2Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcMatrix2x2* out_value)
{
    kzBool found;
    struct KzcMatrix2x2 value;
    struct KzuPropertyMatrix2x2Storage* propertyStorage;

    kzcMatrix2x2SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        kzcMatrix2x2Multiply(&propertyStorage->relativeValue, &propertyStorage->baseValue, &value);
    }

    *out_value = value;
    return found;
}

kzBool kzuObjectNodeGetBaseMatrix2x2Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcMatrix2x2* out_value)
{
    kzBool found;
    struct KzcMatrix2x2 value;
    struct KzuPropertyMatrix2x2Storage* propertyStorage;

    kzcMatrix2x2SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

struct KzcMatrix2x2 kzuObjectNodeGetMatrix2x2PropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    struct KzcMatrix2x2 value;
    if (!kzuObjectNodeGetMatrix2x2Property(objectNode, propertyType, &value))
    {
        const struct KzuMatrix2x2PropertyType* actualType = kzuMatrix2x2PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuMatrix2x2PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
