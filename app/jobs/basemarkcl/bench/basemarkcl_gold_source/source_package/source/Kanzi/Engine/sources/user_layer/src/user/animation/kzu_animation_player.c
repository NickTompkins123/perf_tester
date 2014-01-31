/**
* \file
* Animation player
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_animation_player.h"

#include <user/animation/kzu_animation.h>
#include <user/animation/kzu_time_line_entry.h>
#include <user/animation/kzu_time_line_sequence.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_dynamic_array.h>


struct KzuAnimationPlayer
{
    kzFloat time;           /**< Current time of the player. */
    kzBool paused;          /**< Is animation player paused. */
    struct KzuTimeLineSequence* sequence; /**< Animation sequence. */
    struct KzcHashMap* overrides; /**< Override table for animation player. */
};


kzsError kzuAnimationPlayerCreate(const struct KzcMemoryManager* memoryManager, struct KzuAnimationPlayer** out_animationPlayer)
{
    kzsError result;
    struct KzuAnimationPlayer* player;
    struct KzcHashMapConfiguration overrideMapConfiguration = {kzuHashCodeFromAnimationOverride, kzuCompareAnimationOverrides};

    result = kzcMemoryAllocVariable(memoryManager, player, "AnimationPlayer");
    kzsErrorForward(result);

    result = kzcHashMapCreateWithCapacity(memoryManager, overrideMapConfiguration, 20, &player->overrides);
    kzsErrorForward(result);

    result = kzuTimeLineSequenceCreate(memoryManager, &player->sequence);
    kzsErrorForward(result);

    player->time = 0.0f;
    player->paused = KZ_FALSE;
    
    *out_animationPlayer = player;
    kzsSuccess();
}

kzsError kzuAnimationPlayerDelete(struct KzuAnimationPlayer* player)
{
    kzsError result;
    
    kzsAssert(kzcIsValidPointer(player));

    result = kzcHashMapDelete(player->overrides);
    kzsErrorForward(result);

    result = kzuTimeLineSequenceDelete(player->sequence);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(player);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuAnimationPlayerUpdate(struct KzuAnimationPlayer* player, kzFloat deltaTime)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(player));

    if(!player->paused)
    {
        player->time += deltaTime;
    }

    /* Clear overrides table. It gets refilled on the fly. */
    kzcHashMapClear(player->overrides);

    result = kzuTimeLineSequenceUpdate(player->sequence, player->overrides, player->time);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuAnimationPlayerSetTime(struct KzuAnimationPlayer* player, kzFloat time)
{
    kzsAssert(kzcIsValidPointer(player));
    player->time = time;
}

kzsError kzuAnimationPlayerAddTimeLineEntry(const struct KzuAnimationPlayer* player, struct KzuTimeLineEntry* timeLineEntry)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(player));
    kzsAssert(kzcIsValidPointer(timeLineEntry));

    result = kzuTimeLineSequenceAddEntry(player->sequence, timeLineEntry);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuTimeLineSequence* kzuAnimationPlayerGetTimeLineSequence(const struct KzuAnimationPlayer* player)
{
    kzsAssert(kzcIsValidPointer(player));
    return player->sequence;
}

kzsError kzuAnimationPlayerRemoveTimeLineEntry(const struct KzuAnimationPlayer* player, const struct KzuTimeLineEntry* timeLineEntry)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(player));
    kzsAssert(kzcIsValidPointer(timeLineEntry));

    result = kzuTimeLineSequenceRemoveTimeLineEntry(player->sequence, timeLineEntry);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuAnimationPlayerSetPaused(struct KzuAnimationPlayer* player, kzBool paused)
{
    kzsAssert(kzcIsValidPointer(player));
    player->paused = paused;
}

kzBool kzuAnimationPlayerGetPaused(const struct KzuAnimationPlayer* player)
{
    kzsAssert(kzcIsValidPointer(player));
    return player->paused;
}

kzFloat kzuAnimationPlayerGetTime(const struct KzuAnimationPlayer* player)
{
    kzsAssert(kzcIsValidPointer(player));
    return player->time;
}

kzBool kzuAnimationPlayerGetTimeLineEntryFinished(const struct KzuAnimationPlayer* player, const struct KzuTimeLineEntry* timeLineEntry)
{
    kzBool returnValue = KZ_FALSE;
    kzsAssert(kzcIsValidPointer(player));
    
    if(kzuAnimationPlayerGetTime(player) > kzuTimeLineEntryGetEndingTime(timeLineEntry))
    {
        returnValue = KZ_TRUE;
    }

    return returnValue;
}
