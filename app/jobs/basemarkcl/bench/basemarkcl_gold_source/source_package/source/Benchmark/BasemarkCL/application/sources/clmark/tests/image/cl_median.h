/* 
* OpenCL median filter 
*
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_MEDIAN_H
#define CL_MEDIAN_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include <clutil/clu_opencl_base.h>


/* Forward declarations. */
struct BenchmarkFramework;
struct MedianFilter;


kzsError initMedian(struct BenchmarkFramework* framework, struct BfScene* scene, cl_context context, struct MedianFilter **out_filter);

kzsError applyMedian(cl_command_queue queue, struct MedianFilter *filter, cl_mem inputImage, cl_mem outputImage);

kzsError deleteMedian(struct MedianFilter *filter);


#endif
