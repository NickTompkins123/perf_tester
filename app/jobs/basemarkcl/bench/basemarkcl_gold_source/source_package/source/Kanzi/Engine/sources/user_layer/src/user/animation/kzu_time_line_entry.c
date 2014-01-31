/**
* \file
* Time line entry.
* Describes how animation clips or sequences are used on their parent time line.
* Allows multiple playback modes and adjusting the playback rates.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_time_line_entry.h"

#include <user/animation/kzu_animation.h>
#include <user/animation/kzu_animation_clip.h>
#include <user/animation/kzu_time_line_sequence.h>
#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/scene_graph/kzu_object.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/wrappers/kzs_math.h>


struct KzuTimeLineEntry
{
    kzFloat durationModifier;       /**< Modifier used to control the playback speed. 1.0f normal speed, 0.5f playback twice the normal. */

    enum KzuTimeLineEntryType type; /**< Type of the time line entry. */

    union
    {
        struct KzuAnimationClip* animationClip; /**< Animation clip to animate. */
        struct KzuTimeLineSequence* timelineSequence; /**< Animation sequence to animate. */
    } target;

    struct KzuPropertyType* inputPropertyType; /**< Timeline input property. KZ_NULL to use time as input (default). */

    /* Type specific data, depending if time or property driven. */
    union
    {
        struct 
        {
            enum KzuTimeLineEntryPlaybackMode playbackMode; /**< Defines the playback mode for the content of the clip. */
            kzFloat startTime; /**< Starting time of the animation entry. */
            kzInt repeatCount; /**< How many times the animation is repeated. KZU_TIME_LINE_ENTRY_LOOP_INFINITELY means infinite repeat. */
        } time;
        struct 
        {
            struct KzuPropertyManager* propertyManager; /**< Property manager for input property type. */
            void* inputPropertyTargetObject; /**< Input property target object. */
            enum KzuAnimationTargetAttribute inputTargetAttribute; /**< Target attribute for input, used if input property is specified. */
        } property;
    } inputData;

    struct KzcHashMap* overrides; /**< Animation override mapping table. <sourceProperty, targetProperty>
                                       Source properties get replaced by target properties. */
    kzBool playFirstFrameOnce; /**< Is first frame played once. */
    kzBool deleteAfterPlayback; /**< If set to true the time line entry will be removed from sequence and deleted after it has finished playback. */
    kzBool activeLastFrame; /**< True if the animation was run on last frame. After animation stops it's goal time will be updated once. */
};


/** Internal helper for entry creation. Initializes shared members. */
static kzsError kzuTimeLineEntryCreate_internal(const struct KzcMemoryManager* memoryManager, kzFloat start,
                                                struct KzuTimeLineEntry** out_timeLineEntry);
/** Returns the unmodified duration for the entry. */
static kzFloat kzuTimeLineEntryGetUnscaledDuration_internal(const struct KzuTimeLineEntry* timeLineEntry);


/** Hash code calculation function for animation override keys. */
kzU32 kzuHashCodeFromAnimationOverride(const void* pointer)
{
    const struct KzuAnimationOverride* override = pointer;
    kzU32 objectHash;
    kzU32 typeHash;

    kzsAssert(override != KZ_NULL);

    objectHash = kzcHashCodeFromPointer(override->object);
    typeHash = kzcHashCodeFromPointer(override->propertyType);

    return objectHash ^ typeHash;
}

/** Comparison function for animation override keys. */
kzInt kzuCompareAnimationOverrides(const void* first, const void* second)
{
    kzInt result;

    const struct KzuAnimationOverride* overrideFirst = first;
    const struct KzuAnimationOverride* overrideSecond = second;

    kzsAssert(overrideFirst != KZ_NULL);
    kzsAssert(overrideSecond != KZ_NULL);

    result = kzcComparePointers(overrideFirst->object, overrideSecond->object);

    if (result == 0)
    {
        result = kzcComparePointers(overrideFirst->propertyType, overrideSecond->propertyType);
    }

    return result;
}

static kzsError kzuTimeLineEntryCreate_internal(const struct KzcMemoryManager* memoryManager, kzFloat start,
                                               struct KzuTimeLineEntry** out_timeLineEntry)
{
    kzsError result;
    struct KzuTimeLineEntry* timeLineEntry;

    /* Hash map configuration for overrides. */
    struct KzcHashMapConfiguration overrideMapConfiguration = {kzuHashCodeFromAnimationOverride, kzuCompareAnimationOverrides};

    result = kzcMemoryAllocVariable(memoryManager, timeLineEntry, "Time line entry");
    kzsErrorForward(result);

    timeLineEntry->inputData.time.startTime= start;
    timeLineEntry->inputData.time.repeatCount = 1;
    timeLineEntry->inputData.time.playbackMode = KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_NORMAL;
    timeLineEntry->durationModifier = 1.0f;
    
    timeLineEntry->deleteAfterPlayback = KZ_FALSE;
    timeLineEntry->activeLastFrame = KZ_FALSE;
    timeLineEntry->inputPropertyType = KZ_NULL;
    timeLineEntry->playFirstFrameOnce = KZ_FALSE;
    
    result = kzcHashMapCreate(memoryManager, overrideMapConfiguration, &timeLineEntry->overrides);
    kzsErrorForward(result);

    *out_timeLineEntry = timeLineEntry;
    kzsSuccess();
}


kzsError kzuTimeLineEntryCreateClip(const struct KzcMemoryManager* memoryManager, kzFloat start,
                                    struct KzuAnimationClip* animationClip, struct KzuTimeLineEntry** out_timeLineEntry)
{
    kzsError result;
    struct KzuTimeLineEntry* timeLineEntry;

    kzsAssert(kzcIsValidPointer(animationClip));

    result = kzuTimeLineEntryCreate_internal(memoryManager, start, &timeLineEntry);
    kzsErrorForward(result);

    timeLineEntry->type = KZU_TIME_LINE_ENTRY_ANIMATION_CLIP;
    timeLineEntry->target.animationClip = animationClip;

    *out_timeLineEntry = timeLineEntry;
    kzsSuccess();
}

kzsError kzuTimeLineEntryCreateSequence(const struct KzcMemoryManager* memoryManager, kzFloat start,
                                        struct KzuTimeLineSequence* sequence, struct KzuTimeLineEntry** out_timeLineEntry)
{
    kzsError result;
    struct KzuTimeLineEntry* timeLineEntry;

    kzsAssert(kzcIsValidPointer(sequence));

    result = kzuTimeLineEntryCreate_internal(memoryManager, start, &timeLineEntry);
    kzsErrorForward(result);

    timeLineEntry->type = KZU_TIME_LINE_ENTRY_TIME_LINE_SEQUENCE;
    timeLineEntry->target.timelineSequence = sequence;

    *out_timeLineEntry = timeLineEntry;
    kzsSuccess();
}

kzsError kzuTimeLineEntryDelete(struct KzuTimeLineEntry* timeLineEntry)
{
    kzsError result;
    struct KzcHashMapIterator it;

    kzsAssert(kzcIsValidPointer(timeLineEntry));

    it = kzcHashMapGetIterator(timeLineEntry->overrides);
    while (kzcHashMapIterate(it))
    {
        struct KzuAnimationOverride* key = (struct KzuAnimationOverride*)kzcHashMapIteratorGetKey(it);
        struct KzuAnimationOverride* value = (struct KzuAnimationOverride*)kzcHashMapIteratorGetValue(it);

        result = kzcMemoryFreeVariable(key);
        kzsErrorForward(result);

        result = kzcMemoryFreeVariable(value);
        kzsErrorForward(result);
    }

    result = kzcHashMapDelete(timeLineEntry->overrides);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(timeLineEntry);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuTimeLineEntrySetDeleteAfterPlayback(struct KzuTimeLineEntry* timeLineEntry, kzBool deleteEntry)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    timeLineEntry->deleteAfterPlayback = deleteEntry;
}

kzBool kzuTimeLineEntryGetDeleteAfterPlayback(const struct KzuTimeLineEntry* timeLineEntry)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    return timeLineEntry->deleteAfterPlayback;
}

kzBool kzuTimeLineEntryGetActiveLastFrame(const struct KzuTimeLineEntry* timeLineEntry)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    return timeLineEntry->activeLastFrame;
}

void kzuTimeLineEntrySetActiveLastFrame(struct KzuTimeLineEntry* timeLineEntry, kzBool active)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    timeLineEntry->activeLastFrame = active;
}

kzsError kzuTimeLineEntryUpdate(struct KzuTimeLineEntry* timeLineEntry, struct KzcHashMap* overrides, kzFloat time)
{
    kzsError result;
    
    timeLineEntry->activeLastFrame = KZ_FALSE;

    if(kzuTimeLineEntryIsActiveAtTime(timeLineEntry, time))
    { 
        timeLineEntry->activeLastFrame = KZ_TRUE;

        /* Fill overrides table. */
        {
            struct KzcHashMapIterator it = kzcHashMapGetIterator(timeLineEntry->overrides);
            while(kzcHashMapIterate(it))
            {
                struct KzuAnimationOverride* source = (struct KzuAnimationOverride*)kzcHashMapIteratorGetKey(it);
                struct KzuAnimationOverride* target = (struct KzuAnimationOverride*)kzcHashMapIteratorGetValue(it);
                if(!kzcHashMapContains(overrides, source))
                {
                    result = kzcHashMapPut(overrides, source, target);
                    kzsErrorForward(result);
                }
            }
        }

        if(timeLineEntry->type == KZU_TIME_LINE_ENTRY_ANIMATION_CLIP)
        {
            result = kzuAnimationClipPrepare(timeLineEntry->target.animationClip, overrides);
            kzsErrorForward(result);
        }

        /* Calculate animation clip time. */
        {
            kzFloat targetTime;
            kzFloat targetContentDuration;
            kzFloat entryScaledDuration = kzuTimeLineEntryGetDuration(timeLineEntry);
            kzFloat entryPlayTime = time; /* How long has entry been played. */
            kzInt repeatCount = 1;
            
            if(timeLineEntry->inputPropertyType == KZ_NULL)
            {
                repeatCount = kzuTimeLineEntryGetRepeatCount(timeLineEntry);
                entryPlayTime -= timeLineEntry->inputData.time.startTime;
            }
            
            /* Get remainder of divide time/entryDuration if this is not the last repeat. */
            if(repeatCount == KZU_TIME_LINE_ENTRY_LOOP_INFINITELY || repeatCount > entryPlayTime / entryScaledDuration)
            {
                if(kzsFloatIsEqual(entryScaledDuration, 0.0f))
                {
                    entryPlayTime = 0.0f;
                }
                else
                {
                    entryPlayTime = kzsFmod(entryPlayTime, entryScaledDuration);
                }
            }
            
            /* Map time to targets time. */
            switch(timeLineEntry->type)
            {
                case KZU_TIME_LINE_ENTRY_ANIMATION_CLIP:
                {
                    targetContentDuration = kzuAnimationClipGetDuration(timeLineEntry->target.animationClip);
                    break;
                }
                case KZU_TIME_LINE_ENTRY_TIME_LINE_SEQUENCE:
                {
                    targetContentDuration = kzuTimeLineSequenceGetDuration(timeLineEntry->target.timelineSequence);
                    break;
                }
                default:
                {
                    kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Enum out of range.");
                }
            }

            if(timeLineEntry->inputPropertyType == KZ_NULL)
            {
                switch(timeLineEntry->inputData.time.playbackMode)
                {
                    case KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_PINGPONG:
                    {
                        kzFloat halfDuration = entryScaledDuration * 0.5f;
                        entryPlayTime = (halfDuration - kzsFabsf(entryPlayTime - halfDuration)) * 2.0f;
                        break;
                    }
                    case KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_REVERSE:
                    {
                        entryPlayTime = entryScaledDuration - entryPlayTime;
                        break;
                    }
                    case KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_NORMAL:
                    default:
                    {
                        /* Nothing to do here. */
                        break;
                    } 
                }
            }



            /* Map time to targets time. */
            {
                if(kzsFloatIsEqual(entryScaledDuration, 0.0f))
                {
                    targetTime = 0.0f;
                }
                else
                {
                    targetTime = entryPlayTime / entryScaledDuration * targetContentDuration;
                }
                if(timeLineEntry->inputPropertyType != KZ_NULL)
                {
                    targetTime = kzuAnimationGetPropertyFloat(
                        timeLineEntry->inputData.property.propertyManager,
                        timeLineEntry->inputPropertyType,
                        timeLineEntry->inputData.property.inputTargetAttribute,
                        timeLineEntry->inputData.property.inputPropertyTargetObject) * kzsMaxf(timeLineEntry->durationModifier, KZ_FLOAT_SMALLEST_POSITIVE);
                }

            }

            switch(timeLineEntry->type)
            {
                case KZU_TIME_LINE_ENTRY_ANIMATION_CLIP:
                {
                    result = kzuAnimationClipAnimate(timeLineEntry->target.animationClip, overrides, targetTime);
                    kzsErrorForward(result);
                    break;
                }
                case KZU_TIME_LINE_ENTRY_TIME_LINE_SEQUENCE:
                {
                    result = kzuTimeLineSequenceUpdate(timeLineEntry->target.timelineSequence, overrides, targetTime);
                    kzsErrorForward(result);
                    break;
                }
                default:
                {
                    kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Enum out of range.");
                }
            }
        }

        /* Remove added items from overrides table, so they won't affect animations not contained in the current hierarchy. */
        {
            struct KzcHashMapIterator it = kzcHashMapGetIterator(timeLineEntry->overrides);
            while(kzcHashMapIterate(it))
            {
                struct KzuAnimationOverride* source = (struct KzuAnimationOverride*)kzcHashMapIteratorGetKey(it);
                if(kzcHashMapContains(overrides, source))
                {
                    result = kzcHashMapRemove(overrides, source);
                    kzsErrorForward(result);
                }
            }
        }
    }
    kzsSuccess();
}

void kzuTimeLineEntrySetPlaybackMode(struct KzuTimeLineEntry* timeLineEntry, enum KzuTimeLineEntryPlaybackMode type)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    if(timeLineEntry->inputPropertyType == KZ_NULL)
    {
        timeLineEntry->inputData.time.playbackMode = type;
    }
}

void kzuTimeLineEntrySetInputType(struct KzuTimeLineEntry* timeLineEntry, struct KzuPropertyManager* propertyManager,
                                  struct KzuPropertyType* inputPropertyType, void* inputObject,
                                      enum KzuAnimationTargetAttribute inputTargetAttribute)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    timeLineEntry->inputPropertyType = inputPropertyType;
    if(inputPropertyType != KZ_NULL)
    {
        timeLineEntry->inputData.property.inputPropertyTargetObject = inputObject;
        timeLineEntry->inputData.property.inputTargetAttribute = inputTargetAttribute;
        timeLineEntry->inputData.property.propertyManager = propertyManager;
    }
}

void kzuTimeLineEntrySetRepeatCount(struct KzuTimeLineEntry* timeLineEntry, kzInt repeatCount)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    kzsAssert(repeatCount >= KZU_TIME_LINE_ENTRY_LOOP_INFINITELY);
    
    if(timeLineEntry->inputPropertyType == KZ_NULL)
    {
        timeLineEntry->inputData.time.repeatCount = repeatCount;

        if(timeLineEntry->inputData.time.repeatCount == 0)
        {
            timeLineEntry->inputData.time.repeatCount = 1;
        }
    }
}

kzInt kzuTimeLineEntryGetRepeatCount(const struct KzuTimeLineEntry* timeLineEntry)
{
    kzInt repeatCount = 1;
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    if(timeLineEntry->inputPropertyType == KZ_NULL)
    {
        repeatCount = timeLineEntry->inputData.time.repeatCount;
    }
    return repeatCount;
}


kzBool kzuTimeLineEntryIsPlayFirstFrameOnce(const struct KzuTimeLineEntry* timeLineEntry)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    return timeLineEntry->playFirstFrameOnce;
}

void kzuTimeLineEntrySetPlayFirstFrameOnce(struct KzuTimeLineEntry* timeLineEntry, kzBool active)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    timeLineEntry->playFirstFrameOnce = active;
}

struct KzuAnimationClip* kzuTimeLineEntryGetAnimationClip(const struct KzuTimeLineEntry* entry)
{
    struct KzuAnimationClip* clip = KZ_NULL;

    if(entry->type == KZU_TIME_LINE_ENTRY_ANIMATION_CLIP)
    {
        clip = entry->target.animationClip;
    }
    return clip;
}

void kzuTimeLineEntrySetStartingTime(struct KzuTimeLineEntry* timeLineEntry, kzFloat time)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    if(timeLineEntry->inputPropertyType == KZ_NULL)
    {
        timeLineEntry->inputData.time.startTime = time;
    }
}

/* TODO: Consider adding function kzuTimeLineEntrySetSpeed(struct KzuTimeLineEntry* timeLineEntry, kzFloat speedModifier) */

void kzuTimeLineEntrySetDurationScale(struct KzuTimeLineEntry* timeLineEntry, kzFloat durationModifier)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    timeLineEntry->durationModifier = durationModifier;
}

kzFloat kzuTimeLineEntryGetDuration(const struct KzuTimeLineEntry* timeLineEntry)
{
    kzFloat duration;
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    duration = kzuTimeLineEntryGetUnscaledDuration_internal(timeLineEntry);
    duration *= timeLineEntry->durationModifier;
    return duration;
}

kzFloat kzuTimeLineEntryGetStartingTime(const struct KzuTimeLineEntry* timeLineEntry)
{
    kzFloat time = 0.0f;
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    if(timeLineEntry->inputPropertyType == KZ_NULL)
    {
        time = timeLineEntry->inputData.time.startTime;
    }
    return time;
}

kzFloat kzuTimeLineEntryGetEndingTime(const struct KzuTimeLineEntry* timeLineEntry)
{
    kzFloat endTime;
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    if(timeLineEntry->inputPropertyType != KZ_NULL)
    {
        endTime = KZ_FLOAT_MAXIMUM;
    }
    else
    {
        if(timeLineEntry->inputData.time.repeatCount == KZU_TIME_LINE_ENTRY_LOOP_INFINITELY)
        {
            endTime = KZ_FLOAT_MAXIMUM;
        }
        else
        {
            endTime = timeLineEntry->inputData.time.startTime + kzuTimeLineEntryGetDuration(timeLineEntry) 
                * timeLineEntry->inputData.time.repeatCount;
        }
    }

    return endTime;
}

kzBool kzuTimeLineEntryIsActiveAtTime(const struct KzuTimeLineEntry* timeLineEntry, kzFloat time)
{
    kzBool active = KZ_FALSE;
    
    kzsAssert(kzcIsValidPointer(timeLineEntry));

    if(timeLineEntry->inputPropertyType == KZ_NULL)
    {
        kzFloat endingTime = kzuTimeLineEntryGetEndingTime(timeLineEntry);

        if(time >= timeLineEntry->inputData.time.startTime && time <= endingTime)
        {
            active = KZ_TRUE;
        }
    }
    else
    {
        active = KZ_TRUE;
    }

    return active;
}

static kzFloat kzuTimeLineEntryGetUnscaledDuration_internal(const struct KzuTimeLineEntry* timeLineEntry)
{
    kzFloat duration;
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    switch(timeLineEntry->type)
    {
        case KZU_TIME_LINE_ENTRY_ANIMATION_CLIP:
        {
            duration = kzuAnimationClipGetDuration(timeLineEntry->target.animationClip);
            break;
        }
        case KZU_TIME_LINE_ENTRY_TIME_LINE_SEQUENCE:
        {
            duration = kzuTimeLineSequenceGetDuration(timeLineEntry->target.timelineSequence);
            break;
        }
        default:
        {
            /* Possible error not handled to keep interfaces simple. */
            kzsAssertText(KZ_FALSE, "Enum out of range");
            duration = 1.0f;
        }
    }
    return duration;
}

kzsError kzuTimeLineEntryAddOverride(const struct KzuTimeLineEntry* timeLineEntry, struct KzuAnimationOverride originalObject, struct KzuAnimationOverride overrideObject)
{
    kzsError result;
    struct KzuAnimationOverride* originalEntry;
    struct KzuAnimationOverride* overrideEntry;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcIsValidPointer(timeLineEntry));
    memoryManager = kzcMemoryGetManager(timeLineEntry->overrides);

    result = kzcMemoryAllocVariable(memoryManager, originalEntry, "Animation override original");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, overrideEntry, "Animation override");
    kzsErrorForward(result);

    *originalEntry = originalObject;
    *overrideEntry = overrideObject;

    result = kzcHashMapPut(timeLineEntry->overrides, originalEntry, overrideEntry);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuTimeLineEntryRemoveOverride(const struct KzuTimeLineEntry* timeLineEntry, struct KzuAnimationOverride originalObject)
{
    kzsError result;
    struct KzuAnimationOverride* storedKey;
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    
    storedKey = (struct KzuAnimationOverride*)kzcHashMapGetStoredKey(timeLineEntry->overrides, &originalObject);
    if (storedKey != KZ_NULL)
    {
        struct KzuAnimationOverride* storedValue;
        kzBool found = kzcHashMapGet(timeLineEntry->overrides, &originalObject, (void**)&storedValue);

        KZ_UNUSED_RETURN_VALUE(found);

        result = kzcHashMapRemove(timeLineEntry->overrides, &originalObject);
        kzsErrorForward(result);

        result = kzcMemoryFreeVariable(storedKey);
        kzsErrorForward(result);

        result = kzcMemoryFreeVariable(storedValue);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzBool kzuTimeLineEntryOverrideExists(const struct KzuTimeLineEntry* timeLineEntry, struct KzuAnimationOverride originalObject)
{
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    return kzcHashMapContains(timeLineEntry->overrides, &originalObject);
}

static kzsError kzuTimeLineEntryOverrideProperty_internal(const struct KzuTimeLineEntry* timeLineEntry, const struct KzuAnimation* animation, struct KzuObjectNode* overrideObjectNode)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    if(kzuAnimationGetTargetPropertyType(animation) != KZ_NULL)
    {
        struct KzuAnimationOverride originalObject;
        struct KzuAnimationOverride overrideObject;

        originalObject.object = kzuAnimationGetTargetObject(animation);
        originalObject.propertyType = kzuAnimationGetTargetPropertyType(animation);

        overrideObject.object = overrideObjectNode;
        overrideObject.propertyType = kzuAnimationGetTargetPropertyType(animation);

        if(overrideObject.propertyType != KZ_NULL)
        {
            result = kzuTimeLineEntryAddOverride(timeLineEntry, originalObject, overrideObject);
            kzsErrorForward(result);
        }
    }
    it = kzuAnimationGetChildIterator(animation);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzuAnimation* childAnimation = (struct KzuAnimation*)kzcDynamicArrayIteratorGetValue(it);
        result = kzuTimeLineEntryOverrideProperty_internal(timeLineEntry, childAnimation, overrideObjectNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuTimeLineEntryOverrideProperty(const struct KzuTimeLineEntry* timeLineEntry, struct KzuObjectNode* overrideObjectNode)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(timeLineEntry));
    if(timeLineEntry->type == KZU_TIME_LINE_ENTRY_ANIMATION_CLIP)
    {
        struct KzcDynamicArrayIterator animationIterator = kzcDynamicArrayGetIterator(kzuAnimationClipGetAnimations(timeLineEntry->target.animationClip));
        while(kzcDynamicArrayIterate(animationIterator))
        {
            struct KzuAnimation* animation = (struct KzuAnimation*)kzcDynamicArrayIteratorGetValue(animationIterator); 
            result = kzuTimeLineEntryOverrideProperty_internal(timeLineEntry, animation, overrideObjectNode);
            kzsErrorForward(result);
        }
    }
    kzsSuccess();
}

