/**
* \file
* GLX functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_GLX_H
#define KZS_GLX_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_opengl_base.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
#include <GL/gl.h>
#include <GL/glx.h>
#else
#include <system/wrappers/kzs_opengl.h>
#endif


#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
/** Initializes the GLX context. */
kzsError kzsGLXContextInitialize(Display* display, XVisualInfo *visualInfo, GLXContext* out_glxContext);

/** Makes the GLX surface active. */
kzsError kzsGLXMakeCurrent(Display* display, Window window, GLXContext context);

/** Updates the GLX surface on the screen. */
kzsError kzsGLXUpdateSurface(Display* display, Window window);

/** Sets the swap interval. */
kzsError kzsGLXSwapInterval(kzInt interval);
#endif

#endif
