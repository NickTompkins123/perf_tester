/**
* \file
* Logger for storing kanzi log to file.
* 
* Copyright 201 by Rightware. All rights reserved.
*/
#ifndef BF_LOGGER_H 
#define BF_LOGGER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h>


/* Forward declarations.*/
struct KzcMemoryManager;

/**
* \struct BfLogger
* Contains the bf logger state. File handle and such. 
*/
struct BfLogger;


/** Creates logger struct. */
kzsError bfLoggerCreate(const struct KzcMemoryManager* memoryManager, kzString filePath, kzBool writeToFile, struct BfLogger** out_logger);

/** Deletes logger struct. */
kzsError bfLoggerDelete(struct BfLogger* logger);

/** A callback for system log handler. Stores messages to file. */
KZ_CALLBACK kzsError bfLoggerStoreMessage_callback(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line, void* userData);


#endif
