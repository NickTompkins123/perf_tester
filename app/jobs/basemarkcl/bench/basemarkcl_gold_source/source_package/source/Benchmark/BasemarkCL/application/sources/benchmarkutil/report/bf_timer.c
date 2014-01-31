/**
* \file
* Benchmark framework high resolution timer.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "bf_timer.h"

#include <core/memory/kzc_memory_manager.h>

#ifdef WIN32
#include <windows.h>
#else
#include <stddef.h>
#include <sys/time.h>
#endif


struct BfTimer
{
    double startTimeInMicroSec; /**< Starting time in microseconds. */
#ifdef WIN32
    LARGE_INTEGER frequency; /**< Ticks per second. */
    LARGE_INTEGER startTime; /**< Starting time of high frequency timer. */
#else
    struct timeval startTime;
#endif
};


kzsError bfTimerCreate(const struct KzcMemoryManager* memoryManager, struct BfTimer** out_timer)
{
    kzsError result;
    struct BfTimer* timer;

    result = kzcMemoryAllocVariable(memoryManager, timer, "BfTimer");
    kzsErrorForward(result);

#ifdef WIN32
    QueryPerformanceFrequency(&timer->frequency);
    QueryPerformanceCounter(&timer->startTime);   
#else
    gettimeofday(&timer->startTime, NULL);
#endif

    *out_timer = timer;
    kzsSuccess();
}

kzsError bfTimerDelete(struct BfTimer* timer)
{
    kzsError result;

    result = kzcMemoryFreeVariable(timer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint bfTimerGetElapsedTimeInMilliSeconds(struct BfTimer* timer)
{
#if WIN32
    kzFloat timeMs;
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    timeMs = (time.QuadPart - timer->startTime.QuadPart) * 1000.0f/(kzFloat)(timer->frequency.QuadPart) + 0.5f;
#else
    kzUint timeMs;
    {
        struct timeval time;
        struct timeval dtime;
        gettimeofday(&time, NULL);
        timersub(&time, &timer->startTime, &dtime);
        timeMs = dtime.tv_sec*1000 + dtime.tv_usec/(suseconds_t)1000;
    }
#endif

    return (kzUint)timeMs;
}

kzUint bfTimerGetElapsedTimeInMicroSeconds(struct BfTimer* timer)
{
#if WIN32
    kzFloat timeMs;
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    timeMs = (time.QuadPart - timer->startTime.QuadPart) * 1000000.0f/(kzFloat)(timer->frequency.QuadPart) + 0.5f;
#else
    kzUint timeMs;
    {
        struct timeval time;
        struct timeval dtime;
        gettimeofday(&time, NULL);
        timersub(&time, &timer->startTime, &dtime);
        timeMs = dtime.tv_sec*1000000 + dtime.tv_usec;
    }
#endif

    return (kzUint)timeMs;
}
