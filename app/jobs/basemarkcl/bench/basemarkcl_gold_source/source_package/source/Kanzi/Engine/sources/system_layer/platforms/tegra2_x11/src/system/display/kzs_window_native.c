/**
* \file
* Kanzi window, native portion for the Linux platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_window_native.h"

#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/display/kzs_egl.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

struct KzsWindow;


/** Platform specific surface variables. */
struct KzsWindowNative
{
    Display* display;
    Window window;
};

kzsError kzsWindowNativeCreate(const struct KzsWindowProperties* windowPropertiesRequested, struct KzsDisplay* display, struct KzsWindowProperties* out_windowProperties, struct KzsWindowNative** out_windowNative)
{
    KZ_UNUSED_PARAMETER(display);
    
    struct KzsWindowProperties windowProperties;
    Window root;
    XSetWindowAttributes swa;
    XWindowAttributes rootWindowAttributes;
    
    windowProperties = *windowPropertiesRequested;
    
    struct KzsWindowNative* windowNative = kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");

    windowNative->display = XOpenDisplay(NULL);
    kzsErrorTest(windowNative->display != KZ_NULL, KZS_ERROR_DISPLAY_INITIALIZATION_FAILED, "Unable to open X11 display.");

    root = DefaultRootWindow(windowNative->display);
    kzsErrorTest(root != KZ_NULL, KZS_ERROR_DISPLAY_INITIALIZATION_FAILED, "Unable to get root window.");

    /* Set event flags. */
    swa.event_mask =
        VisibilityChangeMask |  /* Window visibility changed */
        FocusChangeMask |       /* Focus lost or gained */
        PointerMotionMask |     /* Touch screen motion */
        ButtonPressMask |       /* Touch screen press */
        ButtonReleaseMask |     /* Touch screen de-press */
        KeyPressMask |          /* Keyboard key press */
        KeyReleaseMask;         /* Keyboard key de-press */

    XGetWindowAttributes(windowNative->display, root, &rootWindowAttributes);
    
    /*printf("display is %d x %d\n", rootWindowAttributes.width, rootWindowAttributes.height);*/
        
    /* Create the window. */
    windowNative->window = XCreateWindow(windowNative->display, root, 0, 0, rootWindowAttributes.width, rootWindowAttributes.height, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);
    kzsErrorTest(windowNative->window != KZ_NULL, KZS_ERROR_WINDOW_CREATION_FAILED, "Unable to create application window.");
       
    XMapWindow(windowNative->display, windowNative->window);
    XStoreName(windowNative->display, windowNative->window, "Kanzi v2.0");
    
    /* Disable mouse pointer. */
    {
        Cursor invisibleCursor;
        Pixmap bitmapNoData;
        XColor black;
        static char noData[] = { 0,0,0,0,0,0,0,0 };
        black.red = black.green = black.blue = 0;

        bitmapNoData = XCreateBitmapFromData(windowNative->display, windowNative->window, noData, 8, 8);
        invisibleCursor = XCreatePixmapCursor(windowNative->display, bitmapNoData, bitmapNoData, &black, &black, 0, 0);
        XDefineCursor(windowNative->display, windowNative->window, invisibleCursor);
        XFreeCursor(windowNative->display, invisibleCursor);
    }    

    /* Enable keyboard input for the window. */
    {
        XWMHints inputHints;
        inputHints.input = True;
        inputHints.flags = InputHint;
        XSetWMHints(windowNative->display, windowNative->window, &inputHints);
    }
    
    *out_windowProperties = windowProperties;
    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeCreateEmpty(struct KzsWindowNative** out_windowNative)
{
    struct KzsWindowNative* windowNative = kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");

    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeInitialize(struct KzsWindowNative* windowNative,struct KzsWindow* window)
{
    KZ_UNUSED_PARAMETER(windowNative);
    KZ_UNUSED_PARAMETER(window);
    kzsSuccess();
}

kzsError kzsWindowNativeUpdate(const struct KzsWindow* window)
{
    Display* display = kzsWindowNativeGetDisplay(kzsWindowGetNative(window));
    XEvent event;
    
    /* TODO: handle multiple windows properly */
    while (XPending(display))
    {
        kzsInputPointingDeviceHandlerFunction pointingDeviceHandler = kzsWindowGetPointingDeviceHandler(window);
        kzsInputKeyDeviceHandlerFunction keyDeviceHandler = kzsWindowGetKeyDeviceHandler(window);
    
        XNextEvent(display, &event);
        
        switch (event.type)
        {
            /*printf( */
            /*case FocusIn:
            windowFocused = 1;
            break;
            
            case FocusOut:
            windowFocused = 0;
            break;*/
            
            case ButtonPress:
            {
                struct KzsPointingDeviceInputData* inputData;
                struct KzsInputPointingDevice* inputPointingDevice = kzsWindowGetInputPointingDevice(window);
                kzsWindowSetActivePointer(window, 0);
                kzsInputSetPointingDeviceInputData(kzsInputGetPointingDeviceData(inputPointingDevice), 0, event.xbutton.x, event.xbutton.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, KZS_POINTING_DEVICE_STATE_DOWN);
                inputData = kzsWindowGetPointingDeviceData(window);
                pointingDeviceHandler(inputData);
            }
            break;
            
            case ButtonRelease:
            {
                struct KzsPointingDeviceInputData* inputData;
                struct KzsInputPointingDevice* inputPointingDevice = kzsWindowGetInputPointingDevice(window);
                kzsWindowSetActivePointer(window, 0);
                kzsInputSetPointingDeviceInputData(kzsInputGetPointingDeviceData(inputPointingDevice), 0, event.xbutton.x, event.xbutton.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, KZS_POINTING_DEVICE_STATE_UP);
                inputData = kzsWindowGetPointingDeviceData(window);
                pointingDeviceHandler(inputData);
            }
            
            break;
            
            case MotionNotify:
            {
                struct KzsPointingDeviceInputData* inputData;
                struct KzsInputPointingDevice* inputPointingDevice = kzsWindowGetInputPointingDevice(window);
                kzsWindowSetActivePointer(window, 0);
                kzsInputSetPointingDeviceInputData(kzsInputGetPointingDeviceData(inputPointingDevice), 0, event.xbutton.x, event.xbutton.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, KZS_POINTING_DEVICE_STATE_DRAG);
                inputData = kzsWindowGetPointingDeviceData(window);
                pointingDeviceHandler(inputData);
            }
            break;
            
            case KeyPress:
            {
                if(keyDeviceHandler)
                {
                    struct KzsKeyDeviceInputData* inputData;
                    kzsWindowSetButtonData(window, (kzUint)event.xkey.keycode, KZS_KEY_DEVICE_STATE_DOWN);
                    inputData = kzsWindowGetButtonInputData(window);
                    keyDeviceHandler(inputData);
                }
                /*int key = event.xkey.keycode;
                printf("key pressed %d\n", key);*/
            }
            break;
            
            case KeyRelease:
            {
                if(keyDeviceHandler)
                {
                    struct KzsKeyDeviceInputData* inputData;
                    kzsWindowSetButtonData(window, (kzUint)event.xkey.keycode, KZS_KEY_DEVICE_STATE_UP);
                    inputData = kzsWindowGetButtonInputData(window);
                    keyDeviceHandler(inputData);
                }                                    
                /*int key = event.xkey.keycode;
                printf("key released %d\n", key);*/
            }
            break;
            
            default:
            /*printf("unhandled event %d\n", event.type);*/
            break;
        }                                 
    }
    
    kzsSuccess();
}
    
kzsError kzsWindowNativeResize(const struct KzsWindowNative* windowNative, kzUint width, kzUint height)
{
    kzsSuccess();
}

kzsError kzsWindowNativeSetPositionAbsolute(const struct KzsWindowNative* windowNative, kzUint x, kzUint y)
{
    kzsSuccess();
}

kzsError kzsWindowNativeSetTitle(const struct KzsWindowNative* windowNative, kzString titleText)
{
    kzsSuccess();
}

kzsError kzsWindowNativeDelete(struct KzsWindowNative* windowNative)
{
    kzsFree(windowNative);
    kzsSuccess();
}

Display* kzsWindowNativeGetDisplay(const struct KzsWindowNative* windowNative)
{
    Display* display;

    if(windowNative->display == NULL)
    {
        display = NULL;
    }
    else
    {
        display = windowNative->display;
    }

    return display;
}

Window kzsWindowNativeGetWindow(const struct KzsWindowNative* windowNative)
{
    Window window;

    if(windowNative->window == NULL)
    {
        window = NULL;
    }
    else
    {
        window = windowNative->window;
    }

    return windowNative->window;
}
