/**
* \file
* Specifies light property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_light_property.h"

#include "kzu_property_private.h"
#include "kzu_property_collection.h"

#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>


/* Forward declarations */
struct KzuLightNode;


struct KzuLightProperty
{
    struct KzuProperty property;            /**< Base class. */
};

struct KzuLightPropertyType
{
    struct KzuPropertyType propertyType; /**< Parent property type. */
};

struct KzuLightPropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuLightPropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 

    struct KzuPropertyTypeCollection* propertyTypes;    /**< Collection of property types that define this light property type. */
};


kzsError kzuLightPropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuLightPropertyType* propertyType,
                                struct KzuLightProperty** out_property)
{
    kzsError result;
    struct KzuLightProperty* lightProperty;

    result = kzcMemoryAllocVariable(memoryManager, lightProperty, "LightProperty");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&lightProperty->property, (struct KzuPropertyType*)propertyType);

    *out_property = lightProperty;
    kzsSuccess();
}

kzsError kzuLightPropertyDelete(struct KzuLightProperty* property)
{
    kzsError result;

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuLightPropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, /*const struct KzcDynamicArray* group, */
                                    struct KzuLightPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuLightPropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Light property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_LIGHT, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    result = kzuPropertyTypeCollectionCreate(memoryManager, &propertyTypeData->propertyTypes);
    kzsErrorForward(result);

    *out_propertyType = (struct KzuLightPropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuLightPropertyTypeDelete(struct KzuLightPropertyType* propertyType)
{
    kzsError result;

    result = kzuPropertyTypeCollectionDelete(((struct KzuLightPropertyTypeData*)propertyType->propertyType.data)->propertyTypes);
    kzsErrorForward(result);

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuLightPropertyTypeAddProperty(const struct KzuLightPropertyType* propertyType, const struct KzuPropertyType* childPropertyType)
{
    kzsError result;

    result = kzuPropertyTypeCollectionAddPropertyType(((struct KzuLightPropertyTypeData*)propertyType->propertyType.data)->propertyTypes,
                                                      (struct KzuPropertyType*)childPropertyType);
    kzsErrorForward(result);
    
    kzsSuccess();
}

struct KzuLightProperty* kzuLightPropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuLightProperty*)property;
}

struct KzuLightPropertyType* kzuLightPropertyGetLightPropertyType(const struct KzuLightProperty* lightProperty)
{
    kzsAssert(kzcIsValidPointer(lightProperty));

    return (struct KzuLightPropertyType*)lightProperty->property.propertyType;
}

struct KzuProperty* kzuLightPropertyToProperty(struct KzuLightProperty* lightProperty)
{
    kzsAssert(kzcIsValidPointer(lightProperty));
    return &lightProperty->property;
}

struct KzuPropertyType* kzuLightPropertyTypeToPropertyType(struct KzuLightPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));
    return &propertyType->propertyType;
}

struct KzuLightPropertyType* kzuLightPropertyTypeFromPropertyType(const struct KzuPropertyType* propertyType)
{
    return (struct KzuLightPropertyType*)propertyType;
}

struct KzcDynamicArrayIterator kzuLightPropertyTypeGetPropertyTypes(const struct KzuLightPropertyType* propertyType)
{
    return kzuPropertyTypeCollectionGetIterator(((struct KzuLightPropertyTypeData*)propertyType->propertyType.data)->propertyTypes);
}

kzUint kzuLightPropertyTypeGetPropertyTypeCount(const struct KzuLightPropertyType* lightPropertyType)
{
    kzsAssert(kzcIsValidPointer(lightPropertyType));
    return kzuPropertyTypeCollectionGetSize(((struct KzuLightPropertyTypeData*)lightPropertyType->propertyType.data)->propertyTypes);
}
