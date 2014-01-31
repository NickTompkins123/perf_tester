/**
* \file
* Texture descriptor, properties that texture has.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_RESOURCE_TEXTURE_DESCRIPTOR_H
#define KZC_RESOURCE_TEXTURE_DESCRIPTOR_H


#include <system/kzs_types.h>


/** List of available texture operations. */
enum KzcTextureOperation
{
    KZC_TEXTURE_OPERATION_MODULATE, /**< Modulate (multiply). */
    KZC_TEXTURE_OPERATION_REPLACE,  /**< Replace. */
    KZC_TEXTURE_OPERATION_DECAL,    /**< Decal. */
    KZC_TEXTURE_OPERATION_ADD       /**< Additive. */
};

/** Specifies list of available texture formats. */
enum KzcTextureFormat
{
    KZC_TEXTURE_FORMAT_RGB,         /**< RGB texture format. */
    KZC_TEXTURE_FORMAT_RGBA,        /**< RGBA texture format. */
    KZC_TEXTURE_FORMAT_ALPHA,       /**< Alpha texture format. */
    KZC_TEXTURE_FORMAT_RGB565,      /**< RGB565 texture format. */
    KZC_TEXTURE_FORMAT_ETC,         /**< ETC compressed texture format. */
    KZC_TEXTURE_FORMAT_LUMINANCE,    /**< Luminance texture format. */
    KZC_TEXTURE_FORMAT_DXT3_RGBA,         /**< DXT3 RGBA compressed texture format. */
    KZC_TEXTURE_FORMAT_DXT5_RGBA          /**< DXT5 RGBA compressed texture format. */
};

/** Specifies texture filter. */
enum KzcTextureFilter
{
    KZC_TEXTURE_FILTER_POINT_SAMPLE,        /**< Point sample filter. */
    KZC_TEXTURE_FILTER_BILINEAR,            /**< Bilinear filter. */
    KZC_TEXTURE_FILTER_TRILINEAR,           /**< Trilinear filter. */
    KZC_TEXTURE_FILTER_MIPMAP               /**< Point sample filter. */
};

/** Specifies texture wrapping mode. */
enum KzcTextureWrap
{
    KZC_TEXTURE_WRAP_REPEAT,    /**< Repeating texture, maps points outside [0, 1] inside the ares by repeating texture. */
    KZC_TEXTURE_WRAP_CLAMP      /**< Clamp texture, points outside [0, 1] are sampled from texture edges. */
};

/** Specifies list of available texture compressions. */
enum KzcTextureCompression
{
    KZC_TEXTURE_COMPRESSION_NONE,       /**< No texture compression. */
    KZC_TEXTURE_COMPRESSION_ETC,        /**< Ericsson texture compression (ETC1_RGB8_OES)*/
    KZC_TEXTURE_COMPRESSION_DXT3_RGBA,        /**< DXT3 texture compression (COMPRESSED_RGBA_S3TC_DXT3_EXT)*/
    KZC_TEXTURE_COMPRESSION_DXT5_RGBA        /**< DXT5 texture compression (COMPRESSED_RGBA_S3TC_DXT5_EXT)*/
};

/** Texture types. */
enum KzcTextureType
{
    KZC_TEXTURE_TYPE_2D,            /**< Normal 2 dimensional texture. */
    KZC_TEXTURE_TYPE_CUBEMAP,       /**< Cubemap texture. */
    KZC_TEXTURE_TYPE_RENDERTARGET   /**< Render target texture. */
};

/** Structure for texture descriptor. */
struct KzcTextureDescriptor
{
    kzUint width;   /**< Texture width. */
    kzUint height;  /**< Texture height. */

    enum KzcTextureFormat format;   /**< Texture format. */
    enum KzcTextureFilter filter;   /**< Texture filter, e.g. linear / trilinear. */
    enum KzcTextureWrap wrap;       /**< Texture wrap mode, e.g. clamp / repeat. */
    enum KzcTextureCompression compression; /**< Texture compression, e.g. none / ETC */
};


/** Returns memory data size that the texture takes. */
kzUint kzcTextureDescriptorGetMemorySize(const struct KzcTextureDescriptor* textureDescriptor);
/** Returns bits per pixel of the texture descriptor. */
kzUint kzcTextureDescriptorGetBitsPerPixel(const struct KzcTextureDescriptor* textureDescriptor);

/** Initializes a texture descriptor. */
void kzcTextureDescriptorSet(kzUint width, kzUint height, 
                             enum KzcTextureFormat format, enum KzcTextureFilter filter,
                             enum KzcTextureWrap textureWrap, enum KzcTextureCompression compression,
                             struct KzcTextureDescriptor* textureDescriptor);


#endif
