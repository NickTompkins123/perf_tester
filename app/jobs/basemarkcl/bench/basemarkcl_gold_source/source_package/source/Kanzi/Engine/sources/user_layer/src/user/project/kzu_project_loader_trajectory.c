/**
* \file
* Project loader for trajectory files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_trajectory.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_spline.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_util.h>
#include <user/kzu_error_codes.h>

#include <core/debug/kzc_log.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/curve/kzc_trajectory.h>
#include <core/util/curve/kzc_spline.h>
#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/kzs_types.h>


/** Type of the trajectory in kzb file. */
enum KzuBinaryTrajectoryType
{
    KZU_BINARY_TRAJECTORY_TYPE_POINT_SERIE = 0,
    KZU_BINARY_TRAJECTORY_TYPE_CIRCLE = 1,
    KZU_BINARY_TRAJECTORY_TYPE_LINE = 2
};

/** Type of the interpolation in kzb file. */
/* TODO: Remove these.*/
/*enum KzuBinaryTrajectoryInterpolationType
{
    KZU_BINARY_TRAJECTORY_INTERPOLATION_TYPE_LINEAR = 0,
    KZU_BINARY_TRAJECTORY_INTERPOLATION_TYPE_CATMULL_ROM = 1,
    KZU_BINARY_TRAJECTORY_INTERPOLATION_TYPE_HERMITE = 2
};*/


kzsException kzuProjectLoaderLoadTrajectory(const struct KzuProject* project, kzString path, struct KzcTrajectory** out_trajectory)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager;
    struct KzcTrajectory* trajectory;

    kzsAssert(kzcIsValidPointer(project));
    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load trajectory with null path");

    trajectory = (struct KzcTrajectory*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_TRAJECTORY, path);

    if (!kzcTrajectoryIsValid(trajectory))
    {
        struct KzcInputStream* inputStream;
        enum KzuBinaryTrajectoryType type;

        memoryManager = kzcMemoryGetManager(project);

        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_TRAJECTORY, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load trajectory file.");

        result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
        kzsErrorForward(result);

        result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&type);
        kzsErrorForward(result);

        switch(type)
        {
            case KZU_BINARY_TRAJECTORY_TYPE_POINT_SERIE:
            {
                kzString splinePath;
                struct KzcSpline* spline = KZ_NULL;

                result = kzuBinaryDirectoryReadReference(inputStream, file, &splinePath);
                kzsErrorForward(result);

                if(splinePath != KZ_NULL && kzcStringLength(splinePath) > 0)
                {
                    result = kzuProjectLoaderLoadSpline(project, splinePath, &spline);
                    kzsErrorForward(result);
                }

                result = kzcTrajectoryCreateSpline(memoryManager, spline, &trajectory);
                kzsErrorForward(result);

                break;
            }
            case KZU_BINARY_TRAJECTORY_TYPE_CIRCLE:
            {
                kzFloat radius;
                kzFloat x, y, z;

                result = kzcInputStreamReadFloat(inputStream, &radius);
                kzsErrorForward(result);
                result = kzcInputStreamReadFloat(inputStream, &x);
                kzsErrorForward(result);
                result = kzcInputStreamReadFloat(inputStream, &y);
                kzsErrorForward(result);
                result = kzcInputStreamReadFloat(inputStream, &z);
                kzsErrorForward(result);

                /* TODO: Read normal when tool outputs it. */

                {
                    struct KzcVector3 origin = kzcVector3(0.f, 0.f, 0.f);
                    struct KzcVector3 up = kzcVector3(x, y, z);
                    result = kzcTrajectoryCreateCircle(memoryManager, origin, up, radius, &trajectory);
                    kzsErrorForward(result);
                }

                break;
            }
            case KZU_BINARY_TRAJECTORY_TYPE_LINE:
            {
                kzFloat length;
                kzFloat x, y, z;

                result = kzcInputStreamReadFloat(inputStream, &length);
                kzsErrorForward(result);
                result = kzcInputStreamReadFloat(inputStream, &x);
                kzsErrorForward(result);
                result = kzcInputStreamReadFloat(inputStream, &y);
                kzsErrorForward(result);
                result = kzcInputStreamReadFloat(inputStream, &z);
                kzsErrorForward(result);

                /* TODO: Read origin when tool outputs it. */
                {
                    struct KzcVector3 origin = kzcVector3(0.f, 0.f, 0.f);
                    struct KzcVector3 direction = kzcVector3(x, y, z);
                    if(kzcVector3GetLengthSquared(&direction) < 0.0001f)
                    {
                        kzsLog(KZS_LOG_LEVEL_WARNING, "Line trajectory direction was not set. Defaulting to (1, 0, 0).");
                        direction = kzcVector3(1.0f, 0.0f, 0.0f);
                    }
                    result = kzcTrajectoryCreateLine(memoryManager, origin, direction, length, &trajectory);
                    kzsErrorForward(result);
                }

                break;
            }
            default:
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid type loaded for trajectory");
            }
        }
        
        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_TRAJECTORY, trajectory, kzuBinaryFileInfoGetPath(file), 
            (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&trajectory);
        kzsErrorForward(result);

        result = kzcInputStreamDelete(inputStream);
        kzsErrorForward(result);
    }

    *out_trajectory = trajectory;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadTrajectory(struct KzuProject* project, const struct KzcTrajectory* trajectory)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, trajectory);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
