/**
* \file
* OpenGL base header.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_OPENGL_BASE_H
#define KZS_OPENGL_BASE_H


#include <system/kzs_types.h>

#define KZ_OPENGL_ES_1_1    1 /**< OpenGL ES 1.1 version. */
#define KZ_OPENGL_ES_2_0    2 /**< OpenGL ES 2.0 version. */
#define KZ_OPENGL_2_1       3 /**< Desktop OpenGL 2.1 version. */
#define KZ_OPENGL_NONE      4 /**< No OpenGL version specified. */

#ifndef KZ_OPENGL_VERSION
#error KZ_OPENGL_VERSION undefined
#endif

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0 || KZ_OPENGL_VERSION == KZ_OPENGL_2_1
#define KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS
#endif

extern const kzInt KZS_OPENGL_BINARY_FORMAT;
extern const kzString KZS_OPENGL_BINARY_NAME;


/** Global variable for controlling if OpenGL calls are enabled. */
extern kzBool g_kzsEnableOpenGLCalls;


/** Macro to use for starting OpenGL calls to allow disabling them with a control flag. */
#define KZS_OPENGL_CALL_BEGIN \
    if(g_kzsEnableOpenGLCalls) { /*lint --e{904}*/

/** Macro to use for enging OpenGL calls to allow disabling them with a control flag. */
#define KZS_OPENGL_CALL_END \
    }

/** Macro to use for enging OpenGL calls with a return value to allow disabling them with a control flag. */
#define KZS_OPENGL_CALL_END_RETURN_VALUE(x) \
    } return x;


/** Sets OpenGL calls enabled / disabled. */
void kzsOpenGLCallsSetEnabled(kzBool enabled);
/** Checks if opengl calls are enabled. */
kzBool kzsOpenGLCallsIsEnabled(void);


#endif
