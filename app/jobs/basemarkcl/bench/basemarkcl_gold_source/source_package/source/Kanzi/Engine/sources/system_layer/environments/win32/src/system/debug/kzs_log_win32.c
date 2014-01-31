/**
 * \file
 * Logging.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include <system/debug/kzs_log.h>

/* Standard library */
#include <stdio.h>

/* Windows API */
#include <windows.h>


static kzsError visualStudioLogger(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line, void* userData)
{
    kzString prefix;
    kzChar buffer[256];

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
        if (level <= KZS_LOG_LEVEL_CONTEXT_INFO_THRESHOLD)
        {
            _snprintf_s(buffer, 256, _TRUNCATE, "%s: %s [%s:%u]\n", prefix, message, file, line);
        }
        else
        {
            _snprintf_s(buffer, 256, _TRUNCATE, "%s: %s\n", prefix, message);
        }
    }
    else
    {
        _snprintf_s(buffer, 256, _TRUNCATE, "%s %i: %s [%s:%u]\n", prefix, errorCode, message, file, line);
    }
    OutputDebugString(buffer);

    kzsSuccess();
}

kzsError kzsLogCreateDefaultLoggers()
{
    kzsError result;

    result = kzsLogRegisterHandler(kzsLogBasicLogger, KZ_NULL);
    kzsErrorForward(result);
    
    result = kzsLogRegisterHandler(visualStudioLogger, KZ_NULL);
    kzsErrorForward(result);

    kzsSuccess();
}
