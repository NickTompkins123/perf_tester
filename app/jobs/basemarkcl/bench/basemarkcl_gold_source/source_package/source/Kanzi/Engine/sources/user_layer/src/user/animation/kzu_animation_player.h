/**
* \file
* Animation player.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_ANIMATION_PLAYER_H
#define KZU_ANIMATION_PLAYER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager; 
struct KzuTimeLineEntry;
struct KzuTimeLineSequence;
struct KzuProperty;


/**
 * \struct KzuAnimationPlayer
 * Animation Player.
 * Player contains collection of time line entries, which are used to track when to play the animations.
 */
struct KzuAnimationPlayer;


/** Allocates memory for animation player and initializes it. */
kzsError kzuAnimationPlayerCreate(const struct KzcMemoryManager* memoryManager, struct KzuAnimationPlayer** out_animationPlayer);
/** Deallocates memory reserved for animation player. */
kzsError kzuAnimationPlayerDelete(struct KzuAnimationPlayer* animationPlayer);

/** Updates animations in player. */
kzsError kzuAnimationPlayerUpdate(struct KzuAnimationPlayer* player, kzFloat deltaTime);
/** Sets animation player to given time. */
void kzuAnimationPlayerSetTime(struct KzuAnimationPlayer* player, kzFloat time);

/** Sets pause state of the animation player. */
void kzuAnimationPlayerSetPaused(struct KzuAnimationPlayer* player, kzBool paused);
/** Returns KZ_TRUE if player is currently paused. KZ_FALSE if not. */
kzBool kzuAnimationPlayerGetPaused(const struct KzuAnimationPlayer* player);
/** Returns current time of the animation player. */
kzFloat kzuAnimationPlayerGetTime(const struct KzuAnimationPlayer* player);

/** Adds new time line entry to the player. */
kzsError kzuAnimationPlayerAddTimeLineEntry(const struct KzuAnimationPlayer* player, struct KzuTimeLineEntry* timeLineEntry);
/** Returns time line sequence from the player. */
struct KzuTimeLineSequence* kzuAnimationPlayerGetTimeLineSequence(const struct KzuAnimationPlayer* player);

/** Removes time line entry from the player. */
kzsError kzuAnimationPlayerRemoveTimeLineEntry(const struct KzuAnimationPlayer* player, const struct KzuTimeLineEntry* timeLineEntry);

/** Returns boolean indicating if given time line entry is finished on the animation players time line. */
kzBool kzuAnimationPlayerGetTimeLineEntryFinished(const struct KzuAnimationPlayer* player, const struct KzuTimeLineEntry* timeLineEntry);

#endif
