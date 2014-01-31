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
#include <system/display/kzs_egl.h>

#include <EGL/eglplatform.h>

/** Platform specific surface variables. */
struct KzsSurfaceNative 
{
    struct KzsDisplayNative* displayNative;  /**< Pointer to the display. */
    EGLDisplay eglDisplay;  /**< EGL display. Basically this is bound to the win32 window. */
    EGLSurface eglSurface;  /**< EGL surface. */
    EGLContext eglContext;  /**< EGL context. */
};


static EGLConfig    g_EGLConfig;

static NativeWindowType hWnd = NATIVE_WINDOW_800_480;

kzsError kzsSurfaceNativeCreate(const struct KzsSurfaceProperties* surfaceProperties, struct KzsDisplayNative* displayNative, struct KzsWindowNative* windowNative, struct KzsSurfaceNative** out_surfaceNative)
{
    kzsError result;

    EGLint width  = 800;
    EGLint height = 480;

    /* EGL Setup */
    const EGLint s_configAttribs[] =
    {
        EGL_RED_SIZE,       5,
        EGL_GREEN_SIZE,     6,
        EGL_BLUE_SIZE,      5,
        EGL_ALPHA_SIZE,     0,
        EGL_DEPTH_SIZE,     16,
        EGL_STENCIL_SIZE,   0,
        EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,
        EGL_NONE
    };

    const float vertices[] =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };

    EGLint numConfigs;
    EGLint majorVersion;
    EGLint minorVersion;

    struct KzsSurfaceNative* surfaceNative = (struct KzsSurfaceNative*)kzsMalloc(sizeof(struct KzsSurfaceNative));
    kzsErrorTest(surfaceNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native surface.");

    surfaceNative->displayNative = displayNative;

#if 0
    /* Initialize EGL display for the surface in here because in Windows the window is the display. */
    result = kzsEGLDisplayInitialize((NativeDisplayType)kzsWindowNativeGetHDC(windowNative), &surfaceNative->eglDisplay);
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

        if(surfaceProperties->antiAliasing > 0)
        {
            eglConfigurationAttributeArray[i++] = EGL_SAMPLES;
            eglConfigurationAttributeArray[i++] = surfaceProperties->antiAliasing;
        }

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0
        /* TODO: stencils should work on ES1 too.. verify */
        eglConfigurationAttributeArray[i++] = EGL_STENCIL_SIZE;
        eglConfigurationAttributeArray[i++] = surfaceProperties->bitsStencil;
        eglConfigurationAttributeArray[i++] = EGL_RENDERABLE_TYPE;
        eglConfigurationAttributeArray[i++] = EGL_OPENGL_ES2_BIT;
#endif
        eglConfigurationAttributeArray[i++] = EGL_NONE;

        result = kzsEGLSurfaceInitialize(surfaceNative->eglDisplay, kzsWindowNativeGetHandle(windowNative), eglConfigurationAttributeArray, &surfaceNative->eglSurface, &surfaceNative->eglContext);
        kzsErrorForward(result);
    }
#endif



    
    surfaceNative->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(surfaceNative->eglDisplay, &majorVersion, &minorVersion);

    eglGetConfigs(surfaceNative->eglDisplay, NULL, 0, &numConfigs);
    eglChooseConfig(surfaceNative->eglDisplay, s_configAttribs, &g_EGLConfig, 1, &numConfigs);

    surfaceNative->eglContext = eglCreateContext(surfaceNative->eglDisplay, g_EGLConfig, NULL, NULL);

    surfaceNative->eglSurface= eglCreateWindowSurface(surfaceNative->eglDisplay, g_EGLConfig, hWnd, NULL);

    eglMakeCurrent(surfaceNative->eglDisplay, surfaceNative->eglSurface, surfaceNative->eglSurface, surfaceNative->eglContext);

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
        eglMakeCurrent(surfaceNative->eglDisplay, surfaceNative->eglSurface, surfaceNative->eglSurface, surfaceNative->eglContext);
    }
    else
    {
        eglMakeCurrent(surfaceNative->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    kzsSuccess();
}

kzsError kzsSurfaceNativeUpdate(struct KzsSurfaceNative* surfaceNative)
{
    eglSwapBuffers(surfaceNative->eglDisplay, surfaceNative->eglSurface);

    kzsSuccess();
}

kzsError kzsSurfaceNativeReadPixels(struct KzsSurfaceNative* nativeSurface, void* out_pixels)
{
    eglCopyBuffers(nativeSurface->eglDisplay, nativeSurface->eglSurface, out_pixels);

    kzsSuccess();
}
