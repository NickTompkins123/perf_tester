/**
* \file
* Display device.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_display.h"

#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>


struct KzsDisplay
{
    struct KzsDisplayNative*        displayNative;          /**< Platform specific data. */
    
    /* TODO: Add display properties here. */
};

kzsError kzsDisplayCreateFromNative(struct KzsDisplayNative* displayNative, const struct KzsDisplayConfiguration* displayConfiguration, 
                                    struct KzsDisplay** out_display)
{
    struct KzsDisplay* display;
    
    kzsAssert(displayNative != KZ_NULL);
    kzsAssert(displayConfiguration != KZ_NULL);

    display = (struct KzsDisplay*)kzsMalloc(sizeof(*display));
    kzsErrorTest(display != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for display.");

    KZ_UNUSED_PARAMETER(displayConfiguration);

    display->displayNative = displayNative;

    *out_display = display;
    kzsSuccess();
}

kzsError kzsDisplayCreate(kzUint displayID, const struct KzsDisplayConfiguration* displayConfiguration, struct KzsDisplay** out_display)
{
    kzsError result;
    struct KzsDisplayNative* displayNative;
    struct KzsDisplay* display;

    kzsAssert(displayConfiguration != KZ_NULL);

    result = kzsDisplayNativeCreate(displayID, &displayNative);
    kzsErrorForward(result);

    result = kzsDisplayCreateFromNative(displayNative, displayConfiguration, &display);
    kzsErrorForward(result);

    *out_display = display;
    kzsSuccess();
}

kzsError kzsDisplayDelete(struct KzsDisplay* display)
{
    kzsError result;
    kzsAssert(display != KZ_NULL);

    result = kzsDisplayNativeDelete(display->displayNative);
    kzsErrorForward(result);

    kzsFree(display);
    
    kzsSuccess();
}

struct KzsDisplayNative* kzsDisplayGetNative(const struct KzsDisplay* display)
{
    kzsAssert(display != KZ_NULL);

    return display->displayNative;
}
