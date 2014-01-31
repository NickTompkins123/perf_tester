/**
* \file
* Core file utilities.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_FILE_H
#define KZC_FILE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;


/** Get the size of a file with given path. */
kzsError kzcFileGetSize(kzString filePath, kzUint* out_size);

/** Get the file extension of a file with given path. */
kzsError kzcFileGetExtension(const struct KzcMemoryManager* memoryManger, kzString filePath, kzMutableString* out_extension);

/** Reads binary content of a file with given path. */
kzsError kzcFileReadBinaryFile(const struct KzcMemoryManager* memoryManager, kzString filePath, kzUint* out_size, kzByte** out_data);

/** Reads binary content of a resource with given path. */
kzsError kzcFileReadBinaryResource(const struct KzcMemoryManager* memoryManager, kzString resourcePath, kzUint* out_size, kzByte** out_data);

/** Reads text file to the string with given path. */
kzsError kzcFileReadTextFile(const struct KzcMemoryManager* memoryManager, kzString filePath, kzMutableString* out_string);
/** Reads text lines of a file with given path. */
kzsError kzcFileReadTextFileLines(const struct KzcMemoryManager* memoryManager, kzString filePath, kzMutableString** out_lines);

/** Reads text resource file to the string with given path. */
kzsError kzcFileReadTextResource(const struct KzcMemoryManager* memoryManager, kzString filePath, kzMutableString* out_string);
/** Reads text lines of a resource file with given path. */
kzsError kzcFileReadTextResourceLines(const struct KzcMemoryManager* memoryManager, kzString resourcePath, kzMutableString** out_lines);

/** Writes binary data to a file with given path. */
kzsError kzcFileWriteBinaryFile(const struct KzcMemoryManager* memoryManager, kzString filePath, kzUint size, const kzByte* data);

/** Writes text lines to a file with given path. */
kzsError kzcFileWriteTextFile(const struct KzcMemoryManager* memoryManager, kzString filePath, kzUint lineCount, const kzString* lines);

/** Returns true if file exists. */
kzBool kzcFileExists(kzString filePath);


#endif
