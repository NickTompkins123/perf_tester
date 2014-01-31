/**
* \file
* Project loader for object source files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_object_source.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/filter/kzu_object_source.h>
#include <user/filter/kzu_object_set_filter.h>
#include <user/filter/kzu_sort_by_z_object_source.h>
#include <user/filter/kzu_sort_by_material_type_source.h>
#include <user/filter/kzu_object_type_filter.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_light.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>

enum KzuBinaryObjectSourceType
{
    KZU_BINARY_OBJECT_SOURCE_TYPE_ROOT = 0,
    KZU_BINARY_OBJECT_SOURCE_TYPE_COMBINE = 10,
    KZU_BINARY_OBJECT_SOURCE_TYPE_FIXED_SORTER = 50,
    KZU_BINARY_OBJECT_SOURCE_TYPE_FILTER_OBJECT_SET = 100,
    /*KZU_BINARY_OBJECT_SOURCE_TYPE_FILTER_DISTANCE = 101
    KZU_BINARY_OBJECT_SOURCE_TYPE_FILTER_FRUSTUM_CULL = 102
    KZU_BINARY_OBJECT_SOURCE_TYPE_FILTER_CONTAINS_PROPERTY = 103*/
    KZU_BINARY_OBJECT_SOURCE_TYPE_FILTER_OBJECT_TYPE = 104
};

/** Fixed sort types. */
enum KzuBinaryFixedSorterType
{
    KZU_BINARY_FIXED_SORT_Z,    /**< Sort by z. */
    KZU_BINARY_FIXED_SORT_MATERIAL_TYPE /**< Sort by material type. */
};

/* Camera field of view types in binary. */
enum KzuBinaryObjectTypeFilterObjectType
{
    KZU_BINARY_OBJECT_TYPE_FILTER_MESH = 0,
    KZU_BINARY_OBJECT_TYPE_FILTER_CAMERA = 1,
    KZU_BINARY_OBJECT_TYPE_FILTER_LIGHT = 2,
    KZU_BINARY_OBJECT_TYPE_FILTER_CUSTOM = 3,
    KZU_BINARY_OBJECT_TYPE_FILTER_ALL_RENDERABLES = 4
};

kzsException kzuProjectLoaderLoadObjectSource(struct KzuProject* project, kzString path, struct KzuObjectSource** out_objectSource)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuObjectSource* objectSource;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to object source file with null path");

    /* Check if the object source is already loaded. */
    objectSource = (struct KzuObjectSource*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_SOURCE, path);

    if (!kzuObjectSourceIsValid(objectSource))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_OBJECT_SOURCE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load object source file.");

        /* Load the object source of the file info. */
        {
            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                {
                    enum KzuBinaryObjectSourceType type;
                    result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&type);
                    kzsErrorForward(result);

                    if (type == KZU_BINARY_OBJECT_SOURCE_TYPE_ROOT)
                    {
                        objectSource = kzuProjectGetRootSource(project);
                    }
                    else if (type == KZU_BINARY_OBJECT_SOURCE_TYPE_COMBINE)
                    {
                        kzUint numInputs;
                        kzUint i;
                        struct KzuObjectSource** inputs;

                        result = kzcInputStreamReadU32Int(inputStream, &numInputs);
                        kzsErrorForward(result);

                        result = kzcMemoryAllocArray(memoryManager, inputs, numInputs, "Object combiner inputs");
                        kzsErrorForward(result);

                        for (i = 0; i < numInputs; ++i)
                        {
                            kzString inputPath;

                            result = kzuBinaryDirectoryReadReference(inputStream, file, &inputPath);
                            kzsErrorForward(result);

                            result = kzuProjectLoaderLoadObjectSource(project, inputPath, &inputs[i]);
                            kzsErrorForward(result);
                        }

                        result = kzuCombinerObjectSourceCreate(memoryManager, numInputs, inputs, &objectSource);
                        kzsErrorForward(result);
                    }
                    else if (type == KZU_BINARY_OBJECT_SOURCE_TYPE_FIXED_SORTER)
                    {
                        struct KzuObjectSource* input;
                        kzBool isReversed;
                        enum KzuBinaryFixedSorterType sortType;

                        /* Load input of the filter object source. */
                        {
                            kzString inputPath;

                            result = kzuBinaryDirectoryReadReference(inputStream, file, &inputPath);
                            kzsErrorForward(result);

                            result = kzuProjectLoaderLoadObjectSource(project, inputPath, &input);
                            kzsErrorForward(result);
                        }
                        
                        result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&sortType);
                        kzsErrorForward(result);            

                        result = kzcInputStreamReadBoolean(inputStream, &isReversed);
                        kzsErrorForward(result);

                        switch(sortType)
                        {
                            case KZU_BINARY_FIXED_SORT_Z:
                            {
                                result = kzuSortByZObjectSourceCreate(memoryManager, input, isReversed, &objectSource);
                                kzsErrorForward(result);
                                break;
                            }
                            case KZU_BINARY_FIXED_SORT_MATERIAL_TYPE:
                            {
                                result = kzuSortByMaterialTypeObjectSourceCreate(memoryManager, input, &objectSource);
                                kzsErrorForward(result);
                                break;
                            }
                            default:
                            {
                                kzsErrorThrow(KZU_ERROR_INVALID_BINARY_DATA, "Invalid sort type for sort object source!");
                            }
                        }
                    }

                    else /* All other object sources are filter object sources. */
                    {
                        struct KzuObjectSource* input;

                        /* Load input of the filter object source. */
                        {
                            kzString inputPath;

                            result = kzuBinaryDirectoryReadReference(inputStream, file, &inputPath);
                            kzsErrorForward(result);

                            result = kzuProjectLoaderLoadObjectSource(project, inputPath, &input);
                            kzsErrorForward(result);
                        }

                        switch (type)
                        {
                            case KZU_BINARY_OBJECT_SOURCE_TYPE_FILTER_OBJECT_SET:
                            {
                                kzUint objectCount;
                                kzUint i;
                                struct KzuObjectNode** objects;
                                kzBool isExlusive;

                                result = kzcInputStreamReadBoolean(inputStream, &isExlusive);
                                kzsErrorForward(result);
                                result = kzcInputStreamSkip(inputStream, 3);
                                kzsErrorForward(result);

                                result = kzcInputStreamReadU32Int(inputStream, &objectCount);
                                kzsErrorForward(result);

                                result = kzcMemoryAllocArray(memoryManager, objects, objectCount, "Object set filter objects");
                                kzsErrorForward(result);

                                for (i = 0; i < objectCount; ++i)
                                {
                                    kzString objectPath;

                                    result = kzuBinaryDirectoryReadReference(inputStream, file, &objectPath);
                                    kzsErrorForward(result);

                                    result = kzuProjectLoaderLoadObjectNode(project, objectPath, &objects[i]);
                                    kzsErrorForward(result);
                                }

                                result = kzuObjectSetFilterCreate(memoryManager, input, objectCount, objects, isExlusive, &objectSource);
                                kzsErrorForward(result);

                                result = kzcMemoryFreeArray(objects);
                                kzsErrorForward(result);

                                break;
                            }
                            case KZU_BINARY_OBJECT_SOURCE_TYPE_FILTER_OBJECT_TYPE:
                            {
                                kzBool isExlusive;
                                KzuObjectType engineObjectType;
                                enum KzuBinaryObjectTypeFilterObjectType objectType;

                                result = kzcInputStreamReadBoolean(inputStream, &isExlusive);
                                kzsErrorForward(result);
                                result = kzcInputStreamSkip(inputStream, 3);
                                kzsErrorForward(result);

                                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&objectType);
                                kzsErrorForward(result);

                                switch(objectType)
                                {
                                    case KZU_BINARY_OBJECT_TYPE_FILTER_MESH:
                                    {
                                        engineObjectType = KZU_OBJECT_TYPE_MESH;
                                        break;
                                    }
                                    case KZU_BINARY_OBJECT_TYPE_FILTER_CAMERA:
                                    {
                                        engineObjectType = KZU_OBJECT_TYPE_CAMERA;
                                        break;
                                    }
                                    case KZU_BINARY_OBJECT_TYPE_FILTER_LIGHT:
                                    {
                                        engineObjectType = KZU_OBJECT_TYPE_LIGHT;
                                        break;
                                    }
                                    case KZU_BINARY_OBJECT_TYPE_FILTER_CUSTOM:
                                    {
                                        engineObjectType = KZU_OBJECT_TYPE_EMPTY; /* TODO: Consider some other value instead. */
                                        break;
                                    }
                                    case KZU_BINARY_OBJECT_TYPE_FILTER_ALL_RENDERABLES:
                                    {
                                        engineObjectType = KZU_OBJECT_TYPE_RENDERABLE;
                                        break;
                                    }
                                    default:
                                    {
                                        kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid object type filter object type");
                                    }
                                }

                                result = kzuObjectTypeFilterCreate(memoryManager, input, engineObjectType, isExlusive, &objectSource);
                                kzsErrorForward(result);

                                break;
                            }

                            case KZU_BINARY_OBJECT_SOURCE_TYPE_ROOT: /* Root object source type is not allowed here. */
                            case KZU_BINARY_OBJECT_SOURCE_TYPE_COMBINE: /* Combine object source type is not allowed here. */
                            case KZU_BINARY_OBJECT_SOURCE_TYPE_FIXED_SORTER: /* Sorter object source type is not allowed here. */
                            default:
                            {
                                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid binary object source type");
                            }
                        }
                    }
                }

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_SOURCE, objectSource, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&objectSource);
            kzsErrorForward(result);
        }
    }

    *out_objectSource = objectSource;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadObjectSource(struct KzuProject* project, const struct KzuObjectSource* objectSource)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, objectSource);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
