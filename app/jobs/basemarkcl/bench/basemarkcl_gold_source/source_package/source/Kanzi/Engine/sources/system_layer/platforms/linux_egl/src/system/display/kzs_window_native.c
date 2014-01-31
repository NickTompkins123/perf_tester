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


struct KzsWindow;


/** Platform specific surface variables. */
struct KzsWindowNative
{
    kzInt dummy;
};

kzsError kzsWindowNativeCreate(const struct KzsWindowProperties* windowPropertiesRequested, struct KzsDisplay* display, struct KzsWindowProperties* out_windowProperties, struct KzsWindowNative** out_windowNative)
{
    struct KzsWindowProperties windowProperties;
    struct KzsWindowNative* windowNative = kzsMalloc(sizeof(*windowNative));
    kzsErrorTest(windowNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native window.");

    /* Get a copy of requested properties and modify it. */
    windowProperties = *windowPropertiesRequested;
    
    /* In command line Linux we don't have multiple windows, therefore force the window size to match the display. */
    windowProperties.width = kzsDisplayGetCurrentWidth(display);
    windowProperties.height = kzsDisplayGetCurrentHeight(display);

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
    /* TODO: fix these.. init input devices somewhere */
#if 0    
    /* Handle pending messages. */
    int bytesRead = 0;

    /* Keyboard/serial console */
    if(inputDeviceKeyboard)
    {
        /* TODO: needs verification/fixing.. */
        int keyCode = 0;
        bytesRead = read(inputDeviceKeyboard, &keyCode, 1);
        if(bytesRead > 0)
        {
            printf("pressed key: %02x\n", keyCode);
        }
    }

    /* Keypad */
    if(inputDeviceKeypadButtons)
    {
        kzsInputKeyDeviceHandlerFunction keyDeviceHandler = kzsWindowGetKeyDeviceHandler(window);
        struct inputEvent keypadInput;
        bytesRead = read(inputDeviceKeypadButtons, &keypadInput, sizeof(struct inputEvent));
        if(bytesRead == sizeof(struct inputEvent) && keypadInput.type == 0x01)
        {
            /* press or hold */
            {
                if(keypadInput.value == 1 || keypadInput.value == 2)
                {
                    struct KzsKeyDeviceInputData* inputData;
                    kzsWindowSetButtonData(window, (kzUint)keypadInput.code, KZS_KEY_DEVICE_STATE_DOWN);
                    inputData = kzsWindowGetButtonInputData(window);
                    keyDeviceHandler(inputData);
                }

                /* release */
                if(keypadInput.value == 0)
                {
                    struct KzsKeyDeviceInputData* inputData;
                    kzsWindowSetButtonData(window, (kzUint)keypadInput.code, KZS_KEY_DEVICE_STATE_UP);
                    inputData = kzsWindowGetButtonInputData(window);
                    keyDeviceHandler(inputData);
                }
            }
        }
    }

    /* Touch screen. */
    if(inputDeviceTouchScreen)
    {
        kzsInputPointingDeviceHandlerFunction pointingDeviceHandler = kzsWindowGetPointingDeviceHandler(window);
        static int pressed = 0;
        static int lastPress = 0;
        struct ts_sample sample;
        int ret;
        ret = ts_read(inputDeviceTouchScreen, &sample, 1);
        if(ret == 1)
        {
            struct KzsPointingDeviceInputData* inputData;
            struct KzsInputPointingDevice* inputPointingDevice = kzsWindowGetInputPointingDevice(window);
            enum KzsInputPointingDeviceState state = KZS_POINTING_DEVICE_STATE_DRAG;
            if(pressed == 0)
            {
                state = KZS_POINTING_DEVICE_STATE_DOWN;
            }

            kzsWindowSetActivePointer(window, 0);
            kzsInputSetPointingDeviceInputData(kzsInputGetPointingDeviceData(inputPointingDevice), 0, sample.x, sample.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, state);
            inputData = kzsWindowGetPointingDeviceData(window);
            pointingDeviceHandler(inputData);

            pressed = 1;
        }
        else
        {
            if(pressed == 1 && lastPress == 0)
            {
                struct KzsPointingDeviceInputData* inputData;
                struct KzsInputPointingDevice* inputPointingDevice = kzsWindowGetInputPointingDevice(window);
                kzsWindowSetActivePointer(window, 0);
                kzsInputSetPointingDeviceInputData(kzsInputGetPointingDeviceData(inputPointingDevice), 0, sample.x, sample.y, 0.0f, KZS_POINTING_DEVICE_BUTTON_PRIMARY, KZS_POINTING_DEVICE_STATE_UP);
                inputData = kzsWindowGetPointingDeviceData(window);
                pointingDeviceHandler(inputData);

                pressed = 0;
            }
        }

        lastPress = ret;
    }
#endif    /* #if 0 */
    kzsSuccess();
}

kzsError kzsWindowNativeDelete(struct KzsWindowNative* windowNative)
{
    kzsFree(windowNative);
    kzsSuccess();
}
