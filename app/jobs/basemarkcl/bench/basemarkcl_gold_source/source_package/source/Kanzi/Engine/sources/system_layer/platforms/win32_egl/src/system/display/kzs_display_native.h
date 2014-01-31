/**
* \file
* Kanzi display, native portion for the win32 platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_DISPLAY_NATIVE_H
#define KZS_DISPLAY_NATIVE_H

#include <windows.h>

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
#include <gl/GL.h>
#endif

typedef HGLRC (WINAPI *KzsWglCreateContext_internal)(HDC hdc);
typedef BOOL (WINAPI *KzsWglMakeCurrent_internal)(HDC hdc, HGLRC hglrc);
typedef BOOL (WINAPI *KzsWglDeleteContext_internal)(HGLRC hglrc);
typedef int (WINAPI *KzsWglChoosePixelFormat_internal)(HDC hdc, const PIXELFORMATDESCRIPTOR *ppfd);
typedef BOOL (WINAPI *KzsWglSetPixelFormat_internal)(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR *ppfd);
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
typedef const GLubyte* (WINAPI *KzsWin32GlGetString_internal)(GLenum name);
#endif

#endif
