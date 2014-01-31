/* 
* OpenCL histogram based color correction
*
* Copyright 2011 by Rightware. All rights reserved.
*/

#ifndef CL_HISTOGRAM_H
#define CL_HISTOGRAM_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include <clutil/clu_opencl_base.h>


/* Forward declarations. */
struct BenchmarkFramework;
struct HistogramFilter;


kzsError initHistogram(struct BenchmarkFramework* framework, struct BfScene* scene, cl_context context, struct HistogramFilter **out_filter);

kzsError applyHistogram(cl_command_queue queue, struct HistogramFilter *filter, cl_mem inputImage, cl_mem outputImage);

kzsError deleteHistogram(struct HistogramFilter *filter);


#endif
