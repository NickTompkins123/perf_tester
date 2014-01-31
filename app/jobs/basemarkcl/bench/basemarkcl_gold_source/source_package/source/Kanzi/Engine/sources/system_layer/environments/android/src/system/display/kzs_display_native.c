/**
* \file
* Kanzi display, native portion for the Android platform.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_display_native.h"

#include <system/android.h>

#include <system/display/kzs_display.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>

#include <android/log.h>
#include <jni.h>


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

    /* Get screen size */
    {
        jmethodID widthMethod;
        jmethodID heightMethod;
        jint width;
        jint height;
        JNIEnv *env;

        /* query resolution from Android platform */
        (*g_javaVM)->AttachCurrentThread(g_javaVM, &env, NULL);
       
        widthMethod = (*env)->GetMethodID(env, g_View_class, "getScreenWidth", "()I");
        heightMethod = (*env)->GetMethodID(env, g_View_class, "getScreenHeight", "()I");        
        
        width = (*env)->CallIntMethod(env, g_view, widthMethod);
        height = (*env)->CallIntMethod(env, g_view, heightMethod);

        displayNative->displayWidth = width;
        displayNative->displayHeight = height;

        /* Galaxy S
        displayNative->displayWidth = 800;
        displayNative->displayHeight = 480;
        */
    }

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
