/**
 * \file
 * Wrapper for Ericsson texture compression. Original implementation licenced 
 * free of charge by Ericsson. See kzc_etc.c for licence.
 *
 * For technical details, see OES_compressed_ETC1_RGB8_texture extension in the OpenGL ES extensions database.
 *
 * For explanation of the compression routine see
 * Strom, Akenine-Moller:'iPACKMAN: High-Quality, Low-Complexity Texture Compression for Mobile Phones'.
 *
 * (iPackman == ETC)
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_ETC_H
#define KZC_ETC_H


#include <core/memory/kzc_memory_manager.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** ETC compression modes. */
enum KzcEtcCompressionMode
{
    KZC_ETC_COMPRESSION_MODE_FAST, /**< FAST only tests the two most likely base colors. */
    KZC_ETC_COMPRESSION_MODE_MEDIUM, /**< The MEDIUM version tests all colors in a 3x3x3 cube around the average colors. This increases the likelihood that the differential mode is selected. */
    KZC_ETC_COMPRESSION_MODE_SLOW, /**< The SLOW version tests all colors in a a 5x5x5 cube around the average colors. It also tries the nondifferential mode for each block even if the differential succeeds. */
    KZC_ETC_COMPRESSION_MODE_FAST_PERCEPTUAL, /**< FAST with PERCEPTUAL error metric. */
    KZC_ETC_COMPRESSION_MODE_MEDIUM_PERCEPTUAL, /**< MEDIUM with PERCEPTUAL error metric. */
    KZC_ETC_COMPRESSION_MODE_SLOW_PERCEPTUAL /**< SLOW with PERCEPTUAL error metric. */
};


/** 
 * Compress an rgb888 format image in imageData to compressed ETC format. Width MUST be divisible by 2 and height by 4.
 * To compress images of other sizes pad the image data to correct size first.
 * \param[in] imageData rgb888 format image data
 * \param[in] width of image in imageData in pixels
 * \param[in] height of image in imageData in pixels
 * \param[in] destinationBuffer buffer to write the compressed data into. Must be preallocated to at least correct size.
 * \param[out] out_bytesWritten number of bytes written to output
 */
kzsError kzcEtcCompressImageToBuffer(const struct KzcMemoryManager* memoryManager, const kzU8* imageData, kzInt width, kzInt height,
                                     kzU8* destinationBuffer, enum KzcEtcCompressionMode compressionMode, kzUint* out_bytesWritten);

/**
 * Uncompress a compressed rgb888 format image to destination buffer.
 * \param[in] compressedImage ETC compressed texture
 * \param[in] width of compressed texture in actual pixels
 * \param[in] height of compressed texture in actual pixels
 * \param[in] active_width of uncompressed texture. Can be less than width.
 * \param[in] active_height of uncompressed texture. Can be less than height.
 */
kzsError kzcEtcUncompressImageToBuffer(const struct KzcMemoryManager* memoryManager, const kzU8* compressedImage, kzInt width, kzInt height,
                                       kzInt active_width, kzInt active_height, kzU8* destinationImage, kzUint* out_bytesWritten);

/** Return number of bytes required for the storage of an ETC texture of the given dimensions. */
kzUint kzcEtcGetCompressedBufferMemorySize(kzUint width, kzUint height);


#endif
