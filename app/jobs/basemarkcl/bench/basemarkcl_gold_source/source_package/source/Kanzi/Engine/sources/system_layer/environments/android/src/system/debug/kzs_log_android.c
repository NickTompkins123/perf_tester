/**
 * \file
 * Logging.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_string.h>

/* Standard library */
#include <stdio.h>

/* Android API */
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "Kanzi"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


static kzsError androidLogger(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line, void* userData)
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
            LOGI("%s: %s [%s:%u]\n", prefix, message, file, line);
        }
        else
        {
            LOGI("%s: %s\n", prefix, message);
        }
    }
    else
    {
        LOGI("%s %i: %s [%s:%u]\n", prefix, errorCode, message, file, line);
    }

    kzsSuccess();
}

kzsError kzsLogCreateDefaultLoggers()
{
    kzsError result;

    result = kzsLogRegisterHandler(kzsLogBasicLogger, KZ_NULL);
    kzsErrorForward(result);
    
    result = kzsLogRegisterHandler(androidLogger, KZ_NULL);
    kzsErrorForward(result);

    kzsSuccess();
}
