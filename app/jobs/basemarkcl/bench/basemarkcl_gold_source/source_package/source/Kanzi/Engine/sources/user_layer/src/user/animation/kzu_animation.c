/**
* \file
* Animation.
* Contains animation key frames and target for animation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_animation.h"

#include <user/animation/kzu_animation_key.h>
#include <user/animation/kzu_time_line_entry.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager_float.h>
#include <user/properties/kzu_property_manager_vector2.h>
#include <user/properties/kzu_property_manager_vector3.h>
#include <user/properties/kzu_property_manager_vector4.h>
#include <user/properties/kzu_property_manager_matrix4x4.h>
#include <user/properties/kzu_property_manager_color.h>
#include <user/properties/kzu_property_manager_void.h>
#include <user/properties/kzu_property_manager_int.h>
#include <user/properties/kzu_property_manager_bool.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/color/kzc_color.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/kzc_error_codes.h>


struct KzuAnimationData
{
    kzBool selfOwned; /**< Indicates that the object source container will be deleted when this data is deleted. */
    struct KzuPropertyManager* propertyManager;         /**< Property manager used during animation. */
    enum KzuAnimationTargetType targetType;             /**< Target of the animation. */
    enum KzuAnimationTargetAttribute targetAttribute;   /**< Target attribute in the property. */
    struct KzuPropertyType* targetPropertyType;         /**< Property type to animate. */
    const void* targetObject;                           /**< Target object to animate. */

    struct KzcDynamicArray* children;      /**< List of child animations contained in this animation. <KzuAnimation> */
    struct KzcDynamicArray* animationKeys; /**< Keys used for input (time) to output (animated value). 
                                           Ordered chronologically by the starting time of the key. <KzuAnimationKey> */
};

struct KzuAnimation
{
    struct KzuAnimationData* data;
};


/** Returns true if animation target is transformation matrix property. */
kzBool kzuAnimationIsTargetTransformation_internal(const struct KzuAnimation* animation);

/** Internal helper animating a property with a texture. */
kzsError kzuAnimationAnimatePropertyTexture_internal(const struct KzuPropertyManager* propertyManager,
                                                     const struct KzuPropertyType* targetPropertyType, const void* targetObject,
                                                     struct KzcTexture* texture);
/** Internal helper animating a property with a matrix. */
kzsError kzuAnimationAnimatePropertyMatrix4x4_internal(const struct KzuPropertyManager* propertyManager,
                                                       const struct KzuPropertyType* targetPropertyType, const void* targetObject,
                                                       const struct KzcMatrix4x4* matrix);


static kzsError kzuAnimationCreate_internal(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                            enum KzuAnimationTargetType type, const void* targetObject, struct KzuPropertyType* targetPropertyType,
                                            enum KzuAnimationTargetAttribute attribute, 
                                            struct KzuAnimation** out_animation)
{
    kzsError result;
    struct KzuAnimation* animation;

    result = kzcMemoryAllocVariable(memoryManager, animation, "Animation");
    kzsErrorForward(result);
    result = kzcMemoryAllocVariable(memoryManager, animation->data, "AnimationData");
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &animation->data->animationKeys);
    kzsErrorForward(result);
    result = kzcDynamicArrayCreate(memoryManager, &animation->data->children);
    kzsErrorForward(result);

    animation->data->selfOwned = KZ_TRUE;
    animation->data->propertyManager = propertyManager;
    animation->data->targetType = type;
    animation->data->targetAttribute = attribute;
    animation->data->targetPropertyType = targetPropertyType;
    animation->data->targetObject = targetObject;

    *out_animation = animation;
    kzsSuccess();
}


kzsError kzuAnimationCreateMatrix(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                  struct KzuPropertyType* targetPropertyType, const void* targetObject,
                                  struct KzuAnimation** out_animation)
{
    kzsError result;
    struct KzuAnimation* animation;

    result = kzuAnimationCreate_internal(memoryManager, propertyManager, KZU_ANIMATION_TARGET_MATRIX, targetObject, targetPropertyType,
                                         KZU_ANIMATION_TARGET_WHOLE_PROPERTY, &animation);
    kzsErrorForward(result);

    *out_animation = animation;
    kzsSuccess();
}

kzsError kzuAnimationCreateTexture(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                   struct KzuPropertyType* targetPropertyType, const void* targetObject,
                                   struct KzuAnimation** out_animation)
{
    kzsError result;
    struct KzuAnimation* animation;

    result = kzuAnimationCreate_internal(memoryManager, propertyManager, KZU_ANIMATION_TARGET_TEXTURE, targetObject, targetPropertyType,
                                         KZU_ANIMATION_TARGET_WHOLE_PROPERTY, &animation);
    kzsErrorForward(result);

    *out_animation = animation;
    kzsSuccess();
}

kzsError kzuAnimationCreateFloatTarget(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                       struct KzuPropertyType* targetPropertyType, const void* targetObject,
                                       enum KzuAnimationTargetAttribute targetProperty, struct KzuAnimation** out_animation)
{
    kzsError result;
    struct KzuAnimation* animation;

    result = kzuAnimationCreate_internal(memoryManager, propertyManager, KZU_ANIMATION_TARGET_FLOAT, targetObject, targetPropertyType,
                                         targetProperty, &animation);
    kzsErrorForward(result);

    *out_animation = animation;
    kzsSuccess();
}

kzsError kzuAnimationDelete(struct KzuAnimation* animation)
{
    kzsError result;
    kzBool selfOwned;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzuAnimationIsValid(animation));
    selfOwned = animation->data->selfOwned;

    it = kzcDynamicArrayGetIterator(animation->data->animationKeys);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuAnimationKey* animationKey = (struct KzuAnimationKey*)kzcDynamicArrayIteratorGetValue(it);
        result = kzuAnimationKeyDelete(animationKey);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayDelete(animation->data->animationKeys);
    kzsErrorForward(result);

    result = kzcDynamicArrayDelete(animation->data->children);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(animation->data);
    kzsErrorForward(result);

    if(selfOwned)
    {
        result = kzcMemoryFreeVariable(animation);
        kzsErrorForward(result);
    }
    else
    {
        animation->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuAnimationAddChild(const struct KzuAnimation* parent, struct KzuAnimation* child)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(parent));
    kzsAssert(kzcIsValidPointer(child));
    
    kzsErrorTest(parent != child, KZS_ERROR_ILLEGAL_ARGUMENT, "Trying to add animation as child of itself.");

    result = kzcDynamicArrayAdd(parent->data->children, child);
    kzsErrorForward(result);

    kzsSuccess();
}


kzsError kzuAnimationRemoveChild(const struct KzuAnimation* parent, const struct KzuAnimation* child)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(parent));
    kzsAssert(kzcIsValidPointer(child));

    result = kzcDynamicArrayRemove(parent->data->children, child);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcDynamicArrayIterator kzuAnimationGetChildIterator(const struct KzuAnimation* parent)
{
    kzsAssert(kzcIsValidPointer(parent));

    return kzcDynamicArrayGetIterator(parent->data->children);
}

kzsError kzuAnimationAddKeyFrame(const struct KzuAnimation* animation, struct KzuAnimationKey* animationKey)
{
    kzsError result;
    kzUint keyCount;

    kzsAssert(kzcIsValidPointer(animation));
    kzsAssert(kzcIsValidPointer(animationKey));

    keyCount = kzcDynamicArrayGetSize(animation->data->animationKeys);
    if(keyCount == 0)
    {
        result = kzcDynamicArrayAdd(animation->data->animationKeys, animationKey);
        kzsErrorForward(result);
    }
    else
    {
        struct KzuAnimationKey* lastKey = (struct KzuAnimationKey*)kzcDynamicArrayGet(animation->data->animationKeys, keyCount - 1);
        kzFloat newKeyTime = kzuAnimationKeyGetTime(animationKey);
        kzFloat lastKeyTime = kzuAnimationKeyGetTime(lastKey);

        if(lastKeyTime < newKeyTime)
        {
            /* Add to the end. */
            result = kzcDynamicArrayAdd(animation->data->animationKeys, animationKey);
            kzsErrorForward(result);
        }
        else
        {
            /* Add to the correct position in the middle of the queue. */
            kzUint index;
            struct KzuAnimationKey* keySmaller;
            struct KzuAnimationKey* keyBigger;
            if(keyCount > 1)
            {
                result = kzuAnimationGetKeysAtTime(animation, newKeyTime, &keySmaller, &keyBigger);
                kzsErrorForward(result);
                
                /* The required position is before first key frame. */
                if(keySmaller == KZ_NULL)
                {
                    index = 0;
                }
                else
                {
                    kzBool found = kzcDynamicArrayFindIndex(animation->data->animationKeys, keySmaller, &index);
                    kzsErrorTest(found, KZC_ERROR_ELEMENT_NOT_FOUND, "Given animation key was not found");
                }
            }
            else
            {
                /* Must be the new smallest key. */
                index = 0;
            }

            result = kzcDynamicArrayInsert(animation->data->animationKeys, index, animationKey);
            kzsErrorForward(result);
        }            
    }

    kzsSuccess();
}

kzsError kzuAnimationRemoveKeyFrame(const struct KzuAnimation* animation, const struct KzuAnimationKey* animationKey)
{
    kzsError result;

    result = kzcDynamicArrayRemove(animation->data->animationKeys, animationKey);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcDynamicArrayIterator kzuAnimationGetKeyFrameIterator(const struct KzuAnimation* parent)
{
    kzsAssert(kzcIsValidPointer(parent));

    return kzcDynamicArrayGetIterator(parent->data->animationKeys);
}

kzsError kzuAnimationGetKeysAtTime(const struct KzuAnimation* animation, kzFloat time, 
                                   struct KzuAnimationKey** out_animationKey1, struct KzuAnimationKey** out_animationKey2)
{
    kzUint keyCount;
    struct KzuAnimationKey* keySmaller = KZ_NULL;
    struct KzuAnimationKey* keyBigger = KZ_NULL;
    
    kzsAssert(kzcIsValidPointer(animation));
    
    keyCount = kzcDynamicArrayGetSize(animation->data->animationKeys);

    if(keyCount == 1)
    {
        struct KzuAnimationKey* key = (struct KzuAnimationKey*)kzcDynamicArrayGet(animation->data->animationKeys, 0);
        if(time < kzuAnimationKeyGetTime(key))
        {
            keySmaller = KZ_NULL;
            keyBigger = key;
        }
        else
        {
            keySmaller = key;
            keyBigger = KZ_NULL;
        }
    }
    else if(keyCount >= 2)
    {
        kzUint smallerIndex = 0;
        kzUint biggerIndex = keyCount - 1;
        keySmaller = (struct KzuAnimationKey*)kzcDynamicArrayGet(animation->data->animationKeys, smallerIndex);
        keyBigger = (struct KzuAnimationKey*)kzcDynamicArrayGet(animation->data->animationKeys, biggerIndex);

        kzsAssert(kzcIsValidPointer(keySmaller));
        kzsAssert(kzcIsValidPointer(keyBigger));

        /* Handle cases when time is outside the key frames. */
        if(time <= kzuAnimationKeyGetTime(keySmaller))
        {
            keyBigger = keySmaller;
            keySmaller = KZ_NULL;
        }
        else if(time >= kzuAnimationKeyGetTime(keyBigger))
        {
            keySmaller = keyBigger;
            keyBigger = KZ_NULL;
        }
        else
        {
            while(smallerIndex < biggerIndex)
            {
                kzUint midpoint = smallerIndex + (biggerIndex - smallerIndex) / 2;
                struct KzuAnimationKey* key = (struct KzuAnimationKey*)kzcDynamicArrayGet(animation->data->animationKeys, midpoint);
                kzFloat keyTime = kzuAnimationKeyGetTime(key);
                if(keyTime < time)
                {
                    smallerIndex = midpoint + 1;
                }
                else
                {
                    biggerIndex = midpoint;
                }
                kzsAssertText(smallerIndex <= biggerIndex, "Keyframe data incorrect.");
            }
            /* Fetch the correct keys. */
            keySmaller = (struct KzuAnimationKey*)kzcDynamicArrayGet(animation->data->animationKeys, smallerIndex - 1);
            keyBigger = (struct KzuAnimationKey*)kzcDynamicArrayGet(animation->data->animationKeys, smallerIndex);  
            kzsAssert(kzcIsValidPointer(keySmaller));
            kzsAssert(kzcIsValidPointer(keyBigger));          
        }
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "kzuAnimationGetKeysAtTime animation with no keyframes found.");
    }
    kzsAssertText(keySmaller != keyBigger, "Keyframe data is not correct.");

    *out_animationKey1 = keySmaller;
    *out_animationKey2 = keyBigger;
    kzsSuccess();
}

kzsError kzuAnimationApply(const struct KzuAnimation* animation, const struct KzcHashMap* overrides, kzFloat time, kzBool relative)
{
    kzsError result;
    struct KzuAnimationKey* keySmaller;
    struct KzuAnimationKey* keyBigger;
    struct KzuPropertyManager* propertyManager = animation->data->propertyManager;
    const struct KzuPropertyType* targetPropertyType = animation->data->targetPropertyType;
    const void* targetObject = animation->data->targetObject;

    kzsAssert(kzcIsValidPointer(animation));

    if (overrides != KZ_NULL && targetObject != KZ_NULL)
    {
        struct KzuAnimationOverride original;
        struct KzuAnimationOverride* override;
        original.object = targetObject;
        original.propertyType = targetPropertyType;

        if (kzcHashMapGet(overrides, &original, (void**)&override))
        {
            targetObject = override->object;
            targetPropertyType = override->propertyType;
        }
    }

    if(animation->data->targetAttribute != KZU_ANIMATION_TARGET_NO_PROPERTY && !kzcDynamicArrayIsEmpty(animation->data->animationKeys) &&
        targetObject != KZ_NULL)
    {
        result = kzuAnimationGetKeysAtTime(animation, time, &keySmaller, &keyBigger);
        kzsErrorForward(result);

        switch(animation->data->targetType)
        {
            case KZU_ANIMATION_TARGET_FLOAT:
            {
                struct KzuAnimationKeyFloat* keySmallerFloat = (keySmaller == KZ_NULL ? KZ_NULL : kzuAnimationKeyToAnimationKeyFloat(keySmaller));
                struct KzuAnimationKeyFloat* keyBiggerFloat  = (keyBigger == KZ_NULL  ? KZ_NULL : kzuAnimationKeyToAnimationKeyFloat(keyBigger));
                kzFloat value = kzuAnimationKeyFloatInterpolate(time, keySmallerFloat, keyBiggerFloat);
                result = kzuAnimationAnimatePropertyFloat(propertyManager, targetPropertyType, animation->data->targetAttribute, targetObject, relative, value);
                kzsErrorForward(result);
                break;
            }
            case KZU_ANIMATION_TARGET_TEXTURE:
            {
                struct KzuAnimationKeyTexture* keySmallerTexture = (keySmaller == KZ_NULL ? KZ_NULL : kzuAnimationKeyToAnimationKeyTexture(keySmaller));
                struct KzuAnimationKeyTexture* keyBiggerTexture  = (keyBigger == KZ_NULL  ? KZ_NULL : kzuAnimationKeyToAnimationKeyTexture(keyBigger));
                struct KzcTexture* texture = kzuAnimationKeyTextureInterpolate(keySmallerTexture, keyBiggerTexture);
                result = kzuAnimationAnimatePropertyTexture_internal(propertyManager, targetPropertyType, targetObject, texture);
                kzsErrorForward(result);
                break;
            }
            case KZU_ANIMATION_TARGET_MATRIX:
            {
                struct KzuAnimationKeyMatrix4x4* keySmallerMatrix = (keySmaller == KZ_NULL ? KZ_NULL : kzuAnimationKeyToAnimationKeyMatrix4x4(keySmaller));
                struct KzuAnimationKeyMatrix4x4* keyBiggerMatrix = (keyBigger == KZ_NULL  ? KZ_NULL : kzuAnimationKeyToAnimationKeyMatrix4x4(keyBigger));
                struct KzcMatrix4x4 matrix;
                kzuAnimationKeyMatrix4x4Interpolate(time, keySmallerMatrix, keyBiggerMatrix, &matrix);
                result = kzuAnimationAnimatePropertyMatrix4x4_internal(propertyManager, targetPropertyType, targetObject, &matrix);
                kzsErrorForward(result);
                KZS_NOT_IMPLEMENTED_YET_ERROR;
            }
        }
    }
    
    /* Animate child animations. */
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(animation->data->children);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuAnimation* childAnimation = (struct KzuAnimation*)kzcDynamicArrayIteratorGetValue(it);
            result = kzuAnimationApply(childAnimation, overrides, time, relative);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzFloat kzuAnimationGetPropertyFloat(const struct KzuPropertyManager* propertyManager,
                                     const struct KzuPropertyType* propertyType, enum KzuAnimationTargetAttribute targetAttribute,
                                     const void* targetObject)
{
    kzFloat value = 0.0f;
    enum KzuPropertyDataType dataType = kzuPropertyTypeGetDataType(propertyType);

    if(dataType == KZU_PROPERTY_DATA_TYPE_MATRIX4X4)
    {
        struct KzcMatrix4x4 matrix = kzuPropertyManagerGetMatrix4x4Default(propertyManager, targetObject, propertyType);
        switch(targetAttribute)
        {
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_X: value = matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X]; break;
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Y: value = matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y]; break;
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Z: value = matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z]; break;
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_X: value = kzcMatrix4x4GetScaleX(&matrix); break;
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Y: value = kzcMatrix4x4GetScaleY(&matrix); break;
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Z: value = kzcMatrix4x4GetScaleZ(&matrix); break;
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_X:
            {
                struct KzcVector3 eulerAngles;
                /* FIXME: These do not give correct values. */
                kzcMatrix4x4GetEulerAngles(&matrix, &eulerAngles);
                value = eulerAngles.data[0];
                break;
            }
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Y:
            {
                struct KzcVector3 eulerAngles;
                /* FIXME: These do not give correct values. */
                kzcMatrix4x4GetEulerAngles(&matrix, &eulerAngles);
                value = eulerAngles.data[1];
                break;
            }
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Z:
            {
                struct KzcVector3 eulerAngles;
                /* FIXME: These do not give correct values. */
                kzcMatrix4x4GetEulerAngles(&matrix, &eulerAngles);
                value = eulerAngles.data[2];
                break;
            }
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_A:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_R:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_G:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_B:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_FLOAT:
            case KZU_ANIMATION_TARGET_NO_PROPERTY:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_X:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_Y:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_X:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Y:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Z:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_X:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Y:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Z:
            case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_W:
            case KZU_ANIMATION_TARGET_WHOLE_PROPERTY:
            default:
            {
                kzsAssertText(KZ_FALSE, "Invalid target attribute given");
            }
        }
    }
    else if(dataType == KZU_PROPERTY_DATA_TYPE_FLOAT)
    {
        value = kzuPropertyManagerGetFloatDefault(propertyManager, targetObject, propertyType);
    }
    else if(dataType == KZU_PROPERTY_DATA_TYPE_COLOR)
    {
        struct KzcColorRGBA color = kzuPropertyManagerGetColorDefault(propertyManager, targetObject, propertyType);
        if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_R)
        {
            value = color.red;
        }
        else if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_G)
        {
            value = color.green;
        }
        else if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_B)
        {
            value = color.blue;
        }
        else if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_A)
        {
            value = color.alpha;
        }
    }
    else if(dataType == KZU_PROPERTY_DATA_TYPE_VECTOR2)
    {
        struct KzcVector2 vector = kzuPropertyManagerGetVector2Default(propertyManager, targetObject, propertyType);
        if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_X)
        {
            value = vector.data[0];
        }
        else if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_Y)
        {
            value = vector.data[1];
        }
    }
    else if(dataType == KZU_PROPERTY_DATA_TYPE_VECTOR3)
    {
        struct KzcVector3 vector = kzuPropertyManagerGetVector3Default(propertyManager, targetObject, propertyType);
        if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_X)
        {
            value = vector.data[0];
        }
        else if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Y)
        {
            value = vector.data[1];
        }
        else if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Z)
        {
            value = vector.data[2];
        }
    }
    else if(dataType == KZU_PROPERTY_DATA_TYPE_VECTOR4)
    {
        struct KzcVector4 vector = kzuPropertyManagerGetVector4Default(propertyManager, targetObject, propertyType);
        if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_X)
        {
            value = vector.data[0];
        }
        else if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Y)
        {
            value = vector.data[1];
        }
        else if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Z)
        {
            value = vector.data[2];
        }
        else if(targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_W)
        {
            value = vector.data[3];
        }
    }
    else if(dataType == KZU_PROPERTY_DATA_TYPE_BOOL)
    {
        kzBool valueTrue = kzuPropertyManagerGetBoolDefault(propertyManager, targetObject, propertyType);
        value = valueTrue ? 1.0f : 0.0f;
    }
    else if(dataType == KZU_PROPERTY_DATA_TYPE_INT)
    {
        kzInt intValue = kzuPropertyManagerGetIntDefault(propertyManager, targetObject, propertyType);
        value = (kzFloat)intValue;
    }

    return value;
}

kzsError kzuAnimationAnimatePropertyFloat(const struct KzuPropertyManager* propertyManager,
                                          const struct KzuPropertyType* targetPropertyType, enum KzuAnimationTargetAttribute targetAttribute, const void* targetObject,
                                          kzBool relative, kzFloat value)
{
    kzsError result;
    kzBool found;

    switch (targetAttribute)
    {
        /* TODO: Clean-up by removing duplicate code for setting and getting matrices */
        /* Translate */
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_X:
        {
            struct KzcMatrix4x4 matrix = KZC_MATRIX4X4_IDENTITY;
            if (relative)
            {
                found = kzuPropertyManagerGetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            
            matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] = value;

            if (relative)
            {
                result = kzuPropertyManagerSetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Y:
        {            
            struct KzcMatrix4x4 matrix = KZC_MATRIX4X4_IDENTITY;
            if (relative)
            {
                found = kzuPropertyManagerGetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] = value;
            
            if (relative)
            {
                result = kzuPropertyManagerSetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Z:
        {            
            struct KzcMatrix4x4 matrix = KZC_MATRIX4X4_IDENTITY;
            if (relative)
            {
                found = kzuPropertyManagerGetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            matrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] = value;
            
            if (relative)
            {
                result = kzuPropertyManagerSetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            break;
        }
        /* Scale */
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_X:
        {            
            struct KzcMatrix4x4 matrix = KZC_MATRIX4X4_IDENTITY;
            if (relative)
            {
                found = kzuPropertyManagerGetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            kzcMatrix4x4ScaleX(&matrix, value);
            
            if (relative)
            {
                result = kzuPropertyManagerSetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Y:
        {            
            struct KzcMatrix4x4 matrix = KZC_MATRIX4X4_IDENTITY;
            if (relative)
            {
                found = kzuPropertyManagerGetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            kzcMatrix4x4ScaleY(&matrix, value);
            
            if (relative)
            {
                result = kzuPropertyManagerSetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Z:
        {
            struct KzcMatrix4x4 matrix = KZC_MATRIX4X4_IDENTITY;
            if (relative)
            {
                found = kzuPropertyManagerGetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            kzcMatrix4x4ScaleZ(&matrix, value);
            
            if (relative)
            {
                result = kzuPropertyManagerSetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            break;
        }
        /* Rotation */
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_X:
        {            
            struct KzcMatrix4x4 matrix = KZC_MATRIX4X4_IDENTITY;
            if (relative)
            {
                found = kzuPropertyManagerGetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            kzcMatrix4x4RotateX(&matrix, -value);
            
            if (relative)
            {
                result = kzuPropertyManagerSetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Y:
        {            
            struct KzcMatrix4x4 matrix = KZC_MATRIX4X4_IDENTITY;
            if (relative)
            {
                found = kzuPropertyManagerGetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            kzcMatrix4x4RotateY(&matrix,  -value);
            
            if (relative)
            {
                result = kzuPropertyManagerSetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            break;
        }
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Z:
        {            
            struct KzcMatrix4x4 matrix = KZC_MATRIX4X4_IDENTITY;
            if (relative)
            {
                found = kzuPropertyManagerGetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            kzcMatrix4x4RotateZ(&matrix, -value);
            
            if (relative)
            {
                result = kzuPropertyManagerSetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
                kzsErrorForward(result);
            }
            break;
        }
        /* Float property. */
        case KZU_ANIMATION_TARGET_WHOLE_PROPERTY:
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_FLOAT:
        {
            if (relative)
            {
                result = kzuPropertyManagerSetRelativeFloat(propertyManager, targetObject, targetPropertyType, value);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetFloat(propertyManager, targetObject, targetPropertyType, value);
                kzsErrorForward(result);
            }
            break;
        }
        /* Color */
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_R:
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_G:
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_B:
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_A:
        {
            kzUint component;
            struct KzcColorRGBA color = KZC_COLOR_BLACK;

            if (relative)
            {
                found = kzuPropertyManagerGetRelativeColor(propertyManager, targetObject, targetPropertyType, &color);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseColor(propertyManager, targetObject, targetPropertyType, &color);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            

            switch (targetAttribute)
            {
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_R: component = 0; break;
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_G: component = 1; break;
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_B: component = 2; break;
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_COLOR_A: component = 3; break;
                default:
                {
                    kzsAssert(KZ_FALSE);
                    component = 0;
                }
            }

            kzcColorRGBASetComponent(&color, component, value);

            if (relative)
            {
                result = kzuPropertyManagerSetRelativeColor(propertyManager, targetObject, targetPropertyType, color);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetColor(propertyManager, targetObject, targetPropertyType, color);
                kzsErrorForward(result);
            }
            break;
        }
        /* Vector4 */
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_X:
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Y:
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Z:
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_W:
        {
            kzUint component;
            struct KzcVector4 vector = KZC_VECTOR4_ZERO;

            if (relative)
            {
                found = kzuPropertyManagerGetRelativeVector4(propertyManager, targetObject, targetPropertyType, &vector);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseVector4(propertyManager, targetObject, targetPropertyType, &vector);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            switch (targetAttribute)
            {
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_X: component = 0; break;
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Y: component = 1; break;
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_Z: component = 2; break;
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR4_W: component = 3; break;
                default:
                {
                    kzsAssert(KZ_FALSE);
                    component = 0;
                }
            }

            vector.data[component] = value;

            if (relative)
            {
                result = kzuPropertyManagerSetRelativeVector4(propertyManager, targetObject, targetPropertyType, vector);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetVector4(propertyManager, targetObject, targetPropertyType, vector);
                kzsErrorForward(result);
            }
            break;
        }
        /* Vector3 */
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_X:
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Y:
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Z:
        {
            kzUint component;
            struct KzcVector3 vector;

            if (relative)
            {
                found = kzuPropertyManagerGetRelativeVector3(propertyManager, targetObject, targetPropertyType, &vector);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseVector3(propertyManager, targetObject, targetPropertyType, &vector);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            switch (targetAttribute)
            {
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_X: component = 0; break;
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Y: component = 1; break;
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR3_Z: component = 2; break;
                default:
                {
                    kzsAssert(KZ_FALSE);
                    component = 0;
                }
            }

            vector.data[component] = value;

            if (relative)
            {
                result = kzuPropertyManagerSetRelativeVector3(propertyManager, targetObject, targetPropertyType, vector);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetVector3(propertyManager, targetObject, targetPropertyType, vector);
                kzsErrorForward(result);
            }
            
            break;
        }
        /* Vector2 */
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_X:
        case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_Y:
        {
            kzUint component;
            struct KzcVector2 vector;

            if (relative)
            {
                found = kzuPropertyManagerGetRelativeVector2(propertyManager, targetObject, targetPropertyType, &vector);
                KZ_UNUSED_RETURN_VALUE(found);
            }
            else
            {
                found = kzuPropertyManagerGetBaseVector2(propertyManager, targetObject, targetPropertyType, &vector);
                KZ_UNUSED_RETURN_VALUE(found);
            }

            switch (targetAttribute)
            {
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_X: component = 0; break;
                case KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_VECTOR2_Y: component = 1; break;
                default:
                {
                    kzsAssert(KZ_FALSE);
                    component = 0;
                }
            }

            vector.data[component] = value;

            if (relative)
            {
                result = kzuPropertyManagerSetRelativeVector2(propertyManager, targetObject, targetPropertyType, vector);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuPropertyManagerSetVector2(propertyManager, targetObject, targetPropertyType, vector);
                kzsErrorForward(result);
            }
            
            break;
        }
        case KZU_ANIMATION_TARGET_NO_PROPERTY:
        {
            kzsAssertText(KZ_FALSE, "Unknown target property.");
            break;
        }
        default:
        {
            kzsAssertText(KZ_FALSE, "Unknown enum value.");
            break;
        }
    }

    kzsSuccess();
}

kzsError kzuAnimationAnimatePropertyMatrix4x4_internal(const struct KzuPropertyManager* propertyManager,
                                                       const struct KzuPropertyType* targetPropertyType, const void* targetObject,
                                                       const struct KzcMatrix4x4* matrix)
{
    kzsError result;
    result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, matrix);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuAnimationAnimatePropertyTexture_internal(const struct KzuPropertyManager* propertyManager,
                                                     const struct KzuPropertyType* targetPropertyType, const void* targetObject,
                                                     struct KzcTexture* texture)
{
    kzsError result;
    result = kzuPropertyManagerSetVoid(propertyManager, targetObject, targetPropertyType, texture);
    kzsErrorForward(result);

    kzsSuccess();
}

kzFloat kzuAnimationGetStartTime(const struct KzuAnimation* animation)
{
    struct KzuAnimationKey* key;
    struct KzcDynamicArrayIterator it;
    kzFloat startTime;

    kzsAssert(kzcIsValidPointer(animation));
    kzsAssert(!kzcDynamicArrayIsEmpty(animation->data->animationKeys));

    key = kzcDynamicArrayGet(animation->data->animationKeys, 0);
    startTime = kzuAnimationKeyGetTime(key);

    /* Ending time is the minimum of each animation in the hierarchy. */
    it = kzcDynamicArrayGetIterator(animation->data->children);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuAnimation* childAnimation = (struct KzuAnimation*)kzcDynamicArrayIteratorGetValue(it);
        startTime = kzsMinf(startTime, kzuAnimationGetStartTime(childAnimation));
    }

    return startTime;
}

kzFloat kzuAnimationGetEndTime(const struct KzuAnimation* animation)
{
    struct KzuAnimationKey* key;
    struct KzcDynamicArrayIterator it;
    kzFloat endTime;

    kzsAssert(kzcIsValidPointer(animation));
    kzsAssert(!kzcDynamicArrayIsEmpty(animation->data->animationKeys));

    key = kzcDynamicArrayGet(animation->data->animationKeys, kzcDynamicArrayGetSize(animation->data->animationKeys) - 1);
    endTime = kzuAnimationKeyGetTime(key);

    /* Ending time is the maximum of each animation in the hierarchy. */
    it = kzcDynamicArrayGetIterator(animation->data->children);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuAnimation* childAnimation = (struct KzuAnimation*)kzcDynamicArrayIteratorGetValue(it);
        endTime = kzsMaxf(endTime, kzuAnimationGetStartTime(childAnimation));
    }

    return endTime;
}

kzsError kzuAnimationPrepare(const struct KzuAnimation* animation, const struct KzcHashMap* overrides, kzBool relative)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    struct KzuPropertyManager* propertyManager = animation->data->propertyManager;
    const struct KzuPropertyType* targetPropertyType = animation->data->targetPropertyType;
    const void* targetObject = animation->data->targetObject;
    kzsAssert(kzcIsValidPointer(animation));

    /* Use default if no target is specified. */
    if (overrides != KZ_NULL)
    {
        struct KzuAnimationOverride original;
        struct KzuAnimationOverride* override;
        original.object = targetObject;
        original.propertyType = targetPropertyType;

        if (kzcHashMapGet(overrides, &original, (void**)&override))
        {
            targetObject = override->object;
            targetPropertyType = override->propertyType;
        }
    }

    if(kzuAnimationIsTargetTransformation_internal(animation))
    {
        if (relative)
        {
            result = kzuPropertyManagerSetRelativeMatrix4x4(propertyManager, targetObject, targetPropertyType, &KZC_MATRIX4X4_IDENTITY);
            kzsErrorForward(result);
        }
        else
        {
            struct KzcMatrix4x4 matrix;
            if (!kzuPropertyManagerGetBaseMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix))
            {
                matrix = KZC_MATRIX4X4_IDENTITY;
            }

            kzcMatrix4x4SetIdentity3x3(&matrix);
            result = kzuPropertyManagerSetMatrix4x4(propertyManager, targetObject, targetPropertyType, &matrix);
            kzsErrorForward(result);
        }
    }
    else
    {
        /* No reset for container (hierarchical) animations. */
        if(relative && targetObject != KZ_NULL)
        {
            /* Reset relative values. */
            switch(kzuPropertyTypeGetDataType(targetPropertyType))
            {
                case KZU_PROPERTY_DATA_TYPE_COLOR:
                {
                    result = kzuPropertyManagerSetRelativeColor(propertyManager, targetObject, targetPropertyType, KZC_COLOR_BLACK_TRANSPARENT);
                    kzsErrorForward(result);
                    break;
                }
                case KZU_PROPERTY_DATA_TYPE_VECTOR4:
                {
                    result = kzuPropertyManagerSetRelativeVector4(propertyManager, targetObject, targetPropertyType, KZC_VECTOR4_ZERO);
                    kzsErrorForward(result);
                    break;
                }
                case KZU_PROPERTY_DATA_TYPE_VECTOR3:
                {
                    result = kzuPropertyManagerSetRelativeVector3(propertyManager, targetObject, targetPropertyType, KZC_VECTOR3_ZERO);
                    kzsErrorForward(result);
                    break;
                }
                case KZU_PROPERTY_DATA_TYPE_VECTOR2:
                {
                    result = kzuPropertyManagerSetRelativeVector2(propertyManager, targetObject, targetPropertyType, KZC_VECTOR2_ZERO);
                    kzsErrorForward(result);
                    break;
                }
                case KZU_PROPERTY_DATA_TYPE_FLOAT:
                {
                    result = kzuPropertyManagerSetRelativeFloat(propertyManager, targetObject, targetPropertyType, 0.0f);
                    kzsErrorForward(result);
                    break;
                }
                case KZU_PROPERTY_DATA_TYPE_TEXTURE:
                case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
                case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
                case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
                case KZU_PROPERTY_DATA_TYPE_INT:
                case KZU_PROPERTY_DATA_TYPE_LIGHT:
                case KZU_PROPERTY_DATA_TYPE_STRING:
                case KZU_PROPERTY_DATA_TYPE_BOOL:
                case KZU_PROPERTY_DATA_TYPE_VOID:
                default:
                {
                    kzsAssert(KZ_FALSE);
                    break;
                }
            }
        }
    }

    it = kzcDynamicArrayGetIterator(animation->data->children);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuAnimation* childAnimation = (struct KzuAnimation*)kzcDynamicArrayIteratorGetValue(it);
        result = kzuAnimationPrepare(childAnimation, overrides, relative);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzBool kzuAnimationIsValid(const struct KzuAnimation* animation)
{
    return animation != KZ_NULL && animation->data != KZ_NULL;
}

void kzuAnimationTakeOwnership(const struct KzuAnimation* animation)
{
    kzsAssert(kzuAnimationIsValid(animation));
    animation->data->selfOwned = KZ_FALSE;
}

void kzuAnimationTransferData(struct KzuAnimation* target, struct KzuAnimation* source)
{
    kzsAssert(kzcIsValidPointer(target));
    kzsAssert(target->data == KZ_NULL);
    kzsAssert(kzuAnimationIsValid(source));

    target->data = source->data;
    source->data = KZ_NULL;
}

kzBool kzuAnimationIsTargetTransformation_internal(const struct KzuAnimation* animation)
{
    return (animation->data->targetType == KZU_ANIMATION_TARGET_FLOAT &&
            (animation->data->targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_X ||
             animation->data->targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Y ||
             animation->data->targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_TRANSLATION_Z ||
             animation->data->targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_X ||
             animation->data->targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Y ||
             animation->data->targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_SCALE_Z ||
             animation->data->targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_X ||
             animation->data->targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Y ||
             animation->data->targetAttribute == KZU_ANIMATION_TARGET_FLOAT_ATTRIBUTE_ROTATION_Z)) ||
           animation->data->targetType == KZU_ANIMATION_TARGET_MATRIX;
}

struct KzcDynamicArray* kzuAnimationGetKeyFrames(const struct KzuAnimation* animation)
{
    kzsAssert(kzcIsValidPointer(animation));
    return animation->data->animationKeys;
}

 struct KzuPropertyType* kzuAnimationGetTargetPropertyType(const struct KzuAnimation* animation)
 {
    kzsAssert(kzcIsValidPointer(animation));
    return  animation->data->targetPropertyType;
 }

 const void* kzuAnimationGetTargetObject(const struct KzuAnimation* animation)
 {
     kzsAssert(kzcIsValidPointer(animation));
     return  animation->data->targetObject;
 }
