/**
* \file
* Provides clock functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <system/time/kzs_time.h>

#include <sys/time.h>
#include <time.h>

struct KzsTime kzsTimeGetTime(void)
{
    struct KzsTime currentTime;
    
    /* TODO: symbian implementation for getting current time */

    currentTime.hours = 1;
    currentTime.minutes = 1;
    currentTime.seconds = 1;

    currentTime.day = 1;
    currentTime.month = 1;
    currentTime.year = 2009;

    return currentTime;
}

void kzsTimeSetTime(const struct KzsTime* time)
{
    /* TODO: symbian implementation for setting time. */
}


