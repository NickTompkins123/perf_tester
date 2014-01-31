/**
* \file
* Standard file I/O functions
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_FILE_H
#define KZS_FILE_H


#include <system/kzs_types.h>


/** Result value indicating that an error occurred in the function. */
#define KZS_FILE_ERROR -1


#define KZS_FILE_WRITEONLY          "w" /**< File is opened in writeable text mode. */
#define KZS_FILE_WRITEONLY_BINARY   "wb" /**< File is opened in writeable binary mode. */
#define KZS_FILE_READONLY           "r" /**< File is opened in read only text mode. */
#define KZS_FILE_READONLY_BINARY    "rb" /**< File is opened in read only binary mode. */


/** Origin for kzsFseek. */
enum KzsFileOrigin
{
    KZS_FILE_ORIGIN_SEEK_SET = 0,   /**< Seek from the start of the file. */
    KZS_FILE_ORIGIN_SEEK_CUR = 1,   /**< Seek from the current location. */
    KZS_FILE_ORIGIN_SEEK_END = 2    /**< Seek from the end of the file. */
};


/** File type. */
typedef void kzsFile;


/** Opens a file. */
kzsFile* kzsFopen(kzString fileName, kzString mode);

/** Closes a file. */
kzInt kzsFclose(kzsFile* file);

/** Checks if the end of a file has been reached. */
kzBool kzsFeof(kzsFile* file);

/** Checks if there is an error in a file. */
kzInt kzsFerror(kzsFile* file);

/** Seeks to given position in a file. */
kzInt kzsFseek(kzsFile* file, kzUint offset, enum KzsFileOrigin origin);

/** Gets the current position in a file. */
kzInt kzsFtell(kzsFile* file);

/** Flushes the stream. Writes any unwritten data to file. */
kzInt kzsFflush(kzsFile* file);

/** Gets a byte from a file. */
kzInt kzsFgetc(kzsFile* file);

/** Writes a byte to a file. */
kzInt kzsFputc(kzInt character, kzsFile* file);

/** Reads elements from a file. Returns the number of elements read. */
kzUint kzsFread(void* buffer, kzUint elementSize, kzUint elementCount, kzsFile* file);

/** Writes elements to a file. Returns the number of elements written. */
kzUint kzsFwrite(const void* buffer, kzUint elementSize, kzUint elementCount, kzsFile* file);


/* Additional file functions */

/** Get the size of a file. Returns -1 if an error occurred. */
kzInt kzsFileGetSize(kzString fileName);

/**
 * Skips bytes in a file. Returns the number of bytes skipped.
 * Returns KZS_FILE_ERROR if an error occurred.
 * This function is intended for skipping in a file only when reading it.
 * To skip in a writable file, use kzsFseek with KZS_FILE_ORIGIN_SEEK_CUR as origin.
 */
kzInt kzsFileReadSkip(kzsFile* file, kzUint byteCount);


#endif
