/**
* \file
* Kanzi window, native portion for the win32 platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_window_native.h"

#include <system/display/kzs_display.h>
#include <system/display/kzs_window.h>
#include <system/input/kzs_input_native.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>

#include <windows.h>
#include <WindowsX.h>


/* TODO: Clean-up */


#define KZS_WINDOW_NATIVE_NUM_KEYS      256 /**< Number of keyboard keys for window native. */


/* Forward declarations. */
struct KzsWindow;


/** Platform specific window variables. */
struct KzsWindowNative
{
    HDC         hDC;            /**< Device context handle. */
    HWND        hWnd;           /**< Win32 handle for the actual window. */
    kzBool      managedExternally;  /**< Is the actual window created/deleted outside Kanzi (for example the tool preview) */

    /* TODO: access these from the display always instead */
    kzUint      displayWidth;
    kzUint      displayHeight;

    enum KzsInputKeyDeviceState keyStates[KZS_WINDOW_NATIVE_NUM_KEYS];
};


static LRESULT CALLBACK kzsWindowNativeMainWndProc_internal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


kzsError kzsWindowNativeCreate(const struct KzsWindowProperties* windowPropertiesRequested, struct KzsDisplay* display, struct KzsWindowProperties* out_windowProperties, struct KzsWindowNative** out_windowNative)
{
    kzsError result;
    struct KzsWindowNative* windowNative;
    struct KzsWindowProperties windowProperties;

    /* Get a copy of requested properties and modify the copy instead */
    windowProperties = *windowPropertiesRequested;
   
    result = kzsWindowNativeCreateEmpty(&windowNative);
    kzsErrorForward(result);

    /* TODO: access these from the display always instead */
    windowNative->displayWidth = kzsDisplayGetCurrentWidth(display);
    windowNative->displayHeight = kzsDisplayGetCurrentHeight(display);

    /* If resolution set to automatic, get maximum window size available. */
    {
        if(windowProperties.width == 0)
        {
            windowProperties.width = kzsDisplayGetCurrentWidth(display);
        }

        if(windowProperties.height == 0)
        {
            windowProperties.height = kzsDisplayGetCurrentHeight(display);
        }
    }

    /* If full screen style is chosen, override resolution to full size */
    if(windowProperties.style == KZS_WINDOW_STYLE_FULL_SCREEN)
    {
        windowProperties.width = kzsDisplayGetCurrentWidth(display);
        windowProperties.height = kzsDisplayGetCurrentHeight(display);
    }

    /* Register win32 window class. */
    {
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style          = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc    = kzsWindowNativeMainWndProc_internal;
        wcex.cbClsExtra     = 0;
        wcex.cbWndExtra     = 0;
        wcex.hInstance      = NULL;
        wcex.hIcon          = NULL;
        wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName   = NULL;
        wcex.lpszClassName  = TEXT("KanziWindow");
        wcex.hIconSm        = NULL;

        RegisterClassEx(&wcex);
    }
    
    /* Create a win32 window and set it up. */
    {
        RECT rect;

        kzUint windowWidth = windowProperties.width;
        kzUint windowHeight = windowProperties.height;

        DWORD windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        DWORD extendedWindowStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

        /* Set window position to the center of the screen. */
        int windowPositionX = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
        int windowPositionY = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;
        SetRect(&rect, windowPositionX, windowPositionY, windowPositionX + windowWidth, windowPositionY + windowHeight);

        /* Add style options according to the configuration. */
        if(windowProperties.style == KZS_WINDOW_STYLE_RESIZABLE)
        {
            windowStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;
        }

        if(windowProperties.style == KZS_WINDOW_STYLE_FULL_SCREEN)
        {
            windowStyle = WS_EX_TOPMOST | WS_POPUP;
        }

        /* Set window style and position. */
        AdjustWindowRectEx(&rect, windowStyle, FALSE, extendedWindowStyle);

        /* Create the window with given parameters. */
        windowNative->hWnd = CreateWindowEx(extendedWindowStyle, TEXT("KanziWindow"), TEXT("Kanzi v2.0"), windowStyle,
                                            rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                                            NULL, NULL, NULL, NULL);
        kzsErrorTest(windowNative->hWnd != NULL, KZS_ERROR_WINDOW_CREATION_FAILED, "Unable to create window.");

        windowNative->managedExternally = KZ_FALSE;
    }

    /* Reset key state buffer. */
    {
        kzUint i;
        for(i = 0; i < KZS_WINDOW_NATIVE_NUM_KEYS; ++i)
        {
            windowNative->keyStates[i] = KZS_KEY_DEVICE_STATE_UP;
        }
    }

    *out_windowProperties = windowProperties;
    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeCreateEmpty(struct KzsWindowNative** out_windowNative)
{
    struct KzsWindowNative* windowNative = (struct KzsWindowNative*)kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");

    windowNative->hDC = KZ_NULL;
    windowNative->hWnd = KZ_NULL;
    windowNative->managedExternally = KZ_TRUE;

    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeInitialize(struct KzsWindowNative* windowNative, struct KzsWindow* window)
{
    /* Store the display device context handle for the window. */
    windowNative->hDC = GetDC(windowNative->hWnd);
    kzsErrorTest(windowNative->hDC != KZ_NULL, KZS_ERROR_INITIALIZATION_FAILED, "Unable to get device context.");

    /* Store window pointer to window handler. */
    SetWindowLongPtr(windowNative->hWnd, GWL_USERDATA, (LONG)window);

    if(kzsWindowGetStyle(window) != KZS_WINDOW_STYLE_STUDIO_PREVIEW)
    {
        /* Show window and put it on foreground. */
        ShowWindow(windowNative->hWnd, SW_SHOWNORMAL);
        UpdateWindow(windowNative->hWnd);
        SetForegroundWindow(windowNative->hWnd);
    }

    kzsSuccess();
}

kzsError kzsWindowNativeUpdate(const struct KzsWindow* window)
{
    kzsError result;
    struct KzsWindowNative* windowNative = kzsWindowGetNative(window);
    MSG message;

    if(kzsWindowGetStyle(window) != KZS_WINDOW_STYLE_STUDIO_PREVIEW)
    {
        struct KzsInputEventQueue* queue = kzsWindowGetInputEventQueue(window);
        kzsInputEventHandleFunction handler;

        kzsAssert(queue != NULL);

        /* Get pending messages and handle them. */
        while(PeekMessage(&message, kzsWindowNativeGetHandle(windowNative), 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        handler = kzsWindowGetInputEventHandler(window);
        if(handler != KZ_NULL)
        {
            void *userData = kzsWindowGetUserData(window);
            result = handler(queue, userData);
            kzsErrorForward(result);
        }

        kzsInputEventQueueClear(queue);
    }

    kzsSuccess();
}

kzsError kzsWindowNativeResize(const struct KzsWindowNative* windowNative, kzUint width, kzUint height)
{
    WINDOWINFO windowInfo;

    /* Default styles */
    DWORD windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    int newWidth = (int)width;
    int newHeight = (int)height;

    /* Get original window information. */
    windowInfo.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(windowNative->hWnd, &windowInfo);

    /* TODO: do this somewhere else, kzsWindowChangeStyle or something */
    if(newWidth == 0 && newHeight == 0)
    {
        /* Switch to full screen */
        newWidth = windowNative->displayWidth;
        newHeight = windowNative->displayHeight;

        /* No decorations. */
        windowStyle = WS_EX_TOPMOST | WS_POPUP;
        SetWindowLong(windowNative->hWnd, GWL_STYLE, windowStyle);
        ShowWindow(windowNative->hWnd, SW_MAXIMIZE);
    }
    else
    {
        /* Take the borders into account. */
        newWidth += (windowInfo.rcWindow.right - windowInfo.rcWindow.left) - (windowInfo.rcClient.right - windowInfo.rcClient.left);
        newHeight += (windowInfo.rcWindow.bottom - windowInfo.rcWindow.top) - (windowInfo.rcClient.bottom - windowInfo.rcClient.top);

        /* Add decorations */
        //windowStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;

        SetWindowLong(windowNative->hWnd, GWL_STYLE, windowStyle);
        ShowWindow(windowNative->hWnd, SW_NORMAL);
    }

    /* Resize the window, keeping it's original position. */
    SetWindowPos(windowNative->hWnd, NULL, 0, 0, newWidth, newHeight, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

    UpdateWindow(windowNative->hWnd);
    SetForegroundWindow(windowNative->hWnd);

    kzsSuccess();
}

kzsError kzsWindowNativeSetPositionAbsolute(const struct KzsWindowNative* windowNative, kzUint x, kzUint y)
{
    SetWindowPos(windowNative->hWnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER);

    kzsSuccess();
}

kzsError kzsWindowNativeSetTitle(const struct KzsWindowNative* windowNative, kzString titleText)
{
    BOOL result;

    result = SetWindowTextA(windowNative->hWnd, titleText);
    kzsErrorTest(result == TRUE, KZS_ERROR_WINDOW_MODIFICATION_FAILED, "Unable to set window title text.");

    kzsSuccess();
}

kzsError kzsWindowNativeDelete(struct KzsWindowNative* windowNative)
{
    if(!windowNative->managedExternally)
    {
        /* Destroy the window. */
        DestroyWindow(windowNative->hWnd);
    }

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

void kzsWindowNativeSetKeyState(struct KzsWindowNative* windowNative, kzUint keyCode, enum KzsInputKeyDeviceState state)
{
    if(keyCode < KZS_WINDOW_NATIVE_NUM_KEYS)
    {
        windowNative->keyStates[keyCode] = state;
    }
}

kzsError kzsWindowNativeWndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, kzBool* out_handled)
{
    kzsError result;
    kzBool handled;
    struct KzsWindow* window = (struct KzsWindow*)GetWindowLongPtr(hwnd, GWL_USERDATA); /* Retrieve pointer to the Kanzi window struct. */

    if (window != NULL && (kzsWindowGetState(window) == KZS_WINDOW_STATE_ACTIVE ||
                           kzsWindowGetState(window) == KZS_WINDOW_STATE_INACTIVE))
    {
        struct KzsInputEventQueue* queue  = kzsWindowGetInputEventQueue(window);
        kzsWindowEventHandlerFunction eventHandler = kzsWindowGetEventHandler(window);

        result = kzsInputNativeHandleMessage(window, hwnd, message, wparam, lparam, &handled);
        kzsErrorForward(result);

        if(!handled)
        {
            switch (message)
            {
                case WM_CLOSE:
                {
                    kzsWindowClosed(window);
                    break;
                }

                case WM_DESTROY:
                {
                    PostQuitMessage(0);
                    break;
                }

                case WM_KILLFOCUS:
                {
                    /* Release all held keys. */
                    struct KzsWindowNative* windowNative = kzsWindowGetNative(window);
                    kzUint i;
                    for(i = 0; i < KZS_WINDOW_NATIVE_NUM_KEYS; ++i)
                    {
                        if(windowNative->keyStates[i] == KZS_KEY_DEVICE_STATE_DOWN)
                        {
                            kzsInputEventQueueAddKeyDeviceEvent(queue, i, KZS_KEY_DEVICE_STATE_UP);

                            windowNative->keyStates[i] = KZS_KEY_DEVICE_STATE_UP;
                        }
                    }

                    kzsWindowSetState(window, KZS_WINDOW_STATE_INACTIVE);
                    result = eventHandler(window, KZS_WINDOW_EVENT_FOCUS_LOST, kzsWindowGetUserData(window));
                    kzsErrorForward(result);
                    break;
                }

                case WM_SETFOCUS:
                {
                    kzsWindowSetState(window, KZS_WINDOW_STATE_ACTIVE);
                    result = eventHandler(window, KZS_WINDOW_EVENT_FOCUS_GAINED, kzsWindowGetUserData(window));
                    kzsErrorForward(result);
                    break;
                }

                case WM_SIZE:
                {
                    RECT windowSize;
                    if(GetClientRect(hwnd, &windowSize))
                    {
                        kzsWindowSetWidth_private(window, windowSize.right);
                        kzsWindowSetHeight_private(window, windowSize.bottom);

                        result = eventHandler(window, KZS_WINDOW_EVENT_WINDOW_RESIZED, kzsWindowGetUserData(window));
                        kzsErrorForward(result);
                    }
                    break;
                }

                default:
                {
                    handled = KZ_FALSE;
                    break;
                }
            }

        }
    }
    else
    {
        handled = KZ_FALSE;
    }

    *out_handled = handled;
    kzsSuccess();
}

static LRESULT CALLBACK kzsWindowNativeMainWndProc_internal(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    kzsError result;
    LRESULT lresult;
    kzBool handled;

    result = kzsWindowNativeWndProcHandler(hwnd, message, wparam, lparam, &handled);
    kzsErrorIf(result)
    {
        DestroyWindow(hwnd);
        kzsWindowClosed((struct KzsWindow*)GetWindowLongPtr(hwnd, GWL_USERDATA)); /* TODO: This should not be needed, doesn't destroying a window also post a closed event */
    }

    if (handled)
    {
        lresult = 0;
    }
    else
    {
        lresult = DefWindowProc(hwnd, message, wparam, lparam);
    }

    return lresult;
}
