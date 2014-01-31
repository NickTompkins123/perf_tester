/**
* \file
* Kanzi window, native portion for the Linux platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_window_native.h"

#include <system/display/kzs_display.h>
#include <system/display/kzs_display_native.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>

#include <unistd.h>
#include <stdio.h>
#include <system/wrappers/kzs_opengl_base.h>
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#endif
struct KzsWindow;


/** Platform specific surface variables. */
struct KzsWindowNative
{
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    Display* display;           /**< X11 display. */
    Window window;              /**< X11 window. */
    XVisualInfo *visualInfo;
#endif
    kzBool windowFocused;       /**< For tracking the window state internally. */
};

kzsError kzsWindowNativeCreate(const struct KzsWindowProperties* windowPropertiesRequested, struct KzsDisplay* display, struct KzsWindowProperties* out_windowProperties, struct KzsWindowNative** out_windowNative)
{
    #if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    Window root;
    XSetWindowAttributes swa;
    GLint glxAttributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    Colormap colorMap;
    struct KzsWindowProperties windowProperties;   
    struct KzsDisplayNative* displayNative;
    struct KzsWindowNative* windowNative = kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");
    
    /* Get a copy of requested properties and modify it. */
    windowProperties = *windowPropertiesRequested;     

    displayNative = kzsDisplayGetNative(display);
    windowNative->display = kzsDisplayNativeGetX11Display(displayNative);
    windowNative->windowFocused = KZ_TRUE;

    root = DefaultRootWindow(windowNative->display);
    
    
    /* Set event flags. */
    swa.event_mask =
        EnterWindowMask |       /* Window becomes focused */
        LeaveWindowMask |       /* Window loses focus */
        FocusChangeMask |       /* Keyboard focus lost or gained */
        PointerMotionMask |     /* Touch screen motion */
        ButtonPressMask |       /* Touch screen press */
        ButtonReleaseMask |     /* Touch screen de-press */
        KeyPressMask |          /* Keyboard key press */
        KeyReleaseMask;         /* Keyboard key de-press */

    /* If resolution set to automatic, get maximum window size available. */
    {
        if (windowProperties.width == 0)
        {
            windowProperties.width = kzsDisplayGetCurrentWidth(display);
        }

        if (windowProperties.height == 0)
        {
            windowProperties.height = kzsDisplayGetCurrentHeight(display);
        }
    }
    
    windowNative->visualInfo = glXChooseVisual(windowNative->display, 0, glxAttributes);
    
    colorMap = XCreateColormap(windowNative->display, root, windowNative->visualInfo->visual, AllocNone);
    
    swa.colormap = colorMap;
    
    /*if(windowNative->visualInfo == NULL)
    {
        printf("\n\tno appropriate visual found\n\n");
        exit(0);
    } 
    else
    {
        printf("\n\tvisual %p selected\n", (void *)windowNative->visualInfo->visualid);
    }*/
        
    /* Create the window. */
    
    /*printf("window is: %d %d\n", windowProperties.width, windowProperties.height);*/
    windowNative->window = XCreateWindow(windowNative->display, root, 0, 0, windowProperties.width, windowProperties.height, 0, windowNative->visualInfo->depth, InputOutput, windowNative->visualInfo->visual, CWColormap | CWEventMask, &swa);

    //windowNative->window = XCreateWindow(windowNative->display, root, 0, 0, windowProperties.width, windowProperties.height, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);
    
    if(windowNative->window == NULL)
    {
        printf("window is null\n");
    }
   
    /* Enable full screen mode. */
    /*{
        Atom wm_state = XInternAtom(windowNative->display, "_NET_WM_STATE", False);
        Atom fullscreen = XInternAtom(windowNative->display, "_NET_WM_STATE_FULLSCREEN", False);
        XChangeProperty(windowNative->display, windowNative->window, wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char*)&fullscreen, 1);
    }*/

    /* Disable window composition. */
    /*{
        int one = 1;
        Atom non_composited = XInternAtom(windowNative->display, "_HILDON_NON_COMPOSITED_WINDOW", False);        
        XChangeProperty(windowNative->display, windowNative->window, non_composited, XA_INTEGER, 32, PropModeReplace, (unsigned char*)&one, 1);
    }*/
    
    /* Set PID */
    /*{
        Atom pidatom = XInternAtom(windowNative->display, "_NET_WM_PID", False);
        pid_t pid = getpid();
        XChangeProperty(windowNative->display, windowNative->window, pidatom, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&pid, 1);
    }*/
  
    /*{
        XClassHint wmclass;
        wmclass.res_name = "Kanzi";
        wmclass.res_class = "Kanzi";
        XSetClassHint(windowNative->display, windowNative->window, &wmclass);
    }*/
    
    XMapWindow(windowNative->display, windowNative->window);
    
    XStoreName(windowNative->display, windowNative->window, "Kanzi v2.0");

    /* Enable keyboard input for the window. */
    {
        XWMHints inputHints;
        inputHints.input = True;
        inputHints.flags = InputHint;
        XSetWMHints(windowNative->display, windowNative->window, &inputHints);
    }
    
    *out_windowProperties = windowProperties;
    *out_windowNative = windowNative;
#endif
    kzsSuccess();
}

kzsError kzsWindowNativeCreateEmpty(struct KzsWindowNative** out_windowNative)
{
    struct KzsWindowNative* windowNative = kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");

    *out_windowNative = windowNative;
    kzsSuccess();
}

kzsError kzsWindowNativeInitialize(struct KzsWindowNative* windowNative, struct KzsWindow* window)
{
    kzsSuccess();
}

kzsError kzsWindowNativeUpdate(const struct KzsWindow* window)
{
    kzsError result;
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    struct KzsInputEventQueue* queue = kzsWindowGetInputEventQueue(window);
    struct KzsWindowNative* windowNative = kzsWindowGetNative(window);
    Display* display = kzsWindowNativeGetDisplay(windowNative);
    

    XEvent event;
    kzInt eventFocusLostCount = 0;
    kzInt eventFocusGainedCount = 0;
    kzsWindowEventHandlerFunction eventHandler = kzsWindowGetEventHandler(window);
    
    if (windowNative->windowFocused == KZ_FALSE)
    {
        /* Halts the application until an event occurs. */
        /*XPeekEvent(display, &event);*/
    }
    
    /* TODO: handle multiple windows properly */
    
    while (XPending(display))
    {
        XNextEvent(display, &event);
        
        
        switch (event.type)
        {
            case EnterNotify:
            {
                eventFocusGainedCount++;
                windowNative->windowFocused = KZ_TRUE;
            }
            break;
            
            case LeaveNotify:
            {
                eventFocusLostCount++;
                windowNative->windowFocused = KZ_FALSE;
            }
            break;
            
            case ButtonPress:
            {

                struct KzsInputEventQueue* queue = kzsWindowGetInputEventQueue(window);

                kzsAssert(queue != KZ_NULL);
                
             
                kzsInputEventQueueAddPointingDeviceEvent(queue, 0, event.xbutton.x, event.xbutton.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, KZS_POINTING_DEVICE_STATE_DOWN);

/*
                struct KzsPointingDeviceInputData* inputData = kzsWindowSetActivePointer(window, 0);
                if(inputData != KZ_NULL)
                {
                    kzsInputSetPointingDeviceInputData(inputData, 0, event.xbutton.x, event.xbutton.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, KZS_POINTING_DEVICE_STATE_DOWN);
                }*/

            }
            break;
            
            case ButtonRelease:
            {
                struct KzsInputEventQueue* queue = kzsWindowGetInputEventQueue(window);
                
                kzsAssert(queue != KZ_NULL);
                

                kzsInputEventQueueAddPointingDeviceEvent(queue, 0, event.xbutton.x, event.xbutton.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, KZS_POINTING_DEVICE_STATE_UP);
/*
                struct KzsPointingDeviceInputData* inputData = kzsWindowSetActivePointer(window, 0);
                if(inputData != KZ_NULL)
                {
                    kzsInputSetPointingDeviceInputData(inputData, 0, event.xbutton.x, event.xbutton.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, KZS_POINTING_DEVICE_STATE_UP);
                }
*/

            }            
            break;
            
            case MotionNotify:
            {
/*
                struct KzsInputEventQueue* queue = kzsWindowGetInputEventQueue(window);
                
                kzsAssert(queue != KZ_NULL);
                
                
                kzsInputEventQueueAddPointingDeviceEvent(queue, 0, event.xbutton.x, event.xbutton.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, KZS_POINTING_DEVICE_STATE_DRAG);*/

                struct KzsPointingDeviceInputData* inputData = kzsWindowSetActivePointer(window, 0);
                if(inputData != KZ_NULL)
                {
                    kzsInputSetPointingDeviceInputData(inputData, 0, event.xbutton.x, event.xbutton.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, KZS_POINTING_DEVICE_STATE_DRAG);
                }

            }
            break;
            
            case KeyPress:
            {
                struct KzsInputEventQueue* queue = kzsWindowGetInputEventQueue(window);

                kzsAssert(queue != KZ_NULL);

                kzsInputEventQueueAddKeyDeviceEvent(queue, (kzUint)event.xkey.keycode, KZS_KEY_DEVICE_STATE_DOWN);
#ifdef DEBUG                
                printf("Key pressed %d\n", (int)XKeycodeToKeysym(display, event.xkey.keycode, 0));
#endif                
            }
            break;
            
            case KeyRelease:
            {
                struct KzsInputEventQueue* queue = kzsWindowGetInputEventQueue(window);

                kzsAssert(queue != KZ_NULL);

                kzsInputEventQueueAddKeyDeviceEvent(queue, (kzUint)event.xkey.keycode, KZS_KEY_DEVICE_STATE_UP);
#ifdef DEBUG
                printf("Key released %d\n", (int)event.xkey.keycode);
#endif      
            }
            break;
            
            default:
            /*printf("unhandled event %d\n", event.type);*/
            break;
        }                                 
    }
    
    /* There may be multiple focus change events in the buffer, only call the application functions once. */
    if (eventFocusGainedCount > 0 || eventFocusLostCount > 0)
    {
        if (windowNative->windowFocused)
        {
            kzsWindowSetState(window, KZS_WINDOW_STATE_ACTIVE);
            result = eventHandler(window, KZS_WINDOW_EVENT_FOCUS_GAINED, kzsWindowGetUserData(window));        
            kzsErrorForward(result);
        }
        else
        {
            kzsWindowSetState(window, KZS_WINDOW_STATE_INACTIVE);
            result = eventHandler(window, KZS_WINDOW_EVENT_FOCUS_LOST, kzsWindowGetUserData(window));        
            kzsErrorForward(result);
        }
    }
#endif    
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
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    XStoreName(windowNative->display, windowNative->window,  titleText);
#endif
    kzsSuccess();
}

kzsError kzsWindowNativeDelete(struct KzsWindowNative* windowNative)
{
    kzsFree(windowNative);
    kzsSuccess();
}
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
Display* kzsWindowNativeGetDisplay(const struct KzsWindowNative* windowNative)
{
    Display* display;

    if (windowNative->display == KZ_NULL)
    {
        display = KZ_NULL;
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

    if (windowNative->window == KZ_NULL)
    {
        window = KZ_NULL;
    }
    else
    {
        window = windowNative->window;
    }

    return windowNative->window;
}

XVisualInfo* kzsWindowNativeGetVisualInfo(const struct KzsWindowNative* windowNative)
{
    return windowNative->visualInfo;
}
#endif
