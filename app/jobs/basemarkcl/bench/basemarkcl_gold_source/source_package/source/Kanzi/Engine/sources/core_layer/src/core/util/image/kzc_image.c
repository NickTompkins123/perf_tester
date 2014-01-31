/**
* \file
* Image component. Handles loading and saving images.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_image.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/util/image/kzc_etc.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/util/io/kzc_file.h>
#include <core/util/string/kzc_string.h>
#include <core/kzc_error_codes.h>

#include <system/debug/kzs_log.h>
#include <system/debug/kzs_counter.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_math.h>

/* External headers. */
#include <png.h>
#include <jpeglib.h>


/* KzcImage - Kanzi internal image loading & freeing functionality */

struct KzcImage
{
    struct KzcImageData* data;
};

struct KzcImageData
{
    kzBool selfOwned;
    kzUint width;   /**< Image's width. */
    kzUint height;  /**< Image's height. */

    kzMutableString name; /**< Name of the image. */

    enum KzcImageDataFormat dataFormat;    /**< Image's data format. */

    void* data; /**< Image raw data, type specified by data format. */

    struct KzcImage** mipmapLevels; /** Mipmap level data. */
};

/** Returns image's bytes per pixel. */
static kzUint kzcImageGetBytesPerPixel_internal(const struct KzcImage* image);
/** Checks if the data format is in U8 format. */
static kzBool kzcImageIsDataFormatByte_internal(enum KzcImageDataFormat dataFormat);
/** Checks if the data format is in float format. */
static kzBool kzcImageIsDataFormatFloat_internal(enum KzcImageDataFormat dataFormat);
/** Checks if the data format is in float16 format. */
static kzBool kzcImageIsDataFormatFloat16_internal(enum KzcImageDataFormat dataFormat);
/** Returns format's bytes per pixel. */
static kzUint kzcImageGetBytesPerPixelForFormat_internal(enum KzcImageDataFormat dataFormat);


/* PNG loading and storing utils */

/** PNG Callback function for reading from output stream. */
KZ_CALLBACK static void kzcImagePNGReadFunction_internal(png_structp png_ptr, png_bytep data, png_size_t length)
{
    kzsError result;
    struct KzcInputStream* inputStream = (struct KzcInputStream*)png_get_io_ptr(png_ptr);

    /* Callback function does not allow returning other than void. */
    result = kzcInputStreamReadBytes(inputStream, length, (kzByte*)data);
    kzsAssert(result == KZS_SUCCESS);
}

/** PNG Callback function for writing to output stream. Lint error for const parameter suppressed. */
KZ_CALLBACK static void kzcImagePNGWriteFunction_internal(png_structp png_ptr, png_bytep data, png_size_t length)
{
    kzsError result;
    struct KzcOutputStream* outputStream = (struct KzcOutputStream*)png_get_io_ptr(png_ptr);

    /* Callback function does not allow returning other than void. */
    result = kzcOutputStreamWriteBytes(outputStream, length, (kzByte*)data);
    kzsAssert(result == KZS_SUCCESS);
}

/** PNG callback function for flushing. No effect. */
KZ_CALLBACK static void kzcImagePNGFlushFunction_internal(png_structp png_ptr)
{
    /* This function has been intentionally left empty. */
}

/** PNG callback function for allocating memory. Lint warning for const usage suppressed. */
KZ_CALLBACK static png_voidp kzcImagePNGMallocFunction_internal(png_structp png_ptr, png_size_t size)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = (struct KzcMemoryManager*)png_ptr->mem_ptr;
    png_voidp allocatedBlock;

#if 0
    allocatedBlock = kzsMalloc(size);
#else
    result = kzcMemoryAllocPointer(memoryManager, &allocatedBlock, size, "PngAlloc");
    kzsAssert(result == KZS_SUCCESS);
#endif

    return allocatedBlock;
}

/** PNG callback function for freeing memory. */
KZ_CALLBACK static void kzcImagePNGFreeFunction_internal(png_structp png_ptr, png_voidp data)
{
    kzsError result;

#if 0
    kzsFree(data);
#else
    result = kzcMemoryFreePointer(data);
    kzsAssert(result == KZS_SUCCESS);
#endif
}

static kzsError kzcImageCreate_internal(const struct KzcMemoryManager* memoryManager, struct KzcImage** out_image)
{
    kzsError result;
    struct KzcImage* image;
    struct KzcImageData* imageData;

    result = kzcMemoryAllocVariable(memoryManager, image, "Image");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, imageData, "Image data");
    kzsErrorForward(result);

    image->data = imageData;
    imageData->selfOwned = KZ_TRUE;
    imageData->mipmapLevels = KZ_NULL;
    imageData->name = KZ_NULL;

    *out_image = image;
    kzsSuccess();
}

kzsError kzcImageCreateMipmapImage(const struct KzcMemoryManager* memoryManager, struct KzcImage** mipmapImages,
                                   struct KzcImage** out_image)
{
    kzsError result;
    struct KzcImage* image;

    result = kzcImageCreate_internal(memoryManager, &image);
    kzsErrorForward(result);

    image->data->mipmapLevels = mipmapImages;

    *out_image = image; 
    kzsSuccess();
}

kzsError kzcImageLoadPNG(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                         struct KzcImage** out_image)
{
    kzsError result;
    struct KzcImage* image;
    /* LibPNG specific structures. */
    png_structp png_ptr;
    png_infop info_ptr;
    png_infop end_ptr; 
    png_byte pbSig[8];
    png_uint_32 png_result;

    result = kzcImageCreate_internal(memoryManager, &image);
    kzsErrorForward(result);

    result = kzcInputStreamReadBytes(inputStream, 8, (kzByte*)pbSig);
    kzsErrorForward(result);

    /* Check that we're reading valid PNG file. */
    if(!png_check_sig(pbSig, 8))
    {
        kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Invalid PNG image (signature incorrect)!");
    }

    /* Our png_ptr is used by the libpng library to maintain, basically, state information about the PNG file as it is being read in
        by the library. It's used for housekeeping by the libpng library. 
        The other two pointers, info_ptr and end_ptr, are used to help us extract data from the PNG file. */
    /* png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL,
        (png_voidp)memoryManager, kzcImagePNGMallocFunction_internal, kzcImagePNGFreeFunction_internal); */
    /*png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);*/
    png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL,
        (png_voidp)memoryManager, kzcImagePNGMallocFunction_internal, kzcImagePNGFreeFunction_internal);
    kzsErrorTest(png_ptr != NULL, KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Invalid PNG image!");

    /* Create png info structure. */
    info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)KZ_NULL, (png_infopp)NULL);
        kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Invalid PNG image!");
    }

    end_ptr = png_create_info_struct(png_ptr);
    if(end_ptr == NULL) 
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Invalid PNG image!");
    }

    png_set_read_fn(png_ptr, (png_voidp)inputStream, kzcImagePNGReadFunction_internal);
    png_set_sig_bytes(png_ptr, 8);

    /* Read png info. */
    png_read_info(png_ptr, info_ptr);

    /* Read image info (width, height, color format) and data. */
    {
        kzInt bitDepth;
        kzInt colorType;
        kzUint channelCount;
        kzUint rowSize;

        /* Fetch image width, height, bit depth, color type. */
        png_result = png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&image->data->width, (png_uint_32*)&image->data->height, &bitDepth, &colorType, NULL, NULL, NULL);
        kzsErrorTest(png_result != 0, KZC_ERROR_IMAGE_INVALID, "PNG image has invalid header!");

        /* See how many channels this png contains. */
        channelCount = (kzUint)png_get_channels(png_ptr, info_ptr);

        /* Non-paletted image. */
        if(colorType != PNG_COLOR_TYPE_PALETTE)
        {
            /* Support only for 8 bit depth pixels. */
            if(bitDepth == 8)
            {
                switch(channelCount)
                {
                    case 1:
                    {
                        image->data->dataFormat = KZC_IMAGE_DATA_FORMAT_ALPHA_8;
                        break;
                    }
                    case 3:
                    {
                        image->data->dataFormat = KZC_IMAGE_DATA_FORMAT_RGB_888;
                        break;
                    }
                    case 4:
                    {
                        image->data->dataFormat = KZC_IMAGE_DATA_FORMAT_RGBA_8888;
                        break;
                    }
                    default:
                    {
                        kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Unsupported PNG channel format!");
                    }
                }
            }
            else
            {
                kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Image format unsupported!");
            }
        }
        /* Paletted image. */
        else
        {
            image->data->dataFormat = KZC_IMAGE_DATA_FORMAT_RGB_888;
        }

        rowSize = image->data->width * channelCount * (kzUint)bitDepth / 8;
        
        /* Create row pointers for libpng and unpack the data to new buffers, finally assigning to image. */
        {
            kzUint i;
            kzU8* imageData;
            kzU8** pngRowPointers;
            result = kzcMemoryAllocPointer(memoryManager, &imageData, sizeof(*imageData) * rowSize * image->data->height, "PngData");
            kzsErrorForward(result);

            result = kzcMemoryAllocPointer(memoryManager, &pngRowPointers, sizeof(*pngRowPointers) * image->data->height, "PngDataPointers");
            kzsErrorForward(result);

            /* Assign pointers for each row. */
            for(i = 0; i < image->data->height; ++i)
            {
                pngRowPointers[i] = imageData + i * rowSize;
            }

            /* Read image data. */
            png_read_image(png_ptr, (png_bytepp)pngRowPointers);
            png_read_end(png_ptr, NULL);

            /* Assign unpacked data for image. */
            if(colorType != PNG_COLOR_TYPE_PALETTE)
            {
                image->data->data = imageData;
            }
            
            result = kzcMemoryFreePointer(pngRowPointers);
            kzsErrorForward(result);

            /* Convert to RGB data in case of paletted png. */
            if (colorType == PNG_COLOR_TYPE_PALETTE)
            {
                png_colorp palette;
                kzInt numPalette;
                kzU8* convertedImage;

                png_result = png_get_PLTE(png_ptr, info_ptr, &palette, &numPalette);
                if(png_result == 0)
                {
                    kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Invalid PNG palette!");
                }

                result = kzcMemoryAllocPointer(memoryManager, &convertedImage, image->data->width * image->data->height * 3 * sizeof(*convertedImage), "PngTempPixels");
                kzsErrorForward(result);

                /* 4-bit palette, assign 2 pixels at a time. */
                if(bitDepth == 4)
                {
                    kzUint offset = 0;
                    kzUint imageSize = image->data->width * channelCount * (kzUint)bitDepth * image->data->height / 8;

                    /* If both width and height are odd, no support for loading. */
                    if((image->data->width & 1) != 0 && (image->data->height & 1) != 0)
                    {
                        kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "4bit paletted png image with width and height having odd values not supported!");
                    }

                    for(i = 0; i < imageSize; ++i)
                    {
                        kzUint highByte = (kzUint)(imageData[i] >> 4);
                        kzUint lowByte = (kzUint)(imageData[i] & 0x0F);
                        convertedImage[offset++] =     (kzU8)palette[highByte].red;
                        convertedImage[offset++] = (kzU8)palette[highByte].green;
                        convertedImage[offset++] = (kzU8)palette[highByte].blue;
                        convertedImage[offset++] = (kzU8)palette[lowByte].red;
                        convertedImage[offset++] = (kzU8)palette[lowByte].green;
                        convertedImage[offset++] = (kzU8)palette[lowByte].blue;
                    }
                }
                /* 8 bit palette, assign colors by fetching from palette. */
                else if(bitDepth == 8)
                {
                    kzUint offset = 0;
                    kzUint imageSize = rowSize * image->data->height;
                    for(i = 0; i < imageSize; i++)
                    {
                        kzU8 colorValue = imageData[i];
                        convertedImage[offset++] = (kzU8)palette[colorValue].red;
                        convertedImage[offset++] = (kzU8)palette[colorValue].green;
                        convertedImage[offset++] = (kzU8)palette[colorValue].blue;
                    }
                }
                else
                {
                    kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Invalid paletted image format!");
                }

                result = kzcMemoryFreePointer(imageData);
                kzsErrorForward(result);

                image->data->data = convertedImage;
            }
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);

    *out_image = image;
    kzsSuccess();
}


kzsError kzcImageSavePNG(const struct KzcImage* image, struct KzcOutputStream* outputStream)
{
    kzsError result;
    png_structp png_ptr;
    png_infop info_ptr;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcImageIsValid(image));
    kzsAssert(image->data->dataFormat == KZC_IMAGE_DATA_FORMAT_RGB_888); /* Not implemented for other types */
    memoryManager = kzcMemoryGetManager(image);

    /* Create the write struct for png. */
    png_ptr = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL,
        (png_voidp)memoryManager, kzcImagePNGMallocFunction_internal, kzcImagePNGFreeFunction_internal);
    if(png_ptr == NULL)
    {
        kzsErrorThrow(KZC_ERROR_IMAGE_WRITE_ERROR, "Error in writing PNG image!");
    }
    /* Create the info struct for png. Info struct contains info such as width, height, bit depth and other attributes of image. */
    info_ptr = png_create_info_struct (png_ptr);
    if(info_ptr == NULL)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        kzsErrorThrow(KZC_ERROR_IMAGE_WRITE_ERROR, "Error in writing PNG image!");
    }

    png_set_write_fn(png_ptr, (png_voidp)outputStream, kzcImagePNGWriteFunction_internal, kzcImagePNGFlushFunction_internal);
    /* Set png to non-interlaced, non-compressed RGB mode. */
    png_set_IHDR(png_ptr, info_ptr, (png_uint_32)image->data->width, (png_uint_32)image->data->height,
        8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);
    /* Write the actual data. */
    {
        kzUint i;
        kzU8** pngRowPointers;
        /* RGB image, hence * 3 (3 channels) */
        kzUint rowSize = image->data->width * 3;

        result = kzcMemoryAllocPointer(memoryManager, &pngRowPointers, sizeof(*pngRowPointers) * image->data->height, "PngDataPointers");
        kzsErrorForward(result);

        /* Assign pointers for each row. */
        for(i = 0; i < image->data->height; ++i)
        {
            pngRowPointers[i] = (kzU8*)image->data->data + i * rowSize;
        }

        png_write_image(png_ptr, (png_bytepp)pngRowPointers);
        png_write_end(png_ptr, NULL);

        png_destroy_write_struct(&png_ptr, &info_ptr);

        result = kzcMemoryFreePointer(pngRowPointers);
        kzsErrorForward(result);
    }

    kzsSuccess();
}


/* TGA loading and storing utils */

kzsError kzcImageLoadTGA(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream, 
                         struct KzcImage** out_image)
{
    KZS_NOT_IMPLEMENTED_YET_ERROR;
}

kzsError kzcImageSaveTGA(const struct KzcImage* image, struct KzcOutputStream* outputStream)
{
    kzsAssert(kzcImageIsValid(image));
    kzsAssert(kzcIsValidPointer(outputStream));

    KZS_NOT_IMPLEMENTED_YET_ERROR;
}


/* JPEG loading and storing utils */

struct KzcJPEGDecodeStruct_internal
{
    struct jpeg_source_mgr smgr;
    struct KzcInputStream* inputStream;
    JOCTET* buffer;
};

/* Helper structure used for JPEG compressing. */
struct KzcJPEGCompressStruct_internal
{
    struct jpeg_destination_mgr dmgr;   /**< LibJPEG destination manager. */
    struct KzcOutputStream* outputStream;   /**< Kanzi output stream. */
    JOCTET* buffer;                         /**< Buffer containing compressed data. */
};

/* JPEG error management struct; custom extension stores a setjmp context for returning from exception */
struct KzcJPEGErrorManager {
    struct jpeg_error_mgr pub; /* "public" fields */
    jmp_buf setjmp_buffer; /* for "try-catch" error handling */
};

/* Kanzi replacement for the standard JPEG error_exit method */
KZ_CALLBACK static void kzcImageJPEGErrorHandler(j_common_ptr cinfo);
/* Kanzi replacement for the standard JPEG output_message method */
KZ_CALLBACK static void kzcImageJPEGErrorMessage(j_common_ptr cinfo);

/** Maximum buffer size for JPEG compress buffers. */
static const kzUint KZU_IMAGE_JPEG_COMPRESS_MAXIMUM_BUFFER_SIZE = 256;

/** JPEGlib decode callback function. */
KZ_CALLBACK static void kzcImageJPEGDecodeInitFunction_internal(j_decompress_ptr cinfo)
{
    struct KzcJPEGDecodeStruct_internal* decodeStruct = (struct KzcJPEGDecodeStruct_internal*)cinfo->src;

    decodeStruct->buffer = (JOCTET*)(*cinfo->mem->alloc_small)((j_common_ptr)(void*)cinfo, 
        JPOOL_IMAGE, KZU_IMAGE_JPEG_COMPRESS_MAXIMUM_BUFFER_SIZE * sizeof(JOCTET));
    decodeStruct->smgr.bytes_in_buffer = 0;
}

/* JPEGlib call back function for filling input buffer while decoding. Lint error for constant parameter suppressed. */
KZ_CALLBACK static boolean kzcImageJPEGDecodeFillInputBufferFunction_internal(j_decompress_ptr cinfo)
{
    kzsError result;
    kzUint bytesRead;
    struct KzcJPEGDecodeStruct_internal* decodeStruct = (struct KzcJPEGDecodeStruct_internal*)cinfo->src;
    
    result = kzcInputStreamRead(decodeStruct->inputStream, KZU_IMAGE_JPEG_COMPRESS_MAXIMUM_BUFFER_SIZE, &bytesRead, (kzByte*)decodeStruct->buffer);
    kzsAssert(result == KZS_SUCCESS || result == KZC_EXCEPTION_END_OF_STREAM);

    decodeStruct->smgr.next_input_byte = decodeStruct->buffer;
    decodeStruct->smgr.bytes_in_buffer = bytesRead;

    return TRUE;
}

/** Internal callback function for skipping input data while decoding JPEG data. Suppress the usage of long and const. */
/*lint -e{970}*/
KZ_CALLBACK static void kzcImageJPEGDecodeSkipInputData_internal(j_decompress_ptr cinfo, long num_bytes)
{
    struct KzcJPEGDecodeStruct_internal* decodeStruct = (struct KzcJPEGDecodeStruct_internal*)cinfo->src;

    if(num_bytes > 0)
    {
        /* Skip buffer until done, avoiding that bytes in buffer never gets below zero. */
        while(num_bytes > (long)decodeStruct->smgr.bytes_in_buffer)
        {
            num_bytes -= (long)decodeStruct->smgr.bytes_in_buffer;
            kzcImageJPEGDecodeFillInputBufferFunction_internal(cinfo); /*lint !e534 Return value ignored. */
        }
    }
    decodeStruct->smgr.next_input_byte += (size_t)num_bytes;
    decodeStruct->smgr.bytes_in_buffer -= (size_t)num_bytes;
}

/** JPEGlib call back function for terminating source while decoding. */
KZ_CALLBACK static void kzcImageJPEGDecodeTerminateSource_internal(j_decompress_ptr cinfo)
{
}

/** JPEGlib callback function for initializing destination while compressing. */
KZ_CALLBACK static void kzcImageJPEGCompressInitDestination_internal(j_compress_ptr cinfo)
{
    struct KzcJPEGCompressStruct_internal* compressStruct = (struct KzcJPEGCompressStruct_internal*)cinfo->dest;

    compressStruct->buffer = (JOCTET*)(*cinfo->mem->alloc_small)((j_common_ptr)(void*)cinfo, 
        JPOOL_IMAGE, KZU_IMAGE_JPEG_COMPRESS_MAXIMUM_BUFFER_SIZE * sizeof(JOCTET));
    compressStruct->dmgr.next_output_byte = (JOCTET*)compressStruct->buffer;
    compressStruct->dmgr.free_in_buffer = KZU_IMAGE_JPEG_COMPRESS_MAXIMUM_BUFFER_SIZE;
}

/* JPEGlib callback function for emptying buffer while compressing. Constant function parameters suppressed from lint. */
KZ_CALLBACK static boolean kzcImageJPEGCompressEmptyBuffer_internal(j_compress_ptr cinfo)
{
    kzsError result;
    struct KzcJPEGCompressStruct_internal* compressStruct = (struct KzcJPEGCompressStruct_internal*)cinfo->dest;
    result = kzcOutputStreamWriteBytes(compressStruct->outputStream, KZU_IMAGE_JPEG_COMPRESS_MAXIMUM_BUFFER_SIZE, (kzByte*)compressStruct->buffer);
    kzsAssert(result == KZS_SUCCESS);

    compressStruct->dmgr.next_output_byte = compressStruct->buffer;
    compressStruct->dmgr.free_in_buffer = KZU_IMAGE_JPEG_COMPRESS_MAXIMUM_BUFFER_SIZE;

    return TRUE;
}

/* JPEGlib callback function for terminating destination buffer while compressing. Constant function parameters suppressed from lint. */
KZ_CALLBACK static void kzcImageJPEGCompressTermDestination_internal(j_compress_ptr cinfo)
{
    kzsError result;
    struct KzcJPEGCompressStruct_internal* compressStruct = (struct KzcJPEGCompressStruct_internal*)cinfo->dest;
    kzUint size = KZU_IMAGE_JPEG_COMPRESS_MAXIMUM_BUFFER_SIZE - compressStruct->dmgr.free_in_buffer;

    /* If there are remaining bytes to be written, write them. */
    if(size > 0)
    {
        result = kzcOutputStreamWriteBytes(compressStruct->outputStream, size, (kzByte*)compressStruct->buffer);
        kzsAssert(result == KZS_SUCCESS);
    }
}

/* Kanzi replacement for the standard error_exit method */
KZ_CALLBACK static void kzcImageJPEGErrorHandler(j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    struct KzcJPEGErrorManager *myerr = (struct KzcJPEGErrorManager *)cinfo->err;

    /* Display the message. */
    /* TODO: instead of / in addition to logging, pass the message over the 
                longjmp so that it can be thrown with the kzsErrorThrow */
    /* (*cinfo->err->output_message) (cinfo); */
    kzcImageJPEGErrorMessage(cinfo);

    /* Return control to the setjmp point, or "catch block" in kzcImageLoadJPEG */
    longjmp(myerr->setjmp_buffer, 1);
}

/* Kanzi replacement for the standard output_message method */
KZ_CALLBACK static void kzcImageJPEGErrorMessage(j_common_ptr cinfo)
{
  kzChar buffer[JMSG_LENGTH_MAX];

  /* Show a clear-text error message */
  (*cinfo->err->format_message) (cinfo, buffer);  
  kzsLog(KZS_LOG_LEVEL_WARNING, buffer);
}

kzsError kzcImageLoadJPEG(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream,
                          struct KzcImage** out_image)
{
    kzsError result;
    struct KzcImage* image;
    struct jpeg_decompress_struct cinfo;
    struct KzcJPEGDecodeStruct_internal* decodeStruct;
    struct KzcJPEGErrorManager jerr;
    kzInt jpegError;

    /* We set up the normal JPEG error routines, then override error_exit. */
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = kzcImageJPEGErrorHandler;
    jerr.pub.output_message = kzcImageJPEGErrorMessage;
    
    /* Establish the setjmp return context; "catch block" for "exceptions" during decompression */
    if (setjmp(jerr.setjmp_buffer))
    {
        /* If we get here, the JPEG code has signaled an error. */
        /* Clean up the JPEG object, close the input file, and relay the error forward. */
        /* TODO: use error message from JPEG */
        jpeg_destroy_decompress(&cinfo);        
        kzsErrorThrow(KZC_ERROR_IMAGE_INVALID, "Error while loading a JPEG image!");
    }

    /* create decompression info struct and kanzi image struct */
    jpeg_create_decompress(&cinfo);
    result = kzcImageCreate_internal(memoryManager, &image);
    kzsErrorForward(result);
    
    /* Create and initialize the decode structure with function pointers and pass the input stream. */
    result = kzcMemoryAllocVariable(memoryManager, decodeStruct, "DecodeStruct");
    kzsErrorForward(result);
    decodeStruct->inputStream = inputStream;
    decodeStruct->smgr.init_source = kzcImageJPEGDecodeInitFunction_internal;
    decodeStruct->smgr.fill_input_buffer = kzcImageJPEGDecodeFillInputBufferFunction_internal;
    decodeStruct->smgr.skip_input_data = kzcImageJPEGDecodeSkipInputData_internal;
    decodeStruct->smgr.resync_to_restart = jpeg_resync_to_restart;
    decodeStruct->smgr.term_source = kzcImageJPEGDecodeTerminateSource_internal;

    /* Assign our decode struct for source manager of decompress info. */
    cinfo.src = (struct jpeg_source_mgr*)decodeStruct;

    /* Read the header, may "throw exceptions" */
    jpegError = (kzInt)jpeg_read_header(&cinfo, TRUE);
    kzsErrorTest(jpegError == JPEG_HEADER_OK, KZC_ERROR_IMAGE_INVALID, "Invalid JPEG header observed!");
    kzsErrorTest(cinfo.out_color_space == JCS_RGB, KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Unsupported JPEG output format!");

    /* Assign image characteristics. */
    image->data->width = (kzUint)cinfo.image_width;
    image->data->height = (kzUint)cinfo.image_height;
    image->data->dataFormat = KZC_IMAGE_DATA_FORMAT_RGB_888;

    /* Read the jpeg data to image pixel array. Setup row pointers and let libjpeg handle the reading. */
    {
        kzUint rowSize;
        kzU8** rowPointers;
        kzU8* imageData;
        kzUint i;

        /* Calculate row size, since RGB is assumed using value of 3. */
        rowSize = image->data->width * 3;

        result = kzcMemoryAllocPointer(memoryManager, &rowPointers, sizeof(*rowPointers) * image->data->height, "RowPointers");
        kzsErrorForward(result);

        /* Allocate memory for image data. */
        result = kzcMemoryAllocPointer(memoryManager, &imageData, image->data->width * image->data->height * 3, "ImageDataJpeg");
        kzsErrorForward(result);

        /* Assign pointers for each row. */
        for(i = 0; i < image->data->height; ++i)
        {
            rowPointers[i] = imageData + i * rowSize;
        }

        /* Prepare jpeg decompression. */
        if(!jpeg_start_decompress(&cinfo))
        {
            kzsErrorThrow(KZC_ERROR_IMAGE_INVALID, "Error in decompressing JPEG image!");
        }

        /* Read scan lines, data assigned via row pointers. */
        while(cinfo.output_scanline < cinfo.output_height)
        {
            kzInt rowsRead;
            rowsRead = (kzInt)jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&rowPointers[cinfo.output_scanline], 1);
            kzsErrorTest(rowsRead > 0, KZC_ERROR_IMAGE_INVALID, "No rows were able to be read from JPEG image!");
        }
        /* Assign data. */
        image->data->data = imageData;

        /* Free row pointers from memory. */
        result = kzcMemoryFreePointer(rowPointers);
        kzsErrorForward(result);
    }

    /* Finish decompressing. */
    if(!jpeg_finish_decompress(&cinfo))
    {
        kzsErrorThrow(KZC_ERROR_IMAGE_INVALID, "JPEG decompression error in finishing!");
    }

    /* Free source manager helper structure. */
    result = kzcMemoryFreeVariable(decodeStruct);
    kzsErrorForward(result);

    /* Destroy decompress structure. */
    jpeg_destroy_decompress(&cinfo);

    *out_image = image;
    kzsSuccess();
}

kzsError kzcImageSaveJPEG(const struct KzcImage* image, struct KzcOutputStream* outputStream, kzFloat quality)
{
    kzsError result;
    struct KzcImage* convertedImage;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    struct KzcJPEGCompressStruct_internal* compressStruct;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcImageIsValid(image));
    kzsAssert(kzcIsValidPointer(outputStream));
    kzsErrorTest(kzsIsInClosedInterval(quality, 0.0f, 1.0f), KZC_ERROR_IMAGE_WRITE_ERROR, "JPEG compression quality value should be in range 0.0f...1.0f!");

    memoryManager = kzcMemoryGetManager(image);

    /* TODO: do not use default jpeg error handler since it automatically calls exit() internally upon failure */
    /* Set the error manager. */
    cinfo.err = jpeg_std_error(&jerr);
    /* Create compression structure. */
    jpeg_create_compress(&cinfo);

    /* Allocate memory for destination manager. */
    result = kzcMemoryAllocVariable(memoryManager, compressStruct, "JpegCompressStruct");
    kzsErrorForward(result);

    /* Set helper struct for compression structure. */
    cinfo.dest = (struct jpeg_destination_mgr*)compressStruct;

    /** Setup write parameters for image. */
    compressStruct->outputStream = outputStream;
    compressStruct->dmgr.init_destination = kzcImageJPEGCompressInitDestination_internal;
    compressStruct->dmgr.empty_output_buffer = kzcImageJPEGCompressEmptyBuffer_internal;
    compressStruct->dmgr.term_destination = kzcImageJPEGCompressTermDestination_internal;
    
    /* Prepare for compression. Ignore alpha channel.*/
    {
        result = kzcImageCopy(memoryManager, image, &convertedImage);
        kzsErrorForward(result);

        result = kzcImageConvert(convertedImage, KZC_IMAGE_DATA_FORMAT_RGB_888);
        kzsErrorForward(result);
    }


    cinfo.image_width = (JDIMENSION)convertedImage->data->width;
    cinfo.image_height = (JDIMENSION)convertedImage->data->height;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;

    /* Set JPEG default after the color space has been defined. */
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, (kzInt)(quality * 100.0f), TRUE);
    jpeg_start_compress(&cinfo, 1);
    
    /* Write the JPEG scan lines, one by one. */
    {
        JSAMPROW sampleRow[1];
        JDIMENSION rowsWritten;

        while(cinfo.next_scanline < cinfo.image_height)
        {
            kzU8* imageRow = (kzU8*)convertedImage->data->data + convertedImage->data->width * 3 * (kzUint)cinfo.next_scanline;

            sampleRow[0] = (JSAMPROW)&imageRow[0];
            rowsWritten = jpeg_write_scanlines(&cinfo, (JSAMPARRAY)sampleRow, 1);
            if(rowsWritten != 1)
            {
                kzsErrorThrow(KZC_ERROR_IMAGE_WRITE_ERROR, "Invalid amount of rows written when outputting JPEG image!");
            }
        }
    }

    /* Release helper structure from memory. */
    result = kzcMemoryFreePointer(compressStruct);
    kzsErrorForward(result);

    /* Finish compression. */
    jpeg_finish_compress(&cinfo);
    /* Destroy compression structure. */
    jpeg_destroy_compress(&cinfo);

    result = kzcImageDelete(convertedImage);
    kzsErrorForward(result);

    kzsSuccess();
}


/* KzcImage utils - manipulation and getters. */

kzsError kzcImageLoadETC(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream, 
                         struct KzcImage** out_image)
{
    KZS_NOT_IMPLEMENTED_YET_ERROR;
}

kzsError kzcImageLoadMemory(const struct KzcMemoryManager* memoryManager, kzUint width, kzUint height, enum KzcImageDataFormat colorFormat,
                             const void* data, struct KzcImage** out_image)
{
    kzsError result;
    struct KzcImage* image;
    kzUint imagePixelSize;
    kzUint imageDataSize;

    result = kzcImageCreate_internal(memoryManager, &image);
    kzsErrorForward(result);

    image->data->width = width;
    image->data->height = height;
    image->data->dataFormat = colorFormat;

    switch(image->data->dataFormat)
    {
        case KZC_IMAGE_DATA_FORMAT_ETC:
        {
            /* ETC uses 8 bytes to represent 4x4 = 16 pixel blocks */
            imageDataSize = image->data->width * image->data->height / 2;
            imageDataSize = kzsMaxU(8, imageDataSize);
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_DXT3:
        case KZC_IMAGE_DATA_FORMAT_DXT5:
        {
            imageDataSize = image->data->width * image->data->height + 32 * 4;
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_RGB_565:
        case KZC_IMAGE_DATA_FORMAT_RGB_888:
        case KZC_IMAGE_DATA_FORMAT_RGBA_8888:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT16:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT16:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_8:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_8:
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_FLOAT:
        default:
        {
            imagePixelSize = kzcImageGetBytesPerPixel_internal(image);
            if(kzcImageIsDataFormatFloat_internal(image->data->dataFormat))
            {
                imagePixelSize *= 4;
            }
            else if(kzcImageIsDataFormatFloat16_internal(image->data->dataFormat))
            {
                imagePixelSize *= 2;
            }
            imageDataSize = image->data->width * image->data->height * imagePixelSize;
            break;
        }
    }

    result = kzcMemoryAllocPointer(memoryManager, &image->data->data, imageDataSize, "ImageData");
    kzsErrorForward(result);

    kzsMemcpy(image->data->data, data, imageDataSize);

    *out_image = image;
    kzsSuccess();
}

kzsError kzcImageLoadMemoryAssignData(const struct KzcMemoryManager* memoryManager, kzUint width, kzUint height,
                                      enum KzcImageDataFormat colorFormat, void* data, struct KzcImage** out_image)
{
    kzsError result;
    struct KzcImage* image;

    result = kzcImageCreate_internal(memoryManager, &image);
    kzsErrorForward(result);

    image->data->width = width;
    image->data->height = height;
    image->data->dataFormat = colorFormat;

    image->data->data = data;

    *out_image = image;
    kzsSuccess();
}

/* TODO: Rename to kzcImageLoadResource and provide also function for loading from file. */
kzsError kzcImageLoadFile(const struct KzcMemoryManager* memoryManager, kzString fileName, struct KzcImage** out_image)
{
    kzsError result;
    struct KzcInputStream* inputStream;
    struct KzcImage* image;
    kzMutableString extension;
    kzMutableString parsedExtension;

    result = kzcInputStreamCreateFromResource(memoryManager, fileName, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &inputStream);
    kzsErrorForward(result);

    result = kzcFileGetExtension(memoryManager, fileName, &extension);
    kzsErrorForward(result);

    result = kzcStringAsciiToLowerCase(memoryManager, extension, &parsedExtension);
    kzsErrorForward(result);

    if (kzcStringIsEqual(parsedExtension, "png"))
    {
        result = kzcImageLoadPNG(memoryManager, inputStream, &image);
        kzsErrorForward(result);
    }
    else if (kzcStringIsEqual(parsedExtension, "jpg") || kzcStringIsEqual(parsedExtension, "jpeg"))
    {
        result = kzcImageLoadJPEG(memoryManager, inputStream, &image);
        kzsErrorForward(result);
    }
    else
    {
        kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Image format unsupported!");
    }

    result = kzcStringDelete(parsedExtension);
    kzsErrorForward(result);

    result = kzcStringDelete(extension);
    kzsErrorForward(result);

    result = kzcInputStreamDelete(inputStream);
    kzsErrorForward(result);

    *out_image = image;
    kzsSuccess();
}

kzsError kzcImageDelete(struct KzcImage* image)
{
    kzsError result;
    struct KzcImageData* imageData;
    kzBool selfOwned;

    kzsAssert(kzcImageIsValid(image));

    imageData = image->data;
    selfOwned = imageData->selfOwned;

    if(kzcImageIsMipmapImage(image))
    {
        kzUint i;
        for(i = 0; i < kzcArrayLength(imageData->mipmapLevels); ++i)
        {
            result = kzcImageDelete(imageData->mipmapLevels[i]);
            kzsErrorForward(result);
        }

        result = kzcMemoryFreeArray(imageData->mipmapLevels);
        kzsErrorForward(result);
    }
    else
    {
        kzsAssert(kzcIsValidPointer(imageData->data));

        result = kzcMemoryFreePointer(imageData->data);
        kzsErrorForward(result);
    }

    if(image->data->name != KZ_NULL)
    {
        result = kzcStringDelete(image->data->name);
        kzsErrorForward(result);
        image->data->name = KZ_NULL;
    }

    result = kzcMemoryFreeVariable(imageData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(image);
        kzsErrorForward(result);
    }
    else
    {
        image->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzcImageCopySubImageData(const struct KzcImage* targetImage, void* sourceData, 
                                  kzUint targetRectX, kzUint targetRectY, kzUint sourceRectX, kzUint sourceRectY,
                                  kzUint sourceImageWidth, kzUint width, kzUint height)
{
    kzUint horizontalLineLength;
    kzUint bytesPerPixel;

    kzsAssert(kzcImageIsValid(targetImage));

    bytesPerPixel = kzcImageGetBytesPerPixel_internal(targetImage);
    horizontalLineLength = width * bytesPerPixel;

    /* Byte format. */
    if(kzcImageIsDataFormatByte_internal(targetImage->data->dataFormat))
    {
        kzU8* targetBuffer = (kzU8*)targetImage->data->data;
        kzU8* sourceBuffer = (kzU8*)sourceData;
        kzUint j;
        for(j = 0; j < height; ++j)
        {
            kzUint yOffsetTarget = ((targetRectY + j) * targetImage->data->width + targetRectX) * bytesPerPixel;
            kzUint yOffsetSource = ((sourceRectY + j) * sourceImageWidth+ sourceRectX) * bytesPerPixel;
            kzsMemcpy(&targetBuffer[yOffsetTarget], &sourceBuffer[yOffsetSource], horizontalLineLength);
        } 
    }
    /* Float format. */
    else if(kzcImageIsDataFormatFloat_internal(targetImage->data->dataFormat))
    {
        kzFloat* targetBuffer = (kzFloat*)targetImage->data->data;
        kzFloat* sourceBuffer = (kzFloat*)sourceData;
        kzUint j;
        for(j = 0; j < height; ++j)
        {
            kzUint yOffsetTarget = ((targetRectY + j) * targetImage->data->width + targetRectX) * bytesPerPixel;
            kzUint yOffsetSource = ((sourceRectY + j) * sourceImageWidth + sourceRectX) * bytesPerPixel;
            kzsMemcpy(&targetBuffer[yOffsetTarget], &sourceBuffer[yOffsetSource], horizontalLineLength);
        }
    }
    else
    {
        kzsErrorThrow(KZC_ERROR_IMAGE_MANIPULATION_ERROR, "Unsupported format for image flipping!");
    }

    kzsSuccess();
}


kzsError kzcImageCopySubImage(const struct KzcImage* targetImage, const struct KzcImage* sourceImage, 
                              kzUint targetRectX, kzUint targetRectY, kzUint sourceRectX, kzUint sourceRectY, 
                              kzUint width, kzUint height)
{
    kzsError result;

    kzsAssert(kzcImageIsValid(sourceImage));
    kzsAssert(kzcImageIsValid(targetImage));

    if((sourceRectX + width) > sourceImage->data->width || (sourceRectY + height) > sourceImage->data->height)
    {
        kzsErrorThrow(KZC_ERROR_IMAGE_MANIPULATION_ERROR, "Source image area parameters out of image bounds!");
    }

    if((targetRectX + width) > targetImage->data->width || (targetRectY + height) > targetImage->data->height)
    {
        kzsErrorThrow(KZC_ERROR_IMAGE_MANIPULATION_ERROR, "Target image area parameters out of image bounds!");
    }

    /* TODO: implement. */
    if(kzcImageGetBytesPerPixel_internal(sourceImage) != kzcImageGetBytesPerPixel_internal(targetImage))
    {
        KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("SubImage copy is implemented only for images of same data format.");
    }

    result = kzcImageCopySubImageData(targetImage, kzcImageGetData(sourceImage),
        targetRectX, targetRectY, sourceRectX, sourceRectY,
        kzcImageGetWidth(sourceImage), width, height);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcImageTakeOwnership(const struct KzcImage* image)
{
    kzsAssert(kzcImageIsValid(image));

    image->data->selfOwned = KZ_FALSE;
}

void kzcImageRemoveOwnership(const struct KzcImage* image)
{
    kzsAssert(kzcImageIsValid(image));
    image->data->selfOwned = KZ_TRUE;
}

kzBool kzcImageIsValid(const struct KzcImage* image)
{
    return image != KZ_NULL && image->data != KZ_NULL;
}

void kzcImageTransferData(struct KzcImage* targetImage, struct KzcImage* sourceImage)
{
    kzsAssert(kzcIsValidPointer(targetImage));
    kzsAssert(targetImage->data == KZ_NULL);
    kzsAssert(kzcImageIsValid(sourceImage));

    targetImage->data = sourceImage->data;
    sourceImage->data = KZ_NULL;
}

void* kzcImageGetData(const struct KzcImage* image)
{
    void* data;
    kzsAssert(kzcImageIsValid(image));
    if(kzcImageIsMipmapImage(image))
    {
        data = kzcImageGetData(image->data->mipmapLevels[0]);
    }
    else
    {
        data = image->data->data;
    }

    return data;
}

kzUint kzcImageGetDataSize(const struct KzcImage* image)
{
    kzUint dataSize;
    kzsAssert(kzcImageIsValid(image));
    if(kzcImageIsMipmapImage(image))
    {
        dataSize = kzcImageGetDataSize(image->data->mipmapLevels[0]);
    }
    else
    {
        if(kzcImageGetDataFormat(image) == KZC_IMAGE_DATA_FORMAT_ETC)
        {
            dataSize = kzsMaxU(8, image->data->width * image->data->height / 2);
        }
        else if(kzcImageGetDataFormat(image) == KZC_IMAGE_DATA_FORMAT_DXT3)
        {
            dataSize = image->data->width * image->data->height + 32 * 4;
        }
        else if(kzcImageGetDataFormat(image) == KZC_IMAGE_DATA_FORMAT_DXT5)
        {
            dataSize = image->data->width * image->data->height + 32 * 4;
        }
        else
        {
            dataSize = kzcImageGetBytesPerPixel_internal(image) * image->data->width * image->data->height;
        }
    }
    return dataSize;
}

kzUint kzcImageGetWidth(const struct KzcImage* image)
{
    kzUint width;
    kzsAssert(kzcImageIsValid(image));
    if(kzcImageIsMipmapImage(image))
    {
        width = kzcImageGetWidth(image->data->mipmapLevels[0]);
    }
    else
    {
        width = image->data->width;
    }

    return width;
}

kzUint kzcImageGetHeight(const struct KzcImage* image)
{
    kzUint height;
    kzsAssert(kzcImageIsValid(image));
    if(kzcImageIsMipmapImage(image))
    {
        height = kzcImageGetHeight(image->data->mipmapLevels[0]);
    }
    else
    {
        height = image->data->height;
    }

    return height;
}

kzBool kzcImageIsCompressedFormat(const struct KzcImage* image)
{
    kzBool isCompressed = KZ_FALSE;
    switch(image->data->dataFormat)
    {
        case KZC_IMAGE_DATA_FORMAT_ETC:
        case KZC_IMAGE_DATA_FORMAT_DXT3:
        case KZC_IMAGE_DATA_FORMAT_DXT5:
        {
            isCompressed = KZ_TRUE;
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_RGB_565:
        case KZC_IMAGE_DATA_FORMAT_RGB_888:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT16:
        case KZC_IMAGE_DATA_FORMAT_RGBA_8888:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT16:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_8:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_8:
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_FLOAT:
        default:
        {
            isCompressed = KZ_FALSE;
            break;
        }
    }
    return isCompressed;
}

enum KzcImageDataFormat kzcImageGetDataFormat(const struct KzcImage* image)
{
    enum KzcImageDataFormat dataFormat;
    kzsAssert(kzcImageIsValid(image));
    if(kzcImageIsMipmapImage(image))
    {
        dataFormat = kzcImageGetDataFormat(image->data->mipmapLevels[0]);
    }
    else
    {
        dataFormat = image->data->dataFormat;
    }

    return dataFormat;
}

kzUint kzcImageGetMipmapLevelCount(const struct KzcImage* image)
{
    kzsAssert(kzcImageIsValid(image));
    return kzcArrayLength(image->data->mipmapLevels);
}

struct KzcImage* kzcImageGetMipmapLevel(const struct KzcImage* image, kzUint mipmapLevelIndex)
{
    kzsAssert(kzcImageIsValid(image));
    return image->data->mipmapLevels[mipmapLevelIndex];
}

kzBool kzcImageIsMipmapImage(const struct KzcImage* image)
{
    kzsAssert(kzcImageIsValid(image));
    return image->data->mipmapLevels != KZ_NULL;
}

kzsError kzcImageCopy(const struct KzcMemoryManager* memoryManager, const struct KzcImage* sourceImage, struct KzcImage** out_targetImage)
{
    kzsError result;
    struct KzcImage* targetImage;

    kzsAssert(kzcImageIsValid(sourceImage));

    result = kzcImageLoadMemory(memoryManager, sourceImage->data->width, sourceImage->data->height, sourceImage->data->dataFormat, sourceImage->data->data, &targetImage);
    kzsErrorForward(result);

    kzsCounterIncrease("ImageCopy");

    *out_targetImage = targetImage;
    kzsSuccess();
}

KZ_INLINE void kzcImageSetPixelComponents8bit_internal(kzU8* pixelData, enum KzcImageDataFormat dataFormat, kzU8 red8bit,
                                                    kzU8 green8bit,  kzU8 blue8bit, kzU8 alpha8bit)
{
    if(dataFormat == KZC_IMAGE_DATA_FORMAT_RGB_888)
    {
        pixelData[0] = red8bit;
        pixelData[1] = green8bit;
        pixelData[2] = blue8bit;
    }
    else if(dataFormat == KZC_IMAGE_DATA_FORMAT_RGBA_8888)
    {
        pixelData[0] = red8bit;
        pixelData[1] = green8bit;
        pixelData[2] = blue8bit;
        pixelData[3] = alpha8bit;
    }
    else if(dataFormat == KZC_IMAGE_DATA_FORMAT_RGB_565)
    {
        kzU8 red5bit = red8bit >> 3;
        kzU8 green6bit = green8bit >> 2;
        kzU8 blue5bit = blue8bit >> 3;
        kzUint shortValue = (kzUint)((red5bit << 11) | (green6bit << 5) | blue5bit);

        pixelData[0] = (kzU8)(shortValue >> 8);
        pixelData[1] = (kzU8)(shortValue & 0xFF);
    }
    else if(dataFormat == KZC_IMAGE_DATA_FORMAT_ALPHA_8)
    {
        pixelData[0] = alpha8bit;
    }
    else
    {
        kzsAssert(KZ_FALSE);
    }
}

static void kzcImageSetPixelComponents_internal(void* imageData, enum KzcImageDataFormat dataFormat,
                                                kzFloat red, kzFloat green, kzFloat blue, kzFloat alpha)
{
    if(kzcImageIsDataFormatByte_internal(dataFormat))
    {
        kzU8* pixelData = (kzU8*)imageData;
        kzU8 red8bit = (kzU8)(red * 255.9f);
        kzU8 green8bit = (kzU8)(green * 255.9f);
        kzU8 blue8bit = (kzU8)(blue * 255.9f);
        kzU8 alpha8bit = (kzU8)(alpha * 255.9f);

        kzcImageSetPixelComponents8bit_internal(pixelData, dataFormat, red8bit, green8bit, blue8bit, alpha8bit);
    }
    else if(kzcImageIsDataFormatFloat_internal(dataFormat))
    {
        kzFloat* pixelData = (kzFloat*)imageData;
        
        if(dataFormat == KZC_IMAGE_DATA_FORMAT_RGB_FLOAT)
        {
            pixelData[0] = red;
            pixelData[1] = green;
            pixelData[2] = blue;
        }
        else if(dataFormat == KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT)
        {
            pixelData[0] = red;
            pixelData[1] = green;
            pixelData[2] = blue;
            pixelData[3] = alpha;
        }
        else if(dataFormat == KZC_IMAGE_DATA_FORMAT_ALPHA_FLOAT)
        {
            pixelData[0] = alpha;
        }
        else
        {
            kzsAssert(KZ_FALSE);
        }
    }
    else
    {
        kzsAssert(KZ_FALSE);
    }
}

kzsError kzcImageConvert(const struct KzcImage* image, enum KzcImageDataFormat dataFormat)
{
    kzsError result;
    kzUint i;

    kzsAssert(kzcImageIsValid(image));

    /* First check for compressed formats */
    if(dataFormat == KZC_IMAGE_DATA_FORMAT_DXT3)
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Compression to DXT3 not supported");
    }
    else if(dataFormat == KZC_IMAGE_DATA_FORMAT_DXT5)
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Compression to DXT5 not supported");
    }
    else if(dataFormat == KZC_IMAGE_DATA_FORMAT_ETC)
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Use kzcImageConvertToETC to convert image to ETC compressed format");
    }

    if(image->data->dataFormat == KZC_IMAGE_DATA_FORMAT_ETC)
    {
        /* ETC can be converted straight only to RGB 888. First convert to that. */
        kzU8* uncompressedData;
        kzUint uncompressedSize = image->data->width * image->data->height * 3;
        kzUint bytesWritten;
        struct KzcMemoryManager* memoryManager;

        memoryManager = kzcMemoryGetManager(image);

        result = kzcMemoryAllocPointer(memoryManager, &uncompressedData, uncompressedSize, "ETC uncompression buffer");
        kzsErrorForward(result);

        result = kzcEtcUncompressImageToBuffer(memoryManager, image->data->data, (kzInt)image->data->width, (kzInt)image->data->height, 
                                               (kzInt)image->data->width, (kzInt)image->data->height, uncompressedData, &bytesWritten);
        kzsErrorForward(result);

        kzsCounterIncrease("kzcImageConvert");

        /* Check that buffer did not overflow */
        kzsAssertText(bytesWritten == uncompressedSize, "ETC uncompression took less space than expected");

        result = kzcMemoryFreePointer(image->data->data);
        kzsErrorForward(result);

        image->data->data = uncompressedData;

        /* Now image is in rgb888 format. Reformat if this is not the format requested. */
        image->data->dataFormat = KZC_IMAGE_DATA_FORMAT_RGB_888;
        if(dataFormat != KZC_IMAGE_DATA_FORMAT_RGB_888)
        {
            result = kzcImageConvert(image, dataFormat);
            kzsErrorForward(result);
        }
    }
    else
    {
        if((image->data->dataFormat == KZC_IMAGE_DATA_FORMAT_GRAYSCALE_8 && dataFormat == KZC_IMAGE_DATA_FORMAT_ALPHA_8) ||
            (image->data->dataFormat == KZC_IMAGE_DATA_FORMAT_ALPHA_8 && dataFormat == KZC_IMAGE_DATA_FORMAT_GRAYSCALE_8))
        {
            image->data->dataFormat = dataFormat;
        }
        else if(image->data->dataFormat != dataFormat)
        {
            struct KzcMemoryManager* memoryManager;
            kzU8* newDataU8 = KZ_NULL;
            kzFloat* newDataFloat = KZ_NULL;
            kzU8* sourceDataU8 = (kzU8*)image->data->data;
            kzFloat* sourceDataFloat = (kzFloat*)image->data->data;
            kzUint imagePixelCount = image->data->width * image->data->height;
            enum KzcImageDataFormat originalFormat = image->data->dataFormat;
            kzUint newBytesPerPixel = kzcImageGetBytesPerPixelForFormat_internal(dataFormat);
            kzUint originalOffset, newOffset;
            const kzFloat byteToFloatMultiplier = 1.0f / 255.0f;
            kzBool isOriginalFormatFloat = kzcImageIsDataFormatFloat_internal(originalFormat);
            kzBool isOriginalFormatByte = kzcImageIsDataFormatByte_internal(originalFormat);
            kzBool isDataFormatByte = kzcImageIsDataFormatByte_internal(dataFormat);
            kzBool isDataFormatFloat = kzcImageIsDataFormatFloat_internal(dataFormat);

            memoryManager = kzcMemoryGetManager(image);

            kzsCounterIncrease("kzcImageConvert");
            image->data->dataFormat = dataFormat;

            /* Allocate memory for byte data. */
            if(isDataFormatByte)
            {
                result = kzcMemoryAllocPointer(memoryManager, &newDataU8, newBytesPerPixel * image->data->width * image->data->height,
                    "ConvertedImageDataU8");
                kzsErrorForward(result);
            }
            /* Allocate memory for float data. */
            else if(isDataFormatFloat)
            {
                result = kzcMemoryAllocPointer(memoryManager, &newDataFloat, newBytesPerPixel * image->data->width * image->data->height,
                    "ConvertedImageDataFloat");
                kzsErrorForward(result);
            }
            else
            {
                kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Image format for converting unsupported!");
            }

            originalOffset = 0;
            newOffset = 0;

            if(originalFormat == KZC_IMAGE_DATA_FORMAT_RGBA_8888 && dataFormat == KZC_IMAGE_DATA_FORMAT_RGB_888)
            {
                kzsAssert(newDataU8 != KZ_NULL);
                kzsAssert(sourceDataU8 != KZ_NULL);

                for(i = 0; i < imagePixelCount; ++i)
                {
                    newDataU8[newOffset++] = sourceDataU8[originalOffset++];
                    newDataU8[newOffset++] = sourceDataU8[originalOffset++];
                    newDataU8[newOffset++] = sourceDataU8[originalOffset++];
                    ++originalOffset;
                }
            }
            else if(originalFormat == KZC_IMAGE_DATA_FORMAT_RGB_888 && dataFormat == KZC_IMAGE_DATA_FORMAT_RGBA_8888)
            {
                kzsAssert(newDataU8 != KZ_NULL);
                kzsAssert(sourceDataU8 != KZ_NULL);

                for(i = 0; i < imagePixelCount; ++i)
                {
                    newDataU8[newOffset++] = sourceDataU8[originalOffset++];
                    newDataU8[newOffset++] = sourceDataU8[originalOffset++];
                    newDataU8[newOffset++] = sourceDataU8[originalOffset++];
                    newDataU8[newOffset++] = 0xFF;
                }
            }
            else
            {
                for(i = 0; i < imagePixelCount; ++i)
                {
                    kzFloat sourceRed = 0.0f;
                    kzFloat sourceGreen = 0.0f;
                    kzFloat sourceBlue = 0.0f;
                    kzFloat sourceAlpha = 1.0f;

                    if(isOriginalFormatByte)
                    {
                        if(originalFormat == KZC_IMAGE_DATA_FORMAT_ALPHA_8)
                        {
                            if(isDataFormatByte)
                            {
                                /*lint -e{613}*/ /* Suppressing the per-pixel null pointer assert. */
                                kzcImageSetPixelComponents8bit_internal(&newDataU8[newOffset], dataFormat, sourceDataU8[originalOffset],
                                    sourceDataU8[originalOffset], sourceDataU8[originalOffset], sourceDataU8[originalOffset]);
                            }
                            else
                            {
                                sourceAlpha = (kzFloat)sourceDataU8[originalOffset] * byteToFloatMultiplier;
                            }
                            ++originalOffset;
                        }
                        else if(originalFormat == KZC_IMAGE_DATA_FORMAT_RGB_565)
                        {
                            kzUint combinedColor = (kzUint)((sourceDataU8[originalOffset] << 8) | (sourceDataU8[originalOffset + 1]));
                            if(isDataFormatByte)
                            {
                                /*lint -e{613}*/ /* Suppressing the per-pixel null pointer assert. */
                                kzcImageSetPixelComponents8bit_internal(&newDataU8[newOffset], dataFormat,
                                    (kzU8)(combinedColor >> 11), (kzU8)((combinedColor >> 5) & 0x3F),
                                    (kzU8)(combinedColor & 0x1F), 0xFF);
                            }
                            else
                            {
                                sourceRed   = (combinedColor >> 11) / 32.0f;
                                sourceGreen = ((combinedColor >> 5) & 0x3F) / 64.0f;
                                sourceBlue  = (combinedColor & 0x1F) / 32.0f;
                            }

                            originalOffset += 2;
                        }
                        else if(originalFormat == KZC_IMAGE_DATA_FORMAT_RGB_888)
                        {
                            if(isDataFormatByte)
                            {
                                /*lint -e{613}*/ /* Suppressing the per-pixel null pointer assert. */
                                kzcImageSetPixelComponents8bit_internal(&newDataU8[newOffset],
                                    dataFormat, sourceDataU8[originalOffset], sourceDataU8[originalOffset + 1],
                                    sourceDataU8[originalOffset + 2], 255);
                            }
                            else
                            {
                                sourceRed   = (kzFloat)sourceDataU8[originalOffset] * byteToFloatMultiplier;
                                sourceGreen = (kzFloat)sourceDataU8[originalOffset + 1] * byteToFloatMultiplier;
                                sourceBlue  = (kzFloat)sourceDataU8[originalOffset + 2] * byteToFloatMultiplier;
                            }
                            originalOffset += 3;
                        }
                        else if(originalFormat == KZC_IMAGE_DATA_FORMAT_RGBA_8888)
                        {
                            if(isDataFormatByte)
                            {
                                /*lint -e{613}*/ /* Suppressing the per-pixel null pointer assert. */
                                kzcImageSetPixelComponents8bit_internal(&newDataU8[newOffset],
                                    dataFormat, sourceDataU8[originalOffset], sourceDataU8[originalOffset + 1],
                                    sourceDataU8[originalOffset + 2], sourceDataU8[originalOffset + 3]);
                            }
                            else
                            {
                                sourceRed       = (kzFloat)sourceDataU8[originalOffset] * byteToFloatMultiplier;
                                sourceGreen     = (kzFloat)sourceDataU8[originalOffset + 1] * byteToFloatMultiplier;
                                sourceBlue      = (kzFloat)sourceDataU8[originalOffset + 2] * byteToFloatMultiplier;
                                sourceAlpha     = (kzFloat)sourceDataU8[originalOffset + 3] * byteToFloatMultiplier;
                            }
                            originalOffset += 4;
                        }
                        else
                        {
                            kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Unsupported source pixel format!");
                        }
                    }
                    else if(isOriginalFormatFloat)
                    {
                        if(originalFormat == KZC_IMAGE_DATA_FORMAT_ALPHA_FLOAT)
                        {
                            sourceAlpha = sourceDataFloat[i];
                        }
                        else if(originalFormat == KZC_IMAGE_DATA_FORMAT_RGB_FLOAT)
                        {
                            sourceRed   = sourceDataFloat[originalOffset];
                            sourceGreen = sourceDataFloat[originalOffset + 1];
                            sourceBlue  = sourceDataFloat[originalOffset + 2];
                            originalOffset += 3;
                        }
                        else if(originalFormat == KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT)
                        {
                            sourceRed   = sourceDataFloat[originalOffset];
                            sourceGreen = sourceDataFloat[originalOffset + 1];
                            sourceBlue  = sourceDataFloat[originalOffset + 2];
                            sourceAlpha  = sourceDataFloat[originalOffset + 3];
                            originalOffset += 4;
                        }
                        else
                        {
                            kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Unsupported source pixel format!");
                        }
                    }
                    else
                    {
                        kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Unsupported source pixel format!");
                    }

                    if(isDataFormatByte)
                    {
                        if(!isOriginalFormatByte)
                        {
                            /*lint -e{613}*/
                            kzcImageSetPixelComponents_internal((void*)&newDataU8[newOffset], dataFormat,
                                sourceRed, sourceGreen, sourceBlue, sourceAlpha);
                        }
                    }
                    else if(isDataFormatFloat)
                    {
                        /*lint -e{613}*/
                        kzcImageSetPixelComponents_internal((void*)&newDataFloat[newOffset], dataFormat,
                            sourceRed, sourceGreen, sourceBlue, sourceAlpha);
                    }
                    newOffset += newBytesPerPixel;
                }

            }

            result = kzcMemoryFreePointer(image->data->data);
            kzsErrorForward(result);

            /* Assign new data for image. */
            if(kzcImageIsDataFormatByte_internal(dataFormat))
            {
                image->data->data = newDataU8;
            }
            else if(kzcImageIsDataFormatFloat_internal(dataFormat))
            {
                image->data->data = newDataFloat;
            }
        }
    }

    kzsSuccess();
}

kzsError kzcImageSetName(const struct KzcImage* image, kzString name)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(image));

    /* Delete previous name if possible. */
    if(image->data->name != KZ_NULL)
    {
        result = kzcStringDelete(image->data->name);
        kzsErrorForward(result);
    }

    result = kzcStringCopy(kzcMemoryGetManager(image), name, &image->data->name);
    kzsErrorForward(result);

    kzsSuccess();
}

kzString kzcImageGetName(const struct KzcImage* image)
{
    kzsAssert(kzcIsValidPointer(image));
    return image->data->name;
}

kzsError kzcImageConvertToETC(const struct KzcImage* sourceImage, enum KzcEtcCompressionMode compressionMode)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcImageIsValid(sourceImage));

    memoryManager = kzcMemoryGetManager(sourceImage);

    /* Do nothing if already compressed. */
    if(sourceImage->data->dataFormat != KZC_IMAGE_DATA_FORMAT_ETC)
    {
        /* Only RGB888 format can be ETC compressed. Convert first to this format.*/
        if(sourceImage->data->dataFormat != KZC_IMAGE_DATA_FORMAT_RGB_888)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Note: Only rgb888 images can be compressed to ETC format. Converting input image to correct format.");
            result = kzcImageConvert(sourceImage, KZC_IMAGE_DATA_FORMAT_RGB_888);
            kzsErrorForward(result);
        }

        /* Check if image dimensions are suitable for conversion. Height must be divisible by
           4 and width by 2. Resize to compressable size if needed. */
        {
            kzUint widthRemainder = sourceImage->data->width % 2;
            kzUint heightRemainder = sourceImage->data->height % 4;
            if(widthRemainder != 0 || heightRemainder != 0)
            {
                kzUint newWidth = sourceImage->data->width + widthRemainder;
                kzUint newHeight = sourceImage->data->height + heightRemainder;
                kzsLog(KZS_LOG_LEVEL_INFO, "Note: Only images with height%4==0 and width%2==0 can be ETC compressed. Scaling image to fit requirement.");
                result = kzcImageResize(sourceImage, newWidth, newHeight, KZC_IMAGE_RESIZE_FILTER_BILINEAR);
                kzsErrorForward(result);
            }
        }

        /* Convert */
        {
            kzU8* compressedData;
            kzUint compressedDataSize = kzcEtcGetCompressedBufferMemorySize(sourceImage->data->width, sourceImage->data->height);
            kzUint bytesWritten;

            result = kzcMemoryAllocPointer(memoryManager, &compressedData, compressedDataSize, "ETC compressed image buffer");
            kzsErrorForward(result);

            result = kzcEtcCompressImageToBuffer(memoryManager, sourceImage->data->data, (kzInt)sourceImage->data->width, (kzInt)sourceImage->data->height,
                compressedData, compressionMode, &bytesWritten);
            kzsErrorForward(result);

            /* Check that no boundaries were crossed. */
            kzsAssertText(bytesWritten == compressedDataSize, "Unexpected error. ETC compression wrote too few bytes");

            result = kzcMemoryFreePointer(sourceImage->data->data);
            kzsErrorForward(result);

            sourceImage->data->data = compressedData;
            sourceImage->data->dataFormat = KZC_IMAGE_DATA_FORMAT_ETC;
        }
    }
    kzsSuccess();
}

kzsError kzcImageResize(const struct KzcImage* image, kzUint newWidth, kzUint newHeight, enum KzcImageResizeFilter resizeFilter)
{
    kzUint x, y, k;
    kzsError result;
    void* newData;
    struct KzcMemoryManager* memoryManager;
    kzUint bytesPerPixel;
    kzFloat sampleXStep;
    kzFloat sampleYStep;
    kzUint originalWidth, originalHeight;

    kzsAssert(kzcImageIsValid(image));
    kzsAssert(newWidth > 0);
    kzsAssert(newHeight > 0);
    memoryManager = kzcMemoryGetManager(image);

    result = kzcMemoryAllocPointer(memoryManager, &newData, newWidth * newHeight * kzcImageGetBytesPerPixel_internal(image), "NewImageData");
    kzsErrorForward(result);

    kzsMemset(newData, 0, newWidth * newHeight * kzcImageGetBytesPerPixel_internal(image));

    originalWidth = image->data->width;
    originalHeight = image->data->height;

    bytesPerPixel = kzcImageGetBytesPerPixel_internal(image);

    sampleXStep = 1.0f / image->data->width;
    sampleYStep = 1.0f / image->data->height;
    if(kzcImageIsDataFormatByte_internal(image->data->dataFormat))
    {
        kzU8* oldBuffer = (kzU8*)image->data->data;
        kzU8* newBuffer = (kzU8*)newData;

        if(resizeFilter != KZC_IMAGE_RESIZE_FILTER_WEIGHTED_AVERAGE)
        {
            kzUint relativeXStep = (1 << 16) / kzsMaxU(1, (newWidth - 1));
            kzUint relativeYStep = (1 << 16) / kzsMaxU(1, (newHeight - 1));
            kzUint relativeY = 0;

            for(y = 0; y < newHeight; ++y)
            {
                kzUint relativeX = 0;

                for(x = 0; x < newWidth; ++x)
                {
                    switch(resizeFilter)
                    {
                        /* Nearest neighbor, take 1 sample. */
                        case KZC_IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR:
                        {
                            kzUint sampleX = (kzUint)((relativeX * originalWidth) >> 16);
                            kzUint sampleY = (kzUint)((relativeY * originalHeight) >> 16);
                            for(k = 0; k < bytesPerPixel; ++k)
                            {
                                kzUint totalColor;
                                totalColor = (kzUint)oldBuffer[(sampleY * image->data->width + sampleX) * bytesPerPixel + k];
                                newBuffer[(y * newWidth + x) * bytesPerPixel + k] = (kzU8)(totalColor);
                            }

                            break;
                        }
                        case KZC_IMAGE_RESIZE_FILTER_BILINEAR:
                        {
                            kzUint sampleX = (kzUint)((relativeX * originalWidth) >> 16);
                            kzUint sampleY = (kzUint)((relativeY * originalHeight) >> 16);
                            kzUint sampleX2 = (kzUint)kzsMin((kzInt)sampleX + 1, (kzInt)originalWidth - 1);
                            kzUint sampleY2 = (kzUint)kzsMin((kzInt)sampleY + 1, (kzInt)originalHeight - 1);
                            kzUint sample1Index = (sampleY * image->data->width + sampleX);
                            kzUint sample2Index = (sampleY * image->data->width + sampleX2);
                            kzUint sample3Index = (sampleY2 * image->data->width + sampleX);
                            kzUint sample4Index = (sampleY2 * image->data->width + sampleX2);
                            kzUint xMod = (relativeX * originalWidth) & 0xFFFF;
                            kzUint yMod = (relativeY * originalHeight) & 0xFFFF;
                            kzUint xMod2 = (0xFFFF - xMod);
                            kzUint yMod2 = (0xFFFF - yMod);

                            kzsAssert(xMod < 0x10000);
                            kzsAssert(yMod < 0x10000);
                            kzsAssert(xMod2 < 0x10000);
                            kzsAssert(yMod2 < 0x10000);

                            for(k = 0; k < bytesPerPixel; ++k)
                            {
                                kzUint totalColor = 0;
                                totalColor += (kzUint)oldBuffer[sample1Index * bytesPerPixel + k] * (xMod2 >> 8) * (yMod2 >> 8);
                                totalColor += (kzUint)oldBuffer[sample2Index * bytesPerPixel + k] * (xMod >> 8) * (yMod2 >> 8);
                                totalColor += (kzUint)oldBuffer[sample3Index * bytesPerPixel + k] * (xMod2 >> 8) * (yMod >> 8);
                                totalColor += (kzUint)oldBuffer[sample4Index * bytesPerPixel + k] * (xMod >> 8) * (yMod >> 8);
                                newBuffer[(y * newWidth + x) * bytesPerPixel + k] = (kzU8)(totalColor >> 16);
                            }

                            break;
                        }
                        case KZC_IMAGE_RESIZE_FILTER_WEIGHTED_AVERAGE:
                        {
                            kzsAssert(KZ_FALSE);
                            break;
                        }
                        default:
                        {
                            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown resize filter");
                        }
                    }
                    relativeX += relativeXStep;
                }
                relativeY += relativeYStep;
            }
        }
        else
        {
            kzFloat* weightedSamples;
            kzFloat newTexelWidth = (kzFloat)originalWidth / (kzFloat)newWidth;
            kzFloat newTexelWidthInv = 1.0f / newTexelWidth;
            kzFloat newTexelHeight = (kzFloat)originalHeight / (kzFloat)newHeight;
            kzFloat newTexelHeightInv = 1.0f / newTexelHeight;
            kzFloat newTexelArea = (kzFloat)(kzInt)(newWidth * newHeight) / (kzFloat)(kzInt)(originalWidth * originalHeight);
            kzFloat widthRelation = (kzFloat)newWidth / (kzFloat)originalWidth;
            kzFloat heightRelation = (kzFloat)newHeight / (kzFloat)originalHeight;

            result = kzcMemoryAllocPointer(memoryManager, &weightedSamples,
                                           sizeof(*weightedSamples) * bytesPerPixel * newWidth * newHeight,
                                           "WeightedAreaSamples");
            kzsErrorForward(result);

            for(y = 0; y < newHeight; ++y)
            {
                for(x = 0; x < newWidth; ++x)
                {
                    for(k = 0; k < bytesPerPixel; ++k)
                    {
                        weightedSamples[(y * newWidth + x) * bytesPerPixel + k] = 0.0f;
                    }
                }
            }
            for(y = 0; y < originalHeight; ++y)
            {
                kzFloat weight1, weight2, weight3, weight4;
                kzInt intX1, intY1, intX2, intY2;

                kzFloat texelBorderY = (((kzFloat)(kzUint)(y * newTexelHeightInv)) * newTexelHeight + newTexelHeight);
                kzFloat yWidth1 = kzsMinf(1.0f, kzsFabsf((kzFloat)y - texelBorderY));
                kzFloat yWidth2 = 1.0f - yWidth1;
                kzFloat newY1 = (kzFloat)y * heightRelation;

                for(x = 0; x < originalWidth; ++x)
                {
                    kzFloat texelBorderX = (((kzFloat)(kzUint)(x * newTexelWidthInv)) * newTexelWidth + newTexelWidth);
                    
                    kzFloat newX1 = (kzFloat)x * widthRelation;
                    kzFloat xWidth1 = kzsMinf(1.0f, kzsFabsf((kzFloat)x - texelBorderX));
                    kzFloat xWidth2 = 1.0f - xWidth1;

                    weight1 = xWidth1 * yWidth1 * newTexelArea;
                    weight2 = xWidth2 * yWidth1 * newTexelArea;
                    weight3 = xWidth1 * yWidth2 * newTexelArea;
                    weight4 = xWidth2 * yWidth2 * newTexelArea;

                    intX1 = kzsClampi((kzUint)newX1, 0, (kzInt)newWidth - 1);
                    intY1 = kzsClampi((kzUint)newY1, 0, (kzInt)newHeight - 1);
                    intX2 = kzsClampi(intX1 + 1, 0, (kzInt)newWidth - 1);
                    intY2 = kzsClampi(intY1 + 1, 0, (kzInt)newHeight - 1);

                    /* Every original pixel maps to 4 new pixels. */
                    for(k = 0; k < bytesPerPixel; ++k)
                    {
                        kzFloat oldValue = (kzFloat)oldBuffer[(y * originalWidth + x) * bytesPerPixel + k];

                        weightedSamples[((kzUint)intY1 * newWidth + (kzUint)intX1) * bytesPerPixel + k] += oldValue * weight1;
                        weightedSamples[((kzUint)intY1 * newWidth + (kzUint)intX2) * bytesPerPixel + k] += oldValue * weight2;
                        weightedSamples[((kzUint)intY2 * newWidth + (kzUint)intX1) * bytesPerPixel + k] += oldValue * weight3;
                        weightedSamples[((kzUint)intY2 * newWidth + (kzUint)intX2) * bytesPerPixel + k] += oldValue * weight4;
                    }
                }
            }

            for(y = 0; y < newHeight; ++y)
            {
                for(x = 0; x < newWidth; ++x)
                {
                    for(k = 0; k < bytesPerPixel; ++k)
                    {
                        newBuffer[(y * newWidth + x) * bytesPerPixel + k] =
                                (kzU8)kzsClampi((kzInt)weightedSamples[(y * newWidth + x) * bytesPerPixel + k], 0, 0xFF);
                    }
                }
            }
            result = kzcMemoryFreePointer(weightedSamples);
            kzsErrorForward(result);
        }
    }
    else if(kzcImageIsDataFormatFloat_internal(image->data->dataFormat))
    {
        kzFloat* oldBuffer = (kzFloat*)image->data->data;
        kzFloat* newBuffer = (kzFloat*)newData;

        for(y = 0; y < newHeight; ++y)
        {
            kzFloat relativeY = y / (kzFloat)newHeight;
            for(x = 0; x < newWidth; ++x)
            {
                kzFloat relativeX = x / (kzFloat)newWidth;

                switch(resizeFilter)
                {
                    /* Nearest neighbor, take 1 sample. */
                    case KZC_IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR:
                    {
                        kzUint sampleX = (kzUint)(relativeX * originalWidth);
                        kzUint sampleY = (kzUint)(relativeY * originalHeight);
                        for(k = 0; k < bytesPerPixel; ++k)
                        {
                            kzFloat totalColor;
                            totalColor = oldBuffer[(sampleY * image->data->width + sampleX) * bytesPerPixel + k];
                            newBuffer[(y * newWidth + x) * bytesPerPixel + k] = totalColor;
                        }

                        break;
                    }
                    case KZC_IMAGE_RESIZE_FILTER_BILINEAR:
                    {
                        kzUint sampleX = (kzUint)(relativeX * originalWidth);
                        kzUint sampleY = (kzUint)(relativeY * (kzFloat)originalHeight);
                        kzUint sampleX2 = (kzUint)kzsMin((kzInt)sampleX + 1, (kzInt)originalWidth - 1);
                        kzUint sampleY2 = (kzUint)kzsMin((kzInt)sampleY + 1, (kzInt)originalHeight - 1);

                        kzUint sample1Index = (sampleY * image->data->width + sampleX);
                        kzUint sample2Index = (sampleY * image->data->width + sampleX2);
                        kzUint sample3Index = (sampleY2 * image->data->width + sampleX);
                        kzUint sample4Index = (sampleY2 * image->data->width + sampleX2);

                        kzFloat xMod = kzsFmod(relativeX, sampleXStep) / sampleXStep;
                        kzFloat yMod = kzsFmod(relativeY, sampleYStep) / sampleYStep;
                        kzFloat xMod2 = (1.0f - xMod);
                        kzFloat yMod2 = (1.0f - yMod);

                        for(k = 0; k < bytesPerPixel; ++k)
                        {
                            kzFloat totalColor = 0.0f;
                            totalColor += (oldBuffer[sample1Index * bytesPerPixel + k] * xMod2 * yMod2);
                            totalColor += (oldBuffer[sample2Index * bytesPerPixel + k] * xMod * yMod2);
                            totalColor += (oldBuffer[sample3Index * bytesPerPixel + k] * xMod2 * yMod);
                            totalColor += (oldBuffer[sample4Index * bytesPerPixel + k] * xMod * yMod);
                            newBuffer[(y * newWidth + x) * bytesPerPixel + k] = totalColor;
                        }

                        break;
                    }
                    case KZC_IMAGE_RESIZE_FILTER_WEIGHTED_AVERAGE:
                    {
                        KZS_NOT_IMPLEMENTED_YET_ERROR;
                    }
                    default:
                    {
                        kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown resize filter");
                    }
                }
            }
        }
    }

    /* Delete original data, assign new data for image. */
    result = kzcMemoryFreePointer(image->data->data);
    kzsErrorForward(result);
    image->data->data = newData;
    image->data->width = newWidth;
    image->data->height = newHeight;

    kzsSuccess();
}

kzsError kzcImageCrop(const struct KzcImage* image, kzUint startX, kzUint startY, kzUint width, kzUint height)
{
    kzUint x, y, k;
    kzsError result;
    void* newData;
    struct KzcMemoryManager* memoryManager;
    kzUint bitsPerPixel;

    kzsAssert(kzcImageIsValid(image));
    memoryManager = kzcMemoryGetManager(image);

    if((startX + width) > image->data->width || (startY + height) > image->data->height)
    {
        kzsErrorThrow(KZC_ERROR_IMAGE_MANIPULATION_ERROR, "Cropping outside image area is not allowed!");
    }

    result = kzcMemoryAllocPointer(memoryManager, &newData, width * height * kzcImageGetBytesPerPixel_internal(image), "NewImageData");
    kzsErrorForward(result);

    bitsPerPixel = kzcImageGetBytesPerPixel_internal(image);
    if(kzcImageIsDataFormatByte_internal(image->data->dataFormat))
    {
        kzU8* oldBuffer = (kzU8*)image->data->data;
        kzU8* newBuffer = (kzU8*)newData;

        for(y = 0; y < height; ++y)
        {
            kzUint oldImageY = startY + y;
            kzUint newImageY = y;
            for(x = 0; x < width; ++x)
            {
                kzUint oldImageX = startX + x;
                kzUint newImageX = x;

                for(k = 0; k < bitsPerPixel; ++k)
                {
                    newBuffer[(newImageY * width + newImageX) * bitsPerPixel + k] = 
                        oldBuffer[(oldImageY * image->data->width + oldImageX) * bitsPerPixel + k];
                }
            }
        }
    }
    else if(kzcImageIsDataFormatFloat_internal(image->data->dataFormat))
    {
        kzFloat* oldBuffer = (kzFloat*)image->data->data;
        kzFloat* newBuffer = (kzFloat*)newData;

        for(y = 0; y < height; ++y)
        {
            kzUint oldImageY = startY + y;
            kzUint newImageY = y;
            for(x = 0; x < width; ++x)
            {
                kzUint oldImageX = startX + x;
                kzUint newImageX = x;

                for(k = 0; k < bitsPerPixel; ++k)
                {
                    newBuffer[(newImageY * width + newImageX) * bitsPerPixel + k] = 
                        oldBuffer[(oldImageY * image->data->width + oldImageX) * bitsPerPixel + k];
                }
            }
        }
    }
    else
    {
        kzsErrorThrow(KZC_ERROR_IMAGE_FORMAT_UNSUPPORTED, "Unsupported image format for cropping!");
    }

    /* Delete original data, assign new data for image. */
    result = kzcMemoryFreePointer(image->data->data);
    kzsErrorForward(result);
    image->data->data = newData;
    image->data->width = width;
    image->data->height = height;

    kzsSuccess();
}

static kzBool kzcImageIsDataFormatByte_internal(enum KzcImageDataFormat dataFormat)
{
    return (dataFormat == KZC_IMAGE_DATA_FORMAT_RGB_888 ||
        dataFormat == KZC_IMAGE_DATA_FORMAT_RGBA_8888 ||
        dataFormat == KZC_IMAGE_DATA_FORMAT_RGB_565 ||
        dataFormat == KZC_IMAGE_DATA_FORMAT_ALPHA_8);
}

static kzBool kzcImageIsDataFormatFloat_internal(enum KzcImageDataFormat dataFormat)
{
    return (dataFormat == KZC_IMAGE_DATA_FORMAT_RGB_FLOAT ||
        dataFormat == KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT ||
        dataFormat == KZC_IMAGE_DATA_FORMAT_ALPHA_FLOAT);
}

static kzBool kzcImageIsDataFormatFloat16_internal(enum KzcImageDataFormat dataFormat)
{
    return (dataFormat == KZC_IMAGE_DATA_FORMAT_RGB_FLOAT16 || dataFormat == KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT16);
}

static kzUint kzcImageGetBytesPerPixelForFormat_internal(enum KzcImageDataFormat dataFormat)
{
    kzUint bytesPerPixel;

    switch(dataFormat)
    {
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT16:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGB_888:
        {
            bytesPerPixel = 3;
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT16:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGBA_8888:
        {
            bytesPerPixel = 4;
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_RGB_565:
        {
            bytesPerPixel = 2;
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_8:
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_8:
        {
            bytesPerPixel = 1;
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_DXT5:
        case KZC_IMAGE_DATA_FORMAT_DXT3:
        case KZC_IMAGE_DATA_FORMAT_ETC:
        /* ETC uses 8 bytes to represent 16 pixels so the requested number is not applicable to ETC compression */
        default:
        {
            bytesPerPixel = 0;
            kzsAssert(KZ_FALSE);
            break;
        }
    }
    return bytesPerPixel;
}

static kzUint kzcImageGetBytesPerPixel_internal(const struct KzcImage* image)
{
    kzUint bytesPerPixel;

    kzsAssert(kzcImageIsValid(image));

    bytesPerPixel = kzcImageGetBytesPerPixelForFormat_internal(image->data->dataFormat);

    return bytesPerPixel;
}

kzsError kzcImageFlipHorizontally(const struct KzcImage* image)
{
    KZS_NOT_IMPLEMENTED_YET;

    kzsSuccess();
}

kzsError kzcImageFlipVertically(const struct KzcImage* image)
{
    kzsError result;
    kzUint j;
    kzUint bytesPerPixel;
    kzUint horizontalLineLength;
    
    kzsAssert(kzcImageIsValid(image));
    
    bytesPerPixel = kzcImageGetBytesPerPixel_internal(image);
    horizontalLineLength = image->data->width * bytesPerPixel;

    /* Byte format. */
    if(kzcImageIsDataFormatByte_internal(image->data->dataFormat))
    {
        kzU8* pixelBuffer = (kzU8*)image->data->data;
        kzU8* tempBuffer;

        result = kzcMemoryAllocPointer(kzcMemoryGetManager(image), &tempBuffer, horizontalLineLength, "HorizontalLines");
        kzsErrorForward(result);

        for(j = 0; j < image->data->height / 2; ++j)
        {
            kzUint mirroredY = image->data->height - j - 1;
            kzUint yOffset = j * image->data->width * bytesPerPixel;
            kzUint yOffsetMirrored = mirroredY * image->data->width * bytesPerPixel;

            kzsMemcpy(tempBuffer, &pixelBuffer[yOffset], horizontalLineLength);
            kzsMemcpy(&pixelBuffer[yOffset], &pixelBuffer[yOffsetMirrored], horizontalLineLength);
            kzsMemcpy(&pixelBuffer[yOffsetMirrored], tempBuffer, horizontalLineLength);
        }

        result = kzcMemoryFreePointer(tempBuffer);
        kzsErrorForward(result);
    }
    /* Float format. */
    else if(kzcImageIsDataFormatFloat_internal(image->data->dataFormat))
    {
        kzFloat* pixelBuffer = (kzFloat*)image->data->data;
        kzFloat* tempBuffer;

        result = kzcMemoryAllocPointer(kzcMemoryGetManager(image), &tempBuffer, horizontalLineLength, "HorizontalLines");
        kzsErrorForward(result);

        for(j = 0; j < image->data->height / 2; ++j)
        {
            kzUint mirroredY = image->data->height - j - 1;
            kzUint yOffset = j * image->data->width * bytesPerPixel;
            kzUint yOffsetMirrored = mirroredY * image->data->width * bytesPerPixel;

            kzsMemcpy(tempBuffer, &pixelBuffer[yOffset], horizontalLineLength);
            kzsMemcpy(&pixelBuffer[yOffset], &pixelBuffer[yOffsetMirrored], horizontalLineLength);
            kzsMemcpy(&pixelBuffer[yOffsetMirrored], tempBuffer, horizontalLineLength);
        }

        result = kzcMemoryFreePointer(tempBuffer);
        kzsErrorForward(result);
    }
    else
    {
        kzsErrorThrow(KZC_ERROR_IMAGE_MANIPULATION_ERROR, "Unsupported format for image flipping!");
    }

    kzsSuccess();
}

kzsError kzcImageCreateMipmapLevels(const struct KzcMemoryManager* memoryManager, const struct KzcImage* image, struct KzcImage*** out_mipmapLevels)
{
    kzsError result;
    kzUint mipmapLevelCount;
    struct KzcImage** mipmapLevels;
    kzUint width;
    kzUint height;

    kzsAssert(kzcIsValidPointer(image));

    width = kzcImageGetWidth(image);
    height = kzcImageGetHeight(image);

    /* Calculate the mip-map level count */
    {
        kzUint newWidth = width / 2;
        kzUint newHeight = height / 2;
        mipmapLevelCount = 0;
        while (newWidth >= 1 && newHeight >= 1)
        {
            newWidth /= 2;
            newHeight /= 2;
            ++mipmapLevelCount;
        }
    }

    /* Reserve memory for mip-maps */
    result = kzcMemoryAllocArray(memoryManager, mipmapLevels, mipmapLevelCount, "Mipmap level pointers");
    kzsErrorForward(result);

    /* Generate mip-map data */
    {
        kzInt level = 0;
        width /= 2;
        height /= 2;
        while(width >= 1 && height >= 1)
        {
            struct KzcImage* newImage;

            if(level == 0)
            {
                result = kzcImageCopy(memoryManager, image, &newImage);
                kzsErrorForward(result);
            }
            else
            {
                result = kzcImageCopy(memoryManager, mipmapLevels[level - 1], &newImage);
                kzsErrorForward(result);
            }

            result = kzcImageResize(newImage, kzsMaxU(2, width), kzsMaxU(2, height), KZC_IMAGE_RESIZE_FILTER_WEIGHTED_AVERAGE);
            kzsErrorForward(result);

            mipmapLevels[level] = newImage;

            width /= 2;
            height /= 2;
            ++level;
        }
    }

    *out_mipmapLevels = mipmapLevels;
    kzsSuccess();
}

enum KzcTextureFormat kzcImageGetTextureFormat(const struct KzcImage* image)
{
    enum KzcImageDataFormat imageDataFormat;
    enum KzcTextureFormat format = KZC_TEXTURE_FORMAT_RGB;
    kzsAssert(kzcImageIsValid(image));

    imageDataFormat = kzcImageGetDataFormat(image);
    
    switch(imageDataFormat)
    {
        case KZC_IMAGE_DATA_FORMAT_RGB_888: format = KZC_TEXTURE_FORMAT_RGB; break;
        case KZC_IMAGE_DATA_FORMAT_RGBA_8888: format = KZC_TEXTURE_FORMAT_RGBA; break;
        case KZC_IMAGE_DATA_FORMAT_ALPHA_8: format = KZC_TEXTURE_FORMAT_ALPHA; break;
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_8: format = KZC_TEXTURE_FORMAT_LUMINANCE; break;
        case KZC_IMAGE_DATA_FORMAT_ETC: format = KZC_TEXTURE_FORMAT_ETC; break;
        case KZC_IMAGE_DATA_FORMAT_DXT3: format = KZC_TEXTURE_FORMAT_DXT3_RGBA; break;
        case KZC_IMAGE_DATA_FORMAT_DXT5: format = KZC_TEXTURE_FORMAT_DXT5_RGBA; break;
        case KZC_IMAGE_DATA_FORMAT_RGB_565:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT16:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT16:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_FLOAT:
        {
            format = KZC_TEXTURE_FORMAT_RGB;
            kzsAssert(KZ_FALSE);
        }
    }

    return format;
}

enum KzcImageDataFormat kzcTextureFormatGetImageFormat(enum KzcTextureFormat textureFormat)
{
    enum KzcImageDataFormat imageDataFormat;
   
    if(textureFormat == KZC_TEXTURE_FORMAT_RGB)
    {
        imageDataFormat = KZC_IMAGE_DATA_FORMAT_RGB_888;
    }
    else if(textureFormat == KZC_TEXTURE_FORMAT_RGB565)
    {
        imageDataFormat = KZC_IMAGE_DATA_FORMAT_RGB_565;
    }
    else if(textureFormat == KZC_TEXTURE_FORMAT_RGBA)
    {
        imageDataFormat = KZC_IMAGE_DATA_FORMAT_RGBA_8888;
    }
    else if(textureFormat == KZC_TEXTURE_FORMAT_ALPHA)
    {
        imageDataFormat = KZC_IMAGE_DATA_FORMAT_ALPHA_8;
    }
    else
    {
        imageDataFormat = KZC_IMAGE_DATA_FORMAT_RGB_888;
        kzsAssert(KZ_FALSE);
    }

    return imageDataFormat;
}

enum KzcImageDataFormat kzcImageGetImageFormatFromTextureFormat(enum KzcTextureFormat format)
{
    enum KzcImageDataFormat imageDataFormat;
  
    if(format == KZC_TEXTURE_FORMAT_RGB)
    {
        imageDataFormat = KZC_IMAGE_DATA_FORMAT_RGB_888;
    }
    else if(format == KZC_TEXTURE_FORMAT_RGBA)
    {
        imageDataFormat = KZC_IMAGE_DATA_FORMAT_RGBA_8888;
    }
    else if(format == KZC_TEXTURE_FORMAT_ALPHA)
    {
        imageDataFormat = KZC_IMAGE_DATA_FORMAT_ALPHA_8;
    }
    else
    {
        imageDataFormat = KZC_IMAGE_DATA_FORMAT_RGBA_8888;
        kzsAssert(KZ_FALSE);
    }

    return imageDataFormat;
}
