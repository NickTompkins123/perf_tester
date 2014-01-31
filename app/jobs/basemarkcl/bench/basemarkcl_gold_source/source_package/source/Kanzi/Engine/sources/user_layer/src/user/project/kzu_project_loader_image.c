/**
* \file
* Project loader for image files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_image.h"

#include "kzu_project.h"
#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/kzu_error_codes.h>

#include <core/util/image/kzc_image.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>

#include <system/debug/kzs_counter.h>
#include <system/time/kzs_tick.h>


kzsException kzuProjectLoaderLoadImagePng(const struct KzuProject* project, kzString path, struct KzcImage** out_image)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzcImage* image;
    kzUint measurementStart = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(project));

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load png with null path");

    /* Check if the image is already loaded. */
    image = (struct KzcImage*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, path);

    if (!kzcImageIsValid(image))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_IMAGE_PNG, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load PNG image file.");

        /* Load the image of the file info. */
        {
            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzcImageLoadPNG(memoryManager, inputStream, &image);
                kzsErrorForward(result);

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, image, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&image);
            kzsErrorForward(result);
        }
    }

    kzuProjectAddMeasurementCumulativeTime_private((struct KzuProject*)project, measurementStart, KZU_PROJECT_MEASUREMENT_IMAGE);

    *out_image = image;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadImageJpeg(const struct KzuProject* project, kzString path, struct KzcImage** out_image)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzcImage* image;
    kzUint measurementStart = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(project));

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load jpg with null path");

    /* Check if the image is already loaded. */
    image = (struct KzcImage*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, path);

    if (!kzcImageIsValid(image))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_IMAGE_JPEG, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load JPEG image file.");

        /* Load the image of the file info. */
        {
            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzcImageLoadJPEG(memoryManager, inputStream, &image);
                kzsErrorForward(result);

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, image, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&image);
            kzsErrorForward(result);
        }
    }

    kzuProjectAddMeasurementCumulativeTime_private((struct KzuProject*)project, measurementStart, KZU_PROJECT_MEASUREMENT_IMAGE);

    *out_image = image;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadImageETC(const struct KzuProject* project, kzString path, struct KzcImage** out_image)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzcImage* image;
    kzUint measurementStart = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(project));
    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load etc with null path");

    /* Check if the image is already loaded. */
    image = (struct KzcImage*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, path);

    if (!kzcImageIsValid(image))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_IMAGE_ETC, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load ETC image file.");

        /* Load the image of the file info. */
        {
            /* Read data */
            {
                struct KzcInputStream* inputStream;
                kzUint width, height, etcDataSize;
                kzU8* data;

                result = kzuBinaryDirectoryOpenFileWithEndianness(memoryManager, file, KZC_IO_STREAM_ENDIANNESS_BIG_ENDIAN, &inputStream);
                kzsErrorForward(result);

                result = kzcInputStreamReadU32Int(inputStream, &width);
                kzsErrorForward(result);

                result = kzcInputStreamReadU32Int(inputStream, &height);
                kzsErrorForward(result);

                result = kzcInputStreamReadU32Int(inputStream, &etcDataSize);
                kzsErrorForward(result);

                /* Create memory buffer for ETC data. */
                result = kzcMemoryAllocPointer(memoryManager, &data, sizeof(*data) * etcDataSize, "Temporary buffer for ETC data");
                kzsErrorForward(result);

                /* Fill the buffer from stream. */
                result = kzcInputStreamReadBytes(inputStream, etcDataSize, data);
                kzsErrorForward(result);

                result = kzcImageLoadMemoryAssignData(memoryManager, width, height, KZC_IMAGE_DATA_FORMAT_ETC, data, &image);
                kzsErrorForward(result);

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, image, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&image);
            kzsErrorForward(result);
        }
    }

    kzuProjectAddMeasurementCumulativeTime_private((struct KzuProject*)project, measurementStart, KZU_PROJECT_MEASUREMENT_COMPRESSED_IMAGE);

    *out_image = image;
    kzsSuccess();
}

/** Loads DXT image from binary. */
kzsException kzuProjectLoaderLoadImageDXT(const struct KzuProject* project, kzString path, struct KzcImage** out_image)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzcImage* image;
    kzUint measurementStart = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(project));
    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load etc with null path");

    /* Check if the image is already loaded. */
    image = (struct KzcImage*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, path);

    if (!kzcImageIsValid(image))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_IMAGE_DXT, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load ETC image file.");

        /* Load the image of the file info. */
        {
            /* Read data */
            {
                struct KzcInputStream* inputStream;
                kzUint width, height, dxtDataSize;
                kzU8* data;
                enum KzcIOStreamEndianness endianness;

                result = kzuBinaryDirectoryOpenFileWithEndianness(memoryManager, file, KZC_IO_STREAM_ENDIANNESS_UNSPECIFIED, &inputStream);
                kzsErrorForward(result);

                result = kzcInputStreamReadU32Int(inputStream, &dxtDataSize);
                kzsErrorForward(result);

                /* Create memory buffer for ETC data. */
                result = kzcMemoryAllocPointer(memoryManager, &data, sizeof(*data) * dxtDataSize, "Temporary buffer for DXT data");
                kzsErrorForward(result);

                /* Fill the buffer from stream. */
                result = kzcInputStreamReadBytes(inputStream, dxtDataSize, data);
                kzsErrorForward(result);

                endianness = kzcInputStreamGetEndianness(inputStream);
                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);

                result = kzcInputStreamCreateFromMemory(memoryManager, data, dxtDataSize, endianness, &inputStream);
                kzsErrorForward(result);

                /* 8 bytes till width & height. */
                result = kzcInputStreamSkip(inputStream, 4);
                kzsErrorForward(result);
                result = kzcInputStreamSkip(inputStream, 8);
                kzsErrorForward(result);

                result = kzcInputStreamReadU32Int(inputStream, &width);
                kzsErrorForward(result);
                result = kzcInputStreamReadU32Int(inputStream, &height);
                kzsErrorForward(result);

                /* FIXME: Read format from input stream. */
                result = kzcImageLoadMemoryAssignData(memoryManager, width, height, KZC_IMAGE_DATA_FORMAT_DXT3, data, &image);
                kzsErrorForward(result);

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, image, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&image);
            kzsErrorForward(result);
        }
    }

    kzuProjectAddMeasurementCumulativeTime_private((struct KzuProject*)project, measurementStart, KZU_PROJECT_MEASUREMENT_COMPRESSED_IMAGE);

    *out_image = image;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadImageMipmap(const struct KzuProject* project, kzString path, struct KzcImage** out_image)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzcImage* image;

    kzsAssert(kzcIsValidPointer(project));

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load mipmap with null path");

    /* Check if the image is already loaded. */
    image = (struct KzcImage*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, path);

    if (!kzcImageIsValid(image))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_IMAGE_MIPMAP, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load PNG image file.");

        /* Load the image of the file info. */
        {
            kzMutableString* mipmapLevelPaths = KZ_NULL;
            kzUint mipmapLevelCount = 0;
            kzUint i;
            struct KzcImage** mipmapImages = KZ_NULL;

            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzcInputStreamReadU32Int(inputStream, &mipmapLevelCount);
                kzsErrorForward(result);

                result = kzcMemoryAllocArray(memoryManager, mipmapImages, mipmapLevelCount, "MipmapImages");
                kzsErrorForward(result);
                result = kzcMemoryAllocArray(memoryManager, mipmapLevelPaths, mipmapLevelCount, "MipmapPaths");
                kzsErrorForward(result);

                for(i = 0; i < mipmapLevelCount; ++i)
                {
                    result = kzuBinaryDirectoryReadReference(inputStream, file, (kzString*)&mipmapLevelPaths[i]);
                    kzsErrorForward(result);

                    result = kzuProjectLoaderLoadImage(project, mipmapLevelPaths[i], &mipmapImages[i]);
                    kzsErrorForward(result);

                    result = kzuProjectRemoveObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, mipmapLevelPaths[i]);
                    kzsErrorForward(result);

                    kzcImageRemoveOwnership(mipmapImages[i]);
                }

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzcImageCreateMipmapImage(memoryManager, mipmapImages, &image);
            kzsErrorForward(result);

            result = kzcMemoryFreeArray(mipmapLevelPaths);
            kzsErrorForward(result);

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, image, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&image);
            kzsErrorForward(result);
        }
    }

    *out_image = image;
    kzsSuccess();

}

kzsException kzuProjectLoaderLoadImage(const struct KzuProject* project, kzString path, struct KzcImage** out_image)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcImage* image;

    kzsAssert(kzcIsValidPointer(project));
    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load image with null path");

    /* Check if the image is already loaded. */
    image = (struct KzcImage*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, path);

    if (!kzcImageIsValid(image))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        /*lint -e{788} Suppress warning about not listing all possible enum values. */
        switch (kzuBinaryFileInfoGetType(file))
        {
            case KZU_BINARY_FILE_TYPE_IMAGE_PNG:
            {
                result = kzuProjectLoaderLoadImagePng(project, kzuBinaryFileInfoGetPath(file), &image);
                kzsErrorForward(result);
                break;
            }

            case KZU_BINARY_FILE_TYPE_IMAGE_JPEG:
            {
                result = kzuProjectLoaderLoadImageJpeg(project, kzuBinaryFileInfoGetPath(file), &image);
                kzsErrorForward(result);
                break;
            }

            case KZU_BINARY_FILE_TYPE_IMAGE_ETC:
            {
                result = kzuProjectLoaderLoadImageETC(project, kzuBinaryFileInfoGetPath(file), &image);
                kzsErrorForward(result);
                break;
            }
            case KZU_BINARY_FILE_TYPE_IMAGE_DXT:
            {
                result = kzuProjectLoaderLoadImageDXT(project, kzuBinaryFileInfoGetPath(file), &image);
                kzsErrorForward(result);
                break;
            }

            case KZU_BINARY_FILE_TYPE_IMAGE_MIPMAP:
            {
                result = kzuProjectLoaderLoadImageMipmap(project, kzuBinaryFileInfoGetPath(file), &image);
                kzsErrorForward(result);
                break;
            }

            case KZU_BINARY_FILE_TYPE_IMAGE_RAW_1D:
            {
                KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Raw 1D image data not supported yet.");
            }

            case KZU_BINARY_FILE_TYPE_IMAGE_RAW_2D:
            {
                KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Raw 2D image data not supported yet.");
            }

            case KZU_BINARY_FILE_TYPE_IMAGE_RAW_3D:
            {
                KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Raw 3D image data not supported yet.");
            }

            default:
            {
                kzsErrorThrow(KZU_ERROR_WRONG_BINARY_FILE_TYPE, "Wrong file type encountered while trying to load image file.");
            }
        }

        result = kzcImageSetName(image, kzuBinaryFileInfoGetName(file));
        kzsErrorForward(result);

        kzsCounterIncrease("kzuProjectLoaderLoadImage");
    }

    *out_image = image;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadImage(struct KzuProject* project, const struct KzcImage* image)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, image);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
