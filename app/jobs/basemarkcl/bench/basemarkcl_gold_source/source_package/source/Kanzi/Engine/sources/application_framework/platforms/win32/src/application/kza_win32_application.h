/**
* \file
* Application framework. win32 specific implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZA_WIN32_APPLICATION_H
#define KZA_WIN32_APPLICATION_H

#include <windows.h>
#include <system/kzs_types.h>

struct KzaApplication;
struct KzaPlatform;

/** Get win32 specific window handle. */
HINSTANCE kzaPlatformGetInstanceHandle(struct KzaApplication* application);
/** Set win32 specific window handle. */
void kzaPlatformSetInstanceHandle(struct KzaApplication* application, HINSTANCE hinstance);

#endif
