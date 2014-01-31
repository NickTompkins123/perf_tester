/**
* \file
* Kanzi window, native portion for the Tegra2 (openKODE) platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_window_native.h"

#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/display/kzs_egl.h>


struct KzsWindow;


/** Platform specific surface variables. */
struct KzsWindowNative
{
    KDWindow* window;
};

kzsError kzsWindowNativeCreate(const struct KzsWindowProperties* windowPropertiesRequested, struct KzsDisplay* display, struct KzsWindowProperties* out_windowProperties, struct KzsWindowNative** out_windowNative)
{
    struct KzsWindowProperties windowProperties;
    struct KzsWindowNative* windowNative = kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");
    
    windowProperties = *windowPropertiesRequested;
    
    windowNative->window = KD_NULL;

    /* the KDWindow is initialized when creating the surface. */

    *out_windowProperties = windowProperties;
    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeCreateEmpty(struct KzsWindowNative** out_windowNative)
{
    struct KzsWindowNative* windowNative = kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");

    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeInitialize( struct KzsWindowNative* windowNative,struct KzsWindow* window)
{
    kzsSuccess();
}

kzsError kzsWindowNativeUpdate(const struct KzsWindow* window)
{
    /* TODO: implement. */
    
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
    kzsFree(windowNative);
    kzsSuccess();
}

KDWindow* kzsWindowNativeGetKDWindow(const struct KzsWindowNative* windowNative)
{
    KDWindow* window;

    if(windowNative->window == NULL)
    {
        window = NULL;
    }
    else
    {
        window = windowNative->window;
    }

    return windowNative->window;
}
