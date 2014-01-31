/**
* \file
* Display device.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_DISPLAY_H
#define KZS_DISPLAY_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzsDisplay;
struct KzsDisplayNative;


/** Display configuration struct. */
struct KzsDisplayConfiguration
{
    kzInt dummy;    /**< Dummy setting. */
};


/** Creates a display with a previously created native display. */
kzsError kzsDisplayCreateFromNative(struct KzsDisplayNative* displayNative, const struct KzsDisplayConfiguration* displayConfiguration, 
                                    struct KzsDisplay** out_display);

/** Creates a display and also the native display. */
kzsError kzsDisplayCreate(kzUint displayID, const struct KzsDisplayConfiguration* displayConfiguration, struct KzsDisplay** out_display);

/** Deletes a display and frees all memory allocated for it. */
kzsError kzsDisplayDelete(struct KzsDisplay* display);

/** Creates the platform independent portion of the display. */
kzsError kzsDisplayNativeCreate(kzUint displayID, struct KzsDisplayNative** out_displayNative);

/** Deletes the platform independent portion of the display. */
kzsError kzsDisplayNativeDelete(struct KzsDisplayNative* displayNative);

/** Gets number of available displays on the device. */
kzsError kzsDisplayNativeGetNumDisplays(kzUint* out_numDisplays);

/** Returns the native display struct. */
struct KzsDisplayNative* kzsDisplayGetNative(const struct KzsDisplay* display);

/** Returns the current width of the display in pixels. */
kzUint kzsDisplayGetCurrentWidth(const struct KzsDisplay* display);

/** Returns the current height of the display in pixels. */
kzUint kzsDisplayGetCurrentHeight(const struct KzsDisplay* display);

#endif
