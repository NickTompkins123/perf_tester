/**
* \file
* Project loader for spline.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_SPLINE_H
#define KZU_PROJECT_LOADER_SPLINE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzcSpline;


/** Loads spline from binary. */
kzsException kzuProjectLoaderLoadSpline(const struct KzuProject* project, kzString path, struct KzcSpline** out_spline);

/** Unloads spline from project. */
kzsException kzuProjectLoaderUnloadSpline(struct KzuProject* project, const struct KzcSpline* spline);


#endif
