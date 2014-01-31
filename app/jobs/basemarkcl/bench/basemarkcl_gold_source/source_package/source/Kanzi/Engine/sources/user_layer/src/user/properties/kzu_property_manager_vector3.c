/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_vector3.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_vector3_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyVector3Storage* kzuPropertyManagerFindVector3Storage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VECTOR3);

    return (struct KzuPropertyVector3Storage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyVector3Storage* kzuPropertyManagerFindVector3StorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VECTOR3);

    return (struct KzuPropertyVector3Storage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireVector3Storage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyVector3Storage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyVector3Storage* propertyStorage = kzuPropertyManagerFindVector3Storage_internal(object, typeStorage);

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VECTOR3);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Vector3 property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        kzcVector3SetInvalid(&propertyStorage->baseValue);
        propertyStorage->relativeValue = KZC_VECTOR3_ZERO;

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetVector3(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector3 value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyVector3Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireVector3Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeVector3(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector3 value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyVector3Storage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireVector3Storage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetVector3(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector3* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcVector3 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    kzcVector3SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVector3Storage* propertyStorage = kzuPropertyManagerFindVector3StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            kzcVector3Add(&propertyStorage->baseValue, &propertyStorage->relativeValue, &value);
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseVector3(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector3* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcVector3 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzcVector3SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVector3Storage* propertyStorage = kzuPropertyManagerFindVector3StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeVector3(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcVector3* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcVector3 value;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzcVector3SetInvalid(&value);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVector3Storage* propertyStorage = kzuPropertyManagerFindVector3StorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

struct KzcVector3 kzuPropertyManagerGetVector3Default(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    struct KzcVector3 value;

    if (!kzuPropertyManagerGetVector3(propertyManager, object, propertyType, &value))
    {
        const struct KzuVector3PropertyType* actualType = kzuVector3PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuVector3PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetVector3Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcVector3 value)
{
    kzsError result;

    result = kzuPropertyManagerSetVector3(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetVector3Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcVector3* out_value)
{
    kzBool found;
    struct KzcVector3 value;
    struct KzuPropertyVector3Storage* propertyStorage;

    kzcVector3SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        kzcVector3Add(&propertyStorage->baseValue, &propertyStorage->relativeValue, &value);
    }

    *out_value = value;
    return found;
}

kzBool kzuObjectNodeGetBaseVector3Property(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcVector3* out_value)
{
    kzBool found;
    struct KzcVector3 value;
    struct KzuPropertyVector3Storage* propertyStorage;

    kzcVector3SetInvalid(&value);
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

struct KzcVector3 kzuObjectNodeGetVector3PropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    struct KzcVector3 value;
    if (!kzuObjectNodeGetVector3Property(objectNode, propertyType, &value))
    {
        const struct KzuVector3PropertyType* actualType = kzuVector3PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuVector3PropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
