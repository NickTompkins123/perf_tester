/**
* \file
* Transition shader effect.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZA_TRANSITION_SHADER_H
#define KZA_TRANSITION_SHADER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzaApplication;
struct KzuMaterial;
struct KzcMemoryManager;
struct KzaTransition;


/** Updates the transition effect. Time parameter is on the range [0 - 1], where 0 is start and 1 is end. */
kzsError kzaTransitionShaderUpdateEffect(const struct KzaTransition* transition, const kzFloat time);

/** Should be called when transition effect is started. Initializes the buffers */
kzsError kzaTransitionShaderBegin(const struct KzaTransition* transition, struct KzaApplication* application);

/** Ends transition effect. */
kzsError kzaTransitionShaderEnd(const struct KzaTransition* transition, struct KzaApplication* application);


#endif
