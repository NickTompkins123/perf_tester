/**
* \file
* Benchmark framework high resolution timer.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef BF_TIMER_H
#define BF_TIMER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;

/** Benchmark high resolution timer struct. */
struct BfTimer;


/** Creates timer. */
kzsError bfTimerCreate(const struct KzcMemoryManager* memoryManager, struct BfTimer** out_timer);
/** Frees timer. */
kzsError bfTimerDelete(struct BfTimer* timer);

/** Returns elapsed time in milliseconds. */
kzUint bfTimerGetElapsedTimeInMilliSeconds(struct BfTimer* timer);

/** Returns elapsed time in microseconds. */
kzUint bfTimerGetElapsedTimeInMicroSeconds(struct BfTimer* timer);


#endif
