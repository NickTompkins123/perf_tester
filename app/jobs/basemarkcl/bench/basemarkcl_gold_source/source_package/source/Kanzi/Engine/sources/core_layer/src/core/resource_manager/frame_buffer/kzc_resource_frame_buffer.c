/**
 * \file
 * FrameBuffer resource.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_resource_frame_buffer.h"

#include <core/debug/kzc_log.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/texture/kzc_texture_descriptor.h>
#include <core/util/string/kzc_string.h>

#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_opengl.h>
#include <system/time/kzs_tick.h>


/**< Global variable storing depth24 extension. */ /*lint -e{956} Ignored warning for not multi thread safeness. */
static kzInt g_frameBufferDepth24ExtensionSupported = -1;

/** Structure describing FrameBufferObject target textures. */
struct KzcFrameBufferTargetFormat
{
    kzUint depthFormat;                         /**< Format of the depth attachment */
    
    enum KzcFrameBufferTargetType targetType;   /**< Type of the target, color, depth or invalid. */
};

struct KzcFrameBuffer
{
    struct KzcResourceManager* resourceManager; /**< Resource manager that contains this resource. */

    kzUint width;                                   /**< Width of the frame buffer. */
    kzUint height;                                  /**< Height of the frame buffer. */
    kzUint frameBufferGPUHandle;                    /**< Handle for the frame buffer. */

    enum KzcResourceMemoryType memoryType;          /**< Memory type of this resource, either GPU or GPU+RAM. */

    struct KzcFrameBufferTargetFormat targetFormat; /**< Target texture for this frame buffer object. */
};


/** Deploys FrameBuffer to GPU. */
static void kzcFrameBufferDeployToGPU_internal(struct KzcFrameBuffer* frameBuffer);
/** Checks the FrameBuffer status for errors and logs the errors. */
static void kzcFrameBufferLogErrors_internal(void);


static kzBool kzcFrameBufferIsDepth24Supported_internal(void)
{
    if(g_frameBufferDepth24ExtensionSupported == -1)
    {
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
        kzString extensionString;
        const kzString DEPTH_24_EXTENSION = "GL_OES_depth24";
        kzUint index;

        /* Get list of the extensions */
        extensionString = kzsGlGetString(KZS_GL_EXTENSIONS);

        g_frameBufferDepth24ExtensionSupported = kzcStringFindSubstring(extensionString, DEPTH_24_EXTENSION, &index) ? 1 : 0;
#else
        g_frameBufferDepth24ExtensionSupported = 0;
#endif
    }

    return g_frameBufferDepth24ExtensionSupported == 1 ? KZ_TRUE : KZ_FALSE;
}

kzsError kzcFrameBufferCreate(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType, 
                                      kzUint width, kzUint height, struct KzcFrameBuffer** out_frameBuffer)
{    
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzcFrameBuffer* frameBuffer;
    kzUint measurementStartTime = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(resourceManager));

    memoryManager = kzcMemoryGetManager(resourceManager);

    /* Allocate memory for frame buffer. */
    result = kzcMemoryAllocVariable(memoryManager, frameBuffer, "Frame buffer internal");
    kzsErrorForward(result);

    /* Store the initialization data. */
    frameBuffer->memoryType = memoryType;
    frameBuffer->width = width;
    frameBuffer->height = height;
    frameBuffer->targetFormat.targetType = KZC_FRAME_BUFFER_TARGET_INVALID;
    frameBuffer->targetFormat.depthFormat = 0;
    frameBuffer->resourceManager = resourceManager;

#ifdef KZC_RESOURCE_FORCE_TO_RAM
    if(memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        frameBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
#endif

    /* Generate handle. */
    if(frameBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
       frameBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        kzcFrameBufferDeployToGPU_internal(frameBuffer);
    }

    result = kzcResourceManagerAddFrameBuffer(resourceManager, frameBuffer);
    kzsErrorForward(result);

    kzcResourceManagerAddResourceCumulativeTime_private(resourceManager, measurementStartTime, KZC_RESOURCE_MEASUREMENT_FRAME_BUFFER);

    *out_frameBuffer = frameBuffer;
    kzsSuccess();
}

void kzcFrameBufferBindTargetTexture(const struct KzcFrameBuffer* frameBuffer, const struct KzcTexture* texture)
{
    kzsAssert(kzcIsValidPointer(frameBuffer));
    kzsAssert(kzcIsValidPointer(texture));

    kzsGlBindFramebuffer(KZS_GL_FRAMEBUFFER, frameBuffer->frameBufferGPUHandle);
    kzsGlFramebufferTexture2D(KZS_GL_FRAMEBUFFER, KZS_GL_COLOR_ATTACHMENT0, KZS_GL_TEXTURE_2D, 
                              kzcTextureGetTextureHandle(texture), 0);
    kzsGlBindFramebuffer(KZS_GL_FRAMEBUFFER, 0);

    kzcFrameBufferLogErrors_internal();
}

void kzcFrameBufferUnbindTargetTexture(const struct KzcFrameBuffer* frameBuffer)
{
    kzsAssert(kzcIsValidPointer(frameBuffer));

    kzsGlBindFramebuffer(KZS_GL_FRAMEBUFFER, frameBuffer->frameBufferGPUHandle);
    kzsGlFramebufferTexture2D(KZS_GL_FRAMEBUFFER, KZS_GL_COLOR_ATTACHMENT0, KZS_GL_TEXTURE_2D, 0, 0);
    kzsGlBindFramebuffer(KZS_GL_FRAMEBUFFER, 0);
}

void kzcFrameBufferBind(const struct KzcFrameBuffer* frameBuffer, struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(frameBuffer));

     /* Binds the FrameBuffer buffer. */    
    kzcRendererSetActiveFrameBuffer(renderer, frameBuffer->frameBufferGPUHandle);
}

void kzcFrameBufferUnbind(const struct KzcFrameBuffer* frameBuffer, struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(frameBuffer));

    /* Give warning if user is unbinding when there is no need to unbind */
    if(kzcRendererGetActiveFrameBuffer(renderer) == 0)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "kzcFrameBufferUnBind called with no framebuffer attached.");
        kzsAssert(KZ_FALSE);
    }

    /* Unbind current framebuffer. */
    kzcRendererSetActiveFrameBuffer(renderer, 0);
}

void kzcFrameBufferAttachColorAndDepthToTexture(struct KzcFrameBuffer* frameBuffer,
                                                enum KzcFrameBufferDepthFormat depthFormat, 
                                                const struct KzcTexture* texture)
{
    kzUint temporaryHandle;

    /* Generate depth render buffer handle. */
    kzsGlGenRenderbuffers(1, &temporaryHandle);

    /* Bind render buffer. */
    kzsGlBindRenderbuffer(KZS_GL_RENDERBUFFER, temporaryHandle);

    /* Find the correct depth format */
    switch (depthFormat)
    {
        case KZC_FRAME_BUFFER_DEPTH_16:
        { 
            frameBuffer->targetFormat.depthFormat = KZS_GL_DEPTH_COMPONENT16;
            break;
        }
        case KZC_FRAME_BUFFER_DEPTH_24:
        {
            if(kzcFrameBufferIsDepth24Supported_internal())
            {
#ifdef KZS_GL_DEPTH_COMPONENT24
                frameBuffer->targetFormat.depthFormat = KZS_GL_DEPTH_COMPONENT24;
#else
                frameBuffer->targetFormat.depthFormat = KZS_GL_DEPTH_COMPONENT16;
#endif
            }
            /* TODO: Fall-back to 16 should be warned. */
            else
            {
                frameBuffer->targetFormat.depthFormat = KZS_GL_DEPTH_COMPONENT16;
            }
            break;
        }
        case KZC_FRAME_BUFFER_DEPTH_32:
        { 
            /* Can be useful on desktop - ES only supports 16 bits depth */
            KZS_NOT_IMPLEMENTED_YET;
            break;
        }
        default:
        {
            kzsAssert(KZ_FALSE);
        }
    }

    /* Generate depth buffer. */
    kzsGlRenderbufferStorage(KZS_GL_RENDERBUFFER, frameBuffer->targetFormat.depthFormat, 
        (kzInt)frameBuffer->width, (kzInt)frameBuffer->height);

    /* Specify color and depth attachments. */
    kzsGlBindFramebuffer(KZS_GL_FRAMEBUFFER, frameBuffer->frameBufferGPUHandle);
    kzsGlFramebufferTexture2D(KZS_GL_FRAMEBUFFER, KZS_GL_COLOR_ATTACHMENT0, KZS_GL_TEXTURE_2D, 
                              kzcTextureGetTextureHandle(texture), 0);
    kzsGlFramebufferRenderbuffer(KZS_GL_FRAMEBUFFER, KZS_GL_DEPTH_ATTACHMENT, KZS_GL_RENDERBUFFER, temporaryHandle);

    kzcFrameBufferLogErrors_internal();

    /* Unbind frame buffer */
    kzsGlBindFramebuffer(KZS_GL_FRAMEBUFFER, 0);

    /* Free temporary depth render buffer */
    kzsGlDeleteRenderbuffers(1, &temporaryHandle);
}

kzsError kzcFrameBufferFree(struct KzcFrameBuffer* frameBuffer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(frameBuffer));

    kzcFrameBufferFreeGPUMemory(frameBuffer);

    result = kzcResourceManagerRemoveFrameBuffer(frameBuffer->resourceManager, frameBuffer);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(frameBuffer);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcFrameBufferFreeGPUMemory(struct KzcFrameBuffer* frameBuffer)
{
    kzsAssert(kzcIsValidPointer(frameBuffer));

    if(frameBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY || frameBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        /* Free from GPU */
        kzsGlDeleteFramebuffers(1, &frameBuffer->frameBufferGPUHandle);

        if(frameBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            frameBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_NONE;
        }
        else
        {
            frameBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY;
        }
    }
}

kzUint kzcFrameBufferGetWidth(const struct KzcFrameBuffer* frameBuffer)
{
    kzsAssert(kzcIsValidPointer(frameBuffer));
    return frameBuffer->width;
}

kzUint kzcFrameBufferGetHeight(const struct KzcFrameBuffer* frameBuffer)
{
    kzsAssert(kzcIsValidPointer(frameBuffer));
    return frameBuffer->height;
}

enum KzcResourceMemoryType kzcFrameBufferGetMemoryType(const struct KzcFrameBuffer* frameBuffer)
{
    kzsAssert(kzcIsValidPointer(frameBuffer));
    return frameBuffer->memoryType;
}

/** Restores given FrameBuffer to GPU memory. */
kzsError kzcFrameBufferRestoreGPU(struct KzcFrameBuffer* frameBuffer)
{
    kzsAssert(kzcIsValidPointer(frameBuffer));

    if(frameBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
    {
        /* Deploy to GPU */
        kzcFrameBufferDeployToGPU_internal(frameBuffer);
        frameBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
    else if(frameBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "This shader is already contained in GPU and RAM!");
    }
    else if(frameBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "This shader is already contained in GPU!");
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Cannot restore shader that isn't contained in RAM memory!");
    }
    kzsSuccess();
}

static void kzcFrameBufferLogErrors_internal()
{
    kzUint status = kzsGlCheckFramebufferStatus(KZS_GL_FRAMEBUFFER);

    switch(status)
    {
        case KZS_GL_FRAMEBUFFER_COMPLETE:
        {
            /* Complete - nothing to log */
            break;
        }
        case KZS_GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "kzcFrameBufferCheckErrors_internal(): KZS_GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
            break;
        }
        case KZS_GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "kzcFrameBufferCheckErrors_internal(): KZS_GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
            break;
        }
        default:          
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "kzcFrameBufferCheckErrors_internal(): FrameBuffer status unknown.");
            break;
        }
    }
}

static void kzcFrameBufferDeployToGPU_internal(struct KzcFrameBuffer* frameBuffer)
{
    kzsAssert(kzcIsValidPointer(frameBuffer));

    /* Generate handle */
    kzsGlGenFramebuffers(1, &frameBuffer->frameBufferGPUHandle);
}
