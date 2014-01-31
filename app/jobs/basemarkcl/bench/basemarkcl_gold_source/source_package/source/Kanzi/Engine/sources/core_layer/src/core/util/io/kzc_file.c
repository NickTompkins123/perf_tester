/**
* \file
* Core file utilities.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_file.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/util/string/kzc_string.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/util/collection/kzc_dynamic_array.h>

#include <system/file/kzs_file.h>
#include <system/file/kzs_resource_file.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/file/kzs_file_base.h>
#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_string.h>
#include <system/debug/kzs_log.h>


/* TODO: Additional file system functions such as directory manipulation, listing etc. */

kzsError kzcFileGetSize(kzString filePath, kzUint* out_size)
{
    kzInt size = kzsFileGetSize(filePath);
    kzsErrorTest(size >= 0, KZS_ERROR_FILE_OPEN_FAILED, "Failed to query file size");

    *out_size = (kzUint)size;
    kzsSuccess();
}

kzsError kzcFileGetExtension(const struct KzcMemoryManager* memoryManager, kzString filePath, kzMutableString* out_extension)
{
    kzsError result;
    kzMutableString* splitStrings;
    kzUint splitPartCount;
    kzMutableString fileExtension;

    result = kzcStringSplit(memoryManager, filePath, ".", &splitPartCount, &splitStrings);
    kzsErrorForward(result);

    kzsAssert(splitPartCount > 0);

    if(splitPartCount == 1)
    {
        fileExtension = KZ_NULL;
    }
    else
    {
        fileExtension = splitStrings[splitPartCount - 1];
    }

    /* Remove split results */
    {
        kzUint i;
        kzUint maxIndex = (splitPartCount == 1) ? 1 : (splitPartCount - 1);
        for (i = 0; i < maxIndex; ++i)
        {
            result = kzcStringDelete(splitStrings[i]);
            kzsErrorForward(result);
        }
    }

    result = kzcMemoryFreeArray(splitStrings);
    kzsErrorForward(result);

    *out_extension = fileExtension;
    kzsSuccess();
}

kzsError kzcFileReadBinaryFile(const struct KzcMemoryManager* memoryManager, kzString filePath, kzUint* out_size, kzByte** out_data)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    kzUint size;
    kzByte* data;

    result = kzcInputStreamCreateFromFile(memoryManager, filePath, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &inputStream);
    kzsErrorForward(result);

    result = kzcInputStreamReadAllBytes(inputStream, memoryManager, &size, &data);
    kzsErrorForward(result);

    result = kzcInputStreamDelete(inputStream);
    kzsErrorForward(result);

    *out_size = size;
    *out_data = data;
    kzsSuccess();
}

kzsError kzcFileReadBinaryResource(const struct KzcMemoryManager* memoryManager, kzString resourcePath, kzUint* out_size, kzByte** out_data)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    kzUint size;
    kzByte* data;

    result = kzcInputStreamCreateFromResource(memoryManager, resourcePath, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &inputStream);
    kzsErrorForward(result);

    result = kzcInputStreamReadAllBytes(inputStream, memoryManager, &size, &data);
    kzsErrorForward(result);

    result = kzcInputStreamDelete(inputStream);
    kzsErrorForward(result);

    *out_size = size;
    *out_data = data;
    kzsSuccess();
}

kzsError kzcFileWriteBinaryFile(const struct KzcMemoryManager* memoryManager, kzString filePath, kzUint size, const kzByte* data)
{
    kzsError result;

    struct KzcOutputStream* outputStream;

    result = kzcOutputStreamCreateToFile(memoryManager, filePath, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &outputStream);
    kzsErrorForward(result);

    result = kzcOutputStreamWriteBytes(outputStream, size, data);
    kzsErrorForward(result);

    result = kzcOutputStreamDelete(outputStream);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcFileWriteTextFile(const struct KzcMemoryManager* memoryManager, kzString filePath, kzUint lineCount, const kzString* lines)
{
    kzsError result;
    struct KzcOutputStream* outputStream;
    kzUint i;

    result = kzcOutputStreamCreateToFile(memoryManager, filePath, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &outputStream);
    kzsErrorForward(result);

    for (i = 0; i < lineCount; ++i)
    {
        result = kzcOutputStreamWriteBytes(outputStream, kzcStringLength(lines[i]), (kzByte*)lines[i]);
        kzsErrorForward(result);

        result = kzcOutputStreamWriteU8(outputStream, (kzU8)'\n');
        kzsErrorForward(result);
    }

    result = kzcOutputStreamDelete(outputStream);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcFileReadTextFile(const struct KzcMemoryManager* memoryManager, kzString filePath, kzMutableString* out_text)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    kzMutableString text;

    result = kzcInputStreamCreateFromFile(memoryManager, filePath, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &inputStream);
    kzsErrorForward(result);

    result = kzcInputStreamReadText(inputStream, memoryManager, &text);
    kzsErrorForward(result);

    result = kzcInputStreamDelete(inputStream);
    kzsErrorForward(result);

    *out_text = text;
    kzsSuccess();
}

kzsError kzcFileReadTextFileLines(const struct KzcMemoryManager* memoryManager, kzString filePath, kzMutableString** out_lines)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    kzMutableString* lines;

    result = kzcInputStreamCreateFromFile(memoryManager, filePath, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &inputStream);
    kzsErrorForward(result);

    result = kzcInputStreamReadTextLines(inputStream, memoryManager, &lines);
    kzsErrorForward(result);

    result = kzcInputStreamDelete(inputStream);
    kzsErrorForward(result);

    *out_lines = lines;
    kzsSuccess();
}

kzsError kzcFileReadTextResource(const struct KzcMemoryManager* memoryManager, kzString resourcePath, kzMutableString* out_text)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    kzMutableString text;

    result = kzcInputStreamCreateFromResource(memoryManager, resourcePath, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &inputStream);
    kzsErrorForward(result);

    result = kzcInputStreamReadText(inputStream, memoryManager, &text);
    kzsErrorForward(result);

    result = kzcInputStreamDelete(inputStream);
    kzsErrorForward(result);

    *out_text = text;
    kzsSuccess();
}

kzsError kzcFileReadTextResourceLines(const struct KzcMemoryManager* memoryManager, kzString resourcePath, kzMutableString** out_lines)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    kzMutableString* lines;

    result = kzcInputStreamCreateFromResource(memoryManager, resourcePath, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &inputStream);
    kzsErrorForward(result);

    result = kzcInputStreamReadTextLines(inputStream, memoryManager, &lines);
    kzsErrorForward(result);

    result = kzcInputStreamDelete(inputStream);
    kzsErrorForward(result);

    *out_lines = lines;
    kzsSuccess();
}

kzBool kzcFileExists(kzString filePath)
{
    kzBool fileExists;
    kzString resourceDirectory = kzsFileBaseGetResourceDirectory();
    kzsFile* resourceFile;

    if (resourceDirectory != KZ_NULL)
    {
        kzUint resourceDirectoryLength = kzsStrlen(resourceDirectory);
        kzUint resourcePathLength = kzsStrlen(filePath);
        kzUint fullPathLength = resourceDirectoryLength + 1 + resourcePathLength + 1;
        kzMutableString fullPath = (kzMutableString)kzsMalloc(fullPathLength);
        kzsStrcpy(fullPath, resourceDirectory);
        kzsStrcat(fullPath, "/");
        kzsStrcat(fullPath, filePath);
        resourceFile = kzsFopen(fullPath, KZS_FILE_READONLY_BINARY);
        kzsFree(fullPath);
    }
    else
    {
        resourceFile = kzsFopen(filePath, KZS_FILE_READONLY_BINARY);
    }

    if (resourceFile == KZ_NULL)
    {
        fileExists = KZ_FALSE;
    }
    else
    {
        fileExists = KZ_TRUE;
        KZ_UNUSED_RETURN_VALUE(kzsFclose(resourceFile)); /* Ignore possible errors. */
    }

    return fileExists;
}
