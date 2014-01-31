/**
* \file
* Input / output stream
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_IO_STREAM_H
#define KZC_IO_STREAM_H


/** Endianness of a stream. */
enum KzcIOStreamEndianness {
    KZC_IO_STREAM_ENDIANNESS_LITTLE_ENDIAN, /**< The stream data is in little endian format. */
    KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN, /**< The stream data is in big endian format. */
    KZC_IO_STREAM_ENDIANNESS_PLATFORM, /**< The stream data is in platforms native endianness. */
    KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED /**< The stream data endianness is not specified. An error is thrown for endian specific operations. */
};

/** Specifies unknown length for stream. */
#define KZC_IO_STREAM_LENGTH_UNKNOWN -1


#endif
