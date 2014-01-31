/**
* \file
* OpenVG base header.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_OPENVG_BASE_H
#define KZS_OPENVG_BASE_H


#include <system/kzs_types.h>

/** Global variable for controlling if OpenVG calls are enabled. */
extern kzBool g_kzsEnableOpenVGCalls;


/** Macro to use for starting OpenVG calls to allow disabling them with a control flag. */
#define KZS_OPENVG_CALL_BEGIN \
    if(g_kzsEnableOpenVGCalls) { /*lint -e{904}*/

/** Macro to use for enging OpenVG calls to allow disabling them with a control flag. */
#define KZS_OPENVG_CALL_END \
    }

/** Macro to use for enging OpenVG calls with a return value to allow disabling them with a control flag. */
#define KZS_OPENVG_CALL_END_RETURN_VALUE(x) \
    } return x;


/** Sets OpenVG calls enabled / disabled. */
void kzsOpenVGCallsSetEnabled(kzBool enabled);


#endif
