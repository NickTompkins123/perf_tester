/**
* \file
* Stopwatch.
*
* Usage:
* 1) initialize
* 2) start
* 3) stop
* 
* To reuse stopwatch, initialize it after use.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_STOPWATCH_H
#define KZC_STOPWATCH_H


#include <system/kzs_types.h>
#include <system/time/kzs_tick.h>
#include <system/debug/kzs_log.h>


/** State of stopwatch. */
enum KzcStopwatchState
{
    KZC_STOPWATCH_INITIALIZED, /**< Stopwatch is initialized but not started. */
    KZC_STOPWATCH_RUNNING, /**< Stopwatch is running. */
    KZC_STOPWATCH_STOPPED /**< Stopwatch has been stopped. */
};

/** Stopwatch structure to time spans. */
struct KzcStopwatch
{
    kzUint start_private;    /**< Start time of span. */
    kzUint stop_private;     /**< End time of span*/
    kzUint duration_private; /**< Duration of span*/
    kzUint deltaPrevious_private; /**< Delta-time for micro-timings */
    enum KzcStopwatchState state_private;    /**< Stopwatch state */
};


/** Init stopwatch. Begin all loops with call to this. */
struct KzcStopwatch kzcStopwatchInitialize(void);

/** Is the stopwatch running. */
#define kzcStopwatchIsRunning(stopwatch_param) ((stopwatch_param.state_private == KZC_STOPWATCH_RUNNING) ? KZ_TRUE : KZ_FALSE)

/** Start stopwatch. */
void kzcStopwatchStart(struct KzcStopwatch* stopwatch);

/** Stops stopwatch. */
void kzcStopwatchStop(struct KzcStopwatch* stopwatch);

/** Get current time running in the stopwatch. Doesn't stop it. */
kzUint kzcStopwatchGetCurrentTime(const struct KzcStopwatch* stopwatch);

/** Get duration of last interval. If between start and stop result is undefined. */
kzUint kzcStopwatchGetDuration(const struct KzcStopwatch* stopwatch);

/** Get difference from previous call to kzcStopwatchStart or kzcStopwatchSampleDeltaTime, whichever was called later. */
kzUint kzcStopwatchSampleDeltaTime(struct KzcStopwatch* stopwatch);


#endif
