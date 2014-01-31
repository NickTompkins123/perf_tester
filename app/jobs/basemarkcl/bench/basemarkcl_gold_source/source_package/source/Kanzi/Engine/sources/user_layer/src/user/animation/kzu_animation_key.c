/**
* \file
* Animation key.
* Single keyframe of the animation data.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_animation_key.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/curve/kzc_interpolation.h>
#include <core/util/math/kzc_quaternion.h>
#include <core/util/math/kzc_matrix4x4.h>

#include <system/debug/kzs_log.h>


/* This enables slower, but more accurate method for calculating bezier curves. */
#define USE_2D_BEZIER_INTERPOLATION


/**
* Returns the approximated parameter of a parametric curve
* \param time   Value to evaluate
* \param p0     The first interpolation point
* \param c0     The first control point
* \param c1     The second control point
* \param p1     The second interpolation point
* Returns a point that can be used to retrieve value of the curve
*/
static kzFloat kzuAnimationApproximateCubicBezierParameter_internal(kzFloat time, kzFloat p0, kzFloat c0, kzFloat c1, kzFloat p1);


struct KzuAnimationKey
{
    kzFloat time; /**< Input value for the key. */
    enum KzuAnimationInterpolation interpolation; /**< interpolation method for the interval before the next key. */    
};

struct KzuAnimationKeyFloat
{
    struct KzuAnimationKey animationKey; /**< Animation key. Used for inheritance. */ /*lint -esym(754, KzuAnimationKeyFloat::animationKey) Suppressed as this member is required but not referenced. */ 
    kzFloat value;    /**< The value of the key. */
};

struct KzuAnimationKeyFloatAdvanced
{
    struct KzuAnimationKeyFloat animationKey; /**< Animation key. Used for inheritance. */ /*lint -esym(754, KzuAnimationKeyWithTangent::animationKey) Suppressed as this member is required but not referenced. */ 
    struct KzcVector2 inTangent;  /**< Incoming tangent. */
    struct KzcVector2 outTangent; /**< Outgoing tangent. */
};

struct KzuAnimationKeyMatrix4x4
{
    struct KzuAnimationKey animationKey; /**< Animation key. Used for inheritance. */ /*lint -esym(754, KzuAnimationKeyMatrix4x4::animationKey) Suppressed as this member is required but not referenced. */ 
    struct KzcMatrix4x4 outputMatrix;    /**< Matrix value. */
};

struct KzuAnimationKeyTexture
{
    struct KzuAnimationKey animationKey; /**< Animation key. Used for inheritance. */ /*lint -esym(754, KzuAnimationKeyTexture::animationKey) Suppressed as this member is required but not referenced. */ 
    struct KzcTexture* outputTexture;    /**< Texture value. */
};


static kzFloat kzuAnimationApproximateCubicBezierParameter_internal(kzFloat time, kzFloat p0, kzFloat c0, kzFloat c1, kzFloat p1) 
{
    /* Accepted value can be this far from correct value. */
    const kzFloat APPROXIMATION_EPSILON = 1.0e-09f;
    
    /* Maximum iteration count. */
    #define MAXIMUM_ITERATIONS 1000

    kzFloat returnValue;
    kzInt iterationStep = 0;
    kzFloat u = 0.0f; 
    kzFloat v = 1.0f;

    if (time - p0 < KZ_FLOAT_EPSILON)
    {
        returnValue = 0.0f;
    }
    else if (p1 - time < KZ_FLOAT_EPSILON) 
    {
        returnValue = 1.0f;
    }
    else
    {
        while (iterationStep < MAXIMUM_ITERATIONS) 
        {
            /* De Casteljau Subdivision */
            kzFloat a = (p0 + c0) * 0.5f;
            kzFloat b = (c0 + c1) * 0.5f;
            kzFloat c = (c1 + p1) * 0.5f;
            kzFloat d = (a + b) * 0.5f;
            kzFloat e = (b + c) * 0.5f;
            kzFloat f = (d + e) * 0.5f;

            /* The curve point is close enough */
            if (kzsFabsf(f - time) < APPROXIMATION_EPSILON) 
            {
                break;
            }

            /* Split */
            if (f < time) 
            {
                p0 = f;
                c0 = e;
                c1 = c;
                u = (u + v) * 0.5f;
            }
            else 
            {
                c0 = a;
                c1 = d;
                p1 = f;
                v = (u + v) * 0.5f;
            }
            iterationStep++;
        }
        returnValue = kzsClampf((u + v) * 0.5f, 0.0f, 1.0f);
    }
    return returnValue;
}

static void kzuAnimationKeyInitialize_internal(struct KzuAnimationKey* animationKey, kzFloat time, enum KzuAnimationInterpolation interpolation)
{
    animationKey->time = time;
    animationKey->interpolation = interpolation;
}

kzsError kzuAnimationKeyFloatCreate(const struct KzcMemoryManager* memoryManager, kzFloat time, kzFloat value, 
                                    enum KzuAnimationInterpolation interpolation, struct KzuAnimationKeyFloat** out_animationKey)
{
    kzsError result;
    struct KzuAnimationKeyFloat* animationKey;

    kzsErrorTest(interpolation == KZU_ANIMATION_INTERPOLATION_STEP || interpolation == KZU_ANIMATION_INTERPOLATION_LINEAR, 
                 KZS_ERROR_ILLEGAL_OPERATION, "Use kzuAnimationKeyFloatAdvancedCreate instead.");
    
    result = kzcMemoryAllocVariable(memoryManager, animationKey, "Animation Key Float");
    kzsErrorForward(result);

    animationKey->value = value;
    kzuAnimationKeyInitialize_internal(kzuAnimationKeyFloatToAnimationKey(animationKey), time, interpolation);

    *out_animationKey = animationKey;
    kzsSuccess();
}

kzsError kzuAnimationKeyDelete(struct KzuAnimationKey* animationKey)
{
    kzsError result;

    result = kzcMemoryFreeVariable(animationKey);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuAnimationKeyFloatAdvancedCreate(const struct KzcMemoryManager* memoryManager, kzFloat time, kzFloat value, enum KzuAnimationInterpolation interpolation, 
                                          struct KzcVector2 inTangent, struct KzcVector2 outTangent, struct KzuAnimationKeyFloatAdvanced** out_animationKeyWithTangent)
{
    kzsError result;
    struct KzuAnimationKeyFloatAdvanced* key;

    kzsErrorTest(interpolation == KZU_ANIMATION_INTERPOLATION_BEZIER || interpolation == KZU_ANIMATION_INTERPOLATION_HERMITE, 
        KZS_ERROR_ILLEGAL_OPERATION, "Use kzuAnimationKeyCreate instead.");

    result = kzcMemoryAllocVariable(memoryManager, key, "Bezier Animation Key");
    kzsErrorForward(result);

    key->animationKey.value = value;
    key->inTangent = inTangent;
    key->outTangent = outTangent;
    kzuAnimationKeyInitialize_internal(kzuAnimationKeyFloatAdvancedToAnimationKey(key), time, interpolation);

    *out_animationKeyWithTangent = key;
    kzsSuccess();
}

kzsError kzuAnimationKeyTextureCreate(const struct KzcMemoryManager* memoryManager, kzFloat time, struct KzcTexture* outputTexture, 
                                      struct KzuAnimationKeyTexture** out_animationKeyTexture)
{
    kzsError result;
    struct KzuAnimationKeyTexture* key;

    result = kzcMemoryAllocVariable(memoryManager, key, "Texture Animation Key");
    kzsErrorForward(result);

    key->outputTexture = outputTexture;
    kzuAnimationKeyInitialize_internal(kzuAnimationKeyTextureToAnimationKey(key), time, KZU_ANIMATION_INTERPOLATION_STEP);

    *out_animationKeyTexture = key;
    kzsSuccess();
}

kzsError kzuAnimationKeyMatrix4x4Create(const struct KzcMemoryManager* memoryManager, kzFloat time, const struct KzcMatrix4x4* outputMatrix,
                                          struct KzuAnimationKeyMatrix4x4** out_animationKeyMatrix)
{
    kzsError result;
    struct KzuAnimationKeyMatrix4x4* key;

    result = kzcMemoryAllocVariable(memoryManager, key, "Matrix Animation Key");
    kzsErrorForward(result);

    key->outputMatrix = *outputMatrix;
    kzuAnimationKeyInitialize_internal(kzuAnimationKeyMatrix4x4ToAnimationKey(key), time, KZU_ANIMATION_INTERPOLATION_LINEAR);

    *out_animationKeyMatrix = key;
    kzsSuccess();
}

struct KzcTexture* kzuAnimationKeyTextureInterpolate(const struct KzuAnimationKeyTexture* keyStart, const struct KzuAnimationKeyTexture* keyEnd)
{
    struct KzcTexture* value;

    kzsAssert(keyStart == KZ_NULL || kzcIsValidPointer(keyStart));
    kzsAssert(keyEnd == KZ_NULL   || kzcIsValidPointer(keyEnd));
    kzsAssertText(keyStart == KZ_NULL && keyEnd == KZ_NULL, "Both key's can't be KZ_NULL.");

    if(keyStart == KZ_NULL)
    {
        kzsAssert(kzcIsValidPointer(keyEnd));
        value = keyEnd->outputTexture;
    }
    else /* Step animation. */
    {
        value = keyStart->outputTexture;
    }

    return value;
}

kzFloat kzuAnimationInterpolateLinear(kzFloat time, kzFloat startValue, kzFloat endValue, kzFloat startTime, kzFloat endTime)
{
    return startValue + ((endValue - startValue) * (time - startTime)) / (endTime - startTime);
}

kzFloat kzuAnimationInterpolateBezier(kzFloat time, kzFloat startValue, kzFloat endValue, kzFloat startTime, kzFloat endTime, 
                                      const struct KzcVector2* outTangent, const struct KzcVector2* inTangent)
{
        /* Bezier interpolation
       V = P0(1-s)^3 + T0(3s(1-s)^2) + T1(3s^2(1-s)) + P1(s^3)
       P0 key start value
       P1 key end value
       T0 key start out tangent
       T1 key end in tangent */

#ifdef USE_2D_BEZIER_INTERPOLATION
    kzFloat t = kzuAnimationApproximateCubicBezierParameter_internal(time, 
        startTime,
        kzcVector2GetX(outTangent), 
        kzcVector2GetX(inTangent), 
        endTime);
#else
    kzFloat t = (time - startTime) / (endTime - startTime);
#endif
    kzFloat a = t;
    kzFloat aa = a * a;
    kzFloat b = 1.0f - a;
    kzFloat bb = b * b;
    kzFloat br = 3.0f;
    kzFloat cr = 3.0f;

    return startValue * bb * b   
        + br * kzcVector2GetY(outTangent) * bb * a 
        + cr * kzcVector2GetY(inTangent) * aa * b 
        + endValue * a * aa;
}

kzFloat kzuAnimationInterpolateHermite(kzFloat time, kzFloat startValue, kzFloat endValue, kzFloat startTime, kzFloat endTime, 
                                      const struct KzcVector2* outTangent, const struct KzcVector2* inTangent)
{
    /* Hermite interpolation
       V = P0(2s^3 - 3s^2 + 1) + T0(s^3 - 2s^2 + s) + P1(-2s^3 + 3s^2) + T1(s^3 - s^2)
       P0 key start value
       P1 key end value
       T0 key start out tangent
       T1 key end in tangent */

    kzFloat s = (time - startTime) / (endTime - startTime);
    kzFloat ss = s * s;
    return startValue * (2.0f * ss * s - 3.0f * ss + 1.0f) +
            kzcVector2GetY(outTangent) * (ss * s - 2.0f * ss + s) +
            endValue * (-2.0f * ss*s + 3.0f * ss) +
            kzcVector2GetY(inTangent) * (ss * s - ss);
}

kzFloat kzuAnimationKeyFloatInterpolate(kzFloat input, struct KzuAnimationKeyFloat* keyStart, struct KzuAnimationKeyFloat* keyEnd)
{
    kzFloat value;
    kzFloat time;

    kzsAssert(keyStart == KZ_NULL || kzcIsValidPointer(keyStart));
    kzsAssert(keyEnd == KZ_NULL || kzcIsValidPointer(keyEnd));
    kzsAssertText(keyStart != KZ_NULL || keyEnd != KZ_NULL, "Both keys can't be KZ_NULL.");

    if(keyStart == KZ_NULL)
    {
        kzsAssert(kzcIsValidPointer(keyEnd));
        value = keyEnd->value;
    }
    else if(keyEnd == KZ_NULL)
    {
        kzsAssert(kzcIsValidPointer(keyStart));
        value = keyStart->value;
    }
    else 
    {
        enum KzuAnimationInterpolation interpolationType = keyStart->animationKey.interpolation;
        
        /* Use linear interpolation when keys adjacent keys are not of same type. */
        if(keyEnd->animationKey.interpolation != interpolationType)
        {
            interpolationType = KZU_ANIMATION_INTERPOLATION_LINEAR;
        }
        time = kzsClampf(input, keyStart->animationKey.time, keyEnd->animationKey.time);

        switch(interpolationType)
        {
            case KZU_ANIMATION_INTERPOLATION_STEP:
            {
                value = keyStart->value;
                break;
            }
            case KZU_ANIMATION_INTERPOLATION_LINEAR:
            {
                value = kzuAnimationInterpolateLinear(time, keyStart->value, keyEnd->value, keyStart->animationKey.time, keyEnd->animationKey.time);
                break;
            }
            case KZU_ANIMATION_INTERPOLATION_BEZIER:
            /* TODO: Bezier curves are not defined with tangents, they use control points */
            {                
                struct KzuAnimationKeyFloatAdvanced* keyBezierStart = (struct KzuAnimationKeyFloatAdvanced*)keyStart;
                struct KzuAnimationKeyFloatAdvanced* keyBezierEnd = (struct KzuAnimationKeyFloatAdvanced*)keyEnd;
                
                kzsAssertText(time >= keyStart->animationKey.time, "Data is not valid");
                kzsAssertText(time <= keyEnd->animationKey.time, "Data is not valid");
                
                {
#ifdef USE_2D_BEZIER_INTERPOLATION
                    /* TODO: Explain why time is calculated non-linearly */
                    kzFloat t = kzuAnimationApproximateCubicBezierParameter_internal(time, 
                        keyStart->animationKey.time,
                        kzcVector2GetX(&keyBezierStart->outTangent), 
                        kzcVector2GetX(&keyBezierEnd->inTangent), 
                        keyEnd->animationKey.time);
#else
                    kzFloat t = (time - keyStart->animationKey.time) / (keyEnd->animationKey.time - keyStart->animationKey.time);
#endif
                    kzFloat a = t;
                    kzFloat aa = a * a;
                    kzFloat b = 1.0f - a;
                    kzFloat bb = b * b;
                    kzFloat br = 3.0f;
                    kzFloat cr = 3.0f;

                    value = keyStart->value * bb * b   
                        + br * kzcVector2GetY(&keyBezierStart->outTangent) * bb * a 
                        + cr * kzcVector2GetY(&keyBezierEnd->inTangent) * aa * b 
                        + keyEnd->value * a * aa;
                }

                break;
            }
            case KZU_ANIMATION_INTERPOLATION_HERMITE:
            {
                struct KzuAnimationKeyFloatAdvanced* keyHermiteStart = (struct KzuAnimationKeyFloatAdvanced*)keyStart;
                struct KzuAnimationKeyFloatAdvanced* keyHermiteEnd = (struct KzuAnimationKeyFloatAdvanced*)keyEnd;

                kzsAssertText(time >= keyStart->animationKey.time, "Data is not valid");
                kzsAssertText(time <= keyEnd->animationKey.time, "Data is not valid");

                value = kzuAnimationInterpolateHermite(time, keyStart->value, keyEnd->value, 
                    keyStart->animationKey.time, keyEnd->animationKey.time, &keyHermiteStart->outTangent, &keyHermiteEnd->inTangent);
                break;
            }
            default:
            {
                value = 0.0f;
                kzsAssert(KZ_FALSE);
            }
        }
    }

    return value;
}

void kzuAnimationKeyMatrix4x4Interpolate(kzFloat time, const struct KzuAnimationKeyMatrix4x4* keyStart, const struct KzuAnimationKeyMatrix4x4* keyEnd, 
                                         struct KzcMatrix4x4* out_matrix)
{
    struct KzcMatrix4x4 matrix;

    kzsAssert(keyStart == KZ_NULL || kzcIsValidPointer(keyStart));
    kzsAssert(keyEnd == KZ_NULL || kzcIsValidPointer(keyEnd));
    kzsAssertText(keyStart != KZ_NULL || keyEnd != KZ_NULL, "Both key's can't be KZ_NULL.");

    if(keyStart == KZ_NULL)
    {
        kzsAssert(kzcIsValidPointer(keyEnd));
        matrix = keyEnd->outputMatrix;
    }
    else if(keyEnd == KZ_NULL)
    {
        matrix = keyStart->outputMatrix;
    }
    else
    {
        struct KzcQuaternion q1;
        struct KzcQuaternion q2;
        struct KzcQuaternion resultQuaternion;
        kzcQuaternionInitializeFromMatrix4x4(&keyStart->outputMatrix, &q1);
        kzcQuaternionInitializeFromMatrix4x4(&keyEnd->outputMatrix, &q2);
        kzcQuaternionSlerp(&q1, &q2, time, &resultQuaternion);
        kzcQuaternionConvertToMatrix4x4(&resultQuaternion, &matrix);

        {
            kzInt i;
            kzUint translationComponents[3] = 
            {
                KZC_MATRIX4X4_INDEX_TRANSLATION_X,
                KZC_MATRIX4X4_INDEX_TRANSLATION_Y,
                KZC_MATRIX4X4_INDEX_TRANSLATION_Z
            };
            for(i = 0; i < 3; ++i)
            {
                kzUint component = translationComponents[i];
                matrix.data[component] = kzcInterpolateLinear(keyStart->outputMatrix.data[component], 
                    keyEnd->outputMatrix.data[component], time);
            }
        }
    }
    *out_matrix = matrix;
}

kzFloat kzuAnimationKeyGetTime(const struct KzuAnimationKey* key)
{
    kzsAssert(kzcIsValidPointer(key));
    return key->time;
}

kzFloat kzuAnimationKeyFloatGetValue(const struct KzuAnimationKeyFloat* key)
{
    kzsAssert(kzcIsValidPointer(key));
    return key->value;
}

struct KzuAnimationKey* kzuAnimationKeyFloatToAnimationKey(const struct KzuAnimationKeyFloat* key)
{
    kzsAssert(kzcIsValidPointer(key));
    return (struct KzuAnimationKey*)key;
}

struct KzuAnimationKeyFloat* kzuAnimationKeyToAnimationKeyFloat(const struct KzuAnimationKey* key)
{
    kzsAssert(kzcIsValidPointer(key));
    return (struct KzuAnimationKeyFloat*)key;
}

struct KzuAnimationKey* kzuAnimationKeyFloatAdvancedToAnimationKey(const struct KzuAnimationKeyFloatAdvanced* key)
{
    kzsAssert(kzcIsValidPointer(key));
    return (struct KzuAnimationKey*)&key->animationKey;
}

struct KzuAnimationKeyFloatAdvanced* kzuAnimationKeyFloatAdvancedFromAnimationKey(const struct KzuAnimationKey* key)
{
    kzsAssert(kzcIsValidPointer(key));
    return (struct KzuAnimationKeyFloatAdvanced*)(struct KzuAnimationKeyFloat*)key;
}

struct KzuAnimationKey* kzuAnimationKeyTextureToAnimationKey(const struct KzuAnimationKeyTexture* key)
{
    kzsAssert(kzcIsValidPointer(key));
    return (struct KzuAnimationKey*)key;
}

struct KzuAnimationKeyTexture* kzuAnimationKeyToAnimationKeyTexture(const struct KzuAnimationKey* key)
{
    kzsAssert(kzcIsValidPointer(key));
    return (struct KzuAnimationKeyTexture*)key;
}

struct KzuAnimationKey* kzuAnimationKeyMatrix4x4ToAnimationKey(const struct KzuAnimationKeyMatrix4x4* key)
{
    kzsAssert(kzcIsValidPointer(key));
    return (struct KzuAnimationKey*)key;
}

struct KzuAnimationKeyMatrix4x4* kzuAnimationKeyToAnimationKeyMatrix4x4(const struct KzuAnimationKey* key)
{
    kzsAssert(kzcIsValidPointer(key));
    return (struct KzuAnimationKeyMatrix4x4*)key;
}
