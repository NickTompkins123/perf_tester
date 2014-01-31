/**
* \file
* Kanzi window, native part for Maemo platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_WINDOW_NATIVE_H
#define KZS_WINDOW_NATIVE_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>
#include <system/display/kzs_window.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>


struct KzsWindowNative;

Display* kzsWindowNativeGetDisplay(const struct KzsWindowNative* windowNative);
Window kzsWindowNativeGetWindow(const struct KzsWindowNative* windowNative);

#endif
