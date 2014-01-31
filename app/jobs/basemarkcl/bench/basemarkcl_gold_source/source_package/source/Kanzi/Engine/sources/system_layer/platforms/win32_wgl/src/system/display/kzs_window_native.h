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


/** 
 * \struct KzsWindowNative Native window for Win32.
 */
struct KzsWindowNative;


/** Returns the win32 window handle. */
HWND kzsWindowNativeGetHandle(const struct KzsWindowNative* windowNative);

/** Sets the win32 window handle. */
void kzsWindowNativeSetHandle(struct KzsWindowNative* windowNative, HWND windowHandle);

/** Sets key state for window native. */
void kzsWindowNativeSetKeyState(struct KzsWindowNative* windowNative, kzUint keyCode, enum KzsInputKeyDeviceState state);

/** Returns the win32 device context handle. */
HDC kzsWindowNativeGetHDC(const struct KzsWindowNative* windowNative);

kzsError kzsWindowNativeWndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, kzBool* out_handled);


#endif
