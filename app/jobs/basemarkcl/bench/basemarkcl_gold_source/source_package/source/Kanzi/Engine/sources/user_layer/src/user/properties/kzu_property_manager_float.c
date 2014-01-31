/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_float.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_float_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyFloatStorage* kzuPropertyManagerFindFloatStorage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_FLOAT);

    return (struct KzuPropertyFloatStorage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyFloatStorage* kzuPropertyManagerFindFloatStorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_FLOAT);

    return (struct KzuPropertyFloatStorage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireFloatStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyFloatStorage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyFloatStorage* propertyStorage = kzuPropertyManagerFindFloatStorage_internal(object, typeStorage);

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_FLOAT);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Float property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        propertyStorage->baseValue = 0.0f;
        propertyStorage->relativeValue = 0.0f;

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetFloat(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzFloat value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyFloatStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireFloatStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeFloat(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzFloat value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyFloatStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireFloatStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetFloat(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzFloat* out_value)
{
    kzBool found = KZ_FALSE;
    kzFloat value = 0.0f;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyFloatStorage* propertyStorage = kzuPropertyManagerFindFloatStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue + propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseFloat(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzFloat* out_value)
{
    kzBool found = KZ_FALSE;
    kzFloat value = 0.0f;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyFloatStorage* propertyStorage = kzuPropertyManagerFindFloatStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeFloat(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzFloat* out_value)
{
    kzBool found = KZ_FALSE;
    kzFloat value = 0.0f;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyFloatStorage* propertyStorage = kzuPropertyManagerFindFloatStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

kzFloat kzuPropertyManagerGetFloatDefault(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    kzFloat value;

    if (!kzuPropertyManagerGetFloat(propertyManager, object, propertyType, &value))
    {
        const struct KzuFloatPropertyType* actualType = kzuFloatPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuFloatPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetFloatProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzFloat value)
{
    kzsError result;

    result = kzuPropertyManagerSetFloat(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

/* TODO: Implement other types with similar code than here. */
kzBool kzuObjectNodeGetFloatProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzFloat* out_value)
{
    kzBool found;
    kzFloat value = 0.0f;
    struct KzuPropertyFloatStorage* propertyStorage;
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        value = propertyStorage->baseValue + propertyStorage->relativeValue;
    }

    *out_value = value;
    return found;
}

kzBool kzuObjectNodeGetBaseFloatProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzFloat* out_value)
{
    kzBool found;
    kzFloat value = 0.0f;
    struct KzuPropertyFloatStorage* propertyStorage;

    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (propertyStorage != KZ_NULL)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

kzFloat kzuObjectNodeGetFloatPropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    kzFloat value;
    if (!kzuObjectNodeGetFloatProperty(objectNode, propertyType, &value))
    {
        const struct KzuFloatPropertyType* actualType = kzuFloatPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuFloatPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
