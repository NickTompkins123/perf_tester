/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_matrix4x4.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_matrix4x4_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyMatrix4x4Storage* kzuPropertyManagerFindMatrix4x4Storage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_MATRIX4X4);

    return (struct KzuPropertyMatrix4x4Storage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyMatrix4x4Storage* kzuPropertyManagerFindMatrix4x4StorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_MATRIX4X4);

    return (struct KzuPropertyMatrix4x4Storage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireMatrix4x4Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyMatrix4x4Storage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyMatrix4x4Storage* propertyStorage = kzuPropertyManagerFindMatrix4x4Storage_internal(object, typeStorage);
    
    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_MATRIX4X4);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Matrix4x4 property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        kzcMatrix4x4SetInvalid(&propertyStorage->baseValue);
        kzcMatrix4x4SetIdentity(&propertyStorage->relativeValue);

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetMatrix4x4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, const struct KzcMatrix4x4* value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyMatrix4x4Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireMatrix4x4Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = *value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeMatrix4x4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, const struct KzcMatrix4x4* value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyMatrix4x4Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireMatrix4x4Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = *value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetMatrix4x4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix4x4* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcMatrix4x4 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    kzcMatrix4x4SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix4x4Storage* propertyStorage = kzuPropertyManagerFindMatrix4x4StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            kzcMatrix4x4Multiply(&propertyStorage->relativeValue, &propertyStorage->baseValue, &value);
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseMatrix4x4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix4x4* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcMatrix4x4 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzcMatrix4x4SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix4x4Storage* propertyStorage = kzuPropertyManagerFindMatrix4x4StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeMatrix4x4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcMatrix4x4* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcMatrix4x4 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzcMatrix4x4SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix4x4Storage* propertyStorage = kzuPropertyManagerFindMatrix4x4StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

struct KzcMatrix4x4 kzuPropertyManagerGetMatrix4x4Default(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    struct KzcMatrix4x4 value;

    if (!kzuPropertyManagerGetMatrix4x4(propertyManager, object, propertyType, &value))
    {
        const struct KzuMatrix4x4PropertyType* actualType = kzuMatrix4x4PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuMatrix4x4PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetMatrix4x4Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, const struct KzcMatrix4x4* value)
{
    kzsError result;

    result = kzuPropertyManagerSetMatrix4x4(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetMatrix4x4Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcMatrix4x4* out_value)
{
    kzBool found;
    struct KzcMatrix4x4 value;
    struct KzuPropertyMatrix4x4Storage* propertyStorage;

    kzcMatrix4x4SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        kzcMatrix4x4Multiply(&propertyStorage->relativeValue, &propertyStorage->baseValue, &value);
    }

    *out_value = value;
    return found;
}

kzBool kzuObjectNodeGetBaseMatrix4x4Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcMatrix4x4* out_value)
{
    kzBool found;
    struct KzcMatrix4x4 value;
    struct KzuPropertyMatrix4x4Storage* propertyStorage;

    kzcMatrix4x4SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

struct KzcMatrix4x4 kzuObjectNodeGetMatrix4x4PropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    struct KzcMatrix4x4 value;
    if (!kzuObjectNodeGetMatrix4x4Property(objectNode, propertyType, &value))
    {
        const struct KzuMatrix4x4PropertyType* actualType = kzuMatrix4x4PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuMatrix4x4PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
