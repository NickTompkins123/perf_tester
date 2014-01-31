/* 
* Utility functions for OpenCL kernels
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CLU_KERNEL_H
#define CLU_KERNEL_H

/*
Contains functions for running kernels and setting their arguments. Basically all the things which are required after initialization.
*/
#include "clu_opencl_base.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* TODO: Format comment properly. */
/*Set arguments for supplied kernel.
The arguments are provided as size_t arg1_size,const void* arg1_value, size_t arg2_size... etc
Amount of arguments is queried with clGetKernelInfo with flag CL_KERNEL_NUM_ARGS. Calling this function with extra arguments is safe as the extra is simply ignored.
*/
kzsError cluSetKernelArguments(cl_kernel kernel,...);


#endif
