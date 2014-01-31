/**
 * \file
 * Core logging. Higher level helper utilities for logging.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_LOG_H
#define KZC_LOG_H


#include <system/debug/kzs_log.h>
#include <system/debug/kzs_error.h>
#include <system/wrappers/kzs_arg.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzcMemoryManager;


/**
 * Logs a formatted message with arbitrary number of parameters.
 * Arguments parameter provides the formatting arguments as variable arguments list.
 * The formatting is the same as in kzcStringFormat.
 */
kzsError kzcLogList(const struct KzcMemoryManager* memoryManager, KzsLogLevel level, kzString format, kzArgList *arguments);

/**
 * Logs a formatted message with arbitrary number of parameters.
 * Formatting arguments are given as variable arguments.
 * The formatting is the same as in kzcStringFormat.
 */
kzsError kzcLog(const struct KzcMemoryManager* memoryManager, KzsLogLevel level, kzString format, ...);

/**
 * Logs a formatted message with arbitrary number of parameters with KZS_LOG_LEVEL_DEBUG.
 * Formatting arguments are given as variable arguments.
 * The formatting is the same as in kzcStringFormat.
 * Warning: This function always uses a thread unsafe global system memory manager and discards all errors silently.
 */
void kzcLogDebug(kzString format, ...);


#endif
