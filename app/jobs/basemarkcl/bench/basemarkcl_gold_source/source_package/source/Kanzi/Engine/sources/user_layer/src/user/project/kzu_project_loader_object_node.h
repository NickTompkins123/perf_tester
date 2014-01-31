/**
* \file
* Project loader for object node files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_OBJECT_NODE_H
#define KZU_PROJECT_LOADER_OBJECT_NODE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuObjectNode;


/** Loads any type of object node from binary. */
kzsException kzuProjectLoaderLoadObjectNode(struct KzuProject* project, kzString path, struct KzuObjectNode** out_objectNode);

/** Unloads object node from project. */
kzsException kzuProjectLoaderUnloadObjectNode(struct KzuProject* project, const struct KzuObjectNode* objectNode);


#endif
