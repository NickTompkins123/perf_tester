/**
* \file
* Kanzi window, native part for Linux platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_WINDOW_NATIVE_H
#define KZS_WINDOW_NATIVE_H


#include <EGL/egl.h>

struct KzsWindowNative;

struct fbdev_window* kzsWindowNativeGetFramebuffer(struct KzsWindowNative* windowNative);

#endif
