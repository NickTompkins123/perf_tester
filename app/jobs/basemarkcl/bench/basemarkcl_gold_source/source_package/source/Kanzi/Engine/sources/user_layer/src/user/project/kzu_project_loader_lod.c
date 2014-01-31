/**
* \file
* Project loader for LOD selector files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_lod.h"

#include "kzu_project_loader_general.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project_loader_object_node_private.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/scene_graph/kzu_lod_selector.h>
#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_property.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>


/* LOD types in binary. */
enum KzuBinaryLODType
{
    KZU_BINARY_LOD_TYPE_DISTANCE = 0,
    KZU_BINARY_LOD_TYPE_PERSPECTIVE = 1
};


kzsException kzuProjectLoaderLoadLODSelectorNode(struct KzuProject* project, kzString path, struct KzuLODSelectorNode** out_lodSelectorNode)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuLODSelectorNode* lodSelectorNode;
    struct KzuObjectNode* objectNode;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load LOD selector node with null path");

    /* Check if the object node is already loaded. */
    objectNode = (struct KzuObjectNode*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, path);

    if (kzuObjectNodeIsValid(objectNode))
    {
        lodSelectorNode = kzuLODSelectorNodeFromObjectNode(objectNode);
    }
    else
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_LOD_SELECTOR_NODE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load LOD selector node file.");

        /* Create lod node. */
        result = kzuLODSelectorNodeCreate(memoryManager, kzuProjectGetPropertyManager(project), kzuProjectGetMessageDispatcher(project), kzuBinaryFileInfoGetName(file), &lodSelectorNode);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, kzuLODSelectorNodeToObjectNode(lodSelectorNode),
                                        kzuBinaryFileInfoGetPath(file), 
                                        (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&objectNode);
        kzsErrorForward(result);

        lodSelectorNode = kzuLODSelectorNodeFromObjectNode(objectNode);

        {
            struct KzcInputStream* inputStream;

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            /* Load object node base. */
            result = kzuProjectLoaderLoadObjectNodeBase_private(project, inputStream, file, objectNode);
            kzsErrorForward(result);

            /* Load LOD node specific data. */
            {
                enum KzuBinaryLODType lodType;
                kzUint lodLevelCount;
                struct KzuObjectNode** objectNodes;
                kzFloat* constraints;
                kzUint i;

                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&lodType);
                kzsErrorForward(result);

                result = kzcInputStreamReadU32Int(inputStream, &lodLevelCount);
                kzsErrorForward(result);

                result = kzcMemoryAllocArray(memoryManager, objectNodes, lodLevelCount, "LOD objects");
                kzsErrorForward(result);

                result = kzcMemoryAllocArray(memoryManager, constraints, lodLevelCount, "LOD constraints");
                kzsErrorForward(result);

                for (i = 0; i < lodLevelCount; ++i)
                {
                    kzString objectPath;

                    result = kzuBinaryDirectoryReadReference(inputStream, file, &objectPath);
                    kzsErrorForward(result);

                    result = kzuProjectLoaderLoadObjectNode(project, objectPath, &objectNodes[i]);
                    kzsErrorForward(result);

                    result = kzcInputStreamReadFloat(inputStream, &constraints[i]);
                    kzsErrorForward(result);
                }

                switch (lodType)
                {
                    case KZU_BINARY_LOD_TYPE_DISTANCE:
                    {
                        result = kzuLODSelectorNodeInitializeDistance(lodSelectorNode, lodLevelCount, constraints, objectNodes);
                        kzsErrorForward(result);
                        break;
                    }
                    case KZU_BINARY_LOD_TYPE_PERSPECTIVE:
                    {
                        result = kzuLODSelectorNodeInitializeScreenArea(lodSelectorNode, lodLevelCount, constraints, objectNodes);
                        kzsErrorForward(result);
                        break;
                    }
                    default:
                    {
                        kzsErrorThrow(KZU_ERROR_INVALID_BINARY_DATA, "Invalid LOD type in binary");
                    }
                }

                result = kzcMemoryFreeArray(objectNodes);
                kzsErrorForward(result);

                result = kzcMemoryFreeArray(constraints);
                kzsErrorForward(result);
            }

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);
        }
    }

    *out_lodSelectorNode = lodSelectorNode;
    kzsSuccess();
}
