/**
* \file
* OpenGL base.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_opengl_base.h"


#define KZ_PLATFORM_IMG     1 /**< IMG platform. */
#define KZ_PLATFORM_AMD     2 /**< AMD platform. */
#define KZ_PLATFORM_TEGRA2  3 /**< Tegra2 platform. */
#define KZ_PLATFORM_NONE    4 /**< None platform. */


#ifndef KZ_OPENGL_PLATFORM
#define KZ_OPENGL_PLATFORM KZ_PLATFORM_NONE
#endif


#if KZ_OPENGL_PLATFORM == KZ_PLATFORM_IMG

/* TODO: Fix to refer headers. */
#include <GLES2/gl2ext.h>

const kzInt KZS_OPENGL_BINARY_FORMAT = GL_SGX_BINARY_IMG;
const kzString KZS_OPENGL_BINARY_NAME = "IMG";

#elif KZ_OPENGL_PLATFORM == KZ_PLATFORM_AMD

/* #include <GLES2/gl2amdext.h> */

/* TODO: Fetch from gl2amdext.h. */
const kzInt KZS_OPENGL_BINARY_FORMAT = 0x8740;
const kzString KZS_OPENGL_BINARY_NAME = "AMD";

#elif KZ_OPENGL_PLATFORM == KZ_PLATFORM_TEGRA2

/* #include <GLES2/gl2ext.h> */

const kzInt KZS_OPENGL_BINARY_FORMAT =  0x890B;
const kzString KZS_OPENGL_BINARY_NAME = "Tegra";

#else

const kzInt KZS_OPENGL_BINARY_FORMAT = 0;
const kzString KZS_OPENGL_BINARY_NAME = "";

#endif


kzBool g_kzsEnableOpenGLCalls = KZ_TRUE;


void kzsOpenGLCallsSetEnabled(kzBool enabled)
{
    g_kzsEnableOpenGLCalls = enabled;
}

kzBool kzsOpenGLCallsIsEnabled()
{
    return g_kzsEnableOpenGLCalls;
}
