/**
* \file
* Project loader for object node files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_object_node.h"
#include "kzu_project_loader_object_node_private.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_mesh.h"
#include "kzu_project_loader_camera.h"
#include "kzu_project_loader_light.h"
#include "kzu_project_loader_lod.h"
#include "kzu_project_loader_user_interface.h"
#include "kzu_project_loader_instanciator.h"
#include "kzu_project_loader_property.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_light.h>
#include <user/scene_graph/kzu_lod_selector.h>
#include <user/scene_graph/kzu_instanciator.h>
#include <user/properties/kzu_property.h>
#include <user/ui/kzu_ui_component.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>

#include <system/debug/kzs_counter.h>


/** Loads an empty node. */
kzsException kzuProjectLoaderLoadEmptyNode_internal(struct KzuProject* project, kzString path, struct KzuObjectNode** out_objectNode);


kzsException kzuProjectLoaderLoadObjectNode(struct KzuProject* project, kzString path, struct KzuObjectNode** out_objectNode)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzuObjectNode* objectNode;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load object node with null path");

    /* Check if the object node is already loaded. */
    objectNode = (struct KzuObjectNode*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, path);

    if (!kzuObjectNodeIsValid(objectNode))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        /*lint -e{788} Suppress warning about not listing all possible enum values. */
        switch (kzuBinaryFileInfoGetType(file))
        {
            case KZU_BINARY_FILE_TYPE_EMPTY_NODE:
            {
                result = kzuProjectLoaderLoadEmptyNode_internal(project, kzuBinaryFileInfoGetPath(file), &objectNode);
                kzsErrorForward(result);

                break;
            }

            case KZU_BINARY_FILE_TYPE_MESH_NODE:
            {
                struct KzuMeshNode* meshNode;

                result = kzuProjectLoaderLoadMeshNode(project, kzuBinaryFileInfoGetPath(file), &meshNode);
                kzsErrorForward(result);

                objectNode = kzuMeshNodeToObjectNode(meshNode);

                break;
            }

            case KZU_BINARY_FILE_TYPE_CAMERA_NODE:
            {
                struct KzuCameraNode* cameraNode;

                result = kzuProjectLoaderLoadCameraNode(project, kzuBinaryFileInfoGetPath(file), &cameraNode);
                kzsErrorForward(result);

                objectNode = kzuCameraNodeToObjectNode(cameraNode);

                break;
            }

            case KZU_BINARY_FILE_TYPE_LIGHT_NODE:
            {
                struct KzuLightNode* lightNode;

                result = kzuProjectLoaderLoadLightNode(project, kzuBinaryFileInfoGetPath(file), &lightNode);
                kzsErrorForward(result);

                objectNode = kzuLightNodeToObjectNode(lightNode);

                break;
            }

            case KZU_BINARY_FILE_TYPE_LOD_SELECTOR_NODE:
            {
                struct KzuLODSelectorNode* lodSelectorNode;

                result = kzuProjectLoaderLoadLODSelectorNode(project, kzuBinaryFileInfoGetPath(file), &lodSelectorNode);
                kzsErrorForward(result);

                objectNode = kzuLODSelectorNodeToObjectNode(lodSelectorNode);

                break;
            }

            case KZU_BINARY_FILE_TYPE_UI_COMPONENT_NODE:
            {
                struct KzuUiComponentNode* uiComponentNode;

                result = kzuProjectLoaderLoadUiComponentNode(project, kzuBinaryFileInfoGetPath(file), &uiComponentNode);
                kzsErrorForward(result);

                objectNode = kzuUiComponentNodeToObjectNode(uiComponentNode);

                break;
            }

            case KZU_BINARY_FILE_TYPE_INSTANCIATOR_NODE:
            {
                struct KzuInstanciatorNode* instanciatorNode;

                result = kzuProjectLoaderLoadInstanciatorNode(project, kzuBinaryFileInfoGetPath(file), &instanciatorNode);
                kzsErrorForward(result);

                objectNode = kzuInstanciatorNodeToObjectNode(instanciatorNode);

                break;
            }

            default:
            {
                kzsErrorThrow(KZU_ERROR_WRONG_BINARY_FILE_TYPE, "Wrong file type encountered while trying to load object node file.");
            }
        }
    }

    kzsCounterIncrease("projectLoaderLoadObjectNode");

    *out_objectNode = objectNode;
    kzsSuccess();
}

kzsError kzuProjectLoaderLoadObjectNodeBase_private(struct KzuProject* project, struct KzcInputStream* inputStream,
                                                    const struct KzuBinaryFileInfo* file, struct KzuObjectNode* objectNode)
{
    kzsError result;

    /* Children */
    {
        kzUint childCount;
        kzUint i;

        result = kzcInputStreamReadU32Int(inputStream, &childCount);
        kzsErrorForward(result);

        for (i = 0; i < childCount; ++i)
        {
            kzString childPath;
            struct KzuObjectNode* childObjectNode;

            result = kzuBinaryDirectoryReadReference(inputStream, file, &childPath);
            kzsErrorForward(result);

            result = kzuProjectLoaderLoadObjectNode(project, childPath, &childObjectNode);
            kzsErrorForward(result);

            result = kzuObjectNodeAddChild(objectNode, childObjectNode);
            kzsErrorForward(result);

            result = kzuObjectNodeSetChildFromKzb(objectNode, childObjectNode);
            kzsErrorForward(result);
        }
    }

    /* Properties */
    result = kzuProjectLoaderReadProperties(project, inputStream, file, objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsException kzuProjectLoaderLoadEmptyNode_internal(struct KzuProject* project, kzString path, struct KzuObjectNode** out_objectNode)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuObjectNode* objectNode;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load mesh node with null path");

    /* Check if the object node is already loaded. */
    objectNode = (struct KzuObjectNode*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, path);

    if (!kzuObjectNodeIsValid(objectNode))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_EMPTY_NODE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load empty node file.");

        /* Create empty node. */
        result = kzuEmptyNodeCreate(memoryManager, kzuProjectGetPropertyManager(project), kzuProjectGetMessageDispatcher(project), kzuBinaryFileInfoGetName(file), &objectNode);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, objectNode, kzuBinaryFileInfoGetPath(file),
            (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&objectNode);
        kzsErrorForward(result);

        {
            struct KzcInputStream* inputStream;

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            /* Load object node base. */
            result = kzuProjectLoaderLoadObjectNodeBase_private(project, inputStream, file, objectNode);
            kzsErrorForward(result);

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);
        }
    }

    *out_objectNode = objectNode;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadObjectNode(struct KzuProject* project, const struct KzuObjectNode* objectNode)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, objectNode);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
