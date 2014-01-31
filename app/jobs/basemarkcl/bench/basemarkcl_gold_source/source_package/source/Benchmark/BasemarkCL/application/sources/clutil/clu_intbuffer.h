/**
 * \file
 * OpenCL int buffer
 * Copyright 2011 by Rightware. All rights reserved.
 */
#ifndef CLU_INTBUFFER_H
#define CLU_INTBUFFER_H

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include "clu_opencl_base.h"


/* Forward declarations. */
struct KzcMemoryManager;


/** Int buffer. CL and RAM versions of data. */
struct IntBuffer
{
    cl_mem buffer;
    kzInt* data;
    kzUint size;
};


/** Create int buffer. Passing context as KZ_NULL disables cl_mem object creation. */
kzsError cluIntBufferCreate(const struct KzcMemoryManager* memoryManager, kzUint size, cl_context context, struct IntBuffer** out_buffer);
/** Delete int buffer. */
kzsError cluIntBufferDelete(struct IntBuffer* intBuffer);


#endif
