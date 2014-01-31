/**
* \file
* Time line entry.
* 
* Describes how animation clips or sequences are used on their parent time line.
* Allows multiple playback modes and adjusting the playback rates.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TIME_LINE_ENTRY_H
#define KZU_TIME_LINE_ENTRY_H

#include "kzu_animation_base.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Sets time line entry to loop repeatedly. */
#define KZU_TIME_LINE_ENTRY_LOOP_INFINITELY -1


/* Forward declarations */
struct KzuAnimationKey;
struct KzuAnimationClip;
struct KzuTimeLineSequence;
struct KzuPropertyType;
struct KzuPropertyManager;
struct KzcMemoryManager;
struct KzcHashMap;
struct KzuObjectNode;


/** Playback type of the contained animation clip. */
enum KzuTimeLineEntryPlaybackMode
{
    KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_NORMAL,  /**< Plays animation normally. */
    KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_REVERSE, /**< Plays animation in reverse. */
    KZU_TIME_LINE_ENTRY_PLAYBACK_MODE_PINGPONG /**< Plays animation first normally, then in reverse. */
};

/** Time line entry type. Defines the content of the entry. */
enum KzuTimeLineEntryType
{
    KZU_TIME_LINE_ENTRY_ANIMATION_CLIP,     /**< Animation clip to animate. */
    KZU_TIME_LINE_ENTRY_TIME_LINE_SEQUENCE  /**< Time line sequence to animate. */
};


/** Animation override descriptor. */
struct KzuAnimationOverride
{
    const void* object;                     /**< Target object. */
    const struct KzuPropertyType* propertyType;   /**< Target property type. */
};


/**
 * \struct KzuTimeLineEntry
 * Time line entry.
 * The starting and ending times in time line entry are relative to parent sequence's time line.
 */
struct KzuTimeLineEntry;


/** 
* Creates new time line entry, which will play the given animation clip at specified time.
* Repeat count will be set to 1.
* Playback mode will be set to normal.
* End time of entry will be calculated from the given animation clip.
* \param start Starting time of the animation on animation players time line.
* \param animationClip Animation clip to play.
*/
kzsError kzuTimeLineEntryCreateClip(const struct KzcMemoryManager* memoryManager, kzFloat start,
                                    struct KzuAnimationClip* animationClip, struct KzuTimeLineEntry** out_timeLineEntry);
/** 
* Creates new time line entry, which will play the given animation sequence at specified time.
* Repeat count will be set to 1.
* Playback mode will be set to normal.
* \param start Starting time of the animation on animation players time line.
* \param timelineSequence Animation sequence to play.
*/
kzsError kzuTimeLineEntryCreateSequence(const struct KzcMemoryManager* memoryManager, kzFloat start,
                                        struct KzuTimeLineSequence* timelineSequence, struct KzuTimeLineEntry** out_timeLineEntry);
/** Deallocates memory required for time line entry. */
kzsError kzuTimeLineEntryDelete(struct KzuTimeLineEntry* timeLineEntry);


/** Sets the time line entry to be deleted after its playback has finished. False by default. */
void kzuTimeLineEntrySetDeleteAfterPlayback(struct KzuTimeLineEntry* timeLineEntry, kzBool delete_entry);
/** Returns true if time line entry is set to be deleted after its playback has finished. */
kzBool kzuTimeLineEntryGetDeleteAfterPlayback(const struct KzuTimeLineEntry* timeLineEntry);
/** Returns true if time line entry was active on frame before this frame. */
kzBool kzuTimeLineEntryGetActiveLastFrame(const struct KzuTimeLineEntry* timeLineEntry);
/** Sets the status of line entrys active on frame before this frame. */
void kzuTimeLineEntrySetActiveLastFrame(struct KzuTimeLineEntry* timeLineEntry, kzBool active);
/** Returns true if this entry is only played once (first frame). */
kzBool kzuTimeLineEntryIsPlayFirstFrameOnce(const struct KzuTimeLineEntry* timeLineEntry);
/** Sets the status of entry to play first frame once and then delete the entry. */
void kzuTimeLineEntrySetPlayFirstFrameOnce(struct KzuTimeLineEntry* timeLineEntry, kzBool active);


/** Gets animation clip from timeline entry. KZ_NULL if no clip or the timeline entry was not of type ANIMATION_CLIP. */
struct KzuAnimationClip* kzuTimeLineEntryGetAnimationClip(const struct KzuTimeLineEntry* entry);


/** Updates a time line entry. */
kzsError kzuTimeLineEntryUpdate(struct KzuTimeLineEntry* timeLineEntry, struct KzcHashMap* overrides, kzFloat time);


/** Sets playback mode. Default is normal. */
void kzuTimeLineEntrySetPlaybackMode(struct KzuTimeLineEntry* timeLineEntry, enum KzuTimeLineEntryPlaybackMode type);

/** Sets input type for time line entry. KZ_NULL to use time as input (default). */
void kzuTimeLineEntrySetInputType(struct KzuTimeLineEntry* timeLineEntry, struct KzuPropertyManager* propertyManager,
                                    struct KzuPropertyType* inputPropertyType, void* inputObject,
                                    enum KzuAnimationTargetAttribute inputTargetAttribute);

/** Sets animation clip looping times. -1 is for infinite repeat. */
void kzuTimeLineEntrySetRepeatCount(struct KzuTimeLineEntry* timeLineEntry, kzInt repeatCount);
/** Gets animation clip looping times. */
kzInt kzuTimeLineEntryGetRepeatCount(const struct KzuTimeLineEntry* timeLineEntry);

/** Sets new starting time for time line entry and moves the ending time accordingly. The animation duration will not be modified. 
If the time line entry is attached to animation player animation player entries must be sorted again (kzuAnimationPlayerSortAnimations). */
void kzuTimeLineEntrySetStartingTime(struct KzuTimeLineEntry* timeLineEntry, kzFloat time);
/** Gets start time of time line entry */
kzFloat kzuTimeLineEntryGetStartingTime(const struct KzuTimeLineEntry* timeLineEntry);


/** Sets duration modifier. Setting this to 2 will result in animation playing half of the normal speed. */
void kzuTimeLineEntrySetDurationScale(struct KzuTimeLineEntry* timeLineEntry, kzFloat durationModifier);
/** Gets duration of one cycle of the time line entry. */
kzFloat kzuTimeLineEntryGetDuration(const struct KzuTimeLineEntry* timeLineEntry);
/** Gets end time of time line entry. This operation is not simple getter it needs to do some calculations. Avoid using very heavily. */
kzFloat kzuTimeLineEntryGetEndingTime(const struct KzuTimeLineEntry* timeLineEntry);
/** Returns boolean value telling if animation is active at given time. */
kzBool kzuTimeLineEntryIsActiveAtTime(const struct KzuTimeLineEntry* timeLineEntry, kzFloat time);


/** Adds new property override to mappings table. If previous one exists it is replaced. */
kzsError kzuTimeLineEntryAddOverride(const struct KzuTimeLineEntry* timeLineEntry, struct KzuAnimationOverride originalObject, struct KzuAnimationOverride overrideObject);
/** Removes property override from given entry which matches given original property. */
kzsError kzuTimeLineEntryRemoveOverride(const struct KzuTimeLineEntry* timeLineEntry, struct KzuAnimationOverride originalObject);
/** Checks if override exists for given property. */
kzBool kzuTimeLineEntryOverrideExists(const struct KzuTimeLineEntry* timeLineEntry, struct KzuAnimationOverride originalObject);

/** Generate a hash key from animation override. */
kzU32 kzuHashCodeFromAnimationOverride(const void* pointer);
/** Compare animation overrides. */
kzInt kzuCompareAnimationOverrides(const void* first, const void* second);

/** Overrides target property of the animation clips in the time line entry. */
kzsError kzuTimeLineEntryOverrideProperty(const struct KzuTimeLineEntry* timeLineEntry, struct KzuObjectNode* overrideObjectNode);


#endif
