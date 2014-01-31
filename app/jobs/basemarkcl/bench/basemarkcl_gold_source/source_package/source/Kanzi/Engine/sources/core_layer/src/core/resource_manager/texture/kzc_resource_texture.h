/**
 * \file
 * Texture resource. Texture includes all types of GPU textures,
 * that is: 2D, 3D, cube map and render target (framebuffer object) textures. Also information about compression.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_RESOURCE_TEXTURE_H
#define KZC_RESOURCE_TEXTURE_H


#include "kzc_texture_descriptor.h"

#include <core/resource_manager/kzc_resource_memory_type.h>
#include <core/resource_manager/frame_buffer/kzc_frame_buffer_type.h>
#include <core/util/color/kzc_color.h>
#include <core/util/image/kzc_image.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


#define KZC_TEXTURE_CUBE_MAP_POSITIVE_X 0 /**< Cube map face index for positive x direction. */
#define KZC_TEXTURE_CUBE_MAP_NEGATIVE_X 1 /**< Cube map face index for negative x direction. */
#define KZC_TEXTURE_CUBE_MAP_POSITIVE_Y 2 /**< Cube map face index for positive y direction. */
#define KZC_TEXTURE_CUBE_MAP_NEGATIVE_Y 3 /**< Cube map face index for negative y direction. */
#define KZC_TEXTURE_CUBE_MAP_POSITIVE_Z 4 /**< Cube map face index for positive z direction. */
#define KZC_TEXTURE_CUBE_MAP_NEGATIVE_Z 5 /**< Cube map face index for negative z direction. */


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcFrameBuffer;
struct KzcImage;
struct KzcRenderer;


/**
 * \struct KzcTexture
 * Structure of a texture.
 */
struct KzcTexture;


/** Function pointer prototype for loading texture data on demand. */
typedef kzsError (*KzcTextureLoadOnDemandData)(struct KzcTexture* texture, void* userData);


/** Creates a 2D texture resource. */
kzsError kzcTextureCreate(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType, 
                          struct KzcTextureDescriptor* descriptor, const void* data, struct KzcTexture** out_texture);

/** Creates a cube map Texture resource. */
kzsError kzcTextureCubemapCreate(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType, 
                                 const struct KzcTextureDescriptor* descriptor,
                                 void* const* cubemapData, struct KzcTexture** out_texture);

/** Creates a new texture resource from image. */
kzsError kzcTextureCreateFromImage(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                                   enum KzcTextureFilter filter, enum KzcTextureWrap wrapMode, 
                                   enum KzcTextureCompression compression, const struct KzcImage* image, struct KzcTexture** out_texture);
/**
 * Creates a cube map from images. Images array indices should be as defined with the constants KZC_TEXTURE_CUBE_MAP_*.
 */
kzsError kzcTextureCubeMapCreateFromImages(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                                           enum KzcTextureFilter filter, enum KzcTextureWrap wrapMode, 
                                           enum KzcTextureCompression compression, struct KzcImage* image[6], struct KzcTexture** out_textureCube);

/**
 * Creates new texture based on frame buffer.
 * TextureFilter can not be KZC_TEXTURE_FILTER_MIPMAP 
 * TextureFormat can be KZC_TEXTURE, KZC_TEXTURE_FORMAT_RGBA, KZC_TEXTURE_FORMAT_RGB or KZC_TEXTURE_FORMAT_RGB565
 */
kzsError kzcTextureCreateTextureFromFrameBuffer(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType,
                                                struct KzcFrameBuffer* frameBuffer, enum KzcFrameBufferTargetType targetType, 
                                                enum KzcTextureFormat textureFormat, 
                                                enum KzcTextureFilter textureFilter, enum KzcFrameBufferDepthFormat depthFormat, 
                                                struct KzcTexture** out_textureTarget);

/** Creates texture from frame buffer. */
kzsError kzcTextureCreateFrameBufferTexture(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, enum KzcTextureFormat format,
                                            kzUint width, kzUint height, enum KzcTextureFilter textureFilter, struct KzcTexture** out_textureTarget);

/**
 * Takes ownership of the texture container deletion. After this, when a texture is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzcTextureTakeOwnership(const struct KzcTexture* texture);
/** Checks if the texture is valid (pointer and the data are non-null). */
kzBool kzcTextureIsValid(const struct KzcTexture* texture);
/** Moves the data from source texture to the target texture. */
kzsError kzcTextureTransferData(struct KzcTexture* targetTexture, struct KzcTexture* sourceTexture);


/** Applies a texture for rendering. */
kzsError kzcTextureApply(const struct KzcTexture* texture, struct KzcRenderer* renderer);
/** Detaches texture from rendering. */
kzsError kzcTextureDetach(struct KzcRenderer* renderer);
/** Unbind texture from renderer. Used to disable shader uniform unbind calls. */
void kzcTextureUnbindRenderer(const struct KzcTexture* texture);


/** Updates texture data, supported for 2D textures. Data can be either in compressed / uncompressed format, depending on the
    original data. */
kzsError kzcTextureUpdateData(const struct KzcTexture* texture, const void* data);
/** Updates texture sub data, support for 2D textures. */
kzsError kzcTextureUpdateSubData(const struct KzcTexture* texture, const void* data, kzUint x, kzUint y, kzUint width, kzUint height);

/** Blends data to texture. Texture has to be stored in RAM in order this function to work. */
kzsError kzcTextureBlendData(const struct KzcTexture* texture, const void* data, kzInt x, kzInt y, kzUint width, kzUint height,
                             const struct KzcColorRGBA* colorMultiply, enum KzcTextureFormat format);

/** Free given texture from both RAM and GPU memory */
kzsError kzcTextureDelete(struct KzcTexture* texture);

/** Free given texture from GPU memory */
void kzcTextureFreeGPUMemory(const struct KzcTexture* texture);

/** Free given texture from RAM memory */
kzsError kzcTextureFreeRAMMemory(const struct KzcTexture* texture);

/** Restores given texture to GPU memory. */
kzsError kzcTextureRestoreGPU(const struct KzcTexture* texture);

/** Gets the texture handle */
kzUint kzcTextureGetTextureHandle(const struct KzcTexture* texture);

/** Sets external texture handle for texture. */
void kzcTextureSetExternalTextureHandle(struct KzcTexture* texture, kzInt externalTextureHandle);

/** Sets name for the texture. */
kzsError kzcTextureSetName(const struct KzcTexture* texture, kzString name);
/** Gets name of texture. KZ_NULL if not assigned. */
kzString kzcTextureGetName(const struct KzcTexture* texture);

/** Gets the texture width */
kzUint kzcTextureGetWidth(const struct KzcTexture* texture);
/** Gets the texture height */
kzUint kzcTextureGetHeight(const struct KzcTexture* texture);
/** Gets data of texture. KZ_NULL if only on GPU. */
void* kzcTextureGetData(const struct KzcTexture* texture);
/** Gets the texture format */
enum KzcTextureFormat kzcTextureGetTextureFormat(const struct KzcTexture* texture);
/** Gets the texture filtering. */
enum KzcTextureFilter kzcTextureGetTextureFiltering(const struct KzcTexture* texture);
/** Sets frame buffer reference to texture. */
void kzcTextureSetFrameBufferReference(const struct KzcTexture* texture, struct KzcFrameBuffer* frameBuffer);
/** Gets frame buffer reference from texture. */
struct KzcFrameBuffer* kzcTextureGetFrameBufferReference(const struct KzcTexture* texture);
/** Gets downscale target from frame buffer target texture from given index. Returns KZ_NULL if none is bound to this index. */
struct KzcTexture* kzcTextureGetFrameBufferDownscaleTarget(const struct KzcTexture* texture, kzUint index);
/** Sets downscale target to frame buffer target texture. */
kzsError kzcTextureAddFrameBufferDownscaleTarget(const struct KzcTexture* texture, kzUint index, struct KzcTexture* target);

/** Returns true if texture is in compressed format. */
kzBool kzcTextureIsCompressed(const struct KzcTexture* texture);
/** Gets memory size for texture. */
void kzcTextureGetMemorySize(const struct KzcTexture* texture, kzUint* out_GPUMemory, kzUint* out_RAMMemory);

/** Check if compression format supported */
kzBool kzcTextureIsCompressionFormatSupported(enum KzcTextureCompression compression);

/** Gets memory type from texture. */
enum KzcResourceMemoryType kzcTextureGetMemoryType(const struct KzcTexture* texture);

/** Gets compression from image format. Returns KZ_TRUE if format supported. */
kzBool kzcTextureGetCompressionFromImageFormat(enum KzcImageDataFormat imageDataFormat, enum KzcTextureCompression* out_textureCompression);

/** Gets maximum size for texture. */
kzUint kzcTextureGetMaximumSize(void);
/** Gets maximum size for cubemap texture. */
kzUint kzcTextureGetMaximumCubemapSize(void);

/** Gets texture type from texture. */
enum KzcTextureType kzcTextureGetTextureType(const struct KzcTexture* texture);


/** Creates a texture that is loaded on demand. */
kzsError kzcTextureCreateOnDemand(const struct KzcMemoryManager* memoryManager, struct KzcTexture** out_texture);
/** Prepares loading on demand. Must be called before load-on-demand function pointer. */
kzsError kzcTexturePrepareLoadOnDemand(struct KzcTexture* texture);
/** Ends loading on demand. Moves data from source texture to target texture. */
kzsError kzcTextureEndLoadOnDemand(struct KzcTexture* targetTexture, struct KzcTexture* sourceTexture);
/** Sets load on demand data, which is function pointer and arbitrary data for filling texture data. */
void kzcTextureSetLoadOnDemandData(const struct KzcTexture* texture, void* loadData, KzcTextureLoadOnDemandData loadDataFunction);


#endif
