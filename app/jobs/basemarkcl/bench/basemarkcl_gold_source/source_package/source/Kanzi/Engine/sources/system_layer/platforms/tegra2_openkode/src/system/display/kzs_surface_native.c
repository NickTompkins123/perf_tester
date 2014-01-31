/**
* \file
* Kanzi surface, native portion for the Tegra2 (openKODE) platform.
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
#include <system/display/kzs_egl.h>
#include <system/debug/kzs_log.h>

#include <stdio.h>

#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <KD/kd.h>
#include <KD/NV_display.h>

PFNGLCOVERAGEMASKNVPROC glCoverageMaskNV;
PFNGLCOVERAGEOPERATIONNVPROC glCoverageOperationNV;

/** Platform specific surface variables. */
struct KzsSurfaceNative 
{
    struct KzsDisplayNative* displayNative;  /**< Pointer to the display. */
    struct KzsWindowNative* windowNative;
    EGLDisplay eglDisplay;  /**< EGL display. */
    EGLSurface eglSurface;  /**< EGL surface. */
    EGLContext eglContext;  /**< EGL context. */
    EGLConfig eglConfiguration;  /**< EGL configuration. */
    NativeWindowType nativeWindow;
#ifdef EXTERNAL_DISPLAY_MANAGER    
    void* externalDisplayNative; /**< External display native. */
#endif
};

#ifndef EXTERNAL_DISPLAY_MANAGER
kzsError kzsSurfaceNativeCreate(const struct KzsSurfaceProperties* surfaceProperties, struct KzsDisplayNative* displayNative, struct KzsWindowNative* windowNative, struct KzsSurfaceNative** out_surfaceNative)
{
    kzsError result;
    struct KzsSurfaceNative* surfaceNative = (struct KzsSurfaceNative*)kzsMalloc(sizeof(struct KzsSurfaceNative));
    kzsErrorTest(surfaceNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native surface.");

    surfaceNative->displayNative = displayNative;
    surfaceNative->nativeWindow = 0;

    result = kzsEGLDisplayInitialize(EGL_DEFAULT_DISPLAY, &surfaceNative->eglDisplay);
    kzsErrorForward(result);

    /* Set EGL attributes. */
    {
        EGLint eglConfigurationAttributeArray[128];
        int i = 0;
        eglConfigurationAttributeArray[i++] = EGL_RED_SIZE;
        eglConfigurationAttributeArray[i++] = surfaceProperties->bitsColorR;
        eglConfigurationAttributeArray[i++] = EGL_GREEN_SIZE;
        eglConfigurationAttributeArray[i++] = surfaceProperties->bitsColorG;
        eglConfigurationAttributeArray[i++] = EGL_BLUE_SIZE;
        eglConfigurationAttributeArray[i++] = surfaceProperties->bitsColorB;
        eglConfigurationAttributeArray[i++] = EGL_ALPHA_SIZE;
        eglConfigurationAttributeArray[i++] = 0;
        eglConfigurationAttributeArray[i++] = EGL_SURFACE_TYPE;
        eglConfigurationAttributeArray[i++] = EGL_WINDOW_BIT;
        eglConfigurationAttributeArray[i++] = EGL_DEPTH_SIZE;
        eglConfigurationAttributeArray[i++] = surfaceProperties->bitsDepthBuffer;
        eglConfigurationAttributeArray[i++] = EGL_STENCIL_SIZE;
        eglConfigurationAttributeArray[i++] = surfaceProperties->bitsStencil;

        if(surfaceProperties->antiAliasing > 0)
        {
            /* note: using NV extensions */
            eglConfigurationAttributeArray[i++] = EGL_COVERAGE_BUFFERS_NV;
            eglConfigurationAttributeArray[i++] = 1;
            eglConfigurationAttributeArray[i++] = EGL_COVERAGE_SAMPLES_NV;
            eglConfigurationAttributeArray[i++] = surfaceProperties->antiAliasing;
        }

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0
        eglConfigurationAttributeArray[i++] = EGL_RENDERABLE_TYPE;
        eglConfigurationAttributeArray[i++] = EGL_OPENGL_ES2_BIT;
#endif

        /*eglConfigurationAttributeArray[i++] = EGL_RENDER_BUFFER;
        eglConfigurationAttributeArray[i++] = EGL_BACK_BUFFER;*/

        eglConfigurationAttributeArray[i++] = EGL_NONE;
        
        result = kzsEGLConfigure(surfaceNative->eglDisplay, eglConfigurationAttributeArray, &surfaceNative->eglConfiguration);
        kzsErrorForward(result);        
        
        /* display_create_window, g_nativeWindow = add to struct */
#ifndef EXTERNAL_DISPLAY_MANAGER
        /*if(surfaceProperties->externalSurfaceID == -1)*/
        {
            KDint windowSize[2]  = { 800, 480 };
            /*KDint windowOffset[2] = { 0, 0 };*/
            KDint kdresult;
            KDWindow* window = kzsWindowNativeGetKDWindow(windowNative);

            window = kdCreateWindow(surfaceNative->eglDisplay, surfaceNative->eglConfiguration, KD_NULL);
            kzsErrorTest((window != KD_NULL), KZS_ERROR_WINDOW_CREATION_FAILED, "Unable to init KD window.");

            kdresult = kdSetWindowPropertyiv(window, KD_WINDOWPROPERTY_SIZE, windowSize);
            kzsErrorTest((kdresult == 0), KZS_ERROR_WINDOW_CREATION_FAILED, "Unable to set KD window size.");

            /*kdresult = kdSetWindowPropertyiv(window, KD_WINDOWPROPERTY_DESKTOP_OFFSET_NV, windowOffset);
            kzsErrorTest((kdresult == 0), KZS_ERROR_WINDOW_CREATION_FAILED, "Unable to set KD desktop offset.");*/

            kdresult = kdRealizeWindow(window, &surfaceNative->nativeWindow);
            kzsErrorTest((kdresult == 0), KZS_ERROR_WINDOW_CREATION_FAILED, "Unable to realize KD window.");
        }
#endif
        /* NON-Kanzi specific. */
#ifdef EXTERNAL_DISPLAY_MANAGER
        {

            kzsLog(KZS_LOG_LEVEL_DETAIL, "display_create_window() start");
            display_create_window(surfaceNative->eglDisplay, surfaceNative->eglConfiguration, 800, 480, surfaceProperties->externalSurfaceID, &surfaceNative->nativeWindow, &surfaceNative->externalDisplayNative); 
            kzsLog(KZS_LOG_LEVEL_DETAIL, "display_create_window() end");
        }
#endif
        

        result = kzsEGLSurfaceInitialize(surfaceNative->eglDisplay, surfaceNative->nativeWindow, surfaceNative->eglConfiguration, &surfaceNative->eglSurface, &surfaceNative->eglContext);
        kzsErrorForward(result);
        
    }

    glCoverageMaskNV = (PFNGLCOVERAGEMASKNVPROC)kzsGLGetProcAddress("glCoverageMaskNV");
    glCoverageOperationNV = (PFNGLCOVERAGEOPERATIONNVPROC)kzsGLGetProcAddress("glCoverageOperationNV");

    if(glCoverageMaskNV)
    {
        glCoverageMaskNV(GL_TRUE);
        glCoverageOperationNV(GL_COVERAGE_AUTOMATIC_NV);
    }
    
    *out_surfaceNative = surfaceNative;
    kzsSuccess();    
}
#endif

kzsError kzsSurfaceNativeDelete(struct KzsSurfaceNative* surfaceNative)
{
    KDWindow* window;
    kzsAssert(surfaceNative != KZ_NULL);

    window = kzsWindowNativeGetKDWindow(surfaceNative->windowNative);
    kdDestroyWindow(window);
    
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
