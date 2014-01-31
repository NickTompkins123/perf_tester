/**
* \file
* Kanzi window, native portion for the win32 platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_window_native.h"

#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/display/kzs_egl.h>

#include <windows.h>


/* Forward declarations. */
struct KzsWindow;


/** Platform specific window variables. */
struct KzsWindowNative
{
    HINSTANCE   hInstance;      /**< Application handle. */
    HDC         hDC;            /**< Device context handle. */
    HWND        hWnd;           /**< Win32 handle for the actual window. */
};

kzsError kzsWindowNativeCreate(const struct KzsWindowProperties* windowProperties, struct KzsWindowNative** out_windowNative)
{
    kzsError result;
    struct KzsWindowNative* windowNative;
    
    result = kzsWindowNativeCreateEmpty(&windowNative);
    kzsErrorForward(result);    
    
    /* Create a win32 window and set it up. */
    {
        RECT rect;

        kzUint windowWidth = windowProperties->width;
        kzUint windowHeight = windowProperties->height;

        DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        DWORD extendedWindowStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

        /* Set window position to the center of the screen. */
        int windowPositionX = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
        int windowPositionY = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;
        SetRect(&rect, windowPositionX, windowPositionY, windowPositionX + windowWidth, windowPositionY + windowHeight);

        /* Add style options according to the configuration. */
        if(windowProperties->style == KZS_WINDOW_STYLE_RESIZABLE)
        {
            windowStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;
        }

        /* Set window style and position. */
        AdjustWindowRectEx(&rect, windowStyle, FALSE, extendedWindowStyle);

        /* Create the window with given parameters. */
        windowNative->hWnd = CreateWindowEx(extendedWindowStyle, TEXT("KanziWindow"), TEXT("Kanzi v2.0"), windowStyle,
                                            rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                                            NULL, NULL, windowNative->hInstance, NULL);
        kzsErrorTest(windowNative->hWnd != NULL, KZS_ERROR_WINDOW_CREATION_FAILED, "Unable to create window.");
    }

    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeCreateEmpty(struct KzsWindowNative** out_windowNative)
{
    struct KzsWindowNative* windowNative = (struct KzsWindowNative*)kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");

    windowNative->hInstance = KZ_NULL;
    windowNative->hDC = KZ_NULL;
    windowNative->hWnd = KZ_NULL;

    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeInitialize(struct KzsWindowNative* windowNative, struct KzsWindow* window)
{
    /* Store the display device context handle for the window. */
    windowNative->hDC = GetDC(windowNative->hWnd);

    /* Store window pointer to window handler. */
    SetWindowLongPtr(windowNative->hWnd, GWL_USERDATA, (LONG)window);

    /* Show window and put it on foreground. */
    ShowWindow(windowNative->hWnd, SW_SHOWNORMAL);
    UpdateWindow(windowNative->hWnd);
    SetForegroundWindow(windowNative->hWnd);

    kzsSuccess();
}

kzsError kzsWindowNativeResize(const struct KzsWindowNative* windowNative, kzUint width, kzUint height)
{
    WINDOWINFO windowInfo;
    int newWidth = (int)width;
    int newHeight = (int)height;

    /* Get original window information. */
    windowInfo.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(windowNative->hWnd, &windowInfo);

    /* Take the borders into account. */
    newWidth += (windowInfo.rcWindow.right - windowInfo.rcWindow.left) - (windowInfo.rcClient.right - windowInfo.rcClient.left);
    newHeight += (windowInfo.rcWindow.bottom - windowInfo.rcWindow.top) - (windowInfo.rcClient.bottom - windowInfo.rcClient.top);

    /* Resize the window, keeping it's original position. */
    SetWindowPos(windowNative->hWnd, NULL, 0, 0, newWidth, newHeight, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

    kzsSuccess();
}

kzsError kzsWindowNativeDelete(struct KzsWindowNative* windowNative)
{
    /* Release the display device context handle. */
    ReleaseDC(windowNative->hWnd, windowNative->hDC);

    kzsFree(windowNative);

    kzsSuccess();
}

HWND kzsWindowNativeGetHandle(const struct KzsWindowNative* windowNative)
{
    HWND hwnd;

    if(windowNative->hWnd == NULL)
    {
        hwnd = NULL;
    }
    else
    {
        hwnd = windowNative->hWnd;
    }

    return hwnd;
}

void kzsWindowNativeSetHandle(struct KzsWindowNative* windowNative, HWND windowHandle)
{
    windowNative->hWnd = windowHandle;
}

HDC kzsWindowNativeGetHDC(const struct KzsWindowNative* windowNative)
{
    HDC hdc;

    if(windowNative->hDC == NULL)
    {
        hdc = NULL;
    }
    else
    {
        hdc = windowNative->hDC;
    }

    return hdc;
}

void kzsHandleMessages(struct KzsWindow* window, HACCEL hAccelTable)
{
    MSG msg;
    struct KzsWindowNative* windowNative;
    kzInt maxMessages = 10;     /* temporary solution to avoid hanging up... process only max this amount of messages per frame */

    if(window != KZ_NULL)
    {
        windowNative = kzsWindowGetNative(window);

        /* Get pending messages and handle them. */
        while(PeekMessage(&msg, kzsWindowNativeGetHandle(windowNative), 0, 0, PM_REMOVE) && maxMessages > 0)
        {
            if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                maxMessages--;
            }
        }
    }
}
