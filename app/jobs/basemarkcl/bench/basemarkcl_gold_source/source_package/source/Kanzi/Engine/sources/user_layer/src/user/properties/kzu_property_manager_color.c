/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property_manager_color.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_color_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


/** Finds float property storage, returns KZ_NULL if not found. */
static struct KzuPropertyColorStorage* kzuPropertyManagerFindColorStorage_internal(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_COLOR);

    return (struct KzuPropertyColorStorage*)kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
}

/** Finds float property storage taking property groups into account, returns KZ_NULL if not found. */
static struct KzuPropertyColorStorage* kzuPropertyManagerFindColorStorageWithGroups_internal(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_COLOR);

    return (struct KzuPropertyColorStorage*)kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, object, typeStorage);
}

kzsError kzuPropertyManagerAcquireColorStorage_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage, struct KzuPropertyColorStorage** out_floatStorage)
{
    kzsError result;
    struct KzuPropertyColorStorage* propertyStorage = kzuPropertyManagerFindColorStorage_internal(object, typeStorage);

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));
    kzsAssert(kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_COLOR);

    if (propertyStorage == KZ_NULL)
    {
        /* Allocate the float storage. */
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyManager), propertyStorage, "Color property storage");
        kzsErrorForward(result);

        kzuPropertyManagerInitializeBaseProperty_private(&propertyStorage->baseStorage);

        propertyStorage->baseValue = KZC_COLOR_PURPLE;
        propertyStorage->relativeValue = KZC_COLOR_BLACK_TRANSPARENT;

        /* Add to the container. */
        result = kzcHashMapPut(typeStorage->propertyContainer, object, propertyStorage);
        kzsErrorForward(result);
    }

    *out_floatStorage = propertyStorage;
    kzsSuccess();
}

kzsError kzuPropertyManagerSetColor(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcColorRGBA value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyColorStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireColorStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->baseValue = value;

    kzsSuccess();
}

kzsError kzuPropertyManagerSetRelativeColor(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcColorRGBA value)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    struct KzuPropertyColorStorage* propertyStorage;

    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    result = kzuPropertyManagerAcquireColorStorage_private(propertyManager, object, typeStorage, &propertyStorage);
    kzsErrorForward(result);

    propertyStorage->relativeValue = value;

    kzsSuccess();
}

kzBool kzuPropertyManagerGetColor(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcColorRGBA* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcColorRGBA value = KZC_COLOR_PURPLE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyColorStorage* propertyStorage = kzuPropertyManagerFindColorStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            kzcColorRGBAAdd(&propertyStorage->baseValue, &propertyStorage->relativeValue, &value);
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetBaseColor(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcColorRGBA* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcColorRGBA value = KZC_COLOR_PURPLE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyColorStorage* propertyStorage = kzuPropertyManagerFindColorStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->baseValue;
        }
    }

    *out_value = value;
    return found;
}

kzBool kzuPropertyManagerGetRelativeColor(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, struct KzcColorRGBA* out_value)
{
    kzBool found = KZ_FALSE;
    struct KzcColorRGBA value = KZC_COLOR_PURPLE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyColorStorage* propertyStorage = kzuPropertyManagerFindColorStorageWithGroups_internal(propertyManager, object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            value = propertyStorage->relativeValue;
        }
    }

    *out_value = value;
    return found;
}

struct KzcColorRGBA kzuPropertyManagerGetColorDefault(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    struct KzcColorRGBA value;

    if (!kzuPropertyManagerGetColor(propertyManager, object, propertyType, &value))
    {
        const struct KzuColorPropertyType* actualType = kzuColorPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuColorPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}

kzsError kzuObjectNodeSetColorProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcColorRGBA value)
{
    kzsError result;

    result = kzuPropertyManagerSetColor(kzuObjectNodeGetPropertyManager(objectNode), objectNode, propertyType, value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuObjectNodeGetColorProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcColorRGBA* out_value)
{
    kzBool found;
    struct KzcColorRGBA value = KZC_COLOR_PURPLE;
    struct KzuPropertyColorStorage* propertyStorage;
    
    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (found)
    {
        kzcColorRGBAAdd(&propertyStorage->baseValue, &propertyStorage->relativeValue, &value);
    }

    *out_value = value;
    return found;
}

kzBool kzuObjectNodeGetBaseColorProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzcColorRGBA* out_value)
{
    kzBool found;
    struct KzcColorRGBA value = KZC_COLOR_PURPLE;
    struct KzuPropertyColorStorage* propertyStorage;

    found = kzuObjectNodeFindPropertyStorage_private(objectNode, propertyType, (struct KzuPropertyBaseStorage**)&propertyStorage);
    if (propertyStorage != KZ_NULL)
    {
        value = propertyStorage->baseValue;
    }

    *out_value = value;
    return found;
}

struct KzcColorRGBA kzuObjectNodeGetColorPropertyDefault(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    struct KzcColorRGBA value;
    if (!kzuObjectNodeGetColorProperty(objectNode, propertyType, &value))
    {
        const struct KzuColorPropertyType* actualType = kzuColorPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType);        
        value = kzuColorPropertyTypeGetDefaultValue(actualType);
    }

    return value;
}
