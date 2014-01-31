/**
* \file
* Project loader for trajectory files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_TRAJECTORY_H
#define KZU_PROJECT_LOADER_TRAJECTORY_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzcTrajectory;


/** Loads script data structure from binary. */
kzsException kzuProjectLoaderLoadTrajectory(const struct KzuProject* project, kzString path, struct KzcTrajectory** out_trajectory);

/** Unloads trajectory from project. */
kzsException kzuProjectLoaderUnloadTrajectory(struct KzuProject* project, const struct KzcTrajectory* trajectory);

#endif
