/**
 * \file
 * Framebuffer object resource.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_RESOURCE_FRAME_BUFFER_H
#define KZC_RESOURCE_FRAME_BUFFER_H


#include "kzc_frame_buffer_type.h"

#include <core/resource_manager/texture/kzc_texture_descriptor.h>
#include <core/resource_manager/kzc_resource_memory_type.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcResourceManager;
struct KzcTexture;
struct KzcRenderer;


/** 
 * \struct KzcFrameBuffer
 * Structure of a framebuffer.
 * FrameBuffer is used to set rendering pipeline to render directly to textures instead of screen.
 */
struct KzcFrameBuffer;


/** Creates framebuffer object. */
kzsError kzcFrameBufferCreate(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType, 
                              kzUint width, kzUint height, struct KzcFrameBuffer** out_frameBuffer);

/** Attaches a target texture to a framebuffer object. */
void kzcFrameBufferBindTargetTexture(const struct KzcFrameBuffer* frameBuffer, const struct KzcTexture* texture);

/** Unbinds a target texture from framebuffer object. */
void kzcFrameBufferUnbindTargetTexture(const struct KzcFrameBuffer* frameBuffer);

/** Binds target texture to framebuffer with given depth format. */
void kzcFrameBufferAttachColorAndDepthToTexture(struct KzcFrameBuffer* frameBuffer,
                                                        enum KzcFrameBufferDepthFormat depthFormat, 
                                                        const struct KzcTexture* texture);

/** Binds a framebuffer object on renderer. */
void kzcFrameBufferBind(const struct KzcFrameBuffer* frameBuffer, struct KzcRenderer* renderer);

/** Unbinds a framebuffer object from renderer. */
void kzcFrameBufferUnbind(const struct KzcFrameBuffer* frameBuffer, struct KzcRenderer* renderer);

/** Free given framebuffer object from memory GPU and RAM. */
kzsError kzcFrameBufferFree(struct KzcFrameBuffer* frameBuffer);

/** Free given framebuffer object from GPU memory. */
void kzcFrameBufferFreeGPUMemory(struct KzcFrameBuffer* frameBuffer);

/** Restores given framebuffer object to GPU memory. */
kzsError kzcFrameBufferRestoreGPU(struct KzcFrameBuffer* frameBuffer);


/** Returns width of a framebuffer object. */
kzUint kzcFrameBufferGetWidth(const struct KzcFrameBuffer* frameBuffer);
/** Returns height of a framebuffer object. */
kzUint kzcFrameBufferGetHeight(const struct KzcFrameBuffer* frameBuffer);
/** Returns memory type of a framebuffer object. */
enum KzcResourceMemoryType kzcFrameBufferGetMemoryType(const struct KzcFrameBuffer* frameBuffer);


#endif
