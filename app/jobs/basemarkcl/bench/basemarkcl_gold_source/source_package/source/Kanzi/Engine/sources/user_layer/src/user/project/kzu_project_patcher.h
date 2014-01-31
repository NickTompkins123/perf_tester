/**
* \file
* Project patcher. 
* This component provides mechanism for loading and patching a project with .KZB binary data.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_PATCHER_H
#define KZU_PROJECT_PATCHER_H


#include <user/project/kzu_project.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuMaterial;
struct KzuObjectNode;
struct KzuScene;


/**
 * \struct KzuProjectPatcher
 * Project patcher structure.
 */
struct KzuProjectPatcher;


/**
 * Callback function type for project binary patching. If object is KZ_NULL, the object was deleted.
 * path is the object path inside the binary. object is the patched object. userData is the pointer passed as parameter
 * to kzuProjectPatcherSetCallbacks.
 */
typedef kzsError (*KzuProjectPatchCallback)(void* userData, kzString path, enum KzuProjectObjectType objectType, void* object);


/** Creates a new project patcher object. The given arbitrary user data is passed as a parameter in the callbacks. */
kzsError kzuProjectPatcherCreate(const struct KzcMemoryManager* memoryManager, struct KzuProjectPatcher** out_patcher);

/** Deletes a project patcher object. */
kzsError kzuProjectPatcherDelete(struct KzuProjectPatcher* patcher);

/** Sets the callback function for project patching. */
void kzuProjectPatcherSetCallback(struct KzuProjectPatcher* patcher, void* userData, KzuProjectPatchCallback patchCallback);


/**
 * Patches the project with the given .KZB data.
 * out_patchSuccess is set to false if patching is not supported with the given .KZB data.
 * In this case the whole project should be loaded again.
 */
kzsError kzuProjectPatcherPatchProject(const struct KzuProjectPatcher* patcher, struct KzuProject* project,
                                       kzUint byteCount, const kzByte* bytes, kzBool* out_patchSuccess);

/** Patches the project with the .KZB data of given resource. */
kzsError kzuProjectPatcherPatchProjectFromFileResource(const struct KzuProjectPatcher* patcher, struct KzuProject* project,
                                                       kzString resourcePath);


#endif
