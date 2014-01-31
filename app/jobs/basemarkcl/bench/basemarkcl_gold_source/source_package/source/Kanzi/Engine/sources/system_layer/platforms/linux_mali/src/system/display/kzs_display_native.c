/**
* \file
* Kanzi display, native portion for the Linux platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_display_native.h"

#include <system/display/kzs_display.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>

#define WIDESCREEN

/** Platform specific display variables. */
struct KzsDisplayNative
{
    kzUint displayWidth;    /**< Screen resolution, width. */
    kzUint displayHeight;   /**< Screen resolution, height. */
};

kzsError kzsDisplayNativeCreate(kzUint displayID, struct KzsDisplayNative** out_displayNative)
{
    /*kzsError result;*/
    struct KzsDisplayNative* displayNative = kzsMalloc(sizeof(*displayNative));

    /* TODO: find out the native resolution of the display.. read from /dev/fb? */
    /* why not just take the biggest resolution that EGL can offer? --J */
    displayNative->displayWidth = 800;
    displayNative->displayHeight = 480;

#ifdef WIDESCREEN
    displayNative->displayWidth = 1280;
    displayNative->displayHeight = 720;
#endif
    
    *out_displayNative = displayNative;
    kzsSuccess();
}

kzsError kzsDisplayNativeDelete(struct KzsDisplayNative* displayNative)
{
    kzsAssert(displayNative != KZ_NULL);

    kzsFree(displayNative);

    kzsSuccess();
}

kzsError kzsDisplayNativeGetNumDisplays(kzUint* out_numDisplays)
{    
    /* TODO: read the actual number of displays */
    kzUint numDisplays = 1;

    *out_numDisplays = numDisplays;

    kzsSuccess();
}

kzUint kzsDisplayGetCurrentWidth(const struct KzsDisplay* display)
{
    struct KzsDisplayNative* displayNative;
    kzsAssert(display != NULL);

    displayNative = kzsDisplayGetNative(display);

    return displayNative->displayWidth;
}

kzUint kzsDisplayGetCurrentHeight(const struct KzsDisplay* display)
{
    struct KzsDisplayNative* displayNative;
    kzsAssert(display != NULL);

    displayNative = kzsDisplayGetNative(display);

    return displayNative->displayHeight;
}
