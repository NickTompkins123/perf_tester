/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_matrix3x3.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_matrix3x3_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyMatrix3x3Storage* kzuPropertyManagerFindMatrix3x3Storage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_MATRIX3X3);

    return (struct KzuPropertyMatrix3x3Storage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyMatrix3x3Storage* kzuPropertyManagerFindMatrix3x3StorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_MATRIX3X3);

    return (struct KzuPropertyMatrix3x3Storage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireMatrix3x3Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyMatrix3x3Storage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyMatrix3x3Storage* propertyStorage = kzuPropertyManagerFindMatrix3x3Storage_internal(object, typeStorage);

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_MATRIX3X3);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Matrix3x3 property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        kzcMatrix3x3SetInvalid(&propertyStorage->baseValue);
        kzcMatrix3x3SetIdentity(&propertyStorage->relativeValue);

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetMatrix3x3(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, const struct KzcMatrix3x3* value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyMatrix3x3Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireMatrix3x3Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = *value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeMatrix3x3(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, const struct KzcMatrix3x3* value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyMatrix3x3Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireMatrix3x3Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = *value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetMatrix3x3(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix3x3* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcMatrix3x3 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    kzcMatrix3x3SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix3x3Storage* propertyStorage = kzuPropertyManagerFindMatrix3x3StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            kzcMatrix3x3Multiply(&propertyStorage->relativeValue, &propertyStorage->baseValue, &value);
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseMatrix3x3(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix3x3* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcMatrix3x3 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzcMatrix3x3SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix3x3Storage* propertyStorage = kzuPropertyManagerFindMatrix3x3StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeMatrix3x3(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix3x3* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcMatrix3x3 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzcMatrix3x3SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix3x3Storage* propertyStorage = kzuPropertyManagerFindMatrix3x3StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

struct KzcMatrix3x3 kzuPropertyManagerGetMatrix3x3Default(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    struct KzcMatrix3x3 value;

    if (!kzuPropertyManagerGetMatrix3x3(propertyManager, object, propertyType, &value))
    {
        const struct KzuMatrix3x3PropertyType* actualType = kzuMatrix3x3PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuMatrix3x3PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetMatrix3x3Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, const struct KzcMatrix3x3* value)
{
    kzsError result;

    result = kzuPropertyManagerSetMatrix3x3(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetMatrix3x3Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcMatrix3x3* out_value)
{
    kzBool found;
    struct KzcMatrix3x3 value;
    struct KzuPropertyMatrix3x3Storage* propertyStorage;

    kzcMatrix3x3SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        kzcMatrix3x3Multiply(&propertyStorage->relativeValue, &propertyStorage->baseValue, &value);
    }

    *out_value = value;
    return found;
}

kzBool kzuObjectNodeGetBaseMatrix3x3Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcMatrix3x3* out_value)
{
    kzBool found;
    struct KzcMatrix3x3 value;
    struct KzuPropertyMatrix3x3Storage* propertyStorage;

    kzcMatrix3x3SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

struct KzcMatrix3x3 kzuObjectNodeGetMatrix3x3PropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    struct KzcMatrix3x3 value;
    if (!kzuObjectNodeGetMatrix3x3Property(objectNode, propertyType, &value))
    {
        const struct KzuMatrix3x3PropertyType* actualType = kzuMatrix3x3PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuMatrix3x3PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
