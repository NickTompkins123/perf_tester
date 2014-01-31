/**
* \file
* OpenCL FFT wave simulation.
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_WAVE_H
#define CL_WAVE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;


/** Create wave test. */
kzsError waveCreate(const struct BenchmarkFramework* framework, struct BfScene** out_scene);


#endif
