/**
* \file
* Transition effect.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_transition.h"

#include "kzu_transition_private.h"
#include "kzu_transition_shader.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_collection.h>

#include <core/memory/kzc_memory_manager.h>


kzsError kzuTransitionCreate_private(const struct KzcMemoryManager* memoryManager, enum KzuTransitionEffectType type, 
                                     struct KzuTransitionData* data, struct KzuTransition** out_transition)
{
    kzsError result;
    struct KzuTransition* transition;

    result = kzcMemoryAllocVariable(memoryManager, transition, "Transition");
    kzsErrorForward(result);

    transition->data = data;
    transition->data->selfOwned = KZ_TRUE;
    transition->data->type = type;

    *out_transition = transition;
    kzsSuccess();
}

kzsError kzuTransitionDeleteBase_private(struct KzuTransition* transition)
{
    kzsError result;
    struct KzuTransitionData* transitionData;
    kzBool selfOwned;

    kzsAssert(kzuTransitionIsValid(transition));

    transitionData = transition->data;
    selfOwned = transitionData->selfOwned;

    result = kzcMemoryFreeVariable(transitionData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(transition);
        kzsErrorForward(result);
    }
    else
    {
        transition->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuTransitionDelete(struct KzuTransition* transition)
{
    kzsError result;

    kzsAssert(kzuTransitionIsValid(transition));

    switch(kzuTransitionGetType(transition))
    {
        case KZU_EFFECT_TRANSITION_TYPE_SHADER:
        {
            result = kzuTransitionShaderDelete(kzuTransitionShaderFromTransition(transition));
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

void kzuTransitionTakeOwnership(const struct KzuTransition* transition)
{
    kzsAssert(kzuTransitionIsValid(transition));

    transition->data->selfOwned = KZ_FALSE;
}

kzBool kzuTransitionIsValid(const struct KzuTransition* transition)
{
    return transition != KZ_NULL && transition->data != KZ_NULL;
}

void kzuTransitionTransferData(struct KzuTransition* targetTransition, struct KzuTransition* sourceTransition)
{
    kzsAssert(kzcIsValidPointer(targetTransition));
    kzsAssert(targetTransition->data == KZ_NULL);
    kzsAssert(kzuTransitionIsValid(sourceTransition));

    targetTransition->data = sourceTransition->data;
    sourceTransition->data = KZ_NULL;
}

enum KzuTransitionEffectType kzuTransitionGetType(const struct KzuTransition* transition)
{
    kzsAssert(kzcIsValidPointer(transition));
    return transition->data->type;
}

kzsError kzuTransitionStart(struct KzuTransition* transition)
{
    kzsError result;

    kzsAssert(kzuTransitionIsValid(transition));

    switch(kzuTransitionGetType(transition))
    {
        case KZU_EFFECT_TRANSITION_TYPE_SHADER:
        {
            result = kzuTransitionShaderStart(kzuTransitionShaderFromTransition(transition));
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
