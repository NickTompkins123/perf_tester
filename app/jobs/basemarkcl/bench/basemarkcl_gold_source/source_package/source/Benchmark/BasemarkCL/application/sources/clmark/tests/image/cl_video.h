/**
* \file
* OpenCL benchmark video tests.
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_VIDEO_TESTS_H
#define CL_VIDEO_TESTS_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;


/** Create tests which run only the individual video benchmarks. */

/* Creates video test with sharpening effect */
kzsError sharpeningVideoCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);

/* Creates video test with gaussian blur effect */
kzsError blurVideoCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);

/* Creates video test with color correction effect */
kzsError histogramVideoCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);

/* Creates video test with surface smoothing effect */
kzsError bilateralVideoCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);

/* Creates video test with noise removal effect */
kzsError medianVideoCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);


#endif
