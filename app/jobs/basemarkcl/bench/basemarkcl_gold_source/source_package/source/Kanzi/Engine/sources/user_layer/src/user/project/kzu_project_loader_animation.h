/**
* \file
* Project loader for animation files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_ANIMATION_H
#define KZU_PROJECT_LOADER_ANIMATION_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include <user/animation/kzu_animation.h>


/** Animation target attribute type. */
enum KzuBinaryAnimationTargetAttribute
{
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_TRANSLATION_X,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_TRANSLATION_Y,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_TRANSLATION_Z,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_SCALE_X,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_SCALE_Y,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_SCALE_Z,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_ROTATION_Z,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_ROTATION_Y,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_ROTATION_X,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_WHOLE_PROPERTY,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_R,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_G,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_B,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_COLOR_A,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_X,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_Y,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_Z,
    KZU_BINARY_ANIMATION_TARGET_ATTRIBUTE_VECTOR_W
};


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuPropertyType;
struct KzuAnimation;
struct KzuAnimationClip;
struct KzuTimeLineSequence;


/** Loads animation data structure from binary. */
kzsException kzuProjectLoaderLoadAnimation(struct KzuProject* project, kzString path, struct KzuAnimation** out_animation);

/** Loads animation clip data structure from binary. */
kzsException kzuProjectLoaderLoadAnimationClip(struct KzuProject* project, kzString path, struct KzuAnimationClip** out_animationClip);

/** Loads time line sequence data structure from binary. */
kzsException kzuProjectLoaderLoadTimeLineSequence(struct KzuProject* project, kzString path, struct KzuTimeLineSequence** out_timeLineSequence);

/** Converts binary format attribute type to engine attribute type. */
kzsError kzuProjectLoaderLoadAnimationConvertAttribute(const struct KzuPropertyType* propertyType,
                                                       enum KzuBinaryAnimationTargetAttribute binaryTargetAttribute,
                                                       enum KzuAnimationTargetAttribute* out_targetAttribute);

/** Unloads animation from project. */
kzsException kzuProjectLoaderUnloadAnimation(struct KzuProject* project, const struct KzuAnimation* animation);
/** Unloads animation clip from project. */
kzsException kzuProjectLoaderUnloadAnimationClip(struct KzuProject* project, const struct KzuAnimationClip* animationClip);
/** Unloads time line sequence from project. */
kzsException kzuProjectLoaderUnloadTimeLineSequence(struct KzuProject* project, const struct KzuTimeLineSequence* timeLineSequence);



#endif
