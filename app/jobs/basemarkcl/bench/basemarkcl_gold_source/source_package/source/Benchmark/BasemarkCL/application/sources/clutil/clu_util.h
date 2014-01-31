/**
* \file
* OpenCL utility functions
*
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CLU_UTIL_H
#define CLU_UTIL_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include "clu_opencl_base.h"


#ifdef _DEBUG
#define DEBUG_OPENCL 1
#else
#define DEBUG_OPENCL 0
#endif


/** Outputs to log the type of error encountered. */
void cluClErrorTest(cl_int errorCode);


#endif
