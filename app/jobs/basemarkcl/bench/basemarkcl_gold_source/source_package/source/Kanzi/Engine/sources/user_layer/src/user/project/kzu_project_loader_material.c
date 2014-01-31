/**
* \file
* Project loader for material files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_material.h"

#include "kzu_project_loader.h"
#include "kzu_project_gpu_memory_type.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_property.h"
#include "kzu_project_loader_shader.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_util.h>
#include <user/material/kzu_material.h>
#include <user/material/kzu_material_type.h>
#include <user/properties/kzu_property.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/string/kzc_string.h>
#include <core/debug/kzc_log.h>

#include <system/wrappers/kzs_opengl_base.h>
#include <system/debug/kzs_log.h>
#include <system/time/kzs_tick.h>


enum KzuBinaryShaderType
{
    KZU_BINARY_SHADER_TYPE_SEPARATE = 0,
    KZU_BINARY_SHADER_TYPE_COMBINED = 1
};


kzsException kzuProjectLoaderLoadMaterialType(struct KzuProject* project, kzString path, struct KzuMaterialType** out_materialType)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuMaterialType* materialType;
    kzUint measurementStart = kzsTimeGetCurrentTimestamp();

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load material type with null path");

    /* Check if the material type is already loaded. */
    materialType = (struct KzuMaterialType*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE, path);

    /* TODO: Handle binary shaders */
    if (!kzuMaterialTypeIsValid(materialType))
    {
        kzBool validShader = KZ_FALSE;
        enum KzuBinaryGpuMemoryType memoryType;
        enum KzcResourceMemoryType resourceMemoryType;

        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_MATERIAL_TYPE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load material type file.");

        /* Load the material type of the file info. */
        {
            /* Read data */
            {
                struct KzcInputStream* inputStream;
                kzString vertexShaderPath;
                kzString fragmentShaderPath;

                struct KzcDynamicArray* attributes;
                struct KzcDynamicArray* uniforms;

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
                enum KzuBinaryShaderType binaryShaderType = KZU_BINARY_SHADER_TYPE_SEPARATE;
                kzU8* binaryShaderData[2];
                kzUint binaryShaderDataLength[2];

                binaryShaderData[0] = KZ_NULL;
                binaryShaderData[1] = KZ_NULL;
                binaryShaderDataLength[0] = 0;
                binaryShaderDataLength[1] = 0;
#endif

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzuMaterialTypeCreate(memoryManager, &materialType);
                kzsErrorForward(result); 

                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&memoryType);
                kzsErrorForward(result);

                result = kzuBinaryDirectoryReadReference(inputStream, file, &vertexShaderPath);
                kzsErrorForward(result);

                result = kzuBinaryDirectoryReadReference(inputStream, file, &fragmentShaderPath);
                kzsErrorForward(result);

                /* Binary shaders. */
                {
                    kzUint binaryShaderCount;
                    kzUint i;

                    result = kzcInputStreamReadU32Int(inputStream, &binaryShaderCount);
                    kzsErrorForward(result);

                    for(i = 0; i < binaryShaderCount; ++i)
                    {
                        kzMutableString formatName;
                        kzUint tempShaderDataLength[2];
                        kzU8* tempShaderData[2];
                        enum KzuBinaryShaderType tempShaderType;

                        tempShaderData[0] = KZ_NULL;
                        tempShaderData[1] = KZ_NULL;
                        tempShaderDataLength[0] = 0;
                        tempShaderDataLength[1] = 0;

                        result = kzuBinaryReadString(memoryManager, inputStream, &formatName);
                        kzsErrorForward(result);

                        result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&tempShaderType);
                        kzsErrorForward(result);

                        switch(tempShaderType)
                        {
                            case KZU_BINARY_SHADER_TYPE_SEPARATE:
                            {
                                /* Separate vertex. */
                                result = kzcInputStreamReadU32Int(inputStream, &tempShaderDataLength[0]);
                                kzsErrorForward(result);
                                /* Create memory buffer for binary data. */
                                result = kzcMemoryAllocPointer(memoryManager, &tempShaderData[0], sizeof(kzU8) * tempShaderDataLength[0], 
                                    "Temporary buffer for binary vertex data");
                                kzsErrorForward(result);
                                /* Fill the buffer from stream. */
                                result = kzcInputStreamReadBytes(inputStream, tempShaderDataLength[0], tempShaderData[0]);
                                kzsErrorForward(result);

                                /* Separate fragment. */
                                result = kzcInputStreamReadU32Int(inputStream, &tempShaderDataLength[1]);
                                kzsErrorForward(result);
                                /* Create memory buffer for binary data. */
                                result = kzcMemoryAllocPointer(memoryManager, &tempShaderData[1], sizeof(kzU8) * tempShaderDataLength[1], 
                                    "Temporary buffer for binary fragment data");
                                kzsErrorForward(result);
                                /* Fill the buffer from stream. */
                                result = kzcInputStreamReadBytes(inputStream, tempShaderDataLength[1], tempShaderData[1]);
                                kzsErrorForward(result);

                                break;
                            }
                            case KZU_BINARY_SHADER_TYPE_COMBINED:
                            {
                                /* Combined data */
                                result = kzcInputStreamReadU32Int(inputStream, &tempShaderDataLength[0]);
                                kzsErrorForward(result);
                                /* Create memory buffer for binary data. */
                                result = kzcMemoryAllocPointer(memoryManager, &tempShaderData[0], sizeof(kzU8) * tempShaderDataLength[0], 
                                    "Temporary buffer for binary combined data");
                                kzsErrorForward(result);
                                /* Fill the buffer from stream. */
                                result = kzcInputStreamReadBytes(inputStream, tempShaderDataLength[0], tempShaderData[0]);
                                kzsErrorForward(result);

                                break;
                            }

                            default:
                            {
                                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid binary shader type");
                            }
                        }
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
                        if (kzcStringIsEqual(formatName, KZS_OPENGL_BINARY_NAME))
                        {
                            binaryShaderType = tempShaderType;
                            binaryShaderData[0] = tempShaderData[0];
                            binaryShaderData[1] = tempShaderData[1];
                            binaryShaderDataLength[0] = tempShaderDataLength[0];
                            binaryShaderDataLength[1] = tempShaderDataLength[1];
                        }
                        else
#endif
                        {
                            switch(tempShaderType)
                            {
                                case KZU_BINARY_SHADER_TYPE_SEPARATE:
                                {
                                    result = kzcMemoryFreePointer(tempShaderData[0]);
                                    kzsErrorForward(result);
                                    result = kzcMemoryFreePointer(tempShaderData[1]);
                                    kzsErrorForward(result);
                                    break;
                                }
                                case KZU_BINARY_SHADER_TYPE_COMBINED:
                                {
                                    result = kzcMemoryFreePointer(tempShaderData[0]);
                                    kzsErrorForward(result);
                                    break;
                                }
                                default:
                                {
                                    kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid binary shader type");
                                }
                            }
                        }

                        result = kzcStringDelete(formatName);
                        kzsErrorForward(result);
                    }
                }

                /* Property types */
                {
                    kzUint numPropertyTypes;
                    kzUint i;

                    result = kzcInputStreamReadU32Int(inputStream, &numPropertyTypes);
                    kzsErrorForward(result);

                    for (i = 0; i < numPropertyTypes; ++i)
                    {
                        kzString propertyTypePath;
                        struct KzuPropertyType* propertyType;

                        result = kzuBinaryDirectoryReadReference(inputStream, file, &propertyTypePath);
                        kzsErrorForward(result);

                        result = kzuProjectLoaderLoadPropertyType(project, propertyTypePath, &propertyType);
                        kzsErrorForward(result);

                        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE); /* TODO: Set to true already to binary (future format) in tool. */

                        if (propertyType == KZ_NULL)
                        {
                            /* Unknown property type is not an error due to flexible support for future binary formats as well. */
                            kzsLog(KZS_LOG_LEVEL_WARNING, "Unable to load property due to unknown property type");
                        }
                        else
                        {
                            result = kzuMaterialTypeAddPropertyType(materialType, propertyType);
                            kzsErrorForward(result);
                        }
                    }
                }

                /* Attributes */
                {
                    kzUint numAttributes;
                    kzUint i;

                    result = kzcInputStreamReadU32Int(inputStream, &numAttributes);
                    kzsErrorForward(result);

                    result = kzcDynamicArrayCreate(memoryManager, &attributes);
                    kzsErrorForward(result);

                    for (i = 0; i < numAttributes; ++i)
                    {
                        kzMutableString attributeName;

                        result = kzuBinaryReadString(memoryManager, inputStream, &attributeName);
                        kzsErrorForward(result);

                        result = kzcDynamicArrayAdd(attributes, attributeName);
                        kzsErrorForward(result);
                    }
                }

                /* Uniforms */
                {
                    kzUint numUniforms;
                    kzUint i;

                    result = kzcInputStreamReadU32Int(inputStream, &numUniforms);
                    kzsErrorForward(result);

                    result = kzcDynamicArrayCreate(memoryManager, &uniforms);
                    kzsErrorForward(result);

                    for (i = 0; i < numUniforms; ++i)
                    {
                        kzMutableString uniformName;

                        result = kzuBinaryReadString(memoryManager, inputStream, &uniformName);
                        kzsErrorForward(result);

                        result = kzcDynamicArrayAdd(uniforms, uniformName);
                        kzsErrorForward(result);
                    }
                }

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);

                /* Create shader */
                {
                    struct KzuMaterialTypeShaderInfo shaderInfo;
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
                    if(binaryShaderData[0] == KZ_NULL)
                    {
                        kzMutableString vertexShaderSource;
                        kzMutableString fragmentShaderSource;

                        if (vertexShaderPath == KZ_NULL)
                        {
                            vertexShaderSource = KZ_NULL;
                        }
                        else
                        {
                            result = kzuProjectLoaderLoadShaderSource(project, vertexShaderPath, &vertexShaderSource);
                            kzsErrorForward(result);
                        }

                        if (fragmentShaderPath == KZ_NULL)
                        {
                            fragmentShaderSource = KZ_NULL;
                        }
                        else
                        {
                            result = kzuProjectLoaderLoadShaderSource(project, fragmentShaderPath, &fragmentShaderSource);
                            kzsErrorForward(result);
                        }

                        kzuMaterialTypeShaderInfoSetSourceShader(&shaderInfo, vertexShaderSource, fragmentShaderSource, uniforms, attributes);
                    }
                    /* Binary shader. */
                    else
                    {
                        if(binaryShaderType == KZU_BINARY_SHADER_TYPE_SEPARATE)
                        {
                            kzuMaterialTypeShaderInfoSetBinaryShaderSeparated(&shaderInfo, binaryShaderData[0], binaryShaderDataLength[0],
                                binaryShaderData[1], binaryShaderDataLength[1], uniforms, attributes);
                        }
                        else
                        {
                            kzuMaterialTypeShaderInfoSetBinaryShaderCombined(&shaderInfo, binaryShaderData[0], binaryShaderDataLength[0],
                                uniforms, attributes);
                        }
                    }
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
                    {
                        kzuMaterialTypeShaderInfoSetNoShader(&shaderInfo, attributes);
                    }
#elif KZ_OPENGL_VERSION == KZ_OPENGL_NONE
                    {
                        kzuMaterialTypeShaderInfoSetNoShader(&shaderInfo, attributes);
                    }
#else
#error KZ_OPENGL_VERSION undefined or unknown
#endif

                    if (shaderInfo.type != KZU_SHADER_TYPE_SOURCE || (shaderInfo.shaderData.code.vertex != KZ_NULL))
                    {
                        result = kzuProjectLoaderGetMemoryType_private(memoryType, &resourceMemoryType);
                        kzsErrorForward(result);
                        result = kzuMaterialTypeSetShader(materialType, kzuProjectGetResourceManager(project), resourceMemoryType, &shaderInfo, &validShader);
                        kzsErrorForward(result);
                    }

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
                    if(shaderInfo.type == KZU_SHADER_TYPE_BINARY_SEPARATED)
                    {
                        result = kzcMemoryFreePointer(binaryShaderData[0]);
                        kzsErrorForward(result);
                        result = kzcMemoryFreePointer(binaryShaderData[1]);
                        kzsErrorForward(result);
                    }
                    else if(shaderInfo.type == KZU_SHADER_TYPE_BINARY_COMBINED)
                    {
                        result = kzcMemoryFreePointer(binaryShaderData[0]);
                        kzsErrorForward(result);
                    }
#endif
                }

                /* Finally attributes and uniforms should be deleted. */
                
                {
                    struct KzcDynamicArrayIterator it;
                    it = kzcDynamicArrayGetIterator(attributes);
                    while(kzcDynamicArrayIterate(it))
                    {
                        kzMutableString attributeName = (kzMutableString)kzcDynamicArrayIteratorGetValue(it);

                        result = kzcStringDelete(attributeName);
                        kzsErrorForward(result);
                    }
                    result = kzcDynamicArrayDelete(attributes);
                    kzsErrorForward(result);

                    it = kzcDynamicArrayGetIterator(uniforms);
                    while(kzcDynamicArrayIterate(it))
                    {
                        kzMutableString uniformName = (kzMutableString)kzcDynamicArrayIteratorGetValue(it);

                        result = kzcStringDelete(uniformName);
                        kzsErrorForward(result);

                    }
                    result = kzcDynamicArrayDelete(uniforms);
                    kzsErrorForward(result);
                }
                
                /* Fallback to  */
                if(!validShader)
                {
                    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Erroneous material type: %s", kzuBinaryFileInfoGetName(file));
                    kzsErrorForward(result);
                }
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_MATERIAL_TYPE, materialType, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&materialType);
            kzsErrorForward(result);
        }
    }

    kzuProjectAddMeasurementCumulativeTime_private((struct KzuProject*)project, measurementStart, KZU_PROJECT_MEASUREMENT_MATERIAL_TYPE);

    *out_materialType = materialType;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadMaterial(struct KzuProject* project, kzString path, struct KzuMaterial** out_material)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuMaterial* material;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load material with null path");

    /* Check if the material is already loaded. */
    material = (struct KzuMaterial*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_MATERIAL, path);

    if (!kzuMaterialIsValid(material))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_MATERIAL, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load material file.");

        /* Load the material of the file info. */
        {
            kzString materialTypePath;
            struct KzuMaterialType* materialType;

            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzuBinaryDirectoryReadReference(inputStream, file, &materialTypePath);
                kzsErrorForward(result);

                /* TODO: Move loading of material type after properties somehow. */
                result = kzuProjectLoaderLoadMaterialType(project, materialTypePath, &materialType);
                kzsErrorForward(result);

                /* Material type loading failed, assign null material. */
                if (kzuMaterialTypeGetVertexAttributes(materialType) == KZ_NULL)
                {
                    material = KZ_NULL;
                }
                else
                {
                    result = kzuMaterialCreate(memoryManager, kzuProjectGetPropertyManager(project), materialType, kzuBinaryFileInfoGetName(file), &material);
                    kzsErrorForward(result);

                    result = kzuProjectLoaderReadProperties(project, inputStream, file, material);
                    kzsErrorForward(result);
                }

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            if (material != KZ_NULL)
            {
                result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_MATERIAL, material, kzuBinaryFileInfoGetPath(file),
                    (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&material);
                kzsErrorForward(result);
            }
        }
    }

    *out_material = material;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadMaterialType(struct KzuProject* project, const struct KzuMaterialType* materialType)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, materialType);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadMaterial(struct KzuProject* project, const struct KzuMaterial* material)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, material);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
