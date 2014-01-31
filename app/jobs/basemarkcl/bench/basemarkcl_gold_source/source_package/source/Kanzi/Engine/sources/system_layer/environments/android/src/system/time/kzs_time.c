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
    struct tm systemTimeStruct;

    time_t currentSystemTime;
    time(&currentSystemTime);

    systemTimeStruct = *localtime(&currentSystemTime);

    currentTime.hours = systemTimeStruct.tm_hour;
    currentTime.minutes = systemTimeStruct.tm_min;
    currentTime.seconds = systemTimeStruct.tm_sec;

    currentTime.day = systemTimeStruct.tm_mday;
    currentTime.month = systemTimeStruct.tm_mon;
    currentTime.year = systemTimeStruct.tm_year + 1900;

    return currentTime;
}

void kzsTimeSetTime(const struct KzsTime* time)
{
    struct tm systemTimeStruct;
    time_t systemTime;

    systemTimeStruct.tm_hour = time->hours;
    systemTimeStruct.tm_min = time->minutes;
    systemTimeStruct.tm_sec = time->seconds;

    systemTimeStruct.tm_mday = time->day + 1;
    systemTimeStruct.tm_mon = time->month;
    systemTimeStruct.tm_year = time->year - 1900;

    systemTimeStruct.tm_isdst = 0;

    systemTime = mktime(&systemTimeStruct);

    if(systemTime != -1)
    {
        struct timeval timeValue;
        timeValue.tv_sec = systemTime;
        timeValue.tv_usec = 0;

        settimeofday(&timeValue, NULL);
    }        
}


