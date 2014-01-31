/**
* \file
* Data logger.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_stopwatch.h"

#include <system/time/kzs_tick.h>
#include <system/debug/kzs_log.h>


struct KzcStopwatch kzcStopwatchInitialize()
{
    struct KzcStopwatch stopwatch;
    stopwatch.start_private = 0;
    stopwatch.stop_private = 0;
    stopwatch.duration_private= 0;
    stopwatch.deltaPrevious_private = 0;
    stopwatch.state_private = KZC_STOPWATCH_INITIALIZED;
    return stopwatch;
}

void kzcStopwatchStart(struct KzcStopwatch* stopwatch)
{
    if(stopwatch->state_private == KZC_STOPWATCH_INITIALIZED)
    {
        stopwatch->start_private = kzsTimeGetCurrentTimestamp();
        stopwatch->deltaPrevious_private = stopwatch->start_private;
        stopwatch->duration_private = 0;
        stopwatch->state_private = KZC_STOPWATCH_RUNNING;
    }
    else
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Stopwatch started without stopping or initing. Call StopwatchInitialize first");
        kzsAssert(KZ_FALSE);
    }
}

void kzcStopwatchStop(struct KzcStopwatch* stopwatch)
{
    if(stopwatch->state_private == KZC_STOPWATCH_RUNNING)
    {
        stopwatch->stop_private = kzsTimeGetCurrentTimestamp();
        stopwatch->duration_private = stopwatch->stop_private - stopwatch->start_private;
        stopwatch->state_private = KZC_STOPWATCH_STOPPED;
    }
    else
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Stopwatch stopped without starting. Call StopwatchStart first");
        kzsAssert(KZ_FALSE);
    }
}

kzUint kzcStopwatchGetCurrentTime(const struct KzcStopwatch* stopwatch)
{
    kzUint current = kzsTimeGetCurrentTimestamp();
    return current - stopwatch->start_private;
}

kzUint kzcStopwatchGetDuration(const struct KzcStopwatch* stopwatch)
{
    kzUint duration = 0;
    if(stopwatch->state_private == KZC_STOPWATCH_STOPPED)
    {
        duration = stopwatch->duration_private;
    }
    else
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "StopwatchGetDuration called on running stopwatch. Stop the stopwatch first. Use StopwatchCurrentTime to query a running stopwatch.");
        kzsAssert(KZ_FALSE);
    }
    return duration;
}

kzUint kzcStopwatchSampleDeltaTime(struct KzcStopwatch* stopwatch)
{
    kzUint previous = stopwatch->deltaPrevious_private;
    stopwatch->deltaPrevious_private = kzsTimeGetCurrentTimestamp();
    return stopwatch->deltaPrevious_private - previous;
}
