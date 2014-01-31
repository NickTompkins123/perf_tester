/**
 * \file
 * OpenCL Compiler speed tests
 * Copyright 2011 by Rightware. All rights reserved.
 */

#ifndef CL_COMPILER_TESTS_H
#define CL_COMPILER_TESTS_H

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;


kzsError compilerTestCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);


#endif
