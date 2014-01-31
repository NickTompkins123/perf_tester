/**
* \file
* Kanzi display, native portion for the win32 platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_display_native.h"

#include <system/display/kzs_display.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>

#include <windows.h>
#include <stdio.h>
#include <crtdbg.h>

/** Platform specific display variables. */
struct KzsDisplayNative
{
    CHAR* name;      /**< Operating system specific name of the display device. */
};

kzsError kzsDisplayNativeCreate(kzUint displayID, struct KzsDisplayNative** out_displayNative)
{
    struct KzsDisplayNative* displayNative;
    BOOL validDisplay = TRUE;
    kzUint numDisplays = 0;
    kzUint i = 0;

    displayNative = (struct KzsDisplayNative*)kzsMalloc(sizeof(*displayNative));
    kzsErrorTest(displayNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native display.");

    /* Search the display by ID. */
    while(validDisplay && i <= displayID)
    {
        struct _DISPLAY_DEVICEA device;
        memset(&device, 0x00, sizeof(device));

        device.cb = sizeof(device);

        validDisplay = EnumDisplayDevices(NULL, i, &device, 0);

        if(validDisplay)
        {
            /* Check the display is not virtual. */
            if((device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) == 0)
            {
                if(numDisplays == displayID)
                {
                    /* Store the name. */
                    displayNative->name = (CHAR*)kzsMalloc(sizeof(device.DeviceName));
                    strcpy(displayNative->name, device.DeviceName);
                    /*printf("%s\n", displayNative->name);*/

                    {
                        BOOL validMode = TRUE;
                        kzUint j = 0;
                        kzUint numValidModes = 0;

                        struct _devicemodeA deviceMode;
                        deviceMode.dmSize = sizeof(DEVMODE);
                        
                        /* Print out supported resolutions */
                        while(validMode == TRUE)
                        {
                            validMode = EnumDisplaySettings(device.DeviceName, j, &deviceMode);
                            /*if(validMode)
                            {
                                printf("%d %d: %d x %d %d bpp, %d hz\n", displayID, j, deviceMode.dmPelsWidth, deviceMode.dmPelsHeight, deviceMode.dmBitsPerPel, deviceMode.dmDisplayFrequency);
                            }*/
                            j++;
                        }

                        numValidModes = j;
                        j = 0;

                    }
                }
                numDisplays++;
            }
        }
        i++;
    }

    /* Test for OpenGL 2.0 support */
    /*
    {
        WNDCLASSEX dummyWindowClass;
        HWND dummyWindow;
        HGLRC glContext;
        HDC hDC;
        HMODULE win32OpenGLDLL;
        BOOL wglResult;
        KzsWglCreateContext_internal KzsWglCreateContextFunction;
        KzsWglMakeCurrent_internal KzsWglMakeCurrentFunction;
        KzsWglDeleteContext_internal KzsWglDeleteContextFunction;
        KzsWglChoosePixelFormat_internal KzsWglChoosePixelFormatFunction;
        KzsWglSetPixelFormat_internal KzsWglSetPixelFormatFunction;
        KzsWin32GlGetString_internal KzsWin32GlGetStringFunction;

        char* glVersionString;
        int glVersionMajor;

        win32OpenGLDLL = LoadLibrary("opengl32");
        kzsErrorTest(win32OpenGLDLL != NULL, KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");

        KzsWglCreateContextFunction = (KzsWglCreateContext_internal)GetProcAddress(win32OpenGLDLL, "wglCreateContext");
        KzsWglMakeCurrentFunction = (KzsWglMakeCurrent_internal)GetProcAddress(win32OpenGLDLL, "wglMakeCurrent");
        KzsWglDeleteContextFunction = (KzsWglDeleteContext_internal)GetProcAddress(win32OpenGLDLL, "wglDeleteContext");
        KzsWglChoosePixelFormatFunction = (KzsWglChoosePixelFormat_internal)GetProcAddress(win32OpenGLDLL, "wglChoosePixelFormat");
        KzsWglSetPixelFormatFunction = (KzsWglSetPixelFormat_internal)GetProcAddress(win32OpenGLDLL, "wglSetPixelFormat");
        KzsWin32GlGetStringFunction = (KzsWin32GlGetString_internal)GetProcAddress(win32OpenGLDLL, "glGetString");

        ZeroMemory(&dummyWindowClass, sizeof(WNDCLASSEX));
        dummyWindowClass.cbSize = sizeof(WNDCLASSEX);
        dummyWindowClass.style = CS_HREDRAW | CS_VREDRAW;
        dummyWindowClass.lpfnWndProc = DefWindowProc;
        dummyWindowClass.lpszClassName = TEXT("dummywindow");
        RegisterClassEx(&dummyWindowClass);

        dummyWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, TEXT("dummywindow"), NULL, WS_EX_TOPMOST | WS_POPUP, 100, 100, 512, 512, NULL, NULL, NULL, NULL);
        hDC = GetDC(dummyWindow);
        kzsErrorTest((hDC != KZ_NULL), KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");

        {
            int pixelFormat;
            PIXELFORMATDESCRIPTOR pixelFormatDescriptor;

            ZeroMemory(&pixelFormatDescriptor, sizeof(PIXELFORMATDESCRIPTOR));
            pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pixelFormatDescriptor.nVersion = 1;
            pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
            pixelFormatDescriptor.cColorBits = 32;
            pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;

            pixelFormat = KzsWglChoosePixelFormatFunction(hDC, &pixelFormatDescriptor);
            KzsWglSetPixelFormatFunction(hDC, pixelFormat, &pixelFormatDescriptor);
        }

        glContext = KzsWglCreateContextFunction(hDC);
        kzsErrorTest((glContext != KZ_NULL), KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");
        wglResult = KzsWglMakeCurrentFunction(hDC, glContext);
        kzsErrorTest((wglResult != FALSE), KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");

        glVersionMajor = 0;
        glVersionString = (char *)KzsWin32GlGetStringFunction(GL_VERSION);
        sscanf_s(glVersionString, "%d.%*d", &glVersionMajor);

        wglResult = KzsWglMakeCurrentFunction(NULL, NULL);
        kzsErrorTest((wglResult != FALSE), KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");

        wglResult = KzsWglDeleteContextFunction(glContext);
        kzsErrorTest((wglResult != FALSE), KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");

        ReleaseDC(dummyWindow, hDC);
        DestroyWindow(dummyWindow);
        UnregisterClass("dummywindow", NULL);
        FreeLibrary(win32OpenGLDLL);

        kzsErrorTest((glVersionMajor != 0), KZS_ERROR_DISPLAY_OPENGL_VERSION_NOT_DETECTED, "Unable to detect OpenGL version.");
        kzsErrorTest((glVersionMajor >= 2), KZS_ERROR_DISPLAY_OPENGL2_NOT_SUPPORTED, "OpenGL 2.0 or better is not supported.");
    }
    */

    *out_displayNative = displayNative;
    kzsSuccess();
}

kzsError kzsDisplayNativeDelete(struct KzsDisplayNative* displayNative)
{
    kzsAssert(displayNative != KZ_NULL);
    kzsAssert(displayNative->name != KZ_NULL);

    kzsFree(displayNative->name);
    kzsFree(displayNative);

    kzsSuccess();
}

kzsError kzsDisplayNativeGetNumDisplays(kzUint* out_numDisplays)
{    
    kzUint numDisplays = 0;

    numDisplays = GetSystemMetrics(SM_CMONITORS);

    *out_numDisplays = numDisplays;

    kzsSuccess();
}

kzUint kzsDisplayGetCurrentWidth(const struct KzsDisplay* display)
{
    struct KzsDisplayNative* displayNative;
    kzUint screenWidth = 0;
    struct _devicemodeA deviceMode;
    deviceMode.dmSize = sizeof(DEVMODE);

    kzsAssert(display != NULL);
    
    displayNative = kzsDisplayGetNative(display);

    if(EnumDisplaySettings(displayNative->name, ENUM_CURRENT_SETTINGS, &deviceMode))
    {
        screenWidth = deviceMode.dmPelsWidth;
    }

    return screenWidth;
}

kzUint kzsDisplayGetCurrentHeight(const struct KzsDisplay* display)
{
    struct KzsDisplayNative* displayNative;
    kzUint screenHeight = 0;
    struct _devicemodeA deviceMode;
    deviceMode.dmSize = sizeof(DEVMODE);

    kzsAssert(display != NULL);

    displayNative = kzsDisplayGetNative(display);

    if(EnumDisplaySettings(displayNative->name, ENUM_CURRENT_SETTINGS, &deviceMode))
    {
        screenHeight = deviceMode.dmPelsHeight;
    }

    return screenHeight;
}
