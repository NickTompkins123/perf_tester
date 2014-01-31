/**
* \file
* Specifies material property.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_property.h"

#include "kzu_property_private.h"

#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_light_property.h>
#include <user/properties/kzu_matrix2x2_property.h>
#include <user/properties/kzu_matrix3x3_property.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_vector2_property.h>
#include <user/properties/kzu_vector3_property.h>
#include <user/properties/kzu_vector4_property.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/scene_graph/kzu_light.h>
#include <user/scene_graph/kzu_object.h>
#include <user/renderer/kzu_render_pass.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/string/kzc_string.h>
#include <core/renderer/kzc_renderer.h>


void kzuPropertyInitialize_private(struct KzuProperty* property, struct KzuPropertyType* propertyType)
{
    kzsAssert(kzuPropertyTypeIsValid(propertyType));

    property->flags = 0;
    property->propertyType = propertyType;
    property->priority = KZU_PROPERTY_PRIORITY_LOW;
}

kzsError kzuPropertyTypeCreate_private(const struct KzcMemoryManager* memoryManager, enum KzuPropertyDataType dataType,
                                       kzString name, struct KzuPropertyTypeData* propertyTypeData,
                                       struct KzuPropertyType** out_propertyType)
{
    kzsError result;
    struct KzuPropertyType* propertyType;

    if (propertyTypeData == KZ_NULL)
    {
        result = kzcMemoryAllocVariable(memoryManager, propertyTypeData, "Property type data");
        kzsErrorForward(result);
    }

    result = kzcMemoryAllocVariable(memoryManager, propertyType, "Property type");
    kzsErrorForward(result);

    propertyType->data = propertyTypeData;
    propertyTypeData->selfOwned = KZ_TRUE;

    result = kzcStringCopy(memoryManager, name, &propertyTypeData->name);
    kzsErrorForward(result);
    
    propertyTypeData->dataType = dataType;
    propertyTypeData->inherited = KZ_FALSE;

    *out_propertyType = propertyType;
    kzsSuccess();
}

kzsError kzuPropertyDelete(const struct KzuProperty* property)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(property));
    kzsAssert(kzcIsValidPointer(property->propertyType));

    switch (kzuPropertyTypeGetDataType(property->propertyType))
    {
        case KZU_PROPERTY_DATA_TYPE_COLOR:
        {
            result = kzuColorPropertyDelete(kzuColorPropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_FLOAT:
        {
            result = kzuFloatPropertyDelete(kzuFloatPropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_TEXTURE:
        {
            result = kzuTexturePropertyDelete(kzuTexturePropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
        {
            result = kzuMatrix4x4PropertyDelete(kzuMatrix4x4PropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
        {
            result = kzuMatrix3x3PropertyDelete(kzuMatrix3x3PropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
        {
            result = kzuMatrix2x2PropertyDelete(kzuMatrix2x2PropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_INT:
        {
            result = kzuIntPropertyDelete(kzuIntPropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_STRING:
        {
            result = kzuStringPropertyDelete(kzuStringPropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR4:
        {
            result = kzuVector4PropertyDelete(kzuVector4PropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR3:
        {
            result = kzuVector3PropertyDelete(kzuVector3PropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR2:
        {
            result = kzuVector2PropertyDelete(kzuVector2PropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_BOOL:
        {
            result = kzuBoolPropertyDelete(kzuBoolPropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_LIGHT:
        {
            result = kzuLightPropertyDelete(kzuLightPropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VOID:
        {
            result = kzuVoidPropertyDelete(kzuVoidPropertyFromProperty(property));
            kzsErrorForward(result);
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid property type");
        }
    }

    kzsSuccess();
}

kzsError kzuPropertyCopy(const struct KzcMemoryManager* memoryManager, const struct KzuProperty* sourceProperty, struct KzuProperty** out_property)
{
    kzsError result;
    struct KzuProperty* property;

    kzsAssert(kzcIsValidPointer(sourceProperty));
    kzsAssert(kzcIsValidPointer(sourceProperty->propertyType));
    
    switch (kzuPropertyTypeGetDataType(sourceProperty->propertyType))
    {
        case KZU_PROPERTY_DATA_TYPE_COLOR:
        {
            struct KzuColorProperty* colorProperty = kzuColorPropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuColorPropertyType* colorPropertyType = kzuColorPropertyTypeFromPropertyType(propertyType);
            struct KzuColorProperty* newColorProperty;
            result = kzuColorPropertyCreate(memoryManager, colorPropertyType, kzuColorPropertyGetValue(colorProperty), &newColorProperty);
            kzsErrorForward(result);

            property = kzuColorPropertyToProperty(newColorProperty);
            if (kzuColorPropertyIsRelative(colorProperty))
            {
                struct KzcColorRGBA value = kzuColorPropertyGetRelativeValue(colorProperty);
                kzuColorPropertySetRelativeValue(newColorProperty, &value);
            }

            break;
        }

        case KZU_PROPERTY_DATA_TYPE_FLOAT:
        {
            struct KzuFloatProperty* floatProperty = kzuFloatPropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuFloatPropertyType* floatPropertyType = kzuFloatPropertyTypeFromPropertyType(propertyType);
            struct KzuFloatProperty* newFloatProperty;
            result = kzuFloatPropertyCreate(memoryManager, floatPropertyType, kzuFloatPropertyGetValue(floatProperty), &newFloatProperty);
            kzsErrorForward(result);

            property = kzuFloatPropertyToProperty(newFloatProperty);
            if (kzuFloatPropertyIsRelative(floatProperty))
            {
                kzFloat value = kzuFloatPropertyGetRelativeValue(floatProperty);
                kzuFloatPropertySetRelativeValue(newFloatProperty, value);
            }

            break;
        }

        case KZU_PROPERTY_DATA_TYPE_TEXTURE:
        {
            struct KzuTextureProperty* textureProperty = kzuTexturePropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuTexturePropertyType* texturePropertyType = kzuTexturePropertyTypeFromPropertyType(propertyType);
            struct KzuTextureProperty* newProperty;
            result = kzuTexturePropertyCreate(memoryManager, texturePropertyType, kzuTexturePropertyGetValue(textureProperty), &newProperty);
            kzsErrorForward(result);
            property = kzuTexturePropertyToProperty(newProperty);

            break;
        }

        case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
        {
            struct KzuMatrix4x4Property* matrixProperty = kzuMatrix4x4PropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuMatrix4x4PropertyType* matrixPropertyType = kzuMatrix4x4PropertyTypeFromPropertyType(propertyType);
            struct KzuMatrix4x4Property* newProperty;
            {
                struct KzcMatrix4x4 matrixValue = kzuMatrix4x4PropertyGetValue(matrixProperty);
                result = kzuMatrix4x4PropertyCreate(memoryManager, matrixPropertyType, &matrixValue, &newProperty);
                kzsErrorForward(result);
            }

            property = kzuMatrix4x4PropertyToProperty(newProperty);
            if (kzuMatrix4x4PropertyIsRelative(matrixProperty))
            {
                struct KzcMatrix4x4 matrixValue = kzuMatrix4x4PropertyGetRelativeValue(matrixProperty);
                kzuMatrix4x4PropertySetRelativeValue(newProperty, &matrixValue);
            }

            break;
        }

        case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
        {
            struct KzuMatrix3x3Property* matrixProperty = kzuMatrix3x3PropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuMatrix3x3PropertyType* matrixPropertyType = kzuMatrix3x3PropertyTypeFromPropertyType(propertyType);
            struct KzuMatrix3x3Property* newProperty;
            struct KzcMatrix3x3 matrixValue = kzuMatrix3x3PropertyGetValue(matrixProperty);
            result = kzuMatrix3x3PropertyCreate(memoryManager, matrixPropertyType, &matrixValue, &newProperty);
            kzsErrorForward(result);

            property = kzuMatrix3x3PropertyToProperty(newProperty);
            break;

        }
        case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
        {
            struct KzuMatrix2x2Property* matrixProperty = kzuMatrix2x2PropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuMatrix2x2PropertyType* matrixPropertyType = kzuMatrix2x2PropertyTypeFromPropertyType(propertyType);
            struct KzuMatrix2x2Property* newProperty;
            struct KzcMatrix2x2 matrixValue = kzuMatrix2x2PropertyGetValue(matrixProperty);
            result = kzuMatrix2x2PropertyCreate(memoryManager, matrixPropertyType, &matrixValue, &newProperty);
            kzsErrorForward(result);

            property = kzuMatrix2x2PropertyToProperty(newProperty);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_INT:
        {
            struct KzuIntProperty* intProperty = kzuIntPropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuIntPropertyType* intPropertyType = kzuIntPropertyTypeFromPropertyType(propertyType);
            struct KzuIntProperty* newProperty;
            result = kzuIntPropertyCreate(memoryManager, intPropertyType, kzuIntPropertyGetValue(intProperty), &newProperty);
            kzsErrorForward(result);
            property = kzuIntPropertyToProperty(newProperty);

            break;
        }

        case KZU_PROPERTY_DATA_TYPE_STRING:
        {
            struct KzuStringProperty* stringProperty = kzuStringPropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuStringPropertyType* stringPropertyType = kzuStringPropertyTypeFromPropertyType(propertyType);
            struct KzuStringProperty* newProperty;
            result = kzuStringPropertyCreate(memoryManager, stringPropertyType, kzuStringPropertyGetValue(stringProperty), &newProperty);
            kzsErrorForward(result);
            property = kzuStringPropertyToProperty(newProperty);

            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR4:
        {
            struct KzuVector4Property* vecProperty = kzuVector4PropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuVector4PropertyType* vecPropertyType = kzuVector4PropertyTypeFromPropertyType(propertyType);
            struct KzuVector4Property* newProperty;
            result = kzuVector4PropertyCreate(memoryManager, vecPropertyType, kzuVector4PropertyGetValue(vecProperty), &newProperty);
            kzsErrorForward(result);
            property = kzuVector4PropertyToProperty(newProperty);

            if (kzuVector4PropertyIsRelative(vecProperty))
            {
                struct KzcVector4 value = kzuVector4PropertyGetRelativeValue(vecProperty);
                kzuVector4PropertySetRelativeValue(newProperty, value);
            }

            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR3:
        {
            struct KzuVector3Property* vecProperty = kzuVector3PropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuVector3PropertyType* vecPropertyType = kzuVector3PropertyTypeFromPropertyType(propertyType);
            struct KzuVector3Property* newProperty;
            result = kzuVector3PropertyCreate(memoryManager, vecPropertyType, kzuVector3PropertyGetValue(vecProperty), &newProperty);
            kzsErrorForward(result);

            property = kzuVector3PropertyToProperty(newProperty);
            if (kzuVector3PropertyIsRelative(vecProperty))
            {
                struct KzcVector3 value = kzuVector3PropertyGetRelativeValue(vecProperty);
                kzuVector3PropertySetRelativeValue(newProperty, value);
            }
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR2:
        {
            struct KzuVector2Property* vecProperty = kzuVector2PropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuVector2PropertyType* vecPropertyType = kzuVector2PropertyTypeFromPropertyType(propertyType);
            struct KzuVector2Property* newProperty;
            result = kzuVector2PropertyCreate(memoryManager, vecPropertyType, kzuVector2PropertyGetValue(vecProperty), &newProperty);
            kzsErrorForward(result);

            property = kzuVector2PropertyToProperty(newProperty);
            if (kzuVector2PropertyIsRelative(vecProperty))
            {
                struct KzcVector2 value = kzuVector2PropertyGetRelativeValue(vecProperty);
                kzuVector2PropertySetRelativeValue(newProperty, value);
            }
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_BOOL:
        {
            struct KzuBoolProperty* boolProperty = kzuBoolPropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuBoolPropertyType* boolPropertyType = kzuBoolPropertyTypeFromPropertyType(propertyType);
            struct KzuBoolProperty* newProperty;
            result = kzuBoolPropertyCreate(memoryManager, boolPropertyType, kzuBoolPropertyGetValue(boolProperty), &newProperty);
            kzsErrorForward(result);
            property = kzuBoolPropertyToProperty(newProperty);

            break;
        }

        case KZU_PROPERTY_DATA_TYPE_LIGHT:
        {
            /* TODO: Implement me. */
            KZS_NOT_IMPLEMENTED_YET_ERROR;
        }
         
        case KZU_PROPERTY_DATA_TYPE_VOID:
        {
            struct KzuVoidProperty* voidProperty = kzuVoidPropertyFromProperty(sourceProperty);
            struct KzuPropertyType* propertyType = kzuPropertyGetPropertyType(sourceProperty);
            struct KzuVoidPropertyType* voidPropertyType = kzuVoidPropertyTypeFromPropertyType(propertyType);
            struct KzuVoidProperty* newProperty;
            result = kzuVoidPropertyCreate(memoryManager, voidPropertyType, kzuVoidPropertyGetValue(voidProperty), &newProperty);
            kzsErrorForward(result);
            property = kzuVoidPropertyToProperty(newProperty);

            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid property type");
        }
    }

    if (property != KZ_NULL)
    {
        property->flags = sourceProperty->flags;
        property->priority = sourceProperty->priority;
    }

    *out_property = property;
    kzsSuccess();
}

kzsError kzuPropertyCopyValue(const struct KzuProperty* targetProperty, const struct KzuProperty* sourceProperty)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(sourceProperty));
    kzsErrorTest(kzuPropertyTypeGetDataType(targetProperty->propertyType) == kzuPropertyTypeGetDataType(sourceProperty->propertyType), 
                 KZS_ERROR_ILLEGAL_ARGUMENT, "Source and target property must be of same type.");

    switch (kzuPropertyTypeGetDataType(sourceProperty->propertyType))
    {
        case KZU_PROPERTY_DATA_TYPE_COLOR:
        {
            struct KzuColorProperty* sourceColorProperty = kzuColorPropertyFromProperty(sourceProperty);
            struct KzuColorProperty* targetColorProperty = kzuColorPropertyFromProperty(targetProperty);
            struct KzcColorRGBA color = kzuColorPropertyGetValue(sourceColorProperty);
            kzuColorPropertySetValue(targetColorProperty, &color);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_FLOAT:
        {
            struct KzuFloatProperty* sourceFloatProperty = kzuFloatPropertyFromProperty(sourceProperty);
            struct KzuFloatProperty* targetFloatProperty = kzuFloatPropertyFromProperty(targetProperty);
            kzFloat value = kzuFloatPropertyGetValue(sourceFloatProperty);
            kzuFloatPropertySetValue(targetFloatProperty, value);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_TEXTURE:
        {
            struct KzuTextureProperty* sourceTextureProperty = kzuTexturePropertyFromProperty(sourceProperty);
            struct KzuTextureProperty* targetTextureProperty = kzuTexturePropertyFromProperty(targetProperty);
            struct KzcTexture* texture = kzuTexturePropertyGetValue(sourceTextureProperty);
            
            kzuTexturePropertySetValue(targetTextureProperty, texture);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
        {
            struct KzuMatrix4x4Property* sourceMatrixProperty = kzuMatrix4x4PropertyFromProperty(sourceProperty);
            struct KzuMatrix4x4Property* targetMatrixProperty = kzuMatrix4x4PropertyFromProperty(targetProperty);
            struct KzcMatrix4x4 matrix = kzuMatrix4x4PropertyGetValue(sourceMatrixProperty);
            kzuMatrix4x4PropertySetValue(targetMatrixProperty, &matrix);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
        {
            struct KzuMatrix3x3Property* sourceMatrixProperty = kzuMatrix3x3PropertyFromProperty(sourceProperty);
            struct KzuMatrix3x3Property* targetMatrixProperty = kzuMatrix3x3PropertyFromProperty(targetProperty);
            struct KzcMatrix3x3 matrix = kzuMatrix3x3PropertyGetValue(sourceMatrixProperty);
            kzuMatrix3x3PropertySetValue(targetMatrixProperty, &matrix);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
        {
            struct KzuMatrix2x2Property* sourceMatrixProperty = kzuMatrix2x2PropertyFromProperty(sourceProperty);
            struct KzuMatrix2x2Property* targetMatrixProperty = kzuMatrix2x2PropertyFromProperty(targetProperty);
            struct KzcMatrix2x2 matrix = kzuMatrix2x2PropertyGetValue(sourceMatrixProperty);
            kzuMatrix2x2PropertySetValue(targetMatrixProperty, &matrix);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_INT:
        {
            struct KzuIntProperty* sourceIntProperty = kzuIntPropertyFromProperty(sourceProperty);
            struct KzuIntProperty* targetIntProperty = kzuIntPropertyFromProperty(targetProperty);
            kzInt value = kzuIntPropertyGetValue(sourceIntProperty);
            kzuIntPropertySetValue(targetIntProperty, value);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_STRING:
        {
            struct KzuStringProperty* sourceStringProperty = kzuStringPropertyFromProperty(sourceProperty);
            struct KzuStringProperty* targetStringProperty = kzuStringPropertyFromProperty(targetProperty);
            kzString value = kzuStringPropertyGetValue(sourceStringProperty);
            result = kzuStringPropertySetValue(targetStringProperty, value);
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR4:
        {
            struct KzuVector4Property* sourceVectorProperty = kzuVector4PropertyFromProperty(sourceProperty);
            struct KzuVector4Property* targetVectorProperty = kzuVector4PropertyFromProperty(targetProperty);
            struct KzcVector4 value = kzuVector4PropertyGetValue(sourceVectorProperty);
            kzuVector4PropertySetValue(targetVectorProperty, value);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR3:
        {
            struct KzuVector3Property* sourceVectorProperty = kzuVector3PropertyFromProperty(sourceProperty);
            struct KzuVector3Property* targetVectorProperty = kzuVector3PropertyFromProperty(targetProperty);
            struct KzcVector3 value = kzuVector3PropertyGetValue(sourceVectorProperty);
            kzuVector3PropertySetValue(targetVectorProperty, value);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR2:
        {
            struct KzuVector2Property* sourceVectorProperty = kzuVector2PropertyFromProperty(sourceProperty);
            struct KzuVector2Property* targetVectorProperty = kzuVector2PropertyFromProperty(targetProperty);
            struct KzcVector2 value = kzuVector2PropertyGetValue(sourceVectorProperty);
            kzuVector2PropertySetValue(targetVectorProperty, value);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_BOOL:
        {
            struct KzuBoolProperty* sourceBoolProperty = kzuBoolPropertyFromProperty(sourceProperty);
            struct KzuBoolProperty* targetBoolProperty = kzuBoolPropertyFromProperty(targetProperty);
            kzBool value = kzuBoolPropertyGetValue(sourceBoolProperty);
            kzuBoolPropertySetValue(targetBoolProperty, value);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_LIGHT:
        {
            /*
            TODO: Implement me.
            struct KzuLightProperty* sourceLightProperty = kzuLightPropertyFromProperty(sourceProperty);
            struct KzuLightProperty* targetLightProperty = kzuLightPropertyFromProperty(targetProperty);
            */
            KZS_NOT_IMPLEMENTED_YET_ERROR;
        }

        case KZU_PROPERTY_DATA_TYPE_VOID:
        {
            struct KzuVoidProperty* sourceVoidProperty = kzuVoidPropertyFromProperty(sourceProperty);
            struct KzuVoidProperty* targetVoidProperty = kzuVoidPropertyFromProperty(targetProperty);
            void* value = kzuVoidPropertyGetValue(sourceVoidProperty);
            kzuVoidPropertySetValue(targetVoidProperty, value);
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid property type");
        }
    }

    kzsSuccess();
}

kzsError kzuPropertyCopyContent(struct KzuProperty* targetProperty, const struct KzuProperty* sourceProperty)
{
    kzsError result;
    
    kzsAssert(kzcIsValidPointer(sourceProperty));
    kzsErrorTest(kzuPropertyTypeGetDataType(targetProperty->propertyType) == kzuPropertyTypeGetDataType(sourceProperty->propertyType), 
                 KZS_ERROR_ILLEGAL_ARGUMENT, "Source and target property must be of same type.");

    result = kzuPropertyCopyValue(targetProperty, sourceProperty);
    kzsErrorForward(result);

    /* Copy common property stuff. */
    targetProperty->priority = sourceProperty->priority;
    targetProperty->propertyType = sourceProperty->propertyType;
    targetProperty->flags = sourceProperty->flags;

    kzsSuccess();
}

void kzuPropertySetPriority(struct KzuProperty* property, enum KzuPropertyPriority priority)
{
    kzsAssert(kzcIsValidPointer(property));
    property->priority = priority;
}

enum KzuPropertyPriority kzuPropertyGetPriority(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return property->priority;
}

struct KzuPropertyType* kzuPropertyGetPropertyType(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return property->propertyType;
}

kzsError kzuPropertyTypeDeleteBase_private(struct KzuPropertyType* propertyType)
{
    kzsError result;
    struct KzuPropertyTypeData* propertyTypeData;
    kzBool selfOwned;

    kzsAssert(kzuPropertyTypeIsValid(propertyType));

    propertyTypeData = propertyType->data;
    selfOwned = propertyTypeData->selfOwned;

    result = kzcStringDelete(propertyTypeData->name);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(propertyTypeData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(propertyType);
        kzsErrorForward(result);
    }
    else
    {
        propertyType->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuPropertyTypeDelete(struct KzuPropertyType* propertyType)
{
    kzsError result;

    kzsAssert(kzuPropertyTypeIsValid(propertyType));

    switch(propertyType->data->dataType)
    {
        case KZU_PROPERTY_DATA_TYPE_LIGHT:
        {
            result = kzuLightPropertyTypeDelete(kzuLightPropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_COLOR:
        {
            result = kzuColorPropertyTypeDelete(kzuColorPropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_FLOAT:
        {
            result = kzuFloatPropertyTypeDelete(kzuFloatPropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_TEXTURE:
        {
            result = kzuTexturePropertyTypeDelete(kzuTexturePropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
        {
            result = kzuMatrix4x4PropertyTypeDelete(kzuMatrix4x4PropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
        {
            result = kzuMatrix3x3PropertyTypeDelete(kzuMatrix3x3PropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
        {
            result = kzuMatrix2x2PropertyTypeDelete(kzuMatrix2x2PropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_INT:
        {
            result = kzuIntPropertyTypeDelete(kzuIntPropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_STRING:
        {
            result = kzuStringPropertyTypeDelete(kzuStringPropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR4:
        {
            result = kzuVector4PropertyTypeDelete(kzuVector4PropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR3:
        {
            result = kzuVector3PropertyTypeDelete(kzuVector3PropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VECTOR2:
        {
            result = kzuVector2PropertyTypeDelete(kzuVector2PropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_BOOL:
        {
            result = kzuBoolPropertyTypeDelete(kzuBoolPropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        case KZU_PROPERTY_DATA_TYPE_VOID:
        {
            result = kzuVoidPropertyTypeDelete(kzuVoidPropertyTypeFromPropertyType(propertyType));
            kzsErrorForward(result);
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid property type");
        }
    }

    kzsSuccess();
}

void kzuPropertyTypeTakeOwnership(const struct KzuPropertyType* propertyType)
{
    kzsAssert(kzuPropertyTypeIsValid(propertyType));

    propertyType->data->selfOwned = KZ_FALSE;
}

kzBool kzuPropertyTypeIsValid(const struct KzuPropertyType* propertyType)
{
    return propertyType != KZ_NULL && kzcIsValidPointer(propertyType->data);
}

void kzuPropertyTypeTransferData(struct KzuPropertyType* targetPropertyType, struct KzuPropertyType* sourcePropertyType)
{
    kzsAssert(kzcIsValidPointer(targetPropertyType));
    kzsAssert(targetPropertyType->data == KZ_NULL);
    kzsAssert(kzuPropertyTypeIsValid(sourcePropertyType));

    targetPropertyType->data = sourcePropertyType->data;
    sourcePropertyType->data = KZ_NULL;
}

enum KzuPropertyDataType kzuPropertyTypeGetDataType(const struct KzuPropertyType* propertyType)
{
    kzsAssert(kzuPropertyTypeIsValid(propertyType));
    return propertyType->data->dataType;
}

kzString kzuPropertyTypeGetName(const struct KzuPropertyType* propertyType)
{
    kzsAssert(kzuPropertyTypeIsValid(propertyType));
    return propertyType->data->name;
}

void kzuPropertyTypeSetInherited(const struct KzuPropertyType* propertyType, kzBool inherited)
{
    kzsAssert(kzuPropertyTypeIsValid(propertyType));
    propertyType->data->inherited = inherited;
}

kzBool kzuPropertyTypeGetInherited(const struct KzuPropertyType* propertyType)
{
    kzsAssert(kzuPropertyTypeIsValid(propertyType));
    return propertyType->data->inherited;
}

kzBool kzuPropertyGetFromKzb(const struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    return (property->flags & KZU_PROPERTY_FLAG_FROM_KZB) == KZU_PROPERTY_FLAG_FROM_KZB;
}

void kzuPropertySetFromKzb(struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    property->flags |= KZU_PROPERTY_FLAG_FROM_KZB;
}

void kzuPropertyDisableFromKzb(struct KzuProperty* property)
{
    kzsAssert(kzcIsValidPointer(property));
    property->flags &= ~KZU_PROPERTY_FLAG_FROM_KZB;
}

void kzuPropertySetFlags(struct KzuProperty* property, kzU32 flags)
{
    kzsAssert(kzcIsValidPointer(property));
    property->flags = flags;
}

kzBool kzuPropertyIsInherited(const struct KzuProperty* property)
{
    kzBool inherits = KZ_FALSE;
    kzsAssert(kzcIsValidPointer(property));

    /* Get the inherited setting from the type. */
    if (property->propertyType != KZ_NULL)
    {
        inherits = kzuPropertyTypeGetInherited(property->propertyType);
    }    

    /* Check if inheritance is overriden. */
    if ((property->flags & KZU_PROPERTY_FLAG_INHERITANCE_OVERRIDEN) == KZU_PROPERTY_FLAG_INHERITANCE_OVERRIDEN)
    {
        inherits = (property->flags & KZU_PROPERTY_FLAG_INHERITED) == KZU_PROPERTY_FLAG_INHERITED;
    }

    return inherits;
}

void kzuPropertySetInherited(struct KzuProperty* property, kzBool inherits)
{
    kzsAssert(kzcIsValidPointer(property));

    property->flags |= KZU_PROPERTY_FLAG_INHERITANCE_OVERRIDEN;

    if (inherits)
    {
        property->flags |= KZU_PROPERTY_FLAG_INHERITED;
    }
}
