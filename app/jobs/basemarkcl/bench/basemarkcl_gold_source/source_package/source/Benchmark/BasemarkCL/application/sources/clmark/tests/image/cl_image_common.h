/**
* \file
* OpenCL benchmark image and video test common utilities.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_IMAGE_COMMON_H
#define CL_IMAGE_COMMON_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#include <clutil/clu_opencl_base.h>


/* Forward declarations. */
struct BenchmarkFramework;
struct MedianFilter;


/** Types of tests. */
enum ImageTestType
{
    IMAGE_TEST_TYPE_SMOOTHING,
    IMAGE_TEST_TYPE_COLOR_CORRECTION,
    IMAGE_TEST_TYPE_NOISE_REDUCTION,
    IMAGE_TEST_TYPE_SURFACE_SMOOTHING,
    IMAGE_TEST_TYPE_SHARPENING
};


/** Assignt texture to test label object. */
kzsError clImageCommonAssignTexture(struct BenchmarkFramework* framework, enum ImageTestType type);


#endif
