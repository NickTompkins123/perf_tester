/**
* \file
* Kanzi surface, native portion for the Maemo platform.
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
#include <system/wrappers/kzs_opengl_base.h>
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
#include <system/display/kzs_glx.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif

#include <stdio.h>

#if KZ_OPENGL_VERSION == KZ_OPENGL_NONE
#define GLXContext int
#endif

/** Platform specific surface variables. */
struct KzsSurfaceNative 
{
    struct KzsDisplayNative*    displayNative;  /**< Pointer to the display. */
    GLXContext                  glxContext;
    Display* glxDisplay;
    Window glxWindow;
};

kzsError kzsSurfaceNativeCreate(const struct KzsSurfaceProperties* surfaceProperties, struct KzsDisplayNative* displayNative, struct KzsWindowNative* windowNative, struct KzsSurfaceNative** out_surfaceNative)
{
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    kzsError result;
    struct KzsSurfaceNative* surfaceNative = kzsMalloc(sizeof(struct KzsSurfaceNative));
    kzsErrorTest(surfaceNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native surface.");

    surfaceNative->displayNative = displayNative;
    
    surfaceNative->glxDisplay = kzsWindowNativeGetDisplay(windowNative);
    surfaceNative->glxWindow = kzsWindowNativeGetWindow(windowNative);
    
   
    kzsGLXContextInitialize(kzsDisplayNativeGetX11Display(displayNative), kzsWindowNativeGetVisualInfo(windowNative), &surfaceNative->glxContext);
    kzsSurfaceNativeSetActive(surfaceNative, KZ_TRUE);
    
    *out_surfaceNative = surfaceNative;
#endif
    
    kzsSuccess();    
}

kzsError kzsSurfaceNativeDelete(struct KzsSurfaceNative* surfaceNative)
{
    kzsFree(surfaceNative);

    kzsSuccess();
}

kzsError kzsSurfaceNativeSetActive(struct KzsSurfaceNative* surfaceNative, kzBool active)
{
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    kzsError result;
    kzsAssert(surfaceNative != KZ_NULL);

    if(active)
    {
        result = kzsGLXMakeCurrent(surfaceNative->glxDisplay, surfaceNative->glxWindow, surfaceNative->glxContext);
        kzsErrorForward(result);
    }
    else
    {
        result = kzsGLXMakeCurrent(surfaceNative->glxDisplay, None, NULL);
        kzsErrorForward(result);
    }
#endif
    kzsSuccess();
}

kzsError kzsSurfaceNativeUpdate(struct KzsSurfaceNative* surfaceNative)
{
    kzsError result;
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    result = kzsGLXUpdateSurface(surfaceNative->glxDisplay, surfaceNative->glxWindow);
    kzsErrorForward(result);
#endif
    kzsSuccess();
}

kzsError kzsSurfaceNativeReadPixels(struct KzsSurfaceNative* nativeSurface, void* out_pixels)
{

    kzsSuccess();
}

void kzsSurfaceGetUsedSurfaceNativeProperties(const struct KzsSurfaceNative* surfaceNative, struct KzsSurfaceProperties* surfaceProperties)
{
    kzsAssert(surfaceNative != 0);
}

void* kzsSurfaceNativeGetContext(struct KzsSurfaceNative* nativeSurface)
{
    kzsAssert(nativeSurface != 0);
    return nativeSurface->glxContext;
}
