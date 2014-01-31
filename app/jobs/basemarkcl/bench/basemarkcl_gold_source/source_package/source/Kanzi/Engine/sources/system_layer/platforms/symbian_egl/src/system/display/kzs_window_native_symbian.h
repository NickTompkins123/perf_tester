/**
* \file
* Kanzi window, native portion for the Symbian platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_WINDOW_NATIVE_SYMBIAN_H
#define KZS_WINDOW_NATIVE_SYMBIAN_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>

#ifdef __cplusplus
extern "C"
#endif
kzsError kzsWindowNativeSymbianCreate(struct KzsWindowNativeSymbian** out_windowNativeSymbian);

#ifdef __cplusplus
extern "C"
#endif
kzsError kzsWindowNativeSymbianDelete(struct KzsWindowNativeSymbian* windowNativeSymbian);

#ifdef __cplusplus
extern "C"
#endif
void* kzsWindowNativeSymbianGetWindowHandle(struct KzsWindowNativeSymbian* windowNativeSymbian);

#endif
