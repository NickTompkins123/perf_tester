/**
* \file
* Kanzi window, native portion for the symbian platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_window_native.h"

#include <system/display/kzs_window_native_symbian.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/display/kzs_egl.h>


/* Forward declarations. */
struct KzsWindow;
struct KzsWindowNativeSymbian;


/** Platform specific window variables. */
struct KzsWindowNative
{
    struct KzsWindowNativeSymbian *symbianWindow;
};

kzsError kzsWindowNativeCreate(const struct KzsWindowProperties* windowPropertiesRequested, struct KzsDisplay* display, struct KzsWindowProperties* out_windowProperties, struct KzsWindowNative** out_windowNative)
{
    kzsError result;
    struct KzsWindowProperties windowProperties;
    struct KzsWindowNativeSymbian* windowNativeSymbian;
    struct KzsWindowNative* windowNative;
    
    windowProperties = *windowPropertiesRequested;
    
    result = kzsWindowNativeCreateEmpty(&windowNative);
    kzsErrorForward(result);       
    
    result = kzsWindowNativeSymbianCreate(&windowNativeSymbian);
    kzsErrorForward(result);
    
    windowNative->symbianWindow = windowNativeSymbian;

    *out_windowProperties = windowProperties;
    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeCreateEmpty(struct KzsWindowNative** out_windowNative)
{
    struct KzsWindowNative* windowNative = (struct KzsWindowNative*)kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");

    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeInitialize(struct KzsWindowNative* windowNative, struct KzsWindow* window)
{
    kzsSuccess();
}

kzsError kzsWindowInitialize_internal(struct KzsWindow* window, struct KzsWindowNative* windowNative)
{
    kzsSuccess();
}

kzsError kzsWindowNativeResize(const struct KzsWindowNative* windowNative, kzUint width, kzUint height)
{
    kzsSuccess();
}

kzsError kzsWindowNativeSetPositionAbsolute(const struct KzsWindowNative* windowNative, kzUint x, kzUint y)
{
    kzsSuccess();
}

kzsError kzsWindowNativeSetTitle(const struct KzsWindowNative* windowNative, kzString titleText)
{
    kzsSuccess();
}

kzsError kzsWindowNativeDelete(struct KzsWindowNative* windowNative)
{
    kzsWindowNativeSymbianDelete(windowNative->symbianWindow);
    kzsFree(windowNative);

    kzsSuccess();
}

void* kzsWindowNativeGetWindowHandle(struct KzsWindowNative* windowNative)
{
    return kzsWindowNativeSymbianGetWindowHandle(windowNative->symbianWindow);
}
