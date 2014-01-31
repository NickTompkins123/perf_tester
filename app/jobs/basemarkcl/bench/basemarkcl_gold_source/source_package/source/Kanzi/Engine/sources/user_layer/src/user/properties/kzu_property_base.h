/**
* \file
* Specifies property base struct.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROPERTY_BASE_H
#define KZU_PROPERTY_BASE_H


#include <system/kzs_types.h>
#include <system/wrappers/kzs_opengl_base.h>


/** Enumeration for defining property priorities. */
enum KzuPropertyPriority
{
    KZU_PROPERTY_PRIORITY_MIN,      /**< Minimum priority. */
    KZU_PROPERTY_PRIORITY_LOW,      /**< Low priority. */
    KZU_PROPERTY_PRIORITY_NORMAL,   /**< Normal priority. */
    KZU_PROPERTY_PRIORITY_HIGH,     /**< High priority. */
    KZU_PROPERTY_PRIORITY_MAX       /**< Maximum priority. */
};

/** Datatype of property */
enum KzuPropertyDataType
{
    KZU_PROPERTY_DATA_TYPE_FLOAT,            /**< Float value. */
    KZU_PROPERTY_DATA_TYPE_INT,              /**< Integer. */
    KZU_PROPERTY_DATA_TYPE_BOOL,             /**< Boolean. */
    KZU_PROPERTY_DATA_TYPE_COLOR,            /**< Color. */
    KZU_PROPERTY_DATA_TYPE_VECTOR2,          /**< 2-component vector. */
    KZU_PROPERTY_DATA_TYPE_VECTOR3,          /**< 3-component vector. */
    KZU_PROPERTY_DATA_TYPE_VECTOR4,          /**< 4-component vector. */
    KZU_PROPERTY_DATA_TYPE_MATRIX2X2,        /**< Matrix2x2. */
    KZU_PROPERTY_DATA_TYPE_MATRIX3X3,        /**< Matrix3x3. */
    KZU_PROPERTY_DATA_TYPE_MATRIX4X4,        /**< Matrix4x4. */
    KZU_PROPERTY_DATA_TYPE_STRING,           /**< String. */
    KZU_PROPERTY_DATA_TYPE_VOID,             /**< Void. */
    KZU_PROPERTY_DATA_TYPE_LIGHT,            /**< Light. */
    KZU_PROPERTY_DATA_TYPE_TEXTURE           /**< Texture. */
};


#endif
