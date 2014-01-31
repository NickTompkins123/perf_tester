/**
* \file
* Input stream
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_input_stream.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/kzc_error_codes.h>

#include <system/wrappers/kzs_string.h>
#include <system/file/kzs_file_base.h>
#include <system/file/kzs_file.h>
#include <system/file/kzs_resource_file.h>
#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_memory.h>
#include <system/debug/kzs_counter.h>
#include <system/kzs_system.h>
#include <system/kzs_error_codes.h>

/** Structure for tracking input stream to a memory buffer. */
struct KzcMemoryInput
{
    const kzByte* buffer; /**< Memory buffer. */
    kzUint size; /**< Size of the buffer. */
};

/** Structure for tracking input stream to another input stream. */
struct KzcStreamInput
{
    struct KzcInputStream* inputStream; /**< Target stream. */
    kzInt length; /**< Length of the target stream. */
};

/** Structure for custom input stream. */
struct KzcCustomInput
{
    void* customData; /**< Arbitrary data. */
    KzcCustomInputStreamDeleteFunction deleteFunction; /**< Delete function. */
    KzcCustomInputStreamReadFunction readFunction; /**< Read function. */
    KzcCustomInputStreamSkipFunction skipFunction; /**< Skip function. */
};

struct KzcInputStream
{
    enum KzcInputStreamType {KZC_INPUT_STREAM_TYPE_FILE_SYSTEM,
                             KZC_INPUT_STREAM_TYPE_RESOURCE,
                             KZC_INPUT_STREAM_TYPE_MEMORY,
                             KZC_INPUT_STREAM_TYPE_STREAM,
                             KZC_INPUT_STREAM_TYPE_CUSTOM} type; /**< Type of the stream target. */
    enum KzcIOStreamEndianness endianness; /**< Endianness of the stream. */
    union
    {
        kzsFile* file;
        struct KzsResourceFile* resourceFile;
        struct KzcMemoryInput memory;
        struct KzcStreamInput streamInput;
        struct KzcCustomInput customInput;
    } target; /**< Target of the stream. */
    kzUint position; /**< Current position in the stream. */

    kzBool buffered; /**< Flag for enabling the input stream buffering. */
    kzUint bufferSize; /**< Size of the input stream buffer. */
    kzUint actualBufferSize; /**< The actual amount of data read from stream to buffer. */
    kzUint bufferReadPosition; /**< Current position in the buffer. */
    kzByte* inputBuffer; /**< Input stream buffer. */
};

/**
 * Creates an input stream with the given endianness.
 * The stream must be linked to a target after calling this method.
 */
static kzsError kzcInputStreamCreate_internal(const struct KzcMemoryManager* memoryManager, enum KzcIOStreamEndianness endianness,
                                              struct KzcInputStream** out_inputStream)
{
    kzsError result;
    struct KzcInputStream* inputStream;

    result = kzcMemoryAllocVariable(memoryManager, inputStream, "Input stream");
    kzsErrorForward(result);

    inputStream->endianness = endianness;
    inputStream->position = 0;

    inputStream->buffered = KZ_FALSE;
    inputStream->bufferSize = 0;
    inputStream->actualBufferSize = 0;
    inputStream->bufferReadPosition = 0;
    inputStream->inputBuffer = KZ_NULL;

    *out_inputStream = inputStream;
    kzsSuccess();
}

kzsError kzcInputStreamCreateFromFile(const struct KzcMemoryManager* memoryManager, kzString filePath,
                                      enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream)
{
    kzsError result;
    kzMutableString errorString;
    struct KzcInputStream* inputStream;
    kzsFile* file;
    kzString resourceDirectory = kzsFileBaseGetResourceDirectory();

    result = kzcInputStreamCreate_internal(memoryManager, endianness, &inputStream);
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
        file = kzsFopen(fullPath, KZS_FILE_READONLY_BINARY);
        kzsFree(fullPath);
    }
    else
    {
        file = kzsFopen(filePath, KZS_FILE_READONLY_BINARY);
    }

    result = kzcStringFormat(memoryManager, "Failed to open file %s!", &errorString, filePath);
    kzsErrorForward(result);
    kzsErrorTest(file != KZ_NULL, KZS_ERROR_FILE_OPEN_FAILED, errorString);
    result = kzcStringDelete(errorString);
    kzsErrorForward(result);
    
    inputStream->type = KZC_INPUT_STREAM_TYPE_FILE_SYSTEM;
    inputStream->target.file = file;

    *out_inputStream = inputStream;
    kzsSuccess();
}

kzsError kzcInputStreamCreateFromResource(const struct KzcMemoryManager* memoryManager, kzString resourcePath,
                                          enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    struct KzsResourceFile* resourceFile;

    result = kzcInputStreamCreate_internal(memoryManager, endianness, &inputStream);
    kzsErrorForward(result);

    result = kzsResourceFileCreate(resourcePath, &resourceFile);
    kzsErrorForward(result);

    inputStream->type = KZC_INPUT_STREAM_TYPE_RESOURCE;
    inputStream->target.resourceFile = resourceFile;

    *out_inputStream = inputStream;
    kzsSuccess();
}

kzsError kzcInputStreamCreateFromMemory(const struct KzcMemoryManager* memoryManager, const kzByte* buffer, kzUint size,
                                        enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    struct KzcMemoryInput memory;

    result = kzcInputStreamCreate_internal(memoryManager, endianness, &inputStream);
    kzsErrorForward(result);

    memory.buffer = buffer;
    memory.size = size;

    inputStream->type = KZC_INPUT_STREAM_TYPE_MEMORY;
    inputStream->target.memory = memory;

    *out_inputStream = inputStream;
    kzsSuccess();
}

kzsError kzcInputStreamCreateFromStream(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* targetInputStream,
                                        kzInt targetLength, enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    struct KzcStreamInput streamInput;

    result = kzcInputStreamCreate_internal(memoryManager, endianness, &inputStream);
    kzsErrorForward(result);

    streamInput.inputStream = targetInputStream;
    streamInput.length = targetLength;

    inputStream->type = KZC_INPUT_STREAM_TYPE_STREAM;
    inputStream->target.streamInput = streamInput;

    *out_inputStream = inputStream;
    kzsSuccess();
}

kzsError kzcInputStreamCreateCustom(const struct KzcMemoryManager* memoryManager, void* customData,
                                    KzcCustomInputStreamDeleteFunction deleteFunction, KzcCustomInputStreamReadFunction readFunction,
                                    KzcCustomInputStreamSkipFunction skipFunction, enum KzcIOStreamEndianness endianness,
                                    struct KzcInputStream** out_inputStream)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    struct KzcCustomInput customInput;

    result = kzcInputStreamCreate_internal(memoryManager, endianness, &inputStream);
    kzsErrorForward(result);

    customInput.customData = customData;
    customInput.deleteFunction = deleteFunction;
    customInput.readFunction = readFunction;
    customInput.skipFunction = skipFunction;

    inputStream->type = KZC_INPUT_STREAM_TYPE_CUSTOM;
    inputStream->target.customInput = customInput;

    *out_inputStream = inputStream;
    kzsSuccess();
}

kzsError kzcInputStreamDelete(struct KzcInputStream* inputStream)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(inputStream));

    if(inputStream->buffered)
    {
        result = kzcMemoryFreePointer(inputStream->inputBuffer);
        kzsErrorForward(result);
    }

    switch (inputStream->type)
    {
        case KZC_INPUT_STREAM_TYPE_FILE_SYSTEM:
        {
            kzInt closeResult = kzsFclose(inputStream->target.file);
            kzsErrorTest(closeResult == 0, KZS_ERROR_FILE_OPERATION_FAILED, "Failed to close file");
            break;
        }

        case KZC_INPUT_STREAM_TYPE_RESOURCE:
        {
            result = kzsResourceFileDelete(inputStream->target.resourceFile);
            kzsExceptionForward(result);
            break;
        }

        case KZC_INPUT_STREAM_TYPE_MEMORY:
        {
            /* Nothing to delete. */
            break;
        }

        case KZC_INPUT_STREAM_TYPE_STREAM:
        {
            struct KzcStreamInput* streamInput = &inputStream->target.streamInput;
            if (streamInput->length != KZC_IO_STREAM_LENGTH_UNKNOWN)
            {
                kzsAssert((kzUint)streamInput->length >= inputStream->position);
                result = kzcInputStreamSkip(streamInput->inputStream, (kzUint)streamInput->length - inputStream->position);
                kzsErrorForward(result);
            }
            break;
        }

        case KZC_INPUT_STREAM_TYPE_CUSTOM:
        {
            struct KzcCustomInput* customInput = &inputStream->target.customInput;
            result = customInput->deleteFunction(customInput->customData);
            kzsErrorForward(result);
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid input stream type");
        }
    }

    result = kzcMemoryFreeVariable(inputStream);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcInputStreamInitBuffer(struct KzcInputStream* inputStream, kzUint bufferSize)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(inputStream));
    kzsErrorTest(bufferSize > 0, KZS_ERROR_ILLEGAL_ARGUMENT, "Input stream buffer size is zero.");

    inputStream->buffered = KZ_TRUE;
    inputStream->bufferSize = bufferSize;
    inputStream->actualBufferSize = bufferSize;

    /* The buffer is refilled in the next read when the read position is in the end of the buffer. */
    inputStream->bufferReadPosition = inputStream->bufferSize;

    result = kzcMemoryAllocPointer(kzcMemoryGetManager(inputStream), &inputStream->inputBuffer, inputStream->bufferSize, "Input Stream Buffer");
    kzsErrorForward(result);

    kzsSuccess();
}

/**
 * Reads some amount of bytes from the given input stream.
 * The maximum number of bytes is given as parameter and the actual number of bytes read is returned.
 * Negative number is returned, if no bytes can be read because end of stream has been reached.
 * This function can be used to read input stream where the number of expected bytes available is not known.
 */
static kzsError kzcInputStreamRead_internal(struct KzcInputStream* inputStream, kzUint byteCount,
                                                    kzUint* out_bytesReadCount, kzByte* out_bytes)
{
    kzsError result;
    kzUint readByteCount = 0;

    kzsAssert(kzcIsValidPointer(inputStream));

    switch (inputStream->type)
    {
        case KZC_INPUT_STREAM_TYPE_FILE_SYSTEM:
        {
            readByteCount = kzsFread(out_bytes, sizeof(kzByte), byteCount, inputStream->target.file);
            kzsErrorTest(kzsFerror(inputStream->target.file) == 0, KZS_ERROR_FILE_OPERATION_FAILED, "Error while reading from file");
            kzsCounterIncrease("kzcInputStreamReadFileSystem");
            break;
        }

        case KZC_INPUT_STREAM_TYPE_RESOURCE:
        {
            kzInt resourceBytesReadCount = kzsResourceFileRead(inputStream->target.resourceFile, byteCount, out_bytes);
            kzsErrorTest(resourceBytesReadCount >= 0, KZS_ERROR_FILE_OPERATION_FAILED, "Error while reading from resource");
            readByteCount = (kzUint)resourceBytesReadCount;
            kzsCounterIncrease("kzcInputStreamReadResource");
            break;
        }

        case KZC_INPUT_STREAM_TYPE_MEMORY:
        {
            struct KzcMemoryInput* memory = &inputStream->target.memory;
            kzUint bytesToRead = kzsMinU(byteCount, (memory->size - inputStream->position));
            kzsMemcpy(out_bytes, &memory->buffer[inputStream->position], bytesToRead);
            readByteCount = bytesToRead;
            kzsCounterIncrease("kzcInputStreamReadMemory");
            break;
        }

        case KZC_INPUT_STREAM_TYPE_STREAM:
        {
            struct KzcStreamInput* streamInput = &inputStream->target.streamInput;
            kzUint bytesToReadCount = byteCount;
            if (streamInput->length != KZC_IO_STREAM_LENGTH_UNKNOWN)
            {
                kzsAssert(inputStream->position <= (kzUint)streamInput->length);
                bytesToReadCount = kzsMinU(byteCount, (kzUint)streamInput->length - inputStream->position);
            }

            if (bytesToReadCount == 0)
            {
                readByteCount = 0;
            }
            else
            {
                result = kzcInputStreamRead(streamInput->inputStream, bytesToReadCount, &readByteCount, out_bytes);
                kzsErrorForward(result);
            }
            break;
        }

        case KZC_INPUT_STREAM_TYPE_CUSTOM:
        {
            struct KzcCustomInput* customInput = &inputStream->target.customInput;
            result = customInput->readFunction(customInput->customData, byteCount, &readByteCount, out_bytes);
            kzsErrorForward(result);
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid input stream type");
        }
    }
    *out_bytesReadCount = readByteCount;
    kzsSuccess();
}

kzsError kzcInputStreamRead(struct KzcInputStream* inputStream, kzUint byteCount, kzUint* out_bytesReadCount, kzByte* out_bytes)
{
    kzsError result;
    kzUint readByteCount = 0;

    kzsAssert(kzcIsValidPointer(inputStream));

    if (byteCount > 0)
    {
        if(!inputStream->buffered)
        {
            result = kzcInputStreamRead_internal(inputStream, byteCount, &readByteCount, out_bytes);
            kzsErrorForward(result);
            inputStream->position += readByteCount;
        }
        else
        {
            kzUint readByteCountLeft = byteCount;
            /* Assumption: The input stream buffering is efficient when requested data sizes are small.*/
            /* If requested data size is much bigger than the buffer size => Use "Semi-Buffering": */
            /* First copy the remaining buffer to out_bytes. Initially we have (inputStream->bufferReadPosition >= inputStream->actualBufferSize) so...  */
            if(inputStream->bufferReadPosition < inputStream->actualBufferSize)
            {
                readByteCount = kzsMinU((inputStream->actualBufferSize - inputStream->bufferReadPosition), byteCount);
                kzsMemcpy(out_bytes, &inputStream->inputBuffer[inputStream->bufferReadPosition], readByteCount);
                inputStream->bufferReadPosition += readByteCount;
                inputStream->position += readByteCount;
                readByteCountLeft -= readByteCount;
            }

            /* ...if the requested byte size > actual bufferSize, read without buffering i.e. directly to out_bytes + offset... */
            if(readByteCountLeft >= inputStream->actualBufferSize)
            {
                kzUint readFromStreamByteCount;
                result = kzcInputStreamRead_internal(inputStream, readByteCountLeft, &readFromStreamByteCount, (out_bytes + readByteCount));
                kzsErrorForward(result);
                inputStream->bufferReadPosition = inputStream->bufferSize; /* This resets the buffer. */
                inputStream->position += readFromStreamByteCount;
                readByteCount += readFromStreamByteCount;
            }
            else /* ... or fill the buffer and copy remaining requested bytes with memcpy from the buffer. */
            {
                if(inputStream->bufferReadPosition >= inputStream->actualBufferSize)
                {
                    /* Try to refill the whole buffer, but it might be less that we get from stream. */
                    kzUint readRefillByteCount;
                    result = kzcInputStreamRead_internal(inputStream, inputStream->bufferSize, &readRefillByteCount, inputStream->inputBuffer);
                    kzsErrorForward(result);
                    inputStream->bufferReadPosition = 0;
                    inputStream->actualBufferSize = readRefillByteCount;
                }

                if(readByteCountLeft > 0)
                {
                    kzUint readFromBufferByteCount = kzsMinU((inputStream->actualBufferSize - inputStream->bufferReadPosition), readByteCountLeft);
                    kzsMemcpy((out_bytes + readByteCount), &inputStream->inputBuffer[inputStream->bufferReadPosition], readFromBufferByteCount);
                    inputStream->bufferReadPosition += readFromBufferByteCount;
                    inputStream->position += readFromBufferByteCount;
                    readByteCount += readFromBufferByteCount;
                }
            }
        }
    }

    *out_bytesReadCount = readByteCount;
    kzsSuccess();
}

/** Skips the given amount of bytes in an input stream. */
static kzsError kzcInputStreamSkip_internal(struct KzcInputStream* inputStream, kzUint skipAmount)
{
    kzsException result;
    kzUint byteSkipCount = 0;

    while (byteSkipCount < skipAmount)
    {
        kzUint newByteSkipCount;

        switch (inputStream->type)
        {
            case KZC_INPUT_STREAM_TYPE_FILE_SYSTEM:
            {
                kzInt tempBytesSkipped = kzsFileReadSkip(inputStream->target.file, skipAmount - byteSkipCount);
                kzsErrorTest(tempBytesSkipped >= 0, KZS_ERROR_FILE_OPERATION_FAILED, "Error while skipping in a file");
                newByteSkipCount = (kzUint)tempBytesSkipped;
                break;
            }

            case KZC_INPUT_STREAM_TYPE_RESOURCE:
            {
                kzInt tempBytesSkipped = kzsResourceFileSkip(inputStream->target.resourceFile, skipAmount - byteSkipCount);
                kzsErrorTest(tempBytesSkipped >= 0, KZS_ERROR_FILE_OPERATION_FAILED, "Error while skipping in a resource");
                newByteSkipCount = (kzUint)tempBytesSkipped;
                break;
            }

            case KZC_INPUT_STREAM_TYPE_MEMORY:
            {
                struct KzcMemoryInput* memory = &inputStream->target.memory;
                kzsAssert(inputStream->position <= memory->size);
                newByteSkipCount = kzsMinU(skipAmount - byteSkipCount, memory->size - inputStream->position);
                break;
            }

            case KZC_INPUT_STREAM_TYPE_STREAM:
            {
                struct KzcStreamInput* streamInput = &inputStream->target.streamInput;
                newByteSkipCount = skipAmount;
                if (streamInput->length != KZC_IO_STREAM_LENGTH_UNKNOWN)
                {
                    kzsAssert(inputStream->position <= (kzUint)streamInput->length);
                    newByteSkipCount = kzsMinU(skipAmount, (kzUint)streamInput->length - inputStream->position);
                }
                result = kzcInputStreamSkip(streamInput->inputStream, newByteSkipCount);
                kzsExceptionForward(result);
                break;
            }

            case KZC_INPUT_STREAM_TYPE_CUSTOM:
            {
                struct KzcCustomInput* customInput = &inputStream->target.customInput;
                result = customInput->skipFunction(customInput->customData, skipAmount);
                newByteSkipCount = skipAmount;
                kzsExceptionForward(result);
                break;
            }

            default:
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid input stream type");
            }
        }

        kzsExceptionTest(newByteSkipCount > 0, KZC_EXCEPTION_END_OF_STREAM, "End of stream while skipping in input stream");
        byteSkipCount += newByteSkipCount;
    }
    kzsSuccess();
}

kzsException kzcInputStreamSkip(struct KzcInputStream* inputStream, kzUint skipAmount)
{
    kzsException result;
    kzUint skipAmountLeft = 0;

    kzsAssert(kzcIsValidPointer(inputStream));

    if(!inputStream->buffered)
    {
        result = kzcInputStreamSkip_internal(inputStream, skipAmount);
        kzsExceptionForward(result);
        inputStream->position += skipAmount;
    }
    else
    {
        /* Check if we just need skip inside the buffered data... */
        if((inputStream->actualBufferSize - inputStream->bufferReadPosition) >= skipAmount)
        {
            inputStream->bufferReadPosition += skipAmount;
            inputStream->position += skipAmount;
        }
        else /* ...or we need to skip more data than what is available in the current input stream buffer. */
        {
            /* First "skip" the current data from the input stream buffer...*/
            skipAmountLeft = skipAmount - (inputStream->actualBufferSize - inputStream->bufferReadPosition);
            /*...then skip the remaining skip count and set the input stream buffer empty.*/
            result = kzcInputStreamSkip_internal(inputStream, skipAmountLeft);
            kzsExceptionForward(result);
            inputStream->bufferReadPosition = inputStream->actualBufferSize; /* This resets the buffer. */
            inputStream->position += skipAmountLeft;
        }
    }

    kzsSuccess();
}

kzUint kzcInputStreamGetPosition(const struct KzcInputStream* inputStream)
{
    kzsAssert(kzcIsValidPointer(inputStream));

    return inputStream->position;
}

kzsException kzcInputStreamReadBytes(struct KzcInputStream* inputStream, kzUint byteCount, kzByte* out_bytes)
{
    kzsError result;
    kzUint byteReadCount = 0;

    kzsAssert(kzcIsValidPointer(inputStream));

    while (byteReadCount < byteCount)
    {
        kzUint tempBytesRead;

        result = kzcInputStreamRead(inputStream, byteCount - byteReadCount, &tempBytesRead, out_bytes + byteReadCount);
        kzsErrorForward(result);

        kzsExceptionTest(tempBytesRead > 0, KZC_EXCEPTION_END_OF_STREAM, "End of stream while reading from input stream");

        byteReadCount += tempBytesRead;
    }

    kzsAssert(byteReadCount == byteCount);

    kzsCounterIncrease("kzcInputStreamReadBytes");
    kzsSuccess();
}

kzsException kzcInputStreamReadBoolean(struct KzcInputStream* inputStream, kzBool* out_value)
{
    kzsException result;
    kzByte buffer[1];

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadBytes(inputStream, 1, buffer);
    kzsExceptionForward(result);

    kzsAssert(buffer[0] == 0 || buffer[0] == 1);

    *out_value = (buffer[0] != 0);
    kzsSuccess();
}

kzsException kzcInputStreamReadU8(struct KzcInputStream* inputStream, kzU8* out_value)
{
    kzsException result;
    kzByte buffer[1];

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadBytes(inputStream, 1, buffer);
    kzsExceptionForward(result);

    *out_value = (kzU8)buffer[0];
    kzsSuccess();
}

kzsException kzcInputStreamReadU8Int(struct KzcInputStream* inputStream, kzUint* out_value)
{
    kzsException result;
    kzByte buffer[1];

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadBytes(inputStream, 1, buffer);
    kzsExceptionForward(result);

    *out_value = (kzUint)buffer[0];
    kzsSuccess();
}

kzsException kzcInputStreamReadS8(struct KzcInputStream* inputStream, kzS8* out_value)
{
    kzsException result;
    kzByte buffer[1];

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadBytes(inputStream, 1, buffer);
    kzsExceptionForward(result);

    *out_value = (kzS8)buffer[0];
    kzsSuccess();
}

kzsException kzcInputStreamReadS8Int(struct KzcInputStream* inputStream, kzInt* out_value)
{
    kzsException result;
    kzByte buffer[1];

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadBytes(inputStream, 1, buffer);
    kzsExceptionForward(result);

    *out_value = (kzInt)buffer[0];
    kzsSuccess();
}

kzsException kzcInputStreamReadU16(struct KzcInputStream* inputStream, kzU16* out_value)
{
    kzsException result;
    kzByte buffer[2];
    kzU16 value;

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadBytes(inputStream, 2, buffer);
    kzsExceptionForward(result);

    switch (inputStream->endianness)
    {
        case KZC_IO_STREAM_ENDIANNESS_LITTLE_ENDIAN:
        {
            value = (kzU16)(buffer[0] | (buffer[1] << 8));
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN:
        {
            value = (kzU16)((buffer[0] << 8) | buffer[1]);
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_PLATFORM:
        {
            value = *(kzU16*)buffer;
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED:
        {
            kzsErrorThrow(KZC_ERROR_UNSPECIFIED_ENDIANNESS, "Input stream endianness was not specified");
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid input stream type");
        }
    }

    *out_value = value;
    kzsSuccess();
}

kzsException kzcInputStreamReadU16Int(struct KzcInputStream* inputStream, kzUint* out_value)
{
    kzsException result;
    kzU16 value;

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadU16(inputStream, &value);
    kzsExceptionForward(result);

    *out_value = (kzUint)value;
    kzsSuccess();
}

kzsException kzcInputStreamReadU16Array(struct KzcInputStream* inputStream, kzUint numValues, kzU16* values)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(inputStream));

    if (inputStream->endianness == KZC_IO_STREAM_ENDIANNESS_PLATFORM)
    {
        kzByte* buffer = (kzByte*)values;
        
        result = kzcInputStreamReadBytes(inputStream, numValues * sizeof(kzU16), buffer);
        kzsErrorForward(result);
    }
    else
    {
        kzUint i;
        for (i = 0; i < numValues; ++i)
        {
            result = kzcInputStreamReadU16(inputStream, &values[i]);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsException kzcInputStreamReadS16(struct KzcInputStream* inputStream, kzS16* out_value)
{
    kzsException result;
    kzU16 value;

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadU16(inputStream, &value);
    kzsExceptionForward(result);

    *out_value = (kzS16)value;
    kzsSuccess();
}

kzsException kzcInputStreamReadS16Array(struct KzcInputStream* inputStream, kzUint numValues, kzS16* values)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(inputStream));

    if (inputStream->endianness == KZC_IO_STREAM_ENDIANNESS_PLATFORM)
    {
        kzByte* buffer = (kzByte*)values;

        result = kzcInputStreamReadBytes(inputStream, numValues * sizeof(kzS16), buffer);
        kzsErrorForward(result);
    }
    else
    {
        kzUint i;
        for (i = 0; i < numValues; ++i)
        {
            result = kzcInputStreamReadS16(inputStream, &values[i]);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsException kzcInputStreamReadS16Int(struct KzcInputStream* inputStream, kzInt* out_value)
{
    kzsException result;
    kzU16 value;

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadU16(inputStream, &value);
    kzsExceptionForward(result);

    *out_value = (kzInt)(kzS16)value;
    kzsSuccess();
}

kzsException kzcInputStreamReadU32(struct KzcInputStream* inputStream, kzU32* out_value)
{
    kzsException result;
    kzByte buffer[4];
    kzU32 value;

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadBytes(inputStream, 4, buffer);
    kzsExceptionForward(result);

    switch (inputStream->endianness)
    {
        case KZC_IO_STREAM_ENDIANNESS_LITTLE_ENDIAN:
        {
            /* Additional type cast to suppress lint warning */
            value = (kzU32)(buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | ((kzU32)buffer[3] << 24));
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN:
        {
            /* Additional type cast to suppress lint warning */
            value = (kzU32)(((kzU32)buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3]);
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_PLATFORM:
        {
            value = *(kzU32*)buffer;
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED:
        {
            kzsErrorThrow(KZC_ERROR_UNSPECIFIED_ENDIANNESS, "Input stream endianness was not specified");
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid input stream type");
        }
    }

    kzsCounterIncrease("kzcInputStreamReadU32");

    *out_value = value;
    kzsSuccess();
}

kzsException kzcInputStreamReadU32Array(struct KzcInputStream* inputStream, kzUint numValues, kzU32* values)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(inputStream));

    if (inputStream->endianness == KZC_IO_STREAM_ENDIANNESS_PLATFORM)
    {
        kzByte* buffer = (kzByte*)values;

        result = kzcInputStreamReadBytes(inputStream, numValues * sizeof(kzU32), buffer);
        kzsErrorForward(result);
    }
    else
    {
        kzUint i;
        for (i = 0; i < numValues; ++i)
        {
            result = kzcInputStreamReadU32(inputStream, &values[i]);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsException kzcInputStreamReadU32Int(struct KzcInputStream* inputStream, kzUint* out_value)
{
    kzsException result;
    kzU32 value;

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadU32(inputStream, &value);
    kzsExceptionForward(result);

    *out_value = (kzUint)value;
    kzsSuccess();
}

kzsException kzcInputStreamReadS32(struct KzcInputStream* inputStream, kzS32* out_value)
{
    kzsException result;
    kzU32 value;

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadU32(inputStream, &value);
    kzsExceptionForward(result);

    *out_value = (kzS32)value;
    kzsSuccess();
}

kzsException kzcInputStreamReadS32Array(struct KzcInputStream* inputStream, kzUint numValues, kzS32* values)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(inputStream));

    if (inputStream->endianness == KZC_IO_STREAM_ENDIANNESS_PLATFORM)
    {
        kzByte* buffer = (kzByte*)values;

        result = kzcInputStreamReadBytes(inputStream, numValues * sizeof(kzS32), buffer);
        kzsErrorForward(result);
    }
    else
    {
        kzUint i;
        for (i = 0; i < numValues; ++i)
        {
            result = kzcInputStreamReadS32(inputStream, &values[i]);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsException kzcInputStreamReadS32Int(struct KzcInputStream* inputStream, kzInt* out_value)
{
    kzsException result;
    kzU32 value;

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadU32(inputStream, &value);
    kzsExceptionForward(result);

    *out_value = (kzInt)(kzS32)value;
    kzsSuccess();
}

kzsException kzcInputStreamReadFloat(struct KzcInputStream* inputStream, kzFloat* out_value)
{
    kzsException result;
    kzByte buffer[sizeof(kzFloat)];
    kzFloat value;

    kzsAssert(kzcIsValidPointer(inputStream));

    result = kzcInputStreamReadBytes(inputStream, sizeof(kzFloat), buffer);
    kzsExceptionForward(result);

    switch (inputStream->endianness)
    {
        case KZC_IO_STREAM_ENDIANNESS_LITTLE_ENDIAN:
        {
            value = kzsBytesToFloatLittleEndian(buffer);
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN:
        {
            value = kzsBytesToFloatBigEndian(buffer);
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_PLATFORM:
        {
            value = *(kzFloat*)buffer;
            break;
        }

        case KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED:
        {
            kzsErrorThrow(KZC_ERROR_UNSPECIFIED_ENDIANNESS, "Input stream endianness was not specified");
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid input stream type");
        }
    }

    kzsCounterIncrease("kzcInputStreamReadFloat");
    *out_value = value;
    kzsSuccess();
}

kzsException kzcInputStreamReadFloatArray(struct KzcInputStream* inputStream, kzUint numValues, kzFloat* values)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(inputStream));

    if (inputStream->endianness == KZC_IO_STREAM_ENDIANNESS_PLATFORM)
    {
        kzByte* buffer = (kzByte*)values;

        result = kzcInputStreamReadBytes(inputStream, numValues * sizeof(kzFloat), buffer);
        kzsErrorForward(result);
    }
    else
    {
        kzUint i;
        for (i = 0; i < numValues; ++i)
        {
            result = kzcInputStreamReadFloat(inputStream, &values[i]);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsException kzcInputStreamReadDouble(struct KzcInputStream* inputStream, kzDouble* out_value)
{
    KZS_NOT_IMPLEMENTED_YET_ERROR;
}

kzsError kzcInputStreamReadAllBytes(struct KzcInputStream* inputStream, const struct KzcMemoryManager* memoryManager,
                                    kzUint* out_size, kzByte** out_data)
{
    kzsError result;
    kzUint bufferSize = 80; /* Initial size for buffer. */
    kzByte* resultData;
    kzUint bufferWritePosition = 0;
    kzByte* buffer;

    /* Initialize buffer with default size. Will grow later if necessary. */
    result = kzcMemoryAllocPointer(memoryManager, &buffer, bufferSize, "Byte data buffer");
    kzsErrorForward(result);

    {
        kzBool endOfStream = KZ_FALSE;
        
        /* Loop until done reading. */
        while (!endOfStream)
        {
            kzUint numBytesRead;

            /* If the buffer is full, reallocate the buffer with double the size. */
            if (bufferWritePosition == bufferSize - 1)
            {
                kzByte* oldBuffer = buffer;
                kzUint oldBufferSize = bufferSize;

                bufferSize *= 2;
                result = kzcMemoryAllocPointer(memoryManager, &buffer, bufferSize, "Byte data buffer");
                kzsErrorForward(result);

                kzsMemcpy(buffer, oldBuffer, oldBufferSize);

                result = kzcMemoryFreePointer(oldBuffer);
                kzsErrorForward(result);
            }

            result = kzcInputStreamRead(inputStream, bufferSize - bufferWritePosition - 1, &numBytesRead, &buffer[bufferWritePosition]);
            kzsErrorForward(result);

            bufferWritePosition += numBytesRead;

            if (numBytesRead == 0)
            {
                /* End of stream encountered. */
                endOfStream = KZ_TRUE;
            }
        }
    }

    result = kzcMemoryAllocPointer(memoryManager, &resultData, bufferWritePosition, "Byte data");
    kzsErrorForward(result);

    kzsMemcpy(resultData, buffer, bufferWritePosition);

    result = kzcMemoryFreePointer(buffer);
    kzsErrorForward(result);

    *out_size = bufferWritePosition;
    *out_data = resultData;
    kzsSuccess();
}

kzsError kzcInputStreamReadText(struct KzcInputStream* inputStream, const struct KzcMemoryManager* memoryManager, kzMutableString* out_string)
{
    kzsError result;
    kzUint bufferSize = 80; /* Initial size for buffer. */
    kzMutableString resultString;
    kzUint bufferWritePosition = 0;
    kzChar* buffer;

    /* Initialize buffer with default size. Will grow later if necessary. */
    result = kzcMemoryAllocPointer(memoryManager, &buffer, bufferSize, "String buffer");
    kzsErrorForward(result);

    {
        kzBool endOfStream = KZ_FALSE;
        
        /* Loop until done reading. */
        while (!endOfStream)
        {
            kzUint numBytesRead;

            /* If the buffer is full, reallocate the buffer with double the size. */
            if (bufferWritePosition == bufferSize - 1)
            {
                kzChar* oldBuffer = buffer;
                kzUint oldBufferSize = bufferSize;

                bufferSize *= 2;
                result = kzcMemoryAllocPointer(memoryManager, &buffer, bufferSize, "String buffer");
                kzsErrorForward(result);

                kzsMemcpy(buffer, oldBuffer, oldBufferSize);

                result = kzcMemoryFreePointer(oldBuffer);
                kzsErrorForward(result);
            }

            result = kzcInputStreamRead(inputStream, bufferSize - bufferWritePosition - 1, &numBytesRead, (kzByte*)&buffer[bufferWritePosition]);
            kzsErrorForward(result);

            bufferWritePosition += numBytesRead;

            if (numBytesRead == 0)
            {
                /* End of stream encountered. */
                endOfStream = KZ_TRUE;
            }
        }
    }

    result = kzcStringAllocate(memoryManager, bufferWritePosition, &resultString);
    kzsErrorForward(result);

    kzsMemcpy(resultString, buffer, bufferWritePosition);
    resultString[bufferWritePosition] = KZC_STRING_TERMINATION_SYMBOL;

    result = kzcMemoryFreePointer(buffer);
    kzsErrorForward(result);

    *out_string = resultString;
    kzsSuccess();
}

kzsError kzcInputStreamReadTextLines(struct KzcInputStream* inputStream, const struct KzcMemoryManager* memoryManager, kzMutableString** out_lines)
{
    kzsError result;
    struct KzcDynamicArray* dynamicLines;
    kzMutableString* lines;
    kzUint bufferSize = 80; /* Initial size for buffer. */
    kzChar* buffer;

    result = kzcDynamicArrayCreate(memoryManager, &dynamicLines);
    kzsErrorForward(result);

    /* Initialize buffer with default size. Will grow later if necessary. */
    result = kzcMemoryAllocPointer(memoryManager, &buffer, bufferSize, "String buffer");
    kzsErrorForward(result);

    {
        kzBool endOfStream = KZ_FALSE;
        kzUint bufferReadPosition = 0;
        kzUint bufferWritePosition = 0;
        kzUint lineLength = 0;

        /* Loop until done reading. */
        while (!endOfStream)
        {
            kzBool endOfLine = KZ_FALSE;

            /* Producer: Read new bytes to the buffer */
            /* Consumer: Find end of line markers from the buffer and remove full lines when encountered */

            /* Produce more bytes if needed. */
            if (bufferReadPosition == bufferWritePosition)
            {
                kzUint numBytesRead;

                /* If the buffer is full, reallocate the buffer with double the size. */
                if (bufferWritePosition == bufferSize - 1)
                {
                    kzChar* oldBuffer = buffer;
                    kzUint oldBufferSize = bufferSize;

                    bufferSize *= 2;
                    result = kzcMemoryAllocPointer(memoryManager, &buffer, bufferSize, "String buffer");
                    kzsErrorForward(result);

                    kzsMemcpy(buffer, oldBuffer, oldBufferSize);

                    result = kzcMemoryFreePointer(oldBuffer);
                    kzsErrorForward(result);
                }

                result = kzcInputStreamRead(inputStream, bufferSize - bufferWritePosition - 1, &numBytesRead, (kzByte*)&buffer[bufferWritePosition]);
                kzsErrorForward(result);

                bufferWritePosition += numBytesRead;

                if (numBytesRead == 0)
                {
                    /* End of stream encountered. */
                    endOfStream = KZ_TRUE;
                }
            }

            /* Consume the bytes. */
            while (bufferReadPosition < bufferWritePosition && !endOfLine)
            {
                kzChar ch = buffer[bufferReadPosition++];

                if (ch == '\n')
                {
                    endOfLine = KZ_TRUE;
                }
                else if (ch != '\r')
                {
                    ++lineLength;
                }
            }

            if (endOfLine || endOfStream)
            {
                kzMutableString line;

                result = kzcStringAllocate(memoryManager, lineLength, &line);
                kzsErrorForward(result);

                /* Copy characters from buffer to newly created line. */
                {
                    kzUint i = 0;
                    kzUint j = 0;
                    while (j < lineLength)
                    {
                        kzChar ch = buffer[i++];
                        if (ch != '\r')
                        {
                            line[j++] = ch;
                        }
                    }
                    line[j] = KZC_STRING_TERMINATION_SYMBOL;
                }

                /* Add the line to the dynamic array. */
                result = kzcDynamicArrayAdd(dynamicLines, line);

                {
                    kzUint i;
                    /* Move the remaining of the buffer to beginning discarding the previous line. */
                    for (i = 0; i < bufferWritePosition - bufferReadPosition; ++i)
                    {
                        buffer[i] = buffer[bufferReadPosition + i];
                    }
                }
                bufferWritePosition -= bufferReadPosition;
                bufferReadPosition = 0;
                lineLength = 0;
            }
        }

        /* Copy the lines from the dynamic array to a real array. */
        result = kzcDynamicArrayToArray(memoryManager, dynamicLines, (void***)&lines);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreePointer(buffer);
    kzsErrorForward(result);

    result = kzcDynamicArrayDelete(dynamicLines);
    kzsErrorForward(result);

    *out_lines = lines;
    kzsSuccess();
}

enum KzcIOStreamEndianness kzcInputStreamGetEndianness(const struct KzcInputStream* inputStream)
{
    kzsAssert(kzcIsValidPointer(inputStream));
    return inputStream->endianness;
}

void kzcInputStreamSetEndianness(struct KzcInputStream* inputStream, enum KzcIOStreamEndianness endianness)
{
    kzsAssert(kzcIsValidPointer(inputStream));
    inputStream->endianness = endianness;
}
