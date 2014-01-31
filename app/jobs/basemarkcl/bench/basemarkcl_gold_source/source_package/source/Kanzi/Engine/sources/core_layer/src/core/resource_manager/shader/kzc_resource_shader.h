/**
 * \file
 * Shader resource.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_RESOURCE_SHADER_H
#define KZC_RESOURCE_SHADER_H


#include <system/wrappers/kzs_opengl_base.h>

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)

#include <core/resource_manager/kzc_resource_memory_type.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcRenderer;
struct KzcResourceManager;
struct KzcDynamicArray;
struct KzcShader;
struct KzcHashMap;


/**
* Creates a shader program from GLSL source code. 
* \param out_createSuccess Was compilation / linking successful. KZ_NULL if not used.
*/
kzsError kzcShaderSourceCreate(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                               kzString vertexShader, kzString fragmentShader, const struct KzcDynamicArray* attributeList, 
                               kzBool* out_createSuccess, struct KzcShader** out_shader);

/** 
* Creates a binary shader program with binary containing both vertex & fragment programs. 
* \param out_createSuccess Was compilation / linking successful. KZ_NULL if not used.
*/
kzsError kzcShaderBinaryCombinedCreate(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                                       const void* binaryData, kzUint binarySize, const struct KzcDynamicArray* attributeList, 
                                       kzBool* out_createSuccess, struct KzcShader** out_shader);

/**
* Creates a binary shader program with separate binaries for vertex and fragment shader.
* \param out_createSuccess Was compilation / linking successful. KZ_NULL if not used.
*/
kzsError kzcShaderBinarySeparatedCreate(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                                        const void* vertexData, kzUint vertexDataSize, const void* fragmentData, kzUint fragmentDataSize, 
                                        const struct KzcDynamicArray* attributeList, 
                                        kzBool* out_createSuccess, struct KzcShader** out_shader);

/** Creates a shader program from GLSL source code.
* \param out_createSuccess Was compilation / linking successful. KZ_NULL if not used.
*/
kzsError kzcShaderCreateFromFile(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                                 kzString vertexShaderPath, kzString fragmentShaderPath, kzBool* out_createSuccess,
                                 struct KzcShader** out_shader);

/** Free given shader from both RAM and GPU memory. */
kzsError kzcShaderDelete(struct KzcShader* shader);

/** Free given shader from GPU memory */
void kzcShaderFreeGPUMemory(struct KzcShader* shader);

/** Free given shader from RAM memory */
kzsError kzcShaderFreeRAMMemory(struct KzcShader* shader);

/** Restores given shader to GPU memory. */
kzsError kzcShaderRestoreGPU(struct KzcShader* shader);

/** Returns attributes from given shader. KzcHashMap<string(shaderAttributeName), kzInt*(attributeIndex)>, attributeIndex holding pointer to integer value. */
struct KzcHashMap* kzcShaderGetAttributes(const struct KzcShader* shader);

/** Applies shader for renderer. */
kzsError kzcShaderApply(const struct KzcShader* shader, struct KzcRenderer* renderer);

/** Gets memory type from shader. */
enum KzcResourceMemoryType kzcShaderGetMemoryType(const struct KzcShader* shader);


#endif

#endif
