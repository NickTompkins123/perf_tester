/**
* \file
* Project loader for camera files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_camera.h"

#include "kzu_project_loader_general.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project_loader_object_node_private.h"
#include "kzu_project_loader_property.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_property.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/io/kzc_input_stream.h>


/* Camera field of view types in binary. */
enum KzuBinaryCameraFOVType
{
    KZU_BINARY_CAMERA_FOV_TYPE_XFOV = 0,
    KZU_BINARY_CAMERA_FOV_TYPE_YFOV = 1
};

/* Camera orthogonal coordinate system type in binary. */
enum KzuBinaryCameraOrthogonalCoordinateSystemType
{
    KZU_BINARY_CAMERA_ORTHOGONAL_COORDINATE_SYSTEM_TYPE_ABSOLUTE = 0,
    KZU_BINARY_CAMERA_ORTHOGONAL_COORDINATE_SYSTEM_TYPE_RELATIVE = 1
};

/* Camera projection types in binary. */
enum KzuBinaryCameraProjectionType
{
    KZU_BINARY_CAMERA_PROJECTION_TYPE_PERSPECTIVE = 0,
    KZU_BINARY_CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC = 1
};

kzsException kzuProjectLoaderLoadCameraNode(struct KzuProject* project, kzString path, struct KzuCameraNode** out_cameraNode)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuCameraNode* cameraNode;
    struct KzuObjectNode* objectNode;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load camera node with null path");

    /* Check if the object node is already loaded. */
    objectNode = (struct KzuObjectNode*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, path);

    if (kzuObjectNodeIsValid(objectNode))
    {
        cameraNode = kzuCameraNodeFromObjectNode(objectNode);
    }
    else
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_CAMERA_NODE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load camera node file.");

        /* Create camera node. */
        result = kzuCameraNodeCreate(memoryManager, kzuProjectGetPropertyManager(project), kzuProjectGetMessageDispatcher(project),
                                     kzuBinaryFileInfoGetName(file), &cameraNode);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, kzuCameraNodeToObjectNode(cameraNode), kzuBinaryFileInfoGetPath(file),
            (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&objectNode);
        kzsErrorForward(result);

        cameraNode = kzuCameraNodeFromObjectNode(objectNode);

        {
            struct KzcInputStream* inputStream;

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            /* Load object node base. */
            result = kzuProjectLoaderLoadObjectNodeBase_private(project, inputStream, file, objectNode);
            kzsErrorForward(result);

            /* No camera node specific data. */

            /* Translate binary enums to engine enums. */
            {
                /* Field of view type */
                enum KzuBinaryCameraFOVType binaryFOVType = /*lint -e{930}*/(enum KzuBinaryCameraFOVType)kzuCameraNodeGetFovType(cameraNode);
                enum KzuFieldOfViewType fovType;
                switch (binaryFOVType)
                {
                    case KZU_BINARY_CAMERA_FOV_TYPE_XFOV: fovType = KZU_CAMERA_FIELD_OF_VIEW_VERTICAL; break;
                    case KZU_BINARY_CAMERA_FOV_TYPE_YFOV: fovType = KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL; break;
                    default:
                    {
                        kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid camera field of view type");
                    }
                }
                result = kzuCameraNodeSetFovType(cameraNode, fovType);
                kzsErrorForward(result);
            }
            {
                /* Projection type */
                enum KzuBinaryCameraProjectionType binaryProjectionType = /*lint -e{930}*/(enum KzuBinaryCameraProjectionType)kzuCameraNodeGetProjectionType(cameraNode);
                enum KzuProjectionType projectionType;
                switch (binaryProjectionType)
                {
                    case KZU_BINARY_CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC: projectionType = KZU_PROJECTION_ORTHOGRAPHIC; break;
                    case KZU_BINARY_CAMERA_PROJECTION_TYPE_PERSPECTIVE: projectionType = KZU_PROJECTION_PERSPECTIVE; break;
                    default:
                    {
                        kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid camera projection type");
                    }
                }
                result = kzuCameraNodeSetProjectionType(cameraNode, projectionType);
                kzsErrorForward(result);
            }
            {
                /* Coordinate system */
                enum KzuBinaryCameraOrthogonalCoordinateSystemType binaryCoordinateSystem = /*lint -e{930}*/(enum KzuBinaryCameraOrthogonalCoordinateSystemType)kzuCameraNodeGetOrthogonalCoordinateSystemType(cameraNode);
                enum KzuOrthogonalCoordinateSystem coordinateSystem;
                switch (binaryCoordinateSystem)
                {
                    case KZU_BINARY_CAMERA_ORTHOGONAL_COORDINATE_SYSTEM_TYPE_ABSOLUTE: coordinateSystem = KZU_ORTHOGONAL_COORDINATE_SYSTEM_ABSOLUTE; break;
                    case KZU_BINARY_CAMERA_ORTHOGONAL_COORDINATE_SYSTEM_TYPE_RELATIVE: coordinateSystem = KZU_ORTHOGONAL_COORDINATE_SYSTEM_RELATIVE; break;
                    default:
                    {
                        kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid camera orthogonal coordinate system type");
                    }
                }
                result = kzuCameraNodeSetOrthogonalCoordinateSystemType(cameraNode, coordinateSystem);
                kzsErrorForward(result);
            }

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);
        }
    }

    *out_cameraNode = cameraNode;
    kzsSuccess();
}
