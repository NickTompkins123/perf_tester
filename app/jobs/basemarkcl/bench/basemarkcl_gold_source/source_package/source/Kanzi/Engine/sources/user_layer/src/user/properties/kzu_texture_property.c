/**
* \file
* Specifies texture property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_texture_property.h"

#include "kzu_property_private.h"

#include <user/properties/kzu_matrix4x4_property.h>

#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/memory/kzc_memory_manager.h>


/** Texture property type. */
struct KzuTexturePropertyType
{
    struct KzuPropertyType propertyType; /**< Parent property type. */
};

struct KzuTexturePropertyTypeData
{
    struct KzuPropertyTypeData propertyTypeData; /**< Inherited from property type. */ /*lint -esym(754, KzuTexturePropertyTypeData::propertyTypeData) Suppressed as this member is required but not referenced. */ 
    enum KzuTextureOperation operation; /**< Used texture operation. */
    kzUint textureUnit; /**< Texture unit. */
    struct KzuMatrix4x4PropertyType* textureMatrix; /**< Texture matrix. */
    struct KzcTexture* defaultValue; /**< Default value. */
};

/** Texture property structure. */
struct KzuTextureProperty
{
    struct KzuProperty property;    /**< Property. Used for inheritance. */
    struct KzcTexture* texture;     /**< Texture. */
};


kzsError kzuTexturePropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, enum KzuTextureOperation operation, 
                                      kzUint textureUnit, struct KzuMatrix4x4PropertyType* matrixPropertyType,
                                      struct KzcTexture* defaultValue, struct KzuTexturePropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;
    struct KzuTexturePropertyTypeData* propertyTypeData;

    result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Texture property type data");
    kzsErrorForward(result);

    result = kzuPropertyTypeCreate_private(memoryManager, KZU_PROPERTY_DATA_TYPE_TEXTURE, name, &propertyTypeData->propertyTypeData, &propertyType);
    kzsErrorForward(result);

    propertyTypeData->operation = operation;
    propertyTypeData->textureUnit = textureUnit;
    propertyTypeData->textureMatrix = matrixPropertyType;
    propertyTypeData->defaultValue = defaultValue;

    *out_propertyType = (struct KzuTexturePropertyType*)propertyType;
    kzsSuccess();
}

kzsError kzuTexturePropertyTypeDelete(struct KzuTexturePropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(propertyType));

    result = kzuPropertyTypeDeleteBase_private(&propertyType->propertyType);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuTexturePropertyType* kzuTexturePropertyTypeFromPropertyType(struct KzuPropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return (struct KzuTexturePropertyType*)propertyType;
}

struct KzuPropertyType* kzuTexturePropertyTypeToPropertyType(struct KzuTexturePropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return &propertyType->propertyType;
}

struct KzcTexture* kzuTexturePropertyTypeGetDefaultValue(const struct KzuTexturePropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));

    return ((struct KzuTexturePropertyTypeData*)propertyType->propertyType.data)->defaultValue;
}

kzsError kzuTexturePropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuTexturePropertyType* propertyType, 
                                  struct KzcTexture* texture, struct KzuTextureProperty** out_property)
{
    kzsError result;
    struct KzuTextureProperty* textureProperty;

    kzsErrorTest(kzcIsValidPointer(propertyType), KZS_ERROR_ILLEGAL_ARGUMENT, "Uninitialized propertyType used as propertyType");

    if (texture != KZ_NULL)
    {
        kzsErrorTest(kzcIsValidPointer(texture), KZS_ERROR_ILLEGAL_ARGUMENT, "Invalid texture given for texture property");
    }

    result = kzcMemoryAllocVariable(memoryManager, textureProperty, "TextureProperty");
    kzsErrorForward(result);

    kzuPropertyInitialize_private(&textureProperty->property, (struct KzuPropertyType*)propertyType);
    textureProperty->texture = texture;

    *out_property = textureProperty;
    kzsSuccess();
}

kzsError kzuTexturePropertyDelete(struct KzuTextureProperty* property)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(property));

    result = kzcMemoryFreeVariable(property);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuProperty* kzuTexturePropertyToProperty(struct KzuTextureProperty* texture)
{
    return &texture->property;
}

struct KzcTexture* kzuTexturePropertyGetValue(const struct KzuTextureProperty* texture)
{
    kzsAssert(kzcIsValidPointer(texture));
    return texture->texture;
}

void kzuTexturePropertySetValue(struct KzuTextureProperty* textureProperty, struct KzcTexture* texture)
{
    kzsAssert(kzcIsValidPointer(textureProperty));
    kzsAssert(texture == KZ_NULL || kzcIsValidPointer(texture)); /* KZ_NULL texture can be bound to texture, default texture is used in that case. */
    textureProperty->texture = texture;
}

kzUint kzuTexturePropertyTypeGetTextureUnit(const struct KzuTexturePropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));
    return ((struct KzuTexturePropertyTypeData*)propertyType->propertyType.data)->textureUnit;
}

enum KzuTextureOperation kzuTexturePropertyTypeGetTextureOperation(const struct KzuTexturePropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));
    return ((struct KzuTexturePropertyTypeData*)propertyType->propertyType.data)->operation;
}

struct KzuMatrix4x4PropertyType* kzuTexturePropertyTypeGetTextureMatrix(const struct KzuTexturePropertyType* propertyType)
{
    kzsAssert(kzcIsValidPointer(propertyType));
    return ((struct KzuTexturePropertyTypeData*)propertyType->propertyType.data)->textureMatrix;
}

struct KzuTextureProperty* kzuTexturePropertyFromProperty(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (struct KzuTextureProperty*)property;
}
