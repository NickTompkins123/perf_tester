/**
* \file
* Allows suspending the execution of current thread.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <system/time/kzs_sleep.h>

#include <unistd.h>


void kzsSleep(kzUint milliseconds)
{
    usleep(milliseconds * 1000);
}
