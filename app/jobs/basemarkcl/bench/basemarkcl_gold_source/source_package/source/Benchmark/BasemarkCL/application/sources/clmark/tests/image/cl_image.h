/**
* \file
* OpenCL benchmark image tests.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_IMAGE_TESTS_H
#define CL_IMAGE_TESTS_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;

    
/* Create sharpening test */
kzsError sharpeningCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);

/* Create blur test */
kzsError blurCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);

/* Create histogram test */
kzsError histogramCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);

/* Create bilateral test */
kzsError bilateralCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);

/* Create median test */
kzsError medianCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);


#endif
