/**
* \file
* Animation clip.
* Describes how parts of animations can be used.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_ANIMATION_CLIP_H
#define KZU_ANIMATION_CLIP_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzuProperty;
struct KzuAnimation;
struct KzcMemoryManager;
struct KzcDynamicArray;
struct KzcHashMap;


/**
 * \struct KzuAnimationClip
 * Animation clip structure.
 * Animation clip allows users to run sections of animations from requested time.
 * The time in animation clip is relative to animation time line.
 */
struct KzuAnimationClip;


/** Creates new animation clip. */
kzsError kzuAnimationClipCreate(const struct KzcMemoryManager* memoryManager, kzFloat startTime, kzFloat endTime, 
                                struct KzuAnimationClip** out_animationClip);
/** Creates new animation clip which contains given animation. */
kzsError kzuAnimationClipCreateFromAnimation(const struct KzcMemoryManager* memoryManager, struct KzuAnimation* animation, 
                                             struct KzuAnimationClip** out_animationClip);

/** Deallocates memory reserved for animation clip. */
kzsError kzuAnimationClipDelete(struct KzuAnimationClip* animationClip);

/** Sets animation clip relative to original value. */
void kzuAnimationClipSetRelative(const struct KzuAnimationClip* animationClip, kzBool relative);

/** Adds given animation as target for this animation clip. */
kzsError kzuAnimationClipAddAnimation(const struct KzuAnimationClip* animationClip, struct KzuAnimation* animation);

/** Gets starting time of the animation sequence. Returned time is relative to animation time line. */
kzFloat kzuAnimationClipGetStartTime(const struct KzuAnimationClip* animationClip);

/** Gets ending time of the animation sequence. Returned time is relative to animation time line. */
kzFloat kzuAnimationClipGetEndTime(const struct KzuAnimationClip* animationClip);

/** Gets duration of the animation sequence stored in animation clip. */
kzFloat kzuAnimationClipGetDuration(const struct KzuAnimationClip* animationClip);

/**
* Animates given animations clip targeting specified property.
* \param overrides Override hashmap. If no source property is found or overrides is KZ_NULL default target is used.
* \param time Time of animation [0 - clipDuration]
*/
kzsError kzuAnimationClipAnimate(const struct KzuAnimationClip* animationClip, const struct KzcHashMap* overrides, 
                                 kzFloat time);

/** Prepares animation clip target properties for animation. Resets states and values. */
kzsError kzuAnimationClipPrepare(const struct KzuAnimationClip* animationClip, const struct KzcHashMap* overrides);

/** Checks if animation clip is valid. */
kzBool kzuAnimationClipIsValid(const struct KzuAnimationClip* animation);
/** Sets ownership of the memory. Used in project loader. */
void kzuAnimationClipTakeOwnership(const struct KzuAnimationClip* animation);
/** Moves the data from source to the target. */
void kzuAnimationClipTransferData(struct KzuAnimationClip* target, struct KzuAnimationClip* source);

/** Gets animations from animation clip. */
struct KzcDynamicArray* kzuAnimationClipGetAnimations(const struct KzuAnimationClip* animationClip);


/** Sets name for the animation clip. */
kzsError kzuAnimationClipSetName(const struct KzuAnimationClip* animationClip, kzString name);
/** Gets name from the animation clip. */
kzString kzuAnimationClipGetName(const struct KzuAnimationClip* animationClip);


#endif
