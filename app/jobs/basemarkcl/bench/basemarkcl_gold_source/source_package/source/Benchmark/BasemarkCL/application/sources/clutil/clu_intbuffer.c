/**
 * \file
 * OpenCL int buffer
 * Copyright 2011 by Rightware. All rights reserved.
 */
#include "clu_intbuffer.h"

#include <clutil/clu_util.h>

#include <core/memory/kzc_memory_manager.h>

#include <system/wrappers/kzs_memory.h>

#include "clu_opencl_base.h"


kzsError cluIntBufferCreate(const struct KzcMemoryManager* memoryManager, kzUint size, cl_context context, struct IntBuffer** out_buffer)
{
    kzsError result;
    struct IntBuffer* buffer;

    result = kzcMemoryAllocVariable(memoryManager, buffer, "IntBuffer");
    kzsErrorForward(result);

    buffer->size = size * sizeof(kzInt);
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

kzsError cluIntBufferDelete(struct IntBuffer* intBuffer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(intBuffer));

    if(intBuffer->buffer != KZ_NULL)
    {
        cl_int value = clReleaseMemObject(intBuffer->buffer);
        cluClErrorTest(value);
    }

    result = kzcMemoryFreePointer(intBuffer->data);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(intBuffer);
    kzsErrorForward(result);

    kzsSuccess();
}
