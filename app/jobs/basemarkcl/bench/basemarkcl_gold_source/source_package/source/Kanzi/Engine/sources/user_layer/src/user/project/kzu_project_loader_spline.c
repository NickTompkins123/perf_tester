/**
* \file
* Project loader for spline.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_spline.h"

#include "kzu_project_loader_general.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_util.h>
#include <user/kzu_error_codes.h>

#include <core/debug/kzc_log.h>
#include <core/util/curve/kzc_spline.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/memory/kzc_memory_manager.h>


kzsException kzuProjectLoaderLoadSpline(const struct KzuProject* project, kzString path, struct KzcSpline** out_spline)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager;
    struct KzcSpline* spline;

    kzsAssert(kzcIsValidPointer(project));
    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load script with null path");

    spline = (struct KzcSpline*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_SPLINE, path);

    if (!kzcSplineIsValid(spline))
    {
        struct KzcInputStream* inputStream;
        
        kzUint i;
        kzUint dimension;
        kzUint degree;
        kzBool looping;
        kzUint numPoints;
        kzFloat* pointData;
        kzFloat** points;

        memoryManager = kzcMemoryGetManager(project);

        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_SPLINE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load spline file.");

        result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
        kzsErrorForward(result);

        result = kzcInputStreamReadU32Int(inputStream, &dimension);
        kzsErrorForward(result);

        result = kzcInputStreamReadU32Int(inputStream, &degree);
        kzsErrorForward(result);

        result = kzcInputStreamReadBoolean(inputStream, &looping);
        kzsErrorForward(result);

        result = kzcInputStreamSkip(inputStream, 3);
        kzsErrorForward(result);

        result = kzcInputStreamReadU32Int(inputStream, &numPoints);
        kzsErrorForward(result);
        
        result = kzcMemoryAllocArray(memoryManager, pointData, numPoints, "Point Data");
        kzsErrorForward(result);

        for(i = 0; i < numPoints; ++i)
        {
            result = kzcInputStreamReadFloat(inputStream, &pointData[i]);
            kzsErrorForward(result);
        }
        {
            kzUint offset = 0;
            result = kzcMemoryAllocArray(memoryManager, points, numPoints / dimension, "Point Data");
            kzsErrorForward(result);

            for(i = 0; i < numPoints / dimension; ++i)
            {
                kzFloat* data;
                result = kzcMemoryAllocArray(memoryManager, data, 3, "pointData");
                kzsErrorForward(result);
                data[0] = pointData[offset++];
                data[1] = pointData[offset++];
                data[2] = pointData[offset++];
                points[i] = data;
            }
        }

        result = kzcInputStreamDelete(inputStream);
        kzsErrorForward(result);

        /* TODO: Enable when possible. */
        /*result = kzcSplineCreateBezier(memoryManager, dimension, numPoints / dimension, points, looping, &spline);
        kzsErrorForward(result);*/
        result = kzcSplineCreateCatmullRom(memoryManager, dimension, numPoints  / dimension, (const kzFloat* const*)points, looping, &spline);
        kzsErrorForward(result);

        for(i = 0; i < numPoints / dimension; ++i)
        {
            result = kzcMemoryFreeArray(points[i]);
            kzsErrorForward(result);
        }

        result = kzcMemoryFreeArray(points);
        kzsErrorForward(result);

        result = kzcMemoryFreeArray(pointData);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_SPLINE, spline, kzuBinaryFileInfoGetPath(file), 
            (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&spline);
        kzsErrorForward(result);
    }

    *out_spline = spline;
    kzsSuccess();
}
