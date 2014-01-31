/**
* \file
* Transition manager.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZA_TRANSITION_H
#define KZA_TRANSITION_H

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzaApplication;
struct KzuCameraNode;
struct KzuTransition;
struct KzuScene;

/**
* \struct KzaTransition
* Structure for application transitions manager. Drives the transition effects.
*/
struct KzaTransition;


/** Creates transition effect. */
kzsError kzaTransitionCreate(const struct KzcMemoryManager* memoryManager, struct KzaTransition** out_transition);
/** Frees memory allocated for transition effect. */
kzsError kzaTransitionDelete(struct KzaTransition* transition);

/** Starts transition for given settings. */
kzsError kzaTransitionStart(struct KzaTransition* transition, struct KzuTransition* transitionEffect, struct KzaApplication* application, 
                            kzUint duration, struct KzuScene* sourceScene, struct KzuScene* targetScene, struct KzuCameraNode* cameraNode);

/** Updates the transition effect. */
kzsError kzaTransitionUpdate(struct KzaTransition* transition, struct KzaApplication* application, kzUint deltaTime);

/** Returns the source scene of the transition. */
struct KzuScene* kzaTransitionGetSourceScene(const struct KzaTransition* transition);
/** Returns the target scene of the transition. */
struct KzuScene* kzaTransitionGetTargetScene(const struct KzaTransition* transition);
/** Returns the attached transition. */
struct KzuTransition* kzaTransitionGetTransitionEffect(const struct KzaTransition* transition);
/** Returns the target scenes camera node of the transition. */
struct KzuCameraNode* kzaTransitionGetTargetCameraNode(const struct KzaTransition* transition);

#endif
