/**
* \file
* Property query.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property_query.h"

#include "kzu_property_manager.h"
#include "kzu_property_manager_private.h"
#include "kzu_property.h"
#include "kzu_float_property.h"

#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_vector2_property.h>
#include <user/properties/kzu_vector3_property.h>
#include <user/properties/kzu_vector4_property.h>
#include <user/properties/kzu_matrix2x2_property.h>
#include <user/properties/kzu_matrix3x3_property.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_void_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_linked_list.h>


struct KzuPropertyQuery
{
    struct KzuPropertyManager* propertyManager;
    struct KzcLinkedList* stack;
};

struct KzuPropertyQueryEntry
{
    union
    {
        const void* anonymousObject;
        const struct KzuObjectNode* objectNode;
    } object;

    kzBool isObjectNode;
};


kzsError kzuPropertyQueryCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                struct KzuPropertyQuery** out_propertyQuery)
{
    kzsError result;
    struct KzuPropertyQuery* propertyQuery;

    result = kzcMemoryAllocVariable(memoryManager, propertyQuery, "Property query");
    kzsErrorForward(result);

    propertyQuery->propertyManager = propertyManager;

    result = kzcLinkedListCreate(memoryManager, &propertyQuery->stack);
    kzsErrorForward(result);

    *out_propertyQuery = propertyQuery;
    kzsSuccess();
}

kzsError kzuPropertyQueryDelete(struct KzuPropertyQuery* propertyQuery)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyQuery));

    result = kzcLinkedListDelete(propertyQuery->stack);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(propertyQuery);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuPropertyManager* kzuPropertyQueryGetPropertyManager(const struct KzuPropertyQuery* propertyQuery)
{
    kzsAssert(kzcIsValidPointer(propertyQuery));

    return propertyQuery->propertyManager;
}

kzsError kzuPropertyQueryPushObject(const struct KzuPropertyQuery* propertyQuery, const void* object)
{
    kzsError result;
    struct KzuPropertyQueryEntry* entry;

    kzsAssert(kzcIsValidPointer(propertyQuery));

    result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyQuery), entry, "Property query entry");
    kzsErrorForward(result);

    entry->object.anonymousObject = object;
    entry->isObjectNode = KZ_FALSE;

    result = kzcLinkedListAdd(propertyQuery->stack, entry);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuPropertyQueryPushObjectNode(const struct KzuPropertyQuery* propertyQuery, const struct KzuObjectNode* objectNode)
{
    kzsError result;
    struct KzuPropertyQueryEntry* entry;

    kzsAssert(kzcIsValidPointer(propertyQuery));

    result = kzcMemoryAllocVariable(kzcMemoryGetManager(propertyQuery), entry, "Property query entry");
    kzsErrorForward(result);

    entry->object.anonymousObject = objectNode;
    entry->isObjectNode = KZ_TRUE;

    result = kzcLinkedListAdd(propertyQuery->stack, entry);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuPropertyQueryPopObject(const struct KzuPropertyQuery* propertyQuery)
{
    kzsError result;
    struct KzuPropertyQueryEntry* entry;

    kzsAssert(kzcIsValidPointer(propertyQuery));

    result = kzcLinkedListGetLast(propertyQuery->stack, (void**)&entry);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(entry);
    kzsErrorForward(result);

    result = kzcLinkedListRemoveLast(propertyQuery->stack);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Finds property storate from property query. */
static struct KzuPropertyBaseStorage* kzuPropertyQueryFindPropertyStorage_internal(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    struct KzuPropertyBaseStorage* propertyStorage = KZ_NULL;
    struct KzuPropertyTypeStorage* typeStorage;
    
    kzsAssert(kzcIsValidPointer(propertyQuery));

    typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyQuery->propertyManager, propertyType);

    if (typeStorage != KZ_NULL)
    {
        struct KzcLinkedListIterator it;

        it = kzcLinkedListGetReverseIterator(propertyQuery->stack);
        while (kzcLinkedListIterate(it))
        {
            struct KzuPropertyBaseStorage* newPropertyStorage = KZ_NULL;
            const struct KzuPropertyQueryEntry* entry = kzcLinkedListIteratorGetValue(it);
            kzsAssert(kzcIsValidPointer(entry));

            if (entry->isObjectNode)
            {
                const struct KzuObjectNode* unusedNode;
                newPropertyStorage = kzuObjectNodeFindPropertyStorageWithGroups_private(entry->object.objectNode, typeStorage, &unusedNode);
            }
            else
            {
                newPropertyStorage = kzuPropertyManagerFindPropertyStorageWithGroups_private(propertyQuery->propertyManager, entry->object.anonymousObject, typeStorage);
            }

            if (newPropertyStorage != KZ_NULL)
            {
                if (propertyStorage == KZ_NULL || newPropertyStorage->priority > propertyStorage->priority)
                {
                    propertyStorage = newPropertyStorage;
                }
            }
        }
    }

    return propertyStorage;
}

kzFloat kzuPropertyQueryGetFloat(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    kzFloat value = 0.0f;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyFloatStorage* actualProperty = (struct KzuPropertyFloatStorage*)propertyStorage;
        value = actualProperty->baseValue;
    }
    else
    {
        value = kzuFloatPropertyTypeGetDefaultValue(kzuFloatPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}

kzInt kzuPropertyQueryGetInt(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    kzInt value = 0;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyIntStorage* actualProperty = (struct KzuPropertyIntStorage*)propertyStorage;
        value = actualProperty->baseValue;
    }
    else
    {
        value = kzuIntPropertyTypeGetDefaultValue(kzuIntPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}

kzBool kzuPropertyQueryGetBool(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    kzBool value = KZ_FALSE;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyBoolStorage* actualProperty = (struct KzuPropertyBoolStorage*)propertyStorage;
        value = actualProperty->baseValue;
    }
    else
    {
        value = kzuBoolPropertyTypeGetDefaultValue(kzuBoolPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}

struct KzcColorRGBA kzuPropertyQueryGetColor(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    struct KzcColorRGBA value;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyColorStorage* actualProperty = (struct KzuPropertyColorStorage*)propertyStorage;
        kzcColorRGBAAdd(&actualProperty->baseValue, &actualProperty->relativeValue, &value);
    }
    else
    {
        value = kzuColorPropertyTypeGetDefaultValue(kzuColorPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}

struct KzcVector2 kzuPropertyQueryGetVector2(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    struct KzcVector2 value;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyVector2Storage* actualProperty = (struct KzuPropertyVector2Storage*)propertyStorage;
        value = actualProperty->baseValue;
    }
    else
    {
        value = kzuVector2PropertyTypeGetDefaultValue(kzuVector2PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}

struct KzcVector3 kzuPropertyQueryGetVector3(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    struct KzcVector3 value;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyVector3Storage* actualProperty = (struct KzuPropertyVector3Storage*)propertyStorage;
        value = actualProperty->baseValue;
    }
    else
    {
        value = kzuVector3PropertyTypeGetDefaultValue(kzuVector3PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}

struct KzcVector4 kzuPropertyQueryGetVector4(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    struct KzcVector4 value;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyVector4Storage* actualProperty = (struct KzuPropertyVector4Storage*)propertyStorage;
        value = actualProperty->baseValue;
    }
    else
    {
        value = kzuVector4PropertyTypeGetDefaultValue(kzuVector4PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}

struct KzcMatrix2x2 kzuPropertyQueryGetMatrix2x2(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    struct KzcMatrix2x2 value;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix2x2Storage* actualProperty = (struct KzuPropertyMatrix2x2Storage*)propertyStorage;
        kzcMatrix2x2Multiply(&actualProperty->relativeValue, &actualProperty->baseValue, &value);
    }
    else
    {
        value = kzuMatrix2x2PropertyTypeGetDefaultValue(kzuMatrix2x2PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}

struct KzcMatrix3x3 kzuPropertyQueryGetMatrix3x3(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    struct KzcMatrix3x3 value;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix3x3Storage* actualProperty = (struct KzuPropertyMatrix3x3Storage*)propertyStorage;
        kzcMatrix3x3Multiply(&actualProperty->relativeValue, &actualProperty->baseValue, &value);
    }
    else
    {
        value = kzuMatrix3x3PropertyTypeGetDefaultValue(kzuMatrix3x3PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}

struct KzcMatrix4x4 kzuPropertyQueryGetMatrix4x4(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    struct KzcMatrix4x4 value;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyMatrix4x4Storage* actualProperty = (struct KzuPropertyMatrix4x4Storage*)propertyStorage;
        kzcMatrix4x4Multiply(&actualProperty->relativeValue, &actualProperty->baseValue, &value);
    }
    else
    {
        value = kzuMatrix4x4PropertyTypeGetDefaultValue(kzuMatrix4x4PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}

void* kzuPropertyQueryGetVoid(const struct KzuPropertyQuery* propertyQuery, const struct KzuPropertyType* propertyType)
{
    void* value;
    struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyQueryFindPropertyStorage_internal(propertyQuery, propertyType);

    if (propertyStorage != KZ_NULL)
    {
        struct KzuPropertyVoidStorage* actualProperty = (struct KzuPropertyVoidStorage*)propertyStorage;
        value = actualProperty->baseValue;
    }
    else
    {
        value = kzuVoidPropertyTypeGetDefaultValue(kzuVoidPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType));
    }

    return value;
}
