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
