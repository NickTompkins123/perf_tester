/**
* \file
* System memory functionality
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_memory.h"


kzInt g_allocationCount_private = 0;
kzInt g_allocatedMemory_private = 0;


kzInt kzsMemoryGetAllocationCount(void)
{
    return g_allocationCount_private;
}

void kzsMemoryResetAllocationCount(void)
{
    g_allocationCount_private = 0;
    g_allocatedMemory_private = 0;
}
