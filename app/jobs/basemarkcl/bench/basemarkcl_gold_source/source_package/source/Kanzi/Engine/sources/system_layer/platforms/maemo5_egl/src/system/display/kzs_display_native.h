/**
* \file
* Kanzi display, native portion for the Maemo platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_DISPLAY_NATIVE_H
#define KZS_DISPLAY_NATIVE_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

struct KzsDisplayNative;

/** Return the X11 display. */
Display* kzsDisplayNativeGetX11Display(struct KzsDisplayNative* displayNative);

#endif
