/**
* \file
* Animation clip.
* Describes how parts of animations can be used.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_animation_clip.h"

#include <user/animation/kzu_animation.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>


struct KzuAnimationClipData
{        
    kzBool selfOwned; /**< Indicates that the object source container will be deleted when this data is deleted. */
    kzFloat animationStart; /**< Starting time for animations inside this clip. */
    kzFloat animationEnd; /**< Ending time for animations inside this clip. */
    kzBool relative;

    kzMutableString name; /**< Name for the animation clip. */

    struct KzcDynamicArray* animations; /**< Animations used in this animation clip. <KzuAnimation> */
};

struct KzuAnimationClip
{
    struct KzuAnimationClipData* data;
};


kzsError kzuAnimationClipCreate(const struct KzcMemoryManager* memoryManager, kzFloat startTime, kzFloat endTime, 
                                struct KzuAnimationClip** out_animationClip)
{
    kzsError result;
    struct KzuAnimationClip* animationClip;

    result = kzcMemoryAllocVariable(memoryManager, animationClip, "AnimationClip");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, animationClip->data, "AnimationClip Data");
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &animationClip->data->animations);
    kzsErrorForward(result);

    animationClip->data->selfOwned = KZ_TRUE;
    animationClip->data->animationStart = startTime;
    animationClip->data->animationEnd = endTime;
    animationClip->data->relative = KZ_TRUE;
    animationClip->data->name = KZ_NULL;

    *out_animationClip = animationClip;
    kzsSuccess();
}

void kzuAnimationClipSetRelative(const struct KzuAnimationClip* animationClip, kzBool relative)
{
    kzsAssert(kzcIsValidPointer(animationClip));
    animationClip->data->relative = relative;
}

kzsError kzuAnimationClipCreateFromAnimation(const struct KzcMemoryManager* memoryManager, struct KzuAnimation* animation, 
                                             struct KzuAnimationClip** out_animationClip)
{
    kzsError result;
    struct KzuAnimationClip* animationClip;
    kzFloat start;
    kzFloat end;

    start = kzuAnimationGetStartTime(animation);
    end = kzuAnimationGetEndTime(animation);

    result = kzuAnimationClipCreate(memoryManager, start, end, &animationClip);
    kzsErrorForward(result);
    result = kzuAnimationClipAddAnimation(animationClip, animation);
    kzsErrorForward(result);

    *out_animationClip = animationClip;
    kzsSuccess();
}

kzsError kzuAnimationClipDelete(struct KzuAnimationClip* animationClip)
{
    kzsError result;
    kzBool selfOwned;

    kzsAssert(kzuAnimationClipIsValid(animationClip));

    selfOwned = animationClip->data->selfOwned;

    result = kzcDynamicArrayDelete(animationClip->data->animations);
    kzsErrorForward(result);

    if(animationClip->data->name != KZ_NULL)
    {
        result = kzcStringDelete(animationClip->data->name);
        kzsErrorForward(result);
        animationClip->data->name = KZ_NULL;
    }

    result = kzcMemoryFreeVariable(animationClip->data);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(animationClip);
        kzsErrorForward(result);
    }
    else
    {
        animationClip->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuAnimationClipAddAnimation(const struct KzuAnimationClip* animationClip, struct KzuAnimation* animation)
{
    kzsError result;

    kzsAssert(kzuAnimationClipIsValid(animationClip));
    kzsAssert(kzcIsValidPointer(animation));

    result = kzcDynamicArrayAdd(animationClip->data->animations, animation);
    kzsErrorForward(result);

    kzsSuccess();
}

kzFloat kzuAnimationClipGetStartTime(const struct KzuAnimationClip* animationClip)
{
    kzsAssert(kzuAnimationClipIsValid(animationClip));
    return animationClip->data->animationStart;
}

kzFloat kzuAnimationClipGetEndTime(const struct KzuAnimationClip* animationClip)
{
    kzsAssert(kzuAnimationClipIsValid(animationClip));
    return animationClip->data->animationEnd;
}

kzFloat kzuAnimationClipGetDuration(const struct KzuAnimationClip* animationClip)
{
    kzsAssert(kzuAnimationClipIsValid(animationClip));
    return animationClip->data->animationEnd - animationClip->data->animationStart;
}

kzsError kzuAnimationClipAnimate(const struct KzuAnimationClip* animationClip, const struct KzcHashMap* overrides,
                                 kzFloat time)
{
    kzsError result;
    /* Property can be KZ_NULL. */
    kzsAssert(kzuAnimationClipIsValid(animationClip));

    /* Animate contained animations. */
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(animationClip->data->animations);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuAnimation* animation = (struct KzuAnimation*)kzcDynamicArrayIteratorGetValue(it);
            result = kzuAnimationApply(animation, overrides, animationClip->data->animationStart + time, animationClip->data->relative);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzuAnimationClipPrepare(const struct KzuAnimationClip* animationClip, const struct KzcHashMap* overrides)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzuAnimationClipIsValid(animationClip));

    it = kzcDynamicArrayGetIterator(animationClip->data->animations);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuAnimation* animation = (struct KzuAnimation*)kzcDynamicArrayIteratorGetValue(it);
        result = kzuAnimationPrepare(animation, overrides, animationClip->data->relative);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzBool kzuAnimationClipIsValid(const struct KzuAnimationClip* animationClip)
{
    return animationClip != KZ_NULL && animationClip->data != KZ_NULL;
}

void kzuAnimationClipTakeOwnership(const struct KzuAnimationClip* animation)
{
    kzsAssert(kzuAnimationClipIsValid(animation));
    animation->data->selfOwned = KZ_FALSE;
}

void kzuAnimationClipTransferData(struct KzuAnimationClip* target, struct KzuAnimationClip* source)
{
    kzsAssert(kzcIsValidPointer(target));
    kzsAssert(target->data == KZ_NULL);
    kzsAssert(kzuAnimationClipIsValid(source));

    target->data = source->data;
    source->data = KZ_NULL;
}

struct KzcDynamicArray* kzuAnimationClipGetAnimations(const struct KzuAnimationClip* animationClip)
{
    kzsAssert(kzcIsValidPointer(animationClip));
    return animationClip->data->animations;
}

kzsError kzuAnimationClipSetName(const struct KzuAnimationClip* animationClip, kzString name)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(animationClip));

    /* Delete previous name if possible. */
    if(animationClip->data->name != KZ_NULL)
    {
        result = kzcStringDelete(animationClip->data->name);
        kzsErrorForward(result);
    }

    result = kzcStringCopy(kzcMemoryGetManager(animationClip), name, &animationClip->data->name);
    kzsErrorForward(result);

    kzsSuccess();
}

kzString kzuAnimationClipGetName(const struct KzuAnimationClip* animationClip)
{
    kzsAssert(kzcIsValidPointer(animationClip));
    return animationClip->data->name;
}
