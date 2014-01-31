/**
 * \file
 * Named counters.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZS_COUNTER_H
#define KZS_COUNTER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


#ifdef _DEBUG
#if 0
#define KZS_COUNTERS_ENABLED
#endif
#endif


/** Specifies how the counters are sorted when printing. */
enum KzsCounterSortMode
{
    KZS_COUNTER_SORT_MODE_NAME, /**< Sort alphabetically by counter name. */
    KZS_COUNTER_SORT_MODE_COUNT /**< Sort by highest values first. */
};


/** Initializes counters. */
kzsError kzsCountersInitialize(void);

/** Uninitializes counters. */
kzsError kzsCountersUninitialize(void);

/** Resets all counter data. */
void kzsCountersReset(void);

/** Prints the values of all registered counters. */
void kzsCountersPrint(enum KzsCounterSortMode sortMode);


#ifdef KZS_COUNTERS_ENABLED
/** Increases the value of a counter. */
#define kzsCounterIncrease(name_param) kzsCounterIncrease_private(name_param)

/** \see kzsCounterIncrease */
void kzsCounterIncrease_private(kzString name);
#else
/** \see kzsCounterIncrease */
#define kzsCounterIncrease(name_param) (void)0
#endif


#endif
