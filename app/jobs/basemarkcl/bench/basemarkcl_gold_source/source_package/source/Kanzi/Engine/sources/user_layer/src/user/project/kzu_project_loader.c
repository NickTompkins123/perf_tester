/**
* \file
* Project loader. TODO:
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader.h"

#include "kzu_project_loader_general.h"
#include "kzu_project_loader_image.h"
#include "kzu_project_loader_scene.h"
#include "kzu_project_loader_texture.h"
#include "kzu_project_loader_property.h"
#include "kzu_project_loader_shader.h"
#include "kzu_project_loader_material.h"
#include "kzu_project_loader_mesh.h"
#include "kzu_project_loader_camera.h"
#include "kzu_project_loader_light.h"
#include "kzu_project_loader_lod.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project_loader_composer.h"
#include "kzu_project_loader_object_source.h"
#include "kzu_project_loader_property_group.h"
#include "kzu_project_loader_spline.h"
#include "kzu_project_loader_animation.h"
#include "kzu_project_loader_user_interface.h"
#include "kzu_project_loader_font.h"
#include "kzu_project_loader_transition.h"
#include "kzu_project_loader_project.h"
#include "kzu_project_loader_script.h"
#include "kzu_project_loader_trajectory.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>
#include <system/debug/kzs_counter.h>

/* TODO: Add definitions for "Texture Library" etc. */

/* TODO: Assert loadAnything() { path != KZ_NULL } */


/** Loads the content of the given directory to the project using given base directory. */
static kzsError kzuProjectLoaderLoadFromDirectory_internal(struct KzuProject* project, struct KzuBinaryDirectory* loadDirectory);


/** Loads a file of given type and path to the project. */
kzsError kzuProjectLoaderLoadObject(struct KzuProject* project, enum KzuBinaryFileType type, kzString path, void** out_object)
{
    kzsError result;
    void* object;
    
    switch (type)
    {
        case KZU_BINARY_FILE_TYPE_INVALID:
        {
            kzsErrorThrow(KZU_ERROR_WRONG_BINARY_FILE_TYPE, "Invalid binary file type");
        }

        case KZU_BINARY_FILE_TYPE_SCENE:
        {
            result = kzuProjectLoaderLoadScene(project, path, (struct KzuScene**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_EMPTY_NODE:
        case KZU_BINARY_FILE_TYPE_MESH_NODE:
        case KZU_BINARY_FILE_TYPE_CAMERA_NODE:
        case KZU_BINARY_FILE_TYPE_LIGHT_NODE:
        case KZU_BINARY_FILE_TYPE_LOD_SELECTOR_NODE:
        case KZU_BINARY_FILE_TYPE_UI_COMPONENT_NODE:
        case KZU_BINARY_FILE_TYPE_INSTANCIATOR_NODE:
        {
            result = kzuProjectLoaderLoadObjectNode(project, path, (struct KzuObjectNode**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_PROPERTY_GROUP:
        {
            result = kzuProjectLoaderLoadPropertyGroup(project, path, (struct KzuPropertyGroup**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_SPLINE:
        {
            result = kzuProjectLoaderLoadSpline(project, path, (struct KzcSpline**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_TEXTURE:
        {
            result = kzuProjectLoaderLoadTexture(project, path, (struct KzcTexture**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_MATERIAL_TYPE:
        {
            result = kzuProjectLoaderLoadMaterialType(project, path, (struct KzuMaterialType**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_MATERIAL:
        {
            result = kzuProjectLoaderLoadMaterial(project, path, (struct KzuMaterial**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_COMPOSER:
        {
            result = kzuProjectLoaderLoadComposer(project, path, (struct KzuComposer**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_PROPERTY_TYPE:
        {
            result = kzuProjectLoaderLoadPropertyType(project, path, (struct KzuPropertyType**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_MESH:
        {
            result = kzuProjectLoaderLoadMesh(project, path, (struct KzuMesh**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_MESH_BOX:
        {
            result = kzuProjectLoaderLoadBox(project, path, (struct KzuMesh**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_MESH_PLANE:
        {
            result = kzuProjectLoaderLoadPlane(project, path, (struct KzuMesh**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_MESH_SPHERE:
        {
            result = kzuProjectLoaderLoadSphere(project, path, (struct KzuMesh**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_IMAGE_PNG:
        {
            result = kzuProjectLoaderLoadImagePng(project, path, (struct KzcImage**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_IMAGE_JPEG:
        {
            result = kzuProjectLoaderLoadImageJpeg(project, path, (struct KzcImage**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_IMAGE_ETC:
        {
            result = kzuProjectLoaderLoadImageETC(project, path, (struct KzcImage**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_IMAGE_DXT:
        {
            result = kzuProjectLoaderLoadImageDXT(project, path, (struct KzcImage**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_IMAGE_MIPMAP:
        {
            result = kzuProjectLoaderLoadImageMipmap(project, path, (struct KzcImage**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_SHADER_SOURCE:
        {
            result = kzuProjectLoaderLoadShaderSource(project, path, (kzMutableString*)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_SHADER_BINARY:
        {
            KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Binary shaders not supported yet.");
        }

        case KZU_BINARY_FILE_TYPE_OBJECT_SOURCE:
        {
            result = kzuProjectLoaderLoadObjectSource(project, path, (struct KzuObjectSource**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_IMAGE_RAW_1D:
        {
            KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("1D Image data not supported yet.");
        }

        case KZU_BINARY_FILE_TYPE_IMAGE_RAW_2D:
        {
            KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("2D Image data not supported yet.");
        }

        case KZU_BINARY_FILE_TYPE_IMAGE_RAW_3D:
        {
            KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("3D Image data not supported yet.");
        }

        case KZU_BINARY_FILE_TYPE_ANIMATION_DATA:
        {
            result = kzuProjectLoaderLoadAnimation(project, path, (struct KzuAnimation**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_ANIMATION_CLIP:
        {
            result = kzuProjectLoaderLoadAnimationClip(project, path, (struct KzuAnimationClip**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_TIMELINE_SEQUENCE:
        {
            result = kzuProjectLoaderLoadTimeLineSequence(project, path, (struct KzuTimeLineSequence**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_UI_COMPONENT_TYPE:
        {
            result = kzuProjectLoaderLoadUiComponentType(project, path, (struct KzuUiComponentType**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_SCRIPT:
        {
            result = kzuProjectLoaderLoadScript(project, path, (struct KzuScript**)&object);
            kzsErrorForward(result);
            break;
        }
        
        case KZU_BINARY_FILE_TYPE_BITMAP_FONT:
        {
            result = kzuProjectLoaderLoadBitmapFont(project, path, (struct KzcBitmapFont**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_TRUETYPE_FONT:
        {
            result = kzuProjectLoaderLoadTruetypeFont(project, path, (struct KzcFont**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_PROJECT:
        {
            result = kzuProjectLoaderLoadProjectFileFromPath(project, path);
            kzsErrorForward(result);
            object = KZ_NULL;
            break;
        }
        
        case KZU_BINARY_FILE_TYPE_TRANSITION:
        {
            result = kzuProjectLoaderLoadTransition(project, path, (struct KzuTransition**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_TRAJECTORY:
        {
            result = kzuProjectLoaderLoadTrajectory(project, path, (struct KzcTrajectory**)&object);
            kzsErrorForward(result);
            break;
        }

        case KZU_BINARY_FILE_TYPE_BINARY:
        case KZU_BINARY_FILE_TYPE_UTF8_TEXT:
        case KZU_BINARY_FILE_TYPE_ASCII_TEXT:
        case KZU_BINARY_FILE_TYPE_SHORTCUT:
        {
            /* Do nothing with these file types. */
            object = KZ_NULL;
            break;
        }

        default:
        {
            /* This is not an error, unknown file types are just ignored. */
            kzsLog(KZS_LOG_LEVEL_WARNING, "Unknown binary file type");
            object = KZ_NULL;
        }
    }

    *out_object = object;
    kzsSuccess();
}

kzsError kzuProjectLoaderUnloadObject(struct KzuProject* project, kzString path)
{
    kzsError result;
    struct KzuBinaryFileInfo* file;
    kzString* fileReferences;
    kzUint fileReferenceCount;
    kzUint i;
    enum KzuProjectObjectType objectType;

    result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
    kzsErrorForward(result);

    result = kzuBinaryDirectoryGetFileReferences(kzcMemoryGetManager(project), file, &fileReferences);
    kzsErrorForward(result);
    fileReferenceCount = kzcArrayLength(fileReferences);

    objectType = kzuProjectLoaderGetObjectType(kzuBinaryFileInfoGetType(file));

    for(i = 0; i < fileReferenceCount; ++i)
    {
        if(fileReferences[i] != KZ_NULL && kzuProjectGetObject(project, objectType, fileReferences[i]) != KZ_NULL)
        {
            result = kzuProjectLoaderUnloadObject(project, fileReferences[i]);
            kzsErrorForward(result);
        }
    }

    result = kzcMemoryFreeArray((kzMutableString*)fileReferences);
    kzsErrorForward(result);

    if(kzuProjectGetObject(project, objectType, path) != KZ_NULL)
    {
        result = kzuProjectDeleteObject(project, objectType, path);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

/* TODO: rename function */
static kzsError kzuProjectLoaderLoadFromDirectory_internal(struct KzuProject* project, struct KzuBinaryDirectory* loadDirectory)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzcDynamicArray* files;

    result = kzcDynamicArrayCreate(memoryManager, &files);
    kzsErrorForward(result);

    result = kzuBinaryDirectoryMerge(kzuProjectGetBinaryDirectory(project), loadDirectory, files);
    kzsErrorForward(result);

    /* Check that the files are not already loaded. */
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(files);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuBinaryFileInfo* fileInfo = (struct KzuBinaryFileInfo*)kzcDynamicArrayIteratorGetValue(it);
            enum KzuBinaryFileType type = kzuBinaryFileInfoGetType(fileInfo);
            enum KzuProjectObjectType objectType = kzuProjectLoaderGetObjectType(type);
            kzString path = kzuBinaryFileInfoGetPath(fileInfo);
            void* objectData;

            if (objectType != KZU_PROJECT_OBJECT_TYPE_INVALID)
            {
                objectData = kzuProjectGetObject(project, objectType, path);
                kzsErrorTest(objectData == KZ_NULL, KZU_ERROR_PROJECT_LOADING_FAILED, "Object of given binary file already exists in the project");
            }
        }
    }

    /* Load project items. */
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(files);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuBinaryFileInfo* fileInfo = (struct KzuBinaryFileInfo*)kzcDynamicArrayIteratorGetValue(it);
            enum KzuBinaryFileType type = kzuBinaryFileInfoGetType(fileInfo);
            kzString path = kzuBinaryFileInfoGetPath(fileInfo);
            void* ignoredObject;

            result = kzuProjectLoaderLoadObject(project, type, path, &ignoredObject);
            kzsErrorForward(result);
        }
    }

    result = kzcDynamicArrayDelete(files);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuProjectLoaderLoadProject(struct KzuProject* project, kzUint byteCount, const kzByte* bytes)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuBinaryDirectory* temporaryDirectory;

    result = kzuBinaryDirectoryCreateFromMemory(memoryManager, bytes, byteCount, &temporaryDirectory);
    kzsErrorForward(result);

    result = kzuProjectLoaderLoadFromDirectory_internal(project, temporaryDirectory);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuProjectLoaderGetProjectFile_internal(const struct KzuProject* project, struct KzuBinaryFileInfo** out_projectFile)
{
    kzsError result;
    struct KzcDynamicArray* files;
    struct KzcDynamicArrayIterator it;
    struct KzuBinaryFileInfo* projectFile = KZ_NULL;

    result = kzcDynamicArrayCreate(kzcMemoryGetManager(project), &files);
    kzsErrorForward(result);

    result = kzuBinaryFolderInfoGetAllFiles(kzuBinaryDirectoryGetRootFolder(kzuProjectGetBinaryDirectory(project)), files);
    kzsErrorForward(result);

    it = kzcDynamicArrayGetIterator(files);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuBinaryFileInfo* fileInfo = (struct KzuBinaryFileInfo*)kzcDynamicArrayIteratorGetValue(it);
        /* Project file was found. */
        if (kzuBinaryFileInfoGetType(fileInfo) == KZU_BINARY_FILE_TYPE_PROJECT)
        {
            if (!kzcStringIsEqual(kzuBinaryFileInfoGetName(fileInfo), "CoreAssets"))
            {
                projectFile = fileInfo;
                break;
            }
        }
    }

    result = kzcDynamicArrayDelete(files);
    kzsErrorForward(result);

    *out_projectFile = projectFile;
    kzsSuccess();
}

kzsError kzuProjectLoaderLoadProjectSettings(struct KzuProject* project)
{
    kzsError result;
    struct KzuBinaryFileInfo* projectFile;

    result = kzuProjectLoaderGetProjectFile_internal(project, &projectFile);
    kzsErrorForward(result);

    if (projectFile != KZ_NULL)
    {
        result = kzuProjectLoaderLoadProjectFile(project, projectFile);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuProjectLoaderLoadPropertyGroups(struct KzuProject* project)
{
    kzsError result;
    struct KzcDynamicArray* files;
    struct KzcDynamicArrayIterator it;

    result = kzcDynamicArrayCreate(kzcMemoryGetManager(project), &files);
    kzsErrorForward(result);

    result = kzuBinaryFolderInfoGetAllFiles(kzuBinaryDirectoryGetRootFolder(kzuProjectGetBinaryDirectory(project)), files);
    kzsErrorForward(result);

    it = kzcDynamicArrayGetIterator(files);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzuBinaryFileInfo* fileInfo = (struct KzuBinaryFileInfo*)kzcDynamicArrayIteratorGetValue(it);
        if(kzuBinaryFileInfoGetType(fileInfo) == KZU_BINARY_FILE_TYPE_PROPERTY_GROUP)
        {
            struct KzuPropertyGroup* group;

            result = kzuProjectLoaderLoadPropertyGroupFromFileInfo(project, fileInfo, &group);
            kzsErrorForward(result);
        }
    }

    result = kzcDynamicArrayDelete(files);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuProjectLoaderLoadProjectFromFileResource(struct KzuProject* project, kzString resourcePath)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuBinaryDirectory* temporaryDirectory;

    result = kzuBinaryDirectoryCreateFromFileResource(memoryManager, resourcePath, &temporaryDirectory);
    kzsErrorForward(result);

    result = kzuProjectLoaderLoadFromDirectory_internal(project, temporaryDirectory);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuProjectLoaderSetFilePermanent(struct KzuProject* project, kzString resourcePath)
{
    kzsException result;
    struct KzuBinaryFileInfo* fileInfo;

    kzsAssert(resourcePath != KZ_NULL);

    if (!kzuProjectHasPermanentFilePath(project, resourcePath))
    {
        if (kzcStringIsEqual(resourcePath, "PropertyTypeLibrary/Transformation"))
        {
            if (!kzuProjectHasPermanentFilePath(project, "Transformation"))
            {
                result = kzuProjectAddPermanentFilePath(project, "Transformation");
                kzsErrorForward(result);
            }
        }
        else
        {
            kzString* fileReferences;
            kzUint fileReferenceCount;
            kzUint i;

            result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), resourcePath, &fileInfo);
            kzsExceptionForward(result);

            result = kzuBinaryDirectoryGetFileReferences(kzcMemoryGetManager(project), fileInfo, &fileReferences);
            kzsErrorForward(result);

            if (kzuBinaryFileInfoGetType(fileInfo) == KZU_BINARY_FILE_TYPE_PROPERTY_TYPE)
            {
                result = kzuProjectAddPermanentFilePath(project, kzuBinaryFileInfoGetName(fileInfo));
                kzsErrorForward(result);
            }
            else
            {
                result = kzuProjectAddPermanentFilePath(project, resourcePath);
                kzsErrorForward(result);
            }

            fileReferenceCount = kzcArrayLength(fileReferences);

            for (i = 0; i < fileReferenceCount; ++i)
            {
                kzString fileReference = fileReferences[i];
                if (fileReference != KZ_NULL)
                {
                    result = kzuProjectLoaderSetFilePermanent(project, fileReference);
                    kzsErrorForward(result);
                }
            }

            kzsCounterIncrease("setFilePermanent");

            result = kzcMemoryFreeArray((kzMutableString*)fileReferences);
            kzsErrorForward(result);
        }
        
    }

    kzsSuccess();
}

kzsError kzuProjectLoaderLoadDirectory(struct KzuProject* project, const struct KzuBinaryDirectory* directory)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzcDynamicArray* files;

    result = kzcDynamicArrayCreate(memoryManager, &files);
    kzsErrorForward(result);

    result = kzuBinaryFolderInfoGetAllFiles(kzuBinaryDirectoryGetRootFolder(directory), files);
    kzsErrorForward(result);

    /* Load project items. */
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(files);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuBinaryFileInfo* fileInfo = (struct KzuBinaryFileInfo*)kzcDynamicArrayIteratorGetValue(it);
            enum KzuBinaryFileType type = kzuBinaryFileInfoGetType(fileInfo);
            kzString path = kzuBinaryFileInfoGetPath(fileInfo);
            void* ignoredObject;

            result = kzuProjectLoaderLoadObject(project, type, path, &ignoredObject);
            kzsErrorForward(result);
        }
    }

    result = kzcDynamicArrayDelete(files);
    kzsErrorForward(result);

    kzsSuccess();
}
