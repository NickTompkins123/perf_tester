/**
* \file
* Benchmark scene default log updater functions.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "bf_logger.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/util/string/kzc_string.h>


struct BfLogger
{
    struct KzcOutputStream* outputStream; /**< Stream to text file to write log to. */
};


kzsError bfLoggerCreate(const struct KzcMemoryManager* memoryManager, kzString filePath, kzBool writeToFile, struct BfLogger** out_logger)
{
    kzsError result;
    struct BfLogger* logger;

    result = kzcMemoryAllocVariable(memoryManager, logger, "Logger struct");
    kzsErrorForward(result);

    if(writeToFile)
    {
        result = kzcOutputStreamCreateToFile(memoryManager, filePath, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &logger->outputStream);
        kzsErrorForward(result);
        result = kzsLogRegisterHandler(bfLoggerStoreMessage_callback, logger);
        kzsErrorForward(result);
    }
    else
    {
        logger->outputStream = KZ_NULL;
        kzsLog(KZS_LOG_LEVEL_INFO, "Writing log output is disabled.");
    }

    *out_logger = logger;
    kzsSuccess();
}

kzsError bfLoggerDelete(struct BfLogger* logger)
{
    kzsError result;

    kzsLogUnregisterHandler(bfLoggerStoreMessage_callback);

    if(logger->outputStream != KZ_NULL)
    {
        result = kzcOutputStreamDelete(logger->outputStream);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(logger);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError bfLoggerStoreMessage_callback(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line, void* userData)
{
    kzsError result;
    kzString prefix;
    struct BfLogger* logger = (struct BfLogger*)userData;

    switch (level)
    {
        case KZS_LOG_LEVEL_ERROR:
        {
            prefix = "ERROR: ";

            result = kzcOutputStreamWriteBytes(logger->outputStream, kzcStringLength(prefix), (kzByte*)prefix);
            kzsErrorForward(result);
            break;
        }

        case KZS_LOG_LEVEL_WARNING:
        {
            prefix = "WARNING: ";

            result = kzcOutputStreamWriteBytes(logger->outputStream, kzcStringLength(prefix), (kzByte*)prefix);
            kzsErrorForward(result);
            break;    
        }
        default:
        {
            break;
        }
    }

    result = kzcOutputStreamWriteBytes(logger->outputStream, kzcStringLength(message), (kzByte*)message);
    kzsErrorForward(result);

    result = kzcOutputStreamWriteU8(logger->outputStream, (kzU8)'\n');
    kzsErrorForward(result);
    
    result = kzcOutputStreamFlush(logger->outputStream);
    kzsErrorForward(result);

    kzsSuccess();
}
