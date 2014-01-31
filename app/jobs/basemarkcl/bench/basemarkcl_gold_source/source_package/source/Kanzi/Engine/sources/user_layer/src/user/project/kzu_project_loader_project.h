/**
* \file
* Project loader for project file.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_PROJECT_H
#define KZU_PROJECT_LOADER_PROJECT_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryFileInfo;
struct KzuBinaryDirectory;
struct KzuProject;


/** Loads project file from binary and applies it to project. */
kzsException kzuProjectLoaderLoadProjectFile(struct KzuProject* project, const struct KzuBinaryFileInfo* file);
/** Loads project file from binary path and applies it to project. */
kzsException kzuProjectLoaderLoadProjectFileFromPath(struct KzuProject* project, kzString filePath);


#endif
