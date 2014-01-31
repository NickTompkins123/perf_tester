/**
* \file
* Kanzi window, native part for Tegra2 platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_WINDOW_NATIVE_H
#define KZS_WINDOW_NATIVE_H

#include <X11/Xlib.h>

struct KzsWindowNative;


/** Get the X11 Display object */
Display* kzsWindowNativeGetDisplay(const struct KzsWindowNative* windowNative);

/** Get the X11 Window object */
Window kzsWindowNativeGetWindow(const struct KzsWindowNative* windowNative);

#endif
