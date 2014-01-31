/**
* \file
* Project loader for scene.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_SCENE_H
#define KZU_PROJECT_LOADER_SCENE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuScene;


/** Loads scene from binary. */
kzsException kzuProjectLoaderLoadScene(struct KzuProject* project, kzString path, struct KzuScene** out_scene);

/** Unloads scene from project. */
kzsException kzuProjectLoaderUnloadScene(struct KzuProject* project, const struct KzuScene* scene);


#endif
