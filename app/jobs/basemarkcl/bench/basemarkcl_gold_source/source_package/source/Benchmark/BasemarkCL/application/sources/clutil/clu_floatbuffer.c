/**
 * \file
 * OpenCL float buffer
 * Copyright 2011 by Rightware. All rights reserved.
 */
#include "clu_floatbuffer.h"

#include <clutil/clu_util.h>

#include <core/memory/kzc_memory_manager.h>

#include <system/wrappers/kzs_memory.h>

#include "clu_opencl_base.h"


kzsError cluFloatBufferCreate(const struct KzcMemoryManager* memoryManager, kzUint size, cl_context context, struct FloatBuffer** out_buffer)
{
    kzsError result;
    struct FloatBuffer* buffer;

    result = kzcMemoryAllocVariable(memoryManager, buffer, "FloatBuffer");
    kzsErrorForward(result);

    buffer->size = size * sizeof(kzFloat);
    result = kzcMemoryAllocPointer(memoryManager, &buffer->data, buffer->size, "Buffer data");
    kzsErrorForward(result);
    kzsMemset(buffer->data, 0, buffer->size);

    if(context != KZ_NULL)
    {
        cl_int errorCode;
        buffer->buffer = clCreateBuffer(context, (cl_mem_flags)CL_MEM_COPY_HOST_PTR, buffer->size, buffer->data, &errorCode);
        cluClErrorTest(errorCode);
    }
    else
    {
        buffer->buffer = KZ_NULL;
    }

    *out_buffer = buffer;
    kzsSuccess();
}

kzsError cluFloatBufferDelete(struct FloatBuffer* floatBuffer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(floatBuffer));

    if(floatBuffer->buffer != KZ_NULL)
    {
        cl_int value = clReleaseMemObject(floatBuffer->buffer);
        cluClErrorTest(value);
    }

    result = kzcMemoryFreePointer(floatBuffer->data);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(floatBuffer);
    kzsErrorForward(result);

    kzsSuccess();
}
