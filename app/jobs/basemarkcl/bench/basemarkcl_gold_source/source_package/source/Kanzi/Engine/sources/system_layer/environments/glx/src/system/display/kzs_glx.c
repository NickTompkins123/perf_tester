/**
* \file
* GLX functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_glx.h"

#include <system/display/kzs_window.h>
#include <system/kzs_opengl_extensions.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_string.h>
#include <system/debug/kzs_log.h>
#include <system/kzs_error_codes.h>
#include <system/wrappers/kzs_opengl.h> /* TODO: Remove. */
#include <system/wrappers/kzs_opengl_base.h>

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE

kzBool kzsGLXExtensionSupported_internal(kzString extensionString)
{
    return KZ_TRUE;
}

kzsError kzsGLXContextInitialize(Display* display, XVisualInfo* visualInfo, GLXContext* out_glxContext)
{
    GLXContext glxContext;
    
    glxContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
    
    *out_glxContext = glxContext;
    kzsSuccess();
}

kzsError kzsGLXUpdateSurface(Display* display, Window window)
{
    glXSwapBuffers(display, window);
    kzsSuccess();
}

kzsError kzsGLXMakeCurrent(Display* display, Window window, GLXContext context)
{
    glXMakeCurrent(display, window, context);
    kzsSuccess();
}

kzsError kzsGLXSwapInterval(kzInt interval)
{
    /* TODO ? */
    kzsSuccess();
}

void (*kzsGLGetProcAddress(kzString functionName))(void)
{
    return NULL;//(void(*)(void))glxGetProcAddress(functionName);
}
#endif
