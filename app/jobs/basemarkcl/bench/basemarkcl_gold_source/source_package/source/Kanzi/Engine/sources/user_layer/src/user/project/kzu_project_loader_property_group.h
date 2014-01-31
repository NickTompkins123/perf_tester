/**
* \file
* Project loader for property group.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_PROPERTY_GROUP_H
#define KZU_PROJECT_LOADER_PROPERTY_GROUP_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryFileInfo;
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuPropertyGroup;


/** Loads property group from binary diectory specified path. */
kzsException kzuProjectLoaderLoadPropertyGroup(struct KzuProject* project, kzString path, struct KzuPropertyGroup** out_propertyGroup);

/** Loads property group from binary using file info. */
kzsException kzuProjectLoaderLoadPropertyGroupFromFileInfo(struct KzuProject* project, const struct KzuBinaryFileInfo* file, struct KzuPropertyGroup** out_propertyGroup);

/** Unloads property group from project. */
kzsException kzuProjectLoaderUnloadPropertyGroup(struct KzuProject* project, const struct KzuPropertyGroup* propertyGroup);


#endif
