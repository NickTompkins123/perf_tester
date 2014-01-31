/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_vector4.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_vector4_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyVector4Storage* kzuPropertyManagerFindVector4Storage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VECTOR4);

    return (struct KzuPropertyVector4Storage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyVector4Storage* kzuPropertyManagerFindVector4StorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VECTOR4);

    return (struct KzuPropertyVector4Storage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireVector4Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyVector4Storage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyVector4Storage* propertyStorage = kzuPropertyManagerFindVector4Storage_internal(object, typeStorage);
    
    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VECTOR4);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Vector4 property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        kzcVector4SetInvalid(&propertyStorage->baseValue);
        propertyStorage->relativeValue = KZC_VECTOR4_ZERO;

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetVector4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector4 value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyVector4Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireVector4Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeVector4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector4 value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyVector4Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireVector4Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetVector4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector4* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcVector4 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    kzcVector4SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVector4Storage* propertyStorage = kzuPropertyManagerFindVector4StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            kzcVector4Add(&propertyStorage->baseValue, &propertyStorage->relativeValue, &value);
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseVector4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector4* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcVector4 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzcVector4SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVector4Storage* propertyStorage = kzuPropertyManagerFindVector4StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeVector4(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector4* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcVector4 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzcVector4SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVector4Storage* propertyStorage = kzuPropertyManagerFindVector4StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

struct KzcVector4 kzuPropertyManagerGetVector4Default(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    struct KzcVector4 value;

    if (!kzuPropertyManagerGetVector4(propertyManager, object, propertyType, &value))
    {
        const struct KzuVector4PropertyType* actualType = kzuVector4PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuVector4PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetVector4Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcVector4 value)
{
    kzsError result;

    result = kzuPropertyManagerSetVector4(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetVector4Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcVector4* out_value)
{
    kzBool found;
    struct KzcVector4 value;
    struct KzuPropertyVector4Storage* propertyStorage;

    kzcVector4SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        kzcVector4Add(&propertyStorage->baseValue, &propertyStorage->relativeValue, &value);
    }

    *out_value = value;
    return found;
}

kzBool kzuObjectNodeGetBaseVector4Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcVector4* out_value)
{
    kzBool found;
    struct KzcVector4 value;
    struct KzuPropertyVector4Storage* propertyStorage;

    kzcVector4SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

struct KzcVector4 kzuObjectNodeGetVector4PropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    struct KzcVector4 value;
    if (!kzuObjectNodeGetVector4Property(objectNode, propertyType, &value))
    {
        const struct KzuVector4PropertyType* actualType = kzuVector4PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuVector4PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
