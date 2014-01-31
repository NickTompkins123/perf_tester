/**
* \file
* Output stream
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_output_stream.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/util/string/kzc_string.h>
#include <core/kzc_error_codes.h>

#include <system/wrappers/kzs_string.h>
#include <system/file/kzs_file_base.h>
#include <system/file/kzs_file.h>
#include <system/file/kzs_resource_file.h>
#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_system.h>
#include <system/kzs_error_codes.h>
#include <system/debug/kzs_log.h>


/**
 * Structure for tracking output stream to a memory buffer.
 */
struct KzcMemoryOutput
{
    kzByte* buffer; /**< Memory buffer. */
    kzUint size; /**< Size of the buffer. */
};

/**
 * Structure for tracking output stream to another output stream.
 */
struct KzcStreamOutput
{
    struct KzcOutputStream* outputStream; /**< Target stream. */
    kzInt length; /**< Length of the target stream. */
};

/** Structure for custom output stream. */
struct KzcCustomOutput
{
    void* customData; /**< Arbitrary data. */
    KzcCustomOutputStreamDeleteFunction deleteFunction; /**< Delete function. */
    KzcCustomOutputStreamWriteFunction writeFunction; /**< Write function. */
    KzcCustomOutputStreamSkipFunction skipFunction; /**< Skip function. */
};

enum KzcOutputStreamType
{
    KZC_OUTPUT_STREAM_TYPE_FILE_SYSTEM,
    KZC_OUTPUT_STREAM_TYPE_MEMORY,
    KZC_OUTPUT_STREAM_TYPE_STREAM,
    KZC_OUTPUT_STREAM_TYPE_CUSTOM
};

struct KzcOutputStream
{
    enum KzcOutputStreamType type; /**< Type of the stream target. */
    enum KzcIOStreamEndianness endianness; /**< Endianness of the stream. */
    union
    {
        kzsFile* file;
        struct KzcMemoryOutput memory;
        struct KzcStreamOutput streamOutput;
        struct KzcCustomOutput customOutput;
    } target; /**< Target of the stream. */
    kzUint position; /**< Current position in the stream. */
};


static kzsError kzcOutputStreamCreate_internal(const struct KzcMemoryManager* memoryManager, enum KzcIOStreamEndianness endianness, struct KzcOutputStream** out_outputStream)
{
    kzsError result;
    struct KzcOutputStream* outputStream;

    result = kzcMemoryAllocVariable(memoryManager, outputStream, "File output stream");
    kzsErrorForward(result);

    outputStream->endianness = endianness;
    outputStream->position = 0;

    *out_outputStream = outputStream;
    kzsSuccess();
}

kzsError kzcOutputStreamCreateToFile(const struct KzcMemoryManager* memoryManager, kzString filePath,
                                     enum KzcIOStreamEndianness endianness, struct KzcOutputStream** out_outputStream)
{
    kzsError result;
    struct KzcOutputStream* outputStream;
    kzsFile* file;
    kzString resourceDirectory = kzsFileBaseGetResourceDirectory();

    result = kzcOutputStreamCreate_internal(memoryManager, endianness, &outputStream);
    kzsErrorForward(result);

    if (resourceDirectory != KZ_NULL)
    {
        kzUint resourceDirectoryLength = kzsStrlen(resourceDirectory);
        kzUint resourcePathLength = kzsStrlen(filePath);
        kzUint fullPathLength = resourceDirectoryLength + 1 + resourcePathLength + 1;
        kzMutableString fullPath = (kzMutableString)kzsMalloc(fullPathLength);
        kzsStrcpy(fullPath, resourceDirectory);
        kzsStrcat(fullPath, "/");
        kzsStrcat(fullPath, filePath);
        file = kzsFopen(fullPath, KZS_FILE_WRITEONLY_BINARY);
        kzsFree(fullPath);
    }
    else
    {
        file = kzsFopen(filePath, KZS_FILE_WRITEONLY_BINARY);
    }

    if(!file) kzsLog(KZS_LOG_LEVEL_ERROR, filePath);
    kzsErrorTest(file != KZ_NULL, KZS_ERROR_FILE_OPEN_FAILED, "Failed to open file");

    outputStream->type = KZC_OUTPUT_STREAM_TYPE_FILE_SYSTEM;
    outputStream->target.file = file;

    *out_outputStream = outputStream;
    kzsSuccess();
}

kzsError kzcOutputStreamCreateToMemory(const struct KzcMemoryManager* memoryManager, kzByte* buffer, kzUint size,
                                       enum KzcIOStreamEndianness endianness, struct KzcOutputStream** out_outputStream)
{
    kzsError result;
    struct KzcOutputStream* outputStream;
    struct KzcMemoryOutput memory;

    result = kzcOutputStreamCreate_internal(memoryManager, endianness, &outputStream);
    kzsErrorForward(result);

    memory.buffer = buffer;
    memory.size = size;

    outputStream->type = KZC_OUTPUT_STREAM_TYPE_MEMORY;
    outputStream->target.memory = memory;

    *out_outputStream = outputStream;
    kzsSuccess();
}

kzsError kzcOutputStreamCreateToStream(const struct KzcMemoryManager* memoryManager, struct KzcOutputStream* targetOutputStream, kzInt targetLength,
                                       enum KzcIOStreamEndianness endianness, struct KzcOutputStream** out_outputStream)
{
    kzsError result;
    struct KzcOutputStream* outputStream;
    struct KzcStreamOutput streamOutput;

    result = kzcOutputStreamCreate_internal(memoryManager, endianness, &outputStream);
    kzsErrorForward(result);

    streamOutput.outputStream = targetOutputStream;
    streamOutput.length = targetLength;

    outputStream->type = KZC_OUTPUT_STREAM_TYPE_STREAM;
    outputStream->target.streamOutput = streamOutput;

    *out_outputStream = outputStream;
    kzsSuccess();
}

kzsError kzcOutputStreamCreateCustom(const struct KzcMemoryManager* memoryManager, void* customData,
                                     KzcCustomOutputStreamDeleteFunction deleteFunction, KzcCustomOutputStreamWriteFunction writeFunction,
                                     KzcCustomOutputStreamSkipFunction skipFunction, enum KzcIOStreamEndianness endianness,
                                     struct KzcOutputStream** out_outputStream)
{
    kzsError result;
    struct KzcOutputStream* outputStream;
    struct KzcCustomOutput customOutput;

    result = kzcOutputStreamCreate_internal(memoryManager, endianness, &outputStream);
    kzsErrorForward(result);

    customOutput.customData = customData;
    customOutput.deleteFunction = deleteFunction;
    customOutput.writeFunction = writeFunction;
    customOutput.skipFunction = skipFunction;

    outputStream->type = KZC_OUTPUT_STREAM_TYPE_CUSTOM;
    outputStream->target.customOutput = customOutput;

    *out_outputStream = outputStream;
    kzsSuccess();
}

kzsError kzcOutputStreamDelete(struct KzcOutputStream* outputStream)
{
    kzsError result;

    switch (outputStream->type)
    {
        case KZC_OUTPUT_STREAM_TYPE_FILE_SYSTEM:
        {
            kzInt closeResult = kzsFclose(outputStream->target.file);
            kzsErrorTest(closeResult == 0, KZS_ERROR_FILE_OPERATION_FAILED, "Failed to close file");
            break;
        }

        case KZC_OUTPUT_STREAM_TYPE_MEMORY:
        {
            /* Nothing to delete. */
            break;
        }

        case KZC_OUTPUT_STREAM_TYPE_STREAM:
        {
            struct KzcStreamOutput* streamOutput = &outputStream->target.streamOutput;
            if (streamOutput->length != KZC_IO_STREAM_LENGTH_UNKNOWN)
            {
                kzsAssert((kzUint)streamOutput->length >= outputStream->position);
                result = kzcOutputStreamSkip(streamOutput->outputStream, (kzUint)streamOutput->length - outputStream->position);
                kzsErrorForward(result);
            }
            break;
        }

        case KZC_OUTPUT_STREAM_TYPE_CUSTOM:
        {
            struct KzcCustomOutput* customOutput = &outputStream->target.customOutput;
            result = customOutput->deleteFunction(customOutput->customData);
            kzsErrorForward(result);
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid output stream type");
        }
    }

    result = kzcMemoryFreeVariable(outputStream);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcOutputStreamWrite(struct KzcOutputStream* outputStream, kzUint byteCount, const kzByte* bytes, kzUint* out_byteWriteCount)
{
    kzsError result;
    kzUint writeByteCount;

    if (byteCount == 0)
    {
        writeByteCount = 0;
    }
    else
    {
        switch (outputStream->type)
        {
            case KZC_OUTPUT_STREAM_TYPE_FILE_SYSTEM:
            {
                writeByteCount = kzsFwrite(bytes, sizeof(kzByte), byteCount, outputStream->target.file);
                kzsErrorTest(kzsFerror(outputStream->target.file) == 0, KZS_ERROR_FILE_OPERATION_FAILED, "Error while writing to file");
                break;
            }

            case KZC_OUTPUT_STREAM_TYPE_MEMORY:
            {
                struct KzcMemoryOutput* memory = &outputStream->target.memory;
                for (writeByteCount = 0; writeByteCount < byteCount && outputStream->position + writeByteCount < memory->size; ++writeByteCount)
                {
                    memory->buffer[outputStream->position + writeByteCount] = bytes[writeByteCount];
                }
                break;
            }

            case KZC_OUTPUT_STREAM_TYPE_STREAM:
            {
                struct KzcStreamOutput* streamOutput = &outputStream->target.streamOutput;
                kzUint bytesToWriteCount = byteCount;
                if (streamOutput->length != KZC_IO_STREAM_LENGTH_UNKNOWN)
                {
                    kzsAssert(outputStream->position <= (kzUint)streamOutput->length);
                    bytesToWriteCount = kzsMinU(byteCount, (kzUint)streamOutput->length - outputStream->position);
                }

                if (bytesToWriteCount == 0)
                {
                    writeByteCount = 0;
                }
                else
                {
                    result = kzcOutputStreamWrite(streamOutput->outputStream, bytesToWriteCount, bytes, &writeByteCount);
                    kzsErrorForward(result);
                }
                break;
            }

            case KZC_OUTPUT_STREAM_TYPE_CUSTOM:
            {
                struct KzcCustomOutput* customOutput = &outputStream->target.customOutput;
                result = customOutput->writeFunction(customOutput->customData, byteCount, bytes, &writeByteCount);
                kzsErrorForward(result);
                break;
            }

            default:
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid ouput stream type");
            }
        }

        outputStream->position += writeByteCount;
    }

    *out_byteWriteCount = writeByteCount;
    kzsSuccess();
}

kzsException kzcOutputStreamSkip(struct KzcOutputStream* outputStream, kzUint skipAmount)
{
    kzsError result;
    kzUint byteSkipCount = 0;

    while (byteSkipCount < skipAmount)
    {
        kzUint newByteSkipCount;

        switch (outputStream->type)
        {
            case KZC_OUTPUT_STREAM_TYPE_FILE_SYSTEM:
            {
                kzInt seekResult = kzsFseek(outputStream->target.file, skipAmount, KZS_FILE_ORIGIN_SEEK_CUR);
                kzsErrorTest(seekResult == 0, KZS_ERROR_FILE_OPERATION_FAILED, "Error while skipping in output stream");
                newByteSkipCount = skipAmount;
                break;
            }

            case KZC_OUTPUT_STREAM_TYPE_MEMORY:
            {
                struct KzcMemoryOutput* memory = &outputStream->target.memory;
                kzsAssert(outputStream->position <= memory->size);
                newByteSkipCount = kzsMinU(skipAmount - byteSkipCount, memory->size - outputStream->position);
                break;
            }

            case KZC_OUTPUT_STREAM_TYPE_STREAM:
            {
                struct KzcStreamOutput* streamOutput = &outputStream->target.streamOutput;
                newByteSkipCount = skipAmount;
                if (streamOutput->length != KZC_IO_STREAM_LENGTH_UNKNOWN)
                {
                    kzsAssert(outputStream->position <= (kzUint)streamOutput->length);
                    newByteSkipCount = kzsMinU(skipAmount, (kzUint)streamOutput->length - outputStream->position);
                }
                break;
            }

            case KZC_OUTPUT_STREAM_TYPE_CUSTOM:
            {
                struct KzcCustomOutput* customOutput = &outputStream->target.customOutput;
                result = customOutput->skipFunction(customOutput->customData, skipAmount);
                newByteSkipCount = skipAmount;
                kzsErrorForward(result);
                break;
            }

            default:
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid output stream type");
            }
        }

        kzsExceptionTest(newByteSkipCount > 0, KZC_EXCEPTION_END_OF_STREAM, "End of stream while skipping in output stream");
        byteSkipCount += newByteSkipCount;
    }

    outputStream->position += byteSkipCount;

    kzsSuccess();
}

kzUint kzcOutputStreamGetPosition(const struct KzcOutputStream* outputStream)
{
    return outputStream->position;
}

enum KzcIOStreamEndianness kzcOutputStreamGetEndianness(const struct KzcOutputStream* outputStream)
{
    kzsAssert(kzcIsValidPointer(outputStream));
    return outputStream->endianness;
}

void kzcOutputStreamSetEndianness(struct KzcOutputStream* outputStream, enum KzcIOStreamEndianness endianness)
{
    kzsAssert(kzcIsValidPointer(outputStream));
    outputStream->endianness = endianness;
}

kzsException kzcOutputStreamFlush(const struct KzcOutputStream* outputStream)
{
    kzsException result;

    kzsAssert(kzcIsValidPointer(outputStream));

    switch (outputStream->type)
    {
        case KZC_OUTPUT_STREAM_TYPE_FILE_SYSTEM:
        {
            kzInt seekResult = kzsFflush(outputStream->target.file);
            kzsErrorTest(seekResult == 0, KZS_ERROR_FILE_OPERATION_FAILED, "Error while flushing output stream");
            break;
        }

        case KZC_OUTPUT_STREAM_TYPE_STREAM:
        {
            result = kzcOutputStreamFlush(outputStream->target.streamOutput.outputStream);
            kzsErrorForward(result);
            break;
        }

        case KZC_OUTPUT_STREAM_TYPE_MEMORY:
        case KZC_OUTPUT_STREAM_TYPE_CUSTOM:
        {
            /* Nothing to flush. */
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid output stream type");
        }
    }
    kzsSuccess();
}

kzsException kzcOutputStreamWriteBytes(struct KzcOutputStream* outputStream, kzUint byteCount, const kzByte* bytes)
{
    kzsException result;
    kzUint byteWriteCount = 0;

    while (byteWriteCount < byteCount)
    {
        kzUint tempBytesWritten;

        result = kzcOutputStreamWrite(outputStream, byteCount - byteWriteCount, bytes + byteWriteCount, &tempBytesWritten);
        kzsErrorForward(result);

        kzsExceptionTest(tempBytesWritten > 0, KZC_EXCEPTION_END_OF_STREAM, "End of file while writing to output stream");

        byteWriteCount += tempBytesWritten;
    }

    kzsAssert(byteWriteCount == byteCount);

    kzsSuccess();
}

kzsException kzcOutputStreamWriteBoolean(struct KzcOutputStream* outputStream, kzBool value)
{
    kzsException result;
    kzByte buffer[1];

    buffer[0] = (kzByte)(value ? 1 : 0);

    result = kzcOutputStreamWriteBytes(outputStream, 1, buffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsException kzcOutputStreamWriteU8(struct KzcOutputStream* outputStream, kzU8 value)
{
    kzsException result;
    kzByte buffer[1];
    buffer[0] = (kzByte)value;

    result = kzcOutputStreamWriteBytes(outputStream, 1, buffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsException kzcOutputStreamWriteS8(struct KzcOutputStream* outputStream, kzS8 value)
{
    kzsException result;
    kzByte buffer[1];
    buffer[0] = (kzByte)value;

    result = kzcOutputStreamWriteBytes(outputStream, 1, buffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsException kzcOutputStreamWriteU16(struct KzcOutputStream* outputStream, kzU16 value)
{
    kzsException result;
    kzByte buffer[2];

    switch (outputStream->endianness)
    {
        case KZC_IO_STREAM_ENDIANNESS_LITTLE_ENDIAN:
        {
            buffer[0] = (kzByte)value;
            buffer[1] = (kzByte)(value >> 8);
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN:
        {
            buffer[0] = (kzByte)(value >> 8);
            buffer[1] = (kzByte)value;
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_PLATFORM:
        {
            kzByte* valueBuffer = (kzByte*)&value;
            buffer[0] = valueBuffer[0];
            buffer[1] = valueBuffer[1];
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED:
        {
            kzsErrorThrow(KZC_ERROR_UNSPECIFIED_ENDIANNESS, "Output stream endianness was not specified");
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid output stream type");
        }
    }

    result = kzcOutputStreamWriteBytes(outputStream, 2, buffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsException kzcOutputStreamWriteU16Array(struct KzcOutputStream* outputStream, kzUint numValues, const kzU16* values)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(outputStream));

    if (outputStream->endianness == KZC_IO_STREAM_ENDIANNESS_PLATFORM)
    {
        kzByte* buffer = (kzByte*)values;

        result = kzcOutputStreamWriteBytes(outputStream, numValues * sizeof(kzU16), buffer);
        kzsErrorForward(result);
    }
    else
    {
        kzUint i;
        for (i = 0; i < numValues; ++i)
        {
            result = kzcOutputStreamWriteU16(outputStream, values[i]);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsException kzcOutputStreamWriteS16(struct KzcOutputStream* outputStream, kzS16 value)
{
    kzsException result;

    result = kzcOutputStreamWriteU16(outputStream, (kzU16)value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsException kzcOutputStreamWriteS16Array(struct KzcOutputStream* outputStream, kzUint numValues, const kzS16* values)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(outputStream));

    if (outputStream->endianness == KZC_IO_STREAM_ENDIANNESS_PLATFORM)
    {
        kzByte* buffer = (kzByte*)values;

        result = kzcOutputStreamWriteBytes(outputStream, numValues * sizeof(kzS16), buffer);
        kzsErrorForward(result);
    }
    else
    {
        kzUint i;
        for (i = 0; i < numValues; ++i)
        {
            result = kzcOutputStreamWriteS16(outputStream, values[i]);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsException kzcOutputStreamWriteU32(struct KzcOutputStream* outputStream, kzU32 value)
{
    kzsException result;
    kzByte buffer[4];

    switch (outputStream->endianness)
    {
        case KZC_IO_STREAM_ENDIANNESS_LITTLE_ENDIAN:
        {
            buffer[0] = (kzByte)value;
            buffer[1] = (kzByte)(value >> 8);
            buffer[2] = (kzByte)(value >> 16);
            buffer[3] = (kzByte)(value >> 24);
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN:
        {
            buffer[0] = (kzByte)(value >> 24);
            buffer[1] = (kzByte)(value >> 16);
            buffer[2] = (kzByte)(value >> 8);
            buffer[3] = (kzByte)value;
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_PLATFORM:
        {
            kzByte* valueBuffer = (kzByte*)&value;
            buffer[0] = valueBuffer[0];
            buffer[1] = valueBuffer[1];
            buffer[2] = valueBuffer[2];
            buffer[3] = valueBuffer[3];
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED:
        {
            kzsErrorThrow(KZC_ERROR_UNSPECIFIED_ENDIANNESS, "Output stream endianness was not specified");
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid output stream type");
        }
    }

    result = kzcOutputStreamWriteBytes(outputStream, 4, buffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsException kzcOutputStreamWriteU32Array(struct KzcOutputStream* outputStream, kzUint numValues, const kzU32* values)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(outputStream));

    if (outputStream->endianness == KZC_IO_STREAM_ENDIANNESS_PLATFORM)
    {
        kzByte* buffer = (kzByte*)values;

        result = kzcOutputStreamWriteBytes(outputStream, numValues * sizeof(kzU32), buffer);
        kzsErrorForward(result);
    }
    else
    {
        kzUint i;
        for (i = 0; i < numValues; ++i)
        {
            result = kzcOutputStreamWriteU32(outputStream, values[i]);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsException kzcOutputStreamWriteS32(struct KzcOutputStream* outputStream, kzS32 value)
{
    kzsException result;

    result = kzcOutputStreamWriteU32(outputStream, (kzU32)value);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsException kzcOutputStreamWriteS32Array(struct KzcOutputStream* outputStream, kzUint numValues, const kzS32* values)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(outputStream));

    if (outputStream->endianness == KZC_IO_STREAM_ENDIANNESS_PLATFORM)
    {
        kzByte* buffer = (kzByte*)values;

        result = kzcOutputStreamWriteBytes(outputStream, numValues * sizeof(kzS32), buffer);
        kzsErrorForward(result);
    }
    else
    {
        kzUint i;
        for (i = 0; i < numValues; ++i)
        {
            result = kzcOutputStreamWriteS32(outputStream, values[i]);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsException kzcOutputStreamWriteFloat(struct KzcOutputStream* outputStream, kzFloat value)
{
    kzsException result;
    kzByte buffer[sizeof(kzFloat)];

    switch (outputStream->endianness)
    {
        case KZC_IO_STREAM_ENDIANNESS_LITTLE_ENDIAN:
        {
            kzsFloatToBytesLittleEndian(value, buffer);
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN:
        {
            kzsFloatToBytesBigEndian(value, buffer);
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_PLATFORM:
        {
            kzByte* valueBuffer = (kzByte*)&value;
            buffer[0] = valueBuffer[0];
            buffer[1] = valueBuffer[1];
            buffer[2] = valueBuffer[2];
            buffer[3] = valueBuffer[3];
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED:
        {
            kzsErrorThrow(KZC_ERROR_UNSPECIFIED_ENDIANNESS, "Output stream endianness was not specified");
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid output stream type");
        }
    }

    result = kzcOutputStreamWriteBytes(outputStream, sizeof(kzFloat), buffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsException kzcOutputStreamWriteFloatArray(struct KzcOutputStream* outputStream, kzUint numValues, const kzFloat* values)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(outputStream));

    if (outputStream->endianness == KZC_IO_STREAM_ENDIANNESS_PLATFORM)
    {
        kzByte* buffer = (kzByte*)values;

        result = kzcOutputStreamWriteBytes(outputStream, numValues * sizeof(kzFloat), buffer);
        kzsErrorForward(result);
    }
    else
    {
        kzUint i;
        for (i = 0; i < numValues; ++i)
        {
            result = kzcOutputStreamWriteFloat(outputStream, values[i]);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsException kzcOutputStreamWriteDouble(struct KzcOutputStream* outputStream, kzDouble value)
{
    KZS_NOT_IMPLEMENTED_YET_ERROR;
}
