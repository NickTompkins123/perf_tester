/**
 * \file
 * Core logging.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_log.h"

#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_system.h>

#include <system/debug/kzs_log.h>


kzsError kzcLogList(const struct KzcMemoryManager* memoryManager, KzsLogLevel level, kzString format, kzArgList *arguments)
{
    kzsError result;
    kzMutableString message;

    result = kzcStringFormatList(memoryManager, format, arguments, &message);
    kzsErrorForward(result);

    kzsLog_private(level, KZS_SUCCESS, message, "", 0);

    result = kzcStringDelete(message);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcLog(const struct KzcMemoryManager* memoryManager, KzsLogLevel level, kzString format, ...)
{
    kzsError result;
    kzArgList arguments;

    va_start(arguments, format);

    result = kzcLogList(memoryManager, level, format, &arguments);
    kzsErrorForward(result);

    va_end(arguments);

    kzsSuccess();
}

void kzcLogDebug(kzString format, ...)
{
    kzArgList arguments;
    struct KzcMemoryManager* memoryManager;

    KZ_UNUSED_RETURN_VALUE(kzcMemoryManagerCreateSystemManager(&memoryManager)); /* Ignore all errors as this is a debugging function. */

    va_start(arguments, format);
    KZ_UNUSED_RETURN_VALUE(kzcLogList(memoryManager, KZS_LOG_LEVEL_DEBUG, format, &arguments)); /* Ignore all errors as this is a debugging function. */
    va_end(arguments);

    KZ_UNUSED_RETURN_VALUE(kzcMemoryManagerDelete(memoryManager)); /* Ignore all errors as this is a debugging function. */
}
