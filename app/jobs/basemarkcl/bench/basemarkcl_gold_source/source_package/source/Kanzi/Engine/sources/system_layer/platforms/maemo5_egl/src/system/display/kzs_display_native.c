/**
* \file
* Kanzi display, native portion for the Maemo platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_display_native.h"

#include <system/display/kzs_display.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>

#include <stdio.h>

/** Platform specific display variables. */
struct KzsDisplayNative
{
    Display* display;       /**< The X11 display. */
    XWindowAttributes attributes;     /**< Display attributes. */
};

kzsError kzsDisplayNativeCreate(kzUint displayID, struct KzsDisplayNative** out_displayNative)
{
    /*kzsError result;*/
    struct KzsDisplayNative* displayNative = kzsMalloc(sizeof(*displayNative));

    displayNative->display = XOpenDisplay(NULL);
    
    /* Store root window (basically the display) attributes. */
    XGetWindowAttributes(displayNative->display, DefaultRootWindow(displayNative->display), &displayNative->attributes);

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
    /* Only one display supported in maemo devices. */
    kzUint numDisplays = 1;
    
    *out_numDisplays = numDisplays;

    kzsSuccess();
}

kzUint kzsDisplayGetCurrentWidth(const struct KzsDisplay* display)
{
    struct KzsDisplayNative* displayNative;
    displayNative = kzsDisplayGetNative(display);
    
    return (kzUint)displayNative->attributes.width;
}

kzUint kzsDisplayGetCurrentHeight(const struct KzsDisplay* display)
{
    struct KzsDisplayNative* displayNative;
    displayNative = kzsDisplayGetNative(display);
    
    return (kzUint)displayNative->attributes.height;
}

Display* kzsDisplayNativeGetX11Display(struct KzsDisplayNative* displayNative)
{
    kzsAssert(displayNative != KZ_NULL);
    
    return displayNative->display;
}

