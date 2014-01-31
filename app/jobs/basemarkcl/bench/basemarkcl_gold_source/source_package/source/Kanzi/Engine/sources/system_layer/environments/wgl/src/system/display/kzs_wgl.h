/**
* \file
* WGL functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_WGL_H
#define KZS_WGL_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_opengl_base.h>

#include <windows.h>

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/wglext.h>
#endif

/* Forward declarations. */
struct KzsSurfaceProperties;


/** Initializes an WGL display. */
kzsError kzsWGLDisplayInitialize(HDC hdc, HGLRC* out_hrc, const struct KzsSurfaceProperties* surfaceProperties);

/** Makes anWGL surface active. */
kzsError kzsWGLMakeCurrent(HDC hdc, HGLRC hrc);

/** Updates theWGL surface on the screen. */
kzsError kzsWGLUpdateSurface(HDC hdc);

/** Sets the swap interval. */
kzsError kzsWGLSwapInterval(kzInt interval);

void kzsWGLGetUsedSurfaceProperties(HDC hdc, struct KzsSurfaceProperties* usedProperties);


#endif
