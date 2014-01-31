/**
* \file
* Kanzi surface, native portion for the win32 platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_surface_native.h"

#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/display/kzs_display_native.h>
#include <system/display/kzs_window_native.h>
#include <system/display/kzs_egl.h>


/** Platform specific surface variables. */
struct KzsSurfaceNative 
{
    struct KzsDisplayNative* displayNative;  /**< Pointer to the display. */
    EGLDisplay eglDisplay;  /**< EGL display. Basically this is bound to the win32 window. */
    EGLSurface eglSurface;  /**< EGL surface. */
    EGLContext eglContext;  /**< EGL context. */
};


kzsError kzsSurfaceNativeCreate(struct KzsDisplayNative* displayNative, struct KzsWindowNative* windowNative, struct KzsSurfaceNative** out_surfaceNative)
{
    kzsError result;
    struct KzsSurfaceNative* surfaceNative = (struct KzsSurfaceNative*)kzsMalloc(sizeof(struct KzsSurfaceNative));
    kzsErrorTest(surfaceNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native surface.");

    surfaceNative->displayNative = displayNative;

    /* Initialize EGL display for the surface in here because in Windows the window is the display. */
    result = kzsEGLDisplayInitialize((NativeDisplayType)kzsWindowNativeGetHDC(windowNative), &surfaceNative->eglDisplay);
    kzsErrorForward(result);

    /* Set EGL attributes. */
    {
        /* TODO: Read some of these from somewhere. Also specify platform specific optimal/default values here. */
        EGLint eglConfigurationAttributeArray[128];
        int i = 0;
        eglConfigurationAttributeArray[i++] = EGL_RED_SIZE;
        eglConfigurationAttributeArray[i++] = 8;
        eglConfigurationAttributeArray[i++] = EGL_GREEN_SIZE;
        eglConfigurationAttributeArray[i++] = 8;
        eglConfigurationAttributeArray[i++] = EGL_BLUE_SIZE;
        eglConfigurationAttributeArray[i++] = 8;
        eglConfigurationAttributeArray[i++] = EGL_ALPHA_SIZE;
        eglConfigurationAttributeArray[i++] = 8;
        eglConfigurationAttributeArray[i++] = EGL_SURFACE_TYPE;
        eglConfigurationAttributeArray[i++] = EGL_WINDOW_BIT;

        eglConfigurationAttributeArray[i++] = EGL_NONE;

        result = kzsEGLSurfaceInitialize(surfaceNative->eglDisplay, kzsWindowNativeGetHandle(windowNative), eglConfigurationAttributeArray, &surfaceNative->eglSurface, &surfaceNative->eglContext);
        kzsErrorForward(result);
    }

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

    if(active)
    {
        result = kzsEGLMakeCurrent(surfaceNative->eglDisplay, surfaceNative->eglSurface, surfaceNative->eglSurface, surfaceNative->eglContext);
        kzsErrorForward(result);
    }
    else
    {
        result = kzsEGLMakeCurrent(surfaceNative->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        kzsErrorForward(result);
    }
    kzsSuccess();
}

kzsError kzsSurfaceNativeUpdate(struct KzsSurfaceNative* surfaceNative)
{
    kzsError result;
    result = kzsEGLUpdateSurface(surfaceNative->eglDisplay, surfaceNative->eglSurface);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzsSurfaceNativeReadPixels(struct KzsSurfaceNative* nativeSurface, void* out_pixels)
{
    kzsError result;

    result = kzsEGLCopyBuffers(nativeSurface->eglDisplay, nativeSurface->eglSurface, out_pixels);
    kzsErrorForward(result);

    kzsSuccess();
}
