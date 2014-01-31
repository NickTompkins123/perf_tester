/**
* \file
* Project loader for material.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_MATERIAL_H
#define KZU_PROJECT_LOADER_MATERIAL_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuMaterial;
struct KzuMaterialType;


/** Loads material type from binary. */
kzsException kzuProjectLoaderLoadMaterialType(struct KzuProject* project, kzString path, struct KzuMaterialType** out_materialType);

/** Loads material from binary. */
kzsException kzuProjectLoaderLoadMaterial(struct KzuProject* project, kzString path, struct KzuMaterial** out_material);

/** Unloads material type from project. */
kzsException kzuProjectLoaderUnloadMaterialType(struct KzuProject* project, const struct KzuMaterialType* materialType);

/** Unloads material from project. */
kzsException kzuProjectLoaderUnloadMaterial(struct KzuProject* project, const struct KzuMaterial* material);


#endif
