/**
* \file
* OpenCL soft body test.
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CL_SOFT_BODY_H
#define CL_SOFT_BODY_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;
    

/** Create soft body test. */
kzsError softBodyCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene);


#endif
