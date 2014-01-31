/**
* \file
* Project loader for level of detail object.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_LOD_H
#define KZU_PROJECT_LOADER_LOD_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuLODSelectorNode;


/** Loads LOD selector node from binary. */
kzsException kzuProjectLoaderLoadLODSelectorNode(struct KzuProject* project, kzString path, struct KzuLODSelectorNode** out_lodSelectorNode);


#endif
