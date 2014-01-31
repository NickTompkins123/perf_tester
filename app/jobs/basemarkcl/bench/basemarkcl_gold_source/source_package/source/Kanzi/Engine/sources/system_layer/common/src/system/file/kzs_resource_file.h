/**
* \file
* Resource file reading functions.
* Resource files provide simplified API for reading files (but not writing).
* This is the recommended API for reading resource files of the application,
* since full file-system API might not be available on all platforms.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_RESOURCE_FILE_H
#define KZS_RESOURCE_FILE_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/** Result value indicating that an error occurred in the function. */
#define KZS_RESOURCE_FILE_ERROR -1


/**
 * \struct KzsResourceFile
 * Resource structure
 */
struct KzsResourceFile;


/** Opens a resource. */
kzsError kzsResourceFileCreate(kzString resourcePath, struct KzsResourceFile** out_resourceFile);

/** Closes a resource. */
kzsError kzsResourceFileDelete(struct KzsResourceFile* resourceFile);

/**
 * Reads bytes from a resource. Returns the number of bytes read.
 * Returns 0 if no bytes could be read, because the resource has been completely finished.
 * Returns KZS_RESOURCE_FILE_ERROR if an error occurred.
 */
kzInt kzsResourceFileRead(struct KzsResourceFile* resourceFile, kzUint byteCount, kzByte* out_buffer);

/**
 * Skips bytes from a resource. Returns the number of bytes skipped.
 * Returns 0 if no bytes could be skipped, because the resource has been completely finished.
 * Returns KZS_RESOURCE_FILE_ERROR if an error occurred.
 */
kzInt kzsResourceFileSkip(struct KzsResourceFile* resourceFile, kzUint byteCount);

/** Gets the current position in a resource. */
kzUint kzsResourceFileGetPosition(const struct KzsResourceFile* resourceFile);


#endif
