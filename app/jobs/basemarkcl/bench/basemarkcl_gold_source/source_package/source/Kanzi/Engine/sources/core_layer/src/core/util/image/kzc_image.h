/**
* \file
* Image component. Handles loading and saving images.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_IMAGE_H
#define KZC_IMAGE_H


#include "kzc_etc.h"

#include <core/resource_manager/texture/kzc_texture_descriptor.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzcInputStream;
struct KzcOutputStream;
struct KzcMemoryManager;
struct KzsWindow;


/** List of data formats image may have. */
enum KzcImageDataFormat
{
    KZC_IMAGE_DATA_FORMAT_RGB_565,             /**< RGB565 format. */
    KZC_IMAGE_DATA_FORMAT_RGB_888,             /**< RGB 8 bit format. */
    KZC_IMAGE_DATA_FORMAT_RGBA_8888,           /**< RGBA 8 bit format. */
    KZC_IMAGE_DATA_FORMAT_RGB_FLOAT,           /**< RGB float format. */
    KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT,          /**< RGBA float format. */
    KZC_IMAGE_DATA_FORMAT_RGB_FLOAT16,         /**< RGB half float format. */
    KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT16,        /**< RGBA half float format. */
    KZC_IMAGE_DATA_FORMAT_ALPHA_8,             /**< Alpha 8 bit image. */
    KZC_IMAGE_DATA_FORMAT_ALPHA_FLOAT,         /**< Alpha float image. */
    KZC_IMAGE_DATA_FORMAT_GRAYSCALE_8,         /**< Grayscale 8 bit image. */
    KZC_IMAGE_DATA_FORMAT_GRAYSCALE_FLOAT,     /**< Grayscale float image. */
    KZC_IMAGE_DATA_FORMAT_ETC,                 /**< Ericsson texture compressed image. */
    KZC_IMAGE_DATA_FORMAT_DXT3,                /**< DXT3 compressed image. */
    KZC_IMAGE_DATA_FORMAT_DXT5                 /**< DXT5 compressed image. */
};

/** List of possible resizing filters. */
enum KzcImageResizeFilter
{
    KZC_IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR,   /**< Nearest neighbor filter. */
    KZC_IMAGE_RESIZE_FILTER_BILINEAR,           /**< Bilinear filter. */
    KZC_IMAGE_RESIZE_FILTER_WEIGHTED_AVERAGE    /**< Weighted average filter. */
};


/**
* \struct KzcImage
* Structure for image utility.
*/
struct KzcImage;


/** Loads image from PNG file. Color format is output in either RGB8 or RGBA8. */
kzsError kzcImageLoadPNG(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                         struct KzcImage** out_image);
/** Loads image from TGA file. Color format is output in either RGB8 or RGBA8. */
kzsError kzcImageLoadTGA(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream, 
                         struct KzcImage** out_image);
/** Loads image from JPEG file. Color format is output in RGB8. */
kzsError kzcImageLoadJPEG(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                          struct KzcImage** out_image);
/** Loads image from ETC (compressed) format. Output in compressed ETC format. */
kzsError kzcImageLoadETC(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream, 
                         struct KzcImage** out_image);
/** Loads image from memory. Data is copied. */
kzsError kzcImageLoadMemory(const struct KzcMemoryManager* memoryManager, kzUint width, kzUint height,
                            enum KzcImageDataFormat colorFormat, const void* data, struct KzcImage** out_image);
/** Creates image from data, data not copied. */
kzsError kzcImageLoadMemoryAssignData(const struct KzcMemoryManager* memoryManager, kzUint width, kzUint height,
                                      enum KzcImageDataFormat colorFormat, void* data, struct KzcImage** out_image);
/** Loads image from file. PNG, JPEG and TGA are supported. */
kzsError kzcImageLoadFile(const struct KzcMemoryManager* memoryManager, kzString fileName, struct KzcImage** out_image);

/** Creates mipmap image out of list of mipmap images. */
kzsError kzcImageCreateMipmapImage(const struct KzcMemoryManager* memoryManager, struct KzcImage** mipmapImages,
                                   struct KzcImage** out_image);


/** Deletes image. */
kzsError kzcImageDelete(struct KzcImage* image);

/** 
* Copies data from sourceImage to targetImage. 
* Data is copied from sourceImage area [sourceRectX, sourceRectY, sourceRectX + width, sourceRectY + height]
* to targetImage area [targetRectX, targetRectY, targetRectX + width, targetRectY + height]
* Error is returned if area is out of bounds of image.
*/
kzsError kzcImageCopySubImage(const struct KzcImage* targetImage, const struct KzcImage* sourceImage, 
                              kzUint targetRectX, kzUint targetRectY, kzUint sourceRectX, kzUint sourceRectY, 
                              kzUint width, kzUint height);
/** 
* Copies sourceImage data to targetImage. 
*/
kzsError kzcImageCopySubImageData(const struct KzcImage* targetImage, void* sourceData, 
                                  kzUint targetRectX, kzUint targetRectY, kzUint sourceRectX, kzUint sourceRectY,
                                  kzUint sourceImageWidth, kzUint width, kzUint height);

/**
 * Takes ownership of the image container deletion. After this, when a image is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzcImageTakeOwnership(const struct KzcImage* image);
/** Removes ownership from the image. Image itself will then be responsible of its deletion. */
void kzcImageRemoveOwnership(const struct KzcImage* image);
/** Checks if the image is valid (pointer and the data are non-null). */
kzBool kzcImageIsValid(const struct KzcImage* image);
/** Moves the data from source image to the target image. */
void kzcImageTransferData(struct KzcImage* targetImage, struct KzcImage* sourceImage);


/** Saves image to specific output stream, that is in TGA format. Output stream must be closed manually. */
kzsError kzcImageSaveTGA(const struct KzcImage* image, struct KzcOutputStream* outputStream);
/** Saves image to specific output stream, that is in PNG format. Output stream must be closed manually. */
kzsError kzcImageSavePNG(const struct KzcImage* image, struct KzcOutputStream* outputStream);
/** Saves image to specific output stream, that is in JPEG format. Output stream must be closed manually. Quality within range [0,1]*/
kzsError kzcImageSaveJPEG(const struct KzcImage* image, struct KzcOutputStream* outputStream, kzFloat quality);


/** Returns image width. */
kzUint kzcImageGetWidth(const struct KzcImage* image);
/** Returns image height. */
kzUint kzcImageGetHeight(const struct KzcImage* image);
/** Returns color format from image. */
enum KzcImageDataFormat kzcImageGetDataFormat(const struct KzcImage* image);
/** Returns image color format from texture format. */
enum KzcImageDataFormat kzcTextureFormatGetImageFormat(enum KzcTextureFormat textureFormat);
/** Returns image data from image. */
void* kzcImageGetData(const struct KzcImage* image);
/** Returns the size of the image data from image. */
kzUint kzcImageGetDataSize(const struct KzcImage* image);
/** Gets mipmap level count form image. */
kzUint kzcImageGetMipmapLevelCount(const struct KzcImage* image);
/** Gets mipmap level from image. */
struct KzcImage* kzcImageGetMipmapLevel(const struct KzcImage* image, kzUint mipmapLevelIndex);
/** Returns if this image is mipmap image. */
kzBool kzcImageIsMipmapImage(const struct KzcImage* image);


/** Copy operator for image. */
kzsError kzcImageCopy(const struct KzcMemoryManager* memoryManager, const struct KzcImage* sourceImage, struct KzcImage** out_targetImage);
/** Converts image to specified colorFormat. */
kzsError kzcImageConvert(const struct KzcImage* sourceImage, enum KzcImageDataFormat colorFormat);


/**
 * Converts image to ETC compressed format.
 * \param[in] compressionMode mode to use for compression. See kzc_etc.h for possible values.
 */
kzsError kzcImageConvertToETC(const struct KzcImage* sourceImage, enum KzcEtcCompressionMode compressionMode);


/** Sets name for the image. */
kzsError kzcImageSetName(const struct KzcImage* image, kzString name);
/** Gets name of the image. KZ_NULL if not assigned. */
kzString kzcImageGetName(const struct KzcImage* image);


/** Resizes image by specified parameters. */
kzsError kzcImageResize(const struct KzcImage* image, kzUint newWidth, kzUint newHeight, enum KzcImageResizeFilter resizeFilter);
/** Crops image by using (x, y, width, height) area. */
kzsError kzcImageCrop(const struct KzcImage* image, kzUint x, kzUint y, kzUint width, kzUint height);
/** Flips image horizontally. */
kzsError kzcImageFlipHorizontally(const struct KzcImage* image);
/** Flips image vertically. */
kzsError kzcImageFlipVertically(const struct KzcImage* image);


/** Creates set of mipmap images from source image. */
kzsError kzcImageCreateMipmapLevels(const struct KzcMemoryManager* memoryManager, const struct KzcImage* image, struct KzcImage*** out_mipmapLevels);


/** Helper function for conversions between image format and texture format. */
enum KzcTextureFormat kzcImageGetTextureFormat(const struct KzcImage* image);
/** Helper function for conversion between texture format and image format. */
enum KzcImageDataFormat kzcImageGetImageFormatFromTextureFormat(enum KzcTextureFormat format);
/** Returns is image is in compressed format. */
kzBool kzcImageIsCompressedFormat(const struct KzcImage* image);


#endif
