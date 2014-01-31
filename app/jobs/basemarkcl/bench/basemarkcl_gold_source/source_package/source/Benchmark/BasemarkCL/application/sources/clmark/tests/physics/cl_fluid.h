/**
* \file
* OpenCL benchmark fluid test.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_FLUID_H
#define CL_FLUID_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;
    

/** Create fluid test. */
kzsError fluidCreate(const struct BenchmarkFramework* framework, struct BfScene** out_scene);


#endif
