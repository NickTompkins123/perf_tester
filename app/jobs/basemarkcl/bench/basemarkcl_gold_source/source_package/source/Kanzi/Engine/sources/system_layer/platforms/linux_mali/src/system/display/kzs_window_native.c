/**
* \file
* Kanzi window, native portion for the Linux platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_window_native.h"

#include <system/display/kzs_display.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/display/kzs_egl.h>

#include <system/debug/kzs_log.h>

#include <tslib.h>

#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef _DEBUG
#include <stdio.h>
#endif

#define INPUTDEVICE_USB_KEYBOARD "/dev/input/event3"
#define INPUTDEVICE_BUTTONS "/dev/input/event0"
#define INPUTDEVICE_TOUCHSCREEN "/dev/input/event1"

struct KzsWindow;


/** Platform specific surface variables. */
struct KzsWindowNative
{
    struct fbdev_window *framebuffer;
    struct tsdev* touchscreen;
    int pointer_x, pointer_y;
    int keyboard_fd, button_fd;
};

kzsError kzsWindowNativeCreate(const struct KzsWindowProperties* windowPropertiesRequested, struct KzsDisplay* display, struct KzsWindowProperties* out_windowProperties, struct KzsWindowNative** out_windowNative)
{
    struct KzsWindowProperties windowProperties;
    struct KzsWindowNative* windowNative = kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");
    
    /* Get a copy of requested properties and modify it. */
    windowProperties = *windowPropertiesRequested;    

    /* if a zero was supplied, go to full screen mode */
    if (windowProperties.width == 0 || windowProperties.height == 0) {
        windowProperties.width = kzsDisplayGetCurrentWidth(display);
        windowProperties.height = kzsDisplayGetCurrentHeight(display);
    }

    /* initialize the structure describing the Mali framebuffer */
    windowNative->framebuffer = (struct fbdev_window *)kzsMalloc(sizeof(struct fbdev_window));
    windowNative->framebuffer->width = windowProperties.width;
    windowNative->framebuffer->height = windowProperties.height;
    
    /* initialize touchscreen with tslib */
    windowNative->touchscreen = ts_open(INPUTDEVICE_TOUCHSCREEN, 1);
    if (windowNative->touchscreen)
    {
        if (ts_config(windowNative->touchscreen))
        {
            kzsLog(KZS_LOG_LEVEL_ERROR, "Touchscreen config failed.");
        }
    }
    else
    {
        kzsLog(KZS_LOG_LEVEL_ERROR, "Touchscreen init failed.");
    }
    
    /* open usb keyboard device file */
    windowNative->keyboard_fd = open(INPUTDEVICE_USB_KEYBOARD, O_NONBLOCK);
    if (windowNative->keyboard_fd == -1) {
        kzsLog(KZS_LOG_LEVEL_ERROR, "Opening keyboard device failed.");
    }

    /* open keypad / on-board button device file */
    windowNative->button_fd = open(INPUTDEVICE_BUTTONS, O_NONBLOCK);
    if (windowNative->keyboard_fd == -1) {
        kzsLog(KZS_LOG_LEVEL_ERROR, "Opening on-board button device failed.");
    }
    
    kzsLog(KZS_LOG_LEVEL_DEBUG, "WindowNative created");
    
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

kzsError kzsWindowNativeInitialize( struct KzsWindowNative* windowNative,struct KzsWindow* window)
{
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

kzsError kzsWindowNativeUpdate(const struct KzsWindow* window)
{
    struct KzsWindowNative *windowNative;
    kzsError result;
    
    windowNative = kzsWindowGetNative(window);
    
    /* Touch screen */
    
    if (windowNative->touchscreen)
    {
        kzsInputPointingDeviceHandlerFunction pointingDeviceHandler = kzsWindowGetPointingDeviceHandler(window);
        static int pressed = 0;
        struct ts_sample sample;
        int ret;
        ret = ts_read(windowNative->touchscreen, &sample, 1);
        if (ret >= 1)
        {
            struct KzsPointingDeviceInputData* inputData;
            struct KzsInputPointingDevice* inputPointingDevice;
            enum KzsInputPointingDeviceState state;
            
            inputPointingDevice = kzsWindowGetInputPointingDevice(window);
            
            if (sample.pressure > 0) {
                if (pressed == 0)
                {
                    state = KZS_POINTING_DEVICE_STATE_DOWN;
#ifdef _DEBUG                    
                    kzsLog(KZS_LOG_LEVEL_DEBUG, "Touch screen pressed");
#endif                    
                }
                else
                {
                    state = KZS_POINTING_DEVICE_STATE_DRAG;
/*#ifdef _DEBUG
                    {
                        char buf[100];
                        sprintf(buf, "  Dragging, dx,dy = (%d, %d)", sample.x-windowNative->pointer_x, sample.y-windowNative->pointer_y);
                        kzsLog(KZS_LOG_LEVEL_DEBUG, buf);
                    }
#endif*/
                }
                pressed = 1;
            }
            else
            {
                if (pressed == 0)
                {
                       /* shouldn't be here ever */
                    state = KZS_POINTING_DEVICE_STATE_UP;
                }
                else
                {
                    state = KZS_POINTING_DEVICE_STATE_UP;
#ifdef _DEBUG
                    kzsLog(KZS_LOG_LEVEL_DEBUG, "Touch screen released");
#endif
                }
                pressed = 0;
            }

            windowNative->pointer_x = sample.x;
            windowNative->pointer_y = sample.y;      
            
            kzsWindowSetActivePointer(window, 0);
            kzsInputSetPointingDeviceInputData(kzsInputGetPointingDeviceData(inputPointingDevice), 0, windowNative->pointer_x, windowNative->pointer_y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, state);        
            inputData = kzsWindowGetPointingDeviceData(window);
            result = pointingDeviceHandler(inputData, kzsWindowGetUserData(window));
        }
    }
    
    /* USB keyboard */

    if (windowNative->keyboard_fd > -1)
    {
        struct input_event ev[64];
        int s_ev = sizeof(struct input_event);
        int n, nb, extra;
        int i = -1;

        nb = read(windowNative->keyboard_fd, ev, s_ev);
        if (nb > -1) 
        {
            extra = nb % s_ev;            
            if (extra > 0) kzsLog(KZS_LOG_LEVEL_DEBUG, "Short read");
            
            n = (nb - extra) / s_ev;
            /*if (n > 0) kzsLog(KZS_LOG_LEVEL_DEBUG, "Keyboard event");*/
            for (i = 0; i < n; i++) 
            {
                if (EV_KEY == ev[i].type) 
                {
                    kzsInputKeyDeviceHandlerFunction keyDeviceHandler = kzsWindowGetKeyDeviceHandler(window);
                    
                    /*kzcLogDebug("  Key %d %s", ev[i].code, ev[i].value ? "released" : "pressed");*/
                    #ifdef _DEBUG
                        if (ev[i].value == 0)
                            printf("  Key %d released", ev[i].code);
                        else
                            printf("  Key %d pressed", ev[i].code);
                    #endif
                    
                    if (ev[i].value > 0)
                    {
                        struct KzsKeyDeviceInputData* inputData;
                        kzsWindowSetButtonData(window, (kzUint)ev[i].code, KZS_KEY_DEVICE_STATE_DOWN);
                        inputData = kzsWindowGetButtonInputData(window);
                        result = keyDeviceHandler(inputData, kzsWindowGetUserData(window));
                    }
                    
                    if (ev[i].value == 0)
                    {
                        struct KzsKeyDeviceInputData* inputData;
                        kzsWindowSetButtonData(window, (kzUint)ev[i].code, KZS_KEY_DEVICE_STATE_UP);
                        inputData = kzsWindowGetButtonInputData(window);
                        result = keyDeviceHandler(inputData, kzsWindowGetUserData(window));
                    }
                }
            }
        }       
    }        
    
    /* On-board buttons */
    
    if (windowNative->button_fd > -1)
    {
        struct input_event ev[64];
        int s_ev = sizeof(struct input_event);
        int n, nb, extra;
        int i = -1;

        nb = read(windowNative->button_fd, ev, s_ev);
        if (nb > -1) 
        {printf("  meh");
            extra = nb % s_ev;            
            if (extra > 0) kzsLog(KZS_LOG_LEVEL_DEBUG, "Short read");
            
            n = (nb - extra) / s_ev;
            /*if (n > 0) kzsLog(KZS_LOG_LEVEL_DEBUG, "Button event");*/
            for (i = 0; i < n; i++) 
            {
                if (EV_KEY == ev[i].type) 
                {
                    kzsInputKeyDeviceHandlerFunction keyDeviceHandler = kzsWindowGetKeyDeviceHandler(window);
                    
                    /*kzcLogDebug("  Key %d %s", ev[i].code, ev[i].value ? "released" : "pressed");*/
                    #ifdef _DEBUG
                        printf("  Button %d", ev[i].code);
                    #endif
                    printf("  Button %d", ev[i].code);
                    if (ev[i].value == 1)
                    {
                        struct KzsKeyDeviceInputData* inputData;
                        kzsWindowSetButtonData(window, (kzUint)ev[i].code, KZS_KEY_DEVICE_STATE_DOWN);
                        inputData = kzsWindowGetButtonInputData(window);
                        result = keyDeviceHandler(inputData, kzsWindowGetUserData(window));
                    }
                    
                    if (ev[i].value == 0)
                    {
                        struct KzsKeyDeviceInputData* inputData;
                        kzsWindowSetButtonData(window, (kzUint)ev[i].code, KZS_KEY_DEVICE_STATE_UP);
                        inputData = kzsWindowGetButtonInputData(window);
                        result = keyDeviceHandler(inputData, kzsWindowGetUserData(window));
                    }
                }
            }
        }       
    }        

    kzsSuccess();
}

kzsError kzsWindowNativeDelete(struct KzsWindowNative* windowNative)
{
    if (windowNative->touchscreen)
    {
        ts_close(windowNative->touchscreen);
    }
    if (windowNative->keyboard_fd > -1)
    {
        close(windowNative->keyboard_fd);
    }
    if (windowNative->button_fd > -1)
    {
        close(windowNative->button_fd);
    }
    kzsFree(windowNative);
    kzsSuccess();
}

struct fbdev_window* kzsWindowNativeGetFramebuffer(struct KzsWindowNative* windowNative)
{
    return windowNative->framebuffer;
}
