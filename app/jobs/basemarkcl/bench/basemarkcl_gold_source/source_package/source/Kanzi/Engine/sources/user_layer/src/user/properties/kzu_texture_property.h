/**
* \file
* Specifies texture property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TEXTURE_PROPERTY_H
#define KZU_TEXTURE_PROPERTY_H


#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuProperty;
struct KzuPropertyType;
struct KzuMatrix4x4PropertyType;
struct KzcTexture;
struct KzcMemoryManager;


/** 
* \struct KzuTextureProperty
* Structure for texture property.
*/
struct KzuTextureProperty;
/** 
* \struct KzuTexturePropertyType
* Structure for texture property type.
*/
struct KzuTexturePropertyType;


/** List of texture operations. */
enum KzuTextureOperation
{
    KZU_TEXTURE_OPERATION_MODULATE, /**< Modulate (multiply) texture operation. */
    KZU_TEXTURE_OPERATION_REPLACE, /**< Replace texture operation .*/
    KZU_TEXTURE_OPERATION_DECAL, /**< Blend texture operation. */
    KZU_TEXTURE_OPERATION_ADD, /**< Additive texture operation. */
    KZU_TEXTURE_OPERATION_CUSTOM /**< Custom texture operation .*/
};


/** 
 * Creates a property type texture. Texture matrix for fixed pipeline can be assigned via matrix property, which can be KZ_NULL
 * if not used. 
 */
kzsError kzuTexturePropertyTypeCreate(const struct KzcMemoryManager* memoryManager, kzString name, enum KzuTextureOperation operation, 
                                      kzUint textureUnit, struct KzuMatrix4x4PropertyType* matrixPropertyType,
                                      struct KzcTexture* defaultValue, struct KzuTexturePropertyType** out_propertyType);
/** Deletes texture property type */
kzsError kzuTexturePropertyTypeDelete(struct KzuTexturePropertyType* propertyType);


/** Returns texture property type from property type. */
struct KzuTexturePropertyType* kzuTexturePropertyTypeFromPropertyType(struct KzuPropertyType* propertyType);
/** Returns property type from texture property type. */
struct KzuPropertyType* kzuTexturePropertyTypeToPropertyType(struct KzuTexturePropertyType* propertyType);

/** Gets the default value of texture property type. */
struct KzcTexture* kzuTexturePropertyTypeGetDefaultValue(const struct KzuTexturePropertyType* propertyType);


/** Creates a new texture property. */
kzsError kzuTexturePropertyCreate(const struct KzcMemoryManager* memoryManager, struct KzuTexturePropertyType* propertyType, 
                                  struct KzcTexture* texture, struct KzuTextureProperty** out_property);
/** Deletes texture property. */
kzsError kzuTexturePropertyDelete(struct KzuTextureProperty* property);


/** Convert texture property to property */
struct KzuProperty* kzuTexturePropertyToProperty(struct KzuTextureProperty* texture);
/** Converts property to texture property. */
struct KzuTextureProperty* kzuTexturePropertyFromProperty(const struct KzuProperty* property);


/** Returns a texture from texture property. */
struct KzcTexture* kzuTexturePropertyGetValue(const struct KzuTextureProperty* texture);
/** Sets texture for texture property. */
void kzuTexturePropertySetValue(struct KzuTextureProperty* textureProperty, struct KzcTexture* texture);

/** Gets texture unit from texture property type. */
kzUint kzuTexturePropertyTypeGetTextureUnit(const struct KzuTexturePropertyType* propertyType);
/** Gets texture operation from texture property type. */
enum KzuTextureOperation kzuTexturePropertyTypeGetTextureOperation(const struct KzuTexturePropertyType* propertyType);
/** Gets assigned texture matrix from texture property type. */
struct KzuMatrix4x4PropertyType* kzuTexturePropertyTypeGetTextureMatrix(const struct KzuTexturePropertyType* propertyType);


#endif
