/**
* \file
* Project loader for light.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_LIGHT_H
#define KZU_PROJECT_LOADER_LIGHT_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuLightNode;


/** Loads light node from binary. */
kzsException kzuProjectLoaderLoadLightNode(struct KzuProject* project, kzString path, struct KzuLightNode** out_lightNode);


#endif
