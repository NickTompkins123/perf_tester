/**
* \file
* OpenCL smoothed particle hydrodynamics test.
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_SPH_H
#define CL_SPH_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;


/** Create fluid test. */
kzsError sphCreate(const struct BenchmarkFramework* framework, struct BfScene** out_scene);


#endif
