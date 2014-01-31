/**
* \file
* Animation key.
* Single key frame of the animation data.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_ANIMATION_KEY_H
#define KZU_ANIMATION_KEY_H

#include <core/util/math/kzc_vector2.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Type of the animation key data. */
enum KzuAnimationTargetType
{
    KZU_ANIMATION_TARGET_FLOAT,   /**< Animation target is float. */
    KZU_ANIMATION_TARGET_TEXTURE, /**< Animation target is texture. */
    KZU_ANIMATION_TARGET_MATRIX   /**< Animation target is matrix. */
};

/** Used interpolation method for animation key frame. */
enum KzuAnimationInterpolation
{
    KZU_ANIMATION_INTERPOLATION_STEP,   /**< Interpolation step. */
    KZU_ANIMATION_INTERPOLATION_LINEAR, /**< Interpolation linear. */ 
    KZU_ANIMATION_INTERPOLATION_BEZIER, /**< Interpolation Bezier. */
    KZU_ANIMATION_INTERPOLATION_HERMITE /**< Interpolation Hermite. */
};


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcTexture;
struct KzcMatrix4x4;


/**
 * \struct KzuAnimationKey
 * Animation key frame base class.
 */ 
struct KzuAnimationKey;

/**
 * \struct KzuAnimationKeyFloat
 * Float animation key.
 */
struct KzuAnimationKeyFloat;

/**
 * \struct KzuAnimationKeyFloatAdvanced
 * Animation key frame for Bezier and Hermite interpolation. 
 * Can hold tangents or bezier control points.
 */ 
struct KzuAnimationKeyFloatAdvanced;

/**
 * \struct KzuAnimationKeyMatrix4x4
 * Animation key frame for matrix animations.
 */
struct KzuAnimationKeyMatrix4x4;

/**
 * \struct KzuAnimationKeyTexture
 * Animation key frame for texture animations. 
 */
struct KzuAnimationKeyTexture;


/** Creates animation key */
kzsError kzuAnimationKeyFloatCreate(const struct KzcMemoryManager* memoryManager, kzFloat time, kzFloat value, 
                                    enum KzuAnimationInterpolation interpolation, struct KzuAnimationKeyFloat** out_animationKey);
/** Creates animation key, which has tangents or control points. 
* \param inPoint The in tangent or in control point. Used when calculating the shape of curve entering the key frame point
* \param outPoint The out tangent or out control point. Used when calculating the shape of curve leaving the control point.
*/
kzsError kzuAnimationKeyFloatAdvancedCreate(const struct KzcMemoryManager* memoryManager, kzFloat time, kzFloat value, 
                                          enum KzuAnimationInterpolation interpolation, struct KzcVector2 inPoint, 
                                          struct KzcVector2 outPoint, struct KzuAnimationKeyFloatAdvanced** out_animationKey);

/** Creates new animation key for texture animation. */
kzsError kzuAnimationKeyTextureCreate(const struct KzcMemoryManager* memoryManager, kzFloat time, struct KzcTexture* outputTexture, 
                                      struct KzuAnimationKeyTexture** out_animationKeyTexture);

/** Creates animation key for matrix4x4 animation. */
kzsError kzuAnimationKeyMatrix4x4Create(const struct KzcMemoryManager* memoryManager, kzFloat time, const struct KzcMatrix4x4* outputMatrix,
                                        struct KzuAnimationKeyMatrix4x4** out_animationKeyMatrix);

/** Deallocates memory reserved for animation key. */
kzsError kzuAnimationKeyDelete(struct KzuAnimationKey* animationKey);

/** Gets time from a given KzuAnimationKey. */
kzFloat kzuAnimationKeyGetTime(const struct KzuAnimationKey* key);

/** Gets output value from a given KzuAnimationFloatKey. */
kzFloat kzuAnimationKeyFloatGetValue(const struct KzuAnimationKeyFloat* key);


/** Type casts KzuAnimationKeyFloat to KzuAnimationKey. */
struct KzuAnimationKey* kzuAnimationKeyFloatToAnimationKey(const struct KzuAnimationKeyFloat* key);
/** Type casts KzuAnimationKeyFloat from KzuAnimationKey. */
struct KzuAnimationKeyFloat* kzuAnimationKeyToAnimationKeyFloat(const struct KzuAnimationKey* key);

/** Type casts KzuAnimationKeyFloatAdvanced to KzuAnimationKey. */
struct KzuAnimationKey* kzuAnimationKeyFloatAdvancedToAnimationKey(const struct KzuAnimationKeyFloatAdvanced* key);
/** Type casts KzuAnimationKeyFloatAdvanced from KzuAnimationKey. */
struct KzuAnimationKeyFloatAdvanced* kzuAnimationKeyFloatAdvancedFromAnimationKey(const struct KzuAnimationKey* key);

/** Type casts KzuAnimationKeyTexture to KzuAnimationKey. */
struct KzuAnimationKey* kzuAnimationKeyTextureToAnimationKey(const struct KzuAnimationKeyTexture* key);
/** Type casts KzuAnimationKeyTexture from KzuAnimationKey. */
struct KzuAnimationKeyTexture* kzuAnimationKeyToAnimationKeyTexture(const struct KzuAnimationKey* key);

/** Type casts KzuAnimationKeyMatrix4x4 to KzuAnimationKey. */
struct KzuAnimationKey* kzuAnimationKeyMatrix4x4ToAnimationKey(const struct KzuAnimationKeyMatrix4x4* key);
/** Type casts KzuAnimationKeyMatrix4x4 from KzuAnimationKey. */
struct KzuAnimationKeyMatrix4x4* kzuAnimationKeyToAnimationKeyMatrix4x4(const struct KzuAnimationKey* key);

/** Returns the output value from given point input, which is located between the two keys. */
kzFloat kzuAnimationKeyFloatInterpolate(kzFloat input, struct KzuAnimationKeyFloat* keyStart, 
                                                struct KzuAnimationKeyFloat* keyEnd);

/** Returns the interpolated output value from input point, which is between the given two keys. */
void kzuAnimationKeyMatrix4x4Interpolate(kzFloat input, const struct KzuAnimationKeyMatrix4x4* keyStart, 
                                                 const struct KzuAnimationKeyMatrix4x4* keyEnd, struct KzcMatrix4x4* out_matrix);

/** Returns the texture from given key frames. The interpolation is made with step interpolation. */
struct KzcTexture* kzuAnimationKeyTextureInterpolate(const struct KzuAnimationKeyTexture* keyStart, 
                                                             const struct KzuAnimationKeyTexture* keyEnd);

/** Returns linearly interpolated value between given two values. */
kzFloat kzuAnimationInterpolateLinear(kzFloat time, kzFloat startValue, kzFloat endValue, kzFloat startTime, kzFloat endTime);

/** Returns cubic bezier interpolated value from given values. */
kzFloat kzuAnimationInterpolateBezier(kzFloat time, kzFloat startValue, kzFloat endValue, kzFloat startTime, kzFloat endTime, 
                                      const struct KzcVector2* outTangent, const struct KzcVector2* inTangent);

/** Returns hermite interpolated value from given values. */
kzFloat kzuAnimationInterpolateHermite(kzFloat time, kzFloat startValue, kzFloat endValue, kzFloat startTime, kzFloat endTime, 
                                       const struct KzcVector2* outTangent, const struct KzcVector2* inTangent);


#endif
