/**
* \file
* Application framework. win32 specific implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kza_win32_application.h"

#include <application/kza_application.h>
#include <application/kza_application_interface.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>

#include <system/wrappers/kzs_memory.h>
#include <system/display/kzs_window.h>
#include <system/kzs_types.h>


struct KzaApplication;


struct KzaPlatform
{
    HINSTANCE instanceHandle;       /**< Windows specific application instance handle. */
};

kzsError kzaApplicationPlatformCreate(struct KzaApplication* application, struct KzaPlatform** out_platform)
{
    kzsError result;
    struct KzaPlatform *platform;
    struct KzcMemoryManager* memoryManager;

    memoryManager = kzaApplicationGetApplicationMemoryManager(application);
    result = kzcMemoryAllocVariable(memoryManager, platform, "Application framework platform");
    kzsErrorForward(result);

    *out_platform = platform;
    kzsSuccess();
}

kzsError kzaApplicationPlatformDelete(struct KzaApplication* application, struct KzaPlatform* platform)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;

    memoryManager = kzaApplicationGetApplicationMemoryManager(application);
    result = kzcMemoryFreeVariable(platform);

    kzsSuccess();
}

void kzaPlatformSetInstanceHandle(struct KzaApplication* application, HINSTANCE instanceHandle)
{
    struct KzaPlatform* platform;
    platform = kzaApplicationGetPlatform(application);

    platform->instanceHandle = instanceHandle;
}

HINSTANCE kzaPlatformGetInstanceHandle(struct KzaApplication* application)
{
    struct KzaPlatform* platform;
    platform = kzaApplicationGetPlatform(application);

    return platform->instanceHandle;
}
