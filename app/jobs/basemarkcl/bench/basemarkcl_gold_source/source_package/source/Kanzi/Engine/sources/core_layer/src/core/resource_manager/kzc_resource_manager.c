/**
 * \file
 * Resource manager functionality.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_resource_manager.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/image/kzc_image.h>
#include <core/resource_manager/index_buffer/kzc_resource_index_buffer.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex_buffer.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/shader/kzc_resource_shader.h>
#include <core/resource_manager/frame_buffer/kzc_resource_frame_buffer.h>
#include <core/renderer/kzc_renderer.h>
#include <core/debug/kzc_log.h>
#include <core/kzc_error_codes.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_opengl.h>
#include <system/time/kzs_tick.h>
#include <system/wrappers/kzs_opengl_base.h>

#include <stdio.h>

#ifdef _DEBUG
#define KZC_RESOURCE_MANAGER_DEBUG
#endif
/* TODO: Plan for resouce storage handling, as we cannot really assume GPU+RAM or GPU by default in engine resources. */


/** Structure for resource manager measurement info. Used to detect time consumption of GPU resources. */
struct KzcResourceManagerMeasurementInfo
{
    kzUint vertexBufferCumulativeTime; /**< Cumulative vertex buffer time. */
    kzUint indexBufferCumulativeTime; /**< Cumulative index buffer time. */
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzUint shaderCumulativeTime; /**< Cumulative shader time. */
#endif
    kzUint textureCumulativeTime; /**< Cumulative texture time. */
    kzUint frameBufferCumulativeTime; /**< Cumulative frame buffer time. */
};

/** Structure of a Resource Manager. */
struct KzcResourceManager
{
    struct KzcDynamicArray*  indexBuffers;   /**< Hash table for index buffers. <KzcIndexBuffer>. */
    struct KzcDynamicArray*  vertexBuffers;   /**< Hash table for vertex buffers. <KzcVertexBuffer>. */
    struct KzcDynamicArray*  textures;   /**< Hash table for vertex buffers. <KzcTexture>. */
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    struct KzcDynamicArray*  shaders;   /**< Hash table for shaders. <KzcShader, KzcShaderInternal>. */
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    struct KzcDynamicArray* fixedShaderAttributes;  /**< Fixed shader attributes. */
#endif
    struct KzcDynamicArray*  frameBuffers;   /**< Hash table for frameBuffers. <KzcFrameBuffer, KzcFrameBufferInternal>. */

    kzBool measurementEnabled; /**< Resource measurement enabled. */
    struct KzcResourceManagerMeasurementInfo measurementInfo; /**< Resource measurement info. */ 

#ifdef KZC_RESOURCE_MANAGER_DEBUG
    struct KzcDynamicArray* resourceAllocations; /**< Resource allocations for resource manager. */
#endif
};


#ifdef KZC_RESOURCE_MANAGER_DEBUG
/** Structure for Resource manager allocations. */
struct KzcResourceManagerAllocation
{
    kzUint allocationId; /**< Allocation ID for this resource. */
    kzString allocationString;
    void* resourceData; /**< Resource data; Vertex buffer, Index buffer, FrameBuffer, Texture or Shader. */
};
/** This variable gives unique allocation id for each allocation. It can be tracked with KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION macro. */
/*lint -e{956} This is only for debugging purposes. */
static kzUint resourceAllocationId = 1;
/** Set the value of this macro to any positive number to break the debugger when the given allocation is allocated. Set to 0 to disable. */
#define KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION 0
#endif


#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
static const kzString KZC_FIXED_ATTRIBUTES[8] = { "kzPosition", "kzNormal", "kzTangent", "kzColor0",
    "kzTextureCoordinate0", "kzTextureCoordinate1", "kzTextureCoordinate2", "kzTextureCoordinate3" };
#endif


kzsError kzcResourceManagerCreate(const struct KzcMemoryManager* memoryManager, struct KzcResourceManager** out_resourceManager)
{
    kzsError result;
    struct KzcResourceManager* resourceManager;

    result = kzcMemoryAllocVariable(memoryManager, resourceManager, "Resource manager");
    kzsErrorForward(result);

    /* Create dynamic arrays for resources. */
    result = kzcDynamicArrayCreate(memoryManager, &resourceManager->indexBuffers);
    kzsErrorForward(result);
    result = kzcDynamicArrayCreate(memoryManager, &resourceManager->vertexBuffers);
    kzsErrorForward(result);
    result = kzcDynamicArrayCreate(memoryManager, &resourceManager->textures);
    kzsErrorForward(result);

    /* Create hash maps for resources. */
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    result = kzcDynamicArrayCreate(memoryManager, &resourceManager->shaders);
    kzsErrorForward(result);
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    result = kzcDynamicArrayCreate(memoryManager, &resourceManager->fixedShaderAttributes);
    kzsErrorForward(result);
    {
        kzUint i;

        for(i = 0; i < 8; ++i)
        {
            result = kzcDynamicArrayAdd(resourceManager->fixedShaderAttributes, (void*)KZC_FIXED_ATTRIBUTES[i]);
            kzsErrorForward(result);
        }
    }
#endif

#ifdef KZC_RESOURCE_MANAGER_DEBUG
    result = kzcDynamicArrayCreate(memoryManager, &resourceManager->resourceAllocations);
    kzsErrorForward(result);
#endif

    result = kzcDynamicArrayCreate(memoryManager, &resourceManager->frameBuffers);
    kzsErrorForward(result);

    resourceManager->measurementEnabled = KZ_FALSE;
    resourceManager->measurementInfo.indexBufferCumulativeTime = 0;
    resourceManager->measurementInfo.vertexBufferCumulativeTime = 0;
    resourceManager->measurementInfo.textureCumulativeTime = 0;
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    resourceManager->measurementInfo.shaderCumulativeTime = 0;
#endif
    resourceManager->measurementInfo.frameBufferCumulativeTime = 0;

    *out_resourceManager = resourceManager;
    kzsSuccess();
}

kzsError kzcResourceManagerDelete(struct KzcResourceManager* resourceManager)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        if (!kzcDynamicArrayIsEmpty(resourceManager->resourceAllocations))
        {
            struct KzcDynamicArrayMutableIterator it = kzcDynamicArrayGetMutableIterator(resourceManager->resourceAllocations);

            kzsLog(KZS_LOG_LEVEL_WARNING, "Some resources were not released properly:");

            while(kzcDynamicArrayMutableIterate(it))
            {
                kzMutableString formattedString;
                struct KzcResourceManagerAllocation* allocation = (struct KzcResourceManagerAllocation*)kzcDynamicArrayMutableIteratorGetValue(it);

                kzsAssert(allocation != KZ_NULL);

                /* TODO: Don't use printf */
                result = kzcDynamicArrayMutableIteratorRemove(it);
                kzsErrorForward(result);
                result = kzcStringFormat(kzcMemoryGetManager(resourceManager), "%i: %s", &formattedString, allocation->allocationId, allocation->allocationString);
                kzsErrorForward(result);
                printf("%s\n", formattedString);
                result = kzcStringDelete(formattedString);
                kzsErrorForward(result);
                result = kzcMemoryFreeVariable(allocation);
                kzsErrorForward(result);
            }
        }
        result = kzcDynamicArrayDelete(resourceManager->resourceAllocations);
        kzsErrorForward(result);
    }

#endif
    /* Delete vertex buffers. */
    /* TODO: Replace with a while loop that deletes the first item */
    it = kzcDynamicArrayGetIterator(resourceManager->vertexBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcVertexBuffer* vertexBuffer = (struct KzcVertexBuffer*)kzcDynamicArrayIteratorGetValue(it);
        result = kzcVertexBufferFree(vertexBuffer);
        kzsErrorForward(result);
        it = kzcDynamicArrayGetIterator(resourceManager->vertexBuffers);
    }
    result = kzcDynamicArrayDelete(resourceManager->vertexBuffers);
    kzsErrorForward(result);

    /* Delete index buffers. */
    it = kzcDynamicArrayGetIterator(resourceManager->indexBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcIndexBuffer* indexBuffer = (struct KzcIndexBuffer*)kzcDynamicArrayIteratorGetValue(it);
        result = kzcIndexBufferFree(indexBuffer);
        kzsErrorForward(result);
        it = kzcDynamicArrayGetIterator(resourceManager->indexBuffers);
    }
    result = kzcDynamicArrayDelete(resourceManager->indexBuffers);
    kzsErrorForward(result);

    /* Delete textures. */
    it = kzcDynamicArrayGetIterator(resourceManager->textures);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcTexture* texture = (struct KzcTexture*)kzcDynamicArrayIteratorGetValue(it);
        result = kzcTextureDelete(texture);
        kzsErrorForward(result);
        it = kzcDynamicArrayGetIterator(resourceManager->textures);
    }
    result = kzcDynamicArrayDelete(resourceManager->textures);
    kzsErrorForward(result);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    it = kzcDynamicArrayGetIterator(resourceManager->shaders);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcShader* shader = (struct KzcShader*)kzcDynamicArrayIteratorGetValue(it);
        result = kzcShaderDelete(shader);
        kzsErrorForward(result);
        it = kzcDynamicArrayGetIterator(resourceManager->shaders);
    }
    result = kzcDynamicArrayDelete(resourceManager->shaders);
    kzsErrorForward(result);
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    result = kzcDynamicArrayDelete(resourceManager->fixedShaderAttributes);
    kzsErrorForward(result);
#endif

    /* Free frame buffers */
    result = kzcResourceManagerFreeFramebuffers(resourceManager);
    kzsErrorForward(result);

    result = kzcDynamicArrayDelete(resourceManager->frameBuffers);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(resourceManager);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcResourceManagerFreeFramebuffers(const struct KzcResourceManager* resourceManager)
{
    kzsError result;

    struct KzcDynamicArrayIterator it;
    /* Free frame buffers */
    it = kzcDynamicArrayGetIterator(resourceManager->frameBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcFrameBuffer* frameBuffer = (struct KzcFrameBuffer*)kzcDynamicArrayIteratorGetValue(it);
        result = kzcFrameBufferFree(frameBuffer);
        kzsErrorForward(result);
        it = kzcDynamicArrayGetIterator(resourceManager->frameBuffers);
    }

    kzsSuccess();
}

kzsError kzcResourceManagerFreeResourcesFromGPU(const struct KzcResourceManager* resourceManager)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsLog(KZS_LOG_LEVEL_INFO, "Index buffers");
    /* Free index buffer GPU. */
    it = kzcDynamicArrayGetIterator(resourceManager->indexBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcIndexBuffer* resource = (struct KzcIndexBuffer*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcIndexBufferGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
            kzcIndexBufferGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            result = kzcIndexBufferFreeGPUMemory(resource);
            kzsErrorForward(result);
        }
    }

    kzsLog(KZS_LOG_LEVEL_INFO, "Vertex buffers");
    it = kzcDynamicArrayGetIterator(resourceManager->vertexBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcVertexBuffer* resource = (struct KzcVertexBuffer*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcVertexBufferGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
            kzcVertexBufferGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            result = kzcVertexBufferFreeGPUMemory(resource);
            kzsErrorForward(result);
        }
    }

    kzsLog(KZS_LOG_LEVEL_INFO, "Shaders");
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    it = kzcDynamicArrayGetIterator(resourceManager->shaders);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcShader* resource = (struct KzcShader*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcShaderGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
            kzcShaderGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            kzcShaderFreeGPUMemory(resource);
        }
    }
#endif

    kzsLog(KZS_LOG_LEVEL_INFO, "Textures");
    it = kzcDynamicArrayGetIterator(resourceManager->textures);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcTexture* resource = (struct KzcTexture*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcTextureGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
            kzcTextureGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            kzcTextureFreeGPUMemory(resource);
        }
    }

    kzsLog(KZS_LOG_LEVEL_INFO, "Framebuffers");
    it = kzcDynamicArrayGetIterator(resourceManager->frameBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcFrameBuffer* resource = (struct KzcFrameBuffer*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcFrameBufferGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
            kzcFrameBufferGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            kzcFrameBufferFreeGPUMemory(resource);
        }
    }

    kzsLog(KZS_LOG_LEVEL_INFO, "Done");
    kzsSuccess();
}

kzsError kzcResourceManagerUploadResourcesToGPU(const struct KzcResourceManager* resourceManager)
{
    kzsError result;

    struct KzcDynamicArrayIterator it;

    kzsGlFlush();

    /* Free frame buffers */
    it = kzcDynamicArrayGetIterator(resourceManager->indexBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcIndexBuffer* resource = (struct KzcIndexBuffer*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcIndexBufferGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            result = kzcIndexBufferRestoreGPU(resource);
            kzsErrorForward(result);
        }
    }

    it = kzcDynamicArrayGetIterator(resourceManager->vertexBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcVertexBuffer* resource = (struct KzcVertexBuffer*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcVertexBufferGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            result = kzcVertexBufferRestoreGPU(resource);
            kzsErrorForward(result);
        }
    }

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    it = kzcDynamicArrayGetIterator(resourceManager->shaders);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcShader* resource = (struct KzcShader*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcShaderGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            result = kzcShaderRestoreGPU(resource);
            kzsErrorForward(result);
        }
    }
#endif

    it = kzcDynamicArrayGetIterator(resourceManager->textures);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcTexture* resource = (struct KzcTexture*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcTextureGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            result = kzcTextureRestoreGPU(resource);
            kzsErrorForward(result);
        }
    }

    it = kzcDynamicArrayGetIterator(resourceManager->frameBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcFrameBuffer* resource = (struct KzcFrameBuffer*)kzcDynamicArrayIteratorGetValue(it);
        if(kzcFrameBufferGetMemoryType(resource) == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            result = kzcFrameBufferRestoreGPU(resource);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

void kzcResourceManagerGetTextureMemory(const struct KzcResourceManager* resourceManager,
                                        kzUint* out_GPUMemory, kzUint* out_RAMMemory)
{
    struct KzcDynamicArrayIterator it;
    kzUint memoryGPU = 0;
    kzUint memoryRAM = 0;

    it = kzcDynamicArrayGetIterator(resourceManager->textures);
    while(kzcDynamicArrayIterate(it))
    {
        kzUint textureGPU;
        kzUint textureRAM;
        struct KzcTexture* texture = (struct KzcTexture*)kzcDynamicArrayIteratorGetValue(it);
        kzcTextureGetMemorySize(texture, &textureGPU, &textureRAM);
        memoryGPU += textureGPU;
        memoryRAM += textureRAM;
    }

    *out_GPUMemory = memoryGPU;
    *out_RAMMemory = memoryRAM;
}

void kzcResourceManagerGetCompressedTextureMemory(const struct KzcResourceManager* resourceManager,
                                                  kzUint* out_GPUMemory, kzUint* out_RAMMemory)
{
    struct KzcDynamicArrayIterator it;
    kzUint memoryGPU = 0;
    kzUint memoryRAM = 0;

    it = kzcDynamicArrayGetIterator(resourceManager->textures);
    while(kzcDynamicArrayIterate(it))
    {
        kzUint textureGPU;
        kzUint textureRAM;
        struct KzcTexture* texture = (struct KzcTexture*)kzcDynamicArrayIteratorGetValue(it);
        kzcTextureGetMemorySize(texture, &textureGPU, &textureRAM);
        if(kzcTextureIsCompressed(texture))
        {
            memoryGPU += textureGPU;
            memoryRAM += textureRAM;
        }
    }

    *out_GPUMemory = memoryGPU;
    *out_RAMMemory = memoryRAM;
}

void kzcResourceManagerGetVertexBufferMemory(const struct KzcResourceManager* resourceManager,
                                             kzUint* out_GPUMemory, kzUint* out_RAMMemory)
{
    struct KzcDynamicArrayIterator it;
    kzUint memoryGPU = 0;
    kzUint memoryRAM = 0;

    kzsAssert(kzcIsValidPointer(resourceManager));

    it = kzcDynamicArrayGetIterator(resourceManager->vertexBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        kzUint vertexBufferGPU;
        kzUint vertexBufferRAM;
        struct KzcVertexBuffer* vertexBuffer = (struct KzcVertexBuffer*)kzcDynamicArrayIteratorGetValue(it);
        kzcVertexBufferGetMemorySize(vertexBuffer, &vertexBufferGPU, &vertexBufferRAM);
        memoryGPU += vertexBufferGPU;
        memoryRAM += vertexBufferRAM;
    }

    *out_GPUMemory = memoryGPU;
    *out_RAMMemory = memoryRAM;
}

void kzcResourceManagerGetIndexBufferMemory(const struct KzcResourceManager* resourceManager,
                                            kzUint* out_GPUMemory, kzUint* out_RAMMemory)
{
    struct KzcDynamicArrayIterator it;
    kzUint memoryGPU = 0;
    kzUint memoryRAM = 0;

    kzsAssert(kzcIsValidPointer(resourceManager));

    it = kzcDynamicArrayGetIterator(resourceManager->indexBuffers);
    while(kzcDynamicArrayIterate(it))
    {
        kzUint indexBufferGPU;
        kzUint indexBufferRAM;
        struct KzcIndexBuffer* indexBuffer = (struct KzcIndexBuffer*)kzcDynamicArrayIteratorGetValue(it);
        kzcIndexBufferGetMemorySize(indexBuffer, &indexBufferGPU, &indexBufferRAM);
        memoryGPU += indexBufferGPU;
        memoryRAM += indexBufferRAM;
    }

    *out_GPUMemory = memoryGPU;
    *out_RAMMemory = memoryRAM;
}

void kzcResourceManagerGetMemory(const struct KzcResourceManager* resourceManager,
                                 kzUint* out_GPUMemory, kzUint* out_RAMMemory)
{
    kzUint resourceGPU, resourceRAM;
    kzUint memoryGPU = 0;
    kzUint memoryRAM = 0;

    /* Get index buffer memory requirement. */
    kzcResourceManagerGetIndexBufferMemory(resourceManager, &resourceGPU, &resourceRAM);
    memoryGPU += resourceGPU;
    memoryRAM += resourceRAM;
    /* Get vertex buffer memory requirement. */
    kzcResourceManagerGetVertexBufferMemory(resourceManager, &resourceGPU, &resourceRAM);
    memoryGPU += resourceGPU;
    memoryRAM += resourceRAM;
    /* Get index buffer memory requirement. */
    kzcResourceManagerGetTextureMemory(resourceManager, &resourceGPU, &resourceRAM);
    memoryGPU += resourceGPU;
    memoryRAM += resourceRAM;

    *out_GPUMemory = memoryGPU;
    *out_RAMMemory = memoryRAM;
}

kzsError kzcResourceManagerAddTexture(const struct KzcResourceManager* resourceManager, struct KzcTexture* texture)
{
    kzsError result;

    kzsAssertText(!kzcDynamicArrayContains(resourceManager->textures, texture), "Trying to add text that is already added to resource manager");

#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcResourceManagerAllocation* allocation;
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(resourceManager), allocation, "ResourceTexture");
        kzsErrorForward(result);
        allocation->allocationId = resourceAllocationId++;
        allocation->allocationString = "Texture";
        allocation->resourceData = texture;
        result = kzcDynamicArrayAdd(resourceManager->resourceAllocations, allocation);
        kzsErrorForward(result);
#ifdef KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION
        if (allocation->allocationId == KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Breaking for resource manager debug allocation.");
            kzsDebugBreak();
        }
#endif
    }
#endif

    result = kzcDynamicArrayAdd(resourceManager->textures, texture);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcResourceManagerRemoveTexture(const struct KzcResourceManager* resourceManager, const struct KzcTexture* texture)
{
    kzsError result;
    kzsAssertText(kzcDynamicArrayContains(resourceManager->textures, texture), "Trying to remove texture that is not found from resource manager");
#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(resourceManager->resourceAllocations);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzcResourceManagerAllocation* allocation = (struct KzcResourceManagerAllocation*)kzcDynamicArrayIteratorGetValue(it);

            if(allocation->resourceData == texture)
            {
                result = kzcDynamicArrayRemove(resourceManager->resourceAllocations, allocation);
                kzsErrorForward(result);
                result = kzcMemoryFreeVariable(allocation);
                kzsErrorForward(result);
                break;
            }
        }
    }
#endif
    result = kzcDynamicArrayRemove(resourceManager->textures, texture);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError kzcResourceManagerReplaceTexture(const struct KzcResourceManager* resourceManager,
                                          const struct KzcTexture* sourceTexture, struct KzcTexture* targetTexture)
{
    kzUint index;
    kzBool found = kzcDynamicArrayFindIndex(resourceManager->textures, sourceTexture, &index);
    kzsErrorTest(found, KZC_ERROR_ELEMENT_NOT_FOUND, "Source texture not found");
    kzcDynamicArrayReplace(resourceManager->textures, index, targetTexture);
#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(resourceManager->resourceAllocations);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzcResourceManagerAllocation* allocation = (struct KzcResourceManagerAllocation*)kzcDynamicArrayIteratorGetValue(it);

            if(allocation->resourceData == sourceTexture)
            {
                allocation->resourceData = targetTexture;
                break;
            }
        }
    }
#endif

    kzsSuccess();
}

void kzcResourceManagerRemoveRendererReferences(const struct KzcResourceManager* resourceManager)
{
    struct KzcDynamicArrayIterator itemIterator;
    
    itemIterator = kzcDynamicArrayGetIterator(resourceManager->textures);
    while(kzcDynamicArrayIterate(itemIterator))
    {
        struct KzcTexture* texture = (struct KzcTexture*)kzcDynamicArrayIteratorGetValue(itemIterator);
        kzcTextureUnbindRenderer(texture);
    }
}

kzsError kzcResourceManagerAddIndexBuffer(const struct KzcResourceManager* resourceManager, struct KzcIndexBuffer* indexBuffer)
{
    kzsError result;
    kzsAssertText(!kzcDynamicArrayContains(resourceManager->indexBuffers, indexBuffer), "Trying to add index buffer that is already added to resource manager");
#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcResourceManagerAllocation* allocation;
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(resourceManager), allocation, "ResourceIndexBuffer");
        kzsErrorForward(result);
        allocation->allocationId = resourceAllocationId++;
        allocation->allocationString = "IndexBuffer";
        allocation->resourceData = indexBuffer;
        result = kzcDynamicArrayAdd(resourceManager->resourceAllocations, allocation);
        kzsErrorForward(result);
#ifdef KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION
        if (allocation->allocationId == KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Breaking for resource manager debug allocation.");
            kzsDebugBreak();
        }
#endif
    }
#endif
    result = kzcDynamicArrayAdd(resourceManager->indexBuffers, indexBuffer);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError kzcResourceManagerRemoveIndexBuffer(const struct KzcResourceManager* resourceManager, const struct KzcIndexBuffer* indexBuffer)
{
    kzsError result;
    kzsAssertText(kzcDynamicArrayContains(resourceManager->indexBuffers, indexBuffer), "Trying to remove index buffer that is not found from resource manager");
#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(resourceManager->resourceAllocations);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzcResourceManagerAllocation* allocation = (struct KzcResourceManagerAllocation*)kzcDynamicArrayIteratorGetValue(it);

            if(allocation->resourceData == indexBuffer)
            {
                result = kzcDynamicArrayRemove(resourceManager->resourceAllocations, allocation);
                kzsErrorForward(result);
                result = kzcMemoryFreeVariable(allocation);
                kzsErrorForward(result);
                break;
            }
        }
    }
#endif
    result = kzcDynamicArrayRemove(resourceManager->indexBuffers, indexBuffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcResourceManagerAddVertexBuffer(const struct KzcResourceManager* resourceManager, struct KzcVertexBuffer* vertexBuffer)
{
    kzsError result;
    kzsAssertText(!kzcDynamicArrayContains(resourceManager->vertexBuffers, vertexBuffer), "Trying to add vertex buffer that is already added to resource manager");
#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcResourceManagerAllocation* allocation;
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(resourceManager), allocation, "ResourceVertexBuffer");
        kzsErrorForward(result);
        allocation->allocationId = resourceAllocationId++;
        allocation->allocationString = "VertexBuffer";
        allocation->resourceData = vertexBuffer;
        result = kzcDynamicArrayAdd(resourceManager->resourceAllocations, allocation);
        kzsErrorForward(result);
#ifdef KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION
        if (allocation->allocationId == KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Breaking for resource manager debug allocation.");
            kzsDebugBreak();
        }
#endif
    }
#endif

    result = kzcDynamicArrayAdd(resourceManager->vertexBuffers, vertexBuffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcResourceManagerRemoveVertexBuffer(const struct KzcResourceManager* resourceManager, const struct KzcVertexBuffer* vertexBuffer)
{
    kzsError result;
    kzsAssertText(kzcDynamicArrayContains(resourceManager->vertexBuffers, vertexBuffer), "Trying to remove vertex buffer that is not found from resource manager");
#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(resourceManager->resourceAllocations);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzcResourceManagerAllocation* allocation = (struct KzcResourceManagerAllocation*)kzcDynamicArrayIteratorGetValue(it);

            if(allocation->resourceData == vertexBuffer)
            {
                result = kzcDynamicArrayRemove(resourceManager->resourceAllocations, allocation);
                kzsErrorForward(result);
                result = kzcMemoryFreeVariable(allocation);
                kzsErrorForward(result);
                break;
            }
        }
    }
#endif

    result = kzcDynamicArrayRemove(resourceManager->vertexBuffers, vertexBuffer);
    kzsErrorForward(result);
    kzsSuccess();
}

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
kzsError kzcResourceManagerAddShader(const struct KzcResourceManager* resourceManager, struct KzcShader* shader)
{
    kzsError result;
    kzsAssertText(!kzcDynamicArrayContains(resourceManager->shaders, shader), "Trying to add shader that is already added to resource manager");
#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcResourceManagerAllocation* allocation;
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(resourceManager), allocation, "ResourceShader");
        kzsErrorForward(result);
        allocation->allocationId = resourceAllocationId++;
        allocation->allocationString = "Shader";
        allocation->resourceData = shader;
        result = kzcDynamicArrayAdd(resourceManager->resourceAllocations, allocation);
        kzsErrorForward(result);
#ifdef KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION
        if (allocation->allocationId == KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Breaking for resource manager debug allocation.");
            kzsDebugBreak();
        }
#endif
    }
#endif
    result = kzcDynamicArrayAdd(resourceManager->shaders, shader);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcResourceManagerRemoveShader(const struct KzcResourceManager* resourceManager, const struct KzcShader* shader)
{
    kzsError result;
    kzsAssertText(kzcDynamicArrayContains(resourceManager->shaders, shader), "Trying to remove shader that is not found from resource manager");
#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(resourceManager->resourceAllocations);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzcResourceManagerAllocation* allocation = (struct KzcResourceManagerAllocation*)kzcDynamicArrayIteratorGetValue(it);

            if(allocation->resourceData == shader)
            {
                result = kzcDynamicArrayRemove(resourceManager->resourceAllocations, allocation);
                kzsErrorForward(result);
                result = kzcMemoryFreeVariable(allocation);
                kzsErrorForward(result);
                break;
            }
        }
    }
#endif
    result = kzcDynamicArrayRemove(resourceManager->shaders, shader);
    kzsErrorForward(result);
    kzsSuccess();
}
#endif

kzsError kzcResourceManagerAddFrameBuffer(const struct KzcResourceManager* resourceManager, struct KzcFrameBuffer* frameBuffer)
{
    kzsError result;
    
    kzsAssertText(!kzcDynamicArrayContains(resourceManager->frameBuffers, frameBuffer), "Trying to add frame buffer that is already added to resource manager");
#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcResourceManagerAllocation* allocation;
        result = kzcMemoryAllocVariable(kzcMemoryGetManager(resourceManager), allocation, "ResourceFrameBuffer");
        kzsErrorForward(result);
        allocation->allocationId = resourceAllocationId++;
        allocation->allocationString = "FrameBuffer";
        allocation->resourceData = frameBuffer;
        result = kzcDynamicArrayAdd(resourceManager->resourceAllocations, allocation);
        kzsErrorForward(result);
#ifdef KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION
        if (allocation->allocationId == KZC_RESOURCE_MANAGER_BREAK_ON_ALLOCATION)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Breaking for resource manager debug allocation.");
            kzsDebugBreak();
        }
#endif
    }
#endif
    
    result = kzcDynamicArrayAdd(resourceManager->frameBuffers, frameBuffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcResourceManagerRemoveFrameBuffer(const struct KzcResourceManager* resourceManager, const struct KzcFrameBuffer* frameBuffer)
{
    kzsError result;

    kzsAssertText(kzcDynamicArrayContains(resourceManager->frameBuffers, frameBuffer), "Trying to remove frame buffer that is not found from resource manager");
#ifdef KZC_RESOURCE_MANAGER_DEBUG
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(resourceManager->resourceAllocations);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzcResourceManagerAllocation* allocation = (struct KzcResourceManagerAllocation*)kzcDynamicArrayIteratorGetValue(it);

            if(allocation->resourceData == frameBuffer)
            {
                result = kzcDynamicArrayRemove(resourceManager->resourceAllocations, allocation);
                kzsErrorForward(result);
                result = kzcMemoryFreeVariable(allocation);
                kzsErrorForward(result);
                break;
            }
        }
    }
#endif
   
    result = kzcDynamicArrayRemove(resourceManager->frameBuffers, frameBuffer);
    kzsErrorForward(result);
    
    kzsSuccess();
}

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
struct KzcDynamicArray* kzcResourceManagerGetFixedShaderAttributes(const struct KzcResourceManager* resourceManager)
{
    kzsAssert(kzcIsValidPointer(resourceManager));
    return resourceManager->fixedShaderAttributes;
}
#endif

kzsError kzcResourceManagerLogGPUMemory(const struct KzcResourceManager* resourceManager)
{
    kzsError result;
    kzUint memoryRAM;
    kzUint memoryGPU;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcIsValidPointer(resourceManager));

    memoryManager = kzcMemoryGetManager(resourceManager);

    kzcResourceManagerGetTextureMemory(resourceManager, &memoryGPU, &memoryRAM);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Texture memory: GPU %d RAM %d\n", memoryGPU, memoryRAM);
    kzsErrorForward(result);
    kzcResourceManagerGetCompressedTextureMemory(resourceManager, &memoryGPU, &memoryRAM);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "of which compressed: GPU %d RAM %d\n", memoryGPU, memoryRAM);
    kzsErrorForward(result);
    kzcResourceManagerGetVertexBufferMemory(resourceManager, &memoryGPU, &memoryRAM);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Vertex buffer memory: GPU %d RAM %d\n", memoryGPU, memoryRAM);
    kzsErrorForward(result);
    kzcResourceManagerGetIndexBufferMemory(resourceManager, &memoryGPU, &memoryRAM);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Index buffer memory: GPU %d RAM %d\n", memoryGPU, memoryRAM);
    kzsErrorForward(result);
    kzcResourceManagerGetMemory(resourceManager, &memoryGPU, &memoryRAM);
    result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Total memory: GPU %d RAM %d\n", memoryGPU, memoryRAM);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcResourceManagerSetTimeMeasurementEnabled(struct KzcResourceManager* resourceManager, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(resourceManager));
    resourceManager->measurementEnabled = enabled;
}

kzBool kzcResourceManagerIsTimeMeasurementEnabled(const struct KzcResourceManager* resourceManager)
{
    kzsAssert(kzcIsValidPointer(resourceManager));
    return resourceManager->measurementEnabled;
}

void kzcResourceManagerAddResourceCumulativeTime_private(struct KzcResourceManager* resourceManager, kzUint startTime, enum KzcResourceMeasurementType measurementType)
{
    kzsAssert(kzcIsValidPointer(resourceManager));

    if(resourceManager->measurementEnabled)
    {
        kzUint currentTime;
        currentTime = kzsTimeGetCurrentTimestamp();

        switch(measurementType)
        {
        case KZC_RESOURCE_MEASUREMENT_INDEX_BUFFER:
            {
                resourceManager->measurementInfo.indexBufferCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
        case KZC_RESOURCE_MEASUREMENT_VERTEX_BUFFER:
            {
                resourceManager->measurementInfo.vertexBufferCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
        case KZC_RESOURCE_MEASUREMENT_TEXTURE:
            {
                resourceManager->measurementInfo.textureCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
        case KZC_RESOURCE_MEASUREMENT_FRAME_BUFFER:
            {
                resourceManager->measurementInfo.frameBufferCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        case KZC_RESOURCE_MEASUREMENT_SHADER:
            {
                resourceManager->measurementInfo.shaderCumulativeTime += (kzUint)(currentTime - startTime);
                break;
            }
#endif
        default:
            {
                kzsAssertText(KZ_FALSE, "Invalid resource given for cumulative function calculation");
            }
        }
    }
}

kzsError kzcResourceManagerPrintTimeMeasurementInfo(const struct KzcResourceManager* resourceManager)
{
    kzsError result;
    kzMutableString infoString;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(resourceManager);

    result = kzcStringFormat(memoryManager, "Vertex buffer time: %d", &infoString, resourceManager->measurementInfo.vertexBufferCumulativeTime);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_INFO, infoString);
    result = kzcStringDelete(infoString);
    kzsErrorForward(result);

    result = kzcStringFormat(memoryManager, "Index buffer time: %d", &infoString, resourceManager->measurementInfo.indexBufferCumulativeTime);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_INFO, infoString);
    result = kzcStringDelete(infoString);
    kzsErrorForward(result);

    result = kzcStringFormat(memoryManager, "Texture time: %d", &infoString, resourceManager->measurementInfo.textureCumulativeTime);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_INFO, infoString);
    result = kzcStringDelete(infoString);
    kzsErrorForward(result);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    result = kzcStringFormat(memoryManager, "Shader time: %d", &infoString, resourceManager->measurementInfo.shaderCumulativeTime);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_INFO, infoString);
    result = kzcStringDelete(infoString);
    kzsErrorForward(result);
#endif

    result = kzcStringFormat(memoryManager, "Frame buffer time: %d", &infoString, resourceManager->measurementInfo.frameBufferCumulativeTime);
    kzsErrorForward(result);
    kzsLog(KZS_LOG_LEVEL_INFO, infoString);
    result = kzcStringDelete(infoString);
    kzsErrorForward(result);

    kzsSuccess();
}
