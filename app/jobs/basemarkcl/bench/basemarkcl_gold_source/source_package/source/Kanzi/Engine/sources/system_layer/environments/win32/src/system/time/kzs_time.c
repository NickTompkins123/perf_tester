/**
* \file
* Provides clock functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <system/time/kzs_time.h>

#include <windows.h>
#include <time.h>
#include <stdio.h>


struct KzsTime kzsTimeGetTime(void)
{
    struct KzsTime currentTime;
    struct tm systemTimeStruct;

    time_t currentSystemTime;
    time(&currentSystemTime);

    localtime_s(&systemTimeStruct, &currentSystemTime);

    currentTime.hours = systemTimeStruct.tm_hour;
    currentTime.minutes = systemTimeStruct.tm_min;
    currentTime.seconds = systemTimeStruct.tm_sec;

    currentTime.day = systemTimeStruct.tm_mday;
    currentTime.month = systemTimeStruct.tm_mon + 1;
    currentTime.year = systemTimeStruct.tm_year + 1900;

    return currentTime;
}

void kzsTimeSetTime(const struct KzsTime* time)
{
    SYSTEMTIME systemTime;

    systemTime.wDay = (WORD)(time->day); 
    systemTime.wDayOfWeek = 0;  /* ignored */
    systemTime.wMonth = (WORD)(time->month + 1);
    systemTime.wYear = (WORD)(time->year);
    
    /* TODO: time zone needs to be taken into account! */

    systemTime.wHour = (WORD)time->hours;
    systemTime.wMinute = (WORD)time->minutes;
    systemTime.wSecond = (WORD)time->seconds;
    systemTime.wMilliseconds = 0;

    SetSystemTime(&systemTime);
}
