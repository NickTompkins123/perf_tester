/**
* \file
* EGL functions. Tweaked to work with the OpenVG RI mini-EGL implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>

#include <system/display/kzs_egl.h>


kzsError kzsEGLDisplayInitialize(NativeDisplayType displayType, EGLDisplay* out_eglDisplay)
{
    EGLDisplay eglDisplay;

    eglDisplay = eglGetDisplay(displayType);
    kzsErrorTest((eglDisplay != EGL_NO_DISPLAY), KZS_ERROR_DISPLAY_INITIALIZATION_FAILED, "Unable to init EGL display.");

    {
        /* Got a valid display, now init EGL. */
        EGLint iMajorVersion, iMinorVersion;
        EGLBoolean initializeResult = eglInitialize(eglDisplay, &iMajorVersion, &iMinorVersion);
        kzsErrorTest(initializeResult == EGL_TRUE, KZS_ERROR_ILLEGAL_ARGUMENT, "EGL initialization failed.");
    }

    *out_eglDisplay = eglDisplay;
    kzsSuccess();
}

kzsError kzsEGLSurfaceInitialize(EGLDisplay eglDisplay, NativeWindowType eglWindow, EGLint* eglConfigurationAttributeArray, EGLSurface* out_eglSurface, EGLContext* out_eglContext)
{
    EGLConfig  eglConfig = 0;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    EGLContext eglContext = EGL_NO_CONTEXT;

    int configurationCount;

    /* TODO: would it be better to query all configurations and then choose from the list..? */
    /*eglChooseConfig(eglDisplay, eglConfigurationAttributeArray, KZ_NULL, 0, &numConfigurations);
    configurationList = kzsMalloc(numConfigurations *  sizeof(EGLConfig));*/


    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(eglDisplay, NULL, NULL);
    kzsErrorTest(eglGetError() == EGL_SUCCESS, KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Unable to get an EGL configuration.");
    eglBindAPI(EGL_OPENVG_API);

    eglChooseConfig(eglDisplay, eglConfigurationAttributeArray, &eglConfig, 1, &configurationCount);
    kzsErrorTest(eglGetError() == EGL_SUCCESS, KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Unable to get an EGL configuration.");

    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglWindow, NULL);
    kzsErrorTest(eglGetError() == EGL_SUCCESS, KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Unable to create EGL window surface.");
    eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);
    kzsErrorTest(eglGetError() == EGL_SUCCESS, KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Unable to create EGL context.");
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    kzsErrorTest(eglGetError() == EGL_SUCCESS, KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Unable to make EGL context current.");

    *out_eglSurface = eglSurface;
    *out_eglContext = eglContext;
    kzsSuccess();
}

kzsError kzsEGLUpdateSurface(EGLDisplay eglDisplay, EGLSurface eglSurface)
{
    EGLBoolean result;

    result = eglSwapBuffers(eglDisplay, eglSurface);
    kzsErrorTest((result != EGL_FALSE), KZS_ERROR_SURFACE_UPDATE_FAILED, "Unable to update EGL surface.");

    kzsSuccess();
}

kzsError kzsEGLMakeCurrent(EGLDisplay eglDisplay, EGLSurface eglSurfaceRead, EGLSurface eglSurfaceWrite, EGLContext eglContext)
{
    EGLBoolean result;

    result = eglMakeCurrent(eglDisplay, eglSurfaceRead, eglSurfaceWrite, eglContext);
    kzsErrorTest((result != EGL_FALSE), KZS_ERROR_SURFACE_ACTIVATION_FAILED, "Unable to activate EGL surface.");

    kzsSuccess();
}

kzsError kzsEGLCopyBuffers(EGLDisplay eglDisplay, EGLSurface eglSurface, void* target)
{
    EGLBoolean result;
    result = eglCopyBuffers(eglDisplay, eglSurface, target);
#if 0
    if(result != EGL_TRUE)
    {
        EGLint error = eglGetError();
        kzsErrorThrow(KZS_ERROR_ASSERTION_FAILED, "Error on eglCopyBuffers.");
    }
#endif
    kzsSuccess();
}

void (*kzsGLGetProcAddress(kzString functionName))(void)
{
    return eglGetProcAddress(functionName);
}
