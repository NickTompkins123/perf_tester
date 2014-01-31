/**
* \file
* Application framework. Platform specific implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZA_ANDROID_APPLICATION_INTERFACE_H
#define KZA_ANDROID_APPLICATION_INTERFACE_H

void doNothing(void);

/** Get the kzaApplication struct that describes the currently running Kanzi Engine */
struct KzaApplication *kzaAndroidGetApplication(void);

#endif
