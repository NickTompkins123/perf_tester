/**
* \file
* Project loader for object source.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_OBJECT_SOURCE_H
#define KZU_PROJECT_LOADER_OBJECT_SOURCE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuObjectSource;


/** Loads object source from binary. */
kzsException kzuProjectLoaderLoadObjectSource(struct KzuProject* project, kzString path, struct KzuObjectSource** out_objectSource);

/** Unloads object source from project. */
kzsException kzuProjectLoaderUnloadObjectSource(struct KzuProject* project, const struct KzuObjectSource* objectSource);


#endif
