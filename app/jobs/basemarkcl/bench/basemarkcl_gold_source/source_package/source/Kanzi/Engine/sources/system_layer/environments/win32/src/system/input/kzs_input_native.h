/**
* \file
* Kanzi input, win32 specific implementation
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_INPUT_NATIVE_H
#define KZS_INPUT_NATIVE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include <windows.h>


/* Forward declaration structures. */
struct KzsInputPointingDevice;
struct KzsWindow;


/** Message handler for native input. */
kzsError kzsInputNativeHandleMessage(struct KzsWindow* window, HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam, kzBool* out_handled);


#endif
