/**
* \file
* Animation sequence.
* A sequence of time line entries. Used to control how animations are played.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TIME_LINE_SEQUENCE_H
#define KZU_TIME_LINE_SEQUENCE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzcDynamicArray;
struct KzcHashMap;
struct KzuAnimationKey;
struct KzuAnimationClip;
struct KzuTimeLineEntry;
struct KzuProperty;


/**
 * \struct KzuTimeLineSequence
 * A sequence of animation to play.
 * Contains the playing times and targets. Can be used to produce complex reusable animation sequences.
 */
struct KzuTimeLineSequence;


/** Creates new animation sequence. */
kzsError kzuTimeLineSequenceCreate(const struct KzcMemoryManager* memoryManager, struct KzuTimeLineSequence** out_timeLineSequence);
/** Deallocates memory reserved for time line sequence. */
kzsError kzuTimeLineSequenceDelete(struct KzuTimeLineSequence* timeLineSequence);


/** Updates animation in the time line sequence. */
kzsError kzuTimeLineSequenceUpdate(const struct KzuTimeLineSequence* timeLineEntry, struct KzcHashMap* overrides, kzFloat time);

/**
 * Sorts the animations the animation sequence. 
 * This function must be called if starting times of animations are modified.
 */
kzsError kzuTimeLineSequenceSortAnimations(const struct KzuTimeLineSequence* sequence);

/**
* Adds new time line entry to the sequence. The time line entry added to sequence should not be shared. 
* After the call sequence takes ownership of the structure and deletes it upon deletion of the sequence.
*/
kzsError kzuTimeLineSequenceAddEntry(const struct KzuTimeLineSequence* sequence, struct KzuTimeLineEntry* timeLineEntry);
/** Returns KZ_TRUE if sequence contains given KzuTimeLineEntry. KZ_FALSE if does not. */
kzBool kzuTimeLineSequenceContainsTimeLineEntry(const struct KzuTimeLineSequence* sequence, const struct KzuTimeLineEntry* timeLineEntry);
/** Removes a time line entry from sequence. Returns error if element is not found. */
kzsError kzuTimeLineSequenceRemoveTimeLineEntry(const struct KzuTimeLineSequence* sequence, const struct KzuTimeLineEntry* timeLineEntry);
/** Return KZ_TRUE if sequence contains 1 or more animation clip in entry. */
kzBool kzuTimeLineSequenceContainsAnimationClip(const struct KzuTimeLineSequence* sequence, const struct KzuAnimationClip* animationClip);
/** Removes timeline entry containing animation clip. Returns error if not found. If there are multiple entries containing the given clip, it will
    remove the first one found. */
kzsError kzuTimeLineSequenceRemoveAnimationClip(const struct KzuTimeLineSequence* sequence, const struct KzuAnimationClip* animationClip);

/** Gets the active animation count on given time from given time line. */
kzUint kzuTimeLineSequenceGetActiveTimeLineEntryCount(const struct KzuTimeLineSequence* sequence, kzFloat time);

/** Remove all time line entries which have finish playback before the given time. */
kzsError kzuTimeLineSequenceRemoveFinishedTimeLineEntries(const struct KzuTimeLineSequence* sequence, kzFloat time);

/** Removes all time line entries from the sequence. */
kzsError kzuTimeLineSequenceRemoveAllTimeLineEntries(const struct KzuTimeLineSequence* sequence);

/** Gets duration of the time line sequence. */
kzFloat kzuTimeLineSequenceGetDuration(const struct KzuTimeLineSequence* sequence);

/** Gets the timeline entry array from the sequence. */
struct KzcDynamicArray* kzuTimeLineSequenceGetEntries(const struct KzuTimeLineSequence* sequence);

/** Checks if time line sequence is valid. */
kzBool kzuTimeLineSequenceIsValid(const struct KzuTimeLineSequence* sequence);
/** Sets ownership of the memory. Used in project loader. */
void kzuTimeLineSequenceTakeOwnership(const struct KzuTimeLineSequence* sequence);
/** Moves the data from source to the target. */
void kzuTimeLineSequenceTransferData(struct KzuTimeLineSequence* target, struct KzuTimeLineSequence* source);


#endif
