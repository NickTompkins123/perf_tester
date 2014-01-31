/**
* \file
* Application framework. Platform specific implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/

#include "kza_android_application_interface.h"

#include <application/kza_application_interface.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>

#include <system/wrappers/kzs_memory.h>

/* struct KzaApplication; */
#include <application/kza_application.h>

#include <android/log.h>


struct KzaPlatform
{
    /* system */
    kzInt dummy;
};

kzsError kzaApplicationPlatformCreate(struct KzaApplication* application, struct KzaPlatform** out_platform)
{
    kzsError result;
    struct KzaPlatform* platform;
    struct KzcMemoryManager* memoryManager;

    memoryManager = (struct KzcMemoryManager*)kzaApplicationGetApplicationMemoryManager(application);
    result = kzcMemoryAllocVariable(memoryManager, platform, "Application framework platform");
    kzsErrorForward(result);
    
    __android_log_write(ANDROID_LOG_INFO, "kza_android_application_interface", "kzaApplicationPlatformCreate");     
    
    /* calling this is needed so the kza_android_main.c is included in the final .so as well.. */
    /* TODO: find a more elegant solution */
    doNothing();

    *out_platform = platform;
    kzsSuccess();
}

kzsError kzaApplicationPlatformDelete(struct KzaApplication* application, struct KzaPlatform* platform)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;

    memoryManager = (struct KzcMemoryManager*)kzaApplicationGetApplicationMemoryManager(application);
    result = kzcMemoryFreeVariable(platform);

    kzsSuccess();
}
