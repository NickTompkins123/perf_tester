/**
 * \file
 * OpenCL Benchmark load screen scene
 * Copyright 2011 by Rightware. All rights reserved.
 */

#ifndef CL_LOAD_SCREEN
#define CL_LOAD_SCREEN


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;


kzsError clLoadScreenCreate(const struct BenchmarkFramework* framework, struct BfScene** out_scene);
kzsError clLoadScreenCreateScreenshot(const struct BenchmarkFramework* framework, struct BfScene** out_scene);
kzsError clLoadScreenCreateNoGl(const struct BenchmarkFramework* framework, struct BfScene** out_scene);


#endif
