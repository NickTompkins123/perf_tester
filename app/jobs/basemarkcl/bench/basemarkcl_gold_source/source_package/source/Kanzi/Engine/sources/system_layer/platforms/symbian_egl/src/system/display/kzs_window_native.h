/**
* \file
* Kanzi window, native portion for the Symbian platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_WINDOW_NATIVE_H
#define KZS_WINDOW_NATIVE_H

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>
#include <system/display/kzs_window.h>


struct KzsWindowNative;

void* kzsWindowNativeGetWindowHandle(struct KzsWindowNative* windowNative);

#endif
