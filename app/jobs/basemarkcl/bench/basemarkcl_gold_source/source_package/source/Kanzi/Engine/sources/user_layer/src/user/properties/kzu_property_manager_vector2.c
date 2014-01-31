/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_vector2.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_vector2_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyVector2Storage* kzuPropertyManagerFindVector2Storage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VECTOR2);

    return (struct KzuPropertyVector2Storage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyVector2Storage* kzuPropertyManagerFindVector2StorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VECTOR2);

    return (struct KzuPropertyVector2Storage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireVector2Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyVector2Storage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyVector2Storage* propertyStorage = kzuPropertyManagerFindVector2Storage_internal(object, typeStorage);

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VECTOR2);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Vector2 property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        kzcVector2SetInvalid(&propertyStorage->baseValue);
        propertyStorage->relativeValue = KZC_VECTOR2_ZERO;

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetVector2(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector2 value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyVector2Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireVector2Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeVector2(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector2 value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyVector2Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireVector2Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetVector2(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector2* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcVector2 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    kzcVector2SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVector2Storage* propertyStorage = kzuPropertyManagerFindVector2StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            kzcVector2Add(&propertyStorage->baseValue, &propertyStorage->relativeValue, &value);
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseVector2(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector2* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcVector2 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzcVector2SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVector2Storage* propertyStorage = kzuPropertyManagerFindVector2StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeVector2(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector2* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcVector2 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzcVector2SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVector2Storage* propertyStorage = kzuPropertyManagerFindVector2StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

struct KzcVector2 kzuPropertyManagerGetVector2Default(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    struct KzcVector2 value;

    if (!kzuPropertyManagerGetVector2(propertyManager, object, propertyType, &value))
    {
        const struct KzuVector2PropertyType* actualType = kzuVector2PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuVector2PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetVector2Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcVector2 value)
{
    kzsError result;

    result = kzuPropertyManagerSetVector2(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetVector2Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcVector2* out_value)
{
    kzBool found;
    struct KzcVector2 value;
    struct KzuPropertyVector2Storage* propertyStorage;

    kzcVector2SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        kzcVector2Add(&propertyStorage->baseValue, &propertyStorage->relativeValue, &value);
    }

    *out_value = value;
    return found;
}

kzBool kzuObjectNodeGetBaseVector2Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcVector2* out_value)
{
    kzBool found;
    struct KzcVector2 value;
    struct KzuPropertyVector2Storage* propertyStorage;

    kzcVector2SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

struct KzcVector2 kzuObjectNodeGetVector2PropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    struct KzcVector2 value;
    if (!kzuObjectNodeGetVector2Property(objectNode, propertyType, &value))
    {
        const struct KzuVector2PropertyType* actualType = kzuVector2PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuVector2PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
