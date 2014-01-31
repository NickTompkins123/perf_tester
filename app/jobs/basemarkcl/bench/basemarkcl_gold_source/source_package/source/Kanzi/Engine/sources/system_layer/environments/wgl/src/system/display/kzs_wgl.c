/**
* \file
* WGL functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_wgl.h"

#include <system/display/kzs_window.h>
#include <system/kzs_opengl_extensions.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_string.h>
#include <system/debug/kzs_log.h>
#include <system/kzs_error_codes.h>
#include <system/wrappers/kzs_opengl.h> /* TODO: Remove. */
#include <system/display/kzs_surface.h>


kzBool kzsWGLExtensionSupported_internal(kzString extensionString)
{
    typedef kzString (WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC)();
    PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = NULL;

    wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");

    if (kzsStrstr(wglGetExtensionsStringEXT(), extensionString) == NULL)
    {
        return KZ_FALSE;
    }

    return KZ_TRUE;
}

kzsError kzsWGLDisplayInitialize(HDC hdc, HGLRC* out_hrc, const struct KzsSurfaceProperties* surfaceProperties)
{
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
/* TODO: Make a real platform for windows with no OpenGL instead of ifdefs */
    kzsError result;
    WNDCLASSEX dummyWindowClass;
    HWND dummyWindow;
    HDC hDC;
    HGLRC glContext;
    BOOL wglResult;
    PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
    BOOL (WINAPI* kzsWglChoosePixelFormatARB)(HDC hdc, const kzInt *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, kzInt *piFormats, UINT *nNumFormats);
    kzInt (WINAPI *kzsWglChoosePixelFormat)(HDC hdc, const PIXELFORMATDESCRIPTOR *ppfd);
    HMODULE win32OpenGLDLL;
    win32OpenGLDLL = LoadLibrary("opengl32");
    kzsWglChoosePixelFormat = (kzInt (WINAPI*)(HDC, const PIXELFORMATDESCRIPTOR*))GetProcAddress(win32OpenGLDLL, "wglChoosePixelFormat");

    {
        /* Dummy class. */
        ZeroMemory(&dummyWindowClass, sizeof(WNDCLASSEX));
        dummyWindowClass.cbSize = sizeof(WNDCLASSEX);
        dummyWindowClass.style = CS_HREDRAW | CS_VREDRAW;
        dummyWindowClass.lpfnWndProc = DefWindowProc;
        dummyWindowClass.lpszClassName = TEXT("dummywindow");
        RegisterClassEx(&dummyWindowClass);

        /* Dummy window. */
        dummyWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, TEXT("dummywindow"), NULL, WS_EX_TOPMOST | WS_POPUP, 100, 100, 512, 512, NULL, NULL, NULL, NULL);
        hDC = GetDC(dummyWindow);

        /* Set pixel format. */
        {
            kzInt pixelFormat;
            PIXELFORMATDESCRIPTOR pixelFormatDescriptor;

            ZeroMemory(&pixelFormatDescriptor, sizeof(PIXELFORMATDESCRIPTOR));
            pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pixelFormatDescriptor.nVersion = 1;
            pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
            pixelFormatDescriptor.cColorBits = 32;
            pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;

            pixelFormat = (kzInt)ChoosePixelFormat(hDC, &pixelFormatDescriptor);
            if(!SetPixelFormat(hDC, pixelFormat, &pixelFormatDescriptor))
            {
                kzsErrorThrow(KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Could not set pixel format.");
            }
        }

        glContext = wglCreateContext(hDC);
        kzsErrorTest((glContext != KZ_NULL), KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");

        wglResult = wglMakeCurrent(hDC, glContext);
        kzsErrorTest((wglResult != FALSE), KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");

        kzsWglChoosePixelFormatARB = (BOOL (WINAPI*)(HDC, const kzInt*, const FLOAT*, UINT, kzInt*, UINT*))wglGetProcAddress("wglChoosePixelFormatARB");

        /* Deinit win32 OpenGL */
        wglResult = wglMakeCurrent(NULL, NULL);
        kzsErrorTest((wglResult != FALSE), KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");

        wglResult = wglDeleteContext(glContext);
        kzsErrorTest((wglResult != FALSE), KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");

        ReleaseDC(dummyWindow, hDC);
        DestroyWindow(dummyWindow);
        UnregisterClass("dummywindow", NULL);
    }

    /* Set pixel format. */
    if(kzsWglChoosePixelFormatARB != KZ_NULL)
    {
        /* Need to get all the necessary data from the window to recreate it appropriately. */
        float fAttributes[] = {0.0f, 0.0f};
        int iAttributes[] = {
            WGL_RED_BITS_ARB, 8,
            WGL_GREEN_BITS_ARB, 8,
            WGL_BLUE_BITS_ARB, 8,
            WGL_ALPHA_BITS_ARB, 8,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            WGL_SAMPLES_ARB, 4,
            WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
            WGL_COLOR_BITS_ARB, 24,
            WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
            WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            0, 0};
            int pixelFormat;
            UINT numFormats;

            iAttributes[1] = surfaceProperties->bitsColorR;
            iAttributes[3] = surfaceProperties->bitsColorG;
            iAttributes[5] = surfaceProperties->bitsColorB;
            iAttributes[7] = surfaceProperties->bitsAlpha;
            iAttributes[9] = surfaceProperties->bitsDepthBuffer;
            iAttributes[11] = surfaceProperties->bitsStencil;
            iAttributes[13] = surfaceProperties->antiAliasing;
            iAttributes[15] = (surfaceProperties->antiAliasing > 0 ? GL_TRUE : GL_FALSE);

            kzsWglChoosePixelFormatARB(hdc, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
            if(numFormats > 0)
            {
                if(!SetPixelFormat(hdc, pixelFormat, &pixelFormatDescriptor))
                {
                    kzsErrorThrow(KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Could not set pixel format.");
                }
            }
    }
    else /* Default pixel format. */
    {        
        int pixelFormat;        

        ZeroMemory(&pixelFormatDescriptor, sizeof(PIXELFORMATDESCRIPTOR));
        pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pixelFormatDescriptor.nVersion = 1;
        pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
        pixelFormatDescriptor.cColorBits = 32;
        pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;

        pixelFormat = ChoosePixelFormat(hdc, &pixelFormatDescriptor);
        if(!SetPixelFormat(hdc, pixelFormat, &pixelFormatDescriptor))
        {
            kzsErrorThrow(KZS_ERROR_SURFACE_INITIALIZATION_FAILED, "Could not set pixel format.");
        }
    }

    glContext = wglCreateContext(hdc);
    kzsErrorTest((glContext != KZ_NULL), KZS_ERROR_SURFACE_UPDATE_FAILED, "Unable to create WGL context.");
    
    result = kzsWGLMakeCurrent(hdc, glContext);
    kzsErrorForward(result);

    *out_hrc = glContext;
#else 
    *out_hrc = KZ_NULL;
#endif
    kzsSuccess();
}

kzsError kzsWGLUpdateSurface(HDC hdc)
{
    kzBool result;

    result = SwapBuffers(hdc);
    kzsErrorTest((result != KZ_FALSE), KZS_ERROR_SURFACE_UPDATE_FAILED, "Unable to update WGL surface.");

    kzsSuccess();
}

kzsError kzsWGLMakeCurrent(HDC hdc, HGLRC glContext)
{
    kzBool result;

    result = wglMakeCurrent(hdc, glContext);
    kzsErrorTest((result != KZ_FALSE), KZS_ERROR_SURFACE_ACTIVATION_FAILED, "Unable to activate WGL surface.");

    kzsSuccess();
}

kzsError kzsWGLSwapInterval(kzInt interval)
{
    if(kzsWGLExtensionSupported_internal("WGL_EXT_swap_control"))
    {
        typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC) (kzInt interval);

        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");

        wglSwapIntervalEXT(interval);
    }    

    kzsSuccess();
}

void (*kzsGLGetProcAddress(kzString functionName))(void)
{
    return (void(*)(void))wglGetProcAddress(functionName);
}

void kzsWGLGetUsedSurfaceProperties(HDC hdc, struct KzsSurfaceProperties* usedProperties)
{
    PIXELFORMATDESCRIPTOR  pfd;
    int  iPixelFormat;

    // get the current pixel format index  
    iPixelFormat = GetPixelFormat(hdc); 

    // obtain a detailed description of that pixel format  
    DescribePixelFormat(hdc, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    usedProperties->bitsColorR = pfd.cRedBits;
    usedProperties->bitsColorG = pfd.cGreenBits;
    usedProperties->bitsColorB = pfd.cBlueBits;
    usedProperties->bitsAlpha = pfd.cAlphaBits;
    usedProperties->bitsDepthBuffer = pfd.cDepthBits;
    usedProperties->bitsStencil = pfd.cStencilBits;
    /* TODO: Would be great to fetch this also. */
    usedProperties->antiAliasing = -1;
}
