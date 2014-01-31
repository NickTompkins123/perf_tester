/**
* \file
* Display. Linux specific part of the implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>

#include "kzs_display_native.h"


struct KzsDisplayNative
{
    kzUint displayWidth;    /**< Screen resolution, width. */
    kzUint displayHeight;   /**< Screen resolution, height. */
};

kzsError kzsDisplayNativeCreate(struct KzsDisplayNative** out_displayNative)
{
    /*kzsError result;*/
    struct KzsDisplayNative* displayNative = kzsMalloc(sizeof(*displayNative));

    /* TODO: find out the native resolution of the display.. read from /dev/fb? */
    displayNative->displayWidth = 800;
    displayNative->displayHeight = 480;

    *out_displayNative = displayNative;
    kzsSuccess();
}
