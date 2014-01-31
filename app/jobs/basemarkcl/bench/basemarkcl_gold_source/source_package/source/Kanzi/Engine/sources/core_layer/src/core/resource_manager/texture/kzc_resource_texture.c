/**
 * \file
 * Texture resource.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_resource_texture.h"

#include <core/resource_manager/kzc_resource_manager.h>
#include <core/resource_manager/frame_buffer/kzc_resource_frame_buffer.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/renderer/kzc_renderer.h>
#include <core/util/image/kzc_image.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/util/string/kzc_string.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/kzc_error_codes.h>

#include <system/wrappers/kzs_opengl.h>
#include <system/wrappers/kzs_memory.h>
#include <system/debug/kzs_log.h>
#include <system/debug/kzs_counter.h>
#include <system/time/kzs_tick.h>


/* External handle not used. */
#define KZC_TEXTURE_EXTERNAL_HANDLE_NOT_USED -1

/**< Global variable storing ETC extension support. */ /*lint -e{956} Ignored warning for not multi thread safeness. */
static kzInt g_etcCompressionSupported = -1;
/**< Global variable storing DXT3 extension support. */ /*lint -e{956} Ignored warning for not multi thread safeness. */
static kzInt g_dxt3CompressionSupported = -1;
/**< Global variable storing DXT5 extension support. */ /*lint -e{956} Ignored warning for not multi thread safeness. */
static kzInt g_dxt5CompressionSupported = -1;


struct KzcTexture
{
    struct KzcTextureData* data;
    kzInt externalTextureHandle; /**< External texture handle. -1 by default, which means that not used. */
};

struct KzcTextureData
{
    kzBool selfOwned;
    struct KzcResourceManager* resourceManager; /**< Resource manager that contains this resource. */

    kzMutableString name; /**< Name of the texture. */

    enum KzcResourceMemoryType memoryType;      /**< Memory type of texture. */
    enum KzcTextureType textureType;            /**< type of the texture */

    struct KzcRenderer* boundRenderer;          /**< Renderer this texture has been bound. */
    kzUint textureHandle;                       /**< Texture GPU handle. */
    kzUint textureObjectType;                   /**< Type of the texture on GPU. (KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_CUBE_MAP, ..)*/
    struct KzcTextureDescriptor descriptor;     /**< Texture descriptor, includes the dimensions, color depth etc. of texture. */

    kzBool loadOnDemand; /**< Boolean indicating should texture be loaded on demand. */
    void* loadData; /**< Load on demand data. */
    KzcTextureLoadOnDemandData loadDataFunction; /**< Load on demand data fetch function pointer. */

    /* Type specific data. */
    union
    {
        struct
        {
            void* data;  /**< Texture data. */
            kzUint mipmapLevelCount; /**< Mipmap level count. */
            void** mipmapData; /**< Data for mipmap levels. Only valid if mipmaps are used. */
            kzUint* mipmapWidth; /**< Mipmap widths. Only valid if mipmaps are used. */
            kzUint* mipmapHeight; /**< Mipmap heights. Only valid if mipmaps are used. */
            kzUint compressedDataSize; /**< Size for compressed data, if compression in use. */
            kzUint* compressedMipmapDataSizes; /**< Size for compressed mipmap data sizes. */
        } texture2D;

        struct  
        {
            void* cubeMapData[6]; /**< Data for all sides, KZC_TEXTURE_CUBE_MAP_* */
            kzUint compressedDataSize; /**< Size for compressed data, if compression in use. */
            kzUint mipmapLevelCount; /**< Mipmap level count. */
            void** mipmapData[6]; /**< Mipmap data. */
        } textureCubeMap;

        struct
        {
            struct KzcFrameBuffer* frameBuffer;      /**< Reference to frame buffer. */
            kzBool frameBufferOwnedByTexture;         /**< Variable for defining if frame buffer is owned by this texture. */
            struct KzcDynamicArray* downscaleTargets; /**< Target texture for automatic downscaling or KZ_NULL if not used. <struct KzcTexture*> */
        } textureRenderTarget;
    } typeData;
};


/** Deploy CubeMap to GPU */
static kzsError kzcTextureCubemapDeployToGPU_internal(const struct KzcTexture* texture);
/** Deploys texture to GPU. */
static kzsError kzcTextureDeployToGPU_internal(const struct KzcTexture* texture, const void *data, kzUint targetTypeGPU,
                                               kzUint targetBindTypeGPU, kzBool cubeMap, kzUint cubemapSide);
/** Deploys uncompressed texture to GPU. */
static kzsError kzcTextureDeployToGPUUncompressed_internal(const struct KzcTexture* texture, const void *data, kzUint targetTypeGPU,
                                                           kzUint targetBindTypeGPU, kzBool cubeMap, kzUint cubeMapSide);
/** Deploys compressed texture to GPU. */
static kzsError kzcTextureDeployToGPUCompressed_internal(const struct KzcTexture* texture, const void *data, kzUint targetTypeGPU,
                                                         kzUint targetBindTypeGPU, kzBool cubeMap, kzUint cubeMapSide);
/** Gets bytes per pixel from texture format. */
kzUint kzcTextureGetBytesPerPixel_internal(enum KzcTextureFormat format);
/** Get data size for compressed texture format */
static kzUint kzcCompressedTextureDataSizeBytes_internal(kzUint width, kzUint height, enum KzcTextureCompression compressionFormat);
/** Creates texture with mipmaps. */
kzsError kzcTextureCreateWithMipmaps_internal(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType, 
                                              const struct KzcTextureDescriptor* descriptor, const void* const* data, const kzUint* widths,
                                              const kzUint* heights, const kzUint* dataSizes, struct KzcTexture** out_texture);


kzsError kzcTextureCreateOnDemand(const struct KzcMemoryManager* memoryManager, struct KzcTexture** out_texture)
{
    kzsError result;
    struct KzcTexture* texture;
    struct KzcTextureData* textureData;

    result = kzcMemoryAllocVariable(memoryManager, texture, "Texture");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, textureData, "Texture data");
    kzsErrorForward(result);

    texture->externalTextureHandle = KZC_TEXTURE_EXTERNAL_HANDLE_NOT_USED;
    texture->data = textureData;
    textureData->selfOwned = KZ_TRUE;
    textureData->loadOnDemand = KZ_TRUE;
    textureData->name = KZ_NULL;

    *out_texture = texture;
    kzsSuccess();
}

kzsError kzcTexturePrepareLoadOnDemand(struct KzcTexture* texture)
{
    kzsError result;

    result = kzcMemoryFreeVariable(texture->data);
    kzsErrorForward(result);
    texture->data = KZ_NULL;

    kzsSuccess();
}

/** Initializes texture data */
static kzsError kzcTextureCreate_internal(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType,
                                          enum KzcTextureType textureType, const struct KzcTextureDescriptor* descriptor,
                                          struct KzcTexture** out_texture)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzcTexture* texture;
    struct KzcTextureData* textureData;

    memoryManager = kzcMemoryGetManager(resourceManager);

    result = kzcMemoryAllocVariable(memoryManager, texture, "Texture");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, textureData, "Texture data");
    kzsErrorForward(result);

    texture->externalTextureHandle = KZC_TEXTURE_EXTERNAL_HANDLE_NOT_USED;
    texture->data = textureData;
    textureData->selfOwned = KZ_TRUE;
    textureData->resourceManager = resourceManager;
    textureData->boundRenderer = KZ_NULL;
    textureData->name = KZ_NULL;
    textureData->textureType = textureType;
    textureData->memoryType = memoryType;
    textureData->descriptor = *descriptor;
    textureData->loadOnDemand = KZ_FALSE;

#ifdef KZC_RESOURCE_FORCE_TO_RAM
    if(memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        textureData->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
#endif

    textureData->textureHandle = 0;
    textureData->textureObjectType = 0;
    switch(textureType)
    {
        case KZC_TEXTURE_TYPE_2D:
        {
            textureData->typeData.texture2D.data = KZ_NULL;
            textureData->typeData.texture2D.mipmapData = KZ_NULL;
            textureData->typeData.texture2D.mipmapLevelCount = 0;
            break;
        }
        case KZC_TEXTURE_TYPE_CUBEMAP:
        {
            textureData->typeData.textureCubeMap.cubeMapData[0] = KZ_NULL;
            textureData->typeData.textureCubeMap.cubeMapData[1] = KZ_NULL;
            textureData->typeData.textureCubeMap.cubeMapData[2] = KZ_NULL;
            textureData->typeData.textureCubeMap.cubeMapData[3] = KZ_NULL;
            textureData->typeData.textureCubeMap.cubeMapData[4] = KZ_NULL;
            textureData->typeData.textureCubeMap.cubeMapData[5] = KZ_NULL;
            textureData->typeData.textureCubeMap.mipmapData[0] = KZ_NULL;
            textureData->typeData.textureCubeMap.mipmapData[1] = KZ_NULL;
            textureData->typeData.textureCubeMap.mipmapData[2] = KZ_NULL;
            textureData->typeData.textureCubeMap.mipmapData[3] = KZ_NULL;
            textureData->typeData.textureCubeMap.mipmapData[4] = KZ_NULL;
            textureData->typeData.textureCubeMap.mipmapData[5] = KZ_NULL;
            textureData->typeData.textureCubeMap.mipmapLevelCount = 0;
            break;
        }
        case KZC_TEXTURE_TYPE_RENDERTARGET:
        {
            textureData->typeData.textureRenderTarget.frameBuffer = KZ_NULL;
            textureData->typeData.textureRenderTarget.frameBufferOwnedByTexture = KZ_FALSE;
            
            result = kzcDynamicArrayCreate(memoryManager, &textureData->typeData.textureRenderTarget.downscaleTargets);
            kzsErrorForward(result);
            break;
        }
    }

    *out_texture = texture;
    kzsSuccess();
}

kzsError kzcTextureCreateWithMipmaps_internal(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType, 
                                     const struct KzcTextureDescriptor* descriptor, const void* const* data, const kzUint* widths,
                                     const kzUint* heights, const kzUint* dataSizes, struct KzcTexture** out_texture)
{
    kzsError result;
    kzUint size;
    struct KzcMemoryManager* memoryManager;
    struct KzcTexture* texture;
    kzUint measurementStartTime = kzsTimeGetCurrentTimestamp();

    memoryManager = kzcMemoryGetManager(resourceManager);

    if(data == KZ_NULL)
    {
        result = kzcTextureCreate_internal(resourceManager, memoryType, KZC_TEXTURE_TYPE_RENDERTARGET, descriptor, &texture);
        kzsErrorForward(result);
    }
    else
    {
        result = kzcTextureCreate_internal(resourceManager, memoryType, KZC_TEXTURE_TYPE_2D, descriptor, &texture);
        kzsErrorForward(result);
    }

#ifdef KZC_RESOURCE_FORCE_TO_RAM
    if(memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
#endif


    size = kzcTextureDescriptorGetMemorySize(descriptor);

    if (data != KZ_NULL)
    {
        result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.texture2D.data, size, "Texture internal data");
        kzsErrorForward(result);
        kzsMemcpy(texture->data->typeData.texture2D.data, data[0], size);
        if(descriptor->compression == KZC_TEXTURE_COMPRESSION_ETC)
        {
            texture->data->typeData.texture2D.compressedDataSize = size;
        }
    }

    if (descriptor->compression == KZC_TEXTURE_COMPRESSION_ETC && 
        (descriptor->filter == KZC_TEXTURE_FILTER_MIPMAP || descriptor->filter == KZC_TEXTURE_FILTER_TRILINEAR))
    {
        kzUint i;

        result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.texture2D.mipmapData,
            kzcArrayLength(data) * sizeof(void*), "Mipmap level pointers etc");
        kzsErrorForward(result);
        texture->data->typeData.texture2D.mipmapLevelCount = kzcArrayLength(data) - 1;

        result = kzcMemoryAllocArray(memoryManager, texture->data->typeData.texture2D.mipmapWidth,
            kzcArrayLength(data), "Mipmap level widths");
        kzsErrorForward(result);
        result = kzcMemoryAllocArray(memoryManager, texture->data->typeData.texture2D.mipmapHeight,
            kzcArrayLength(data), "Mipmap level heights");
        kzsErrorForward(result);
        result = kzcMemoryAllocArray(memoryManager, texture->data->typeData.texture2D.compressedMipmapDataSizes,
            kzcArrayLength(data), "Compressed mipmap level sizes");
        kzsErrorForward(result);

        for(i = 0; i < texture->data->typeData.texture2D.mipmapLevelCount; ++i)
        {
            result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.texture2D.mipmapData[i], dataSizes[i + 1],
                "Mipmap level data");
            kzsErrorForward(result);

            texture->data->typeData.texture2D.mipmapWidth[i] = widths[i + 1];
            texture->data->typeData.texture2D.mipmapHeight[i] = heights[i + 1];
            texture->data->typeData.texture2D.compressedMipmapDataSizes[i] = dataSizes[i + 1];

            kzsAssert(data != KZ_NULL);
            kzsMemcpy(texture->data->typeData.texture2D.mipmapData[i], data[i + 1], dataSizes[i + 1]);
        }
    }
    /* Generate mipmap levels */
    else if(texture->data->textureType == KZC_TEXTURE_TYPE_2D &&
        (descriptor->filter == KZC_TEXTURE_FILTER_MIPMAP || descriptor->filter == KZC_TEXTURE_FILTER_TRILINEAR))
    {
        kzUint i;

        result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.texture2D.mipmapData,
            kzcArrayLength(data) * sizeof(void*), "Mipmap level pointers uncompressed");
        kzsErrorForward(result);
        texture->data->typeData.texture2D.mipmapLevelCount = kzcArrayLength(data) - 1;
        
        result = kzcMemoryAllocArray(memoryManager, texture->data->typeData.texture2D.mipmapWidth,
            kzcArrayLength(data), "Mipmap level widths");
        kzsErrorForward(result);
        result = kzcMemoryAllocArray(memoryManager, texture->data->typeData.texture2D.mipmapHeight,
            kzcArrayLength(data), "Mipmap level heights");
        kzsErrorForward(result);

        for(i = 0; i < texture->data->typeData.texture2D.mipmapLevelCount; ++i)
        {
            result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.texture2D.mipmapData[i], dataSizes[i + 1],
                "Mipmap level data");
            kzsErrorForward(result);

            texture->data->typeData.texture2D.mipmapWidth[i] = widths[i + 1];
            texture->data->typeData.texture2D.mipmapHeight[i] = heights[i + 1];

            kzsAssert(data != KZ_NULL);
            kzsMemcpy(texture->data->typeData.texture2D.mipmapData[i], data[i + 1], dataSizes[i + 1]);
        }
    }

    /* Deploy the texture to GPU memory */
    if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY || texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        if(texture->data->textureType == KZC_TEXTURE_TYPE_RENDERTARGET)
        {
            result = kzcTextureDeployToGPU_internal(texture, KZ_NULL, KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_2D, KZ_FALSE, 0);
            kzsErrorForward(result);
        }
        else
        {
            result = kzcTextureDeployToGPU_internal(texture, texture->data->typeData.texture2D.data, KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_2D, KZ_FALSE, 0);
            kzsErrorForward(result);
        }
    }

    result = kzcResourceManagerAddTexture(resourceManager, texture);
    kzsErrorForward(result);

    /* If the memory is intended to be GPU only, delete the RAM memory. */
    if (texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
        result = kzcTextureFreeRAMMemory(texture);
        kzsErrorForward(result);
    }

    kzcResourceManagerAddResourceCumulativeTime_private(resourceManager, measurementStartTime, KZC_RESOURCE_MEASUREMENT_TEXTURE);

    *out_texture = texture;
    kzsSuccess();
}

kzsError kzcTextureCreate(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType, 
                          struct KzcTextureDescriptor* descriptor, const void* data, struct KzcTexture** out_texture)
{
    kzsError result;
    kzUint size;
    struct KzcMemoryManager* memoryManager;
    struct KzcTexture* texture;
    kzUint measurementStartTime = kzsTimeGetCurrentTimestamp();

    memoryManager = kzcMemoryGetManager(resourceManager);

    if(data == KZ_NULL)
    {
        result = kzcTextureCreate_internal(resourceManager, memoryType, KZC_TEXTURE_TYPE_RENDERTARGET, descriptor, &texture);
        kzsErrorForward(result);
    }
    else
    {
        result = kzcTextureCreate_internal(resourceManager, memoryType, KZC_TEXTURE_TYPE_2D, descriptor, &texture);
        kzsErrorForward(result);
    }

    size = kzcTextureDescriptorGetMemorySize(descriptor);

    if (data != KZ_NULL)
    {
        result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.texture2D.data, size, "Texture internal data");
        kzsErrorForward(result);
        kzsMemcpy(texture->data->typeData.texture2D.data, data, size);
        if(descriptor->compression == KZC_TEXTURE_COMPRESSION_ETC)
        {
            texture->data->typeData.texture2D.compressedDataSize = size;
        }
    }

    /* Mipmaps can only be loaded from images. */
    if (descriptor->compression == KZC_TEXTURE_COMPRESSION_ETC && 
        (descriptor->filter == KZC_TEXTURE_FILTER_MIPMAP || descriptor->filter == KZC_TEXTURE_FILTER_TRILINEAR))
    {
        descriptor->filter = KZC_TEXTURE_FILTER_BILINEAR;
        texture->data->descriptor.filter = KZC_TEXTURE_FILTER_BILINEAR;
        kzsLog(KZS_LOG_LEVEL_WARNING, "ETC with no generated mipmaps was tried to be applied with mipmap. Reverting \
            to bilinear filtering");
    }

    /* Generate mipmap levels */
    if(texture->data->textureType == KZC_TEXTURE_TYPE_2D &&
        (descriptor->filter == KZC_TEXTURE_FILTER_MIPMAP || descriptor->filter == KZC_TEXTURE_FILTER_TRILINEAR))
    {
        kzUint i;
        struct KzcImage* image;
        struct KzcImage** mipmapLevels;
        enum KzcImageDataFormat imageFormat = kzcImageGetImageFormatFromTextureFormat(descriptor->format);
        kzUint width = descriptor->width;
        kzUint height = descriptor->height;
        result = kzcImageLoadMemory(memoryManager, width, height, imageFormat, data, &image);
        kzsErrorForward(result);

        result = kzcImageCreateMipmapLevels(memoryManager, image, &mipmapLevels);
        kzsErrorForward(result);

        result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.texture2D.mipmapData, kzcArrayLength(mipmapLevels) * sizeof(void*), "Mipmap level pointers");
        kzsErrorForward(result);
        result = kzcMemoryAllocArray(memoryManager, texture->data->typeData.texture2D.mipmapWidth, kzcArrayLength(mipmapLevels), "MipmapWidths");
        kzsErrorForward(result);
        result = kzcMemoryAllocArray(memoryManager, texture->data->typeData.texture2D.mipmapHeight, kzcArrayLength(mipmapLevels), "MipmapHeights");
        kzsErrorForward(result);
        texture->data->typeData.texture2D.mipmapLevelCount = kzcArrayLength(mipmapLevels);

        for(i = 0; i < kzcArrayLength(mipmapLevels); ++i)
        {
            kzUint mipmapLevelSize = kzcImageGetDataSize(mipmapLevels[i]);
            result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.texture2D.mipmapData[i], mipmapLevelSize,
                "Mipmap level data");
            kzsErrorForward(result);

            texture->data->typeData.texture2D.mipmapWidth[i] = kzcImageGetWidth(mipmapLevels[i]);
            texture->data->typeData.texture2D.mipmapHeight[i] = kzcImageGetHeight(mipmapLevels[i]);

            kzsMemcpy(texture->data->typeData.texture2D.mipmapData[i], kzcImageGetData(mipmapLevels[i]), mipmapLevelSize);

            result = kzcImageDelete(mipmapLevels[i]);
            kzsErrorForward(result);
        }

        result = kzcMemoryFreeArray(mipmapLevels);
        kzsErrorForward(result);
        
        result = kzcImageDelete(image);
        kzsErrorForward(result);
    }

    /* Deploy the texture to GPU memory */
    if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY || texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        if(texture->data->textureType == KZC_TEXTURE_TYPE_RENDERTARGET)
        {
            result = kzcTextureDeployToGPU_internal(texture, KZ_NULL, KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_2D, KZ_FALSE, 0);
            kzsErrorForward(result);
        }
        else
        {
            result = kzcTextureDeployToGPU_internal(texture, texture->data->typeData.texture2D.data, KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_2D, KZ_FALSE, 0);
            kzsErrorForward(result);
        }
    }

    result = kzcResourceManagerAddTexture(resourceManager, texture);
    kzsErrorForward(result);

    /* If the memory is intended to be GPU only, delete the RAM memory. */
    if (texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
        result = kzcTextureFreeRAMMemory(texture);
        kzsErrorForward(result);
    }

    kzcResourceManagerAddResourceCumulativeTime_private(resourceManager, measurementStartTime, KZC_RESOURCE_MEASUREMENT_TEXTURE);

    *out_texture = texture;
    kzsSuccess();
}

kzsError kzcTextureSetName(const struct KzcTexture* texture, kzString name)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(texture));

    /* Delete previous name if possible. */
    if(texture->data->name != KZ_NULL)
    {
        result = kzcStringDelete(texture->data->name);
        kzsErrorForward(result);
    }

    result = kzcStringCopy(kzcMemoryGetManager(texture), name, &texture->data->name);
    kzsErrorForward(result);

    kzsSuccess();
}

kzString kzcTextureGetName(const struct KzcTexture* texture)
{
    kzsAssert(kzcIsValidPointer(texture));
    return texture->data->name;
}

kzsError kzcTextureCubemapCreate(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType, 
                                 const struct KzcTextureDescriptor* descriptor,
                                 void* const* cubemapData, struct KzcTexture** out_texture)
{
    kzsError result;
    kzUint size;
    struct KzcMemoryManager* memoryManager;
    struct KzcTexture* texture;
    kzUint measurementStartTime = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(resourceManager));

    memoryManager = kzcMemoryGetManager(resourceManager);

    result = kzcTextureCreate_internal(resourceManager, memoryType, KZC_TEXTURE_TYPE_CUBEMAP, descriptor, &texture);
    kzsErrorForward(result);

    size = kzcTextureDescriptorGetMemorySize(descriptor);
    result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.textureCubeMap.cubeMapData[0], size, "Texture internal data");
    kzsErrorForward(result);
    kzsMemcpy(texture->data->typeData.textureCubeMap.cubeMapData[0], cubemapData[0], size);
    result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.textureCubeMap.cubeMapData[1], size, "Texture internal data");
    kzsErrorForward(result);
    kzsMemcpy(texture->data->typeData.textureCubeMap.cubeMapData[1], cubemapData[1], size);
    result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.textureCubeMap.cubeMapData[2], size, "Texture internal data");
    kzsErrorForward(result);
    kzsMemcpy(texture->data->typeData.textureCubeMap.cubeMapData[2], cubemapData[2], size);
    result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.textureCubeMap.cubeMapData[3], size, "Texture internal data");
    kzsErrorForward(result);
    kzsMemcpy(texture->data->typeData.textureCubeMap.cubeMapData[3], cubemapData[3], size);
    result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.textureCubeMap.cubeMapData[4], size, "Texture internal data");
    kzsErrorForward(result);
    kzsMemcpy(texture->data->typeData.textureCubeMap.cubeMapData[4], cubemapData[4], size);
    result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.textureCubeMap.cubeMapData[5], size, "Texture internal data");
    kzsErrorForward(result);
    kzsMemcpy(texture->data->typeData.textureCubeMap.cubeMapData[5], cubemapData[5], size);

    if(texture->data->descriptor.compression == KZC_TEXTURE_COMPRESSION_ETC)
    {
        texture->data->typeData.textureCubeMap.compressedDataSize = size;
    }

    /* Generate mipmap levels */
    if(texture->data->textureType == KZC_TEXTURE_TYPE_CUBEMAP &&
        (descriptor->filter == KZC_TEXTURE_FILTER_MIPMAP || descriptor->filter == KZC_TEXTURE_FILTER_TRILINEAR))
    {
        kzUint i, j;
        struct KzcImage* image;
        struct KzcImage** mipmapLevels;
        enum KzcImageDataFormat imageFormat = kzcImageGetImageFormatFromTextureFormat(descriptor->format);
        kzUint width = descriptor->width;
        kzUint height = descriptor->width;

        for(j = 0; j < 6; ++j)
        {
            result = kzcImageLoadMemory(memoryManager, width, height, imageFormat, texture->data->typeData.textureCubeMap.cubeMapData[j], &image);
            kzsErrorForward(result);

            result = kzcImageCreateMipmapLevels(memoryManager, image, &mipmapLevels);
            kzsErrorForward(result);

            result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.textureCubeMap.mipmapData[j], kzcArrayLength(mipmapLevels) * sizeof(void*), "Mipmap level pointers");
            kzsErrorForward(result);
            texture->data->typeData.textureCubeMap.mipmapLevelCount = kzcArrayLength(mipmapLevels);

            for(i = 0; i < kzcArrayLength(mipmapLevels); ++i)
            {
                kzUint mipmapLevelSize = kzcImageGetDataSize(mipmapLevels[i]);
                result = kzcMemoryAllocPointer(memoryManager, &texture->data->typeData.textureCubeMap.mipmapData[j][i], mipmapLevelSize,
                    "Mipmap level data");
                kzsErrorForward(result);

                kzsMemcpy(texture->data->typeData.textureCubeMap.mipmapData[j][i], kzcImageGetData(mipmapLevels[i]), mipmapLevelSize);

                result = kzcImageDelete(mipmapLevels[i]);
                kzsErrorForward(result);
            }

            result = kzcMemoryFreeArray(mipmapLevels);
            kzsErrorForward(result);

            result = kzcImageDelete(image);
            kzsErrorForward(result);
        }
    }

    if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY || texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        result = kzcTextureCubemapDeployToGPU_internal(texture);
        kzsErrorForward(result);
    }

    result = kzcResourceManagerAddTexture(resourceManager, texture);
    kzsErrorForward(result);

    /* If the memory is intended to be GPU only, delete the RAM memory. */
    if (texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
        result = kzcTextureFreeRAMMemory(texture);
        kzsErrorForward(result);
    }

    kzcResourceManagerAddResourceCumulativeTime_private(resourceManager, measurementStartTime, KZC_RESOURCE_MEASUREMENT_TEXTURE);

    *out_texture = texture;
    kzsSuccess();
}

kzBool kzcTextureGetCompressionFromImageFormat(enum KzcImageDataFormat imageDataFormat, enum KzcTextureCompression* out_textureCompression)
{
    kzBool formatIsSupported = KZ_TRUE;
    enum KzcTextureCompression textureCompression;

    /* Check the correct compression format based on image format. */
    switch(imageDataFormat)
    {
        case KZC_IMAGE_DATA_FORMAT_DXT3:
        {
#ifdef KZS_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
            textureCompression = KZC_TEXTURE_COMPRESSION_DXT3_RGBA;
            formatIsSupported = kzcTextureIsCompressionFormatSupported(textureCompression);
#else
            textureCompression = KZC_TEXTURE_COMPRESSION_NONE;
            formatIsSupported = KZ_FALSE;
#endif
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_DXT5:
        {
#ifdef KZS_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
            textureCompression = KZC_TEXTURE_COMPRESSION_DXT5_RGBA;
            formatIsSupported = kzcTextureIsCompressionFormatSupported(textureCompression);
#else
            textureCompression = KZC_TEXTURE_COMPRESSION_NONE;
            formatIsSupported = KZ_FALSE;
#endif
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_ETC:
        {
#if KZ_OPENGL_VERSION != KZ_OPENGL_2_1
            textureCompression = KZC_TEXTURE_COMPRESSION_ETC;
            formatIsSupported = kzcTextureIsCompressionFormatSupported(textureCompression);
#else
            textureCompression = KZC_TEXTURE_COMPRESSION_NONE;
            formatIsSupported = KZ_FALSE;
#endif
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_RGB_565:
        case KZC_IMAGE_DATA_FORMAT_RGB_888:
        case KZC_IMAGE_DATA_FORMAT_RGBA_8888:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_8:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_8:
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT16:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT16:
        default:
        {
            textureCompression = KZC_TEXTURE_COMPRESSION_NONE;
            formatIsSupported = KZ_TRUE;
            break;
        }
    }

    *out_textureCompression = textureCompression;
    return formatIsSupported;
}

kzsError kzcTextureCreateFromImage(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                                   enum KzcTextureFilter filter, enum KzcTextureWrap wrapMode, 
                                   enum KzcTextureCompression compression, const struct KzcImage* image, struct KzcTexture** out_texture)
{
    kzsError result;
    kzBool formatIsSupported;
    struct KzcTextureDescriptor descriptor;
    struct KzcTexture* texture;
    enum KzcTextureCompression textureCompression;

    kzsAssert(kzcIsValidPointer(image));

    formatIsSupported = kzcTextureGetCompressionFromImageFormat(kzcImageGetDataFormat(image), &textureCompression);

    /* If format is not supported, convert to rgb888 and create texture. */
    if(!formatIsSupported)
    {
        if(kzcImageGetTextureFormat(image) == KZC_TEXTURE_FORMAT_DXT3_RGBA)
        {
            kzsErrorThrow(KZC_ERROR_TEXTURE_FORMAT_UNSUPPORTED, "DXT3 format not supported by GL implementation");
        }
        else if(kzcImageGetTextureFormat(image) == KZC_TEXTURE_FORMAT_DXT5_RGBA)
        {
            kzsErrorThrow(KZC_ERROR_TEXTURE_FORMAT_UNSUPPORTED, "DXT5 format not supported by GL implementation");
        }
        else
        {
            struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(resourceManager);
            struct KzcImage* temporaryImage;

            result = kzcImageCopy(memoryManager, image, &temporaryImage);
            kzsErrorForward(result);

            result = kzcImageConvert(temporaryImage, KZC_IMAGE_DATA_FORMAT_RGB_888);
            kzsErrorForward(result);

            kzcTextureDescriptorSet(kzcImageGetWidth(temporaryImage), kzcImageGetHeight(temporaryImage),
                kzcImageGetTextureFormat(temporaryImage), filter, wrapMode, KZC_TEXTURE_COMPRESSION_NONE, &descriptor);

            result = kzcTextureCreate(resourceManager, memoryType, &descriptor, kzcImageGetData(temporaryImage), &texture);
            kzsErrorForward(result);

            result = kzcImageDelete(temporaryImage);
            kzsErrorForward(result);
        }
    }
    else
    {
        kzcTextureDescriptorSet(kzcImageGetWidth(image), kzcImageGetHeight(image),
            kzcImageGetTextureFormat(image), filter, wrapMode, textureCompression, &descriptor);

        if(kzcImageIsMipmapImage(image))
        {
            kzUint i;
            const void** data;
            kzUint* widths;
            kzUint* heights;
            kzUint* dataSizes;
            struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(image);

            result = kzcMemoryAllocArray(memoryManager, data, kzcImageGetMipmapLevelCount(image), "MipmapLevels");
            kzsErrorForward(result);
            result = kzcMemoryAllocArray(memoryManager, widths, kzcImageGetMipmapLevelCount(image), "MipmapLevels");
            kzsErrorForward(result);
            result = kzcMemoryAllocArray(memoryManager, heights, kzcImageGetMipmapLevelCount(image), "MipmapLevels");
            kzsErrorForward(result);
            result = kzcMemoryAllocArray(memoryManager, dataSizes, kzcImageGetMipmapLevelCount(image), "MipmapLevels");
            kzsErrorForward(result);

            for(i = 0; i < kzcImageGetMipmapLevelCount(image); ++i)
            {
                struct KzcImage* mipmapImage = kzcImageGetMipmapLevel(image, i);
                data[i] = kzcImageGetData(mipmapImage);
                widths[i] = kzcImageGetWidth(mipmapImage);
                heights[i] = kzcImageGetHeight(mipmapImage);
                dataSizes[i] = kzcImageGetDataSize(mipmapImage);
            }

            result = kzcTextureCreateWithMipmaps_internal(resourceManager, memoryType, 
                &descriptor, data, widths, heights, dataSizes, &texture);
            kzsErrorForward(result);

            result = kzcMemoryFreeArray((void*)data);
            kzsErrorForward(result);
            result = kzcMemoryFreeArray(widths);
            kzsErrorForward(result);
            result = kzcMemoryFreeArray(heights);
            kzsErrorForward(result);
            result = kzcMemoryFreeArray(dataSizes);
            kzsErrorForward(result);
        }
        else
        {
            result = kzcTextureCreate(resourceManager, memoryType, &descriptor, kzcImageGetData(image), &texture);
            kzsErrorForward(result);
        }
    }

    if(compression != textureCompression)
    {
        if(compression == KZC_TEXTURE_COMPRESSION_NONE)
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Could not use hinted compression mode. Defaulted to compressed texture!");
        }
        else
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Could not use hinted compression mode. Defaulted to uncompressed texture!");
        }
    }

    kzsCounterIncrease("textureCreateFromImage");

    *out_texture = texture;
    kzsSuccess();
}

kzsError kzcTextureCubeMapCreateFromImages(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                                           enum KzcTextureFilter filter, enum KzcTextureWrap wrapMode, 
                                           enum KzcTextureCompression compression, struct KzcImage* image[6], struct KzcTexture** out_textureCube)
{
    kzsError result;
    struct KzcTexture* textureCube;
    void* data[6];
    kzUint width, height;
    kzUint i;
    kzBool formatIsSupported = KZ_TRUE;

    struct KzcTextureDescriptor descriptor;
    enum KzcImageDataFormat imageDataFormat;

    imageDataFormat = kzcImageGetDataFormat(image[0]);
    
    /* Validate images. */
    for(i = 0; i < 6; ++i)
    {
        enum KzcImageDataFormat format;
        
        kzsAssert(kzcIsValidPointer(image[i]));

        format = kzcImageGetDataFormat(image[1]);
        kzsErrorTest(imageDataFormat == format, KZC_ERROR_IMAGE_INVALID, "Cube map initialization failed. Not all cube map images were using ETC compression");
    }

    /* First check the correct compression format based on image format. */
    switch(imageDataFormat)
    {
        case KZC_IMAGE_DATA_FORMAT_DXT3:
        {
#ifdef KZS_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
            formatIsSupported = kzcTextureIsCompressionFormatSupported(KZC_TEXTURE_COMPRESSION_DXT3_RGBA);
#else
            formatIsSupported = KZ_FALSE;
#endif
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_DXT5:
        {
#ifdef KZS_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
            formatIsSupported = kzcTextureIsCompressionFormatSupported(KZC_TEXTURE_COMPRESSION_DXT5_RGBA);
#else
            formatIsSupported = KZ_FALSE;
#endif
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_ETC:
        {
#if KZ_OPENGL_VERSION != KZ_OPENGL_2_1
            formatIsSupported = kzcTextureIsCompressionFormatSupported(KZC_TEXTURE_COMPRESSION_ETC);
#else
            formatIsSupported = KZ_FALSE;
#endif
            break;
        }
        case KZC_IMAGE_DATA_FORMAT_RGB_565:
        case KZC_IMAGE_DATA_FORMAT_RGB_888:
        case KZC_IMAGE_DATA_FORMAT_RGBA_8888:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_8:
        case KZC_IMAGE_DATA_FORMAT_ALPHA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_8:
        case KZC_IMAGE_DATA_FORMAT_GRAYSCALE_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGB_FLOAT16:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT:
        case KZC_IMAGE_DATA_FORMAT_RGBA_FLOAT16:
        default:
        {
            formatIsSupported = KZ_TRUE;
            break;
        }
    }

     /* If format is not supported, convert to rgb888 and create texture. */
    if(!formatIsSupported)
    {
        struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(resourceManager);
        struct KzcImage* temporaryImage[6];

        for(i = 0; i < 6; ++i)
        {
            result = kzcImageCopy(memoryManager, image[i], &temporaryImage[i]);
            kzsErrorForward(result);
            result = kzcImageConvert(temporaryImage[i], KZC_IMAGE_DATA_FORMAT_RGB_888);
            kzsErrorForward(result);
            kzcTextureDescriptorSet(kzcImageGetWidth(temporaryImage[i]), kzcImageGetHeight(temporaryImage[i]),
                kzcImageGetTextureFormat(temporaryImage[i]), filter, wrapMode, KZC_TEXTURE_COMPRESSION_NONE, &descriptor);

            data[i] = (void*)kzcImageGetData(temporaryImage[i]);
        }
        width = kzcImageGetWidth(temporaryImage[0]);
        height = kzcImageGetHeight(temporaryImage[0]);

        /* Generate cube map texture. */
        kzcTextureDescriptorSet(width, height, kzcImageGetTextureFormat(temporaryImage[0]), filter, wrapMode, KZC_TEXTURE_COMPRESSION_NONE, &descriptor);
        result = kzcTextureCubemapCreate(resourceManager, memoryType, &descriptor, data, &textureCube);
        kzsErrorForward(result);

        for(i = 0; i < 6; ++i)
        {            
            result = kzcImageDelete(temporaryImage[i]);
            kzsErrorForward(result);
        }
    }
    else
    {
        /* Load cube maps */
        {
            for(i = 0; i < 6; ++i)
            {
                data[i] = (void*)kzcImageGetData(image[i]);
            }

            /* Load width and height */
            width = kzcImageGetWidth(image[0]);
            height = kzcImageGetHeight(image[0]);
        }

        /* Generate cube map texture. */
        kzcTextureDescriptorSet(width, height, kzcImageGetTextureFormat(image[0]), filter, wrapMode, compression, &descriptor);
        result = kzcTextureCubemapCreate(resourceManager, memoryType, &descriptor, data, &textureCube);
        kzsErrorForward(result);
    }

    *out_textureCube = textureCube;
    kzsSuccess();
}

kzsError kzcTextureCreateTextureFromFrameBuffer(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType,
                                                struct KzcFrameBuffer* frameBuffer, enum KzcFrameBufferTargetType targetType,  
                                                enum KzcTextureFormat textureFormat, 
                                                enum KzcTextureFilter textureFilter, enum KzcFrameBufferDepthFormat depthFormat, 
                                                struct KzcTexture** out_textureTarget)
{
    kzsError result;
    struct KzcTexture* texture;
    struct KzcTextureDescriptor descriptor;

    kzsAssert(kzcIsValidPointer(frameBuffer));

    /* Log a warning on incorrect filtering mode */
    kzsErrorTest(textureFilter != KZC_TEXTURE_FILTER_MIPMAP, KZS_ERROR_ILLEGAL_ARGUMENT, "textureFilter type KZC_TEXTURE_FILTER_MIPMAP is forbidden.");

    /* Add new color target */
    switch(targetType)
    {
        case KZC_FRAME_BUFFER_TARGET_COLOR:
        {
            kzUint width;
            kzUint height;

            width = kzcFrameBufferGetWidth(frameBuffer);
            height = kzcFrameBufferGetHeight(frameBuffer);

            kzcTextureDescriptorSet(width, height, 
                textureFormat, textureFilter,
                KZC_TEXTURE_WRAP_CLAMP, KZC_TEXTURE_COMPRESSION_NONE, &descriptor);

            result = kzcTextureCreate(resourceManager, memoryType, &descriptor, KZ_NULL, &texture);
            kzsErrorForward(result);

            kzcFrameBufferAttachColorAndDepthToTexture(frameBuffer, depthFormat, texture);

            break;
        }
        case KZC_FRAME_BUFFER_TARGET_DEPTH:
        case KZC_FRAME_BUFFER_TARGET_INVALID:
        default:
        {
            KZS_NOT_IMPLEMENTED_YET_ERROR;
        }
    }

    texture->data->typeData.textureRenderTarget.frameBufferOwnedByTexture = KZ_FALSE;
    texture->data->typeData.textureRenderTarget.frameBuffer = frameBuffer;
    *out_textureTarget = texture;
    kzsSuccess();
}

kzsError kzcTextureCreateFrameBufferTexture(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, enum KzcTextureFormat format,
                                            kzUint width, kzUint height, enum KzcTextureFilter textureFilter, struct KzcTexture** out_textureTarget)
{
    kzsError result;
    struct KzcTexture* texture;
    struct KzcTextureDescriptor descriptor;
    struct KzcFrameBuffer* frameBuffer;
    kzUint measurementStartTime = kzsTimeGetCurrentTimestamp();

    /* Log a warning on incorrect filtering mode */
    kzsErrorTest(textureFilter != KZC_TEXTURE_FILTER_MIPMAP, KZS_ERROR_ILLEGAL_ARGUMENT,
                 "textureFilter type KZC_TEXTURE_FILTER_MIPMAP is forbidden.");

    result = kzcFrameBufferCreate(resourceManager, memoryType, width, height, &frameBuffer);
    kzsErrorForward(result);

    /* Add new color target */
    kzcTextureDescriptorSet(width, height, format, textureFilter,
                            KZC_TEXTURE_WRAP_CLAMP, KZC_TEXTURE_COMPRESSION_NONE, &descriptor);

    result = kzcTextureCreate(resourceManager, memoryType, &descriptor, KZ_NULL, &texture);
    kzsErrorForward(result);

    kzcFrameBufferBindTargetTexture(frameBuffer, texture);

    kzcFrameBufferAttachColorAndDepthToTexture(frameBuffer, KZC_FRAME_BUFFER_DEPTH_16, texture);

    texture->data->typeData.textureRenderTarget.frameBufferOwnedByTexture = KZ_TRUE;
    texture->data->typeData.textureRenderTarget.frameBuffer = frameBuffer;

    kzcResourceManagerAddResourceCumulativeTime_private(resourceManager, measurementStartTime, KZC_RESOURCE_MEASUREMENT_TEXTURE);

    *out_textureTarget = texture;
    kzsSuccess();
}

void kzcTextureDescriptorSet(kzUint width, kzUint height, enum KzcTextureFormat format, enum KzcTextureFilter filter,
                             enum KzcTextureWrap textureWrap, enum KzcTextureCompression compression,
                             struct KzcTextureDescriptor* textureDescriptor)
{
    textureDescriptor->width = width;
    textureDescriptor->height = height;
    textureDescriptor->compression = compression;
    textureDescriptor->filter = filter;
    textureDescriptor->format = format;
    textureDescriptor->wrap = textureWrap;
}

kzsError kzcTextureApply(const struct KzcTexture* texture, struct KzcRenderer* renderer)
{
    kzsError result;
    enum KzcRendererTextureType textureType = KZC_RENDERER_TEXTURE_TYPE_2D;

    kzsAssert(kzcTextureIsValid(texture));

    if(texture->externalTextureHandle != KZC_TEXTURE_EXTERNAL_HANDLE_NOT_USED)
    {
        result = kzcRendererApplyTexture(renderer, (kzUint)texture->externalTextureHandle, textureType, 0, 0);
        kzsErrorForward(result);
    }
    else
    {
        kzUint textureWidth;
        kzUint textureHeight;

        if(texture->data->loadOnDemand)
        {
            result = texture->data->loadDataFunction((struct KzcTexture*)texture, texture->data->loadData);
            kzsErrorForward(result);
            texture->data->loadOnDemand = KZ_FALSE;
        }

        textureWidth = kzcTextureGetWidth(texture);
        textureHeight = kzcTextureGetHeight(texture);

        texture->data->boundRenderer = renderer;
        if(texture->data->textureObjectType == KZS_GL_TEXTURE_2D)
        {
            textureType = KZC_RENDERER_TEXTURE_TYPE_2D;
        }
        else if(texture->data->textureObjectType == KZS_GL_TEXTURE_CUBE_MAP)
        {
            textureType = KZC_RENDERER_TEXTURE_TYPE_CUBEMAP;
        }
        result = kzcRendererApplyTexture(renderer, texture->data->textureHandle, textureType, textureWidth, textureHeight);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzcTextureDelete(struct KzcTexture* texture)
{
    kzsError result;
    struct KzcTextureData* textureData;
    kzBool selfOwned;
    
    kzsAssert(kzcTextureIsValid(texture));

    textureData = texture->data;
    selfOwned = textureData->selfOwned;

    if(!textureData->loadOnDemand)
    {
        kzcTextureFreeGPUMemory(texture);

        result = kzcTextureFreeRAMMemory(texture);
        kzsErrorForward(result);

        if(texture->data->name != KZ_NULL)
        {
            result = kzcStringDelete(texture->data->name);
            kzsErrorForward(result);
            texture->data->name = KZ_NULL;
        }

        if(texture->data->textureType == KZC_TEXTURE_TYPE_RENDERTARGET)
        {
                result = kzcDynamicArrayDelete(textureData->typeData.textureRenderTarget.downscaleTargets);
                kzsErrorForward(result);      
            if(texture->data->typeData.textureRenderTarget.frameBufferOwnedByTexture)
            {
                result = kzcFrameBufferFree(texture->data->typeData.textureRenderTarget.frameBuffer);
                kzsErrorForward(result);
            }
        }

        result = kzcResourceManagerRemoveTexture(texture->data->resourceManager, texture);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(textureData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(texture);
        kzsErrorForward(result);
    }
    else
    {
        texture->data = KZ_NULL;
    }

    kzsSuccess();
}

void kzcTextureTakeOwnership(const struct KzcTexture* texture)
{
    kzsAssert(kzcTextureIsValid(texture));

    texture->data->selfOwned = KZ_FALSE;
}

kzBool kzcTextureIsValid(const struct KzcTexture* texture)
{
    return texture != KZ_NULL && texture->data != KZ_NULL;
}

void kzcTextureUnbindRenderer(const struct KzcTexture* texture)
{
    kzsAssert(kzcIsValidPointer(texture));

    texture->data->boundRenderer = KZ_NULL;
}

kzsError kzcTextureTransferData(struct KzcTexture* targetTexture, struct KzcTexture* sourceTexture)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(targetTexture));
    kzsAssert(targetTexture->data == KZ_NULL);
    kzsAssert(kzcTextureIsValid(sourceTexture));

    targetTexture->data = sourceTexture->data;
    sourceTexture->data = KZ_NULL;

    result = kzcResourceManagerReplaceTexture(targetTexture->data->resourceManager, sourceTexture, targetTexture);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcTextureEndLoadOnDemand(struct KzcTexture* targetTexture, struct KzcTexture* sourceTexture)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(targetTexture));
    kzsAssert(targetTexture->data == KZ_NULL);
    kzsAssert(kzcTextureIsValid(sourceTexture));

    targetTexture->data = sourceTexture->data;

    result = kzcResourceManagerReplaceTexture(targetTexture->data->resourceManager, sourceTexture, targetTexture);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(sourceTexture);
    kzsErrorForward(result);
    kzcTextureTakeOwnership(targetTexture);

    kzsSuccess();
}

kzsError kzcTextureDetach(struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));

    kzcRendererDetachTexture(renderer);

    kzsSuccess();
}

void kzcTextureFreeGPUMemory(const struct KzcTexture* texture)
{
    kzsAssert(kzcTextureIsValid(texture));

    /* Remove the texture from GPU. */
    if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
        texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {

        /* If this texture was bound to renderer, remove it from texture slots. */
        if(texture->data->boundRenderer != KZ_NULL)
        {
            kzcRendererDetachTextureHandle(texture->data->boundRenderer, texture->data->textureHandle);
        }

        kzsGlDeleteTextures(1, &texture->data->textureHandle);
        
        /* Put invalid value to texture handle. */
        texture->data->textureHandle = 0;
        texture->data->textureObjectType = 0;

        if(texture->data->textureType == KZC_TEXTURE_TYPE_RENDERTARGET)
        {
            if(texture->data->typeData.textureRenderTarget.frameBuffer != KZ_NULL &&
               texture->data->typeData.textureRenderTarget.frameBufferOwnedByTexture)
            {
                kzcFrameBufferFreeGPUMemory(texture->data->typeData.textureRenderTarget.frameBuffer);
            }
        }

        kzsGlFinish();
        if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_NONE;
        }
        else
        {
            texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY;
        }
    }
}

kzsError kzcTextureFreeRAMMemory(const struct KzcTexture* texture)
{
    kzsError result;

    kzsAssert(kzcTextureIsValid(texture));

    /* Delete the data if it is allocated. */
    if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
       texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
    {
        switch(texture->data->textureType)
        {
            case KZC_TEXTURE_TYPE_2D:
            {
                if(texture->data->typeData.texture2D.mipmapData != KZ_NULL)
                {
                    kzUint i;
                    for(i = 0; i < texture->data->typeData.texture2D.mipmapLevelCount; ++i)
                    {
                        result = kzcMemoryFreePointer(texture->data->typeData.texture2D.mipmapData[i]);
                        kzsErrorForward(result);
                    }
                    result = kzcMemoryFreePointer(texture->data->typeData.texture2D.mipmapData);
                    kzsErrorForward(result);
                    result = kzcMemoryFreeArray(texture->data->typeData.texture2D.mipmapWidth);
                    kzsErrorForward(result);
                    result = kzcMemoryFreeArray(texture->data->typeData.texture2D.mipmapHeight);
                    kzsErrorForward(result);
                    if(texture->data->descriptor.compression == KZC_TEXTURE_COMPRESSION_ETC)
                    {
                        result = kzcMemoryFreeArray(texture->data->typeData.texture2D.compressedMipmapDataSizes);
                        kzsErrorForward(result);
                    }
                    
                    texture->data->typeData.texture2D.mipmapData = KZ_NULL;
                }

                result = kzcMemoryFreePointer(texture->data->typeData.texture2D.data);
                kzsErrorForward(result);

                texture->data->typeData.texture2D.data = KZ_NULL;
                break;
            }
            case KZC_TEXTURE_TYPE_CUBEMAP:
            {
                kzUint i;
                for(i = 0; i < 6; ++i)
                {
                    result = kzcMemoryFreePointer(texture->data->typeData.textureCubeMap.cubeMapData[i]);
                    kzsErrorForward(result);
                    texture->data->typeData.textureCubeMap.cubeMapData[i] = KZ_NULL;

                    if(texture->data->typeData.textureCubeMap.mipmapData[i] != KZ_NULL)
                    {
                        kzUint j;
                        for(j = 0; j < texture->data->typeData.textureCubeMap.mipmapLevelCount; ++j)
                        {
                            result = kzcMemoryFreePointer(texture->data->typeData.textureCubeMap.mipmapData[i][j]);
                            kzsErrorForward(result);
                        }
                        result = kzcMemoryFreePointer(texture->data->typeData.textureCubeMap.mipmapData[i]);
                        kzsErrorForward(result);
                    }
                }

                break;
            }

            case KZC_TEXTURE_TYPE_RENDERTARGET:
            {
                /* Nothing to be deleted from RAM. */
                break;
            }
            default:
            {
                kzsAssertText(KZ_FALSE, "Invalid texture type!");
            }
        }
        if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
        {
            texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY;
        }
        else if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_NONE;
        }
    }

    kzsSuccess();
}

kzsError kzcTextureRestoreGPU(const struct KzcTexture* texture)
{
    kzsError result;

    if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
    {
        switch(texture->data->textureType)
        {
            case KZC_TEXTURE_TYPE_2D:
            {
                result = kzcTextureDeployToGPU_internal(texture, texture->data->typeData.texture2D.data, KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_2D, KZ_FALSE, 0);
                kzsErrorForward(result);
                break;
            }
            case KZC_TEXTURE_TYPE_CUBEMAP:
            { 
                result = kzcTextureCubemapDeployToGPU_internal(texture);
                kzsErrorForward(result);
                break;
            }
            case KZC_TEXTURE_TYPE_RENDERTARGET:
            {
                result = kzcTextureDeployToGPU_internal(texture, KZ_NULL, KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_2D, KZ_FALSE, 0);
                kzsErrorForward(result);
                if(kzcFrameBufferGetMemoryType(texture->data->typeData.textureRenderTarget.frameBuffer) == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
                {
                    result = kzcFrameBufferRestoreGPU(texture->data->typeData.textureRenderTarget.frameBuffer);
                    kzsErrorForward(result);
                }
                kzcFrameBufferBindTargetTexture(texture->data->typeData.textureRenderTarget.frameBuffer, texture);
                kzcFrameBufferAttachColorAndDepthToTexture(texture->data->typeData.textureRenderTarget.frameBuffer, KZC_FRAME_BUFFER_DEPTH_16, texture);
                break;
            }
            default:
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown texturetype in kzcShaderRestoreGPU");
            }
        }

        texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
    else if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "This texture is already contained in GPU and RAM!");
    }
    else if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "This texture is already contained in GPU!");
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Cannot restore texture that isn't contained in RAM memory!");
    }

    kzsSuccess();
}

static kzsError kzcTextureCubemapDeployToGPU_internal(const struct KzcTexture* texture)
{
    kzsError result;
    kzUint i;
    const kzUint targets[6] = 
    { 
        KZS_GL_TEXTURE_CUBE_MAP_POSITIVE_X,  /**< Cube map face direction. */
        KZS_GL_TEXTURE_CUBE_MAP_NEGATIVE_X,  /**< Cube map face direction. */
        KZS_GL_TEXTURE_CUBE_MAP_POSITIVE_Y,  /**< Cube map face direction. */
        KZS_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,  /**< Cube map face direction. */
        KZS_GL_TEXTURE_CUBE_MAP_POSITIVE_Z,  /**< Cube map face direction. */
        KZS_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z   /**< Cube map face direction. */
    };

    /* Create texture handle */
    kzsGlGenTextures(1, &texture->data->textureHandle);
    texture->data->textureObjectType = KZS_GL_TEXTURE_CUBE_MAP;
    kzsGlBindTexture(KZS_GL_TEXTURE_CUBE_MAP, texture->data->textureHandle);

    /* Deploy all cube map textures to GPU memory */
    for(i = 0; i < 6; ++i)
    {
        result = kzcTextureDeployToGPU_internal(texture, texture->data->typeData.textureCubeMap.cubeMapData[i], targets[i], KZS_GL_TEXTURE_CUBE_MAP, KZ_TRUE, i);
        kzsErrorForward(result);
    }
    
    /* Unbind the cube map */
    kzsGlBindTexture(KZS_GL_TEXTURE_CUBE_MAP, 0);
    kzsSuccess();
}

static void kzcTextureGetFormat_internal(enum KzcTextureFormat format, kzUint* out_textureFormat, kzInt* out_internalFormat,
                                         kzUint* out_dataFormat)
{
    kzInt internalFormat = 0;
    kzUint textureFormat = 0;
    kzUint dataFormat = 0;
    
    switch(format)
    {
        case KZC_TEXTURE_FORMAT_RGB:
        {
            textureFormat = KZS_GL_RGB;
#if KZ_OPENGL_VERSION == KZ_OPENGL_2_1
            internalFormat = KZS_GL_RGB8;
#else
            internalFormat = KZS_GL_RGB;
#endif
            dataFormat = KZS_GL_UNSIGNED_BYTE;
            break;
        }
        case KZC_TEXTURE_FORMAT_RGBA:
        {
            textureFormat = KZS_GL_RGBA;
#if KZ_OPENGL_VERSION == KZ_OPENGL_2_1
            internalFormat = KZS_GL_RGBA8;
#else
            internalFormat = KZS_GL_RGBA;
#endif
            dataFormat = KZS_GL_UNSIGNED_BYTE;
            break;
        }
        case KZC_TEXTURE_FORMAT_ALPHA:
        {
            textureFormat = KZS_GL_ALPHA;
#if KZ_OPENGL_VERSION == KZ_OPENGL_2_1
            internalFormat = KZS_GL_ALPHA8;
#else
            internalFormat = KZS_GL_ALPHA;
#endif
            dataFormat = KZS_GL_UNSIGNED_BYTE;
            break;
        }
        case KZC_TEXTURE_FORMAT_LUMINANCE:
        {
            textureFormat = KZS_GL_LUMINANCE;
#if KZ_OPENGL_VERSION == KZ_OPENGL_2_1
            internalFormat = KZS_GL_LUMINANCE8;
#else
            internalFormat = KZS_GL_LUMINANCE;
#endif
            dataFormat = KZS_GL_UNSIGNED_BYTE;
            break;
        }
        case KZC_TEXTURE_FORMAT_RGB565:
        {
            textureFormat = KZS_GL_RGB;
#if KZ_OPENGL_VERSION == KZ_OPENGL_2_1
            internalFormat = KZS_GL_RGB8;
#else
            internalFormat = KZS_GL_RGB;
#endif
            dataFormat = KZS_GL_UNSIGNED_SHORT_5_6_5;
            break;            
        }
        case KZC_TEXTURE_FORMAT_DXT3_RGBA:
        case KZC_TEXTURE_FORMAT_DXT5_RGBA:
        case KZC_TEXTURE_FORMAT_ETC:
        default:
        {
            kzsAssertText(KZ_FALSE, "Unknown texture format!");
        }
    }

    if(out_textureFormat != KZ_NULL)
    {
        *out_textureFormat = textureFormat;
    }
    if(out_internalFormat != KZ_NULL)
    {
        *out_internalFormat = internalFormat;
    }
    if(out_dataFormat != KZ_NULL)
    {
        *out_dataFormat = dataFormat;
    }
}

static kzsError kzcTextureDeployToGPUUncompressed_internal(const struct KzcTexture* texture, const void *data, kzUint targetTypeGPU,
                                                           kzUint targetBindTypeGPU, kzBool cubeMap, kzUint cubeMapSide)
{
    kzInt mipmapLevel = 0;
    kzInt internalFormat;
    kzUint textureFormat;
    kzUint dataFormat;

    kzsAssert(data == KZ_NULL || kzcIsValidPointer(data));
    kzsAssert(texture->data->descriptor.compression == KZC_TEXTURE_COMPRESSION_NONE);

    if(!cubeMap)
    {        
        kzsGlDeleteTextures(1, &texture->data->textureHandle);
        kzsGlGenTextures(1, &texture->data->textureHandle);
        texture->data->textureObjectType = targetBindTypeGPU;
        kzsGlBindTexture(targetBindTypeGPU, texture->data->textureHandle);
    }

    kzcTextureGetFormat_internal(texture->data->descriptor.format, &textureFormat, &internalFormat, &dataFormat);

    kzsGlTexImage2D(targetTypeGPU, mipmapLevel, (kzUint)internalFormat, (kzInt)texture->data->descriptor.width, 
        (kzInt)texture->data->descriptor.height, (kzInt)0, textureFormat, dataFormat, data);
    
    if((texture->data->textureType == KZC_TEXTURE_TYPE_2D || texture->data->textureType == KZC_TEXTURE_TYPE_CUBEMAP) &&
       (texture->data->descriptor.filter == KZC_TEXTURE_FILTER_MIPMAP || texture->data->descriptor.filter == KZC_TEXTURE_FILTER_TRILINEAR)
        && dataFormat != KZS_GL_UNSIGNED_SHORT_5_6_5) /* TODO: Allow RGB565 mipmaps in future. */
    {
        kzUint width = texture->data->descriptor.width / 2;
        kzUint height = texture->data->descriptor.height / 2;
        kzUint mipmapLevelCount = texture->data->textureType == KZC_TEXTURE_TYPE_2D ? texture->data->typeData.texture2D.mipmapLevelCount :
            texture->data->typeData.textureCubeMap.mipmapLevelCount;
        mipmapLevelCount = (kzUint)kzsMax(0, (kzInt)mipmapLevelCount - 1);

        while(width >= 1 || height >= 1)
        {
            kzUint level = kzsMinU((kzUint)mipmapLevel, mipmapLevelCount);
            kzUint textureWidth = kzsMaxU(1, width);
            kzUint textureHeight = kzsMaxU(1, height);

            if(texture->data->textureType == KZC_TEXTURE_TYPE_CUBEMAP)
            {
                kzsGlTexImage2D(targetTypeGPU, mipmapLevel + 1, (kzUint)internalFormat,
                    (kzInt)textureWidth, (kzInt)textureHeight, 
                    (kzInt)0, textureFormat, KZS_GL_UNSIGNED_BYTE, texture->data->typeData.textureCubeMap.mipmapData[cubeMapSide][level]);
            }
            else
            {
                kzsGlTexImage2D(targetTypeGPU, mipmapLevel + 1, (kzUint)internalFormat,
                    (kzInt)textureWidth, (kzInt)textureHeight, 
                    (kzInt)0, textureFormat, KZS_GL_UNSIGNED_BYTE, texture->data->typeData.texture2D.mipmapData[level]);
            }
            ++mipmapLevel;
            width /= 2;
            height /= 2;
        }
    }

    kzsSuccess();
}

kzsError kzcTextureUpdateData(const struct KzcTexture* texture, const void* data)
{
    kzsError result;
    if(texture->data->textureType == KZC_TEXTURE_TYPE_2D)
    {
        result = kzcTextureDeployToGPU_internal(texture, data, KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_2D, KZ_FALSE, 0);
        kzsErrorForward(result);
    }
    
    kzsSuccess();
}

kzsError kzcTextureBlendData(const struct KzcTexture* texture, const void* data, kzInt x, kzInt y, kzUint width, kzUint height,
                             const struct KzcColorRGBA* colorMultiply, enum KzcTextureFormat format)
{
    kzsError result;
    if(texture->data->textureType == KZC_TEXTURE_TYPE_2D)
    {
        kzUint textureFormat;
        kzUint dataFormat;

        kzcTextureGetFormat_internal(texture->data->descriptor.format, &textureFormat, KZ_NULL, &dataFormat);

        /* Upload subimage to RAM, too. */
        if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
        {
            if(dataFormat == KZS_GL_UNSIGNED_BYTE)
            {
                kzUint j;
                
                kzU8* textureData = (kzU8*)texture->data->typeData.texture2D.data;
                kzU8* sourceData = (kzU8*)data;

                if(textureFormat == KZS_GL_RGB)
                {
                    kzUint bitsPerPixel = 3;
                    
                    if(format == KZC_TEXTURE_FORMAT_ALPHA)
                    {
                        for(j = 0; j < height; ++j)
                        {
                            kzInt targetY = ((kzInt)j + y);
                            kzUint i;
                            for(i = 0; i < width; ++i)
                            {
                                kzInt targetX = ((kzInt)i + x);
                                if(targetY >= 0 && targetY < (kzInt)texture->data->descriptor.height &&
                                    targetX >= 0 && targetX < (kzInt)texture->data->descriptor.width)
                                {
                                    kzUint alpha = (kzUint)sourceData[(j * width + i)];
                                    kzUint newColorRed =
                                        ((kzUint)textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel] * (255 - alpha)
                                        + (kzUint)((kzFloat)alpha * colorMultiply->red * 255)) >> 8;
                                    kzUint newColorGreen =
                                        ((kzUint)textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 1] * (255 - alpha)
                                        + (kzUint)((kzFloat)alpha * colorMultiply->green * 255)) >> 8;
                                    kzUint newColorBlue =
                                        ((kzUint)textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 2] * (255 - alpha)
                                        + (kzUint)((kzFloat)alpha * colorMultiply->blue * 255)) >> 8;

                                    textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel] = (kzU8)newColorRed;
                                    textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 1] = (kzU8)newColorGreen;
                                    textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 2] = (kzU8)newColorBlue;
                                }

                            }
                        }

                        kzcTextureFreeGPUMemory(texture);
                        result = kzcTextureDeployToGPU_internal(texture, textureData, KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_2D, KZ_FALSE, 0);
                        kzsErrorForward(result);
                        texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
                    }
                    else if(format == KZC_TEXTURE_FORMAT_RGBA)
                    {
                        for(j = 0; j < height; ++j)
                        {
                            kzInt targetY = ((kzInt)j + y);
                            kzUint i;
                            for(i = 0; i < width; ++i)
                            {
                                kzInt targetX = ((kzInt)i + x);
                                if(targetY >= 0 && targetY < (kzInt)texture->data->descriptor.height &&
                                    targetX >= 0 && targetX < (kzInt)texture->data->descriptor.width)
                                {
                                    kzUint alpha = (kzUint)sourceData[(j * width + i) * 4 + 3];
                                    kzUint newColorRed =
                                        ((kzUint)textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel] * (255 - alpha)
                                        + (kzUint)((kzFloat)alpha * colorMultiply->red * 255)) >> 8;
                                    kzUint newColorGreen =
                                        ((kzUint)textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 1] * (255 - alpha)
                                        + (kzUint)((kzFloat)alpha * colorMultiply->green * 255)) >> 8;
                                    kzUint newColorBlue =
                                        ((kzUint)textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 2] * (255 - alpha)
                                        + (kzUint)((kzFloat)alpha * colorMultiply->blue * 255)) >> 8;

                                    textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel] = (kzU8)newColorRed;
                                    textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 1] = (kzU8)newColorGreen;
                                    textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 2] = (kzU8)newColorBlue;
                                }

                            }
                        }

                        kzcTextureFreeGPUMemory(texture);
                        result = kzcTextureDeployToGPU_internal(texture, textureData, KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_2D, KZ_FALSE, 0);
                        kzsErrorForward(result);
                        texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
                    }
                }
                else if(textureFormat == KZS_GL_RGBA)
                {
                    kzUint bitsPerPixel = 4;

                    if(format == KZC_TEXTURE_FORMAT_ALPHA)
                    {
                        for(j = 0; j < height; ++j)
                        {
                            kzInt targetY = ((kzInt)j + y);
                            kzUint i;
                            for(i = 0; i < width; ++i)
                            {
                                kzInt targetX = ((kzInt)i + x);
                                
                                if(targetY >= 0 && targetY < (kzInt)texture->data->descriptor.height &&
                                    targetX >= 0 && targetX < (kzInt)texture->data->descriptor.width)
                                {
                                    kzUint alpha = (kzUint)sourceData[(j * width + i)];
                                    kzUint newColorRed =
                                        ((kzUint)textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel] * (255 - alpha)
                                        + (kzUint)((kzFloat)alpha * colorMultiply->red * 255)) >> 8;
                                    kzUint newColorGreen =
                                        ((kzUint)textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 1] * (255 - alpha)
                                        + (kzUint)((kzFloat)alpha * colorMultiply->green * 255)) >> 8;
                                    kzUint newColorBlue =
                                        ((kzUint)textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 2] * (255 - alpha)
                                        + (kzUint)((kzFloat)alpha * colorMultiply->blue * 255)) >> 8;
                                    kzUint newColorAlpha =
                                        ((kzUint)textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 3] * (255 - alpha)
                                        + (kzUint)((kzFloat)alpha * 255)) >> 8;

                                    textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel] = (kzU8)newColorRed;
                                    textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 1] = (kzU8)newColorGreen;
                                    textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 2] = (kzU8)newColorBlue;
                                    textureData[((kzUint)targetY * texture->data->descriptor.width + (kzUint)targetX) * bitsPerPixel + 3] = (kzU8)newColorAlpha;
                                }
                            }
                        }

                        kzcTextureFreeGPUMemory(texture);
                        result = kzcTextureDeployToGPU_internal(texture, textureData, KZS_GL_TEXTURE_2D, KZS_GL_TEXTURE_2D, KZ_FALSE, 0);
                        kzsErrorForward(result);
                        texture->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
                    }
                }
            }
            else
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Sub data RAM copy only supported for byte format");
            }
        }
    }
    kzsSuccess();
}

kzsError kzcTextureUpdateSubData(const struct KzcTexture* texture, const void* data, kzUint x, kzUint y, kzUint width, kzUint height)
{
    if(texture->data->textureType == KZC_TEXTURE_TYPE_2D)
    {
        kzUint textureFormat;
        kzUint dataFormat;

        kzcTextureGetFormat_internal(texture->data->descriptor.format, &textureFormat, KZ_NULL, &dataFormat);
        kzsGlBindTexture(KZS_GL_TEXTURE_2D, texture->data->textureHandle);
        kzsGlPixelStorei(KZS_GL_UNPACK_ALIGNMENT, 1);

        kzsGlTexSubImage2D(KZS_GL_TEXTURE_2D, 0, (kzInt)x, (kzInt)y, (kzInt)width, (kzInt)height, textureFormat, dataFormat, data);

        /* Upload subimage to RAM, too. */
        if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
        {
            kzUint j;
            if(dataFormat == KZS_GL_UNSIGNED_BYTE)
            {
                kzUint bitsPerPixel = 1;

                if(textureFormat == KZS_GL_RGB)
                {
                    bitsPerPixel = 3;
                }
                else if(textureFormat == KZS_GL_RGBA)
                {
                    bitsPerPixel = 4;
                }

                for(j = 0; j < height; ++j)
                {
                    kzU8* rowPointer = (kzU8*)texture->data->typeData.texture2D.data;
                    kzU8* sourceRowPointer = (kzU8*)data;

                    kzsMemcpy(&rowPointer[(j + y) * texture->data->descriptor.width * bitsPerPixel + x * bitsPerPixel], &sourceRowPointer[j * width * bitsPerPixel], width * bitsPerPixel);
                }
            }
            else
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Sub data RAM copy only supported for byte format");
            }
        }
    }

    kzsSuccess();
}

static kzBool kzcTextureIsCompressionFormatSupported_internal(kzInt glCompressionFormat)
{
    kzInt compressedFormatsCount = 0;
    kzInt compressedFormats[256];
    kzInt i;
    kzBool found = KZ_FALSE;
    kzsGlGetIntegerv(KZS_GL_NUM_COMPRESSED_TEXTURE_FORMATS, &compressedFormatsCount);

    if (compressedFormatsCount > 256)
    {
        /* Too many formats. */
        kzsLog(KZS_LOG_LEVEL_WARNING, "Unable to check for compressed texture format support.");
    }
    else
    {
        kzsGlGetIntegerv(KZS_GL_COMPRESSED_TEXTURE_FORMATS, compressedFormats);
        for(i = 0; i < compressedFormatsCount; ++i)
        {
            if(compressedFormats[i] == glCompressionFormat)
            {
                found = KZ_TRUE;
                break;
            }
        }
    }
    return found;
}

kzBool kzcTextureIsCompressionFormatSupported(enum KzcTextureCompression compression)
{
    kzBool supported = KZ_FALSE;

    switch(compression)
    {
        case KZC_TEXTURE_COMPRESSION_NONE:
        {
            break;
        }
        case KZC_TEXTURE_COMPRESSION_ETC:
        {
            if(g_etcCompressionSupported == -1)
            {
#ifdef KZS_GL_ETC1_RGB8_OES
                supported = kzcTextureIsCompressionFormatSupported_internal(KZS_GL_ETC1_RGB8_OES);
                if(supported)
                {
                    g_etcCompressionSupported = 1;
                }
                else
                {
                    g_etcCompressionSupported = 0;
                }
#else
                g_etcCompressionSupported = 0;
#endif
            }
            supported = g_etcCompressionSupported == 1 ? KZ_TRUE : KZ_FALSE;
            
            break;
        }
        case KZC_TEXTURE_COMPRESSION_DXT3_RGBA:
        {
            if(g_dxt3CompressionSupported == -1)
            {
#ifdef KZS_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
                supported = kzcTextureIsCompressionFormatSupported_internal(KZS_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
                if(supported)
                {
                    g_dxt3CompressionSupported = 1;
                }
                else
                {
                    g_dxt3CompressionSupported = 0;
                }
#else
                g_dxt3CompressionSupported = 0;
#endif
            }
            supported = g_dxt3CompressionSupported == 1 ? KZ_TRUE : KZ_FALSE;
            break;
        }
        case KZC_TEXTURE_COMPRESSION_DXT5_RGBA:
        {
            if(g_dxt3CompressionSupported == -1)
            {
#ifdef KZS_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
                supported = kzcTextureIsCompressionFormatSupported_internal(KZS_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
                if(supported)
                {
                    g_dxt5CompressionSupported = 1;
                }
                else
                {
                    g_dxt5CompressionSupported = 0;
                }
#else
                g_dxt5CompressionSupported = 0;
#endif
            }
            supported = g_dxt5CompressionSupported == 1 ? KZ_TRUE : KZ_FALSE;
            break;
        }
    }
    return supported;
}


static kzsError kzcTextureDeployToGPUCompressed_internal(const struct KzcTexture* texture, const void *data, kzUint targetTypeGPU,
                                                         kzUint targetBindTypeGPU, kzBool cubeMap, kzUint cubeMapSide)
{
    kzInt mipmapLevel = 0;
    kzUint internalFormat = 0;

    kzBool formatSupported = KZ_TRUE;
    kzsAssert(kzcTextureIsValid(texture));
    kzsAssert(kzcIsValidPointer(data));

    kzsAssertText(texture->data->descriptor.compression != KZC_TEXTURE_COMPRESSION_NONE, "Uncompressed texture applied for compressed texture deploy function!");

    if(!cubeMap)
    {
        kzsGlGenTextures(1, &texture->data->textureHandle);
        texture->data->textureObjectType = targetBindTypeGPU;
        kzsGlBindTexture(targetBindTypeGPU, texture->data->textureHandle);
    }

    switch(texture->data->descriptor.compression)
    {
        case KZC_TEXTURE_COMPRESSION_ETC:
        case KZC_TEXTURE_COMPRESSION_DXT3_RGBA:
        case KZC_TEXTURE_COMPRESSION_DXT5_RGBA:
        {
            kzInt width;
            kzInt height;
            kzInt imageSize; /* Verify that compression format is supported */
            kzByte* gpuData = (kzByte*)data;

            if(texture->data->descriptor.compression == KZC_TEXTURE_COMPRESSION_ETC)
            {
#ifdef KZS_GL_ETC1_RGB8_OES
                internalFormat = KZS_GL_ETC1_RGB8_OES;
#else
                kzsAssertText(KZ_FALSE, "ETC GL constant not specified");
#endif
            }
            else if(texture->data->descriptor.compression == KZC_TEXTURE_COMPRESSION_DXT3_RGBA)
            {
#ifdef KZS_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
                internalFormat = KZS_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                gpuData += 32 * 4; /* 32 unsigned integers in header, skip to pixel data. */
#else
                kzsAssertText(KZ_FALSE, "DXT3 GL constant not specified");
#endif
            }
            else if(texture->data->descriptor.compression == KZC_TEXTURE_COMPRESSION_DXT5_RGBA)
            {
#ifdef KZS_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
                internalFormat = KZS_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                gpuData += 32 * 4; /* 32 unsigned integers in header, skip to pixel data. */
#else
                kzsAssertText(KZ_FALSE, "DXT5 GL constant not specified");
#endif
            }

            width = (kzInt)texture->data->descriptor.width;
            height = (kzInt)texture->data->descriptor.height;
            imageSize = (kzInt)kzcCompressedTextureDataSizeBytes_internal((kzUint)width, (kzUint)height, texture->data->descriptor.compression);

            formatSupported = kzcTextureIsCompressionFormatSupported(texture->data->descriptor.compression);
            kzsErrorTest(formatSupported, KZC_ERROR_TEXTURE_FORMAT_UNSUPPORTED, "Texture format not supported.");

            kzsGlCompressedTexImage2D(targetTypeGPU, mipmapLevel, internalFormat, width, height, 0, imageSize, gpuData);

            break;
        }
        case KZC_TEXTURE_COMPRESSION_NONE:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Not a known texture compression format!");
        }
    }

    if(texture->data->descriptor.filter == KZC_TEXTURE_FILTER_MIPMAP ||
       texture->data->descriptor.filter == KZC_TEXTURE_FILTER_TRILINEAR)
    {
        kzUint width = texture->data->descriptor.width / 2;
        kzUint height = texture->data->descriptor.height / 2;
        kzUint mipmapLevelCount = texture->data->textureType == KZC_TEXTURE_TYPE_2D ? texture->data->typeData.texture2D.mipmapLevelCount :
            texture->data->typeData.textureCubeMap.mipmapLevelCount;
        mipmapLevelCount = (kzUint)kzsMax(0, (kzInt)mipmapLevelCount - 1);

        while(width >= 1 || height >= 1)
        {
            kzUint level = kzsMinU((kzUint)mipmapLevel, mipmapLevelCount);
            kzUint textureWidth = kzsMaxU(1, width);
            kzUint textureHeight = kzsMaxU(1, height);
            kzUint imageSize = texture->data->typeData.texture2D.compressedMipmapDataSizes[level];

            if(texture->data->textureType == KZC_TEXTURE_TYPE_CUBEMAP)
            {
                kzsGlCompressedTexImage2D(targetTypeGPU, mipmapLevel + 1, internalFormat,
                    (kzInt)textureWidth, (kzInt)textureHeight, 
                    0, (kzInt)imageSize, texture->data->typeData.textureCubeMap.mipmapData[cubeMapSide][level]);
            }
            else
            {
                kzsGlCompressedTexImage2D(targetTypeGPU, mipmapLevel + 1, internalFormat,
                    (kzInt)textureWidth, (kzInt)textureHeight, 
                    0, (kzInt)imageSize, texture->data->typeData.texture2D.mipmapData[level]);
            }
            ++mipmapLevel;
            width /= 2;
            height /= 2;
        }
    }

    kzsSuccess();
}

static kzsError kzcTextureDeployToGPU_internal(const struct KzcTexture* texture, const void *data, kzUint targetTypeGPU,
                                               kzUint targetBindTypeGPU, kzBool cubeMap, kzUint cubemapSide)
{
    kzsError result;

    kzsAssert(kzcTextureIsValid(texture));
    kzsAssert(data == KZ_NULL || kzcIsValidPointer(data));

    switch(texture->data->descriptor.compression)
    {
        case KZC_TEXTURE_COMPRESSION_NONE:
        {
            result = kzcTextureDeployToGPUUncompressed_internal(texture, data, targetTypeGPU, targetBindTypeGPU, cubeMap, cubemapSide);
            kzsErrorForward(result);
            break;
        }
        case KZC_TEXTURE_COMPRESSION_DXT3_RGBA:
        case KZC_TEXTURE_COMPRESSION_DXT5_RGBA:
        case KZC_TEXTURE_COMPRESSION_ETC:
        {
            result = kzcTextureDeployToGPUCompressed_internal(texture, data, targetTypeGPU, targetBindTypeGPU, cubeMap, cubemapSide);
            kzsErrorForward(result);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "invalid texture compression applied for textureDeployToGPU");
        }
    }

    switch(texture->data->descriptor.filter)
    {
        case KZC_TEXTURE_FILTER_POINT_SAMPLE:
        {
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_MIN_FILTER, KZS_GL_NEAREST);
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_MAG_FILTER, KZS_GL_NEAREST);
            break;
        }
        case KZC_TEXTURE_FILTER_BILINEAR:
        {
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_MIN_FILTER, KZS_GL_LINEAR);
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_MAG_FILTER, KZS_GL_LINEAR);
            break;
        }
        case KZC_TEXTURE_FILTER_TRILINEAR:
        {
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_MIN_FILTER, KZS_GL_LINEAR_MIPMAP_LINEAR);
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_MAG_FILTER, KZS_GL_LINEAR);
            break;
        }
        case KZC_TEXTURE_FILTER_MIPMAP:
        {
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_MIN_FILTER, KZS_GL_NEAREST_MIPMAP_NEAREST);
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_MAG_FILTER, KZS_GL_NEAREST);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown filter mode!");
        }
    }

    switch(texture->data->descriptor.wrap)
    {
        case KZC_TEXTURE_WRAP_CLAMP:
        {
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_WRAP_S, KZS_GL_CLAMP_TO_EDGE);
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_WRAP_T, KZS_GL_CLAMP_TO_EDGE);
            break;
        }
        case KZC_TEXTURE_WRAP_REPEAT:
        {
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_WRAP_S, KZS_GL_REPEAT);
            kzsGlTexParameteri(targetBindTypeGPU, KZS_GL_TEXTURE_WRAP_T, KZS_GL_REPEAT);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown wrap mode!");
        }
    }

    if(!cubeMap)
    {
        kzsGlBindTexture(targetBindTypeGPU, 0);
    }

    kzsSuccess();
}

kzUint kzcTextureDescriptorGetMemorySize(const struct KzcTextureDescriptor* textureDescriptor)
{
    kzUint size = 0;
    switch(textureDescriptor->compression)
    {
        case KZC_TEXTURE_COMPRESSION_NONE:
        {
            kzUint bitsPerPixel = kzcTextureDescriptorGetBitsPerPixel(textureDescriptor);
            size = bitsPerPixel * textureDescriptor->width * textureDescriptor->height / 8;
            break;
        }
        case KZC_TEXTURE_COMPRESSION_DXT3_RGBA:
        case KZC_TEXTURE_COMPRESSION_DXT5_RGBA:
        case KZC_TEXTURE_COMPRESSION_ETC:
        {
            size =  kzcCompressedTextureDataSizeBytes_internal(textureDescriptor->width, textureDescriptor->height, textureDescriptor->compression);
            break;
        }
        default:
        {
            kzsErrorLog(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unsupported texture compression!");
            break;
        }
    }
    return size;
}

kzUint kzcTextureDescriptorGetBitsPerPixel(const struct KzcTextureDescriptor* textureDescriptor)
{
    kzUint bitsPerPixel = 0;
    switch(textureDescriptor->format)
    {
        case KZC_TEXTURE_FORMAT_RGB:
        {
            bitsPerPixel = 24;
            break;
        }
        case KZC_TEXTURE_FORMAT_RGBA:
        {
            bitsPerPixel = 32;
            break;
        }
        case KZC_TEXTURE_FORMAT_RGB565:
        {
            bitsPerPixel = 16;
            break;
        }
        case KZC_TEXTURE_FORMAT_DXT5_RGBA:
        case KZC_TEXTURE_FORMAT_DXT3_RGBA:
        {
            bitsPerPixel = 8;
            break;
        }
        case KZC_TEXTURE_FORMAT_ETC:
        {
            bitsPerPixel = 4;
            break;
        }
        case KZC_TEXTURE_FORMAT_LUMINANCE:
        case KZC_TEXTURE_FORMAT_ALPHA:
        {
            bitsPerPixel = 8;
            break;
        }
        default:
        {
            kzsErrorLog(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown texture descriptor format");
            kzsAssert(KZ_FALSE);
            break;
        }
    }
    return bitsPerPixel;
}

static kzUint kzcCompressedTextureDataSizeBytes_internal(kzUint width, kzUint height, enum KzcTextureCompression compressionFormat)
{
    kzUint size = 0;
    switch(compressionFormat)
    {
        case KZC_TEXTURE_COMPRESSION_DXT3_RGBA:
        {
            size = width * height;
            break;
        }
        case KZC_TEXTURE_COMPRESSION_DXT5_RGBA:
        {
            size = width * height;
            break;
        }
        case KZC_TEXTURE_COMPRESSION_ETC:
        {
            /* ETC stores 4x4 = 16 pixel blocks in 8 bytes */
            size = width * height / 2; 
            /* Minimum size for ETC texture is one 4x4 pixel block */
            size = kzsMaxU(size, 8);
            break;
        }
        case KZC_TEXTURE_COMPRESSION_NONE:
        {
            kzsAssert(KZ_FALSE);
            break;
        }
        default:
        {
            kzsErrorLog(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unsupported compression format!");
            break;
        }
    }
    return size;
}

kzUint kzcTextureGetTextureHandle(const struct KzcTexture* texture)
{
    kzsAssert(kzcTextureIsValid(texture));

    return texture->data->textureHandle;
}

void kzcTextureSetExternalTextureHandle(struct KzcTexture* texture, kzInt externalTextureHandle)
{
    kzsAssert(kzcTextureIsValid(texture));
    texture->externalTextureHandle = externalTextureHandle;
}

kzUint kzcTextureGetWidth(const struct KzcTexture* texture)
{
    kzsAssert(kzcTextureIsValid(texture));

    return texture->data->descriptor.width;
}

kzUint kzcTextureGetHeight(const struct KzcTexture* texture)
{
    kzsAssert(kzcTextureIsValid(texture));

    return texture->data->descriptor.height;
}

void* kzcTextureGetData(const struct KzcTexture* texture)
{
    void* data = KZ_NULL;
    kzsAssert(kzcTextureIsValid(texture));
    if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY ||
        texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        data = texture->data->typeData.texture2D.data;
    }
    return data;
}

enum KzcTextureFormat kzcTextureGetTextureFormat(const struct KzcTexture* texture)
{
    kzsAssert(kzcTextureIsValid(texture));

    return texture->data->descriptor.format;
}

enum KzcTextureFilter kzcTextureGetTextureFiltering(const struct KzcTexture* texture)
{
    kzsAssert(kzcTextureIsValid(texture));

    return texture->data->descriptor.filter;
}

void kzcTextureSetFrameBufferReference(const struct KzcTexture* texture, struct KzcFrameBuffer* frameBuffer)
{
    kzsAssert(kzcTextureIsValid(texture));
    kzsAssert(texture->data->textureType == KZC_TEXTURE_TYPE_RENDERTARGET);
    texture->data->typeData.textureRenderTarget.frameBuffer = frameBuffer;
}

struct KzcFrameBuffer* kzcTextureGetFrameBufferReference(const struct KzcTexture* texture)
{
    kzsAssert(kzcTextureIsValid(texture));
    kzsAssert(texture->data->textureType == KZC_TEXTURE_TYPE_RENDERTARGET);

    return texture->data->typeData.textureRenderTarget.frameBuffer;
}


kzsError kzcTextureAddFrameBufferDownscaleTarget(const struct KzcTexture* texture, kzUint index, struct KzcTexture* target)
{
    kzsError result;
    kzsAssert(kzcTextureIsValid(texture));
    kzsAssert(texture->data->textureType == KZC_TEXTURE_TYPE_RENDERTARGET);

    result = kzcDynamicArrayInsert(texture->data->typeData.textureRenderTarget.downscaleTargets, index, target);
    kzsErrorForward(result);
    kzsSuccess();
}

struct KzcTexture* kzcTextureGetFrameBufferDownscaleTarget(const struct KzcTexture* texture, kzUint index)
{
    struct KzcTexture* textureOut = KZ_NULL;

    kzsAssert(kzcTextureIsValid(texture));
    kzsAssert(texture->data->textureType == KZC_TEXTURE_TYPE_RENDERTARGET);

    if(index < kzcDynamicArrayGetSize(texture->data->typeData.textureRenderTarget.downscaleTargets))
    {
        textureOut = kzcDynamicArrayGet(texture->data->typeData.textureRenderTarget.downscaleTargets, index);
    }
    return textureOut;
}


kzUint kzcTextureGetBytesPerPixel_internal(enum KzcTextureFormat format)
{
    kzUint bytesPerPixel = 0;
    switch(format)
    {
        case KZC_TEXTURE_FORMAT_LUMINANCE:
        case KZC_TEXTURE_FORMAT_ALPHA:
        {
            bytesPerPixel = 1;
            break;
        }
        case KZC_TEXTURE_FORMAT_RGB565:
        {
            bytesPerPixel = 2;
            break;
        }
        case KZC_TEXTURE_FORMAT_RGB:
        {
            bytesPerPixel = 3;
            break;
        }
        case KZC_TEXTURE_FORMAT_RGBA:
        {
            bytesPerPixel = 4;
            break;
        }
        case KZC_TEXTURE_FORMAT_ETC:
        case KZC_TEXTURE_FORMAT_DXT3_RGBA:
        case KZC_TEXTURE_FORMAT_DXT5_RGBA:
        default:
        {
            kzsAssert(KZ_FALSE);
        }
    }
    return bytesPerPixel;
}

kzBool kzcTextureIsCompressed(const struct KzcTexture* texture)
{
    kzBool isCompressed = KZ_FALSE;
    kzsAssert(kzcTextureIsValid(texture));

    if(texture->data->descriptor.compression == KZC_TEXTURE_COMPRESSION_ETC)
    {
        isCompressed = KZ_TRUE;
    }

    return isCompressed;
}

void kzcTextureGetMemorySize(const struct KzcTexture* texture, kzUint* out_GPUMemory, kzUint* out_RAMMemory)
{
    kzUint memorySizeGPU = 0;
    kzUint memorySizeRAM = 0;

    kzsAssert(texture != KZ_NULL);

    if(texture->data->descriptor.compression == KZC_TEXTURE_COMPRESSION_NONE)
    {
        memorySizeGPU = kzcTextureGetBytesPerPixel_internal(texture->data->descriptor.format) *
            texture->data->descriptor.width * texture->data->descriptor.height;

        if(texture->data->textureType == KZC_TEXTURE_TYPE_2D)
        {
            if(texture->data->typeData.texture2D.mipmapLevelCount > 0)
            {
                memorySizeGPU *= 2;
            }
        }
        else if(texture->data->textureType == KZC_TEXTURE_TYPE_CUBEMAP)
        {
            if(texture->data->typeData.textureCubeMap.mipmapLevelCount > 0)
            {
                memorySizeGPU *= 2;
            }
        }

        if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
        {
            memorySizeRAM = memorySizeGPU;
        }
        else if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            memorySizeRAM = memorySizeGPU;
            memorySizeGPU = 0;
        }
    }
    else
    {
        if(texture->data->textureType == KZC_TEXTURE_TYPE_2D)
        {
            memorySizeGPU = texture->data->typeData.texture2D.compressedDataSize;
            if(texture->data->typeData.texture2D.mipmapLevelCount > 0)
            {
                memorySizeGPU *= 2;
            }
        }
        else if(texture->data->textureType == KZC_TEXTURE_TYPE_CUBEMAP)
        {
            memorySizeGPU = texture->data->typeData.textureCubeMap.compressedDataSize;
            if(texture->data->typeData.textureCubeMap.mipmapLevelCount > 0)
            {
                memorySizeGPU *= 2;
            }
        }

        if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
        {
            memorySizeRAM = memorySizeGPU;
        }
        else if(texture->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            memorySizeRAM = memorySizeGPU;
            memorySizeGPU = 0;
        }
        
    }

    if(texture->data->textureType == KZC_TEXTURE_TYPE_CUBEMAP)
    {
        memorySizeGPU *= 6;
        memorySizeRAM *= 6;
    }

    *out_GPUMemory = memorySizeGPU;
    *out_RAMMemory = memorySizeRAM;
}

enum KzcResourceMemoryType kzcTextureGetMemoryType(const struct KzcTexture* texture)
{
    kzsAssert(kzcIsValidPointer(texture));
    return texture->data->memoryType;
}

enum KzcTextureType kzcTextureGetTextureType(const struct KzcTexture* texture)
{
    kzsAssert(kzcIsValidPointer(texture));
    return texture->data->textureType;
}

kzUint kzcTextureGetMaximumSize()
{
    kzInt maxTextureSize = 0;
    kzsGlGetIntegerv(KZS_GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    return (kzUint)maxTextureSize;
#else
    return KZ_UINT_MAXIMUM;
#endif
}

kzUint kzcTextureGetMaximumCubemapSize()
{
    kzInt maxCubemapTextureSize = 0;
    kzsGlGetIntegerv(KZS_GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubemapTextureSize);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    return (kzUint)maxCubemapTextureSize;
#else
    return KZ_UINT_MAXIMUM;
#endif
}

void kzcTextureSetLoadOnDemandData(const struct KzcTexture* texture, void* loadData, KzcTextureLoadOnDemandData loadDataFunction)
{
    kzsAssert(kzcIsValidPointer(texture));

    texture->data->loadOnDemand = KZ_TRUE;
    texture->data->loadData = loadData;
    texture->data->loadDataFunction = loadDataFunction;
}
