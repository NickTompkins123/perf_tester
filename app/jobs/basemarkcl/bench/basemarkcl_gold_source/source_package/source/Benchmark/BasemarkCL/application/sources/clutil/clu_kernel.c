/* 
* Utility functions for OpenCL kernels
* Copyright 2011 by Rightware. All rights reserved.
*/

#include "clu_kernel.h"

#include <clutil/clu_util.h>

#include <stdarg.h>


kzsError cluSetKernelArguments(cl_kernel kernel,...)
{
    va_list arguments; 
    cl_int clResult;
    cl_uint numargs;
    cl_uint curarg = 0;
    
    clResult = clGetKernelInfo(kernel, CL_KERNEL_NUM_ARGS, sizeof(numargs), (void*)&numargs, NULL);    
    cluClErrorTest(clResult);

    va_start(arguments, kernel);

    while(curarg < numargs)
    {
        size_t size = /*lint -e(662,826)*/ va_arg(arguments, size_t);
        void* argp = /*lint -e(662,826,661)*/ va_arg(arguments, void*);
        clResult = clSetKernelArg(kernel, curarg++, size, argp);
        cluClErrorTest(clResult);
    }
    
    va_end(arguments);

    kzsSuccess();
}
