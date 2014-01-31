/**
* \file
* Project loader for texture files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_texture.h"

#include "kzu_project_loader.h"
#include "kzu_project_gpu_memory_type.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_image.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/util/image/kzc_image.h>
#include <core/util/string/kzc_string.h>
#include <core/kzc_error_codes.h>

#include <system/wrappers/kzs_math.h>
#include <system/debug/kzs_log.h>


/** Helper function for loading texture data. */
static kzsException kzuProjectLoaderLoadTextureData_internal(const struct KzuProject* project, kzString path,
                                                             struct KzcTexture** out_texture);


enum KzuBinaryTextureType
{
    KZU_BINARY_TEXTURE_TYPE_1D = 0,
    KZU_BINARY_TEXTURE_TYPE_2D = 1,
    KZU_BINARY_TEXTURE_TYPE_3D = 2,
    KZU_BINARY_TEXTURE_TYPE_CUBE_MAP = 3,
    KZU_BINARY_TEXTURE_TYPE_RENDER_TARGET = 4,
    KZU_BINARY_TEXTURE_TYPE_GENERATED = 5
};

enum KzuBinaryTextureFormat
{
    KZU_BINARY_TEXTURE_FORMAT_RGB_8 = 0,
    KZU_BINARY_TEXTURE_FORMAT_RGBA_8 = 1,
    KZU_BINARY_TEXTURE_FORMAT_RGB_FLOAT_16 = 2,
    KZU_BINARY_TEXTURE_FORMAT_RGBA_FLOAT_16 = 3,
    KZU_BINARY_TEXTURE_FORMAT_RGB_FLOAT_32 = 4,
    KZU_BINARY_TEXTURE_FORMAT_RGBA_FLOAT_32 = 5,
    KZU_BINARY_TEXTURE_FORMAT_ALPHA_8 = 6,
    KZU_BINARY_TEXTURE_FORMAT_ALPHA_FLOAT_16 = 7,
    KZU_BINARY_TEXTURE_FORMAT_ALPHA_FLOAT_32 = 8,
    KZU_BINARY_TEXTURE_FORMAT_RGB_565 = 9,
    KZU_BINARY_TEXTURE_FORMAT_GRAYSCALE_8 = 10
};

enum KzuBinaryTextureFilter
{
    KZU_BINARY_TEXTURE_FILTER_NEAREST = 0,
    KZU_BINARY_TEXTURE_FILTER_BILINEAR = 1,
    KZU_BINARY_TEXTURE_FILTER_TRILINEAR = 2,
    KZU_BINARY_TEXTURE_FILTER_MIPMAP_NEAREST = 3,
    KZU_BINARY_TEXTURE_FILTER_MIPMAP_BILINEAR = 4,
    KZU_BINARY_TEXTURE_FILTER_MIPMAP_TRILINEAR = 5
};

enum KzuBinaryTextureWrapMode
{
    KZU_BINARY_TEXTURE_WRAP_MODE_REPEAT = 0,
    KZU_BINARY_TEXTURE_WRAP_MODE_CLAMP = 1,
    KZU_BINARY_TEXTURE_WRAP_MODE_REPEAT_MIRRORED = 2
};

static kzsError kzuProjectLoaderGetImageFormat_internal(enum KzuBinaryTextureFormat binaryTextureFormat, enum KzcImageDataFormat* out_imageFormat)
{
    /* TODO: What is this doing..? Does not seem to work at all with float formats. */
    enum KzcImageDataFormat imageFormat;
    switch (binaryTextureFormat)
    {
        case KZU_BINARY_TEXTURE_FORMAT_RGB_8: imageFormat = KZC_IMAGE_DATA_FORMAT_RGB_888; break;
        case KZU_BINARY_TEXTURE_FORMAT_RGBA_8: imageFormat = KZC_IMAGE_DATA_FORMAT_RGBA_8888; break;
        case KZU_BINARY_TEXTURE_FORMAT_RGB_FLOAT_16: imageFormat = KZC_IMAGE_DATA_FORMAT_RGB_888; break;
        case KZU_BINARY_TEXTURE_FORMAT_RGB_FLOAT_32: imageFormat = KZC_IMAGE_DATA_FORMAT_RGB_888; break;
        case KZU_BINARY_TEXTURE_FORMAT_RGBA_FLOAT_16: imageFormat = KZC_IMAGE_DATA_FORMAT_RGB_888; break;
        case KZU_BINARY_TEXTURE_FORMAT_RGBA_FLOAT_32: imageFormat = KZC_IMAGE_DATA_FORMAT_RGB_888; break;
        case KZU_BINARY_TEXTURE_FORMAT_ALPHA_8: imageFormat = KZC_IMAGE_DATA_FORMAT_ALPHA_8; break;
        case KZU_BINARY_TEXTURE_FORMAT_ALPHA_FLOAT_16: imageFormat = KZC_IMAGE_DATA_FORMAT_RGB_888; break;
        case KZU_BINARY_TEXTURE_FORMAT_ALPHA_FLOAT_32: imageFormat = KZC_IMAGE_DATA_FORMAT_RGB_888; break;
        case KZU_BINARY_TEXTURE_FORMAT_RGB_565: imageFormat = KZC_IMAGE_DATA_FORMAT_RGB_565;  break;
        case KZU_BINARY_TEXTURE_FORMAT_GRAYSCALE_8: imageFormat = KZC_IMAGE_DATA_FORMAT_GRAYSCALE_8;  break;
        default: kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid binary texture format");
    }
    *out_imageFormat = imageFormat;
    kzsSuccess();
}

static kzsError kzuProjectLoaderGetTextureFormat_internal(enum KzuBinaryTextureFormat binaryTextureFormat, enum KzcTextureFormat* out_textureFormat)
{
    /* TODO: What is this doing..? Does not seem to work at all with float formats. */
    enum KzcTextureFormat textureFormat;
    switch (binaryTextureFormat)
    {
    case KZU_BINARY_TEXTURE_FORMAT_RGB_8: textureFormat = KZC_TEXTURE_FORMAT_RGB; break;
    case KZU_BINARY_TEXTURE_FORMAT_RGBA_8: textureFormat = KZC_TEXTURE_FORMAT_RGBA; break;
    case KZU_BINARY_TEXTURE_FORMAT_RGB_FLOAT_16: textureFormat = KZC_TEXTURE_FORMAT_RGB; break;
    case KZU_BINARY_TEXTURE_FORMAT_RGB_FLOAT_32: textureFormat = KZC_TEXTURE_FORMAT_RGB; break;
    case KZU_BINARY_TEXTURE_FORMAT_RGBA_FLOAT_16: textureFormat = KZC_TEXTURE_FORMAT_RGBA; break;
    case KZU_BINARY_TEXTURE_FORMAT_RGBA_FLOAT_32: textureFormat = KZC_TEXTURE_FORMAT_RGBA; break;
    case KZU_BINARY_TEXTURE_FORMAT_ALPHA_8: textureFormat = KZC_TEXTURE_FORMAT_ALPHA; break;
    case KZU_BINARY_TEXTURE_FORMAT_ALPHA_FLOAT_16: textureFormat = KZC_TEXTURE_FORMAT_ALPHA; break;
    case KZU_BINARY_TEXTURE_FORMAT_ALPHA_FLOAT_32: textureFormat = KZC_TEXTURE_FORMAT_ALPHA; break;
    case KZU_BINARY_TEXTURE_FORMAT_GRAYSCALE_8: textureFormat = KZC_TEXTURE_FORMAT_LUMINANCE; break;
    case KZU_BINARY_TEXTURE_FORMAT_RGB_565: textureFormat = KZC_TEXTURE_FORMAT_RGB565;  break;
    default: kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid binary texture format");
    }
    *out_textureFormat = textureFormat;
    kzsSuccess();
}

static kzsError kzuProjectLoaderGetTextureFilter_internal(enum KzuBinaryTextureFilter binaryTextureFilter, enum KzcTextureFilter* out_textureFilter)
{
    enum KzcTextureFilter textureFilter;

    switch (binaryTextureFilter)
    {
        case KZU_BINARY_TEXTURE_FILTER_NEAREST: textureFilter = KZC_TEXTURE_FILTER_POINT_SAMPLE; break;
        case KZU_BINARY_TEXTURE_FILTER_BILINEAR: textureFilter = KZC_TEXTURE_FILTER_BILINEAR; break;
        case KZU_BINARY_TEXTURE_FILTER_TRILINEAR: textureFilter = KZC_TEXTURE_FILTER_TRILINEAR; break;
        case KZU_BINARY_TEXTURE_FILTER_MIPMAP_NEAREST: textureFilter = KZC_TEXTURE_FILTER_MIPMAP; break;
        case KZU_BINARY_TEXTURE_FILTER_MIPMAP_BILINEAR: KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Texture Filter MIPMAP_BILINEAR not supported yet.");
        case KZU_BINARY_TEXTURE_FILTER_MIPMAP_TRILINEAR: KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Texture Filter MIPMAP_TRILINEAR not supported yet.");
        default: kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid binary texture filter");
    }

    *out_textureFilter = textureFilter;
    kzsSuccess();
}

static kzsError kzuProjectLoaderGetTextureWrapMode_internal(enum KzuBinaryTextureWrapMode binaryTextureWrapMode, enum KzcTextureWrap* out_textureWrapMode)
{
    enum KzcTextureWrap textureWrapMode;

    switch (binaryTextureWrapMode)
    {
        case KZU_BINARY_TEXTURE_WRAP_MODE_REPEAT: textureWrapMode = KZC_TEXTURE_WRAP_REPEAT; break;
        case KZU_BINARY_TEXTURE_WRAP_MODE_CLAMP: textureWrapMode = KZC_TEXTURE_WRAP_CLAMP; break;
        case KZU_BINARY_TEXTURE_WRAP_MODE_REPEAT_MIRRORED: KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Texture Clamp mode REPEAT_MIRROR not supported yet.");
        default: kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid binary texture wrap mode");
    }

    *out_textureWrapMode = textureWrapMode;
    kzsSuccess();
}


struct KzcTextureStreamStructure_internal
{
    struct KzuProject* project;
    kzString path;
};

static kzsException kzuProjectLoaderStreamedTexture_internal(struct KzcTexture* oldTexture, void* userData)
{
    kzsError result;
    struct KzcTextureStreamStructure_internal* internalData = (struct KzcTextureStreamStructure_internal*)userData;
    struct KzuProject* project = internalData->project;
    struct KzuBinaryFileInfo* file;
    kzString path = internalData->path;
    struct KzcTexture* texture;

    result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
    kzsErrorForward(result);

    result = kzcTexturePrepareLoadOnDemand(oldTexture);
    kzsErrorForward(result);

    result = kzuProjectLoaderLoadTextureData_internal(project, path, &texture);
    kzsErrorForward(result);

    result = kzcTextureEndLoadOnDemand(oldTexture, texture);
    kzsErrorForward(result);

    result = kzcTextureSetName(oldTexture, kzuBinaryFileInfoGetName(file));
    kzsErrorForward(result);

    /*result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_TEXTURE, oldTexture, path,
        (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&oldTexture);
    kzsErrorForward(result);*/

    result = kzcMemoryFreeVariable(internalData);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsException kzuProjectLoaderLoadTextureData_internal(const struct KzuProject* project, kzString path,
                                                             struct KzcTexture** out_texture)
{
    kzsError result;
    struct KzuBinaryFileInfo* file;
    struct KzcTexture* texture;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);

    /* Get file info from directory corresponding the given path. */
    result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
    kzsExceptionForward(result);

    kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_TEXTURE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
        "Wrong file type encountered while trying to load texture file.");

    /* Load the texture of the file info. */
    {
        /* Read data */
        {
            struct KzcInputStream* inputStream;
            enum KzuBinaryTextureFormat format;
            enum KzuBinaryTextureType textureType;
            enum KzcImageDataFormat imageFormat;
            enum KzcTextureFilter textureFilter;
            enum KzcTextureWrap textureWrapMode;
            enum KzcResourceMemoryType resourceMemoryType;
            enum KzcTextureCompression textureCompression = KZC_TEXTURE_COMPRESSION_NONE;

            {
                enum KzuBinaryTextureFilter filter;
                enum KzuBinaryTextureWrapMode wrapMode;
                enum KzuBinaryGpuMemoryType memoryType;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&textureType);
                kzsErrorForward(result);
                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&format);
                kzsErrorForward(result);
                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&filter);
                kzsErrorForward(result);
                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&wrapMode);
                kzsErrorForward(result);
                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&memoryType);
                kzsErrorForward(result);

                result = kzuProjectLoaderGetImageFormat_internal(format, &imageFormat);
                kzsErrorForward(result);
                result = kzuProjectLoaderGetTextureFilter_internal(filter, &textureFilter);
                kzsErrorForward(result);
                result = kzuProjectLoaderGetTextureWrapMode_internal(wrapMode, &textureWrapMode);
                kzsErrorForward(result);
                result = kzuProjectLoaderGetMemoryType_private(memoryType, &resourceMemoryType);
                kzsErrorForward(result);
            };

            switch (textureType)
            {
                case KZU_BINARY_TEXTURE_TYPE_1D:
                {
                    KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("1 Dimensional Texture not supported yet.");
                }

                case KZU_BINARY_TEXTURE_TYPE_2D:
                {
                    kzString imagePath;
                    struct KzcImage* image;
                    struct KzcImage* convertedImage;
                    kzBool deleteImage = KZ_TRUE;
                    kzBool imageConverted = KZ_FALSE;

                    result = kzuBinaryDirectoryReadReference(inputStream, file, &imagePath);
                    kzsErrorForward(result);

                    if (imagePath == KZ_NULL)
                    {
                        /* Use default texture. */
                        imagePath = "Resource Files/Images/Default Texture Image";
                    }

                    /* If this image was found from project previously (by application developer), it can not be released for texture. */
                    if(kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, imagePath) != KZ_NULL)
                    {
                        deleteImage = KZ_FALSE;
                    }

                    result = kzuProjectLoaderLoadImage(project, imagePath, &image);
                    kzsErrorForward(result);

                    if(kzcImageGetWidth(image) > kzcTextureGetMaximumSize() ||
                        kzcImageGetHeight(image) > kzcTextureGetMaximumSize())
                    {
                        kzMutableString warningString;
                        kzUint maximumTextureSize = kzcTextureGetMaximumSize();

                        result = kzcStringFormat(kzcMemoryGetManager(image), "Given image %s is too big (%d, %d), maximum size is: (%d, %d). Fall-back to default image.", &warningString,
                            imagePath, kzcImageGetWidth(image), kzcImageGetHeight(image), maximumTextureSize, maximumTextureSize);
                        kzsErrorForward(result);
                        kzsLog(KZS_LOG_LEVEL_WARNING, warningString);
                        result = kzcStringDelete(warningString);
                        kzsErrorForward(result);

                        result = kzcImageDelete(image);
                        kzsErrorForward(result);

                        imagePath = "Resource Files/Images/Default Texture Image";

                        result = kzuProjectLoaderLoadImage(project, imagePath, &image);
                        kzsErrorForward(result);

                        /* If this image was found from project previously (by application developer), it can not be released for texture. */
                        if(kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, imagePath) != KZ_NULL)
                        {
                            deleteImage = KZ_FALSE;
                        }

                        result = kzuProjectLoaderLoadImage(project, imagePath, &image);
                        kzsErrorForward(result);
                    }

                    /* Non-compressed image. */
                    if (!kzcImageIsCompressedFormat(image))
                    {
                        if(!kzcImageIsMipmapImage(image))
                        {
                            if(imageFormat != kzcImageGetDataFormat(image))
                            {
                                result = kzcImageCopy(memoryManager, image, &convertedImage);
                                kzsErrorForward(result);
                                result = kzcImageConvert(convertedImage, imageFormat);
                                kzsErrorForward(result);
                                imageConverted = KZ_TRUE;
                            }
                            else
                            {
                                imageConverted = KZ_FALSE;
                                convertedImage = image;
                            }
                        }
                        else
                        {
                            convertedImage = image;
                        }
                    }
                    else
                    {
                        kzBool supported;

                        convertedImage = image;
                        supported = kzcTextureGetCompressionFromImageFormat(imageFormat, &textureCompression);
                        if(!supported)
                        {
                            kzsErrorThrow(KZC_ERROR_TEXTURE_FORMAT_UNSUPPORTED, "Texture format unsupported");
                        }
                    }

                    result = kzcTextureCreateFromImage(kzuProjectGetResourceManager(project), resourceMemoryType,
                        textureFilter, textureWrapMode, textureCompression, convertedImage, &texture);
                    kzsErrorForward(result);

                    if (imageConverted)
                    {
                        result = kzcImageDelete(convertedImage);
                        kzsErrorForward(result);
                    }

                    if(deleteImage)
                    {
                        result = kzuProjectDeleteObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, imagePath);
                        kzsErrorForward(result);
                    }

                    break;
                }

                case KZU_BINARY_TEXTURE_TYPE_3D:
                {
                    KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("3d Textures not supported yet.");
                }

                case KZU_BINARY_TEXTURE_TYPE_CUBE_MAP:
                {
                    struct KzcImage* orderedImages[6];
                    struct KzcImage* convertedImages[6];
                    kzUint i;
                    kzUint referenceWidth = 0;
                    kzUint referenceHeight = 0;
                    kzString imagePath[6];
                    kzBool imageConverted[6] = { KZ_FALSE, KZ_FALSE, KZ_FALSE, KZ_FALSE, KZ_FALSE, KZ_FALSE };
                    kzBool deleteImage[6] = { KZ_TRUE, KZ_TRUE, KZ_TRUE, KZ_TRUE, KZ_TRUE, KZ_TRUE };

                    for (i = 0; i < 6; ++i)
                    {
                        struct KzcImage* image;
                        struct KzcImage* convertedImage;

                        result = kzuBinaryDirectoryReadReference(inputStream, file, &imagePath[i]);
                        kzsErrorForward(result);

                        if (imagePath[i] == KZ_NULL)
                        {
                            /* Use default texture. */
                            imagePath[i] = "Resource Files/Images/Default Texture Image";
                        }

                        /* If this image was found from project previously (by application developer), it can not be released for texture. */
                        if(kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, imagePath[i]) != KZ_NULL)
                        {
                            deleteImage[i] = KZ_FALSE;
                        }
                        result = kzuProjectLoaderLoadImage(project, imagePath[i], &image);
                        kzsErrorForward(result);

                        if(!kzcImageIsCompressedFormat(image))
                        {
                            if(!kzcImageIsMipmapImage(image))
                            {
                                result = kzcImageCopy(memoryManager, image, &convertedImage);
                                kzsErrorForward(result);
                                result = kzcImageConvert(convertedImage, imageFormat);
                                kzsErrorForward(result);
                                imageConverted[i] = KZ_TRUE;
                            }
                            else
                            {
                                convertedImage = image;
                            }

                            /* Check the width of first image, save to . */
                            if(i == 0)
                            {
                                referenceWidth = kzcImageGetWidth(convertedImage);
                                referenceHeight = kzcImageGetHeight(convertedImage);
                            }
                            else
                            {
                                if(kzcImageGetWidth(convertedImage) != referenceWidth || kzcImageGetWidth(convertedImage) != referenceHeight)
                                {
                                    result = kzcImageResize(convertedImage, referenceWidth, referenceHeight, KZC_IMAGE_RESIZE_FILTER_BILINEAR);
                                    kzsErrorForward(result);
                                }
                            }
                        }
                        else
                        {
                            kzBool supported;

                            convertedImage = image;
                            supported = kzcTextureGetCompressionFromImageFormat(imageFormat, &textureCompression);
                            if(!supported)
                            {
                                kzsErrorThrow(KZC_ERROR_TEXTURE_FORMAT_UNSUPPORTED, "Texture format unsupported");
                            }
                        }

                        convertedImages[i] = convertedImage;
                    }

                    orderedImages[KZC_TEXTURE_CUBE_MAP_POSITIVE_Y] = convertedImages[0];
                    orderedImages[KZC_TEXTURE_CUBE_MAP_NEGATIVE_Y] = convertedImages[1];
                    orderedImages[KZC_TEXTURE_CUBE_MAP_POSITIVE_Z] = convertedImages[2];
                    orderedImages[KZC_TEXTURE_CUBE_MAP_NEGATIVE_Z] = convertedImages[3];
                    orderedImages[KZC_TEXTURE_CUBE_MAP_NEGATIVE_X] = convertedImages[4];
                    orderedImages[KZC_TEXTURE_CUBE_MAP_POSITIVE_X] = convertedImages[5];

                    if(referenceWidth > kzcTextureGetMaximumCubemapSize() || referenceHeight > kzcTextureGetMaximumCubemapSize())
                    {
                        kzMutableString warningString;
                        kzUint maximumCubemapTextureSize = kzcTextureGetMaximumCubemapSize();

                        result = kzcStringFormat(kzcMemoryGetManager(orderedImages[0]), "Maximum cube map size is: (%d %d), given input data for cubemap is too big", &warningString,
                            maximumCubemapTextureSize, maximumCubemapTextureSize);
                        kzsErrorForward(result);
                        kzsLog(KZS_LOG_LEVEL_WARNING, warningString);
                        result = kzcStringDelete(warningString);
                        kzsErrorForward(result);
                        kzsErrorThrow(KZC_ERROR_TEXTURE_SIZE_UNSUPPORTED, "Assertion failed in cube map");
                    }

                    result = kzcTextureCubeMapCreateFromImages(kzuProjectGetResourceManager(project), resourceMemoryType,
                        textureFilter, textureWrapMode, textureCompression, orderedImages, &texture);
                    kzsErrorForward(result);

                    for(i = 0; i < 6; ++i)
                    {
                        struct KzcImage* image = kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, imagePath[i]);

                        if(imageConverted[i])
                        {
                            result = kzcImageDelete(convertedImages[i]);
                            kzsErrorForward(result);
                        }

                        if(image != KZ_NULL && deleteImage[i])
                        {
                            result = kzuProjectDeleteObject(project, KZU_PROJECT_OBJECT_TYPE_IMAGE, imagePath[i]);
                            kzsErrorForward(result);
                        }
                    }
                    break;
                }

                case KZU_BINARY_TEXTURE_TYPE_RENDER_TARGET:
                {
                    kzUint width;
                    kzUint height;
                    kzUint type;
                    kzString downscaleTargetPath;
                    enum KzcTextureFormat textureFormat;

                    {
                        result = kzcInputStreamReadU32Int(inputStream, &width);
                        kzsErrorForward(result);
                        result = kzcInputStreamReadU32Int(inputStream, &height);
                        kzsErrorForward(result);
                        result = kzcInputStreamReadU32Int(inputStream, &type);
                        kzsErrorForward(result);
                    }

                    result = kzuProjectLoaderGetTextureFormat_internal(format, &textureFormat);
                    kzsErrorForward(result);

                    result = kzcTextureCreateFrameBufferTexture(kzuProjectGetResourceManager(project),
                        resourceMemoryType, textureFormat, width, height, textureFilter, &texture);
                    kzsErrorForward(result);

                    result = kzuBinaryDirectoryReadReference(inputStream, file, &downscaleTargetPath);
                    kzsErrorForward(result);

                    /* Render target */
                    if (downscaleTargetPath != KZ_NULL)
                    {
                        struct KzcTexture* downsampleRenderTarget;
                        kzUint sampleWidth, sampleHeight;
                        kzUint downscaleWidth, downscaleHeight;
                        kzUint target = 0;

                        result = kzuProjectLoaderLoadTexture(project, downscaleTargetPath, &downsampleRenderTarget);
                        kzsErrorForward(result);

                        sampleWidth = kzcTextureGetWidth(texture);
                        sampleHeight = kzcTextureGetHeight(texture);
                        downscaleWidth = kzcTextureGetWidth(downsampleRenderTarget);
                        downscaleHeight = kzcTextureGetHeight(downsampleRenderTarget);

                        sampleWidth = kzsMaxU(sampleWidth / 2, downscaleWidth);
                        sampleHeight = kzsMaxU(sampleHeight / 2, downscaleHeight);

                        while (sampleWidth > downscaleWidth && sampleHeight > downscaleHeight)
                        {
                            struct KzcTexture* downsampleTargetTexture;
                            enum KzcTextureFilter filter = kzcTextureGetTextureFiltering(downsampleRenderTarget);
                            result = kzcTextureCreateFrameBufferTexture(kzuProjectGetResourceManager(project), KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, 
                                textureFormat, sampleWidth, sampleHeight, filter, &downsampleTargetTexture);
                            kzsErrorForward(result);
                            result = kzcTextureAddFrameBufferDownscaleTarget(texture, target, downsampleTargetTexture);
                            kzsErrorForward(result);

                            sampleWidth = kzsMaxU(sampleWidth / 2, downscaleWidth);
                            sampleHeight = kzsMaxU(sampleHeight / 2, downscaleHeight);

                            target++;
                        } 

                        result = kzcTextureAddFrameBufferDownscaleTarget(texture, target, downsampleRenderTarget);
                        kzsErrorForward(result);
                    }

                    break;
                }

                case KZU_BINARY_TEXTURE_TYPE_GENERATED:
                {
                    KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Generated Textures are not supported yet.");
                }

                default:
                {
                    kzsErrorThrow(KZU_ERROR_INVALID_BINARY_DATA, "Invalid texture type");
                }
            }

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);
        }

        result = kzcTextureSetName(texture, kzuBinaryFileInfoGetName(file));
        kzsErrorForward(result);
    }

    *out_texture = texture;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadTexture(const struct KzuProject* project, kzString path, struct KzcTexture** out_texture)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;

    struct KzcTexture* texture;

    kzsAssert(kzcIsValidPointer(project));
    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load texture with null path");

    /* Check if the texture is already loaded. */
    texture = (struct KzcTexture*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_TEXTURE, path);

    if (!kzcTextureIsValid(texture))
    {
        kzU32 binaryFileFlags;
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_TEXTURE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load texture file.");

        binaryFileFlags = kzuBinaryFileInfoGetFlags(file) & kzuBinaryDirectoryGetFileFlagMask(kzuProjectGetBinaryDirectory(project));
        if ((binaryFileFlags & KZU_BINARY_FILE_INFO_FLAG_LOAD_ON_DEMAND) != 0)
        {
            struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
            struct KzcTextureStreamStructure_internal* internalStructure;
            result = kzcTextureCreateOnDemand(memoryManager, &texture);
            kzsErrorForward(result);

            result = kzcMemoryAllocVariable(memoryManager, internalStructure, "StreamedTexture");
            kzsErrorForward(result);

            internalStructure->project = (void*)project;
            internalStructure->path = path;
            kzcTextureSetLoadOnDemandData(texture, internalStructure, kzuProjectLoaderStreamedTexture_internal);
        }
        else
        {
            result = kzuProjectLoaderLoadTextureData_internal(project, path, &texture);
            kzsErrorForward(result);
        }

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_TEXTURE, texture, path,
            (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&texture);
        kzsErrorForward(result);
    }

    *out_texture = texture;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadTexture(struct KzuProject* project, const struct KzcTexture* texture)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, texture);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
