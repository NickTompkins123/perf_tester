/**
* \file
* EGL functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_EGL_H
#define KZS_EGL_H


#include <system/wrappers/kzs_opengl_base.h>
#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_opengl_base.h>

#include <EGL/egl.h>


/** Initializes an EGL display. */
kzsError kzsEGLDisplayInitialize(NativeDisplayType displayType, EGLDisplay* out_eglDisplay);

/** Returns the number of available EGL configurations. */
kzInt kzsEGLGetNumConfigurations(EGLDisplay eglDisplay);

/** Returns all the available EGL configurations. Memory must have been previously allocated for the configuration array. */
kzsError kzsEGLGetAllConfigurations(EGLDisplay eglDisplay, kzInt numConfigurations, EGLConfig* out_configurations);

/** Returns all the EGL configurations that match eglConfigurationAttributeArray. Memory must have been previously allocated for the configuration array. */
kzsError kzsEGLGetMatchingConfigurations(EGLDisplay eglDisplay, EGLint *eglConfigurationAttributeArray, kzInt numConfigurations, EGLConfig* out_configurations, kzInt *numReturnedConfigurations);

/** Returns the optimal EGL configuration for the platform. */
kzsError kzsEGLConfigure(EGLDisplay eglDisplay, EGLint* eglConfigurationAttributeArray, EGLConfig *out_eglConfiguration);

/** Initializes an EGL surface with given configuration. */
kzsError kzsEGLSurfaceInitialize(EGLDisplay eglDisplay, NativeWindowType eglWindow, EGLConfig* eglConfiguration, EGLSurface* out_eglSurface, EGLContext* out_eglContext);

/** Makes an EGL surface active. */
kzsError kzsEGLMakeCurrent(EGLDisplay eglDisplay, EGLSurface eglSurfaceRead, EGLSurface eglSurfaceWrite, EGLContext eglContext);

/** Updates the EGL surface on the screen. */
kzsError kzsEGLUpdateSurface(EGLDisplay eglDisplay, EGLSurface eglSurface);

/** Copy buffers. */
kzsError kzsEGLCopyBuffers(EGLDisplay eglDisplay, EGLSurface eglSurface, void* target);

/** Sets the swap interval. */
kzsError kzsEGLSwapInterval(EGLDisplay eglDisplay, EGLint interval);


#endif
