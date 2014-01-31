/**
* \file
* Resource file reading functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_resource_file.h"

#include "kzs_file_base.h"

#include "kzs_file.h"

#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_string.h>
#include <system/kzs_error_codes.h>
#include <system/debug/kzs_log.h>


/**
 * Resource structure
 */
struct KzsResourceFile
{
    kzsFile* file;
    kzUint position;
};


kzsError kzsResourceFileCreate(kzString resourcePath, struct KzsResourceFile** out_resourceFile)
{
    struct KzsResourceFile* resourceFile;
    kzString resourceDirectory = kzsFileBaseGetResourceDirectory();

    resourceFile = kzsMalloc(sizeof(*resourceFile));
    kzsErrorTest(resourceFile != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while opening resource");

    if (resourceDirectory != KZ_NULL)
    {
        kzUint resourceDirectoryLength = kzsStrlen(resourceDirectory);
        kzUint resourcePathLength = kzsStrlen(resourcePath);
        /* resourceDirectory + '/' + resourcePath + '\0' */
        kzUint fullPathLength = resourceDirectoryLength + 1 + resourcePathLength + 1;
        kzMutableString fullPath = (kzMutableString)kzsMalloc(fullPathLength);
        kzsStrcpy(fullPath, resourceDirectory);
        kzsStrcat(fullPath, "/");
        kzsStrcat(fullPath, resourcePath);
        resourceFile->file = kzsFopen(fullPath, KZS_FILE_READONLY_BINARY);

        if(resourceFile->file == KZ_NULL)
        {
            kzChar errorMessage[1024] = "";
            kzString errorLog = "Failed to open resource file from resourceDirectory ";
            kzUint length = kzsStrlen(errorLog);

            kzsStrcpy(errorMessage, errorLog);
            kzsStrncat(errorMessage, fullPath, 1023 - length);
            errorMessage[1023] = '\0';

            kzsErrorThrow(KZS_ERROR_FILE_OPEN_FAILED, errorMessage);
        }
        kzsFree(fullPath);
    }
    else
    {
        resourceFile->file = kzsFopen(resourcePath, KZS_FILE_READONLY_BINARY);

        if(resourceFile->file == KZ_NULL)
        {
            kzChar errorMessage[256] = "";
            kzString errorLog = "Failed to open resource file ";
            kzUint length = kzsStrlen(errorLog);

            kzsStrcpy(errorMessage, errorLog);
            kzsStrncat(errorMessage, resourcePath, 255 - length);
            errorMessage[255] = '\0';

            kzsErrorThrow(KZS_ERROR_FILE_OPEN_FAILED, errorMessage);
        }
    }

    resourceFile->position = 0;

    *out_resourceFile = resourceFile;
    kzsSuccess();
}

kzsError kzsResourceFileDelete(struct KzsResourceFile* resourceFile)
{
    kzInt result = kzsFclose(resourceFile->file);
    kzsErrorTest(result == 0, KZS_ERROR_FILE_OPERATION_FAILED, "Failed to close resource file.");

    kzsFree(resourceFile);

    kzsSuccess();
}

kzInt kzsResourceFileRead(struct KzsResourceFile* resourceFile, kzUint byteCount, kzByte* out_buffer)
{
    kzsFile* file = resourceFile->file;
    kzInt result;

    if (kzsFeof(file))
    {
        result = 0;
    }
    else
    {
        kzUint readByteCount = (kzUint)kzsFread(out_buffer, sizeof(kzByte), byteCount, file);
        resourceFile->position += readByteCount;

        if (kzsFerror(file) != 0)
        {
            result = KZS_RESOURCE_FILE_ERROR;
        }
        else
        {
            result = (kzInt)readByteCount;
        }
    }

    return result;
}

kzInt kzsResourceFileSkip(struct KzsResourceFile* resourceFile, kzUint byteCount)
{
    kzsFile* file = resourceFile->file;
    kzInt result = kzsFileReadSkip(file, byteCount);

    if (result != KZS_FILE_ERROR)
    {
        resourceFile->position += (kzUint)result;
    }
    else
    {
        result = KZS_RESOURCE_FILE_ERROR;
    }

    return result;
}

kzUint kzsResourceFileGetPosition(const struct KzsResourceFile* resourceFile)
{
    return resourceFile->position;
}
