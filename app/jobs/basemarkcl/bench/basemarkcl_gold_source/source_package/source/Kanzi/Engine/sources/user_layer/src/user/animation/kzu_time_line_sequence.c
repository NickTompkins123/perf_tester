/**
* \file
* Animation sequence.
* A sequence of time line entries. Used to control how animations are played.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_time_line_sequence.h"
#include "kzu_time_line_entry.h"

#include <user/animation/kzu_animation_clip.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>

#include <system/wrappers/kzs_math.h>
#include <system/debug/kzs_log.h>


struct KzuTimeLineSequenceData
{
    kzBool selfOwned; /**< Indicates that the object source container will be deleted when this data is deleted. */
    struct KzcDynamicArray* timeLineEntries; /** Time line entries contained in this sequence ordered by starting time. <KzuTimeLineEntry>. */
};

struct KzuTimeLineSequence
{
    struct KzuTimeLineSequenceData* data;
};


/** Comparator for time line entries. */
static kzInt kzuTimeLineEntryComparator_internal(const void* first, const void* second);


kzsError kzuTimeLineSequenceCreate(const struct KzcMemoryManager* memoryManager, struct KzuTimeLineSequence** out_timeLineSequence)
{
    kzsError result;
    struct KzuTimeLineSequence* timeLineSequence;

    result = kzcMemoryAllocVariable(memoryManager, timeLineSequence, "Time line sequence");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, timeLineSequence->data, "Time line sequence data");
    kzsErrorForward(result);

    timeLineSequence->data->selfOwned = KZ_TRUE;
    result = kzcDynamicArrayCreate(memoryManager, &timeLineSequence->data->timeLineEntries);
    kzsErrorForward(result);
    
    *out_timeLineSequence = timeLineSequence;
    kzsSuccess();
}

kzsError kzuTimeLineSequenceDelete(struct KzuTimeLineSequence* timeLineSequence)
{
    kzsError result;
    kzBool selfOwned;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzcIsValidPointer(timeLineSequence));

    selfOwned = timeLineSequence->data->selfOwned;

    it = kzcDynamicArrayGetIterator(timeLineSequence->data->timeLineEntries);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTimeLineEntry* entry = (struct KzuTimeLineEntry*)kzcDynamicArrayIteratorGetValue(it);
        result = kzuTimeLineEntryDelete(entry);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayDelete(timeLineSequence->data->timeLineEntries);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(timeLineSequence->data);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(timeLineSequence);
        kzsErrorForward(result);
    }
    else
    {
        timeLineSequence->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuTimeLineSequenceUpdate(const struct KzuTimeLineSequence* timeLineSequence, struct KzcHashMap* overrides, kzFloat time)
{
    kzsError result;
    struct KzcDynamicArrayMutableIterator it;

    kzsAssert(kzcIsValidPointer(timeLineSequence));
    
    it = kzcDynamicArrayGetMutableIterator(timeLineSequence->data->timeLineEntries);
    while(kzcDynamicArrayMutableIterate(it))
    {
        struct KzuTimeLineEntry* entry = kzcDynamicArrayMutableIteratorGetValue(it);
        kzFloat endingTime = kzuTimeLineEntryGetEndingTime(entry);

        if(kzuTimeLineEntryIsPlayFirstFrameOnce(entry))
        {
            result = kzuTimeLineEntryUpdate(entry, overrides, kzuTimeLineEntryGetStartingTime(entry));
            kzsErrorForward(result);
            result = kzcDynamicArrayMutableIteratorRemove(it);
            kzsErrorForward(result);
            result = kzuTimeLineEntryDelete(entry);
            kzsErrorForward(result);
        }
        else
        {
            if(endingTime < time)
            {
                if(kzuTimeLineEntryGetActiveLastFrame(entry))
                {
                    result = kzuTimeLineEntryUpdate(entry, overrides, endingTime);
                    kzsErrorForward(result);
                    kzuTimeLineEntrySetActiveLastFrame(entry, KZ_FALSE);
                }
                if(kzuTimeLineEntryGetDeleteAfterPlayback(entry))
                {
                    result = kzcDynamicArrayMutableIteratorRemove(it);
                    kzsErrorForward(result);
                    result = kzuTimeLineEntryDelete(entry);
                    kzsErrorForward(result);
                }
            }
            else
            {
                result = kzuTimeLineEntryUpdate(entry, overrides, time);
                kzsErrorForward(result);
            }
        }
    }

    kzsSuccess();
}

static kzInt kzuTimeLineEntryComparator_internal(const void* first, const void* second)
{
    struct KzuTimeLineEntry* firstObject = *(struct KzuTimeLineEntry**)first;
    struct KzuTimeLineEntry* secondObject = *(struct KzuTimeLineEntry**)second;

    kzFloat firstTime = kzuTimeLineEntryGetStartingTime(firstObject);
    kzFloat secondTime = kzuTimeLineEntryGetStartingTime(secondObject);

    return kzcCompareFloats(&firstTime, &secondTime);
}

kzsError kzuTimeLineSequenceSortAnimations(const struct KzuTimeLineSequence* sequence)
{
    kzsAssert(kzcIsValidPointer(sequence));
    kzcDynamicArraySort(sequence->data->timeLineEntries, kzuTimeLineEntryComparator_internal);
    kzsSuccess();
}

kzsError kzuTimeLineSequenceAddEntry(const struct KzuTimeLineSequence* sequence, struct KzuTimeLineEntry* timeLineEntry)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(sequence));
    kzsAssert(kzcIsValidPointer(timeLineEntry));

    result = kzcDynamicArrayAdd(sequence->data->timeLineEntries, timeLineEntry);
    kzsErrorForward(result);

    result = kzuTimeLineSequenceSortAnimations(sequence);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuTimeLineSequenceContainsTimeLineEntry(const struct KzuTimeLineSequence* sequence, const struct KzuTimeLineEntry* timeLineEntry)
{
    kzsAssert(kzcIsValidPointer(sequence));
    kzsAssert(kzcIsValidPointer(timeLineEntry));

    return kzcDynamicArrayContains(sequence->data->timeLineEntries, timeLineEntry);
}

kzsError kzuTimeLineSequenceRemoveTimeLineEntry(const struct KzuTimeLineSequence* sequence, const struct KzuTimeLineEntry* timeLineEntry)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(sequence));
    kzsAssert(kzcIsValidPointer(timeLineEntry));

    result = kzcDynamicArrayRemove(sequence->data->timeLineEntries, timeLineEntry);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuTimeLineSequenceContainsAnimationClip(const struct KzuTimeLineSequence* sequence, const struct KzuAnimationClip* animationClip)
{
    struct KzcDynamicArrayIterator it;
    kzBool found = KZ_FALSE;
    kzsAssert(kzcIsValidPointer(sequence));
    kzsAssert(kzcIsValidPointer(animationClip));

    it = kzcDynamicArrayGetIterator(sequence->data->timeLineEntries);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzuTimeLineEntry* entry = kzcDynamicArrayIteratorGetValue(it);
        if(kzuTimeLineEntryGetAnimationClip(entry) == animationClip)
        {
            found = KZ_TRUE;
            break;
        }
    }

    return found;
}

kzsError kzuTimeLineSequenceRemoveAnimationClip(const struct KzuTimeLineSequence* sequence, const struct KzuAnimationClip* animationClip)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    struct KzuTimeLineEntry* foundEntry = KZ_NULL;
    kzsAssert(kzcIsValidPointer(sequence));
    kzsAssert(kzcIsValidPointer(animationClip));

    it = kzcDynamicArrayGetIterator(sequence->data->timeLineEntries);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzuTimeLineEntry* entry = kzcDynamicArrayIteratorGetValue(it);
        if(kzuTimeLineEntryGetAnimationClip(entry) == animationClip)
        {
            foundEntry = entry;
            break;
        }
    }

    result = kzuTimeLineSequenceRemoveTimeLineEntry(sequence, foundEntry);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzuTimeLineSequenceGetActiveTimeLineEntryCount(const struct KzuTimeLineSequence* sequence, kzFloat time)
{
    kzUint activeAnimationCount = 0;
    kzsAssert(kzcIsValidPointer(sequence));

    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(sequence->data->timeLineEntries);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuTimeLineEntry* entry = (struct KzuTimeLineEntry*)kzcDynamicArrayIteratorGetValue(it);
            if(kzuTimeLineEntryIsActiveAtTime(entry, time))
            {
                ++activeAnimationCount;
            }
        }
    }
    return activeAnimationCount;
}

kzsError kzuTimeLineSequenceRemoveAllTimeLineEntries(const struct KzuTimeLineSequence* sequence)
{
    kzsAssert(kzcIsValidPointer(sequence));

    kzcDynamicArrayClear(sequence->data->timeLineEntries);
    
    kzsSuccess();
}

kzFloat kzuTimeLineSequenceGetDuration(const struct KzuTimeLineSequence* sequence)
{
    kzFloat duration = 0.0f;
    kzsAssert(kzuTimeLineSequenceIsValid(sequence));

    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(sequence->data->timeLineEntries);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuTimeLineEntry* entry = (struct KzuTimeLineEntry*)kzcDynamicArrayIteratorGetValue(it);
            duration = kzsMaxf(duration, kzuTimeLineEntryGetEndingTime(entry));
        }
    }
    return duration;
}

struct KzcDynamicArray* kzuTimeLineSequenceGetEntries(const struct KzuTimeLineSequence* sequence)
{
    kzsAssert(kzuTimeLineSequenceIsValid(sequence));
    return sequence->data->timeLineEntries;
}

kzBool kzuTimeLineSequenceIsValid(const struct KzuTimeLineSequence* sequence)
{
    return sequence != KZ_NULL && sequence->data != KZ_NULL;
}

void kzuTimeLineSequenceTakeOwnership(const struct KzuTimeLineSequence* sequence)
{
    kzsAssert(kzuTimeLineSequenceIsValid(sequence));
    sequence->data->selfOwned = KZ_FALSE;
}

void kzuTimeLineSequenceTransferData(struct KzuTimeLineSequence* target, struct KzuTimeLineSequence* source)
{
    kzsAssert(kzcIsValidPointer(target));
    kzsAssert(target->data == KZ_NULL);
    kzsAssert(kzuTimeLineSequenceIsValid(source));

    target->data = source->data;
    source->data = KZ_NULL;
}
