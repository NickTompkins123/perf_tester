/**
* \file
* Project patcher. This component provides mechanism for loading and patching a project object with .KZB binary data.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
/*
Checklist for adding new object types for realtime patching:
1) Separate container from the object type
2) Add the following three functions for the object type:
  a) void kzuObjectTakeOwnership(...);
  b) kzBool kzuObjectIsValid(...);
  c) void kzuObjectTransferData(...);
3) Update all asserts in the object file from isValidPointer to objectIsValid
4) Find all references for object != KZ_NULL or object == KZ_NULL, convert them to objectIsValid if needed
5) In object loader change if (object == KZ_NULL) to if (!objectIsValid(object))
6) In object loader change kzuProjectAddObject to kzuProjectObjectLoaded
7) Add support in kzu_project.c: kzuProjectTakeObjectOwnership_internal
8) Add support in kzu_project.c: kzuProjectDeleteObject
9) Add support in kzu_project_patcher.c: kzuProjectPatcherIsSupported_internal
*/

#include "kzu_project_patcher.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_object_private.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_light_property.h>
#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property_group.h>
#include <user/material/kzu_material.h>
#include <user/renderer/kzu_render_pass.h>
#include <user/effect_system/transition/kzu_transition.h>
#include <user/kzu_error_codes.h>
#include <user/animation/kzu_animation_player.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/debug/kzs_log.h>


/* Forward declarations */
struct KzuBinaryFolderInfo;


struct KzuProjectPatcher
{
    void* userData; /**< Arbitrary data passed as parameter for the callbacks. */
    KzuProjectPatchCallback patchCallback; /**< Callback function which is called for each patched object. */
};


/**
 * Defines ordering in which some project object types must be patched.
 * Array of pairs, where in each pair items of first type will be patched before items of second type.
 */
static const enum KzuProjectObjectType KZU_PROJECT_PATCHER_ORDERINGS[][2] =
{
    {KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE, KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE},
    {KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE, KZU_PROJECT_OBJECT_TYPE_MATERIAL},
    {KZU_PROJECT_OBJECT_TYPE_IMAGE, KZU_PROJECT_OBJECT_TYPE_TEXTURE},
    {KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE}    /* Needed for UI component nodes. */
};

/** Number of patch orderings. */
static const kzUint KZU_PROJECT_PATCHER_ORDERING_COUNT = sizeof(KZU_PROJECT_PATCHER_ORDERINGS) / sizeof(*KZU_PROJECT_PATCHER_ORDERINGS);


/** Helper function used from ...patchProject and ...patchProjectFromFileResource. */
static kzsError kzuProjectPatcherPatchFromDirectory_internal(const struct KzuProjectPatcher* patcher, struct KzuProject* project,
                                                             struct KzuBinaryDirectory* patchDirectory, kzBool* out_patchSuccess);


kzsError kzuProjectPatcherCreate(const struct KzcMemoryManager* memoryManager, struct KzuProjectPatcher** out_patcher)
{
    kzsError result;
    struct KzuProjectPatcher* patcher;

    result = kzcMemoryAllocVariable(memoryManager, patcher, "Project patcher");
    kzsErrorForward(result);

    patcher->userData = KZ_NULL;
    patcher->patchCallback = KZ_NULL;

    *out_patcher = patcher;
    kzsSuccess();
}

kzsError kzuProjectPatcherDelete(struct KzuProjectPatcher* patcher)
{
    kzsError result;

    result = kzcMemoryFreeVariable(patcher);
    kzsErrorForward(result);
    
    kzsSuccess();
}

void kzuProjectPatcherSetCallback(struct KzuProjectPatcher* patcher, void* userData, KzuProjectPatchCallback patchCallback)
{
    kzsAssert(kzcIsValidPointer(patcher));

    patcher->userData = userData;
    patcher->patchCallback = patchCallback;
}

kzsError kzuProjectPatcherPatchProject(const struct KzuProjectPatcher* patcher, struct KzuProject* project,
                                       kzUint byteCount, const kzByte* bytes, kzBool* out_patchSuccess)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuBinaryDirectory* temporaryDirectory;
    kzBool patchSuccess;

    result = kzuBinaryDirectoryCreateFromMemory(memoryManager, bytes, byteCount, &temporaryDirectory);
    kzsErrorForward(result);

    result = kzuProjectPatcherPatchFromDirectory_internal(patcher, project, temporaryDirectory, &patchSuccess);
    kzsErrorForward(result);

    *out_patchSuccess = patchSuccess;
    kzsSuccess();
}

kzsError kzuProjectPatcherPatchProjectFromFileResource(const struct KzuProjectPatcher* patcher, struct KzuProject* project, kzString resourcePath)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuBinaryDirectory* temporaryDirectory;
    kzBool patchSuccess;

    result = kzuBinaryDirectoryCreateFromFileResource(memoryManager, resourcePath, &temporaryDirectory);
    kzsErrorForward(result);

    result = kzuProjectPatcherPatchFromDirectory_internal(patcher, project, temporaryDirectory, &patchSuccess);
    kzsErrorForward(result);

    /** When patching from a file, it is not meaningful to signal an unsupported patch data. */
    kzsErrorTest(patchSuccess, KZU_ERROR_PROJECT_LOADING_FAILED, "Failed to apply binary patch from file resource: One ore more of the file types do not support realtime patching.");

    kzsSuccess();
}

/** Checks if the given object type supports on-the-fly patching. */
static kzBool kzuProjectPatcherIsSupported_internal(enum KzuProjectObjectType objectType)
{
    kzBool supported;

    switch (objectType)
    {
        case KZU_PROJECT_OBJECT_TYPE_MATERIAL:
        case KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE:
        case KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE:
        case KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE:
        case KZU_PROJECT_OBJECT_TYPE_SCENE:
        case KZU_PROJECT_OBJECT_TYPE_TEXTURE:
        case KZU_PROJECT_OBJECT_TYPE_IMAGE:
        case KZU_PROJECT_OBJECT_TYPE_COMPOSER:
        case KZU_PROJECT_OBJECT_TYPE_MESH:
        case KZU_PROJECT_OBJECT_TYPE_OBJECT_SOURCE:
        case KZU_PROJECT_OBJECT_TYPE_PROPERTY_TYPE:
        case KZU_PROJECT_OBJECT_TYPE_ANIMATION:
        case KZU_PROJECT_OBJECT_TYPE_ANIMATION_CLIP:
        case KZU_PROJECT_OBJECT_TYPE_TIMELINE_SEQUENCE:
        case KZU_PROJECT_OBJECT_TYPE_UI_COMPONENT_TYPE:
        case KZU_PROJECT_OBJECT_TYPE_FONT:
        case KZU_PROJECT_OBJECT_TYPE_TRANSITION:
        case KZU_PROJECT_OBJECT_TYPE_SCRIPT:
        case KZU_PROJECT_OBJECT_TYPE_TRAJECTORY:
        case KZU_PROJECT_OBJECT_TYPE_PROPERTY_GROUP:
        case KZU_PROJECT_OBJECT_TYPE_SPLINE:
        {
            supported = KZ_TRUE;
            break;
        }

        case KZU_PROJECT_OBJECT_TYPE_INVALID:
        default:
        {
            kzsAssert(KZ_FALSE);
            supported = KZ_FALSE;
        }
    }

    return supported;
}

static kzInt kzuProjectPatchOrdering_internal(const void* first, const void* second)
{
    kzInt result = 0;
    const struct KzuBinaryFileInfo* firstFileInfo = *(const struct KzuBinaryFileInfo**)first;
    const struct KzuBinaryFileInfo* secondFileInfo = *(const struct KzuBinaryFileInfo**)second;
    enum KzuBinaryFileType firstType = kzuBinaryFileInfoGetType(firstFileInfo);
    enum KzuBinaryFileType secondType = kzuBinaryFileInfoGetType(secondFileInfo);
    enum KzuProjectObjectType firstObjectType = kzuProjectLoaderGetObjectType(firstType);
    enum KzuProjectObjectType secondObjectType = kzuProjectLoaderGetObjectType(secondType);
    kzUint i;

    for (i = 0; i < KZU_PROJECT_PATCHER_ORDERING_COUNT; ++i)
    {
        const enum KzuProjectObjectType* ordering = KZU_PROJECT_PATCHER_ORDERINGS[i];
        enum KzuProjectObjectType enforceFirstObjectType = ordering[0];
        enum KzuProjectObjectType enforceSecondObjectType = ordering[1];
        if (firstObjectType == enforceFirstObjectType && secondObjectType == enforceSecondObjectType)
        {
            result = -1;
            break;
        }
        else if (secondObjectType == enforceFirstObjectType && firstObjectType == enforceSecondObjectType)
        {
            result = 1;
            break;
        }
    }

    return result;
}

/** Creates a copy of each child node reference which does not come from KZB. */
static kzsError kzuProjectPatcherGetCustomObjectData_internal(const struct KzcMemoryManager* memoryManager,
                                                              const struct KzuObjectNode* objectNode,
                                                              struct KzcDynamicArray** out_customChildren,
                                                              struct KzuObjectNode** out_parent)
{
    kzsError result;
    struct KzcDynamicArray* childReferences;
    struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(objectNode);

    result = kzcDynamicArrayCreate(memoryManager, &childReferences);
    kzsErrorForward(result);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuObjectNode* childNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        kzBool fromKzb = kzuObjectNodeIsChildFromKzb(objectNode, childNode);

        /* If child object was not loaded from kzb, copy it. */
        if (!fromKzb)
        {
            result = kzcDynamicArrayAdd(childReferences, childNode);
            kzsErrorForward(result);
        }
    }

    *out_customChildren = childReferences;
    *out_parent = kzuObjectNodeGetParent(objectNode);
    kzsSuccess();
}

/** Creates a copy of each child node reference which does not come from KZB. */
static kzsError kzuProjectPatcherRestoreCustomObjectData_internal(struct KzuObjectNode* objectNode,
                                                                  const struct KzcDynamicArray* customChildren,
                                                                  struct KzuObjectNode* parent)
{
    kzsError result;
    struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(customChildren);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzuObjectNode* child = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        result = kzuObjectNodeAddChild(objectNode, child);
        kzsErrorForward(result);
    }

    if (parent != KZ_NULL)
    {
        kzuObjectNodeSetParent_private(objectNode, parent);
    }

    kzsSuccess();
}

/**
 * Patches the content of the given patch directory to the project using given base directory.
 * Returns KZ_FALSE if patching is not supported for the given content.
 */
static kzsError kzuProjectPatcherPatchFromDirectory_internal(const struct KzuProjectPatcher* patcher, struct KzuProject* project,
                                                             struct KzuBinaryDirectory* patchDirectory, kzBool* out_patchSuccess)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    kzBool patchSupported = KZ_TRUE;
    struct KzcDynamicArray* files;

    KZ_UNUSED_PARAMETER(patcher); /* TODO: If the patcher is not needed in the future, remove it completely */

    result = kzcDynamicArrayCreate(memoryManager, &files);
    kzsErrorForward(result);

    result = kzuBinaryDirectoryMerge(kzuProjectGetBinaryDirectory(project), patchDirectory, files);
    kzsErrorForward(result);

    kzcDynamicArraySort(files, kzuProjectPatchOrdering_internal);

    /* First check if patching is supported for all file types in the patch. */
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(files);
        while (kzcDynamicArrayIterate(it))
        {
            const struct KzuBinaryFileInfo* fileInfo = (struct KzuBinaryFileInfo*)kzcDynamicArrayIteratorGetValue(it);
            enum KzuBinaryFileType type = kzuBinaryFileInfoGetType(fileInfo);
            enum KzuProjectObjectType objectType = kzuProjectLoaderGetObjectType(type);

            /* Check if the file type is stored in the project. */
            if (objectType != KZU_PROJECT_OBJECT_TYPE_INVALID)
            {
                kzBool filePatchSupported = kzuProjectPatcherIsSupported_internal(objectType);
                patchSupported &= filePatchSupported;
            }
            /* TODO: Otherwise let custom code to reload it. */
        }
    }

    /* If patching is possible, patch all supported object types. */
    if (patchSupported)
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(files);

        while (kzcDynamicArrayIterate(it))
        {
            struct KzuBinaryFileInfo* fileInfo = (struct KzuBinaryFileInfo*)kzcDynamicArrayIteratorGetValue(it);
            enum KzuBinaryFileType type = kzuBinaryFileInfoGetType(fileInfo);
            enum KzuProjectObjectType objectType = kzuProjectLoaderGetObjectType(type);
            kzString path = kzuBinaryFileInfoGetPath(fileInfo);
            void* object;
            void* newObject;
            kzString storedPath;

            /* Object node custom data */
            struct KzcDynamicArray* customObjectChildren = KZ_NULL;
            struct KzuObjectNode* customObjectParent = KZ_NULL;

            /* Special handling for property types, since the name is meaningful and not just the path */
            if (type == KZU_BINARY_FILE_TYPE_PROPERTY_TYPE)
            {
                storedPath = kzuBinaryFileInfoGetName(fileInfo);
            }
            else
            {
                storedPath = path;
            }

            /* Check if an object already exist in the project. (If not, it was added in this patch). */
            object = kzuProjectGetObject(project, objectType, storedPath);
            if (object != KZ_NULL)
            {
                if (objectType == KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE)
                {
                    /* Store custom data from object nodes. */
                    result = kzuProjectPatcherGetCustomObjectData_internal(memoryManager, (struct KzuObjectNode*)object, &customObjectChildren, &customObjectParent);
                    kzsErrorForward(result);
                }

                /* Delete the old object. */
                result = kzuProjectDeleteObject(project, objectType, storedPath);
                kzsErrorForward(result);
            }

            /* Load the file. */
            result = kzuProjectLoaderLoadObject(project, type, path, &newObject);
            kzsErrorForward(result);

            if (newObject != KZ_NULL)
            {
                /* If old children were found. */
                if (object != KZ_NULL && objectType == KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE)
                {
                    /* Store custom data from object nodes. */
                    result = kzuProjectPatcherRestoreCustomObjectData_internal((struct KzuObjectNode*)object, customObjectChildren, customObjectParent);
                    kzsErrorForward(result);

                    result = kzcDynamicArrayDelete(customObjectChildren);
                    kzsErrorForward(result);
                }            

                /* Call callback function. */
                if (patcher->patchCallback != KZ_NULL)
                {
                    result = patcher->patchCallback(patcher->userData, path, objectType, newObject);
                    kzsErrorForward(result);
                }
            }
        }
    }

    result = kzcDynamicArrayDelete(files);
    kzsErrorForward(result);

    *out_patchSuccess = patchSupported;
    kzsSuccess();
}
