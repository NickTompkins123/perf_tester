/**
* \file
* Kanzi display, native portion for the Tegra2 (openKODE) platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_display_native.h"

#include <system/display/kzs_window.h>
#include <system/display/kzs_display.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/debug/kzs_log.h>

#include <KD/kd.h>
#include <KD/NV_display.h>


/** Platform specific display variables. */
struct KzsDisplayNative
{
    kzUint displayWidth;    /**< Screen resolution, width. */
    kzUint displayHeight;   /**< Screen resolution, height. */
};


#ifndef EXTERNAL_DISPLAY_MANAGER
kzsError kzsDisplayNativeCreate(kzUint displayID, struct KzsDisplayNative** out_displayNative)
{
    struct KzsDisplayNative* displayNative;

    displayNative = (struct KzsDisplayNative*)kzsMalloc(sizeof(*displayNative));
    kzsErrorTest(displayNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native display.");

    /* TODO: replace with display_init() */
#ifndef EXTERNAL_DISPLAY_MANAGER
    {
        KDboolean enabled = KD_TRUE;
        KDboolean power = KD_DISPLAY_POWER_ON;

        KDDesktopNV* kdDesktop = KD_NULL;
        KDDisplayNV* kdDisplay = KD_NULL;

        kdDesktop = kdGetDesktopNV(KD_DEFAULT_DESKTOP_NV, KD_NULL);
        kzsErrorTest((kdDesktop != NULL), KZS_ERROR_DISPLAY_INITIALIZATION_FAILED, "Unable to init KD desktop.");

        kdDisplay = kdGetDisplayNV(KD_DEFAULT_DISPLAY_NV, KD_NULL);
        /*kdDisplay = kdGetDisplayNV("Tegra:HDMI0", KD_NULL);*/
        kzsErrorTest((kdDisplay != NULL), KZS_ERROR_DISPLAY_INITIALIZATION_FAILED, "Unable to init KD display.");

        kdSetDisplayPropertybvNV(kdDisplay, KD_DISPLAYPROPERTY_ENABLED_NV, &enabled);
        kdSetDisplayPropertyivNV(kdDisplay, KD_DISPLAYPROPERTY_POWER_NV, &power);
        kdSetDisplayPropertycvNV(kdDisplay, KD_DISPLAYPROPERTY_DESKTOP_NAME_NV, KD_DEFAULT_DESKTOP_NV);

        {
            KDDisplayModeNV mode;
            mode.width = 800;
            mode.height = 480;
            mode.refresh = 60;
            kdSetDisplayModeNV(kdDisplay, &mode, KD_DISPLAY_PROTOCOL_AUTOMATIC_NV);
        }

        kdReleaseDisplayNV(kdDesktop);
        kdReleaseDesktopNV(kdDisplay);
    }
#endif

    /* TODO: find out the native resolution of the display.. */
    
    displayNative->displayWidth = 800;
    displayNative->displayHeight = 480;
#ifdef EXTERNAL_DISPLAY_MANAGER
    kzsLog(KZS_LOG_LEVEL_DETAIL, "display_init() start");
    /*display_init(&displayNative->displayWidth, &displayNative->displayHeight);*/
    kzsLog(KZS_LOG_LEVEL_DETAIL, "display_init() end");
#endif

    *out_displayNative = displayNative;
    kzsSuccess();
}
#endif

kzsError kzsDisplayNativeDelete(struct KzsDisplayNative* displayNative)
{
    kzsAssert(displayNative != KZ_NULL);

    kzsFree(displayNative);

    kzsSuccess();
}

kzsError kzsDisplayNativeGetNumDisplays(kzUint* out_numDisplays)
{    
    kzUint numDisplays = 1;

    *out_numDisplays = numDisplays;

    kzsSuccess();
}

kzUint kzsDisplayGetCurrentWidth(const struct KzsDisplay* display)
{
    return 800;
}

kzUint kzsDisplayGetCurrentHeight(const struct KzsDisplay* display)
{
    return 480;
}
