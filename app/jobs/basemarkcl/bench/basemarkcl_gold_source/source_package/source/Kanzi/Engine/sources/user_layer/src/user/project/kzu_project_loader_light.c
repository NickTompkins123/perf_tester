/**
* \file
* Project loader for light files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_light.h"

#include "kzu_project_loader_general.h"
#include "kzu_project_loader_object_node_private.h"
#include "kzu_project_loader_property.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/scene_graph/kzu_light.h>
#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_light_property.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>


kzsException kzuProjectLoaderLoadLightNode(struct KzuProject* project, kzString path, struct KzuLightNode** out_lightNode)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuLightNode* lightNode;
    struct KzuObjectNode* objectNode;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load light node with null path");

    /* Check if the object node is already loaded. */
    objectNode = (struct KzuObjectNode*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, path);

    if (kzuObjectNodeIsValid(objectNode))
    {
        lightNode = kzuLightNodeFromObjectNode(objectNode);
    }
    else
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_LIGHT_NODE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load light node file.");

        /* Create light node. */
        result = kzuLightNodeCreate(memoryManager, kzuProjectGetPropertyManager(project), kzuProjectGetMessageDispatcher(project), kzuBinaryFileInfoGetName(file), &lightNode);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, kzuLightNodeToObjectNode(lightNode),
                                        kzuBinaryFileInfoGetPath(file), (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&objectNode);
        kzsErrorForward(result);

        lightNode = kzuLightNodeFromObjectNode(objectNode);

        {
            struct KzcInputStream* inputStream;

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            /* Load object node base. */
            result = kzuProjectLoaderLoadObjectNodeBase_private(project, inputStream, file, objectNode);
            kzsErrorForward(result);

            /* Load light node specific data. */
            {
                struct KzuPropertyType* lightPropertyType;

                result = kzuProjectLoaderReadProperty(project, inputStream, file, lightNode, &lightPropertyType);
                kzsErrorForward(result);

                kzsErrorTest(kzuPropertyTypeGetDataType(lightPropertyType) == KZU_PROPERTY_DATA_TYPE_LIGHT,
                             KZU_ERROR_INVALID_BINARY_DATA, "Light property must be of type PROPERTY_TYPE_LIGHT");

                kzuLightNodeInitialize(lightNode, kzuLightPropertyTypeFromPropertyType(lightPropertyType));
            }

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);
        }
    }

    *out_lightNode = lightNode;
    kzsSuccess();
}
