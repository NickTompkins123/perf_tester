/* OpenCL Platform Layer utility functions

Copyright 2011 by Rightware. All rights reserved.
*/

#include "clu_platform.h"

#include <clutil/clu_util.h>


#include "clu_opencl_base.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef _GNUC_
#include <GL/gl.h>
#include <GL/glx.h>
#endif

#ifdef ANDROID
#include <EGL/egl.h>
#endif 

#include <system/wrappers/kzs_opengl_base.h>

#include <core/debug/kzc_log.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h> 
#include <core/util/collection/kzc_hash_map.h>

#include <system/display/kzs_surface.h>
#include <system/debug/kzs_log.h>
#include <system/debug/kzs_error.h>

#include <stdio.h>


#define MAXIMUM_PLATFORMS 255
#define STRING_MAX_LENGTH 1024




kzsError cluInfoStoreValue(struct KzcHashMap* hashMap, kzString key, kzString valueFormat, ...)
{
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(hashMap);
    kzsError result;
    kzArgList arguments;
    kzMutableString keyLowerCase;

    result = kzcStringAsciiToLowerCase(memoryManager, key, &keyLowerCase);
    kzsErrorForward(result);

    va_start(arguments, valueFormat);
    {
        kzMutableString message;
        result = kzcStringFormatList(memoryManager, valueFormat, &arguments, &message);
        kzsErrorForward(result);

        result = kzcHashMapPut(hashMap, keyLowerCase, message);
        kzsErrorForward(result);
    }
    va_end(arguments);

    kzsSuccess();
}

kzsError cluInfoCreate(const struct KzcMemoryManager* memoryManager, struct CluInfo** out_info)
{
    kzsError result;
    struct CluInfo* info;

    result = kzcMemoryAllocVariable(memoryManager, info, "CL platform information container");
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &info->platforms);
    kzsErrorForward(result);

    /* Fill cl info. */
    {
        kzUint i;
        kzUint platformCount;
        cl_platform_id platforms[MAXIMUM_PLATFORMS];
        
        result = clGetPlatformIDs(MAXIMUM_PLATFORMS, platforms, &platformCount);
        cluClErrorTest(result);

        kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Available OpenCL platforms %d: ", platformCount);

        for(i = 0; i < platformCount; ++i)
        {
            kzUint n;
            kzUint deviceCount;
            cl_device_id devices[256];
            struct CluPlatformInfo* platformInfo;

            result = kzcMemoryAllocVariable(memoryManager, platformInfo, "CL platform information");
            kzsErrorForward(result);

            platformInfo->id = i;
            result = kzcStringCreate(memoryManager, STRING_MAX_LENGTH, &platformInfo->name);
            kzsErrorForward(result);
            result = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, STRING_MAX_LENGTH, platformInfo->name, KZ_NULL);
            cluClErrorTest(result);
            result = kzcStringCreate(memoryManager, STRING_MAX_LENGTH, &platformInfo->version);
            kzsErrorForward(result);
            result = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, STRING_MAX_LENGTH, platformInfo->version, KZ_NULL);
            cluClErrorTest(result);
            result = kzcStringCreate(memoryManager, STRING_MAX_LENGTH, &platformInfo->vendor);
            kzsErrorForward(result);
            result = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, STRING_MAX_LENGTH, platformInfo->vendor, KZ_NULL);
            cluClErrorTest(result);
            result = kzcStringCreate(memoryManager, STRING_MAX_LENGTH, &platformInfo->profile);
            kzsErrorForward(result);
            result = clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, STRING_MAX_LENGTH, platformInfo->profile, KZ_NULL);
            cluClErrorTest(result);

            result = kzcDynamicArrayAdd(info->platforms, platformInfo);
            kzsErrorForward(result);

            kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, " Platform %d:", (i));
            kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "  Name: %s", platformInfo->name);
            kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "  Profile: %s", platformInfo->profile);
            kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "  Version: %s", platformInfo->version);
            kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "  Vendor: %s", platformInfo->vendor);

            result = kzcDynamicArrayCreate(memoryManager, &platformInfo->devices);
            kzsErrorForward(result);
            
            clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 256, devices, &deviceCount);

            kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "  Available devices for platform devices %d: ", deviceCount);

            for(n = 0; n < deviceCount; ++n)
            {
                kzUint vendorId;
                struct CluDeviceInfo* deviceInfo;

                result = kzcMemoryAllocVariable(memoryManager, deviceInfo, "CL device information");
                kzsErrorForward(result);

                result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &deviceInfo->deviceConfig);
                kzsErrorForward(result);

                deviceInfo->id = n;
                result = kzcStringCreate(memoryManager, STRING_MAX_LENGTH, &deviceInfo->name);
                kzsErrorForward(result);
                result = clGetDeviceInfo(devices[n], CL_DEVICE_NAME, STRING_MAX_LENGTH, deviceInfo->name, KZ_NULL);
                cluClErrorTest(result);
                result = clGetDeviceInfo(devices[n], CL_DEVICE_VENDOR_ID, sizeof(vendorId), &vendorId, KZ_NULL);
                cluClErrorTest(result);
                result = kzcStringFormat(memoryManager, "Vendor ID: %u", &deviceInfo->vendorId, vendorId);
                kzsErrorForward(result);
                result = kzcStringCreate(memoryManager, STRING_MAX_LENGTH, &deviceInfo->vendor);
                kzsErrorForward(result);
                result = clGetDeviceInfo(devices[n], CL_DEVICE_VENDOR, STRING_MAX_LENGTH, deviceInfo->vendor, KZ_NULL);
                cluClErrorTest(result);
                result = kzcStringCreate(memoryManager, STRING_MAX_LENGTH, &deviceInfo->deviceVersion);
                kzsErrorForward(result);
                result = clGetDeviceInfo(devices[n], CL_DEVICE_VERSION, STRING_MAX_LENGTH, deviceInfo->deviceVersion, KZ_NULL);
                cluClErrorTest(result);
                result = kzcStringCreate(memoryManager, STRING_MAX_LENGTH, &deviceInfo->driverVersion);
                kzsErrorForward(result);
                result = clGetDeviceInfo(devices[n], CL_DRIVER_VERSION, STRING_MAX_LENGTH, deviceInfo->driverVersion, KZ_NULL);
                cluClErrorTest(result);

                {
                    cl_device_type deviceType;
                    result = clGetDeviceInfo(devices[n], CL_DEVICE_TYPE, sizeof(deviceType), &deviceType, KZ_NULL);
                    cluClErrorTest(result);
                    result = kzcStringCreateEmpty(memoryManager, &deviceInfo->deviceType);
                    kzsErrorForward(result);

                    if((deviceType & CL_DEVICE_TYPE_CPU) != 0)
                    {
                        kzMutableString str = deviceInfo->deviceType;
                        kzcStringFormat(memoryManager, "%s %s", &deviceInfo->deviceType, str, "CPU ");
                        result = kzcStringDelete(str);
                        kzsErrorForward(result);
                    }
                    if((deviceType & CL_DEVICE_TYPE_GPU) != 0)
                    {
                        kzMutableString str = deviceInfo->deviceType;
                        kzcStringFormat(memoryManager, "%s %s", &deviceInfo->deviceType, str, "GPU ");
                        result = kzcStringDelete(str);
                        kzsErrorForward(result);
                    }
                    if((deviceType & CL_DEVICE_TYPE_ACCELERATOR) != 0)
                    {
                        kzMutableString str = deviceInfo->deviceType;
                        kzcStringFormat(memoryManager, "%s %s", &deviceInfo->deviceType, str, "Accelerator ");
                        result = kzcStringDelete(str);
                        kzsErrorForward(result);
                    }
                    if((deviceType & CL_DEVICE_TYPE_DEFAULT) != 0)
                    {
                        kzMutableString str = deviceInfo->deviceType;
                        kzcStringFormat(memoryManager, "%s %s", &deviceInfo->deviceType, str, "Default");
                        result = kzcStringDelete(str);
                        kzsErrorForward(result);
                    }
                }

                kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "   Device %d:", (n));
                kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    Name: %s", deviceInfo->name);
                kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    Vendor: %s", deviceInfo->vendor);
                kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    Vendor ID: %s", deviceInfo->vendorId);
                kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    Device version: %s", deviceInfo->deviceVersion);
                kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    Driver version: %s", deviceInfo->driverVersion);
                kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    Device type: %s", deviceInfo->deviceType);
        
                {
                    size_t item;
                    result = clGetDeviceInfo(devices[n], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(item), &item, KZ_NULL);
                    cluClErrorTest(result);
                    kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    CL_DEVICE_MAX_WORK_GROUP_SIZE        %d", item);
                    result = cluInfoStoreValue(deviceInfo->deviceConfig, "CL_DEVICE_MAX_WORK_GROUP_SIZE", "%d", item);
                    kzsErrorForward(result);
                }

                {
                    cl_uint item;
                    result = clGetDeviceInfo(devices[n], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(item), &item, KZ_NULL);
                    cluClErrorTest(result);
                    kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS   %d", item);
                    result = cluInfoStoreValue(deviceInfo->deviceConfig, "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS", "%d", item);
                    kzsErrorForward(result);
                }
                {
                    cl_uint item;
                    result = clGetDeviceInfo(devices[n], CL_DEVICE_MAX_SAMPLERS, sizeof(item), &item, KZ_NULL);
                    cluClErrorTest(result);
                    kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    CL_DEVICE_MAX_SAMPLERS               %d", item);
                    result = cluInfoStoreValue(deviceInfo->deviceConfig, "CL_DEVICE_MAX_SAMPLERS", "%d", item);
                    kzsErrorForward(result);
                }
                {
                    cl_bool item;
                    result = clGetDeviceInfo(devices[n], CL_DEVICE_IMAGE_SUPPORT, sizeof(item), &item, KZ_NULL);
                    cluClErrorTest(result);
                    kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    CL_DEVICE_IMAGE_SUPPORT              %d", item ? 1 : 0);
                    result = cluInfoStoreValue(deviceInfo->deviceConfig, "CL_DEVICE_IMAGE_SUPPORT", "%d", item);
                    kzsErrorForward(result);
                }
                {
                    size_t item;
                    result = clGetDeviceInfo(devices[n], CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(item), &item, KZ_NULL);
                    cluClErrorTest(result);
                    kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    CL_DEVICE_IMAGE2D_MAX_WIDTH          %d", (kzUint)item);
                    result = cluInfoStoreValue(deviceInfo->deviceConfig, "CL_DEVICE_IMAGE2D_MAX_WIDTH", "%d", item);
                    kzsErrorForward(result);
                }
                {
                    size_t item;
                    result = clGetDeviceInfo(devices[n], CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(item), &item, KZ_NULL);
                    cluClErrorTest(result);
                    kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    CL_DEVICE_IMAGE2D_MAX_HEIGHT         %d", (kzUint)item);
                    result = cluInfoStoreValue(deviceInfo->deviceConfig, "CL_DEVICE_IMAGE2D_MAX_HEIGHT", "%d", item);
                    kzsErrorForward(result);
                }
                {
                    cl_bool item;
                    result = clGetDeviceInfo(devices[n], CL_DEVICE_COMPILER_AVAILABLE, sizeof(item), &item, KZ_NULL);
                    cluClErrorTest(result);
                    kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    CL_DEVICE_COMPILER_AVAILABLE         %d", item ? 1 : 0);
                    result = cluInfoStoreValue(deviceInfo->deviceConfig, "CL_DEVICE_COMPILER_AVAILABLE", "%d", item);
                    kzsErrorForward(result);
                }
                {
                    cl_uint item;
                    result = clGetDeviceInfo(devices[n], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(item), &item, KZ_NULL);
                    cluClErrorTest(result);
                    kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "    CL_DEVICE_MAX_COMPUTE_UNITS          %d", item);
                    result = cluInfoStoreValue(deviceInfo->deviceConfig, "CL_DEVICE_MAX_COMPUTE_UNITS", "%d", item);
                    kzsErrorForward(result);
                }

                /* TODO: Add all available info here. */

                result = kzcDynamicArrayAdd(platformInfo->devices, deviceInfo);
                kzsErrorForward(result);
            }
        }
    }

    *out_info = info;
    kzsSuccess();
}

kzsError cluInfoDelete(struct CluInfo* info)
{
    kzsError result;

    /* Free items in array. */
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(info->platforms);
        while (kzcDynamicArrayIterate(it))
        {
            struct CluPlatformInfo* platformInfo = (struct CluPlatformInfo*)kzcDynamicArrayIteratorGetValue(it);
            struct KzcDynamicArrayIterator it2 = kzcDynamicArrayGetIterator(platformInfo->devices);

            while (kzcDynamicArrayIterate(it2))
            {
                struct CluDeviceInfo* deviceInfo = (struct CluDeviceInfo*)kzcDynamicArrayIteratorGetValue(it2);
                {
                    struct KzcHashMapIterator it = kzcHashMapGetIterator(deviceInfo->deviceConfig);

                    while(kzcHashMapIterate(it))
                    {
                        kzMutableString key = kzcHashMapIteratorGetKey(it);
                        kzMutableString value = kzcHashMapIteratorGetValue(it);
                        result = kzcStringDelete(key);
                        kzsErrorForward(result);
                        result = kzcStringDelete(value);
                        kzsErrorForward(result);
                    }
                    result = kzcHashMapDelete(deviceInfo->deviceConfig);
                    kzsErrorForward(result);
                }

                result = kzcStringDelete(deviceInfo->driverVersion);
                kzsErrorForward(result);
                result = kzcStringDelete(deviceInfo->deviceVersion);
                kzsErrorForward(result);
                result = kzcStringDelete(deviceInfo->vendorId);
                kzsErrorForward(result);
                result = kzcStringDelete(deviceInfo->vendor);
                kzsErrorForward(result);
                result = kzcStringDelete(deviceInfo->name);
                kzsErrorForward(result);
                result = kzcStringDelete(deviceInfo->deviceType);
                kzsErrorForward(result);
                result = kzcMemoryFreeVariable(deviceInfo);
                kzsErrorForward(result);
            }
            result = kzcDynamicArrayDelete(platformInfo->devices);
            kzsErrorForward(result);

            result = kzcStringDelete(platformInfo->name);
            kzsErrorForward(result);
            result = kzcStringDelete(platformInfo->profile);
            kzsErrorForward(result);
            result = kzcStringDelete(platformInfo->vendor);
            kzsErrorForward(result);
            result = kzcStringDelete(platformInfo->version);
            kzsErrorForward(result);
            result = kzcMemoryFreeVariable(platformInfo);
            kzsErrorForward(result);
        }
    }
    result = kzcDynamicArrayDelete(info->platforms);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(info);
    kzsErrorForward(result);

    kzsSuccess();
}



kzsError cluCreateGPUcontext(const struct KzcMemoryManager* memoryManager, const struct KzsSurfaceNative* nativeSurface, kzUint platformIndex, kzInt *deviceIndexP, cl_context* outcontext)
{
    kzsError result;
    kzUint platformCount;
    cl_context context;
    kzUint propertyIndex = 0;
    cl_device_id devices[2] = {0};
    cl_device_id gpuDevice = NULL;
    cl_context_properties properties[7] = {0};
    cl_platform_id platforms[MAXIMUM_PLATFORMS];
    cl_device_type deviceType = CL_DEVICE_TYPE_ALL;
    kzInt deviceIndex = *deviceIndexP;
    result = clGetPlatformIDs(MAXIMUM_PLATFORMS, platforms, &platformCount);

    cluClErrorTest(result);
    kzsErrorTest(platformIndex < platformCount, KZS_ERROR_ILLEGAL_ARGUMENT, "Platform index out of bounds.");






    properties[propertyIndex++] = CL_CONTEXT_PLATFORM;
    properties[propertyIndex++] = (cl_context_properties) platforms[platformIndex];

    if(deviceIndex == -2) /* Fetch a GPU device */
    {
        kzUint deviceCount;
        cl_device_id devicesIds[256];
        cl_int retVal;
        kzInt i;

        retVal = clGetDeviceIDs(platforms[platformIndex], CL_DEVICE_TYPE_GPU, 1, &gpuDevice, &deviceCount);        
        retVal = clGetDeviceIDs(platforms[platformIndex], CL_DEVICE_TYPE_ALL, 256, devicesIds, &deviceCount);
        deviceIndex = 0;
        for(i = 0; i < deviceCount; i++)
        {
            if(gpuDevice == devicesIds[i])
            {
                deviceIndex = i;
            }
        }
        devices[0] = devicesIds[deviceIndex];
        
    }
    else   
    /* Fetch device pointer. */
    {
        kzUint deviceCount;
        cl_device_id devicesIds[256];
        cl_int retVal;
        retVal = clGetDeviceIDs(platforms[platformIndex], deviceType, 256, devicesIds, &deviceCount);

        
        kzsErrorTest(deviceIndex < deviceCount, KZS_ERROR_ILLEGAL_ARGUMENT, "Platform index out of bounds.");
        
        devices[0] = devicesIds[deviceIndex];
    }


#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    if(nativeSurface != KZ_NULL)
    {
#if defined(_WIN32)
         HDC hdc = wglGetCurrentDC();
         HGLRC hglrc =  wglGetCurrentContext();
         properties[propertyIndex++] = CL_GL_CONTEXT_KHR;
         properties[propertyIndex++] = (cl_context_properties) hglrc;    
         properties[propertyIndex++] = CL_WGL_HDC_KHR;
         properties[propertyIndex++] = (cl_context_properties) hdc;
#elif defined(ANDROID)
       /* void* eglContext = kzsSurfaceNativeGetContext(nativeSurface);
        void* eglDisplay = kzsSurfaceNativeGetDisplay(nativeSurface);*/

        properties[propertyIndex++] = CL_GL_CONTEXT_KHR;
        properties[propertyIndex++] = (cl_context_properties) eglGetCurrentContext();
        properties[propertyIndex++] = CL_EGL_DISPLAY_KHR;
        properties[propertyIndex++] = (cl_context_properties) eglGetCurrentDisplay();

#else
	/* TODO: ifdef for X11 and opengl*/
        properties[propertyIndex++] = CL_GL_CONTEXT_KHR;
        properties[propertyIndex++] = (cl_context_properties) glXGetCurrentContext();    
        properties[propertyIndex++] = CL_GLX_DISPLAY_KHR;
        properties[propertyIndex++] = (cl_context_properties) glXGetCurrentDisplay();
#endif   
    }
#endif
    kzsLog(KZS_LOG_LEVEL_INFO, "");
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Used OpenCL platform index: %d", platformIndex);
    kzsErrorForward(result);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Used OpenCL device index: %d", deviceIndex);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_INFO, "");

    context = clCreateContext(properties, 1, devices, KZ_NULL, KZ_NULL, &result);
    cluClErrorTest(result);
    *deviceIndexP = deviceIndex;
    *outcontext = context;
    kzsSuccess();
}

