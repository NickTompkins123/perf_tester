/**
* \file
* Property manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/scene_graph/kzu_object.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


void kzuPropertyManagerInitializeBaseProperty_private(struct KzuPropertyBaseStorage* property)
{
    property->priority = KZU_PROPERTY_PRIORITY_NORMAL;
    property->flags = 0;
}

struct KzuPropertyTypeStorage* kzuPropertyManagerFindTypeStorage_private(const struct KzuPropertyManager* propertyManager, const struct KzuPropertyType* propertyType)
{
    kzBool found;
    struct KzuPropertyTypeStorage* foundStorage;

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(propertyType));

    found = kzcHashMapGet(propertyManager->typeContainer, propertyType, (void**)&foundStorage);
    KZ_UNUSED_RETURN_VALUE(found);

    return foundStorage;
}

struct KzuPropertyBaseStorage* kzuPropertyManagerFindPropertyStorage_private(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzBool found;
    struct KzuPropertyBaseStorage* foundStorage;    

    kzsAssert(kzcIsValidPointer(typeStorage));

    found = kzcHashMapGet(typeStorage->propertyContainer, object, (void**)&foundStorage);
    KZ_UNUSED_RETURN_VALUE(found);

    return foundStorage;
}

struct KzuPropertyBaseStorage* kzuPropertyManagerFindPropertyStorageWithGroups_private(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    struct KzuPropertyBaseStorage* propertyStorage;
    struct KzcDynamicArray* groupArray;

    kzsAssert(kzcIsValidPointer(propertyManager));
    kzsAssert(kzcIsValidPointer(typeStorage));

    /* Find the storage for the object itself. */
    propertyStorage = kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);

    /* Search the groups. */
    if (kzcHashMapGet(propertyManager->groupContainer, object, (void**)&groupArray))
    {
        /* Process groups in reverse order. */
        kzUint i = kzcDynamicArrayGetSize(groupArray);
        for (; i != 0; --i)
        {
            struct KzuPropertyGroup* group = kzcDynamicArrayGet(groupArray, i - 1);
            struct KzuPropertyBaseStorage* otherPropertyStorage = kzuPropertyManagerFindPropertyStorage_private(group, typeStorage);
            if (otherPropertyStorage != KZ_NULL)
            {
                if (propertyStorage == KZ_NULL || (propertyStorage->priority < otherPropertyStorage->priority))
                {
                    propertyStorage = otherPropertyStorage;
                }
            }
        }
    }

    return propertyStorage;
}

struct KzuPropertyBaseStorage* kzuObjectNodeFindPropertyStorageWithGroups_private(const struct KzuObjectNode* objectNode, const struct KzuPropertyTypeStorage* typeStorage,
                                                                                  const struct KzuObjectNode** out_objectNode)
{
    struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(objectNode);
    const struct KzuObjectNode* currentObjectNode = objectNode;
    struct KzuPropertyBaseStorage* propertyStorage = KZ_NULL;
    const struct KzuObjectNode* sourceObjectNode = KZ_NULL;
    kzBool inherited;

    kzsAssert(kzcIsValidPointer(typeStorage));
    inherited = kzuPropertyTypeGetInherited(typeStorage->type);

    while (currentObjectNode != KZ_NULL)
    {
        struct KzuPropertyBaseStorage* otherPropertyStorage = kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyManager, currentObjectNode, typeStorage);
        if (otherPropertyStorage != KZ_NULL)
        {
            if (propertyStorage == KZ_NULL || (propertyStorage->priority < otherPropertyStorage->priority))
            {
                propertyStorage = otherPropertyStorage;
                sourceObjectNode = currentObjectNode;
            }
        }

        if (!inherited)
        {
            /* Property does not support inheritance. */
            break;
        }

        currentObjectNode = kzuObjectNodeGetParent(currentObjectNode);
    }

    *out_objectNode = sourceObjectNode;
    return propertyStorage;
}

kzBool kzuObjectNodeFindPropertyStorage_private(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType, struct KzuPropertyBaseStorage** out_propertyStorage)
{
    struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(objectNode);
    struct KzuPropertyBaseStorage* propertyStorage = KZ_NULL;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    if (typeStorage != KZ_NULL)
    {
        const struct KzuObjectNode* unusedNode;
        propertyStorage = kzuObjectNodeFindPropertyStorageWithGroups_private(objectNode, typeStorage, &unusedNode);
    }

    *out_propertyStorage = propertyStorage;
    return propertyStorage != KZ_NULL;
}

kzsError kzuPropertyManagerRemovePropertyStorage_private(const void* object, const struct KzuPropertyTypeStorage* typeStorage)
{
    kzsError result;    
    struct KzuPropertyBaseStorage* property;

    if (kzcHashMapGet(typeStorage->propertyContainer, object, (void**)&property))
    {
        if (kzuPropertyTypeGetDataType(typeStorage->type) == KZU_PROPERTY_DATA_TYPE_STRING)
        {
            struct KzuPropertyStringStorage* stringStorage = (struct KzuPropertyStringStorage*)property;

            if (stringStorage->baseValue != KZ_NULL)
            {
                result = kzcStringDelete(stringStorage->baseValue);
                kzsErrorForward(result);
            }
            if (stringStorage->relativeValue != KZ_NULL)
            {
                result = kzcStringDelete(stringStorage->relativeValue);
                kzsErrorForward(result);
            }
        }

        result = kzcMemoryFreeVariable(property);
        kzsErrorForward(result);

        result = kzcHashMapRemove(typeStorage->propertyContainer, object);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

/** Gets a property type storage if it exists, or creates new storage if it is not found. */
kzsError kzuPropertyManagerAcquireTypeStorage_private(const struct KzuPropertyManager* propertyManager, const struct KzuPropertyType* propertyType, struct KzuPropertyTypeStorage** out_storage)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzuPropertyTypeIsValid(propertyType));
    kzsAssert(kzcIsValidPointer(propertyManager));

    if (typeStorage == KZ_NULL)
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(propertyManager);
        kzMutableString propertyTypeName;

        /* Allocate the storage itself. */
        result = kzcMemoryAllocVariable(memoryManager, typeStorage, "Property type storage");
        kzsErrorForward(result);

        typeStorage->type = propertyType;

        /* Allocate the property container. */
        result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &typeStorage->propertyContainer);
        kzsErrorForward(result);

        /* Add the storage to the registry. */
        result = kzcHashMapPut(propertyManager->typeContainer, propertyType, typeStorage);
        kzsErrorForward(result);

        result = kzcStringCopy(memoryManager, kzuPropertyTypeGetName(propertyType), &propertyTypeName);
        kzsErrorForward(result);

        result = kzcHashMapPut(propertyManager->typesByNames, propertyTypeName, (void*)propertyType);
        kzsErrorForward(result);
    }

    *out_storage = typeStorage;
    kzsSuccess();
}
