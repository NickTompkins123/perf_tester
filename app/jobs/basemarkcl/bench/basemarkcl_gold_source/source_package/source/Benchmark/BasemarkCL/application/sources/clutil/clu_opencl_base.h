/**
* \file
* OpenCL base header.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef CLU_OPENCL_BASE_H
#define CLU_OPENCL_BASE_H


#include <system/kzs_types.h>


#define CLU_OPENCL_1_1   1 /**< OpenCL 1.1 version. */
#define CLU_OPENCL_NONE  2 /**< No OpenCL version specified. Use empty functions. */

#ifdef ANDROID
#define CLU_OPENCL_VERSION CLU_OPENCL_NONE
#else
#define CLU_OPENCL_VERSION CLU_OPENCL_1_1
#endif


#ifndef CLU_OPENCL_VERSION
#error CLU_OPENCL_VERSION undefined
#endif

#if CLU_OPENCL_VERSION == CLU_OPENCL_1_1
#include <CL/cl.h>
#include <CL/cl_gl.h>
#else
#include "clu_opencl_none.h"
#include "clu_opencl_gl_none.h"
#endif


#endif
