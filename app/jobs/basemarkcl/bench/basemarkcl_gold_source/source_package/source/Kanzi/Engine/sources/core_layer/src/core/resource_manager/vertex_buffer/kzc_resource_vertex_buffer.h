/**
 * \file
 * Vertex buffer structure.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_RESOURCE_VERTEX_BUFFER_H
#define KZC_RESOURCE_VERTEX_BUFFER_H


#include <core/resource_manager/vertex_buffer/kzc_resource_vertex.h>
#include <core/resource_manager/kzc_resource_memory_type.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Vertex buffer usage, either static or dynamic. */
enum KzcVertexBufferUsage
{
    KZC_VERTEX_BUFFER_USAGE_STATIC, /**< Static usage of vertex buffer. */
    KZC_VERTEX_BUFFER_USAGE_DYNAMIC /**< Dynamic usage of vertex buffer. */
};


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcDynamicArray;
struct KzcVertexAttributeInternal;
struct KzcVertexList;
struct KzcRenderer;
struct KzcHashMap;

/**
* \struct KzcVertexBuffer
* Structure of a Vertex buffer.
*/
struct KzcVertexBuffer;


/** Creates new vertex buffer resource. */
kzsError kzcVertexBufferCreate(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType,
                               kzBool useVertexBufferObjects, enum KzcVertexBufferUsage usage, struct KzcVertexList* vertexList,
                               struct KzcVertexBuffer** out_vertexBuffer);

/** Copies vertex buffer to new vertex buffer with new GPU handle and own data. */
kzsError kzcVertexBufferCopy(const struct KzcVertexBuffer* sourceVertexBuffer, struct KzcVertexBuffer** out_targetVertexBuffer);

/** Free given vertex buffer from both RAM and GPU memory */
kzsError kzcVertexBufferFree(struct KzcVertexBuffer* vertexBuffer);

/** Free given vertex buffer from GPU memory */
kzsError kzcVertexBufferFreeGPUMemory(struct KzcVertexBuffer* vertexBuffer);

/** Free given vertex buffer from RAM memory */
kzsError kzcVertexBufferFreeRAMMemory(struct KzcVertexBuffer* vertexBuffer);

/** Restores given vertex buffer to GPU memory. */
kzsError kzcVertexBufferRestoreGPU(struct KzcVertexBuffer* vertexBuffer);

/** Gets the vertex buffer GPU handle */
kzUint kzcVertexBufferGetHandle(const struct KzcVertexBuffer* vertexBuffer);

/** Assigns vertex attribute mapping to vertex buffer. HashMap<string(shaderAttributeName), string(vertexAttributeName)>. */
kzsError kzcVertexBufferAssignAttributeMapping(struct KzcVertexBuffer* vertexBuffer, const void* material,
                                               const struct KzcHashMap* vertexAttributeMap,
                                               const struct KzcDynamicArray* shaderAttributes);
/** Sets active material (generic pointer type) to vertex buffer, with shader attributes. */
kzsError kzcVertexBufferSetActiveShader(struct KzcVertexBuffer* vertexBuffer, const void* material, 
                                          const struct KzcDynamicArray* shaderAttributes);

/** Attaches a vertex buffer for rendering. */
kzsError kzcVertexBufferAttach(struct KzcVertexBuffer* vertexBuffer, struct KzcRenderer* renderer);
/** Detaches a vertex buffer currently attached. */
kzsError kzcVertexBufferDetach(struct KzcVertexBuffer* vertexBuffer, struct KzcRenderer* renderer);

/** Gets vertex buffer's memory size. */
void kzcVertexBufferGetMemorySize(const struct KzcVertexBuffer* vertexBuffer, kzUint* out_GPUMemory, kzUint* out_RAMMemory);

/** Gets vertices from vertex buffer as array of KzcVector3. */
kzsError kzcVertexBufferGetVertices(const struct KzcMemoryManager* memoryManager, const struct KzcVertexBuffer* vertexBuffer, struct KzcDynamicArray** out_vertices);
/** Gets semantic data from vertex buffer. Returns KZ_NULL if no semantic found. */
kzsError kzcVertexBufferGetSemanticData(const struct KzcMemoryManager* memoryManager, const struct KzcVertexBuffer* vertexBuffer,
                                        enum KzcVertexAttributeSemantic semantic, void** out_data);
/** Gets vertex count from vertex buffer. */
kzUint kzcVertexBufferGetVertexCount(const struct KzcVertexBuffer* vertexBuffer);

/** Gets size of a vertex with all attributes. */
kzUint kzcVertexBufferGetVertexSize(const struct KzcVertexBuffer* vertexBuffer);

/* TODO: Comment. */
void kzcVertexBufferAddOverrideSemantic(struct KzcVertexBuffer* vertexBuffer, enum KzcVertexAttributeSemantic semantic,
                                        kzUint dimension, void* data);
void kzcVertexBufferRemoveOverrideSemantic(struct KzcVertexBuffer* vertexBuffer, enum KzcVertexAttributeSemantic semantic);
void kzcVertexBufferRemoveOverrideSemantics(struct KzcVertexBuffer* vertexBuffer);


/** Returns if vertex buffer contains specific shader attribute. */
kzBool kzcVertexBufferContainsShaderAttribute(const struct KzcVertexBuffer* vertexBuffer, kzString shaderAttributeName);


/** Returns if half float vertex format is supported. */
kzBool kzcVertexBufferIsHalfFloatSupported(void);


/** Gets memory type from vertex buffer. */
enum KzcResourceMemoryType kzcVertexBufferGetMemoryType(const struct KzcVertexBuffer* vertexBuffer);


#endif
