/**
* \file
* Project loader for script files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_script.h"

#include "kzu_project_loader_general.h"
#include "kzu_project.h"

#include <user/scripting/kzu_script.h>
#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_util.h>
#include <user/kzu_error_codes.h>

#include <core/debug/kzc_log.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/util/string/kzc_string.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/memory/kzc_memory_manager.h>


kzsException kzuProjectLoaderLoadScript(const struct KzuProject* project, kzString path, struct KzuScript** out_script)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager;
    struct KzuScript* script;
    kzMutableString text;

    kzsAssert(kzcIsValidPointer(project));
    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load script with null path");

    script = (struct KzuScript*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_SCRIPT, path);

    if (!kzuScriptIsValid(script))
    {
        memoryManager = kzcMemoryGetManager(project);

        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_SCRIPT, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load script file.");

        {
            struct KzcInputStream* inputStream;
            kzUint fileSize = kzuBinaryFileInfoGetSize(file);

            result = kzcStringAllocate(memoryManager, fileSize, &text);
            kzsErrorForward(result);

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            result = kzcInputStreamReadBytes(inputStream, fileSize, (kzByte*)text);
            kzsErrorForward(result);

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);

            text[fileSize] = KZC_STRING_TERMINATION_SYMBOL;
        }

        result = kzuScriptCreateSource(memoryManager, kzuBinaryFileInfoGetName(file), KZU_SCRIPTING_LANGUAGE_LUA, text, &script);
        kzsErrorForward(result);
        
        result = kzcStringDelete(text);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_SCRIPT, script, kzuBinaryFileInfoGetPath(file), 
            (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&script);
        kzsErrorForward(result);
    }

    *out_script = script;
    kzsSuccess();
}
