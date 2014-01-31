/**
* \file
* Kanzi surface, native portion for the win32 platform.
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
#include <system/display/kzs_wgl.h>
#include <system/wrappers/kzs_opengl.h> /* TODO: Remove. */


/** Platform specific surface variables. */
struct KzsSurfaceNative 
{
    struct KzsDisplayNative* displayNative;  /**< Pointer to the display. */
    HGLRC glContext; /**< Pointer to OpenGL context. */
    HDC hdc; /**< Handle to context device. */
};


kzsError kzsSurfaceNativeCreate(const struct KzsSurfaceProperties* surfaceProperties, struct KzsDisplayNative* displayNative, 
    struct KzsWindowNative* windowNative, struct KzsSurfaceNative** out_surfaceNative)
{
    kzsError result;
    struct KzsSurfaceNative* surfaceNative = (struct KzsSurfaceNative*)kzsMalloc(sizeof(struct KzsSurfaceNative));
    kzsErrorTest(surfaceNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native surface.");

    surfaceNative->displayNative = displayNative;
    surfaceNative->hdc = kzsWindowNativeGetHDC(windowNative);

    result = kzsWGLDisplayInitialize(surfaceNative->hdc, &surfaceNative->glContext, surfaceProperties);
    kzsErrorForward(result);

    *out_surfaceNative = surfaceNative;
    kzsSuccess();    
}

kzsError kzsSurfaceNativeDelete(struct KzsSurfaceNative* surfaceNative)
{
    kzsAssert(surfaceNative != KZ_NULL);

    kzsFree(surfaceNative);

    kzsSuccess();
}

kzsError kzsSurfaceNativeSetActive(struct KzsSurfaceNative* surfaceNative, kzBool active)
{
    kzsError result;
    kzsAssert(surfaceNative != KZ_NULL);

    if(active)
    {
        result = kzsWGLMakeCurrent(surfaceNative->hdc, surfaceNative->glContext);
        kzsErrorForward(result);
    }
    else
    {
        result = kzsWGLMakeCurrent(KZ_NULL, KZ_NULL);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzsSurfaceNativeUpdate(struct KzsSurfaceNative* surfaceNative)
{
    kzsError result;
    result = kzsWGLUpdateSurface(surfaceNative->hdc);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzsSurfaceNativeReadPixels(struct KzsSurfaceNative* nativeSurface, void* out_pixels)
{
    KZS_NOT_IMPLEMENTED_YET_ERROR;
}

void* kzsSurfaceNativeGetDisplay(struct KzsSurfaceNative* nativeSurface)
{
    KZS_NOT_IMPLEMENTED_YET;
    return KZ_NULL;
}

void* kzsSurfaceNativeGetContext(struct KzsSurfaceNative* nativeSurface)
{
    kzsAssert(nativeSurface != 0);
    return nativeSurface->glContext;
}

void kzsSurfaceGetUsedSurfaceNativeProperties(const struct KzsSurfaceNative* surfaceNative, struct KzsSurfaceProperties* surfaceProperties)
{
    kzsAssert(surfaceNative != 0);
    kzsWGLGetUsedSurfaceProperties(surfaceNative->hdc, surfaceProperties);
}
