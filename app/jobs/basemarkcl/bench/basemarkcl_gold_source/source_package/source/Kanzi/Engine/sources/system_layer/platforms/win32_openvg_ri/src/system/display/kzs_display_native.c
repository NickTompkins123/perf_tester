/**
* \file
* Kanzi display, native portion for the win32 platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_display_native.h"

#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>


/** Platform specific display variables. */
struct KzsDisplayNative
{
    kzInt dummy;    /**< Dummy content. */
};


kzsError kzsDisplayNativeCreate(struct KzsDisplayNative** out_displayNative)
{
    struct KzsDisplayNative* displayNative = (struct KzsDisplayNative*)kzsMalloc(sizeof(*displayNative));
    kzsErrorTest(displayNative != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for native display.");

    /* Nothing to initialize at the moment. */

    *out_displayNative = displayNative;
    kzsSuccess();
}

kzsError kzsDisplayNativeDelete(struct KzsDisplayNative* displayNative)
{
    kzsAssert(displayNative);
    kzsFree(displayNative);

    kzsSuccess();
}
