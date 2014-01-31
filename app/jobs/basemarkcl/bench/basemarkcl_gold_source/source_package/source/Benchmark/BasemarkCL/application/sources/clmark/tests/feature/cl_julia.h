/**
* \file
* OpenCL julia fractal.
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_JULIA_H
#define CL_JULIA_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;


/** Create the Julia test. */
kzsError juliaCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);


#endif
