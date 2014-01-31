/*
* \file
* Project loader for mesh files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_MESH_H
#define KZU_PROJECT_LOADER_MESH_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuMesh;
struct KzuMeshNode;


/** Loads mesh from binary. */
kzsException kzuProjectLoaderLoadMesh(struct KzuProject* project, kzString path, struct KzuMesh** out_mesh);

/** Loads box from binary. */
kzsException kzuProjectLoaderLoadBox(struct KzuProject* project, kzString path, struct KzuMesh** out_mesh);
/** Loads sphere from binary. */
kzsException kzuProjectLoaderLoadSphere(struct KzuProject* project, kzString path, struct KzuMesh** out_mesh);
/** Loads plane from binary. */
kzsException kzuProjectLoaderLoadPlane(struct KzuProject* project, kzString path, struct KzuMesh** out_mesh);

/** Loads mesh node from binary. */
kzsException kzuProjectLoaderLoadMeshNode(struct KzuProject* project, kzString path, struct KzuMeshNode** out_meshNode);


#endif
