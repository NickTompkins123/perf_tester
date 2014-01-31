/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_string.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_string_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


kzsError kzuPropertyManagerAssignBaseString_internal(struct KzuPropertyStringStorage* stringStorage, kzString string)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(stringStorage));

    if (stringStorage->baseValue != KZ_NULL)
    {
        result = kzcStringDelete(stringStorage->baseValue);
        kzsErrorForward(result);
    }

    if (string == KZ_NULL)
    {
        stringStorage->baseValue = KZ_NULL;
    }
    else
    {
        result = kzcStringCopy(kzcMemoryGetManager(stringStorage), string, &stringStorage->baseValue);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuPropertyManagerAssignRelativeString_internal(struct KzuPropertyStringStorage* stringStorage, kzString string)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(stringStorage));

    if (stringStorage->relativeValue != KZ_NULL)
    {
        result = kzcStringDelete(stringStorage->relativeValue);
        kzsErrorForward(result);
    }

    if (string == KZ_NULL)
    {
        stringStorage->relativeValue = KZ_NULL;
    }
    else
    {
        result = kzcStringCopy(kzcMemoryGetManager(stringStorage), string, &stringStorage->relativeValue);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyStringStorage* kzuPropertyManagerFindStringStorage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_STRING);

    return (struct KzuPropertyStringStorage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyStringStorage* kzuPropertyManagerFindStringStorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_STRING);

    return (struct KzuPropertyStringStorage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireStringStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyStringStorage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyStringStorage* propertyStorage = kzuPropertyManagerFindStringStorage_internal(object, typeStorage);

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_STRING);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "String property storage");
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

kzsError kzuPropertyManagerSetString(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzString value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyStringStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireStringStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAssignBaseString_internal(propertyStorage, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeString(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzString value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyStringStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireStringStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAssignRelativeString_internal(propertyStorage, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuPropertyManagerGetString(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzString* out_value)
{
    kzBool found = KZ_FALSE;
    kzString value = KZ_NULL;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyStringStorage* propertyStorage = kzuPropertyManagerFindStringStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseString(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzString* out_value)
{
    kzBool found = KZ_FALSE;
    kzString value = KZ_NULL;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyStringStorage* propertyStorage = kzuPropertyManagerFindStringStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeString(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzString* out_value)
{
    kzBool found = KZ_FALSE;
    kzString value = KZ_NULL;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyStringStorage* propertyStorage = kzuPropertyManagerFindStringStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

kzString kzuPropertyManagerGetStringDefault(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    kzString value;

    if (!kzuPropertyManagerGetString(propertyManager, object, propertyType, &value))
    {
        const struct KzuStringPropertyType* actualType = kzuStringPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuStringPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetStringProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzString value)
{
    kzsError result;

    result = kzuPropertyManagerSetString(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetStringProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzString* out_value)
{
    return kzuObjectNodeGetBaseStringProperty(objectNode, propertyType, out_value);
}

kzBool kzuObjectNodeGetBaseStringProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, kzString* out_value)
{
    kzBool found;
    kzString value = KZ_NULL;
    struct KzuPropertyStringStorage* propertyStorage;
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

kzString kzuObjectNodeGetStringPropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    kzString value;
    if (!kzuObjectNodeGetStringProperty(objectNode, propertyType, &value))
    {
        const struct KzuStringPropertyType* actualType = kzuStringPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuStringPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
