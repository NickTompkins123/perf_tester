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
#include <user/properties/kzu_void_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyVoidStorage* kzuPropertyManagerFindVoidStorage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VOID ||
              kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_TEXTURE);

    return (struct KzuPropertyVoidStorage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyVoidStorage* kzuPropertyManagerFindVoidStorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VOID ||
              kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_TEXTURE);

    return (struct KzuPropertyVoidStorage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireVoidStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyVoidStorage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyVoidStorage* propertyStorage = kzuPropertyManagerFindVoidStorage_internal(object, typeStorage);

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_VOID ||
              kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_TEXTURE);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Void property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        propertyStorage->baseValue = KZ_NULL;
        propertyStorage->relativeValue = KZ_NULL;

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetVoid(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, void* value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyVoidStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireVoidStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeVoid(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, void* value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyVoidStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireVoidStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetVoid(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, void** out_value)
{
    kzBool found = KZ_FALSE;
    void* value = KZ_NULL;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVoidStorage* propertyStorage = kzuPropertyManagerFindVoidStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseVoid(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, void** out_value)
{
    kzBool found = KZ_FALSE;
    void* value = KZ_NULL;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVoidStorage* propertyStorage = kzuPropertyManagerFindVoidStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeVoid(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, void** out_value)
{
    kzBool found = KZ_FALSE;
    void* value = KZ_NULL;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyVoidStorage* propertyStorage = kzuPropertyManagerFindVoidStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

void* kzuPropertyManagerGetVoidDefault(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    void* value;

    if (!kzuPropertyManagerGetVoid(propertyManager, object, propertyType, &value))
    {
        const struct KzuVoidPropertyType* actualType = kzuVoidPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuVoidPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetVoidProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, void* value)
{
    kzsError result;

    result = kzuPropertyManagerSetVoid(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetVoidProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, void** out_value)
{
    return kzuObjectNodeGetBaseVoidProperty(objectNode, propertyType, out_value);
}

kzBool kzuObjectNodeGetBaseVoidProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, void** out_value)
{
    kzBool found;
    void* value = KZ_NULL;
    struct KzuPropertyVoidStorage* propertyStorage;

    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (propertyStorage != KZ_NULL)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

void* kzuObjectNodeGetVoidPropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    void* value;
    if (!kzuObjectNodeGetVoidProperty(objectNode, propertyType, &value))
    {
        const struct KzuVoidPropertyType* actualType = kzuVoidPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuVoidPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
