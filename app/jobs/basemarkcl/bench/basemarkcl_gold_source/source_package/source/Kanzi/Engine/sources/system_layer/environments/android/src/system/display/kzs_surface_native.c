/**
* \file
* Kanzi surface, native portion for the Android platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_surface_native.h"

#include <system/display/kzs_window.h>
#include <system/display/kzs_surface.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/display/kzs_display_native.h>
#include <system/display/kzs_window_native.h>
#include <system/debug/kzs_log.h>


/** Platform specific surface variables. */
struct KzsSurfaceNative 
{
    struct KzsDisplayNative*    displayNative;  /**< Pointer to the display. */
};

kzsError kzsSurfaceNativeCreate(const struct KzsSurfaceProperties* surfaceProperties, struct KzsDisplayNative* displayNative, struct KzsWindowNative* windowNative, struct KzsSurfaceNative** out_surfaceNative)
{
    kzsError result;
    struct KzsSurfaceNative* surfaceNative = kzsMalloc(sizeof(struct KzsSurfaceNative));

    surfaceNative->displayNative = displayNative;

    /* The surface has already been created in the Java code */
    
    *out_surfaceNative = surfaceNative;
    kzsSuccess();    
}

kzsError kzsSurfaceNativeDelete(struct KzsSurfaceNative* surfaceNative)
{
    kzsFree(surfaceNative);

    kzsSuccess();
}

kzsError kzsSurfaceNativeSetActive(struct KzsSurfaceNative* surfaceNative, kzBool active)
{
    kzsError result;
    kzsAssert(surfaceNative != KZ_NULL);

    kzsSuccess();
}

kzsError kzsSurfaceNativeUpdate(struct KzsSurfaceNative* surfaceNative)
{
    kzsError result;

    kzsSuccess();
}

kzsError kzsSurfaceNativeReadPixels(struct KzsSurfaceNative* nativeSurface, void* out_pixels)
{
    kzsError result;

    kzsSuccess();
}

void* kzsSurfaceNativeGetDisplay(struct KzsSurfaceNative* nativeSurface)
{
    return KZ_NULL;
}

void* kzsSurfaceNativeGetContext(struct KzsSurfaceNative* nativeSurface)
{
    return KZ_NULL;
}

void kzsSurfaceGetUsedSurfaceNativeProperties(const struct KzsSurfaceNative* surfaceNative, struct KzsSurfaceProperties* surfaceProperties)
{
    kzsAssert(surfaceNative != 0);

    /* TODO: fetch from native window. */
    surfaceProperties->antiAliasing = 0;
    surfaceProperties->bitsAlpha = 0;
    surfaceProperties->bitsColorB = 0;
    surfaceProperties->bitsColorR = 0;
    surfaceProperties->bitsColorG = 0;
    surfaceProperties->bitsDepthBuffer = 0;
    surfaceProperties->bitsStencil = 0;
    surfaceProperties->externalSurfaceID = 0;
}
