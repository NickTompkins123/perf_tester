/**
 * \file
 * Shader resource
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */

#include "kzc_resource_shader.h"


#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/io/kzc_file.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/kzc_error_codes.h>
#include <core/debug/kzc_log.h>

#include <system/debug/kzs_log.h>
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_2_0
#include <system/wrappers/kzs_opengles20.h>
#elif KZ_OPENGL_VERSION == KZ_OPENGL_2_1
#include <system/wrappers/kzs_opengl21.h>
#endif 
#include <system/wrappers/kzs_memory.h>
#include <system/time/kzs_tick.h>


/* Define for disabling shader compilation. */
#define COMPILE_SHADERS


/** Shader type. I.e. binary or source format. */
enum KzcShaderType
{
    KZC_SHADER_TYPE_ONLINE,             /**< Online compiled shader. */
    KZC_SHADER_TYPE_BINARY_COMBINED,    /**< Offline compiled shader containing shared data for both vertex & fragment shader. */
    KZC_SHADER_TYPE_BINARY_SEPARATED    /**< Offline compiled shader containing separated data for vertex & fragment shader. */
};

struct KzcShader
{
    struct KzcResourceManager* resourceManager; /** Resource manager that contains this resource. */

    enum KzcShaderType type;                 /**< Shader type. */
    union
    {
        struct
        {
            kzUint vertexDataSize;          /**< Vertex binary size in bytes. */
            kzUint fragmentDataSize;        /**< Fragment binary size in bytes. */
            void* vertexData;               /**< Binary code for vertex shader. */
            void* fragmentData;             /**< Binary code for fragment shader. */
        } binarySeparated;                  /**< Binary separated data. */
        struct
        {
            kzUint size;                    /**< Binary size in bytes. */
            void* data;                     /**< Binary code. */
        } binaryCombined;                   /**< Binary combined data. */
        struct
        {
            kzMutableString vertex;         /**< Vertex shader. */
            kzMutableString fragment;       /**< Fragment shader. */
        } code;                             /**< Code data. */
    } shaderData;

    struct KzcHashMap* vertexAttributes; /**< Vertex attributes for this shader. <string(attributeName), uint(attributeIndex)>. */
    kzUint* vertexAttributeIndices; /**< Vertex attribute indices. */

    kzUint programHandle;                   /**< Shader program handle. */
    kzUint vertexShaderHandle;              /**< Vertex shader object handle. */
    kzUint fragmentShaderHandle;            /**< Fragment shader object handle. */
    enum KzcResourceMemoryType memoryType;  /**< Memory type of this resource, either GPU or GPU+RAM. */
};


/** Helper function for deploying shader to GPU. */
static kzsError kzcShaderDeployToGPU_internal(struct KzcShader* shader, kzBool* out_deploySuccess);


static kzsError kzcShaderCreate_internal(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType,
                                         enum KzcShaderType shaderType, const struct KzcDynamicArray* attributeList,
                                         struct KzcShader** out_shader)
{
    kzsError result;
    struct KzcShader* shader;
    struct KzcDynamicArrayIterator it;
    struct KzcMemoryManager* memoryManager;
    kzUint i;

    kzsAssert(kzcIsValidPointer(resourceManager));

    memoryManager = kzcMemoryGetManager(resourceManager);

    /* Allocate memory for shader. */
    result = kzcMemoryAllocVariable(memoryManager, shader, "Shader");
    kzsErrorForward(result);

    /* Assign data for shader. */
    shader->memoryType = memoryType;
    shader->resourceManager = resourceManager;
    shader->type = shaderType;

#ifdef KZC_RESOURCE_FORCE_TO_RAM
    if(memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        shader->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
#endif

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &shader->vertexAttributes);
    kzsErrorForward(result);

    it = kzcDynamicArrayGetIterator(attributeList);
    result = kzcMemoryAllocArray(memoryManager, shader->vertexAttributeIndices, kzcDynamicArrayGetSize(attributeList), "ShaderAttributeIndices");
    kzsErrorForward(result);
    i = 0;
    while (kzcDynamicArrayIterate(it))
    {
        kzString attributeName = (kzString)kzcDynamicArrayIteratorGetValue(it);
        shader->vertexAttributeIndices[i] = i;
        result = kzcHashMapPut(shader->vertexAttributes, attributeName, &shader->vertexAttributeIndices[i]);
        kzsErrorForward(result);
        ++i;
    }

    *out_shader = shader;
    kzsSuccess();
}

kzsError kzcShaderSourceCreate(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                               kzString vertexShader, kzString fragmentShader, const struct KzcDynamicArray* attributeList, 
                               kzBool* out_createSuccess, struct KzcShader** out_shader)
{
    kzsError result;
    struct KzcShader* shader;
    struct KzcMemoryManager* memoryManager;
    kzBool createSuccess = KZ_TRUE;
    kzUint measurementStartTime = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(resourceManager));
    memoryManager = kzcMemoryGetManager(resourceManager);

    result = kzcShaderCreate_internal(resourceManager, memoryType, KZC_SHADER_TYPE_ONLINE, attributeList, &shader);
    kzsErrorForward(result);

    /* Copy vertex and fragment shader code data. */
    result = kzcStringCopy(memoryManager, vertexShader, &shader->shaderData.code.vertex);
    kzsErrorForward(result);
    result = kzcStringCopy(memoryManager, fragmentShader, &shader->shaderData.code.fragment);
    kzsErrorForward(result);
    
    if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY || shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        result = kzcShaderDeployToGPU_internal(shader, &createSuccess);
        kzsErrorForward(result);
    }

    result = kzcResourceManagerAddShader(resourceManager, shader);
    kzsErrorForward(result);

    /* If the memory is intended to be GPU only, delete the RAM memory. */
    if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        /* Temporarily put shader memory type to GPU and RAM, as it's instantly removed from RAM. */
        shader->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
        result = kzcShaderFreeRAMMemory(shader);
        kzsErrorForward(result);
    }
    
    kzcResourceManagerAddResourceCumulativeTime_private(resourceManager, measurementStartTime, KZC_RESOURCE_MEASUREMENT_SHADER);

    if(out_createSuccess != KZ_NULL)
    {
        *out_createSuccess = createSuccess;
    }
    *out_shader = shader;
    kzsSuccess();
}

kzsError kzcShaderBinaryCombinedCreate(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                                       const void* binaryData, kzUint binarySize, const struct KzcDynamicArray* attributeList, 
                                       kzBool* out_createSuccess, struct KzcShader** out_shader)
{
    kzsError result;
    struct KzcShader* shader;
    struct KzcMemoryManager* memoryManager;
    kzBool createSuccess = KZ_TRUE;
    kzUint measurementStartTime = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(resourceManager));
    memoryManager = kzcMemoryGetManager(resourceManager);

    result = kzcShaderCreate_internal(resourceManager, memoryType, KZC_SHADER_TYPE_BINARY_COMBINED, attributeList, &shader);
    kzsErrorForward(result);

    /* Copy and assign the binary variables. */
    result = kzcMemoryAllocPointer(memoryManager, &shader->shaderData.binaryCombined.data, binarySize, "binaryShaderData");
    kzsErrorForward(result);
    kzsMemcpy(shader->shaderData.binaryCombined.data, binaryData, binarySize);
    shader->shaderData.binaryCombined.size = binarySize;

    if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY || shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        result = kzcShaderDeployToGPU_internal(shader, &createSuccess);
        kzsErrorForward(result);
    }

    result = kzcResourceManagerAddShader(resourceManager, shader);
    kzsErrorForward(result);

    /* If the memory is intended to be GPU only, delete the RAM memory. */
    if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        /* Temporarily put shader memory type to GPU and RAM, as it's instantly removed from RAM. */
        shader->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
        result = kzcShaderFreeRAMMemory(shader);
        kzsErrorForward(result);
    }

    kzcResourceManagerAddResourceCumulativeTime_private(resourceManager, measurementStartTime, KZC_RESOURCE_MEASUREMENT_SHADER);

    if(out_createSuccess != KZ_NULL)
    {
        *out_createSuccess = createSuccess;
    }

    *out_shader = shader;
    kzsSuccess();
}

kzsError kzcShaderBinarySeparatedCreate(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                                        const void* vertexData, kzUint vertexDataSize, const void* fragmentData, kzUint fragmentDataSize, 
                                        const struct KzcDynamicArray* attributeList, kzBool* out_createSuccess, struct KzcShader** out_shader)
{
    kzsError result;
    struct KzcShader* shader;
    struct KzcMemoryManager* memoryManager;
    kzBool createSuccess = KZ_TRUE;
    kzUint measurementStartTime = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(resourceManager));
    memoryManager = kzcMemoryGetManager(resourceManager);

    result = kzcShaderCreate_internal(resourceManager, memoryType, KZC_SHADER_TYPE_BINARY_SEPARATED, attributeList, &shader);
    kzsErrorForward(result);

    /* Copy and assign the binary variables. */
    result = kzcMemoryAllocPointer(memoryManager, &shader->shaderData.binarySeparated.vertexData, vertexDataSize, "binaryVertexData");
    kzsErrorForward(result);
    result = kzcMemoryAllocPointer(memoryManager, &shader->shaderData.binarySeparated.fragmentData, fragmentDataSize, "binaryFragmentData");
    kzsErrorForward(result);
    kzsMemcpy(shader->shaderData.binarySeparated.vertexData, vertexData, vertexDataSize);
    shader->shaderData.binarySeparated.vertexDataSize = vertexDataSize;
    kzsMemcpy(shader->shaderData.binarySeparated.fragmentData, fragmentData, fragmentDataSize);
    shader->shaderData.binarySeparated.fragmentDataSize = fragmentDataSize;

    if(memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY || memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        result = kzcShaderDeployToGPU_internal(shader, &createSuccess);
        kzsErrorForward(result);
    }

    result = kzcResourceManagerAddShader(resourceManager, shader);
    kzsErrorForward(result);

    /* If the memory is intended to be GPU only, delete the RAM memory. */
    if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        /* Temporarily put shader memory type to GPU and RAM, as it's instantly removed from RAM. */
        shader->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
        result = kzcShaderFreeRAMMemory(shader);
        kzsErrorForward(result);
    }

    kzcResourceManagerAddResourceCumulativeTime_private(resourceManager, measurementStartTime, KZC_RESOURCE_MEASUREMENT_SHADER);

    if(out_createSuccess != KZ_NULL)
    {
        *out_createSuccess = createSuccess;
    }
    *out_shader = shader;
    kzsSuccess();
}

kzsError kzcShaderCreateFromFile(struct KzcResourceManager* resourceManager, enum KzcResourceMemoryType memoryType, 
                                 kzString vertexShaderPath, kzString fragmentShaderPath, kzBool* out_createSuccess,
                                 struct KzcShader** out_shader)
{
    kzsError result;
    struct KzcShader* shader;
    struct KzcMemoryManager* memoryManager;
    kzMutableString vertexShader;
    kzMutableString fragmentShader;
    struct KzcDynamicArray* attributeList;

    kzsAssert(kzcIsValidPointer(resourceManager));
    memoryManager = kzcMemoryGetManager(resourceManager);

    result = kzcFileReadTextFile(memoryManager, vertexShaderPath, &vertexShader);
    kzsErrorForward(result);

    result = kzcFileReadTextFile(memoryManager, fragmentShaderPath, &fragmentShader);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &attributeList);
    kzsErrorForward(result);

    {
        kzUint attributeStartIndex = 0;
        kzUint index;
        kzMutableString statement;

        result = kzcStringAllocate(memoryManager, kzcStringLength(vertexShader), &statement);
        kzsErrorForward(result);

        while(kzcStringFindNextSubstring(vertexShader, "attribute", attributeStartIndex, &index))
        {
            kzUint statementEnd;
            if(kzcStringFindNextSubstring(vertexShader, ";", index, &statementEnd))
            {
                kzUint j;
                
                kzUint stringCount;
                kzMutableString* splitStrings;

                attributeStartIndex = statementEnd + 1;

                kzsMemcpy(statement, &vertexShader[index], statementEnd - index);
                statement[statementEnd - index] = KZC_STRING_TERMINATION_SYMBOL;

                for(j = 0; j < statementEnd - index; ++j)
                {
                    if(statement[j] == '\n' || statement[j] == '\t' || statement[j] == '\r')
                    {
                        statement[j] = ' ';
                    }
                }

                result = kzcStringSplit(memoryManager, statement, " ", &stringCount, &splitStrings);
                kzsErrorForward(result);
                index = 0;
                
                for(j = 0; j < stringCount; ++j)
                {
                    if (kzcStringIsEqual(splitStrings[j], "vec3") ||
                        kzcStringIsEqual(splitStrings[j], "vec4") ||
                        kzcStringIsEqual(splitStrings[j], "vec2") ||
                        kzcStringIsEqual(splitStrings[j], "float") ||
                        kzcStringIsEqual(splitStrings[j], "ivec4") ||
                        kzcStringIsEqual(splitStrings[j], "ivec3") ||
                        kzcStringIsEqual(splitStrings[j], "ivec2"))
                    {
                        index = j;
                        break;
                    }
                }

                /* Find the attribute name. */
                {
                    kzMutableString parsedAttributeString;
                    result = kzcStringReplace(memoryManager, splitStrings[index + 1], ";", "", &parsedAttributeString);
                    kzsErrorForward(result);

                    result = kzcDynamicArrayAdd(attributeList, parsedAttributeString);
                    kzsErrorForward(result);
                }

                for(j = 0; j < stringCount; ++j)
                {
                    result = kzcStringDelete(splitStrings[j]);
                    kzsErrorForward(result);
                }
                result = kzcMemoryFreeArray(splitStrings);
                kzsErrorForward(result);

            }
        }
        result = kzcStringDelete(statement);
        kzsErrorForward(result);
    }
    
    result = kzcShaderSourceCreate(resourceManager, memoryType, vertexShader, fragmentShader, attributeList,
        out_createSuccess, &shader);
    kzsErrorForward(result);

    /* Delete attributes. */
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(attributeList);
        while(kzcDynamicArrayIterate(it))
        {
            kzMutableString attributeString = (kzMutableString)kzcDynamicArrayIteratorGetValue(it);
            
            result = kzcStringDelete(attributeString);
            kzsErrorForward(result);
        }
        result = kzcDynamicArrayDelete(attributeList);
        kzsErrorForward(result);
    }

    result = kzcStringDelete(fragmentShader);
    kzsErrorForward(result);

    result = kzcStringDelete(vertexShader);
    kzsErrorForward(result);

    *out_shader = shader;
    kzsSuccess();
}

kzsError kzcShaderDelete(struct KzcShader* shader)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(shader));

    kzcShaderFreeGPUMemory(shader);

    result = kzcShaderFreeRAMMemory(shader);
    kzsErrorForward(result);

    result = kzcHashMapDelete(shader->vertexAttributes);
    kzsErrorForward(result);
    result = kzcMemoryFreeArray(shader->vertexAttributeIndices);
    kzsErrorForward(result);

    result = kzcResourceManagerRemoveShader(shader->resourceManager, shader);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(shader);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcShaderFreeGPUMemory(struct KzcShader* shader)
{
    kzsAssert(kzcIsValidPointer(shader));
    
    if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
       shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        if(shader->programHandle != 0)
        {
            kzsGlDetachShader(shader->programHandle, shader->vertexShaderHandle);
            kzsGlDetachShader(shader->programHandle, shader->fragmentShaderHandle);
            kzsGlDeleteProgram(shader->programHandle);
        }

        if(shader->vertexShaderHandle != 0)
        {
            kzsGlDeleteShader(shader->vertexShaderHandle);
        }
        if(shader->fragmentShaderHandle != 0)
        {
            kzsGlDeleteShader(shader->fragmentShaderHandle);
        }

        if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            shader->memoryType = KZC_RESOURCE_MEMORY_TYPE_NONE;
        }
        else
        {
            shader->memoryType = KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY;
        }
    }

}

kzsError kzcShaderFreeRAMMemory(struct KzcShader* shader)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(shader));

    if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
        shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
    {
        /* Delete the data. */
        switch(shader->type)
        {
            /* Online compiled shader contains the vertex & fragment shader code data. */
            case KZC_SHADER_TYPE_ONLINE:
            {
                result = kzcMemoryFreePointer(shader->shaderData.code.vertex);
                kzsErrorForward(result);
                result = kzcMemoryFreePointer(shader->shaderData.code.fragment);
                kzsErrorForward(result);
                break;
            }
            case KZC_SHADER_TYPE_BINARY_SEPARATED:
            {
                result = kzcMemoryFreePointer(shader->shaderData.binarySeparated.fragmentData);
                kzsErrorForward(result);
                result = kzcMemoryFreePointer(shader->shaderData.binarySeparated.vertexData);
                kzsErrorForward(result);
                break;
            }
            /* Offline compiled shader contains the binary data. */
            case KZC_SHADER_TYPE_BINARY_COMBINED:
            {
                result = kzcMemoryFreePointer(shader->shaderData.binaryCombined.data);
                kzsErrorForward(result);
                break;
            }
            default:
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown shader type when deleting RAM data!");
            }
        }

        if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
        {
            shader->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY;
        }
        else if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            shader->memoryType = KZC_RESOURCE_MEMORY_TYPE_NONE;
        }
    }

    kzsSuccess();
}

kzsError kzcShaderRestoreGPU(struct KzcShader* shader)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(shader));

    if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
    {
        kzBool deploySuccess;
        
        result = kzcShaderDeployToGPU_internal(shader, &deploySuccess);
        kzsErrorForward(result);

        if(!deploySuccess)
        {
            kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Shader failed to link/compile when restored to GPU!");
        }

        shader->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
    else if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "This shader is already contained in GPU and RAM!");
    }
    else if(shader->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "This shader is already contained in GPU!");
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Cannot restore shader that isn't contained in RAM memory!");
    }

    kzsSuccess();
}

/* TODO: Remove this function, as it exposes internals */
/* TODO: Do not remove this function as it allows using shaders with custom VBO's. */
struct KzcHashMap* kzcShaderGetAttributes(const struct KzcShader* shader)
{
    kzsAssert(kzcIsValidPointer(shader));

    return shader->vertexAttributes;
}

static kzsError kzcShaderCompile_internal(const struct KzcMemoryManager* memoryManager, kzString shaderCode, kzUint shaderType,
                                          kzUint* out_handle, kzBool* out_compileSuccess)
{
    kzsError result;
    kzUint shaderHandle;
    kzBool shaderCompiled;
    
    *out_compileSuccess = KZ_TRUE;
    
    /* Create shader object. */
    shaderHandle = kzsGlCreateShader(shaderType);
    kzsErrorTest(shaderHandle != 0, KZC_ERROR_OPENGL_ERROR, "Unable to create shader object!");

    /* Setup shader code for compiling. */
    kzsGlShaderSource(shaderHandle, 1, &shaderCode, KZ_NULL);
    /* Compile shader object. */
    kzsGlCompileShader(shaderHandle);
    {
        kzInt compileStatus = 1;
        kzsGlGetShaderiv(shaderHandle, KZS_GL_COMPILE_STATUS, &compileStatus);
        if(compileStatus == 0)
        {
            shaderCompiled = KZ_FALSE;
        }
        else
        {
            shaderCompiled = KZ_TRUE;
        }
    }

    /* Shader did not compile, print the error log. */
    if(!shaderCompiled)
    {
        kzInt infoLength = 0;
        kzsGlGetShaderiv(shaderHandle, KZS_GL_INFO_LOG_LENGTH, &infoLength);
        if(infoLength > 1)
        {
            kzMutableString infoLog;
            result = kzcMemoryAllocPointer(memoryManager, &infoLog, (kzUint)infoLength, "shaderLog");
            kzsErrorForward(result);
            kzsGlGetShaderInfoLog(shaderHandle, infoLength, KZ_NULL, infoLog);

            kzsLog(KZS_LOG_LEVEL_WARNING, infoLog);

            result = kzcMemoryFreePointer(infoLog);
            kzsErrorForward(result);
        }
        if(shaderType == KZS_GL_FRAGMENT_SHADER)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Error in fragment shader!");
            kzsLog(KZS_LOG_LEVEL_INFO, shaderCode);
        }
        else if(shaderType == KZS_GL_VERTEX_SHADER)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Error in vertex shader!");
            kzsLog(KZS_LOG_LEVEL_INFO, shaderCode);
        }

        kzsGlDeleteShader(shaderHandle);
        *out_compileSuccess = KZ_FALSE;
    }

    *out_handle = shaderHandle;
    kzsSuccess();
}

#if KZ_OPENGL_VERSION == KZ_OPENGL_2_1
static kzsError kzcShaderES2GLSLtoGL21GLSL_internal(const struct KzcMemoryManager* memoryManager, kzMutableString* out_vertexShader, kzMutableString* out_fragmentShader)
{
    kzsError result;
    kzMutableString vertexShader;
    kzMutableString fragmentShader;

    /* TODO: This solution could be faster. By padding ' ' where match is found this could be done in place without requiring any memory copying. */
    {
        kzUint i;
        #define PRECISION_STRING_COUNT 6
        kzMutableString vsStrings[PRECISION_STRING_COUNT + 1];
        kzMutableString fsStrings[PRECISION_STRING_COUNT + 1];
        kzString precisionStrings[PRECISION_STRING_COUNT] = 
        {
            "precision lowp float;",
            "precision mediump float;",
            "precision highp float;",
            "mediump",
            "lowp",
            "highp"
        };

        result = kzcStringCopy(memoryManager, *out_fragmentShader, &fsStrings[0]);
        kzsErrorForward(result);
        result = kzcStringCopy(memoryManager, *out_vertexShader, &vsStrings[0]);
        kzsErrorForward(result);

        for(i = 0; i < PRECISION_STRING_COUNT; ++i)
        {
            result = kzcStringReplace(memoryManager, fsStrings[i], precisionStrings[i], "", &fsStrings[i + 1]);
            kzsErrorForward(result);
            result = kzcStringReplace(memoryManager, vsStrings[i], precisionStrings[i], "", &vsStrings[i + 1]);
            kzsErrorForward(result);
        }

        result = kzcStringCopy(memoryManager, fsStrings[PRECISION_STRING_COUNT], &fragmentShader);
        kzsErrorForward(result);
        result = kzcStringCopy(memoryManager, vsStrings[PRECISION_STRING_COUNT], &vertexShader);
        kzsErrorForward(result);

        for(i = 0; i < PRECISION_STRING_COUNT + 1; ++i)
        {
            result = kzcStringDelete(vsStrings[i]);
            kzsErrorForward(result);
            result = kzcStringDelete(fsStrings[i]);
            kzsErrorForward(result);
        }
    }

    result = kzcStringDelete(*out_vertexShader);
    kzsErrorForward(result);
    result = kzcStringDelete(*out_fragmentShader);
    kzsErrorForward(result);

    *out_vertexShader = vertexShader;
    *out_fragmentShader = fragmentShader;
    kzsSuccess();
}
#endif

static kzsError kzcShaderDeployToGPU_internal(struct KzcShader* shader, kzBool* out_deploySuccess)
{
#ifdef COMPILE_SHADERS

    kzsError result;
    kzBool shaderLinked;
    struct KzcHashMapIterator it;
    kzBool deploySuccess = KZ_TRUE;

    shader->vertexShaderHandle = 0;
    shader->fragmentShaderHandle = 0;
    shader->programHandle = 0;

    /* Online compiled shader, vertex and fragment shaders need to be compiled separately (and later linked to shader program). */
    if(shader->type == KZC_SHADER_TYPE_ONLINE)
    {
#if KZ_OPENGL_VERSION == KZ_OPENGL_2_1
        result = kzcShaderES2GLSLtoGL21GLSL_internal(kzcMemoryGetManager(shader), &shader->shaderData.code.vertex, &shader->shaderData.code.fragment);
        kzsErrorForward(result);;
#endif

        result = kzcShaderCompile_internal(kzcMemoryGetManager(shader), shader->shaderData.code.vertex,
            KZS_GL_VERTEX_SHADER, &shader->vertexShaderHandle, &deploySuccess);
        kzsErrorForward(result);

        if(deploySuccess)
        {
            result = kzcShaderCompile_internal(kzcMemoryGetManager(shader), shader->shaderData.code.fragment,
                KZS_GL_FRAGMENT_SHADER, &shader->fragmentShaderHandle, &deploySuccess);
            kzsErrorForward(result);
        }
    }
    /* Next follows the binary shaders. Binary shaders rely heavily on vendor, so these might not be 100% correct. */
    /* Combined binary. */
    else if(shader->type == KZC_SHADER_TYPE_BINARY_COMBINED)
    {
        kzUint shaders[2];

        shader->vertexShaderHandle = kzsGlCreateShader(KZS_GL_VERTEX_SHADER);
        kzsErrorTest(shader->vertexShaderHandle != 0, KZC_ERROR_OPENGL_ERROR, "Unable to create shader object!");

        shader->fragmentShaderHandle = kzsGlCreateShader(KZS_GL_FRAGMENT_SHADER);
        kzsErrorTest(shader->fragmentShaderHandle != 0, KZC_ERROR_OPENGL_ERROR, "Unable to create shader object!");

        shaders[0] = shader->vertexShaderHandle;
        shaders[1] = shader->fragmentShaderHandle;

        kzsGlShaderBinary(2, shaders, (kzUint)KZS_OPENGL_BINARY_FORMAT, shader->shaderData.binaryCombined.data, 
            (kzInt)shader->shaderData.binaryCombined.size);
    }
    /* Separated binary. */
    else if(shader->type == KZC_SHADER_TYPE_BINARY_SEPARATED)
    {
        /* Create shader object. */
        shader->vertexShaderHandle = kzsGlCreateShader(KZS_GL_VERTEX_SHADER);
        kzsErrorTest(shader->vertexShaderHandle != 0, KZC_ERROR_OPENGL_ERROR, "Unable to create shader object!");
        kzsGlShaderBinary(1, &shader->vertexShaderHandle, (kzUint)KZS_OPENGL_BINARY_FORMAT, shader->shaderData.binarySeparated.vertexData, 
            (kzInt)shader->shaderData.binarySeparated.vertexDataSize);
        shader->fragmentShaderHandle = kzsGlCreateShader(KZS_GL_FRAGMENT_SHADER);
        kzsErrorTest(shader->fragmentShaderHandle != 0, KZC_ERROR_OPENGL_ERROR, "Unable to create shader object!");
        kzsGlShaderBinary(1, &shader->fragmentShaderHandle, (kzUint)KZS_OPENGL_BINARY_FORMAT, shader->shaderData.binarySeparated.fragmentData, 
            (kzInt)shader->shaderData.binarySeparated.fragmentDataSize);
    }

    /* Create shader program. */
    if(deploySuccess)
    {
        shader->programHandle = kzsGlCreateProgram();
        kzsErrorTest(shader->programHandle != 0, KZC_ERROR_OPENGL_ERROR, "Unable to create shader program!");

        /* See that vertex and fragment shader contain valid handles. */
        kzsAssert(shader->vertexShaderHandle != 0 && shader->fragmentShaderHandle != 0);

        /* Attach vertex and fragment shader objects to shader program. */
        {
            kzsGlAttachShader(shader->programHandle, shader->vertexShaderHandle);
            kzsGlAttachShader(shader->programHandle, shader->fragmentShaderHandle);
        }

        it = kzcHashMapGetIterator(shader->vertexAttributes);
        /* Bind vertex attributes to shader. */
        while(kzcHashMapIterate(it))
        {
            kzString attributeName = (kzString)kzcHashMapIteratorGetKey(it);
            kzUint* attributeIndex = (kzUint*)kzcHashMapIteratorGetValue(it);
            
            kzsAssert(attributeIndex != KZ_NULL);

            kzsGlBindAttribLocation(shader->programHandle, *attributeIndex, attributeName);
        }

        /* Link shader objects to one shader program. */
        kzsGlLinkProgram(shader->programHandle);

        /* See if linking succeeded. */
        {
            kzInt linkStatus;
            kzsGlGetProgramiv(shader->programHandle, KZS_GL_LINK_STATUS, &linkStatus);
            if(linkStatus == 0)
            {
                shaderLinked = KZ_FALSE;
            }
            else
            {
                shaderLinked = KZ_TRUE;
            }
        }
        /* Print error log from linking. */
        if(!shaderLinked)
        {
            kzInt infoLength;
            kzsGlGetProgramiv(shader->programHandle, KZS_GL_INFO_LOG_LENGTH, &infoLength);
            if(infoLength > 1)
            {
                kzMutableString infoLog;
                result = kzcMemoryAllocPointer(kzcMemoryGetManager(shader), &infoLog, (kzUint)infoLength, "programLog");
                kzsErrorForward(result);
                kzsGlGetProgramInfoLog(shader->programHandle, infoLength, KZ_NULL, infoLog);

                kzsLog(KZS_LOG_LEVEL_WARNING, infoLog);

                result = kzcMemoryFreePointer(infoLog);
                kzsErrorForward(result);
            }
            kzsGlDeleteProgram(shader->programHandle);
            kzsErrorThrow(KZC_ERROR_OPENGL_ERROR, "Shader failed to link!");
        }
    }

    *out_deploySuccess = deploySuccess;
#endif
    kzsSuccess();
}

kzsError kzcShaderApply(const struct KzcShader* shader, struct KzcRenderer* renderer)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(shader));

    /* Set active shader for renderer. */
    result = kzcRendererSetActiveShaderHandle(renderer, shader->programHandle);
    kzsErrorForward(result);

    /* Set vertex attribute mappings for renderer. */
    kzcRendererSetActiveVertexAttributeMap(renderer, shader->vertexAttributes);

    kzsSuccess();
}

enum KzcResourceMemoryType kzcShaderGetMemoryType(const struct KzcShader* shader)
{
    kzsAssert(kzcIsValidPointer(shader));
    return shader->memoryType;
}

#else

#include <system/kzs_types.h>

KZ_EMPTY_SOURCE_FILE;

#endif
