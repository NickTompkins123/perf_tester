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
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_vector2_property.h>
#include <user/properties/kzu_vector3_property.h>
#include <user/properties/kzu_vector4_property.h>
#include <user/properties/kzu_matrix2x2_property.h>
#include <user/properties/kzu_matrix3x3_property.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_void_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/debug/kzc_log.h>

#include <system/wrappers/kzs_math.h>


kzsError kzuPropertyManagerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager** out_propertyManager)
{
    kzsError result;
    struct KzuPropertyManager* propertyManager;

    result = kzcMemoryAllocVariable(memoryManager, propertyManager, "Property manager");
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &propertyManager->typeContainer);
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &propertyManager->groupContainer);
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &propertyManager->typesByNames);
    kzsErrorForward(result);

    *out_propertyManager = propertyManager;

    kzsSuccess();
}

kzsError kzuPropertyManagerDelete(struct KzuPropertyManager* propertyManager)
{
    kzsError result;
    struct KzcHashMapIterator it;

    kzsAssert(kzcIsValidPointer(propertyManager));

    /* Destroy type storage. */
    it = kzcHashMapGetIterator(propertyManager->typeContainer);
    while (kzcHashMapIterate(it))
    {
        struct KzuPropertyTypeStorage* typeStorage = (struct KzuPropertyTypeStorage*)kzcHashMapIteratorGetValue(it);

        kzsAssert(typeStorage != KZ_NULL);

        result = kzcHashMapDelete(typeStorage->propertyContainer);
        kzsErrorForward(result);

        result = kzcMemoryFreeVariable(typeStorage);
        kzsErrorForward(result);
    }

    it = kzcHashMapGetIterator(propertyManager->groupContainer);
    while (kzcHashMapIterate(it))
    {
        struct KzcDynamicArray* groupArray = (struct KzcDynamicArray*)kzcHashMapIteratorGetValue(it);

        result = kzcDynamicArrayDelete(groupArray);
        kzsErrorForward(result);
    }

    result = kzcHashMapDelete(propertyManager->groupContainer);
    kzsErrorForward(result);

    result = kzcHashMapDelete(propertyManager->typeContainer);
    kzsErrorForward(result);

    /* Destroy name lookup */
    it = kzcHashMapGetIterator(propertyManager->typesByNames);
    while (kzcHashMapIterate(it))
    {
        kzMutableString propertyName = (kzMutableString)kzcHashMapIteratorGetKey(it);

        result = kzcStringDelete(propertyName);
        kzsErrorForward(result);
    }
    result = kzcHashMapDelete(propertyManager->typesByNames);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(propertyManager);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuPropertyManagerSetPropertyPriority(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, enum KzuPropertyPriority priority)
{
    kzBool found = KZ_FALSE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            propertyStorage->priority = priority;
        }
    }

    return found;
}

kzBool kzuPropertyManagerGetPropertyPriority(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, enum KzuPropertyPriority* out_priority)
{
    kzBool found = KZ_FALSE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);
    enum KzuPropertyPriority priority = KZU_PROPERTY_PRIORITY_NORMAL;

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            priority = propertyStorage->priority;
        }
    }

    *out_priority = priority;
    return found;
}

kzBool kzuPropertyManagerSetPropertyFlag(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzU32 flag)
{
    kzBool found = KZ_FALSE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            propertyStorage->flags |= flag;
        }
    }

    return found;
}

kzBool kzuPropertyManagerClearPropertyFlag(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzU32 flag)
{
    kzBool found = KZ_FALSE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            propertyStorage->flags &= ~flag;
        }
    }

    return found;
}

kzBool kzuPropertyManagerIsPropertyFlagSet(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType, kzU32 flag, kzBool* out_isSet)
{
    kzBool found = KZ_FALSE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);
    kzBool isSet = KZ_FALSE;

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
        if (propertyStorage != KZ_NULL)
        {
            found = KZ_TRUE;
            isSet = (propertyStorage->flags & flag) == flag;
        }
    }

    *out_isSet = isSet;
    return found;
}

kzsError kzuPropertyManagerRegisterPropertyType(const struct KzuPropertyManager* propertyManager, const struct KzuPropertyType* propertyType)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage;
    
    result = kzuPropertyManagerAcquireTypeStorage_private(propertyManager, propertyType, &typeStorage);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuPropertyManagerUnregisterPropertyType(const struct KzuPropertyManager* propertyManager, const struct KzuPropertyType* propertyType)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (typeStorage != KZ_NULL)
    {
        kzMutableString name;

        result = kzcHashMapDelete(typeStorage->propertyContainer);
        kzsErrorForward(result);

        result = kzcHashMapRemove(propertyManager->typeContainer, propertyType);
        kzsErrorForward(result);

        result = kzcMemoryFreeVariable(typeStorage);
        kzsErrorForward(result);

        name = (kzMutableString)kzcHashMapGetStoredKey(propertyManager->typesByNames, kzuPropertyTypeGetName(propertyType));        

        result = kzcHashMapRemove(propertyManager->typesByNames, kzuPropertyTypeGetName(propertyType));
        kzsErrorForward(result);       

        result = kzcStringDelete(name);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

const struct KzuPropertyType* kzuPropertyManagerFindPropertyType(const struct KzuPropertyManager* propertyManager, kzString propertyTypeName)
{
    const struct KzuPropertyType* propertyType;
    kzBool found;

    kzsAssert(kzcIsValidPointer(propertyManager));

    found = kzcHashMapGet(propertyManager->typesByNames, propertyTypeName, (void**)&propertyType);
    KZ_UNUSED_RETURN_VALUE(found);

    return propertyType;
}

kzsError kzuPropertyManagerAddPropertyGroup(const struct KzuPropertyManager* propertyManager, const void* object, struct KzuPropertyGroup* propertyGroup)
{
    kzsError result;
    struct KzcDynamicArray* groupArray;

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (!kzcHashMapGet(propertyManager->groupContainer, object, (void**)&groupArray))
    {
        result = kzcDynamicArrayCreate(kzcMemoryGetManager(propertyManager), &groupArray);
        kzsErrorForward(result);

        result = kzcHashMapPut(propertyManager->groupContainer, object, groupArray);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayAdd(groupArray, (void*)propertyGroup);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuPropertyManagerRemovePropertyGroup(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyGroup* propertyGroup)
{
    kzsError result;
    struct KzcDynamicArray* groupArray;

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (kzcHashMapGet(propertyManager->groupContainer, object, (void**)&groupArray))
    {
        struct KzcDynamicArrayMutableIterator it = kzcDynamicArrayGetMutableIterator(groupArray);
        while(kzcDynamicArrayMutableIterate(it))
        {
            struct KzuPropertyGroup* foundGroup = kzcDynamicArrayMutableIteratorGetValue(it);
            if (foundGroup == propertyGroup)
            {
                result = kzcDynamicArrayMutableIteratorRemove(it);                
                kzsErrorForward(result);

                break;
            }
        }
    }

    kzsSuccess();
}

kzsError kzuPropertyManagerRemovePropertyGroups(const struct KzuPropertyManager* propertyManager, const void* object)
{
    kzsError result;
    struct KzcDynamicArray* groupArray;

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (kzcHashMapGet(propertyManager->groupContainer, object, (void**)&groupArray))
    {
        result = kzcHashMapRemove(propertyManager->groupContainer, object);
        kzsErrorForward(result);

        result = kzcDynamicArrayDelete(groupArray);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuPropertyManagerGetPropertyGroups(const struct KzuPropertyManager* propertyManager, const struct KzcMemoryManager* memoryManager, const void* object, struct KzcDynamicArray** out_propertyGroups)
{
    kzsError result;
    struct KzcDynamicArray* groupArray;
    struct KzcDynamicArray* newGroupArray = KZ_NULL;

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (kzcHashMapGet(propertyManager->groupContainer, object, (void**)&groupArray))
    {
        result = kzcDynamicArrayCopy(memoryManager, groupArray, &newGroupArray);
        kzsErrorForward(result);
    }

    *out_propertyGroups = newGroupArray;

    kzsSuccess();
}

kzsError kzuPropertyManagerCopyProperyGroups(const struct KzuPropertyManager* propertyManager, const void* sourceObject, const void* targetObject)
{
    kzsError result;
    struct KzcDynamicArray* sourceGroupArray;

    kzsAssert(kzcIsValidPointer(propertyManager));

    if (kzcHashMapGet(propertyManager->groupContainer, sourceObject, (void**)&sourceGroupArray))
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(propertyManager);
        struct KzcDynamicArray* targetGroupArray;
        struct KzcDynamicArray* oldGroupArray;

        result = kzcDynamicArrayCopy(memoryManager, sourceGroupArray, &targetGroupArray);
        kzsErrorForward(result);

        if (kzcHashMapGet(propertyManager->groupContainer, targetObject, (void**)&oldGroupArray))
        {
            result = kzcDynamicArrayDelete(oldGroupArray);
            kzsErrorForward(result);
        }

        result = kzcHashMapPut(propertyManager->groupContainer, targetObject, targetGroupArray);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzBool kzuObjectNodeHasProperty(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    return kzuObjectNodeFindHighestPriorityPropertySource(objectNode, propertyType) != KZ_NULL;
}

struct KzuObjectNode* kzuObjectNodeFindHighestPriorityPropertySource(const struct KzuObjectNode* objectNode, const struct KzuPropertyType* propertyType)
{
    const struct KzuObjectNode* foundNode = KZ_NULL;
    struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(objectNode);
    const struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyBaseStorage* storage = kzuObjectNodeFindPropertyStorageWithGroups_private(objectNode, typeStorage, &foundNode);
        KZ_UNUSED_RETURN_VALUE(storage);
    }

    return (struct KzuObjectNode*)foundNode;
}

kzsError kzuPropertyManagerRemoveProperty(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);    

    if (typeStorage != KZ_NULL)
    {
        result = kzuPropertyManagerRemovePropertyStorage_private(object, typeStorage);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuPropertyManagerRemoveProperties(const struct KzuPropertyManager* propertyManager, const void* object)
{
    kzsError result;
    struct KzcHashMapIterator it;

    kzsAssert(kzcIsValidPointer(propertyManager));

    it = kzcHashMapGetIterator(propertyManager->typeContainer);
    while (kzcHashMapIterate(it))
    {
        struct KzuPropertyTypeStorage* typeStorage = kzcHashMapIteratorGetValue(it);
        result = kzuPropertyManagerRemovePropertyStorage_private(object, typeStorage);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuPropertyManagerGetPropertyTypes(const struct KzuPropertyManager* propertyManager, const struct KzcMemoryManager* memoryManager, const void* object, struct KzcDynamicArray** out_propertyTypes)
{
    kzsError result;
    struct KzcDynamicArray* typeArray;
    struct KzcHashMapIterator it;

    kzsAssert(kzcIsValidPointer(propertyManager));

    result = kzcDynamicArrayCreate(memoryManager, &typeArray);
    kzsErrorForward(result);

    it = kzcHashMapGetIterator(propertyManager->typeContainer);
    while (kzcHashMapIterate(it))
    {
        struct KzuPropertyTypeStorage* typeStorage = kzcHashMapIteratorGetValue(it);
        struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);

        kzsAssert(kzcIsValidPointer(typeStorage));

        if (propertyStorage != KZ_NULL)
        {
            result = kzcDynamicArrayAdd(typeArray, (void*)typeStorage->type);
            kzsErrorForward(result);
        }
    }

    *out_propertyTypes = typeArray;
    kzsSuccess();
}

kzBool kzuPropertyManagerHasProperty(const struct KzuPropertyManager* propertyManager, const void* object, const struct KzuPropertyType* propertyType)
{
    kzBool found = KZ_FALSE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    if (typeStorage != KZ_NULL)
    {
        struct KzuPropertyBaseStorage* propertyStorage = kzuPropertyManagerFindPropertyStorage_private(object, typeStorage);
        found = (propertyStorage != KZ_NULL);
    }

    return found;
}

kzBool kzuPropertyManagerIsPropertyEqual(const struct KzuPropertyManager* propertyManager, const void* object1, const void* object2, const struct KzuPropertyType* propertyType)
{
    kzBool equal = KZ_FALSE;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    if (typeStorage == KZ_NULL)
    {
        equal = KZ_TRUE;
    }
    else
    {
        struct KzuPropertyBaseStorage* propertyStorage1 = kzuPropertyManagerFindPropertyStorage_private(object1, typeStorage);
        struct KzuPropertyBaseStorage* propertyStorage2 = kzuPropertyManagerFindPropertyStorage_private(object2, typeStorage);

        if (propertyStorage1 == KZ_NULL && propertyStorage2 == KZ_NULL)
        {
            equal = KZ_TRUE;
        }
        else if (propertyStorage1 != KZ_NULL && propertyStorage2 != KZ_NULL)
        {
            switch (kzuPropertyTypeGetDataType(propertyType))
            {
                case KZU_PROPERTY_DATA_TYPE_FLOAT:
                {
                    struct KzuPropertyFloatStorage* actualStorage1 = (struct KzuPropertyFloatStorage*)propertyStorage1;
                    struct KzuPropertyFloatStorage* actualStorage2 = (struct KzuPropertyFloatStorage*)propertyStorage2;

                    equal = kzsFloatIsEqual(actualStorage1->baseValue, actualStorage2->baseValue);
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_INT:
                {
                    struct KzuPropertyIntStorage* actualStorage1 = (struct KzuPropertyIntStorage*)propertyStorage1;
                    struct KzuPropertyIntStorage* actualStorage2 = (struct KzuPropertyIntStorage*)propertyStorage2;

                    equal = actualStorage1->baseValue == actualStorage2->baseValue;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_BOOL:
                {
                    struct KzuPropertyBoolStorage* actualStorage1 = (struct KzuPropertyBoolStorage*)propertyStorage1;
                    struct KzuPropertyBoolStorage* actualStorage2 = (struct KzuPropertyBoolStorage*)propertyStorage2;

                    equal = actualStorage1->baseValue == actualStorage2->baseValue;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_COLOR:
                {
                    struct KzuPropertyColorStorage* actualStorage1 = (struct KzuPropertyColorStorage*)propertyStorage1;
                    struct KzuPropertyColorStorage* actualStorage2 = (struct KzuPropertyColorStorage*)propertyStorage2;

                    equal = kzcColorRGBAIsEqual(&actualStorage1->baseValue, &actualStorage2->baseValue);
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_VECTOR2:
                {
                    struct KzuPropertyVector2Storage* actualStorage1 = (struct KzuPropertyVector2Storage*)propertyStorage1;
                    struct KzuPropertyVector2Storage* actualStorage2 = (struct KzuPropertyVector2Storage*)propertyStorage2;

                    equal = kzcVector2IsEqual(&actualStorage1->baseValue, &actualStorage2->baseValue);
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_VECTOR3:
                {
                    struct KzuPropertyVector3Storage* actualStorage1 = (struct KzuPropertyVector3Storage*)propertyStorage1;
                    struct KzuPropertyVector3Storage* actualStorage2 = (struct KzuPropertyVector3Storage*)propertyStorage2;

                    equal = kzcVector3IsEqual(&actualStorage1->baseValue, &actualStorage2->baseValue);
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_VECTOR4:
                {
                    struct KzuPropertyVector4Storage* actualStorage1 = (struct KzuPropertyVector4Storage*)propertyStorage1;
                    struct KzuPropertyVector4Storage* actualStorage2 = (struct KzuPropertyVector4Storage*)propertyStorage2;

                    equal = kzcVector4IsEqual(&actualStorage1->baseValue, &actualStorage2->baseValue);
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
                {
                    struct KzuPropertyMatrix2x2Storage* actualStorage1 = (struct KzuPropertyMatrix2x2Storage*)propertyStorage1;
                    struct KzuPropertyMatrix2x2Storage* actualStorage2 = (struct KzuPropertyMatrix2x2Storage*)propertyStorage2;

                    equal = kzcMatrix2x2IsEqual(&actualStorage1->baseValue, &actualStorage2->baseValue);
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
                {
                    struct KzuPropertyMatrix3x3Storage* actualStorage1 = (struct KzuPropertyMatrix3x3Storage*)propertyStorage1;
                    struct KzuPropertyMatrix3x3Storage* actualStorage2 = (struct KzuPropertyMatrix3x3Storage*)propertyStorage2;

                    equal = kzcMatrix3x3IsEqual(&actualStorage1->baseValue, &actualStorage2->baseValue);
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
                {
                    struct KzuPropertyMatrix4x4Storage* actualStorage1 = (struct KzuPropertyMatrix4x4Storage*)propertyStorage1;
                    struct KzuPropertyMatrix4x4Storage* actualStorage2 = (struct KzuPropertyMatrix4x4Storage*)propertyStorage2;

                    equal = kzcMatrix4x4IsEqual(&actualStorage1->baseValue, &actualStorage2->baseValue);
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_STRING:
                {
                    struct KzuPropertyStringStorage* actualStorage1 = (struct KzuPropertyStringStorage*)propertyStorage1;
                    struct KzuPropertyStringStorage* actualStorage2 = (struct KzuPropertyStringStorage*)propertyStorage2;

                    equal = kzcStringIsEqual(actualStorage1->baseValue, actualStorage2->baseValue);
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_VOID:
                case KZU_PROPERTY_DATA_TYPE_LIGHT:
                case KZU_PROPERTY_DATA_TYPE_TEXTURE:
                {
                    struct KzuPropertyVoidStorage* actualStorage1 = (struct KzuPropertyVoidStorage*)propertyStorage1;
                    struct KzuPropertyVoidStorage* actualStorage2 = (struct KzuPropertyVoidStorage*)propertyStorage2;

                    equal = actualStorage1->baseValue == actualStorage2->baseValue;
                    break;
                }

                default:
                {
                    kzsAssert(KZ_FALSE);
                }
            }
            /* TODO: implement other types. */
        }
    }

    return equal;
}

kzsError kzuPropertyManagerCopyProperty(const struct KzuPropertyManager* propertyManager, const void* sourceObject, const void* targetObject, const struct KzuPropertyType* propertyType)
{
    kzsError result;
    struct KzuPropertyTypeStorage* typeStorage = kzuPropertyManagerFindTypeStorage_private(propertyManager, propertyType);

    if(typeStorage != KZ_NULL)
    {
        struct KzuPropertyBaseStorage* sourceStorage = kzuPropertyManagerFindPropertyStorage_private(sourceObject, typeStorage);

        if (sourceStorage != KZ_NULL)
        {
            switch (kzuPropertyTypeGetDataType(propertyType))
            {
                case KZU_PROPERTY_DATA_TYPE_FLOAT:
                {
                    struct KzuPropertyFloatStorage* actualSourceStorage = (struct KzuPropertyFloatStorage*)sourceStorage;
                    struct KzuPropertyFloatStorage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireFloatStorage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_INT:
                {
                    struct KzuPropertyIntStorage* actualSourceStorage = (struct KzuPropertyIntStorage*)sourceStorage;
                    struct KzuPropertyIntStorage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireIntStorage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_BOOL:
                {
                    struct KzuPropertyBoolStorage* actualSourceStorage = (struct KzuPropertyBoolStorage*)sourceStorage;
                    struct KzuPropertyBoolStorage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireBoolStorage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_COLOR:
                {
                    struct KzuPropertyColorStorage* actualSourceStorage = (struct KzuPropertyColorStorage*)sourceStorage;
                    struct KzuPropertyColorStorage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireColorStorage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_VECTOR2:
                {
                    struct KzuPropertyVector2Storage* actualSourceStorage = (struct KzuPropertyVector2Storage*)sourceStorage;
                    struct KzuPropertyVector2Storage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireVector2Storage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_VECTOR3:
                {
                    struct KzuPropertyVector3Storage* actualSourceStorage = (struct KzuPropertyVector3Storage*)sourceStorage;
                    struct KzuPropertyVector3Storage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireVector3Storage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_VECTOR4:
                {
                    struct KzuPropertyVector4Storage* actualSourceStorage = (struct KzuPropertyVector4Storage*)sourceStorage;
                    struct KzuPropertyVector4Storage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireVector4Storage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
                {
                    struct KzuPropertyMatrix2x2Storage* actualSourceStorage = (struct KzuPropertyMatrix2x2Storage*)sourceStorage;
                    struct KzuPropertyMatrix2x2Storage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireMatrix2x2Storage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
                {
                    struct KzuPropertyMatrix3x3Storage* actualSourceStorage = (struct KzuPropertyMatrix3x3Storage*)sourceStorage;
                    struct KzuPropertyMatrix3x3Storage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireMatrix3x3Storage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
                {
                    struct KzuPropertyMatrix4x4Storage* actualSourceStorage = (struct KzuPropertyMatrix4x4Storage*)sourceStorage;
                    struct KzuPropertyMatrix4x4Storage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireMatrix4x4Storage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_VOID:
                case KZU_PROPERTY_DATA_TYPE_LIGHT:
                case KZU_PROPERTY_DATA_TYPE_TEXTURE:
                {
                    struct KzuPropertyVoidStorage* actualSourceStorage = (struct KzuPropertyVoidStorage*)sourceStorage;
                    struct KzuPropertyVoidStorage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireVoidStorage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    *actualTargetStorage = *actualSourceStorage;
                    break;
                }

                case KZU_PROPERTY_DATA_TYPE_STRING:
                {
                    struct KzuPropertyStringStorage* actualSourceStorage = (struct KzuPropertyStringStorage*)sourceStorage;
                    struct KzuPropertyStringStorage* actualTargetStorage;

                    result = kzuPropertyManagerAcquireStringStorage_private(propertyManager, targetObject, typeStorage, &actualTargetStorage);
                    kzsErrorForward(result);

                    actualTargetStorage->baseStorage = actualSourceStorage->baseStorage;
                    result = kzuPropertyManagerAssignBaseString_internal(actualTargetStorage, actualSourceStorage->baseValue);
                    kzsErrorForward(result);
                    result = kzuPropertyManagerAssignRelativeString_internal(actualTargetStorage, actualSourceStorage->relativeValue);
                    kzsErrorForward(result);

                    break;
                }

                default:
                {
                    kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid property type");
                }
            }
        }
    }

    kzsSuccess();
}

kzsError kzuPropertyManagerCopyProperties(const struct KzuPropertyManager* propertyManager, const void* sourceObject, const void* targetObject)
{
    kzsError result;
    struct KzcHashMapIterator it;

    kzsAssert(kzcIsValidPointer(propertyManager));

    it = kzcHashMapGetIterator(propertyManager->typeContainer);
    while (kzcHashMapIterate(it))
    {
        struct KzuPropertyTypeStorage* typeStorage = kzcHashMapIteratorGetValue(it);

        kzsAssert(kzcIsValidPointer(typeStorage));

        result = kzuPropertyManagerCopyProperty(propertyManager, sourceObject, targetObject, typeStorage->type);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuPropertyManagerGetPropertyByName_old(const struct KzuPropertyManager* propertyManager, const void* object, kzString propertyName, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuProperty* resultProperty = KZ_NULL;
    const struct KzuPropertyType* propertyType = kzuPropertyManagerFindPropertyType(propertyManager, propertyName);
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(object);

    if (propertyType != KZ_NULL)
    {
        enum KzuPropertyDataType dataType = kzuPropertyTypeGetDataType(propertyType);

        switch (dataType)
        {
            case KZU_PROPERTY_DATA_TYPE_FLOAT:
            {
                kzFloat value;
                if (kzuPropertyManagerGetFloat(propertyManager, object, propertyType, &value))
                {
                    struct KzuFloatProperty* property;
                    result = kzuFloatPropertyCreate(memoryManager, kzuFloatPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuFloatPropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_INT:
            {
                kzInt value;
                if (kzuPropertyManagerGetInt(propertyManager, object, propertyType, &value))
                {
                    struct KzuIntProperty* property;
                    result = kzuIntPropertyCreate(memoryManager, kzuIntPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuIntPropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_BOOL:
            {
                kzBool value;
                if (kzuPropertyManagerGetBool(propertyManager, object, propertyType, &value))
                {
                    struct KzuBoolProperty* property;
                    result = kzuBoolPropertyCreate(memoryManager, kzuBoolPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuBoolPropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_COLOR:
            {
                struct KzcColorRGBA value;
                if (kzuPropertyManagerGetColor(propertyManager, object, propertyType, &value))
                {
                    struct KzuColorProperty* property;
                    result = kzuColorPropertyCreate(memoryManager, kzuColorPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuColorPropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_VECTOR2:
            {
                struct KzcVector2 value;
                if (kzuPropertyManagerGetVector2(propertyManager, object, propertyType, &value))
                {
                    struct KzuVector2Property* property;
                    result = kzuVector2PropertyCreate(memoryManager, kzuVector2PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuVector2PropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_VECTOR3:
            {
                struct KzcVector3 value;
                if (kzuPropertyManagerGetVector3(propertyManager, object, propertyType, &value))
                {
                    struct KzuVector3Property* property;
                    result = kzuVector3PropertyCreate(memoryManager, kzuVector3PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuVector3PropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_VECTOR4:
            {
                struct KzcVector4 value;
                if (kzuPropertyManagerGetVector4(propertyManager, object, propertyType, &value))
                {
                    struct KzuVector4Property* property;
                    result = kzuVector4PropertyCreate(memoryManager, kzuVector4PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuVector4PropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
            {
                struct KzcMatrix2x2 value;
                if (kzuPropertyManagerGetMatrix2x2(propertyManager, object, propertyType, &value))
                {
                    struct KzuMatrix2x2Property* property;
                    result = kzuMatrix2x2PropertyCreate(memoryManager, kzuMatrix2x2PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), &value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuMatrix2x2PropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
            {
                struct KzcMatrix3x3 value;
                if (kzuPropertyManagerGetMatrix3x3(propertyManager, object, propertyType, &value))
                {
                    struct KzuMatrix3x3Property* property;
                    result = kzuMatrix3x3PropertyCreate(memoryManager, kzuMatrix3x3PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), &value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuMatrix3x3PropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
            {
                struct KzcMatrix4x4 value;
                if (kzuPropertyManagerGetMatrix4x4(propertyManager, object, propertyType, &value))
                {
                    struct KzuMatrix4x4Property* property;
                    result = kzuMatrix4x4PropertyCreate(memoryManager, kzuMatrix4x4PropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), &value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuMatrix4x4PropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_STRING:
            {
                kzString value;
                if (kzuPropertyManagerGetString(propertyManager, object, propertyType, &value))
                {
                    struct KzuStringProperty* property;
                    result = kzuStringPropertyCreate(memoryManager, kzuStringPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuStringPropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_VOID:
            {
                void* value;
                if (kzuPropertyManagerGetVoid(propertyManager, object, propertyType, &value))
                {
                    struct KzuVoidProperty* property;
                    result = kzuVoidPropertyCreate(memoryManager, kzuVoidPropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuVoidPropertyToProperty(property);
                }
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_LIGHT:
            {
                /* Contact Alex if you encounter this. */
                kzsAssert(KZ_FALSE);
                break;
            }
            case KZU_PROPERTY_DATA_TYPE_TEXTURE:
            {
                void* value;
                if (kzuPropertyManagerGetVoid(propertyManager, object, propertyType, &value))
                {
                    struct KzuTextureProperty* property;
                    result = kzuTexturePropertyCreate(memoryManager, kzuTexturePropertyTypeFromPropertyType((struct KzuPropertyType*)propertyType), value, &property);
                    kzsErrorForward(result);
                    resultProperty = kzuTexturePropertyToProperty(property);
                }
            }
        }
    }

    *out_property = resultProperty;
    kzsSuccess();
}
