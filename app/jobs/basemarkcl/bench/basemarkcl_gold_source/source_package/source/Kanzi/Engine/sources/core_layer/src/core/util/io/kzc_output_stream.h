/**
* \file
* Output stream
* Output streams are used for wrutubg binary data to several different data sources.
* Supprted data sources are:
* <ul>
*   <li>Files</li>
*   <li>Resources</li>
*   <li>Memory buffers</li>
*   <li>Nested output streams</li>
* </ul>
* Streams are configured to write data in either little-endian, big-endian or platform's native endianness format.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_OUTPUT_STREAM_H
#define KZC_OUTPUT_STREAM_H


#include "kzc_io_stream.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;


/**
 * \struct KzcOutputStream
 * Output stream for writing binary data to files, memory or nested output streams.
 */
struct KzcOutputStream;


/** Function type for deleting custom output stream. */
typedef kzsError (*KzcCustomOutputStreamDeleteFunction)(void* customData);
/** Function type for writing bytes to custom output stream. */
typedef kzsError (*KzcCustomOutputStreamWriteFunction)(void* customData, kzUint byteCount, const kzByte* bytes, kzUint* out_bytesWrittenCount);
/** Function type for skipping in a custom output stream. */
typedef kzsError (*KzcCustomOutputStreamSkipFunction)(void* customData, kzUint skipAmount);


/** Creates an output stream pointing to a file, whose path is given as a parameter. */
kzsError kzcOutputStreamCreateToFile(const struct KzcMemoryManager* memoryManager, kzString filePath,
                                     enum KzcIOStreamEndianness endianness, struct KzcOutputStream** out_outputStream);

/** Creates an output stream pointing to a given memory buffer of given size. */
kzsError kzcOutputStreamCreateToMemory(const struct KzcMemoryManager* memoryManager, kzByte* buffer, kzUint size,
                                       enum KzcIOStreamEndianness endianness, struct KzcOutputStream** out_outputStream);

/**
 * Creates an output stream pointing to another output stream.
 * Length of the target output stream can be given as a parameter. In this case the stream will write at most
 * that many bytes to the target stream and possibly unwritten bytes are skipped when deleting the stream.
 * \param targetLength Length of the output stream or KZC_IO_STREAM_LENGTH_UNKNOWN if unknown.
 */
kzsError kzcOutputStreamCreateToStream(const struct KzcMemoryManager* memoryManager, struct KzcOutputStream* targetOutputStream, kzInt targetLength,
                                       enum KzcIOStreamEndianness endianness, struct KzcOutputStream** out_outputStream);

/** Creates an output stream using custom write functions. */
kzsError kzcOutputStreamCreateCustom(const struct KzcMemoryManager* memoryManager, void* customData,
                                     KzcCustomOutputStreamDeleteFunction deleteFunction, KzcCustomOutputStreamWriteFunction writeFunction,
                                     KzcCustomOutputStreamSkipFunction skipFunction, enum KzcIOStreamEndianness endianness,
                                     struct KzcOutputStream** out_outputStream);


/** Deletes an output stream. */
kzsError kzcOutputStreamDelete(struct KzcOutputStream* outputStream);


/**
 * Writes some amount of bytes to the given output stream.
 * The maximum number of bytes is given as parameter and the actual number of bytes written is returned.
 * Negative number is returned, if no bytes can be written because end of stream has been reached.
 * This function can be used to write to output stream when the number of bytes that can be written at once is not known.
 */
kzsError kzcOutputStreamWrite(struct KzcOutputStream* outputStream, kzUint byteCount, const kzByte* bytes, kzUint* out_bytesWrittenCount);

/** Skips the given amount of bytes in an output stream. */
kzsException kzcOutputStreamSkip(struct KzcOutputStream* outputStream, kzUint skipAmount);

/** Gets the current position in an output stream. */
kzUint kzcOutputStreamGetPosition(const struct KzcOutputStream* outputStream);


/** Gets endianness of output stream. */
enum KzcIOStreamEndianness kzcOutputStreamGetEndianness(const struct KzcOutputStream* outputStream);
/** Sets endianness for output stream. */
void kzcOutputStreamSetEndianness(struct KzcOutputStream* outputStream, enum KzcIOStreamEndianness endianness);


/** Flushes the output stream. */
kzsException kzcOutputStreamFlush(const struct KzcOutputStream* outputStream);

/**
 * Writes given amount of bytes to the output stream.
 * An error is returned if the output stream has an error, or end of stream is reached too soon.
 */
kzsException kzcOutputStreamWriteBytes(struct KzcOutputStream* outputStream, kzUint byteCount, const kzByte* bytes);

/** Writes a boolean to an output stream. */
kzsException kzcOutputStreamWriteBoolean(struct KzcOutputStream* outputStream, kzBool value);

/** Writes an unsigned 8-bit value to an output stream. */
kzsException kzcOutputStreamWriteU8(struct KzcOutputStream* outputStream, kzU8 value);
/** Writes a signed 8-bit value to an output stream. */
kzsException kzcOutputStreamWriteS8(struct KzcOutputStream* outputStream, kzS8 value);

/** Writes an unsigned 16-bit value to an output stream. */
kzsException kzcOutputStreamWriteU16(struct KzcOutputStream* outputStream, kzU16 value);
/** Writes an array of unsigned 16-bit values to an output stream. */
kzsException kzcOutputStreamWriteU16Array(struct KzcOutputStream* outputStream, kzUint numValues, const kzU16* values);
/** Writes a signed 16-bit value to an output stream. */
kzsException kzcOutputStreamWriteS16(struct KzcOutputStream* outputStream, kzS16 value);
/** Writes an array of signed 16-bit values to an output stream. */
kzsException kzcOutputStreamWriteS16Array(struct KzcOutputStream* outputStream, kzUint numValues, const kzS16* values);

/** Writes an unsigned 32-bit value to an output stream. */
kzsException kzcOutputStreamWriteU32(struct KzcOutputStream* outputStream, kzU32 value);
/** Writes an array of unsigned 32-bit values to an output stream. */
kzsException kzcOutputStreamWriteU32Array(struct KzcOutputStream* outputStream, kzUint numValues, const kzU32* values);
/** Writes a signed 32-bit value to an output stream. */
kzsException kzcOutputStreamWriteS32(struct KzcOutputStream* outputStream, kzS32 value);
/** Writes an array of signed 32-bit values to an output stream. */
kzsException kzcOutputStreamWriteS32Array(struct KzcOutputStream* outputStream, kzUint numValues, const kzS32* values);

/** Writes a 32-bit floating point value to an output stream. */
kzsException kzcOutputStreamWriteFloat(struct KzcOutputStream* outputStream, kzFloat value);
/** Writes an array of 32-bit floating point values to an output stream. */
kzsException kzcOutputStreamWriteFloatArray(struct KzcOutputStream* outputStream, kzUint numValues, const kzFloat* values);
/** Writes a 64-bit floating point value to an output stream. */
kzsException kzcOutputStreamWriteDouble(struct KzcOutputStream* outputStream, kzDouble value);


#endif
