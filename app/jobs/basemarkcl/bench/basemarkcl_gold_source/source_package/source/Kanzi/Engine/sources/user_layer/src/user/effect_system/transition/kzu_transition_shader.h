/**
* \file
* Transition shader effect.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TRANSITION_SHADER_H
#define KZU_TRANSITION_SHADER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuMaterial;
struct KzuObjectSource;
struct KzuTransition;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzuPropertyManager;
struct KzuMessageDispatcher;


/**
* \struct KzuTransitionShader
* Structure for shader based transition effect.
*/
struct KzuTransitionShader;


/** Creates a transition shader effect. */
kzsError kzuTransitionShaderCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                   struct KzcResourceManager* resourceManager, struct KzuObjectSource* rootObjectSource,
                                   kzUint renderTargetWidth, kzUint renderTargetHeight, struct KzuMaterial* material, 
                                   struct KzuTransitionShader** out_transition);

/** Deletes transition shader effect. */
kzsError kzuTransitionShaderDelete(struct KzuTransitionShader* transition);

/** Start transition. Must be called when transition is started. */
kzsError kzuTransitionShaderStart(const struct KzuTransitionShader* transition);

/** Type casts transition shader to transition. */
struct KzuTransitionShader* kzuTransitionShaderFromTransition(struct KzuTransition* transition);
/** Type casts transition to transition shader. */
struct KzuTransition* kzuTransitionFromTransitionShader(struct KzuTransitionShader* transitionShader);
/** Type casts shader transition to transition. */
struct KzuTransition* kzuTransitionShaderToTransition(struct KzuTransitionShader* transition);


#endif
