/**
* \file
* Project loader for camera.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_CAMERA_H
#define KZU_PROJECT_LOADER_CAMERA_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuCameraNode;


/** Loads camera node from binary. */
kzsException kzuProjectLoaderLoadCameraNode(struct KzuProject* project, kzString path, struct KzuCameraNode** out_cameraNode);


#endif
