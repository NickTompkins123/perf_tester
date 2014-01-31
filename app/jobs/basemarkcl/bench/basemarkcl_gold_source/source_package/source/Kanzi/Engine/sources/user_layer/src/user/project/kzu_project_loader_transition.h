/**
* \file
* Project loader for scene.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_TRANSITION_H
#define KZU_PROJECT_LOADER_TRANSITION_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuTransition;


/** Loads transition from binary. */
kzsException kzuProjectLoaderLoadTransition(struct KzuProject* project, kzString path, struct KzuTransition** out_transition);

/** Unloads transition from project. */
kzsException kzuProjectLoaderUnloadTransition(struct KzuProject* project, const struct KzuTransition* transition);

#endif
