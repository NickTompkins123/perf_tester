/**
* \file
* Application framework. Platform specific implementation.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kza_qnx_application_interface.h"

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
    kzInt dummy;
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
