/**
* \file
* Project loader private functions for object node files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_OBJECT_NODE_PRIVATE_H
#define KZU_PROJECT_LOADER_OBJECT_NODE_PRIVATE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuBinaryFileInfo;
struct KzuProject;
struct KzuObjectNode;
struct KzcInputStream;


kzsError kzuProjectLoaderLoadObjectNodeBase_private(struct KzuProject* project, struct KzcInputStream* inputStream,
                                                    const struct KzuBinaryFileInfo* file, struct KzuObjectNode* objectNode);


#endif
