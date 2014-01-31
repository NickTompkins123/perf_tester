/* 
* OpenCL Gaussian blur filter 
*
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_BLUR_H
#define CL_BLUR_H

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include <clutil/clu_opencl_base.h>


/* Forward declarations. */
struct BenchmarkFramework;
struct BlurFilter;


kzsError initBlur(struct BenchmarkFramework* framework, struct BfScene* scene, cl_context context, cl_mem inputImage, struct BlurFilter **out_filter);

kzsError applyBlur(cl_command_queue queue, struct BlurFilter *filter, cl_mem inputImage, cl_mem outputImage);

kzsError deleteBlur(struct BlurFilter *filter);


#endif
