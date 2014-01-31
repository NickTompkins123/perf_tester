/**
 * \file
 * Index buffer resource.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_RESOURCE_INDEX_BUFFER_H
#define KZC_RESOURCE_INDEX_BUFFER_H

#include "kzc_index_buffer_type.h"

#include <core/resource_manager/kzc_resource_memory_type.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcResourceManager;
struct KzcRenderer;


/**
 * \struct KzcIndexBuffer
 * Structure of an index buffer.
 */
struct KzcIndexBuffer;


/** Returns a data for index buffer. */
kzsError kzcIndexBufferFetchData(const struct KzcIndexBuffer* indexBuffer, void** out_data,
                                 kzUint* out_indexCount,  kzUint* out_bufferID);

/** Free given index buffer from both RAM and GPU memory */
kzsError kzcIndexBufferFree(struct KzcIndexBuffer* indexBuffer);

/** Free given index buffer from GPU memory */
kzsError kzcIndexBufferFreeGPUMemory(struct KzcIndexBuffer* indexBuffer);

/** Free given index buffer from RAM memory */
kzsError kzcIndexBufferFreeRAMMemory(struct KzcIndexBuffer* indexBuffer);

/** Restores given index buffer to GPU memory. */
kzsError kzcIndexBufferRestoreGPU(struct KzcIndexBuffer* indexBuffer);

/** Attaches an index buffer for rendering. */
kzsError kzcIndexBufferAttach(const struct KzcIndexBuffer* indexBuffer, struct KzcRenderer* renderer);

/** Detaches an index buffer. */
void kzcIndexBufferDetach(struct KzcRenderer* renderer);

/** Gets memory requirements for index buffer. */
void kzcIndexBufferGetMemorySize(const struct KzcIndexBuffer* indexBuffer, kzUint* out_GPUMemory, kzUint* out_RAMMemory);

/** Internal constructor for index buffer. */
kzsError kzcIndexBufferCreate(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType, 
                              const kzU16* indexArray, kzUint indexCount, enum KzcPrimitiveType primitiveType,
                              struct KzcIndexBuffer** out_indexBuffer);
/** Copies index buffer to another. */
kzsError kzcIndexBufferCopy(const struct KzcIndexBuffer* sourceIndexBuffer, struct KzcIndexBuffer** out_targetIndexBuffer);

/** Gets memory type from index buffer. */
enum KzcResourceMemoryType kzcIndexBufferGetMemoryType(const struct KzcIndexBuffer* indexBuffer);


#endif
