/**
* \file
* Transition effect.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kza_transition.h"
#include "kza_transition_shader.h"

#include <application/kza_application.h>

#include <user/effect_system/transition/kzu_transition.h>
#include <user/effect_system/transition/kzu_transition_base.h>

#include <core/memory/kzc_memory_manager.h>

#include <system/wrappers/kzs_math.h>
#include <system/debug/kzs_log.h>


/** State of the playback. */
enum KzaTransitionPlayback
{
    KZA_TRANSITION_PLAYBACK_PLAY,  /**< Transition is playing. */
    KZA_TRANSITION_PLAYBACK_START, /**< Transition will start on next frame. */
    KZA_TRANSITION_PLAYBACK_END,   /**< Transition will end on next frame. */
    KZA_TRANSITION_PLAYBACK_IDLE   /**< No playback. */
};


struct KzaTransition
{
    kzUint duration;                    /**< Duration of the transition effect. */
    kzUint timeElapsed;                 /**< How long this effect has been running. */
    struct KzuTransition* transition;   /**< Transition effect to run. */
    struct KzuScene* targetScene;       /**<T arget scene of the transition effect. */
    struct KzuScene* sourceScene;       /**< Source scene of the transition effect. */
    struct KzuCameraNode* targetSceneCameraNode; /**< Target camera node. If KZ_NULL target camera is scene default. */

    enum KzaTransitionPlayback state;   /**< State of the transition playback. */
};


/** Prepares the transition effect. */
static kzsError kzaTransitionBegin_internal(const struct KzaTransition* transition, struct KzaApplication* application);
/** Ends the transition effect. */
static kzsError kzaTransitionEnd_internal(const struct KzaTransition* transition, struct KzaApplication* application);


kzsError kzaTransitionCreate(const struct KzcMemoryManager* memoryManager, struct KzaTransition** out_transition)
{
    kzsError result;
    struct KzaTransition* transition;

    result = kzcMemoryAllocVariable(memoryManager, transition, "Application Transition");
    kzsErrorForward(result);

    transition->duration = 1000;
    transition->timeElapsed = 0;
    transition->transition = KZ_NULL;
    transition->sourceScene = KZ_NULL;
    transition->targetScene = KZ_NULL;
    transition->targetSceneCameraNode = KZ_NULL;
    transition->state = KZA_TRANSITION_PLAYBACK_IDLE;

    *out_transition = transition;
    kzsSuccess();
}

kzsError kzaTransitionDelete(struct KzaTransition* transition)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(transition));

    result = kzcMemoryFreeVariable(transition);
    kzsErrorForward(result);

    kzsSuccess();
}

/* TODO: Remove application parameter */
kzsError kzaTransitionStart(struct KzaTransition* transition, struct KzuTransition* transitionEffect, struct KzaApplication* application, 
                            kzUint duration, struct KzuScene* sourceScene, struct KzuScene* targetScene, struct KzuCameraNode* cameraNode)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(transition));

    KZ_UNUSED_PARAMETER(application);

    transition->transition = transitionEffect;
    transition->duration = duration;
    transition->sourceScene = sourceScene;
    transition->targetScene = targetScene;
    transition->timeElapsed = 0;
    transition->state = KZA_TRANSITION_PLAYBACK_START;
    transition->targetSceneCameraNode = cameraNode;

    result = kzuTransitionStart(kzaTransitionGetTransitionEffect(transition));
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzaTransitionBegin_internal(const struct KzaTransition* transition, struct KzaApplication* application)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(transition));

    switch (kzuTransitionGetType(transition->transition))
    {
        case KZU_EFFECT_TRANSITION_TYPE_SHADER:
        {
            result = kzaTransitionShaderBegin(transition, application);
            kzsErrorForward(result);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid transition type");
        }
    }

    kzsSuccess();
}
                            
kzsError kzaTransitionUpdate(struct KzaTransition* transition, struct KzaApplication* application, kzUint deltaTime)
{
    kzsError result;
    kzFloat time;

    kzsAssert(kzcIsValidPointer(transition));

    switch(transition->state)
    {
        case KZA_TRANSITION_PLAYBACK_START:
        {
            transition->state = KZA_TRANSITION_PLAYBACK_PLAY;
            result = kzaTransitionBegin_internal(transition, application);
            kzsErrorForward(result);
            break;
        }
        case KZA_TRANSITION_PLAYBACK_END:
        {
            result = kzaTransitionEnd_internal(transition, application);
            kzsErrorForward(result);
            transition->state = KZA_TRANSITION_PLAYBACK_IDLE;
            break;
        }
        case KZA_TRANSITION_PLAYBACK_PLAY:
        {
            time = transition->timeElapsed / (kzFloat)transition->duration;
            time = kzsClampf(time, 0.0f, 1.0f);

            switch (kzuTransitionGetType(transition->transition))
            {
                case KZU_EFFECT_TRANSITION_TYPE_SHADER:
                {
                    result = kzaTransitionShaderUpdateEffect(transition, time);
                    kzsErrorForward(result);
                    break;
                }
                default:
                {
                    kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid transition type");
                }
            }

            transition->timeElapsed += deltaTime;

            if(transition->timeElapsed > transition->duration)
            {
                transition->state = KZA_TRANSITION_PLAYBACK_END;
                transition->timeElapsed = transition->duration;
            }
            break;
        }
        case KZA_TRANSITION_PLAYBACK_IDLE:
        {
            break;
        }
    }
    
    kzsSuccess();
}

static kzsError kzaTransitionEnd_internal(const struct KzaTransition* transition, struct KzaApplication* application)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(transition));

    switch (kzuTransitionGetType(transition->transition))
    {
        case KZU_EFFECT_TRANSITION_TYPE_SHADER:
        {
            result = kzaTransitionShaderEnd(transition, application);
            kzsErrorForward(result);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid transition type");
        }
    }

    kzsSuccess();
}

struct KzuScene* kzaTransitionGetSourceScene(const struct KzaTransition* transition)
{
    kzsAssert(kzcIsValidPointer(transition));
    return transition->sourceScene;
}

struct KzuScene* kzaTransitionGetTargetScene(const struct KzaTransition* transition)
{
    kzsAssert(kzcIsValidPointer(transition));
    return transition->targetScene;
}
struct KzuTransition* kzaTransitionGetTransitionEffect(const struct KzaTransition* transition)
{
    kzsAssert(kzcIsValidPointer(transition));
    return transition->transition;
}

struct KzuCameraNode* kzaTransitionGetTargetCameraNode(const struct KzaTransition* transition)
{
    kzsAssert(kzcIsValidPointer(transition));
    return transition->targetSceneCameraNode;
}
