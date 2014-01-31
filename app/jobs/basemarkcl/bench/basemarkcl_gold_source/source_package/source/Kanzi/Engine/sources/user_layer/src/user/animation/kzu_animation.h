/**
* \file
* Animation.
* Contains animation key frames and target for animation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_ANIMATION_H
#define KZU_ANIMATION_H


#include "kzu_animation_base.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzuProperty;
struct KzuAnimationKey;
struct KzuPropertyManager;
struct KzuPropertyType;
struct KzcHashMap;


/**
 * \struct KzuAnimation
 * Animation structure.
 */
struct KzuAnimation;

/**
* \struct KzuAnimationData
* Animation data.
*/
struct KzuAnimationData;


/** Allocates memory for animation structure for animating given float property. */
kzsError kzuAnimationCreateFloatTarget(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                       struct KzuPropertyType* targetPropertyType, const void* targetObject,
                                       enum KzuAnimationTargetAttribute targetProperty, struct KzuAnimation** out_animation);

/** Allocates memory for animation structure for animating given matrix property. */
kzsError kzuAnimationCreateMatrix(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                  struct KzuPropertyType* targetPropertyType, const void* targetObject,
                                  struct KzuAnimation** out_animation);

/** Allocates memory for animation structure for animating given texture property. */
kzsError kzuAnimationCreateTexture(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                   struct KzuPropertyType* targetPropertyType, const void* targetObject,
                                   struct KzuAnimation** out_animation);


/** Deallocates memory reserved for animation structure. */
kzsError kzuAnimationDelete(struct KzuAnimation* animation);

/** Add child animation to given animation. */
kzsError kzuAnimationAddChild(const struct KzuAnimation* parent, struct KzuAnimation* child);

/** Removes child from animation. */
kzsError kzuAnimationRemoveChild(const struct KzuAnimation* parent, const struct KzuAnimation* child);

/** Gets iterator for animations children. */
struct KzcDynamicArrayIterator kzuAnimationGetChildIterator(const struct KzuAnimation* parent);


/** Adds new animation key to animation. Takes ownership of animationKey. */
kzsError kzuAnimationAddKeyFrame(const struct KzuAnimation* animation, struct KzuAnimationKey* animationKey);

/** Removes animation key from animation. */
kzsError kzuAnimationRemoveKeyFrame(const struct KzuAnimation* animation, const struct KzuAnimationKey* animationKey);

/** Gets iterator for animations key frames. */
struct KzcDynamicArrayIterator kzuAnimationGetKeyFrameIterator(const struct KzuAnimation* parent);

/** 
* Gets nearest smaller and bigger available animation keys for given animation.
* The nearest smaller key is returned in out_animationKey1 and nearest higher key is returned in out_animationKey2. 
* If there are no keys at some direction the corresponding out_animationKey is set to KZ_NULL.
*/
kzsError kzuAnimationGetKeysAtTime(const struct KzuAnimation* animation, kzFloat time, 
                                   struct KzuAnimationKey** out_animationKey1, struct KzuAnimationKey** out_animationKey2);

/** 
* Applies given animation and its children at given time for the target specified by parameter property.
* \param overrides Override hashmap. If no source property is found or overrides is KZ_NULL default target is used.
* \param time Time of animation. If out of range [0, (endTime - startTime)] the first or last key value is used. 
* If animation targets scale or rotation components of matrix, the transformations from the target matrix is set to identity.
*/
kzsError kzuAnimationApply(const struct KzuAnimation* animation, const struct KzcHashMap* overrides, kzFloat time, kzBool relative);

/** Gets the starting time of the animation and its children. */
kzFloat kzuAnimationGetStartTime(const struct KzuAnimation* animation);

/** Gets the ending time of the animation and its children. */
kzFloat kzuAnimationGetEndTime(const struct KzuAnimation* animation);

/** Gets float value from property, using property type and its target attribute. */
kzFloat kzuAnimationGetPropertyFloat(const struct KzuPropertyManager* propertyManager,
                                     const struct KzuPropertyType* propertyType, enum KzuAnimationTargetAttribute targetAttribute,
                                     const void* targetObject);

/** Prepares animations target properties for animation update. Resets states and values.
* \param overrides Override hashmap. If no source property is found or overrides is KZ_NULL default target is used.
*/
kzsError kzuAnimationPrepare(const struct KzuAnimation* animation, const struct KzcHashMap* overrides, kzBool relative);

/** Checks if animation is valid. */
kzBool kzuAnimationIsValid(const struct KzuAnimation* animation);
/** Sets ownership of the memory. Used in project loader. */
void kzuAnimationTakeOwnership(const struct KzuAnimation* animation);
/** Moves the data from source to the target. */
void kzuAnimationTransferData(struct KzuAnimation* target, struct KzuAnimation* source);

/** Gets animation key frames as dynamic array. */
struct KzcDynamicArray* kzuAnimationGetKeyFrames(const struct KzuAnimation* animation);

/** Sets given animation property and property attribute to given float value. */
kzsError kzuAnimationAnimatePropertyFloat(const struct KzuPropertyManager* propertyManager,
                                          const struct KzuPropertyType* targetPropertyType, enum KzuAnimationTargetAttribute targetAttribute, const void* targetObject,
                                          kzBool relative, kzFloat value);

/** Gets animation target property type to animate. */
struct KzuPropertyType* kzuAnimationGetTargetPropertyType(const struct KzuAnimation* animation);

/** Gets animation target property object to animate. */
const void* kzuAnimationGetTargetObject(const struct KzuAnimation* animation);

#endif
