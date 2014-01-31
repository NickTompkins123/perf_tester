/**
* \file
* Provides clock functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_TIME_H
#define KZS_TIME_H


#include <system/kzs_types.h>


/**< Structure for time and date values. */
struct KzsTime
{
    kzUint hours;   /**< Hours 0-23 */
    kzUint minutes; /**< Minutes 0-59. */
    kzUint seconds; /**< Seconds 0-59. */
    kzUint day;     /**< Day of month. 1 is the first day. */
    kzUint month;   /**< Month of year. 1 is January. */
    kzUint year;    /**< Year. */
};

/** Returns the current time and date in a structure. */
struct KzsTime kzsTimeGetTime(void);

/** Sets the current time and date. */
void kzsTimeSetTime(const struct KzsTime* time);


#endif
