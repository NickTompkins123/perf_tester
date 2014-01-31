/**
* \file
* Animation base definitions. 
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_ANIMATION_BASE_H
#define KZU_ANIMATION_BASE_H


#include <system/kzs_types.h>


/** List of animation target attributes. */
enum KzuAnimationTargetAttribute
{
    KZU_ANIMATION_TARGET_NO_PROPERTY = 0, /**< Empty animation container, which does not animate anything. */
    KZU_ANIMATION_TARGET_WHOLE_PROPERTY, /**< Specifies that we want to animate the whole property. Always the type of matrix and texture animations. */    
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_X, /**< Translation X */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Y, /**< Translation Y */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Z, /**< Translation Z */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_X, /**< Scale X */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Y, /**< Scale Y */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Z, /**< Scale Z */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_X, /**< Rotation X */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Y, /**< Rotation Y */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Z, /**< Rotation Z */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_FLOAT, /**< Float property. */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_R, /**< Color blue */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_G, /**< Color green */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_B, /**< Color blue */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_A, /**< Color alpha */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_X, /**< Vector4 x */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Y, /**< Vector4 y */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Z, /**< Vector4 z */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_W, /**< Vector4 w */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_X, /**< Vector3 x */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Y, /**< Vector3 y */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Z, /**< Vector3 z */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_X, /**< Vector2 x */
    KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_Y  /**< Vector2 y */
};


#endif
