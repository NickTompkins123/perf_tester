/**
* \file
* Kanzi window, native portion for the win32 platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_WINDOW_NATIVE_H
#define KZS_WINDOW_NATIVE_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>
#include <system/display/kzs_window.h>

#include <windows.h>


struct KzsWindowNative;


/** Returns the win32 window handle. */
HWND kzsWindowNativeGetHandle(const struct KzsWindowNative* windowNative);
/** Sets the win32 window handle. */
void kzsWindowNativeSetHandle(struct KzsWindowNative* windowNative, HWND windowHandle);

/** Returns the win32 device context handle. */
HDC kzsWindowNativeGetHDC(const struct KzsWindowNative* windowNative);

/** Handles the win32 messages. */
void kzsHandleMessages(struct KzsWindow* window, HACCEL hAccelTable);


#endif
