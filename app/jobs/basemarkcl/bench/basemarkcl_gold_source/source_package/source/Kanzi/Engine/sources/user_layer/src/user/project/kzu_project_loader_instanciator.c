/**
* \file
* Project loader for instanciator files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_instanciator.h"

#include "kzu_project_loader_general.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project_loader_object_node_private.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/scene_graph/kzu_instanciator.h>
#include <user/scene_graph/kzu_object.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>


kzsException kzuProjectLoaderLoadInstanciatorNode(struct KzuProject* project, kzString path, struct KzuInstanciatorNode** out_instanciatorNode)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuInstanciatorNode* instanciatorNode;
    struct KzuObjectNode* objectNode;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load instanciator node with null path");

    /* Check if the object node is already loaded. */
    objectNode = (struct KzuObjectNode*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, path);

    if (kzuObjectNodeIsValid(objectNode))
    {
        instanciatorNode = kzuInstanciatorNodeFromObjectNode(objectNode);
    }
    else
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_INSTANCIATOR_NODE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load instanciator node file.");

        /* Create instanciator node. */
        result = kzuInstanciatorNodeCreate(memoryManager, kzuProjectGetPropertyManager(project), kzuProjectGetMessageDispatcher(project), kzuBinaryFileInfoGetName(file), &instanciatorNode);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, kzuInstanciatorNodeToObjectNode(instanciatorNode),
                                        kzuBinaryFileInfoGetPath(file), (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&objectNode);
        kzsErrorForward(result);

        instanciatorNode = kzuInstanciatorNodeFromObjectNode(objectNode);

        {
            struct KzcInputStream* inputStream;

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            /* Load object node base. */
            result = kzuProjectLoaderLoadObjectNodeBase_private(project, inputStream, file, objectNode);
            kzsErrorForward(result);

            /* Load instanciator node specific data. */
            {
                struct KzuObjectNode* targetObjectNode;
                kzString targetPath;

                result = kzuBinaryDirectoryReadReference(inputStream, file, &targetPath);
                kzsErrorForward(result);

                if (targetPath != KZ_NULL)
                {
                    result = kzuProjectLoaderLoadObjectNode(project, targetPath, &targetObjectNode);
                    kzsErrorForward(result);
                }
                else
                {
                    targetObjectNode = KZ_NULL;
                }

                result = kzuInstanciatorNodeInitialize(instanciatorNode, targetObjectNode);
                kzsErrorForward(result);
            }

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);
        }
    }

    *out_instanciatorNode = instanciatorNode;
    kzsSuccess();
}
