/**
 * \file
 * Named counters.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzs_counter.h"

#ifdef KZS_COUNTERS_ENABLED

#include <system/wrappers/kzs_string.h>
#include <system/wrappers/kzs_math.h>

#include <stdio.h>


/** Maximum number of counter keys. Must be power of two. */
#define KZS_MAXIMUM_COUNTERS_COUNT 256


/** Single counter hash table entry. */
struct KzsCounterHashEntry
{
    kzString name; /**< Name of the counter. */
    kzU32 hashCode; /**< Hash value calculated from the name. */
    kzUint value; /**< Counter value. */
};

/** Single counter entry for sorting. */
struct KzsCounterEntry
{
    kzString name; /**< Name of the counter. */
    kzUint value; /**< Counter value. */
};

/** Hash table of counters. */
static struct KzsCounterHashEntry g_kzsCounters[KZS_MAXIMUM_COUNTERS_COUNT];


/** Calculates hash code from given string. */
static kzU32 kzsCounterGetHashCode_private(kzString string)
{
    kzU32 hash = 5381;
    kzUint i;

    /* djb2 hash algorithm */
    for (i = 0; string[i] != '\0'; ++i)
    {
        hash = ((hash << 5) + hash) + (kzByte)string[i]; /* hash * 33 + c */
    }

    return hash;
}

kzsError kzsCountersInitialize()
{
    kzsCountersReset();

    kzsSuccess();
}

kzsError kzsCountersUninitialize()
{
    kzsSuccess();
}

void kzsCountersReset()
{
    kzUint i;

    for (i = 0; i < KZS_MAXIMUM_COUNTERS_COUNT; ++i) {
        g_kzsCounters[i].name = KZ_NULL;
    }
}

/** Comparator for sorting counters by their name. */
static kzInt kzsCounterComparatorName_internal(const void* first, const void* second)
{
    struct KzsCounterEntry* firstEntry = (struct KzsCounterEntry*)first;
    struct KzsCounterEntry* secondEntry = (struct KzsCounterEntry*)second;

    return kzsStrcmp(firstEntry->name, secondEntry->name);
}

/** Comparator for sorting counters by their value. */
static kzInt kzsCounterComparatorValue_internal(const void* first, const void* second)
{
    struct KzsCounterEntry* firstEntry = (struct KzsCounterEntry*)first;
    struct KzsCounterEntry* secondEntry = (struct KzsCounterEntry*)second;

    return ((kzInt)secondEntry->value - (kzInt)firstEntry->value);
}

void kzsCountersPrint(enum KzsCounterSortMode sortMode)
{
    kzUint counterCount = 0;
    struct KzsCounterEntry countersForSort[KZS_MAXIMUM_COUNTERS_COUNT];
    kzInt (*comparator)(const void*, const void*);

    /* Copy counters to sort array. */
    {
        kzUint i;
        for (i = 0; i < KZS_MAXIMUM_COUNTERS_COUNT; ++i)
        {
            kzString name = g_kzsCounters[i].name;
            if (name != KZ_NULL)
            {
                countersForSort[counterCount].name = name;
                countersForSort[counterCount].value = g_kzsCounters[i].value;
                ++counterCount;
            }
        }
    }

    /* Sort. */
    switch (sortMode)
    {
        case KZS_COUNTER_SORT_MODE_NAME: comparator = kzsCounterComparatorName_internal; break;
        case KZS_COUNTER_SORT_MODE_COUNT: comparator = kzsCounterComparatorValue_internal; break;
        default:
        {
            kzsAssert(KZ_FALSE);
            comparator = KZ_NULL;
        }
    }
    kzsQsort(countersForSort, counterCount, sizeof(struct KzsCounterEntry), comparator);

    /* Print counters. */
    {
        kzUint i;
        printf("Counters:\n");
        for (i = 0; i < counterCount; ++i)
        {
            printf("\t%u\t%s\n", countersForSort[i].value, countersForSort[i].name);
        }
    }
}

/** Find hash table index for given counter name. */
static kzUint kzsCounterFindIndex_private(kzString name)
{
    kzU32 hashCode = kzsCounterGetHashCode_private(name);
    kzUint i = 0;
    kzBool found = KZ_FALSE;
    kzUint index;

    do
    {
        kzString existingName;
        index = ((kzUint)hashCode + i) & (KZS_MAXIMUM_COUNTERS_COUNT - 1);
        existingName = g_kzsCounters[index].name;

        /* Check if the slot is empty. */
        if (existingName == KZ_NULL)
        {
            found = KZ_TRUE;
            /* Initialize the slot. */
            g_kzsCounters[index].name = name;
            g_kzsCounters[index].hashCode = hashCode;
            g_kzsCounters[index].value = 0;
        }
        /* If not, check if the slot matches the given name. */
        else if (g_kzsCounters[index].hashCode == hashCode &&
                 kzsStrcmp(existingName, name) == 0)
        {
            found = KZ_TRUE;
        }

        /* Otherwise move to the next slot. */
        if (!found)
        {
            ++i;
            kzsAssert(i < KZS_MAXIMUM_COUNTERS_COUNT);
        }
    } while (!found);

    return index;
}

void kzsCounterIncrease_private(kzString name)
{
    kzUint index = kzsCounterFindIndex_private(name);
    ++g_kzsCounters[index].value;
}

#else

kzsError kzsCountersInitialize()
{
    kzsSuccess();
}

kzsError kzsCountersUninitialize()
{
    kzsSuccess();
}

void kzsCountersReset()
{
}

void kzsCountersPrint(enum KzsCounterSortMode sortMode)
{
    KZ_UNUSED_PARAMETER(sortMode);
}

#endif
