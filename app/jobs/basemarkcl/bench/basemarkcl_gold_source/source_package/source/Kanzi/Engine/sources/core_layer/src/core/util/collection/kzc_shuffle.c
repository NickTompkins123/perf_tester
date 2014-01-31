/**
 * \file
 * Generic shuffle functions
 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_shuffle.h"

#include <core/util/math/kzc_random.h>
#include <system/wrappers/kzs_math.h>


void kzcShuffle(void* buffer, kzUint elementCount, kzUint elementSize)
{
    struct KzcRandom random = kzcRandomInlineDefault();

    kzcShuffleWithRandom(&random, buffer, elementCount, elementSize);
}

void kzcShuffleWithRandom(struct KzcRandom* random, void* buffer, kzUint elementCount, kzUint elementSize)
{
    kzUint i;

    for (i = elementCount; i > 0; --i)
    {
        kzUint index = (kzUint)kzcRandomInteger(random, 0, (kzInt)i);
        kzsSwap((kzByte*)buffer + ((i - 1) * elementSize),
                (kzByte*)buffer + (index * elementSize),
                elementSize);
    }
}
