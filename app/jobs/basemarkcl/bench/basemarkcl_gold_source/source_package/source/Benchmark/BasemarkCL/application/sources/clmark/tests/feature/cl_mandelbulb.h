/**
 * \file
 * OpenCL Mandelbulb raytracer.
 * Copyright 2011 by Rightware. All rights reserved.
 */
#ifndef CL_MANDELBULB_H
#define CL_MANDELBULB_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;


/** Create the mandelbulb test. */
kzsError mandelbulbCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);


#endif
