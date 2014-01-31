/**
* \file
* Display.
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

struct KzsSurfaceNative 
{
    struct KzsDisplayNative* displayNative;  /**< Pointer to the display. */
    EGLDisplay eglDisplay;  /**< EGL display. */
    EGLSurface eglSurface;  /**< EGL surface. */
    EGLContext eglContext;  /**< EGL context. */
    EGLConfig eglConfiguration;  /**< EGL configuration. */
};

kzsError kzsSurfaceNativeCreate(struct KzsDisplayNative* displayNative, struct KzsWindowNative* windowNative, struct KzsSurfaceNative** out_surfaceNative)
{
    kzsError result;
    struct KzsSurfaceNative* surfaceNative = kzsMalloc(sizeof(struct KzsSurfaceNative));
    EGLint eglConfigurationAttributeArray[128];

    surfaceNative->displayNative = displayNative;

    /* Initialize EGL display for the surface. Use EGL_DEFAULT_DISPLAY because there are no other displays. */
    result = kzsEGLDisplayInitialize(EGL_DEFAULT_DISPLAY, &surfaceNative->eglDisplay);
    kzsErrorForward(result);

    /* Set EGL attributes. */
    {
        /* TODO: Read some of these from somewhere. Also specify platform specific optimal/default values here. */
        int i = 0;
        eglConfigurationAttributeArray[i++] = EGL_RED_SIZE;
        eglConfigurationAttributeArray[i++] = 5;
        eglConfigurationAttributeArray[i++] = EGL_GREEN_SIZE;
        eglConfigurationAttributeArray[i++] = 6;
        eglConfigurationAttributeArray[i++] = EGL_BLUE_SIZE;
        eglConfigurationAttributeArray[i++] = 5;
        eglConfigurationAttributeArray[i++] = EGL_ALPHA_SIZE;
        eglConfigurationAttributeArray[i++] = 0;
        eglConfigurationAttributeArray[i++] = EGL_SURFACE_TYPE;
        eglConfigurationAttributeArray[i++] = EGL_WINDOW_BIT;
        eglConfigurationAttributeArray[i++] = EGL_DEPTH_SIZE;
        eglConfigurationAttributeArray[i++] = 16;
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0
        eglConfigurationAttributeArray[i++] = EGL_RENDERABLE_TYPE;
        eglConfigurationAttributeArray[i++] = EGL_OPENGL_ES2_BIT;
        eglConfigurationAttributeArray[i++] = EGL_SAMPLES;
        eglConfigurationAttributeArray[i++] = 0;
#endif
        eglConfigurationAttributeArray[i++] = EGL_NONE;
    }

    result = kzsEGLConfigure(surfaceNative->eglDisplay, eglConfigurationAttributeArray, &surfaceNative->eglConfiguration);
    kzsErrorForward(result);
    
    result = kzsEGLSurfaceInitialize(surfaceNative->eglDisplay, 0, surfaceNative->eglConfiguration, &surfaceNative->eglSurface, &surfaceNative->eglContext);
    kzsErrorForward(result);

    *out_surfaceNative = surfaceNative;
    kzsSuccess();    
}

kzsError kzsSurfaceNativeDelete(struct KzsSurfaceNative* surfaceNative)
{
    kzsFree(surfaceNative);

    kzsSuccess();
}

kzsError kzsSurfaceNativeSetActive(struct KzsSurfaceNative* surfaceNative)
{
    kzsEGLMakeCurrent(surfaceNative->eglDisplay, surfaceNative->eglSurface, surfaceNative->eglSurface, surfaceNative->eglContext);
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
