/**
* \file
* Provides relative time in millisecond from the system.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <system/time/kzs_tick.h>

#include <sys/time.h>
#include <time.h>


kzUint kzsTimeGetCurrentTimestamp()
{
    struct timeval timeInformation;
    kzUint milliSeconds;
    
    gettimeofday(&timeInformation, NULL);

    /* Combine microseconds and seconds into milliseconds. */
    milliSeconds = (timeInformation.tv_usec / 1000) + (timeInformation.tv_sec * 1000);

    return milliSeconds;
}


