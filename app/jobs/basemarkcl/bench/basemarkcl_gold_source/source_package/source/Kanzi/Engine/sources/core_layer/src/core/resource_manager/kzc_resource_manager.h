/**
 * \file
 * Resource manager component. Resource manager holds all low level resources of application.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_RESOURCE_MANAGER_H
#define KZC_RESOURCE_MANAGER_H


#include <system/wrappers/kzs_opengl_base.h>
#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


#if 1
/** If defined, vertex buffer objects are used. */
#define KZC_USE_VERTEXBUFFEROBJECTS
#endif

#ifdef ANDROID
#define KZC_RESOURCE_FORCE_TO_RAM
#endif


/** Resource measurement types, used for measuring creation time of GPU resources. */
enum KzcResourceMeasurementType
{
    KZC_RESOURCE_MEASUREMENT_INDEX_BUFFER, /**< Measurement for index buffer. */
    KZC_RESOURCE_MEASUREMENT_VERTEX_BUFFER, /**< Measurement for vertex buffer. */
    KZC_RESOURCE_MEASUREMENT_TEXTURE, /**< Measurement for texture. */
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    KZC_RESOURCE_MEASUREMENT_SHADER, /**< Measurement for shader. */
#endif
    KZC_RESOURCE_MEASUREMENT_FRAME_BUFFER /**< Measurement for frame buffer. */
};


/* Forward declarations */
struct KzcTexture;
struct KzcVertexBuffer;
struct KzcIndexBuffer;
struct KzcFrameBuffer;
struct KzcShader;
struct KzcMemoryManager;
struct KzcDynamicArray;

/**
 * \struct KzcResourceManager
 * Structure for resource manager. Manager for all GPU-specific resources:
 * vertex buffers, index buffers, textures, shaders and frame buffer objects.
 */
struct KzcResourceManager;


/** Creates a new resource manager that contains access to the low-level resources of application. */
kzsError kzcResourceManagerCreate(const struct KzcMemoryManager* memoryManager, struct KzcResourceManager** out_resourceManager);
/** Deletes resource manager and all elements associated to it. */
kzsError kzcResourceManagerDelete(struct KzcResourceManager* resourceManager);
/** Frees all frame buffers from resource manager. */
kzsError kzcResourceManagerFreeFramebuffers(const struct KzcResourceManager* resourceManager);

/** Uploads all resources to GPU. */
kzsError kzcResourceManagerUploadResourcesToGPU(const struct KzcResourceManager* resourceManager);
/** Frees all resources from GPU. */
kzsError kzcResourceManagerFreeResourcesFromGPU(const struct KzcResourceManager* resourceManager);

/** Gets memory requirements of all textures in total. */
void kzcResourceManagerGetTextureMemory(const struct KzcResourceManager* resourceManager,
                                        kzUint* out_GPUMemory, kzUint* out_RAMMemory);
/** Gets memory requirements of compressed textures in total. */
void kzcResourceManagerGetCompressedTextureMemory(const struct KzcResourceManager* resourceManager,
                                                  kzUint* out_GPUMemory, kzUint* out_RAMMemory);
/** Gets memory requirements for all vertex buffers in total. */
void kzcResourceManagerGetVertexBufferMemory(const struct KzcResourceManager* resourceManager,
                                             kzUint* out_GPUMemory, kzUint* out_RAMMemory);
/** Gets memory requirements for all index buffers in total. */
void kzcResourceManagerGetIndexBufferMemory(const struct KzcResourceManager* resourceManager,
                                            kzUint* out_GPUMemory, kzUint* out_RAMMemory);
/** Gets memory requirements for all resources in total. */
void kzcResourceManagerGetMemory(const struct KzcResourceManager* resourceManager,
                                 kzUint* out_GPUMemory, kzUint* out_RAMMemory);

/** Adds texture to resource manager. */
kzsError kzcResourceManagerAddTexture(const struct KzcResourceManager* resourceManager, struct KzcTexture* texture);
/** Removes texture from resource manager. */
kzsError kzcResourceManagerRemoveTexture(const struct KzcResourceManager* resourceManager, const struct KzcTexture* texture);
/** Replaces a texture in resource manager with another one. */
kzsError kzcResourceManagerReplaceTexture(const struct KzcResourceManager* resourceManager,
                                          const struct KzcTexture* sourceTexture, struct KzcTexture* targetTexture);
/** Replaces a texture in resource manager with another one. */
void kzcResourceManagerRemoveRendererReferences(const struct KzcResourceManager* resourceManager);

/** Adds index buffer to resource manager. */
kzsError kzcResourceManagerAddIndexBuffer(const struct KzcResourceManager* resourceManager, struct KzcIndexBuffer* indexBuffer);
/** Removes index buffer from resource manager. */
kzsError kzcResourceManagerRemoveIndexBuffer(const struct KzcResourceManager* resourceManager, const struct KzcIndexBuffer* indexBuffer);

/** Adds vertex buffer to resource manager. */
kzsError kzcResourceManagerAddVertexBuffer(const struct KzcResourceManager* resourceManager, struct KzcVertexBuffer* vertexBuffer);
/** Removes vertex buffer from resource manager. */
kzsError kzcResourceManagerRemoveVertexBuffer(const struct KzcResourceManager* resourceManager, const struct KzcVertexBuffer* vertexBuffer);

/** Adds shader to resource manager. */
kzsError kzcResourceManagerAddShader(const struct KzcResourceManager* resourceManager, struct KzcShader* shader);
/** Removes shader from resource manager. */
kzsError kzcResourceManagerRemoveShader(const struct KzcResourceManager* resourceManager, const struct KzcShader* shader);

/** Adds frame buffer to resource manager. */
kzsError kzcResourceManagerAddFrameBuffer(const struct KzcResourceManager* resourceManager, struct KzcFrameBuffer* frameBuffer);
/** Removes frame buffer from resource manager. */
kzsError kzcResourceManagerRemoveFrameBuffer(const struct KzcResourceManager* resourceManager, const struct KzcFrameBuffer* frameBuffer);

/** Prints debug informations on resource manager gpu usage. */
kzsError kzcResourceManagerLogGPUMemory(const struct KzcResourceManager* resourceManager);

/** Sets time measurement enabled for resource manager. */
void kzcResourceManagerSetTimeMeasurementEnabled(struct KzcResourceManager* resourceManager, kzBool enabled);
/** Returns if resource manager's time measurement is enabled. */
kzBool kzcResourceManagerIsTimeMeasurementEnabled(const struct KzcResourceManager* resourceManager);
/** Private function for adding cumulative time for resource manager's resources. */
void kzcResourceManagerAddResourceCumulativeTime_private(struct KzcResourceManager* resourceManager, kzUint startTime, enum KzcResourceMeasurementType measurementType);
/** Prints resource manager measurement info. */
kzsError kzcResourceManagerPrintTimeMeasurementInfo(const struct KzcResourceManager* resourceManager);

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
/** Returns fixed shader attributes. */
struct KzcDynamicArray* kzcResourceManagerGetFixedShaderAttributes(const struct KzcResourceManager* resourceManager);
#endif


#endif
