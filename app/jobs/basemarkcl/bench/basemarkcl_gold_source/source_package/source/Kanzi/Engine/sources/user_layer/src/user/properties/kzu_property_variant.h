/**
* \file
* Property variant.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROPERTY_VARIANT_H
#define KZU_PROPERTY_VARIANT_H


#include <user/properties/kzu_property_base.h>

#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector4.h>
#include <core/util/math/kzc_matrix2x2.h>
#include <core/util/math/kzc_matrix3x3.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/color/kzc_color.h>
#include <core/util/string/kzc_string.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


struct KzuPropertyVariant 
{
    enum KzuPropertyDataType dataType;  /**< Data type stored in the variant. */

    union
    {
        kzFloat floatData;                  /**< Float data.*/
        kzInt intData;                      /**< Int data.*/
        kzBool boolData;                    /**< Bool data.*/
        struct KzcColorRGBA colorData;      /**< Color data.*/
        struct KzcVector2 vector2Data;      /**< Vector2 data.*/
        struct KzcVector3 vector3Data;      /**< Vector3 data.*/
        struct KzcVector4 vector4Data;      /**< Vector4 data.*/
        struct KzcMatrix2x2 matrix2x2Data;  /**< Matrix2x2 data.*/
        struct KzcMatrix3x3 matrix3x3Data;  /**< Matrix3x3 data.*/
        struct KzcMatrix4x4 matrix4x4Data;  /**< Matrix4x4 data.*/
        kzMutableString stringData;         /**< String data.*/
        void* voidData;                     /**< Void data.*/
    } data;
};


KZ_INLINE kzsError kzuPropertyVariantUninitialize(const struct KzuPropertyVariant* variant)
{
    kzsError result;

    kzsAssert(variant != KZ_NULL);

    if (variant->dataType == KZU_PROPERTY_DATA_TYPE_STRING && variant->data.stringData != KZ_NULL)
    {
        result = kzcStringDelete(variant->data.stringData);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_INLINE enum KzuPropertyDataType kzuPropertyVariantGetType(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);

    return variant->dataType;
}

KZ_INLINE void kzuPropertyVariantInitializeFloat(struct KzuPropertyVariant* variant, kzFloat value)
{
    kzsAssert(variant != KZ_NULL);
    
    variant->dataType = KZU_PROPERTY_DATA_TYPE_FLOAT;
    variant->data.floatData = value;
}

KZ_INLINE kzFloat kzuPropertyVariantGetFloat(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_FLOAT);

    return variant->data.floatData;
}

KZ_INLINE void kzuPropertyVariantInitializeInt(struct KzuPropertyVariant* variant, kzInt value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_INT;
    variant->data.intData = value;
}

KZ_INLINE kzInt kzuPropertyVariantGetInt(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_INT);

    return variant->data.intData;
}

KZ_INLINE void kzuPropertyVariantInitializeBool(struct KzuPropertyVariant* variant, kzBool value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_BOOL;
    variant->data.boolData = value;
}

KZ_INLINE kzBool kzuPropertyVariantGetBool(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_BOOL);

    return variant->data.boolData;
}

KZ_INLINE void kzuPropertyVariantInitializeColor(struct KzuPropertyVariant* variant, struct KzcColorRGBA value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_COLOR;
    variant->data.colorData = value;
}

KZ_INLINE struct KzcColorRGBA kzuPropertyVariantGetColor(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_COLOR);

    return variant->data.colorData;
}

KZ_INLINE void kzuPropertyVariantInitializeVector2(struct KzuPropertyVariant* variant, struct KzcVector2 value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_VECTOR2;
    variant->data.vector2Data = value;
}

KZ_INLINE struct KzcVector2 kzuPropertyVariantGetVector2(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_VECTOR2);

    return variant->data.vector2Data;
}

KZ_INLINE void kzuPropertyVariantInitializeVector3(struct KzuPropertyVariant* variant, struct KzcVector3 value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_VECTOR3;
    variant->data.vector3Data = value;
}

KZ_INLINE struct KzcVector3 kzuPropertyVariantGetVector3(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_VECTOR3);

    return variant->data.vector3Data;
}

KZ_INLINE void kzuPropertyVariantInitializeVector4(struct KzuPropertyVariant* variant, struct KzcVector4 value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_VECTOR4;
    variant->data.vector4Data = value;
}

KZ_INLINE struct KzcVector4 kzuPropertyVariantGetVector4(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_VECTOR4);

    return variant->data.vector4Data;
}

KZ_INLINE void kzuPropertyVariantInitializeMatrix2x2(struct KzuPropertyVariant* variant, struct KzcMatrix2x2 value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_MATRIX2X2;
    variant->data.matrix2x2Data = value;
}

KZ_INLINE struct KzcMatrix2x2 kzuPropertyVariantGetMatrix2x2(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_MATRIX2X2);

    return variant->data.matrix2x2Data;
}

KZ_INLINE void kzuPropertyVariantInitializeMatrix3x3(struct KzuPropertyVariant* variant, struct KzcMatrix3x3 value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_MATRIX3X3;
    variant->data.matrix3x3Data = value;
}

KZ_INLINE struct KzcMatrix3x3 kzuPropertyVariantGetMatrix3x3(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_MATRIX3X3);

    return variant->data.matrix3x3Data;
}

KZ_INLINE void kzuPropertyVariantInitializeMatrix4x4(struct KzuPropertyVariant* variant, struct KzcMatrix4x4 value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_MATRIX4X4;
    variant->data.matrix4x4Data = value;
}

KZ_INLINE struct KzcMatrix4x4 kzuPropertyVariantGetMatrix4x4(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_MATRIX4X4);

    return variant->data.matrix4x4Data;
}

KZ_INLINE kzsError kzuPropertyVariantInitializeString(const struct KzcMemoryManager* memoryManager, struct KzuPropertyVariant* variant, kzString value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_STRING;

    if (value == KZ_NULL)
    {
        variant->data.stringData = KZ_NULL;
    }
    else
    {
        kzsError result;
        result = kzcStringCopy(memoryManager, value, &variant->data.stringData);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_INLINE kzString kzuPropertyVariantGetString(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_STRING);

    return variant->data.stringData;
}

KZ_INLINE void kzuPropertyVariantInitializeVoid(struct KzuPropertyVariant* variant, void* value)
{
    kzsAssert(variant != KZ_NULL);

    variant->dataType = KZU_PROPERTY_DATA_TYPE_VOID;
    variant->data.voidData = value;
}

KZ_INLINE void* kzuPropertyVariantGetVoid(const struct KzuPropertyVariant* variant)
{
    kzsAssert(variant != KZ_NULL);
    kzsAssert(variant->dataType == KZU_PROPERTY_DATA_TYPE_VOID);

    return variant->data.voidData;
}


#endif
