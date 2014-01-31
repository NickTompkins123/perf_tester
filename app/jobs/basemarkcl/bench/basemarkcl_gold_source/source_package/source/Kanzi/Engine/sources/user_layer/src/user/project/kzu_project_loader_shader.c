/**
* \file
* Project loader for shader files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_shader.h"

#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/string/kzc_string.h>


kzsException kzuProjectLoaderLoadShaderSource(const struct KzuProject* project, kzString path, kzMutableString* out_shaderSource)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    kzMutableString shaderSource;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load shader source with null path");

    /* Check if the shader source is already loaded. */
    shaderSource = (kzMutableString)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE, path);

    if (shaderSource == KZ_NULL)
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_SHADER_SOURCE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load shader source file.");

        /* Load the shader source of the file info. */
        {
            /* Read data */
            {
                struct KzcInputStream* inputStream;
                kzUint fileSize = kzuBinaryFileInfoGetSize(file);

                result = kzcStringAllocate(memoryManager, fileSize, &shaderSource);
                kzsErrorForward(result);

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzcInputStreamReadBytes(inputStream, fileSize, (kzByte*)shaderSource);
                kzsErrorForward(result);

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);

                shaderSource[fileSize] = KZC_STRING_TERMINATION_SYMBOL;
            }

            result = kzuProjectAddObject(project, KZU_PROJECT_OBJECT_TYPE_SHADER_SOURCE, shaderSource, kzuBinaryFileInfoGetPath(file));
            kzsErrorForward(result);
        }
    }

    *out_shaderSource = shaderSource;
    kzsSuccess();
}
