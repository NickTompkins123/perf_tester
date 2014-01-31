/**
 * \file
 * IndexBuffer resource
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_resource_index_buffer.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/renderer/kzc_renderer.h>

#include <core/resource_manager/kzc_resource_manager.h>

#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_opengl.h>
#include <system/debug/kzs_log.h>
#include <system/time/kzs_tick.h>


/** Specifies how index buffer is used, i.e. are its contents static or dynamic. */
enum KzcIndexBufferUsage
{
    KZC_INDEX_BUFFER_USAGE_STATIC = 0  /**< Static usage. */
};

/* Forward declaration structures. */
struct KzcMemoryManager;


/** Internal structure of IndexBuffer */
struct KzcIndexBuffer
{
    struct KzcResourceManager* resourceManager;         /**< Resource manager that contains this resource. */

    kzUint bufferHandle;                                /**< OpenGL handle for this index buffer. */

    kzU16* data;                                        /**< Index data. */
    kzUint indexCount;                                  /**< Number of indices in this buffer. */
    enum KzcPrimitiveType type;                         /**< Type of the buffer data. */

    enum KzcIndexBufferUsage usage;                     /**< Index buffer usage, static or dynamic. */
    enum KzcResourceMemoryType memoryType;              /**< Memory type of this resource, either GPU or GPU+RAM. */
};


/** Internal function for deploying index bguffer to GPU memory. */
static void kzcIndexBufferDeployToGPU_internal(struct KzcIndexBuffer* indexBuffer);


kzsError kzcIndexBufferCreate(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType, 
                              const kzU16* indexArray, kzUint indexCount, enum KzcPrimitiveType primitiveType,
                              struct KzcIndexBuffer** out_indexBuffer)
{
    kzsError result;
    struct KzcIndexBuffer* indexBuffer;
    struct KzcMemoryManager* memoryManager;
    kzUint measurementStartTime = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(resourceManager));

    memoryManager = kzcMemoryGetManager(resourceManager);

    /* Allocate memory for index buffer. */
    result = kzcMemoryAllocVariable(memoryManager, indexBuffer, "Index buffer");
    kzsErrorForward(result);

    /* Save the initialized data to the buffer. */
#ifdef KZC_USE_VERTEXBUFFEROBJECTS
    indexBuffer->memoryType = memoryType;
#else
    indexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
#endif

#ifdef KZC_RESOURCE_FORCE_TO_RAM
    if(memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        indexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
#endif

    result = kzcMemoryAllocPointer(memoryManager, &indexBuffer->data, sizeof(*indexBuffer->data) * indexCount, 
        "Index buffer data");
    kzsErrorForward(result);
    kzsMemcpy(indexBuffer->data, indexArray, sizeof(*indexBuffer->data) * indexCount);
    indexBuffer->indexCount = indexCount;
    indexBuffer->type = primitiveType;
    indexBuffer->resourceManager = resourceManager;

    /* Creates a new index buffer. */
#ifdef KZC_USE_VERTEXBUFFEROBJECTS
    if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM || indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        kzcIndexBufferDeployToGPU_internal(indexBuffer);
    }
#endif

    result = kzcResourceManagerAddIndexBuffer(resourceManager, indexBuffer);
    kzsErrorForward(result);

    /* If the memory is intended to be GPU only, delete the RAM memory. */
    if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        /* Temporarily put indexbuffer memory type to GPU and RAM, as it's instantly removed from RAM. */
        indexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
        result = kzcIndexBufferFreeRAMMemory(indexBuffer);
        kzsErrorForward(result);
    }

    kzcResourceManagerAddResourceCumulativeTime_private(resourceManager, measurementStartTime, KZC_RESOURCE_MEASUREMENT_INDEX_BUFFER);

    *out_indexBuffer = indexBuffer;
    kzsSuccess();
}

kzsError kzcIndexBufferCopy(const struct KzcIndexBuffer* sourceIndexBuffer, struct KzcIndexBuffer** out_targetIndexBuffer)
{
    kzsError result;
    struct KzcIndexBuffer* indexBuffer;

    result = kzcIndexBufferCreate(sourceIndexBuffer->resourceManager, sourceIndexBuffer->memoryType, sourceIndexBuffer->data,
        sourceIndexBuffer->indexCount, sourceIndexBuffer->type, &indexBuffer);
    kzsErrorForward(result);

    *out_targetIndexBuffer = indexBuffer;
    kzsSuccess();
}

kzsError kzcIndexBufferFree(struct KzcIndexBuffer* indexBuffer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(indexBuffer));

    result = kzcIndexBufferFreeGPUMemory(indexBuffer);
    kzsErrorForward(result);

    result = kzcIndexBufferFreeRAMMemory(indexBuffer);
    kzsErrorForward(result);

    result = kzcResourceManagerRemoveIndexBuffer(indexBuffer->resourceManager, indexBuffer);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(indexBuffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcIndexBufferFreeGPUMemory(struct KzcIndexBuffer* indexBuffer)
{
    kzsAssert(kzcIsValidPointer(indexBuffer));

    if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
        indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
#ifdef KZC_USE_VERTEXBUFFEROBJECTS
        /* Remove only the buffer from GPU. */
        kzsGlDeleteBuffers(1, &indexBuffer->bufferHandle);
        indexBuffer->bufferHandle = 0;
#endif

        if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            indexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_NONE;
        }
        else
        {
            indexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY;
        }
    }

    kzsSuccess();
}

kzsError kzcIndexBufferFreeRAMMemory(struct KzcIndexBuffer* indexBuffer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(indexBuffer));

    if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
        indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
    {
        /* Delete the data. */
        result = kzcMemoryFreePointer(indexBuffer->data);
        kzsErrorForward(result);
        indexBuffer->data = KZ_NULL;

        if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
        {
            indexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY;
        }
        else if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            indexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_NONE;
        }
    }

    kzsSuccess();
}

static void kzcIndexBufferDeployToGPU_internal(struct KzcIndexBuffer* indexBuffer)
{
    /* Store original state. */
    kzUint originalBuffer;
    kzsGlGetIntegerv(KZS_GL_ELEMENT_ARRAY_BUFFER_BINDING, (kzInt*)&originalBuffer);
    kzsGlGenBuffers(1, &indexBuffer->bufferHandle);
    /* Use the index buffer. */
    kzsGlBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, indexBuffer->bufferHandle);
    /* Allocate memory for the buffer. */
    kzsGlBufferData(KZS_GL_ELEMENT_ARRAY_BUFFER, (kzInt)(indexBuffer->indexCount * sizeof(kzU16)), indexBuffer->data, KZS_GL_STATIC_DRAW);
    /* Set back to original state. */
    kzsGlBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, originalBuffer);

    indexBuffer->usage = KZC_INDEX_BUFFER_USAGE_STATIC;
}

kzsError kzcIndexBufferRestoreGPU(struct KzcIndexBuffer* indexBuffer)
{
    kzsAssert(kzcIsValidPointer(indexBuffer));

    if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
    {
        kzcIndexBufferDeployToGPU_internal(indexBuffer);

        indexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
    else if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "This index buffer is already contained in GPU and RAM!");
    }
    else if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "This index buffer is already contained in GPU!");
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Cannot restore shader that isn't contained in RAM memory!");
    }

    kzsSuccess();
}

kzsError kzcIndexBufferAttach(const struct KzcIndexBuffer* indexBuffer, struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(indexBuffer));

#ifdef KZC_USE_VERTEXBUFFEROBJECTS
    kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_INDEX, indexBuffer->bufferHandle);
    kzcRendererSetIndexData(renderer, KZ_NULL);
#else
    kzcRendererSetIndexData(renderer, indexBuffer->data);
#endif

    /** Renderer needs to know how many indices this index buffer contains. */
    kzcRendererSetIndexCount(renderer, indexBuffer->indexCount);
    /** Renderer needs to know what type of primitives to draw. */
    kzcRendererSetIndexType(renderer, indexBuffer->type);

    kzsSuccess();
}

void kzcIndexBufferDetach(struct KzcRenderer* renderer)
{
#ifdef KZC_USE_VERTEXBUFFEROBJECTS
    kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);
#endif
}

void kzcIndexBufferGetMemorySize(const struct KzcIndexBuffer* indexBuffer, kzUint* out_GPUMemory, kzUint* out_RAMMemory)
{
    kzUint memorySizeGPU;
    kzUint memorySizeRAM = 0;

    kzsAssert(indexBuffer != KZ_NULL);

#ifdef KZC_USE_VERTEXBUFFEROBJECTS
    memorySizeGPU = sizeof(*indexBuffer->data) * indexBuffer->indexCount;
    if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        memorySizeRAM = memorySizeGPU;
    }
    else if(indexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
    {
        memorySizeRAM = memorySizeGPU;
        memorySizeGPU = 0;
    }
#else
    memorySizeGPU = 0;
    memorySizeRAM = sizeof(*indexBuffer->data) * indexBuffer->indexCount;
#endif

    *out_GPUMemory = memorySizeGPU;
    *out_RAMMemory = memorySizeRAM;
}

enum KzcResourceMemoryType kzcIndexBufferGetMemoryType(const struct KzcIndexBuffer* indexBuffer)
{
    kzsAssert(kzcIsValidPointer(indexBuffer));
    return indexBuffer->memoryType;
}
