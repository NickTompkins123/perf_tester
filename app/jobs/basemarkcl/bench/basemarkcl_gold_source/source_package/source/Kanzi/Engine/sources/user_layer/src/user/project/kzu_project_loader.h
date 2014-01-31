/**
* \file
* Project loader.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_H
#define KZU_PROJECT_LOADER_H


#include "kzu_project.h"
#include "kzu_project_loader_image.h"
#include "kzu_project_loader_scene.h"
#include "kzu_project_loader_texture.h"
#include "kzu_project_loader_property.h"
#include "kzu_project_loader_shader.h"
#include "kzu_project_loader_material.h"
#include "kzu_project_loader_mesh.h"
#include "kzu_project_loader_camera.h"
#include "kzu_project_loader_light.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project_loader_composer.h"
#include "kzu_project_loader_object_source.h"
#include "kzu_project_loader_script.h"

#include <user/binary/kzu_binary_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuProject;


/** Loads a project object of any specified file type with given path from binary. */
kzsError kzuProjectLoaderLoadObject(struct KzuProject* project, enum KzuBinaryFileType type, kzString path, void** out_object);
/** Unloads object with given path. */
kzsError kzuProjectLoaderUnloadObject(struct KzuProject* project, kzString path);

/**
 * Loads the given .KZB data to the project. This function doesn't touch the existing content of the project.
 * The project must not contain any of the files that exist in the binary.
 */
kzsError kzuProjectLoaderLoadProject(struct KzuProject* project, kzUint byteCount, const kzByte* bytes);

/** Loads project settings. */
kzsError kzuProjectLoaderLoadProjectSettings(struct KzuProject* project);
/** Loads project groups. */
kzsError kzuProjectLoaderLoadPropertyGroups(struct KzuProject* project);

/**
 * Loads the .KZB data of given resource to the project. This function doesn't touch the existing content of the project.
 * The project must not contain any of the files that exist in the binary.
 */
kzsError kzuProjectLoaderLoadProjectFromFileResource(struct KzuProject* project, kzString resourcePath);

/** Keeps specific file undeleted when clearing the project. */
kzsError kzuProjectLoaderSetFilePermanent(struct KzuProject* project, kzString resourcePath);

/**
 * Loads all items from the given directory to project. This function doesn't touch the existing content of the project.
 */
kzsError kzuProjectLoaderLoadDirectory(struct KzuProject* project, const struct KzuBinaryDirectory* directory);



#endif
