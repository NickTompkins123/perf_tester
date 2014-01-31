/**
 * \file
 * OpenCL float buffer
 * Copyright 2011 by Rightware. All rights reserved.
 */
#ifndef CLU_FLOATBUFFER_H
#define CLU_FLOATBUFFER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include "clu_opencl_base.h"


/* Forward declarations. */
struct KzcMemoryManager;


/** Float buffer. CL and RAM versions of data. */
struct FloatBuffer
{
    cl_mem buffer;
    kzFloat* data;
    kzUint size;
};


/** Create float buffer. Passing context as KZ_NULL disables cl_mem object creation. */
kzsError cluFloatBufferCreate(const struct KzcMemoryManager* memoryManager, kzUint size, cl_context context, struct FloatBuffer** out_buffer);
/** Delete float buffer. */
kzsError cluFloatBufferDelete(struct FloatBuffer* floatBuffer);


#endif
