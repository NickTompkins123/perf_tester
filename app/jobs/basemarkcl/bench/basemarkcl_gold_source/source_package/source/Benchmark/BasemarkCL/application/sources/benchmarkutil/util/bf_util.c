/**
* \file
* Benchmark framework utility functions.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/

#include "bf_util.h"

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/screenshot/bf_screenshot.h>
#include <benchmarkutil/settings/bf_settings.h>

#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/debug/kzs_error.h>
#include <system/file/kzs_file_base.h>
#include <system/wrappers/kzs_opengl_base.h>

#include <stdio.h>


kzString bfGLApiName()
{
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0
    static kzString apiname = "GL_ES_20";
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    static kzString apiname = "GL_ES_11";
#else
    static kzString apiname = "GL_NONE";
#endif
    return apiname;
}

kzString bfVGApiName()
{
#ifdef KZ_OPENVG_VERSION
    static kzString apiname = "OPENVG_11";
#else
    static kzString apiname = "OPENVG_NONE";
#endif
    return apiname;
}

kzsError bfGetResourcePath( const struct KzcMemoryManager* manager, kzString path, kzMutableString* out_path )
{
    kzsError result;

    kzString resourceDirectory = kzsFileBaseGetResourceDirectory();

    kzMutableString fullPath;
    if (resourceDirectory != KZ_NULL)
    {

        result = kzcStringConcatenateMultiple(manager, 3, &fullPath, resourceDirectory, "/", path);
        kzsErrorForward(result);
    }
    else
    {
        result = kzcStringCopy(manager, path, &fullPath);
        kzsErrorForward(result);
    }

    *out_path = fullPath;
    kzsSuccess();
}

kzsError bfUtilReadLocalWorkgroupSize(const struct BenchmarkFramework* framework, kzString propertyName, kzUint dimensions, size_t** out_workgroupSizes)
{
    size_t* localWorkSize;

    /* TODO: Re-enable if we decide to use workgroup sizes from settings again. */
    
    kzsError result;
    kzInt workSize;
/*
    struct KzcSettingContainer* settings = bfGetSettings(framework);
    
    result = settingGetInt(settings, propertyName, &workSize);
    //kzsErrorForward(result);
    if(result != KZS_SUCCESS)
    {
        *out_workgroupSizes = NULL;
        kzsSuccess();
    }
    
    if(workSize == 0)
    {
        localWorkSize = KZ_NULL;            
    }
    else
    {
        kzUint i;

        result = kzcMemoryAllocArray(bfGetMemoryManager(framework), localWorkSize, dimensions, "Local work size");
        kzsErrorForward(result);

        for(i = 0; i < dimensions; ++i)
        {
            localWorkSize[i] = (size_t)workSize;
        }
    }
    */

    localWorkSize = KZ_NULL;

    *out_workgroupSizes = localWorkSize;
    kzsSuccess();
}

kzsError bfUtilFreeLocalWorkgroupSize(size_t* workgroupSizes)
{
    kzsError result;

    if(workgroupSizes != KZ_NULL)
    {
        result = kzcMemoryFreeArray(workgroupSizes);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
