/* 
* OpenCL sharpening filter 
*
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_SHARPENING_H
#define CL_SHARPENING_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include <clutil/clu_opencl_base.h>


/* Forward declarations. */
struct BenchmarkFramework;
struct SharpeningFilter;


kzsError initSharpening(struct BenchmarkFramework* framework, struct BfScene* scene, cl_context context, struct SharpeningFilter **out_filter);

kzsError applySharpening(cl_command_queue queue, struct SharpeningFilter *filter, cl_mem summationImage, cl_mem inputImage, cl_mem outputImage);

kzsError deleteSharpening(struct SharpeningFilter *filter);


#endif
