/**
* \file
* OpenCL utility functions
*
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "clu_util.h"


#if DEBUG_OPENCL
#include <system/debug/kzs_log.h>

#include "clu_opencl_base.h"


void cluClErrorTest(cl_int error)
{
    switch(error)
    {
        case CL_SUCCESS:
        {
            break;
        }
        case CL_DEVICE_NOT_FOUND:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_DEVICE_NOT_FOUND");
            break;
        }
        case CL_DEVICE_NOT_AVAILABLE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_DEVICE_NOT_AVAILABLE");
            break;
        }
        case CL_COMPILER_NOT_AVAILABLE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_COMPILER_NOT_AVAILABLE");
            break;
        }
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_MEM_OBJECT_ALLOCATION_FAILURE");
            break;
        }
        case CL_OUT_OF_RESOURCES:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_OUT_OF_RESOURCES");
            break;
        }
        case CL_OUT_OF_HOST_MEMORY:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_OUT_OF_HOST_MEMORY");
            break;
        }
        case CL_PROFILING_INFO_NOT_AVAILABLE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_PROFILING_INFO_NOT_AVAILABLE");
            break;
        }
        case CL_MEM_COPY_OVERLAP:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_MEM_COPY_OVERLAP");
            break;
        }
        case CL_IMAGE_FORMAT_MISMATCH:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_IMAGE_FORMAT_MISMATCH");
            break;
        }
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_IMAGE_FORMAT_NOT_SUPPORTED");
            break;
        }
        case CL_BUILD_PROGRAM_FAILURE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_BUILD_PROGRAM_FAILURE");
            break;
        }
        case CL_MAP_FAILURE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_MAP_FAILURE");
            break;
        }
        case CL_MISALIGNED_SUB_BUFFER_OFFSET:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_MISALIGNED_SUB_BUFFER_OFFSET");
            break;
        }
        case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST");
            break;
        }
        case CL_INVALID_VALUE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_VALUE");
            break;
        }
        case CL_INVALID_DEVICE_TYPE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_DEVICE_TYPE");
            break;
        }
        case CL_INVALID_PLATFORM:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_PLATFORM");
            break;
        }
        case CL_INVALID_DEVICE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_DEVICE");
            break;
        }
        case CL_INVALID_CONTEXT:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_CONTEXT");
            break;
        }
        case CL_INVALID_QUEUE_PROPERTIES:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_QUEUE_PROPERTIES");
            break;
        }
        case CL_INVALID_COMMAND_QUEUE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_COMMAND_QUEUE");
            break;
        }
        case CL_INVALID_HOST_PTR:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_HOST_PTR");
            break;
        }
        case CL_INVALID_MEM_OBJECT:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_MEM_OBJECT");
            break;
        }
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR");
            break;
        }
        case CL_INVALID_IMAGE_SIZE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_IMAGE_SIZE");
            break;
        }
        case CL_INVALID_SAMPLER:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_SAMPLER");
            break;
        }
        case CL_INVALID_BINARY:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_BINARY");
            break;
        }
        case CL_INVALID_BUILD_OPTIONS:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_BUILD_OPTIONS");
            break;
        }
        case CL_INVALID_PROGRAM:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_PROGRAM");
            break;
        }
        case CL_INVALID_PROGRAM_EXECUTABLE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_PROGRAM_EXECUTABLE");
            break;
        }
        case CL_INVALID_KERNEL_NAME:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_KERNEL_NAME");
            break;
        }
        case CL_INVALID_KERNEL_DEFINITION:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_KERNEL_DEFINITION");
            break;
        }
        case CL_INVALID_KERNEL:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_KERNEL");
            break;
        }
        case CL_INVALID_ARG_INDEX:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_ARG_INDEX");
            break;
        }
        case CL_INVALID_ARG_VALUE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_ARG_VALUE");
            break;
        }
        case CL_INVALID_ARG_SIZE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_ARG_SIZE");
            break;
        }
        case CL_INVALID_KERNEL_ARGS:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_KERNEL_ARGS");
            break;
        }
        case CL_INVALID_WORK_DIMENSION:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_WORK_DIMENSION");
            break;
        }
        case CL_INVALID_WORK_GROUP_SIZE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_WORK_GROUP_SIZE");
            break;
        }
        case CL_INVALID_WORK_ITEM_SIZE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_WORK_ITEM_SIZE");
            break;
        }
        case CL_INVALID_GLOBAL_OFFSET:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_GLOBAL_OFFSET");
            break;
        }
        case CL_INVALID_EVENT_WAIT_LIST:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_EVENT_WAIT_LIST");
            break;
        }
        case CL_INVALID_EVENT:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_EVENT");
            break;
        }
        case CL_INVALID_OPERATION:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_OPERATION");
            break;
        }
        case CL_INVALID_GL_OBJECT:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_GL_OBJECT");
            break;
        }
        case CL_INVALID_BUFFER_SIZE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_BUFFER_SIZE");
            break;
        }
        case CL_INVALID_MIP_LEVEL:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_MIP_LEVEL");
            break;
        }
        case CL_INVALID_GLOBAL_WORK_SIZE:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_GLOBAL_WORK_SIZE");
            break;
        }
        case CL_INVALID_PROPERTY:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "CL_INVALID_PROPERTY");
            break;
        }
        default: 
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Unknown OpenCL error");
            break;
        }
    }

    if(error != CL_SUCCESS)
    {
        kzsAssert(KZ_FALSE);
    }
}

#else
void cluClErrorTest(cl_int error)
{
KZ_UNUSED_PARAMETER(error);
}
#endif
