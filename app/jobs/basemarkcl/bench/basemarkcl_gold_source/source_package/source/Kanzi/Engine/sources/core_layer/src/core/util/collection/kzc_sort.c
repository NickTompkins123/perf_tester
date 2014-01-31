/**
 * \file
 * Generic sorting functions
 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_sort.h"

#include <system/wrappers/kzs_math.h>


void kzcSort(void* buffer, kzUint elementCount, kzUint elementSize, KzcComparatorFunction comparator)
{
    kzsQsort(buffer, elementCount, elementSize, comparator);
}

/* TODO: Use better algorithm such as quicksort */
void kzcSortWithContext(void* buffer, kzUint elementCount, kzUint elementSize, KzcComparatorWithContextFunction comparator,
                        const void* context)
{
    kzUint i, j;
    kzUint minimum;
    for (i = 0; i + 1 < elementCount; ++i)
    {
        minimum = i;
        for (j = (i + 1); j < elementCount; ++j)
        {
            if (comparator((kzByte*)buffer + (j * elementSize),
                           (kzByte*)buffer + (minimum * elementSize),
                           context) < 0)
            {
                minimum = j;
            }
        }
        if (i != minimum)
        {
            kzsSwap((kzByte*)buffer + (i * elementSize),
                    (kzByte*)buffer + (minimum * elementSize),
                    elementSize);
        }
    }
}
