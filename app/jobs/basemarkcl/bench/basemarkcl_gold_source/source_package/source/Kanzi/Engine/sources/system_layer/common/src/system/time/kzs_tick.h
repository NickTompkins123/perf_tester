/**
* \file
* Provides relative time in millisecond from the system.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_TICK_H
#define KZS_TICK_H


#include <system/kzs_types.h>




/**
 * Returns current time from the system in milliseconds.
 * Values should be only used relative to each other because starting time is not specified.
 */
kzUint kzsTimeGetCurrentTimestamp(void);

/** 
* Returns the accuracy of timestamp received with the above function call.
* This returns the minimum difference in there can be between two timestamp calls.
* The call is blocking and can take even 20 milliseconds to complete.
*/
kzUint kzsTimeGetTimestampAccuracy(void);


#endif
