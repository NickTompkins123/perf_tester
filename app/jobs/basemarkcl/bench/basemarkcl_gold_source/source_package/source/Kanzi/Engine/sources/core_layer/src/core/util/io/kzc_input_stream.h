/**
* \file
* Input stream.
* Input streams are used for reading binary data from several different data sources.
* Supprted data sources are:
* <ul>
*   <li>Files</li>
*   <li>Resources</li>
*   <li>Memory buffers</li>
*   <li>Nested input streams</li>
* </ul>
* Streams are configured to read data in either little-endian, big-endian or platform's native endianness format.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_INPUT_STREAM_H
#define KZC_INPUT_STREAM_H


#include "kzc_io_stream.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

#define KZC_INPUT_STREAM_BUFFER_DEFAULT_SIZE (512 * 8)

/* Forward declarations */
struct KzcMemoryManager;


/**
 * \struct KzcInputStream
 * Input stream for reading binary data from files, resource files, memory or nested input streams.
 */
struct KzcInputStream;


/** Function type for deleting custom input stream. */
typedef kzsError (*KzcCustomInputStreamDeleteFunction)(void* customData);
/** Function type for reading bytes from custom input stream. */
typedef kzsError (*KzcCustomInputStreamReadFunction)(void* customData, kzUint byteCount, kzUint* out_bytesReadCount, kzByte* out_bytes);
/** Function type for skipping in a custom input stream. */
typedef kzsError (*KzcCustomInputStreamSkipFunction)(void* customData, kzUint skipAmount);


/** Creates an input stream pointing to a file, whose path is given as a parameter. */
kzsError kzcInputStreamCreateFromFile(const struct KzcMemoryManager* memoryManager, kzString filePath,
                                      enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream);

/** Creates an input stream pointing to a resource file, whose path is given as a parameter. */
kzsError kzcInputStreamCreateFromResource(const struct KzcMemoryManager* memoryManager, kzString resourcePath,
                                          enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream);

/** Creates an input stream pointing to a given memory buffer of given size. */
kzsError kzcInputStreamCreateFromMemory(const struct KzcMemoryManager* memoryManager, const kzByte* buffer, kzUint size,
                                        enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream);

/**
 * Creates an input stream pointing to another input stream.
 * Length of the target input stream can be given as a parameter. In this case the stream will read at most
 * that many bytes from the target stream and possibly unread bytes are skipped when deleting the stream.
 * \param targetLength Length of the target input stream or KZC_IO_STREAM_LENGTH_UNKNOWN if unknown.
 */
kzsError kzcInputStreamCreateFromStream(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* targetInputStream, kzInt targetLength,
                                        enum KzcIOStreamEndianness endianness, struct KzcInputStream** out_inputStream);

/** Creates an input stream using custom read functions. */
kzsError kzcInputStreamCreateCustom(const struct KzcMemoryManager* memoryManager, void* customData,
                                    KzcCustomInputStreamDeleteFunction deleteFunction, KzcCustomInputStreamReadFunction readFunction,
                                    KzcCustomInputStreamSkipFunction skipFunction, enum KzcIOStreamEndianness endianness,
                                    struct KzcInputStream** out_inputStream);

/** Deletes an input stream. */
kzsError kzcInputStreamDelete(struct KzcInputStream* inputStream);

/** Initializes the size of input stream buffer. */
kzsError kzcInputStreamInitBuffer(struct KzcInputStream* inputStream, kzUint bufferSize);

/**
 * Reads some amount of bytes from the given input stream.
 * The maximum number of bytes is given as parameter and the actual number of bytes read is returned.
 * Negative number is returned, if no bytes can be read because end of stream has been reached.
 * This function can be used to read input stream where the number of expected bytes available is not known.
 */
kzsError kzcInputStreamRead(struct KzcInputStream* inputStream, kzUint byteCount, kzUint* out_bytesReadCount, kzByte* out_bytes);

/** Skips the given amount of bytes in an input stream. */
kzsException kzcInputStreamSkip(struct KzcInputStream* inputStream, kzUint skipAmount);

/** Gets the current position in an input stream. */
kzUint kzcInputStreamGetPosition(const struct KzcInputStream* inputStream);


/** Gets endianness of input stream. */
enum KzcIOStreamEndianness kzcInputStreamGetEndianness(const struct KzcInputStream* inputStream);
/** Sets endianness for input stream. */
void kzcInputStreamSetEndianness(struct KzcInputStream* inputStream, enum KzcIOStreamEndianness endianness);


/**
 * Reads given amount of bytes from the input stream.
 * An error is returned if the input stream has an error, or end of stream is reached too soon.
 */
kzsException kzcInputStreamReadBytes(struct KzcInputStream* inputStream, kzUint numBytes, kzByte* out_bytes);

/** Reads a boolean from an input stream. */
kzsException kzcInputStreamReadBoolean(struct KzcInputStream* inputStream, kzBool* out_value);

/** Reads an unsigned 8-bit value from an input stream. */
kzsException kzcInputStreamReadU8(struct KzcInputStream* inputStream, kzU8* out_value);
/** Reads an unsigned 8-bit value casted to an unsigned integer from an input stream. */
kzsException kzcInputStreamReadU8Int(struct KzcInputStream* inputStream, kzUint* out_value);
/** Reads a signed 8-bit value from an input stream. */
kzsException kzcInputStreamReadS8(struct KzcInputStream* inputStream, kzS8* out_value);
/** Reads a signed 8-bit value casted to a signed integer from an input stream. */
kzsException kzcInputStreamReadS8Int(struct KzcInputStream* inputStream, kzInt* out_value);

/** Reads an unsigned 16-bit value from an input stream. */
kzsException kzcInputStreamReadU16(struct KzcInputStream* inputStream, kzU16* out_value);
/** Reads an array of 16-bit values form an input stream. */
kzsException kzcInputStreamReadU16Array(struct KzcInputStream* inputStream, kzUint numValues, kzU16* values);
/** Reads an unsigned 16-bit value casted to an unsigned integer from an input stream. */
kzsException kzcInputStreamReadU16Int(struct KzcInputStream* inputStream, kzUint* out_value);
/** Reads a signed 16-bit value from an input stream. */
kzsException kzcInputStreamReadS16(struct KzcInputStream* inputStream, kzS16* out_value);
/** Reads an array of signed 16-bit values form an input stream. */
kzsException kzcInputStreamReadS16Array(struct KzcInputStream* inputStream, kzUint numValues, kzS16* values);
/** Reads a signed 16-bit value casted to a signed integer from an input stream. */
kzsException kzcInputStreamReadS16Int(struct KzcInputStream* inputStream, kzInt* out_value);

/** Reads an unsigned 32-bit value from an input stream. */
kzsException kzcInputStreamReadU32(struct KzcInputStream* inputStream, kzU32* out_value);
/** Reads an array of unsigned 32-bit values from an input stream. */
kzsException kzcInputStreamReadU32Array(struct KzcInputStream* inputStream, kzUint numValues, kzU32* values);
/** Reads an unsigned 32-bit value casted to an unsigned integer from an input stream. */
kzsException kzcInputStreamReadU32Int(struct KzcInputStream* inputStream, kzUint* out_value);
/** Reads a signed 32-bit value from an input stream. */
kzsException kzcInputStreamReadS32(struct KzcInputStream* inputStream, kzS32* out_value);
/** Reads an array of signed 32-bit values from an input stream. */
kzsException kzcInputStreamReadS32Array(struct KzcInputStream* inputStream, kzUint numValues, kzS32* values);
/** Reads a signed 32-bit value casted to a signed integer from an input stream. */
kzsException kzcInputStreamReadS32Int(struct KzcInputStream* inputStream, kzInt* out_value);

/** Reads a 32-bit floating point value from an input stream. */
kzsException kzcInputStreamReadFloat(struct KzcInputStream* inputStream, kzFloat* out_value);
/** Reads a float array from an input stream. */
kzsException kzcInputStreamReadFloatArray(struct KzcInputStream* inputStream, kzUint numValues, kzFloat* values);
/** Reads a 64-bit floating point value from an input stream. */
kzsException kzcInputStreamReadDouble(struct KzcInputStream* inputStream, kzDouble* out_value);


/** Reads the rest of the input stream to an array of bytes. The caller must free the returned data using kzcMemoryFreePointer(). */
kzsError kzcInputStreamReadAllBytes(struct KzcInputStream* inputStream, const struct KzcMemoryManager* memoryManager,
                                    kzUint* out_size, kzByte** out_data);

/** Reads the rest of the input stream to a string. The caller must free the returned string using kzcStringDelete(). */
kzsError kzcInputStreamReadText(struct KzcInputStream* inputStream, const struct KzcMemoryManager* memoryManager,
                                kzMutableString* out_string);

/**
 * Reads the rest of the input stream to an array of strings. Each item in the array is a line of text from the input stream.
 * Newline and carriage return characters are discarded from the text.
 * The caller must free the returned lines using kzcStringDelete() and the string array using kzcMemoryFreeArray().
 */
kzsError kzcInputStreamReadTextLines(struct KzcInputStream* inputStream, const struct KzcMemoryManager* memoryManager,
                                     kzMutableString** out_lines);


#endif
