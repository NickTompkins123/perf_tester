/**
* \file
* Provides relative time in millisecond from the system.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <system/time/kzs_tick.h>

#include <windows.h>


kzUint kzsTimeGetCurrentTimestamp()
{
    return (kzUint)timeGetTime();
}

kzUint kzsTimeGetTimestampAccuracy(void)
{
    kzUint time1, time2;
    time1 = time2 = kzsTimeGetCurrentTimestamp();

    while(time1 == time2)
    {
        time2 = kzsTimeGetCurrentTimestamp();
    }

    return (time2 - time1);
}
