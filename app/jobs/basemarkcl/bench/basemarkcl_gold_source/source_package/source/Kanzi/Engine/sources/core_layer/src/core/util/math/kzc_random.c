/**
* \file
* Pseudo-random numnber generators.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_random.h"

#include <core/memory/kzc_memory_manager.h>

#include <system/time/kzs_tick.h>


kzsError kzcRandomCreate(const struct KzcMemoryManager* memoryManager, kzS32 seed, struct KzcRandom** out_random)
{
    kzsError result;
    struct KzcRandom* random;
    
    result = kzcMemoryAllocVariable(memoryManager, random, "Random");
    kzsErrorForward(result);

    *random = kzcRandomInline(seed);

    *out_random = random;
    kzsSuccess();
}

kzsError kzcRandomCreateDefault(const struct KzcMemoryManager* memoryManager, struct KzcRandom** out_random)
{
    kzsError result;
    struct KzcRandom* random;
    kzS32 seed = (kzS32)kzsTimeGetCurrentTimestamp();

    result = kzcRandomCreate(memoryManager, seed, &random);
    kzsErrorForward(result);

    *out_random = random;
    kzsSuccess();
}

struct KzcRandom kzcRandomInline(kzS32 seed)
{
    struct KzcRandom random;

    random.seed_private = seed;

    return random;
}

struct KzcRandom kzcRandomInlineDefault()
{
    return kzcRandomInline((kzS32)kzsTimeGetCurrentTimestamp());
}

kzsError kzcRandomDelete(struct KzcRandom* random)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(random));

    result = kzcMemoryFreeVariable(random);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcRandomSetSeed(struct KzcRandom* random, kzS32 seed)
{
    kzsAssert(random != KZ_NULL);

    random->seed_private = seed;
}

kzFloat kzcRandomFloat(struct KzcRandom* random)
{
    kzS32 k;
    kzFloat answer;
    kzS32 seed;

    kzsAssert(random != KZ_NULL);

    /* Park-Miller minimal random number generator */
    seed = random->seed_private;

    seed ^= 123459876;
    k = seed / 127773;
    seed = 16807 * (seed - k*127773) - 2836 * k;
    if (seed < 0) seed += 2147483647;
    answer = (1.0f / 2147483647) * (kzFloat)seed;
    seed ^= 123459876;

    random->seed_private = seed;
    return answer;
}

kzInt kzcRandomInteger(struct KzcRandom* random, kzInt min, kzInt max)
{
    kzsAssert(random != KZ_NULL);
    kzsAssert(max >= min);

    random->seed_private = 1664525L * random->seed_private + 1013904223L;

    /* Use high 16 bits for */
    return min + (kzInt)((kzU32)random->seed_private >> 16) * (max - min) / 0x10000;
}
