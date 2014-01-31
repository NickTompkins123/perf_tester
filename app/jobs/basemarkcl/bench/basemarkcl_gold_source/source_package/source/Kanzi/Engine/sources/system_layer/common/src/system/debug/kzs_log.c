/**
 * \file
 * Logging.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzs_log.h"

#include <system/kzs_platform_overrides.h>
#include <system/debug/kzs_error.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_string.h>
#include <system/kzs_error_codes.h>

/* Standard library */
#include <stdio.h>


struct LogHandler
{
    KzsLogFunction logFunction;
    void* userData;
};

static struct LogHandler basicHandler =
{
    kzsLogBasicLogger,
    KZ_NULL
};


static const kzUint MAX_HANDLERS = 10;
static kzUint handlerCount = 0;
static struct LogHandler* handlers = KZ_NULL;


kzsError kzsLogInitialize()
{
    kzsError result;

    handlers = kzsMalloc(MAX_HANDLERS * sizeof(struct LogHandler));
    kzsErrorTest(handlers != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while trying to allocate memory");

    handlerCount = 0;

    result = kzsLogCreateDefaultLoggers();
    kzsErrorSubstitute(result, KZS_ERROR_INITIALIZATION_FAILED, "Failed to create default loggers");

    kzsSuccess();
}

kzsError kzsLogUninitialize()
{
    kzsFree(handlers);

    handlerCount = 0;
    handlers = KZ_NULL;

    kzsSuccess();
}

kzsError kzsLogRegisterHandler(KzsLogFunction logFunction, void* userData)
{
    struct LogHandler* handler;

    kzsErrorTest(handlerCount < MAX_HANDLERS, KZS_ERROR_TOO_MANY_LOGGERS, "Too many log handlers");

    handler = &handlers[handlerCount++];
    handler->logFunction = logFunction;
    handler->userData = userData;

    kzsSuccess();
}

void kzsLogUnregisterHandler(KzsLogFunction logFunction)
{
    kzUint i;    
    for(i = 0; i < handlerCount; i++)
    {
        struct LogHandler* handler = &handlers[i];

        /* Find the handler to remove. */
        if(handler->logFunction == logFunction)
        {
            /* Replace it with the last handler. */
            if(handlerCount > 1)
            {
                handlers[i] = handlers[handlerCount - 1];
            }
            --handlerCount;
        }
    }
}

static kzsError kzsLogNotifyLogEntry(const struct LogHandler* handler, KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line)
{
    kzsError result;

    result = handler->logFunction(level, errorCode, message, file, line, handler->userData);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzsLog_private(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line)
{
#ifndef _DEBUG
    file = "";
    line = 0;
#endif

    if (level > KZS_LOG_LEVEL_ERROR)
    {
        file = "";
        line = 0;
    }

    if (handlerCount == 0)
    {
        /*lint -e{534} Discard error value to prevent infinite error loop. */
        kzsLogNotifyLogEntry(&basicHandler, level, errorCode, message, file, line);
    }
    else
    {
        kzUint i;
        for (i = 0; i < handlerCount; ++i)
        {
            struct LogHandler* handler = &handlers[i];
            /*lint -e{534} Discard error value to prevent infinite error loop. */
            kzsLogNotifyLogEntry(handler, level, errorCode, message, file, line);
        }
    }
}


KZ_CALLBACK kzsError kzsLogBasicLogger(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line, void* userData)
{
    kzString prefix;

    switch (level)
    {
        case KZS_LOG_LEVEL_ERROR:
        {
            prefix = "ERROR";
            break;
        }

        case KZS_LOG_LEVEL_WARNING:
        {
            prefix = "WARNING";
            break;
        }

        case KZS_LOG_LEVEL_INFO:
        {
            prefix = "INFO";
            break;
        }

        default:
        {
            prefix = "LOG";
            break;
        }
    }

    if (errorCode == KZS_SUCCESS)
    {
        if (level <= KZS_LOG_LEVEL_CONTEXT_INFO_THRESHOLD && kzsStrlen(file) > 0)
        {
            printf("%s: %s [%s:%u]\n", prefix, message, file, line);
        }
        else
        {
            printf("%s: %s\n", prefix, message);
        }
    }
    else
    {
        printf("%s %i: %s [%s:%u]\n", prefix, errorCode, message, file, line);
    }

    /*lint -e{534, 613} Ignore return value of fflush and prevent false warning about using stdout */
    fflush(stdout);

    kzsSuccess();
}

#ifndef KZS_OVERRIDE_LOG_CREATE_DEFAULT_LOGGERS
kzsError kzsLogCreateDefaultLoggers()
{
    kzsError result;

    result = kzsLogRegisterHandler(kzsLogBasicLogger, KZ_NULL);
    kzsErrorForward(result);

    kzsSuccess();
}
#endif
