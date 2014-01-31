/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_int.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_int_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyIntStorage* kzuPropertyManagerFindIntStorage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_INT);

    return (struct KzuPropertyIntStorage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyIntStorage* kzuPropertyManagerFindIntStorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_INT);

    return (struct KzuPropertyIntStorage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireIntStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyIntStorage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyIntStorage* propertyStorage = kzuPropertyManagerFindIntStorage_internal(object, typeStorage);

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_INT);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Int property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        propertyStorage->baseValue = 0;
        propertyStorage->relativeValue = 0;

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetInt(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzInt value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyIntStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireIntStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeInt(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzInt value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyIntStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireIntStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetInt(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzInt* out_value)
{
    kzBool found = KZ_FALSE;
    kzInt value = 0;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyIntStorage* propertyStorage = kzuPropertyManagerFindIntStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue + propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseInt(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzInt* out_value)
{
    kzBool found = KZ_FALSE;
    kzInt value = 0;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyIntStorage* propertyStorage = kzuPropertyManagerFindIntStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeInt(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzInt* out_value)
{
    kzBool found = KZ_FALSE;
    kzInt value = 0;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyIntStorage* propertyStorage = kzuPropertyManagerFindIntStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

kzInt kzuPropertyManagerGetIntDefault(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    kzInt value;

    if (!kzuPropertyManagerGetInt(propertyManager, object, propertyType, &value))
    {
        const struct KzuIntPropertyType* actualType = kzuIntPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuIntPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetIntProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzInt value)
{
    kzsError result;

    result = kzuPropertyManagerSetInt(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetIntProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzInt* out_value)
{
    kzBool found;
    kzInt value = 0;
    struct KzuPropertyIntStorage* propertyStorage;
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        value = propertyStorage->baseValue + propertyStorage->relativeValue;
    }

    *out_value = value;
    return found;
}

kzBool kzuObjectNodeGetBaseIntProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzInt* out_value)
{
    kzBool found;
    kzInt value = 0;
    struct KzuPropertyIntStorage* propertyStorage;
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

kzInt kzuObjectNodeGetIntPropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    kzInt value;
    if (!kzuObjectNodeGetIntProperty(objectNode, propertyType, &value))
    {
        const struct KzuIntPropertyType* actualType = kzuIntPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuIntPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
