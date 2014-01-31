/**
* \file
* Transition effect project object.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TRANSITION_H
#define KZU_TRANSITION_H


#include <user/effect_system/transition/kzu_transition_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzuProperty;
struct KzuPropertyType;
struct KzaApplication;


/**
* \struct KzuTransition
* Structure for application transitions effect.
*/
struct KzuTransition;


/** Frees memory allocated for transition effect. */
kzsError kzuTransitionDelete(struct KzuTransition* transition);


/** Returns the source scene of the transition. */
struct KzuScene* kzuTransitionGetSourceScene(const struct KzuTransition* transition);
/** Returns the target scene of the transition. */
struct KzuScene* kzuTransitionGetTargetScene(const struct KzuTransition* transition);


/** Gets the transition type from given transition. */
enum KzuTransitionEffectType kzuTransitionGetType(const struct KzuTransition* transition);

/** Frees memory allocated for transition effect. */
kzsError kzuTransitionStart(struct KzuTransition* transition);

/**
 * Takes ownership of the effect type transition deletion. After this, when a transition type is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuTransitionTakeOwnership(const struct KzuTransition* transition);
/** Checks if the transition is valid (pointer and the data are non-null). */
kzBool kzuTransitionIsValid(const struct KzuTransition* transition);
/** Moves the data from source transition to the target transition. */
void kzuTransitionTransferData(struct KzuTransition* targetTransition, struct KzuTransition* sourceTransition);


#endif
