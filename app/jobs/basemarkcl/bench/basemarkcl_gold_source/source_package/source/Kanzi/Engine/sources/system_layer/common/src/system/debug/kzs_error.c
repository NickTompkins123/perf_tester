/**
 * \file
 * Error.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzs_error.h"

#include "kzs_log.h"

#include <system/wrappers/kzs_memory.h>
#include <system/kzs_platform_overrides.h>
#include <system/kzs_error_codes.h>
#include <system/kzs_globals.h>
#include <system/wrappers/kzs_string.h>

#include <assert.h> /*lint -efile(766,assert.h) This file is referenced inside macros. */


#define KZS_ERROR_GLOBAL_BUFFER_KEY "ERROR_STRING" /**< Key for kzsGlobals for error string. */
#define KZS_ERROR_STRING_MAXIMUM_LENGTH 256 /**< Maximum lenght of string length. */



void kzsExceptionLog_private(kzString message)
{
    /* TODO: Write out to log? */
    KZ_UNUSED_PARAMETER(message);
}

void kzsErrorLog_private(kzsError errorCode, kzString message, kzString file, kzUint line)
{
    /* Prevent recursion in error handling in the case of kzsLog() throwing an error. */
    static kzBool errorLoggingInProgress = KZ_FALSE;

    if (!errorLoggingInProgress)
    {
        errorLoggingInProgress = KZ_TRUE;
        kzsLog_private(KZS_LOG_LEVEL_ERROR, errorCode, message, file, line);
        errorLoggingInProgress = KZ_FALSE;

        {
            kzMutableString errorString = (kzMutableString)kzsGlobalsGet(KZS_ERROR_GLOBAL_BUFFER_KEY);
            if (errorString != KZ_NULL)
            {
                kzsStrncpy(errorString, message, KZS_ERROR_STRING_MAXIMUM_LENGTH);
                errorString[KZS_ERROR_STRING_MAXIMUM_LENGTH - 1] = '\0';
            }
        }
    }
}

#ifndef KZS_OVERRIDE_ASSERT_PRIVATE
void kzsAssert_private(void)
{
    assert(KZ_FALSE); /*lint !e506 !e944 !e774 Suppress warnings of constant boolean */
}
#endif

#ifndef KZS_OVERRIDE_ERROR_BREAK_PRIVATE
void kzsErrorBreak_private()
{
    kzsAssert(KZ_FALSE);
}
#endif

#ifndef KZS_OVERRIDE_DEBUG_BREAK_PRIVATE
void kzsDebugBreak_private()
{
    /* Do nothing */
}
#endif


kzsError kzsErrorInitialize()
{
    kzChar* errorString;
    errorString = kzsMalloc(KZS_ERROR_STRING_MAXIMUM_LENGTH);

    if (errorString == KZ_NULL) 
    {
        kzsErrorThrow(KZS_ERROR_OUT_OF_MEMORY, "Memory allocation error!");
    }
    errorString[0] = '\0';
    
    kzsGlobalsPut(KZS_ERROR_GLOBAL_BUFFER_KEY, errorString);

    kzsSuccess();
}

kzsError kzsErrorUninitialize()
{
    kzMutableString errorString = (kzMutableString)kzsGlobalsGet(KZS_ERROR_GLOBAL_BUFFER_KEY);
    
    kzsFree(errorString);

    kzsSuccess();
}

kzString kzsErrorGetLastErrorMessage(void)
{
    return (kzString)kzsGlobalsGet(KZS_ERROR_GLOBAL_BUFFER_KEY);
}
