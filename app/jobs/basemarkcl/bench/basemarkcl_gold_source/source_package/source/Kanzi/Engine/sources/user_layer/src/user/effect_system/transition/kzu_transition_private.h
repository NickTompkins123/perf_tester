/**
* \file
* Transition private members.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TRANSITION_PRIVATE_H
#define KZU_TRANSITION_PRIVATE_H


#include <user/effect_system/transition/kzu_transition_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


struct KzuTransition
{
    struct KzuTransitionData* data;             /**< Transition data. */
};

struct KzuTransitionData
{
    kzBool selfOwned;                           /**< Data is owned by the transition. */
    enum KzuTransitionEffectType type;          /**< Type of the transition effect. */
};


/** Creates transition effect. */
kzsError kzuTransitionCreate_private(const struct KzcMemoryManager* memoryManager, enum KzuTransitionEffectType type, 
                                     struct KzuTransitionData* data, struct KzuTransition** out_transition);

/** Deletes the base structure of property type. */
kzsError kzuTransitionDeleteBase_private(struct KzuTransition* transition);


#endif
