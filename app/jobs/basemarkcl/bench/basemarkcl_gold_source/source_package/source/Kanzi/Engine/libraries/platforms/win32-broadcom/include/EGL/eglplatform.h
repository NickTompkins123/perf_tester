#ifndef __eglplatform_h_
#define __eglplatform_h_

/*
** Copyright (c) 2007-2009 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

/* Platform-specific types and definitions for egl.h
 * $Revision: 7244 $ on $Date: 2009-01-20 17:06:59 -0800 (Tue, 20 Jan 2009) $
 *
 * Adopters may modify khrplatform.h and this file to suit their platform.
 * You are encouraged to submit all modifications to the Khronos group so that
 * they can be included in future versions of this file.  Please submit changes
 * by sending them to the public Khronos Bugzilla (http://khronos.org/bugzilla)
 * by filing a bug against product "EGL" component "Registry".
 */

#include "../KHR/khrplatform.h"

/* Macros used in EGL function prototype declarations.
 *
 * EGLAPI return-type EGLAPIENTRY eglFunction(arguments);
 * typedef return-type (EXPAPIENTRYP PFNEGLFUNCTIONPROC) (arguments);
 *
 * On Windows, EGLAPIENTRY can be defined like APIENTRY.
 * On most other platforms, it should be empty.
 */

#ifndef EGLAPIENTRY
#define EGLAPIENTRY
#endif
#ifndef EGLAPIENTRYP
#define EGLAPIENTRYP EGLAPIENTRY *
#endif
#ifndef EGLAPI
#ifdef KHAPI
#define EGLAPI KHAPI
#else
#define EGLAPI extern
#endif
#endif

/* The types NativeDisplayType, NativeWindowType, and NativePixmapType
 * are aliases of window-system-dependent types, such as X Display * or
 * Windows Device Context. They must be defined in platform-specific
 * code below. The EGL-prefixed versions of Native*Type are the same
 * types, renamed in EGL 1.3 so all types in the API start with "EGL".
 */

/* Unix (tentative)
    #include <X headers>
    typedef Display *NativeDisplayType;
      - or maybe, if encoding "hostname:display.head"
    typedef const char *NativeWindowType;
        etc.
 */

typedef void *EGLNativeDisplayType;
typedef void *EGLNativePixmapType;
typedef void *EGLNativeWindowType;

#if defined ( WIN32 ) || defined ( __linux__ )
#define EGL_SERVER_SMALLINT  //win32 platform currently only supports this
#endif

#ifndef EGL_SERVER_SMALLINT
#ifdef _VIDEOCORE
#include "interface/vmcs_host/vc_dispmanx.h"
/* TODO: EGLNativeWindowType is really one of these but I'm leaving it
 * as void* for now, in case changing it would cause problems
 */
typedef struct {
   DISPMANX_ELEMENT_HANDLE_T element;
   int width;   /* This is necessary because dispmanx elements are not queriable. */
   int height;
} EGL_DISPMANX_WINDOW_T;
#else

#error Cannot work out what native window type should be

#endif //VIDEOCORE

#else //EGL_SEVER_SMALLINT
#define NATIVE_WINDOW_800_480    ((NativeWindowType)0)
#define NATIVE_WINDOW_640_480    ((NativeWindowType)1)
#define NATIVE_WINDOW_320_240    ((NativeWindowType)2)
#define NATIVE_WINDOW_240_320    ((NativeWindowType)3)
#define NATIVE_WINDOW_64_64      ((NativeWindowType)4)
#define NATIVE_WINDOW_400_480_A  ((NativeWindowType)5)
#define NATIVE_WINDOW_400_480_B  ((NativeWindowType)6)
#define NATIVE_WINDOW_512_512    ((NativeWindowType)7)
#define NATIVE_WINDOW_360_640    ((NativeWindowType)8)
#define NATIVE_WINDOW_640_360    ((NativeWindowType)9)
#define NATIVE_WINDOW_1280_720   ((NativeWindowType)10)
#define NATIVE_WINDOW_1920_1080  ((NativeWindowType)11)
#define NATIVE_WINDOW_480_320    ((NativeWindowType)12)

#define NATIVE_WINDOW_MAX        ((NativeWindowType)12)
#endif

/* EGL 1.2 types, renamed for consistency in EGL 1.3 */
typedef EGLNativeDisplayType NativeDisplayType;
typedef EGLNativePixmapType  NativePixmapType;
typedef EGLNativeWindowType  NativeWindowType;


/* Define EGLint. This must be a signed integral type large enough to contain
 * all legal attribute names and values passed into and out of EGL, whether
 * their type is boolean, bitmask, enumerant (symbolic constant), integer,
 * handle, or other.  While in general a 32-bit integer will suffice, if
 * handles are 64 bit types, then EGLint should be defined as a signed 64-bit
 * integer type.
 */
typedef khronos_int32_t EGLint;

#ifdef KHRONOS_NAME_MANGLING
#include "middleware/khronos/client/mangle.h"
#endif

#endif /* __eglplatform_h */