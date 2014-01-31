/**
* \file
* OpenVG base.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_openvg_base.h"


kzBool g_kzsEnableOpenVGCalls = KZ_TRUE;


void kzsOpenVGCallsSetEnabled(kzBool enabled)
{
    g_kzsEnableOpenVGCalls = enabled;
}
