/**
 * \file
 * Vertex Buffer resource
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_resource_vertex_buffer.h"

#include "kzc_resource_vertex.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_byte_buffer.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/string/kzc_string.h>
#include <core/util/math/kzc_vector3.h>
#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/debug/kzc_log.h>
#include <core/kzc_error_codes.h>

#include <system/wrappers/kzs_opengl_base.h>
#include <system/wrappers/kzs_opengl.h>
#include <system/wrappers/kzs_memory.h>
#include <system/time/kzs_tick.h>


/**< Global variable storing half float extension support. */ /*lint -e{956} Ignored warning for not multi thread safeness. */
static kzInt g_vertexHalfFloatExtensionSupported = -1;


#define KZC_VERTEX_BUFFER_MAX_OVERRIDE_SEMANTICS 8


/** Internal structure for Vertex attribute. */
struct KzcVertexAttributeInternal
{
    kzMutableString name;                       /**< Name of vertex attribute. Must be unique within vertex buffer. */
    enum KzcVertexAttributeSemantic semantic;   /**< Semantic for vertex attribute. */
    kzUint padding;                             /**< Padding for vertex attribute. */

    kzUint dimension;                           /**< Vertex attribute dimension. */
    enum KzcVertexAttributeDataType dataType;   /**< Vertex attribute data type. */
};

/* TODO: Create structure for single attribute mapping hash map, nested maps are confusing */
struct KzcVertexBuffer
{
    struct KzcResourceManager* resourceManager; /** Resource manager that contains this resource. */
    
    kzUint bufferHandle;                       /**< Vertex buffer GPU handle. */
    kzBool isVBO;                              /**< Is VBO. KZ_FALSE is for vertex array. */

    void* data;                                /**< Vertex buffer data. */
    kzUint vertexCount;                        /**< Vertex count for this vertex buffer. */

    struct KzcDynamicArray* vertexAttributes;  /**< List of vertex attributes (KzcVertexAttributeInternal). */
    kzUint vertexSize;                         /**< Size of one vertex, that is, all attributes summed. */

    struct KzcHashMap* currentAttributeMap;    /**< Hashmap<string(shaderAttributeName), string(vertexAttributeName)> */
    struct KzcHashMap* attributeMappings;      /**< Hashmap<void*(material type), hashmap <string(shaderAttribute), string(vertexAttribute)> */

    enum KzcVertexBufferUsage usage;           /**< Usage of internal vertex buffer, either static or dynamic. */
    enum KzcResourceMemoryType memoryType;     /**< Memory type of resource, GPU or GPU+RAM. */

    struct KzcVertexAttributeInternal overrideSemantics[KZC_VERTEX_BUFFER_MAX_OVERRIDE_SEMANTICS]; /**< Override semantics. */
    void* overrideSemanticDatas[KZC_VERTEX_BUFFER_MAX_OVERRIDE_SEMANTICS]; /**< Override semantic datas. */
    kzUint overrideSemanticCount; /**< Override semantic count. */

    kzBool invalidAttributesLogged;            /**< Debug variable to check if invalid attributes have been logged. */
};


/** Deploys vertex buffer to GPU. */
static void kzcVertexBufferDeployToGPU_internal(struct KzcVertexBuffer* vertexBuffer);
/** Gets override semantic from vertex buffer. */
static kzBool kzcVertexBufferGetOverrideSemantic_internal(const struct KzcVertexBuffer* vertexBuffer, enum KzcVertexAttributeSemantic semantic,
                                                          void** out_data, kzUint* out_size);


#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
static kzUint kzcVertexAttributeDataTypeGetGLConstant_internal(enum KzcVertexAttributeDataType dataType)
{
    kzUint glDataType = KZS_GL_FLOAT;
    switch(dataType)
    {
        case KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT16:
        {
#ifdef KZS_GL_HALF_FLOAT
            glDataType = KZS_GL_HALF_FLOAT;
#else
            kzsAssertText(KZ_FALSE, "Half float not supported by GL implementation");
#endif
            break;
        }
        case KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32: glDataType = KZS_GL_FLOAT; break;
        case KZC_VERTEX_ATTRIBUTE_DATA_TYPE_S16: glDataType = KZS_GL_SHORT; break;
        case KZC_VERTEX_ATTRIBUTE_DATA_TYPE_U16: glDataType = KZS_GL_UNSIGNED_SHORT; break;
        case KZC_VERTEX_ATTRIBUTE_DATA_TYPE_S8: glDataType = KZS_GL_BYTE; break;
        case KZC_VERTEX_ATTRIBUTE_DATA_TYPE_U8: glDataType = KZS_GL_UNSIGNED_BYTE; break;
        case KZC_VERTEX_ATTRIBUTE_DATA_TYPE_S32:
        case KZC_VERTEX_ATTRIBUTE_DATA_TYPE_U32:
        default:
        {
            kzsAssertText(KZ_FALSE, "Invalid vertex attribute data type");
        }
    }
    return glDataType;
}
#endif

kzsError kzcVertexBufferCreate(struct KzcResourceManager* resourceManager, const enum KzcResourceMemoryType memoryType,
                               kzBool useVertexBufferObjects, enum KzcVertexBufferUsage usage, struct KzcVertexList* vertexList, struct KzcVertexBuffer** out_vertexBuffer)
{
    kzsError result;
    kzUint j;
    struct KzcMemoryManager* memoryManager;
    struct KzcVertexBuffer* vertexBuffer;
    kzUint vertexCount = kzcVertexListGetNumDataElements(vertexList);
    kzUint measurementStartTime = kzsTimeGetCurrentTimestamp();
    void* newData;

    kzsAssert(kzcIsValidPointer(vertexList));

    memoryManager = kzcMemoryGetManager(resourceManager);

    result = kzcVertexListConvertHalfFloatToFloat(vertexList, &newData);
    kzsErrorForward(result);
    
    result = kzcMemoryAllocVariable(memoryManager, vertexBuffer, "Vertex buffer");
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &vertexBuffer->vertexAttributes);
    kzsErrorForward(result);

    /* Attribute mappings from material (shader) to hash map. */
    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &vertexBuffer->attributeMappings);
    kzsErrorForward(result);
 
    vertexBuffer->vertexSize = 0;
    vertexBuffer->vertexCount = 0;
    vertexBuffer->invalidAttributesLogged = KZ_FALSE;
    vertexBuffer->currentAttributeMap = KZ_NULL;
    vertexBuffer->isVBO = useVertexBufferObjects;
    vertexBuffer->usage = usage;
    vertexBuffer->overrideSemanticCount = 0;
    /* TODO: Implement usage properly. */
    if(memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        vertexBuffer->usage = KZC_VERTEX_BUFFER_USAGE_DYNAMIC;
        vertexBuffer->isVBO = KZ_FALSE;
    }

    /* Copy the vertex attribute data to vertex buffer. */
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(kzcVertexListGetVertexAttributes(vertexList));
        while (kzcDynamicArrayIterate(it))
        {
            struct KzcVertexAttribute* attribute = (struct KzcVertexAttribute*)kzcDynamicArrayIteratorGetValue(it);
            struct KzcVertexAttributeInternal* newAttribute;
            kzUint attributeDimension;
            kzUint attributeElementSize;
            
            /* Set internal attribute. */
            result = kzcMemoryAllocVariable(memoryManager, newAttribute, "Vertex attribute");
            kzsErrorForward(result);
            result = kzcDynamicArrayAdd(vertexBuffer->vertexAttributes, newAttribute);
            kzsErrorForward(result);
            
            attributeDimension = kzcVertexAttributeGetDimension(attribute);
            attributeElementSize = kzcVertexAttributeGetElementSize(attribute);

            newAttribute->padding = vertexBuffer->vertexSize;
            newAttribute->semantic = kzcVertexAttributeGetSemantic(attribute);
            newAttribute->dimension = kzcVertexAttributeGetDimension(attribute);
            newAttribute->dataType = kzcVertexAttributeGetDataType(attribute);
            {
                kzMutableString attributeName = kzcVertexAttributeGetName(attribute);

                result = kzcStringCopy(memoryManager, attributeName, &newAttribute->name);
                kzsErrorForward(result);
            }

            /* Add the vertex attribute size to data size. */
            vertexBuffer->vertexSize += attributeDimension * attributeElementSize;
        }
    }

    vertexBuffer->vertexCount = vertexCount;

    /* Allocate memory for data. */
    result = kzcMemoryAllocPointer(memoryManager, &vertexBuffer->data, 
        vertexBuffer->vertexSize * vertexBuffer->vertexCount, "Vertex attribute data");
    kzsErrorForward(result);

    /* Arrange vertex data if it's list of arrays. */
    if(vertexBuffer->vertexCount > 0)
    {
        if(kzcVertexListGetFormat(vertexList) == KZC_VERTEX_LIST_FORMAT_LIST_OF_ARRAYS)
        {
            /* Rearrange the vertex data. */
            {
                kzByte* originalData = (kzByte*)kzcVertexListGetData(vertexList);
                struct KzcDynamicArrayIterator it;
                struct KzcDynamicArrayIterator attributeIterator;

                kzsAssert(kzcIsValidPointer(originalData));

                it = kzcDynamicArrayGetIterator(kzcVertexListGetVertexAttributes(vertexList));
                attributeIterator = kzcDynamicArrayGetIterator(vertexBuffer->vertexAttributes);

                /* Copy attributes to data struct. */
                while (kzcDynamicArrayIterate(it) && kzcDynamicArrayIterate(attributeIterator))
                {
                    struct KzcVertexAttribute* attribute = (struct KzcVertexAttribute*)kzcDynamicArrayIteratorGetValue(it);
                    struct KzcVertexAttributeInternal* attributeInternal = (struct KzcVertexAttributeInternal*)kzcDynamicArrayIteratorGetValue(attributeIterator);
                    kzUint attributeDimension = kzcVertexAttributeGetDimension(attribute);
                    kzUint attributeElementSize = kzcVertexAttributeGetElementSize(attribute);
                    kzByte* vertexBufferData = (kzU8*)vertexBuffer->data + attributeInternal->padding;

                    /*kzcByteBufferSetOffset(&byteBuffer, attributeInternal->padding);*/

                    /** Arrange attributes to optimal slot. */
                    for(j = 0; j < vertexBuffer->vertexCount; ++j)
                    {
                        /* Write vectors to vertex buffer data. */
                        kzsMemcpy(vertexBufferData, originalData, attributeElementSize * attributeDimension);
                        vertexBufferData += attributeDimension * attributeElementSize;
                        originalData += attributeDimension * attributeElementSize;
                        /* Skip the gap amount of bytes unless it's last element. */
                        if(j < vertexBuffer->vertexCount - 1)
                        {
                            vertexBufferData += vertexBuffer->vertexSize - attributeDimension * attributeElementSize;
                        }
                    }
                }
            }
        }
        /* Otherwise copy the data to vertex buffer. */
        else
        {
            kzsMemcpy(vertexBuffer->data, kzcVertexListGetData(vertexList), vertexBuffer->vertexSize * vertexBuffer->vertexCount);
        }
    }

    if(newData != KZ_NULL)
    {
        result = kzcMemoryFreePointer(newData);
        kzsErrorForward(result);
    }

    /* Save the initialized data to the buffer. */
    if(vertexBuffer->isVBO)
    {
        vertexBuffer->memoryType = memoryType;
    }
    else
    {
        vertexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
#ifdef KZC_RESOURCE_FORCE_TO_RAM
    if(memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        vertexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
#endif
    vertexBuffer->resourceManager = resourceManager;

    /* Deploys the vertex buffer to GPU memory. */
    if(vertexBuffer->isVBO && vertexBuffer->vertexCount > 0)
    {
        if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM || vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            kzcVertexBufferDeployToGPU_internal(vertexBuffer);
        }
    }

    result = kzcResourceManagerAddVertexBuffer(resourceManager, vertexBuffer);
    kzsErrorForward(result);

    /* If the memory is intended to be GPU only, delete the RAM memory. */
    if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        /* Temporarily put vertex buffer memory type to GPU + RAM, as it's instantly removed from RAM where it
           is located now, too. */
        vertexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
        result = kzcVertexBufferFreeRAMMemory(vertexBuffer);
        kzsErrorForward(result);
    }

    kzcResourceManagerAddResourceCumulativeTime_private(resourceManager, measurementStartTime, KZC_RESOURCE_MEASUREMENT_VERTEX_BUFFER);

    *out_vertexBuffer = vertexBuffer;
    kzsSuccess();
}

kzsError kzcVertexBufferFree(struct KzcVertexBuffer* vertexBuffer)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    
    kzsAssert(kzcIsValidPointer(vertexBuffer));

    /* Free vertex buffer from both GPU and RAM */
    result = kzcVertexBufferFreeGPUMemory(vertexBuffer);
    kzsErrorForward(result);

    /* If it exists in RAM memory, remove from there. */
    result = kzcVertexBufferFreeRAMMemory(vertexBuffer);
    kzsErrorForward(result);

    /* Delete attribute mappings. */
    {
        struct KzcHashMapIterator attributeIterator = kzcHashMapGetIterator(vertexBuffer->attributeMappings);

        while(kzcHashMapIterate(attributeIterator))
        {
            struct KzcHashMap* attributeMap = (struct KzcHashMap*)kzcHashMapIteratorGetValue(attributeIterator);
            struct KzcHashMapIterator attributeMapIterator;

            attributeMapIterator = kzcHashMapGetIterator(attributeMap);
            while(kzcHashMapIterate(attributeMapIterator))
            {
                kzMutableString attributeName = (kzMutableString)kzcHashMapIteratorGetKey(attributeMapIterator);

                result = kzcStringDelete(attributeName);
                kzsErrorForward(result);
            }

            result = kzcHashMapDelete(attributeMap);
            kzsErrorForward(result);
        }

        result = kzcHashMapDelete(vertexBuffer->attributeMappings);
        kzsErrorForward(result);
    }

    /* Delete the vertex attributes from RAM. */
    it = kzcDynamicArrayGetIterator(vertexBuffer->vertexAttributes);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzcVertexAttributeInternal* vertexAttribute = (struct KzcVertexAttributeInternal*)kzcDynamicArrayIteratorGetValue(it);

        result = kzcStringDelete(vertexAttribute->name);
        kzsErrorForward(result);

        result = kzcMemoryFreeVariable(vertexAttribute);
        kzsErrorForward(result);
    }
    result = kzcDynamicArrayDelete(vertexBuffer->vertexAttributes);
    kzsErrorForward(result);
    result = kzcResourceManagerRemoveVertexBuffer(vertexBuffer->resourceManager, vertexBuffer);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(vertexBuffer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcVertexBufferFreeGPUMemory(struct KzcVertexBuffer* vertexBuffer)
{
    kzsAssert(kzcIsValidPointer(vertexBuffer));

    if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
        vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        /* Remove only the buffer from GPU. */
        if(vertexBuffer->isVBO)
        {
            kzsGlDeleteBuffers(1, &vertexBuffer->bufferHandle);
            vertexBuffer->bufferHandle = 0;
        }
        if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
        {
            vertexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_NONE;
        }
        else
        {
            vertexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY;
        }
    }

    kzsSuccess();
}

kzsError kzcVertexBufferFreeRAMMemory(struct KzcVertexBuffer* vertexBuffer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(vertexBuffer));

    if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM ||
        vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
    {
        /* Delete the data. */
        result = kzcMemoryFreePointer(vertexBuffer->data);
        kzsErrorForward(result);
        vertexBuffer->data = KZ_NULL;

        if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
        {
            vertexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY;
        }
        else if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
        {
            vertexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_NONE;
        }
    }

    kzsSuccess();
}

kzsError kzcVertexBufferCopy(const struct KzcVertexBuffer* sourceVertexBuffer, struct KzcVertexBuffer** out_targetVertexBuffer)
{
    kzsError result;
    struct KzcVertexBuffer* vertexBuffer;
    struct KzcVertexList* vertexList;
    struct KzcDynamicArrayIterator it;

    result = kzcVertexListCreate(kzcMemoryGetManager(sourceVertexBuffer), &vertexList);
    kzsErrorForward(result);

    it = kzcDynamicArrayGetIterator(sourceVertexBuffer->vertexAttributes);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcVertexAttributeInternal* internalVertexAttribute = (struct KzcVertexAttributeInternal*)kzcDynamicArrayIteratorGetValue(it);
        kzUint elementSize = sizeof(kzFloat);
        if(internalVertexAttribute->dataType == KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT16)
        {
            elementSize = 2;
        }
        result = kzcVertexListAddAttribute(vertexList, internalVertexAttribute->name, internalVertexAttribute->semantic,
            internalVertexAttribute->dataType, elementSize, internalVertexAttribute->dimension);
        kzsErrorForward(result);
    }

    result = kzcVertexListSetData(vertexList, sourceVertexBuffer->data, sourceVertexBuffer->vertexCount);
    kzsErrorForward(result);
    kzcVertexListSetFormat(vertexList, KZC_VERTEX_LIST_FORMAT_LIST_OF_ATTRIBUTES);

    result = kzcVertexBufferCreate(sourceVertexBuffer->resourceManager, sourceVertexBuffer->memoryType,
        sourceVertexBuffer->isVBO, sourceVertexBuffer->usage, vertexList, &vertexBuffer);
    kzsErrorForward(result);

    result = kzcVertexListDelete(vertexList);
    kzsErrorForward(result);

    *out_targetVertexBuffer = vertexBuffer;
    kzsSuccess();
}

static void kzcVertexBufferDeployToGPU_internal(struct KzcVertexBuffer* vertexBuffer)
{
    kzUint originalBuffer = 0;
    
    if(vertexBuffer->vertexCount >  0)
    {
        /* Store the original state. */
        kzsGlGetIntegerv(KZS_GL_ARRAY_BUFFER_BINDING, (kzInt*)&originalBuffer);
        /* Creates a new vertex buffer. */
        kzsGlGenBuffers(1, &vertexBuffer->bufferHandle);
        /* Use the vertex buffer. */
        kzsGlBindBuffer(KZS_GL_ARRAY_BUFFER, vertexBuffer->bufferHandle);
        /* Allocate memory for the buffer. */
        kzsGlBufferData(KZS_GL_ARRAY_BUFFER, (kzInt)(vertexBuffer->vertexCount * vertexBuffer->vertexSize),
            vertexBuffer->data, KZS_GL_STATIC_DRAW);
        /* Switch back to original state. */
        kzsGlBindBuffer(KZS_GL_ARRAY_BUFFER, originalBuffer);
    }
}

kzsError kzcVertexBufferRestoreGPU(struct KzcVertexBuffer* vertexBuffer)
{
    kzsAssert(kzcIsValidPointer(vertexBuffer));

    if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY)
    {
        kzcVertexBufferDeployToGPU_internal(vertexBuffer);
        vertexBuffer->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }
    else if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "This vertex buffer is already contained in GPU and RAM!");
    }
    else if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "This vertex buffer is already contained in GPU!");
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Cannot restore vertex buffer that isn't contained in RAM memory!");
    }

    kzsSuccess();
}

kzUint kzcVertexBufferGetHandle(const struct KzcVertexBuffer* vertexBuffer)
{
    kzsAssert(kzcIsValidPointer(vertexBuffer));
    return vertexBuffer->bufferHandle;
}

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
static void* kzcCreateVBOPointer_internal(kzUint offset)
{
    return (void*)offset;   /*lint !e923*/
}
#endif

struct KzcVertexAttributeKnownMapping
{
    kzString attributeName;
    enum KzcVertexAttributeSemantic semantic;
};

static const struct KzcVertexAttributeKnownMapping KZC_VERTEX_ATTRIBUTE_KNOWN_MAPPING[] =
{
    { "kzPosition", KZC_VERTEX_ATTRIBUTE_POSITION },
    { "kzNormal", KZC_VERTEX_ATTRIBUTE_NORMAL },
    { "kzTangent", KZC_VERTEX_ATTRIBUTE_TANGENT },
    { "kzColor0", KZC_VERTEX_ATTRIBUTE_COLOR },
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    { "kzColor1", KZC_VERTEX_ATTRIBUTE_COLOR },
    { "kzColor2", KZC_VERTEX_ATTRIBUTE_COLOR },
    { "kzColor3", KZC_VERTEX_ATTRIBUTE_COLOR },
#endif
    { "kzTextureCoordinate0", KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE },
    { "kzTextureCoordinate1", KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE },
    { "kzTextureCoordinate2", KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE },
    { "kzTextureCoordinate3", KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE },
    { "kzWeight", KZC_VERTEX_ATTRIBUTE_WEIGHT },
    { "kzMatrixIndices", KZC_VERTEX_ATTRIBUTE_MATRIX_INDEX },
    { KZ_NULL, KZC_VERTEX_ATTRIBUTE_POSITION }
};

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
#define KZC_VERTEX_ATTRIBUTE_MAX_KNOWN_MAPPINGS 13
#else
#define KZC_VERTEX_ATTRIBUTE_MAX_KNOWN_MAPPINGS 10
#endif

/** Internal function for mapping shader attribute name to specific vertex attribute semantic. */
static kzsError kzcVertexBufferMapKnownAttribute_internal(const struct KzcVertexBuffer* vertexBuffer, struct KzcHashMap* attributeMap,
                                                          kzString shaderAttributeName)
{
    kzUint i;
    kzsError result;

    for(i = 0; i < KZC_VERTEX_ATTRIBUTE_MAX_KNOWN_MAPPINGS; ++i)
    {
        kzString knownAttributeName = KZC_VERTEX_ATTRIBUTE_KNOWN_MAPPING[i].attributeName;
        if(knownAttributeName != KZ_NULL)
        {
            enum KzcVertexAttributeSemantic targetAttributeSemantic = KZC_VERTEX_ATTRIBUTE_KNOWN_MAPPING[i].semantic;

            /* Map known vertex semantics. */
            if (kzcStringIsEqual(shaderAttributeName, knownAttributeName))
            {
                struct KzcDynamicArrayIterator vertexBufferAttributeIterator =
                        kzcDynamicArrayGetIterator(vertexBuffer->vertexAttributes);

                while(kzcDynamicArrayIterate(vertexBufferAttributeIterator))
                {
                    struct KzcVertexAttributeInternal* attribute = kzcDynamicArrayIteratorGetValue(vertexBufferAttributeIterator);

                    /* Attributes match, add to hash map. */
                    if(attribute->semantic == targetAttributeSemantic)
                    {
                        kzBool hashMapHasValue = KZ_FALSE;
                        struct KzcHashMapIterator valueIterator;

                        valueIterator = kzcHashMapGetIterator(attributeMap);
                        while(kzcHashMapIterate(valueIterator))
                        {
                            struct KzcVertexAttributeInternal* vertexAttribute = (struct KzcVertexAttributeInternal*)kzcHashMapIteratorGetValue(valueIterator);

                            /* Vertex attribute was already found from map. */
                            if(attribute == vertexAttribute)
                            {
                                hashMapHasValue = KZ_TRUE;
                                break;
                            }
                        }
                        
                        /* No value was found, can be safely auto-mapped. */
                        if(!hashMapHasValue)
                        {
                            struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(vertexBuffer);
                            kzMutableString newString;

                            /* Copy string so that vertex buffer map now owns it. */
                            result = kzcStringCopy(memoryManager, shaderAttributeName, &newString);
                            kzsErrorForward(result);

                            result = kzcHashMapPut(attributeMap, newString, attribute);
                            kzsErrorForward(result);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            break;
        }
    }

    kzsSuccess();
}

/** Maps vertex buffer attributes to shader attributes. */
/*lint -esym(528,kzcVertexBufferMapKnownAttributes_internal) Only used in ES2 */
static kzsError kzcVertexBufferMapKnownAttributes_internal(const struct KzcVertexBuffer* vertexBuffer, 
                                                           const struct KzcDynamicArray* shaderAttributes,
                                                           struct KzcHashMap** out_attributeMap)
{
    kzsError result;
    
    struct KzcDynamicArrayIterator it;
    struct KzcHashMap* attributeMap;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(vertexBuffer);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &attributeMap);
    kzsErrorForward(result);

    if(shaderAttributes != KZ_NULL)
    {
        it = kzcDynamicArrayGetIterator(shaderAttributes);

        while(kzcDynamicArrayIterate(it))
        {
            kzString shaderAttributeName = (kzString)kzcDynamicArrayIteratorGetValue(it);
            result = kzcVertexBufferMapKnownAttribute_internal(vertexBuffer, attributeMap, shaderAttributeName);
            kzsErrorForward(result);
        }
    }

    *out_attributeMap = attributeMap;
    kzsSuccess();
}

kzsError kzcVertexBufferSetActiveShader(struct KzcVertexBuffer* vertexBuffer, const void* material, 
                                          const struct KzcDynamicArray* shaderAttributes)
{
    struct KzcHashMap* attributeMap;
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    KZ_UNUSED_PARAMETER(material);
    KZ_UNUSED_PARAMETER(shaderAttributes);

    if(kzcHashMapIsEmpty(vertexBuffer->attributeMappings))
    {
        /* Attribute mapping from shader attribute -> vertex attribute assignment. */
        attributeMap = KZ_NULL;
    }
    else
    {
        if(!kzcHashMapGet(vertexBuffer->attributeMappings, KZ_NULL, (void**)&attributeMap))
        {
            kzsErrorThrow(KZC_ERROR_RESOURCE_VERTEX_BUFFER_MAPPING_INVALID, "ES1 material attribute mappings not found!");
        }
    }
#elif defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;

    if(!kzcHashMapGet(vertexBuffer->attributeMappings, material, (void**)&attributeMap))
    {
        result = kzcVertexBufferMapKnownAttributes_internal(vertexBuffer, shaderAttributes, &attributeMap);
        kzsErrorForward(result);

        result = kzcHashMapPut(vertexBuffer->attributeMappings, material, attributeMap);
        kzsErrorForward(result);
    }
#else
    KZ_UNUSED_PARAMETER(vertexBuffer);
    KZ_UNUSED_PARAMETER(material);
    KZ_UNUSED_PARAMETER(shaderAttributes);
    attributeMap = KZ_NULL;
#endif

    vertexBuffer->currentAttributeMap = attributeMap;
    kzsSuccess();
}

static kzsError kzcVertexBufferMapAttribute_internal(const struct KzcVertexBuffer* internalVertexBuffer, struct KzcHashMap* attributeMap,
                                                     kzString shaderAttributeName, kzString vertexAttributeName)
{
    kzBool matchFound = KZ_FALSE;
    kzsError result;

    /* Map vertex attributes to shader attributes */
    struct KzcDynamicArrayIterator vertexBufferAttributeIterator = kzcDynamicArrayGetIterator(internalVertexBuffer->vertexAttributes);
    while(kzcDynamicArrayIterate(vertexBufferAttributeIterator))
    {
        struct KzcVertexAttributeInternal* attribute = (struct KzcVertexAttributeInternal*)kzcDynamicArrayIteratorGetValue(vertexBufferAttributeIterator);

        if (kzcStringIsEqual(vertexAttributeName, attribute->name))
        {
            struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(internalVertexBuffer);
            kzMutableString newString;

            /* Copy string so that vertex buffer map now owns it. */
            result = kzcStringCopy(memoryManager, shaderAttributeName, &newString);
            kzsErrorForward(result);

            /* Now we have mapping from shaderAttributeName -> vertexAttribute */
            result = kzcHashMapPut(attributeMap, newString, attribute);
            kzsErrorForward(result);
            matchFound = KZ_TRUE;
            break;
        }
    }

    kzsErrorTest(matchFound, KZC_ERROR_RESOURCE_VERTEX_BUFFER_MAPPING_INVALID,
                 "Invalid vertex buffer mapping (vertex attribute matching shader attribute not found!");

    kzsSuccess();
}

/** Maps shader attributes to vertex buffer attributes. */
static kzsError kzcVertexBufferMapAttributes_internal(const struct KzcVertexBuffer* vertexBuffer, 
                                                      const struct KzcHashMap* shaderAttributeToVertexAttribute,
                                                      const struct KzcDynamicArray* shaderAttributes,
                                                      struct KzcHashMap** out_attributeMap)
{
    kzsError result;

    struct KzcHashMapIterator it;
    struct KzcHashMap* attributeMap;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(vertexBuffer);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &attributeMap);
    kzsErrorForward(result);

    it = kzcHashMapGetIterator(shaderAttributeToVertexAttribute);

    while(kzcHashMapIterate(it))
    {
        kzMutableString shaderAttributeName = (kzMutableString)kzcHashMapIteratorGetKey(it);
        kzString vertexAttributeName = (kzString)kzcHashMapIteratorGetValue(it);
        
        /* Get string from shader attributes as hash map string pointer is deleted soon. */
        if(shaderAttributes != KZ_NULL)
        {
            struct KzcDynamicArrayIterator shaderIterator = kzcDynamicArrayGetIterator(shaderAttributes);
            while(kzcDynamicArrayIterate(shaderIterator))
            {
                kzMutableString shaderIteratorValue = (kzMutableString)kzcDynamicArrayIteratorGetValue(shaderIterator);
                if (kzcStringIsEqual(shaderAttributeName, shaderIteratorValue))
                {
                    shaderAttributeName = shaderIteratorValue;
                }
            }
        }

        result = kzcVertexBufferMapAttribute_internal(vertexBuffer, attributeMap, shaderAttributeName, vertexAttributeName);
        kzsErrorForward(result);
    }
    
    *out_attributeMap = attributeMap;
    kzsSuccess();
}

kzsError kzcVertexBufferAssignAttributeMapping(struct KzcVertexBuffer* vertexBuffer, const void* material,
                                               const struct KzcHashMap* vertexAttributeMap,
                                               const struct KzcDynamicArray* shaderAttributes)
{
    kzsError result;
    struct KzcHashMap* attributeMap;

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    KZ_UNUSED_PARAMETER(shaderAttributes);
#endif

    kzsAssert(kzcIsValidPointer(vertexAttributeMap));

    if(!kzcHashMapGet(vertexBuffer->attributeMappings, material, (void**)&attributeMap))
    {
        const struct KzcDynamicArray* attributes;

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
        
        /* On fixed pipeline, always use fixed attributes. */
        attributes = kzcResourceManagerGetFixedShaderAttributes(vertexBuffer->resourceManager);
#else
        attributes = shaderAttributes;
#endif

        /* No attribute map for this material specified, assign them. */
        result =  kzcVertexBufferMapAttributes_internal(vertexBuffer, vertexAttributeMap, attributes, &attributeMap);
        kzsErrorForward(result);

        result = kzcHashMapPut(vertexBuffer->attributeMappings, material, attributeMap);
        kzsErrorForward(result);
    }

    vertexBuffer->currentAttributeMap = attributeMap;

    kzsSuccess();
}

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
static kzBool kzcVertexBufferMapsIterate_internal(struct KzcDynamicArrayIterator* it, struct KzcHashMapIterator* hashMapIterator,
                                                  const struct KzcHashMap* attributeMap, struct KzcVertexAttributeInternal** out_internalVertexAttribute,
                                                  enum KzcVertexAttributeSemantic* out_semantic, kzUint* out_textureUnit)
{
    kzBool result = KZ_FALSE;
    struct KzcVertexAttributeInternal* internalVertexAttribute = KZ_NULL;
    enum KzcVertexAttributeSemantic semantic = KZC_VERTEX_ATTRIBUTE_POSITION;
    kzUint textureUnit;

    if(attributeMap != KZ_NULL)
    {
        textureUnit = 0;
        result = kzcHashMapIterate((*hashMapIterator));
        if(result)
        {
            kzString shaderAttributeName = (kzString)kzcHashMapIteratorGetKey((*hashMapIterator));
            internalVertexAttribute = (struct KzcVertexAttributeInternal*)kzcHashMapIteratorGetValue((*hashMapIterator));
            if (kzcStringIsEqual(shaderAttributeName, "kzPosition"))
            {
                semantic = KZC_VERTEX_ATTRIBUTE_POSITION;
            }
            else if (kzcStringIsEqual(shaderAttributeName, "kzNormal"))
            {
                semantic = KZC_VERTEX_ATTRIBUTE_NORMAL;
            }
            else if (kzcStringIsEqual(shaderAttributeName, "kzTangent"))
            {
                semantic = KZC_VERTEX_ATTRIBUTE_TANGENT;
            }
            else if (kzcStringIsEqual(shaderAttributeName, "kzColor0"))
            {
                semantic = KZC_VERTEX_ATTRIBUTE_COLOR;
            }
            else if (kzcStringIsEqual(shaderAttributeName, "kzTextureCoordinate0"))
            {
                semantic = KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE;
                textureUnit = 0;
            }
            else if (kzcStringIsEqual(shaderAttributeName, "kzTextureCoordinate1"))
            {
                semantic = KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE;
                textureUnit = 1;
            }
            else if (kzcStringIsEqual(shaderAttributeName, "kzTextureCoordinate2"))
            {
                semantic = KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE;
                textureUnit = 2;
            }
            else if (kzcStringIsEqual(shaderAttributeName, "kzTextureCoordinate3"))
            {
                semantic = KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE;
                textureUnit = 3;
            }
        }
    }
    else
    {
        textureUnit = (*out_textureUnit);
        result = kzcDynamicArrayIterate((*it));
        if(result)
        {
            internalVertexAttribute = (struct KzcVertexAttributeInternal*)kzcDynamicArrayIteratorGetValue((*it));
            semantic = internalVertexAttribute->semantic;
        }
    }

    *out_semantic = semantic;
    *out_textureUnit = textureUnit;
    *out_internalVertexAttribute = internalVertexAttribute;
    return result;
}
#endif

kzsError kzcVertexBufferAttach(/*lint -e{818}*/struct KzcVertexBuffer* vertexBuffer, struct KzcRenderer* renderer)
{
    kzUint vertexSize;

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
    struct KzcHashMap* shaderAttributeMap;
    struct KzcHashMapIterator shaderAttributeIterator;
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    struct KzcDynamicArrayIterator it;
    struct KzcVertexAttributeInternal* internalVertexAttribute;
    struct KzcHashMapIterator shaderAttributeIterator;
    enum KzcVertexAttributeSemantic semantic;
    kzUint textureUnit = 0;
#else
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(vertexBuffer);
#endif

    kzsAssert(kzcIsValidPointer(vertexBuffer));

    kzcRendererSetVertexCount(renderer, vertexBuffer->vertexCount);

    if(vertexBuffer->vertexCount > 0)
    {
        vertexSize = vertexBuffer->vertexSize;
        kzsAssert(vertexSize > 0);

        if(vertexBuffer->isVBO)
        {
            /* Bind vertex buffer. */
            kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_VERTEX, vertexBuffer->bufferHandle);
        }

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        shaderAttributeMap = kzcRendererGetActiveShaderAttributeMap(renderer);
        shaderAttributeIterator = kzcHashMapGetIterator(shaderAttributeMap);

        /* Loop through shader attributes. */
        while(kzcHashMapIterate(shaderAttributeIterator))
        {
            kzString shaderAttribute = (kzString)kzcHashMapIteratorGetKey(shaderAttributeIterator);
            struct KzcVertexAttributeInternal* internalVertexAttribute;

            if(kzcHashMapGet(vertexBuffer->currentAttributeMap, shaderAttribute, (void**)&internalVertexAttribute))
            {
                void* attributeData;
                kzUint* attributeIndex = (kzUint*)kzcHashMapIteratorGetValue(shaderAttributeIterator);
                kzUint glType;
                kzUint appliedVertexSize = vertexSize;

                if(!kzcVertexBufferGetOverrideSemantic_internal(vertexBuffer, internalVertexAttribute->semantic, &attributeData, &appliedVertexSize))
                {
                    if(vertexBuffer->isVBO)
                    {
                        attributeData = kzcCreateVBOPointer_internal(internalVertexAttribute->padding);
                    }
                    else
                    {
                        attributeData = (void*)((kzByte*)vertexBuffer->data + internalVertexAttribute->padding);
                    }
                }

                kzsAssert(attributeIndex != KZ_NULL);
                glType = kzcVertexAttributeDataTypeGetGLConstant_internal(internalVertexAttribute->dataType);

                kzsGlEnableVertexAttribArray(*attributeIndex);
                kzsGlVertexAttribPointer(*attributeIndex, (kzInt)internalVertexAttribute->dimension, glType, KZ_FALSE, (kzInt)appliedVertexSize, attributeData);
            }
            else
            {
                if(!vertexBuffer->invalidAttributesLogged)
                {
                    result = kzcLog(kzcMemoryGetManager(vertexBuffer), KZS_LOG_LEVEL_WARNING,
                        "Could not find attribute \"%s\" from vertex buffer while it was defined for material shader", shaderAttribute);
                    kzsErrorForward(result);
                    vertexBuffer->invalidAttributesLogged = KZ_TRUE;
                }
            }
        }

#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
        it = kzcDynamicArrayGetIterator(vertexBuffer->vertexAttributes);
        if(vertexBuffer->currentAttributeMap != KZ_NULL)
        {
            shaderAttributeIterator = kzcHashMapGetIterator(vertexBuffer->currentAttributeMap);
        }

        while(kzcVertexBufferMapsIterate_internal(&it, &shaderAttributeIterator, vertexBuffer->currentAttributeMap, &internalVertexAttribute,
            &semantic, &textureUnit))
        {
            kzUint glType;
            void* attributeData;
            kzUint appliedVertexSize = vertexSize;

            if(!kzcVertexBufferGetOverrideSemantic_internal(vertexBuffer, semantic, &attributeData, &appliedVertexSize))
            {
                if(vertexBuffer->isVBO)
                {
                    attributeData = kzcCreateVBOPointer_internal(internalVertexAttribute->padding);
                }
                else
                {
                    attributeData = (void*)((kzByte*)vertexBuffer->data + internalVertexAttribute->padding);
                }
            }

            glType = kzcVertexAttributeDataTypeGetGLConstant_internal(internalVertexAttribute->dataType);

            if(semantic == KZC_VERTEX_ATTRIBUTE_POSITION)
            {
                kzsGlEnableClientState(KZS_GL_VERTEX_ARRAY);
                kzsGlVertexPointer(3, glType, (kzInt)appliedVertexSize, attributeData);
            }
            else if(semantic == KZC_VERTEX_ATTRIBUTE_COLOR)
            {
                kzsGlEnableClientState(KZS_GL_COLOR_ARRAY);
                kzsGlColorPointer(4, glType, (kzInt)appliedVertexSize, attributeData);
            }
            else if(semantic == KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE)
            {
                if(vertexBuffer->currentAttributeMap != KZ_NULL)
                {
                    kzsGlClientActiveTexture(KZS_GL_TEXTURE0 + textureUnit);
                    kzsGlEnableClientState(KZS_GL_TEXTURE_COORD_ARRAY);
                    kzsGlTexCoordPointer(2, glType, (kzInt)appliedVertexSize, attributeData);
                }
                else
                {
                    kzUint i;
                    for(i = 0; i < KZC_RENDERER_MAX_TEXTURE_UNITS; ++i)
                    {
                        if(kzcRendererIsTexturingEnabled(renderer, i))
                        {
                            kzsGlClientActiveTexture(KZS_GL_TEXTURE0 + i);
                            kzsGlEnableClientState(KZS_GL_TEXTURE_COORD_ARRAY);
                            kzsGlTexCoordPointer(2, glType, (kzInt)appliedVertexSize, attributeData);
                        }
                    }
                }

            }
            else if(semantic == KZC_VERTEX_ATTRIBUTE_NORMAL)
            {
                kzsGlEnableClientState(KZS_GL_NORMAL_ARRAY);
                kzsGlNormalPointer(glType, (kzInt)appliedVertexSize, attributeData);
            }
            else if(semantic == KZC_VERTEX_ATTRIBUTE_TANGENT)
            {

            }
        }
#endif
    }

    kzsSuccess();
}

kzsError kzcVertexBufferDetach(/*lint -e{818}*/struct KzcVertexBuffer* vertexBuffer, struct KzcRenderer* renderer)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
    struct KzcHashMap* shaderAttributeMap;
    struct KzcHashMapIterator shaderAttributeIterator;
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    struct KzcDynamicArrayIterator it;
    struct KzcHashMapIterator shaderAttributeIterator;
    struct KzcVertexAttributeInternal* internalVertexAttribute;
    enum KzcVertexAttributeSemantic semantic;
    kzUint textureUnit = 0;
#else
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(vertexBuffer);
#endif

    if(vertexBuffer->vertexCount > 0)
    {
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        shaderAttributeMap = kzcRendererGetActiveShaderAttributeMap(renderer);
        shaderAttributeIterator = kzcHashMapGetIterator(shaderAttributeMap);

        while(kzcHashMapIterate(shaderAttributeIterator))
        {
            kzString shaderAttribute = (kzString)kzcHashMapIteratorGetKey(shaderAttributeIterator);
            struct KzcVertexAttributeInternal* internalVertexAttribute;

            if(kzcHashMapGet(vertexBuffer->currentAttributeMap, shaderAttribute, (void**)&internalVertexAttribute))
            {
                kzUint* attributeIndex = (kzUint*)kzcHashMapIteratorGetValue(shaderAttributeIterator);
                kzsAssert(attributeIndex != KZ_NULL);
                kzsGlDisableVertexAttribArray(*attributeIndex);
            }
            else
            {
                if(!vertexBuffer->invalidAttributesLogged)
                {
                    result = kzcLog(kzcMemoryGetManager(vertexBuffer), KZS_LOG_LEVEL_WARNING,
                        "Could not find attribute \"%s\" from vertex buffer while it was defined for material shader", shaderAttribute);
                    kzsErrorForward(result);
                    vertexBuffer->invalidAttributesLogged = KZ_TRUE;
                }
            }
        }
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
        it = kzcDynamicArrayGetIterator(vertexBuffer->vertexAttributes);
        if(vertexBuffer->currentAttributeMap != KZ_NULL)
        {
            shaderAttributeIterator = kzcHashMapGetIterator(vertexBuffer->currentAttributeMap);
        }

        while(kzcVertexBufferMapsIterate_internal(&it, &shaderAttributeIterator, vertexBuffer->currentAttributeMap, &internalVertexAttribute,
            &semantic, &textureUnit))
        {
            if(semantic == KZC_VERTEX_ATTRIBUTE_POSITION)
            {
                kzsGlDisableClientState(KZS_GL_VERTEX_ARRAY);
            }
            else if(semantic == KZC_VERTEX_ATTRIBUTE_NORMAL)
            {
                kzsGlDisableClientState(KZS_GL_NORMAL_ARRAY);
            }
            else if(semantic == KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE)
            {
                if(vertexBuffer->currentAttributeMap != KZ_NULL)
                {
                    kzsGlClientActiveTexture(KZS_GL_TEXTURE0 + textureUnit);
                    kzsGlDisableClientState(KZS_GL_TEXTURE_COORD_ARRAY);
                }
                else
                {
                    kzUint i;
                    for(i = 0; i < KZC_RENDERER_MAX_TEXTURE_UNITS; ++i)
                    {
                        if(kzcRendererIsTexturingEnabled(renderer, i))
                        {
                            kzsGlClientActiveTexture(KZS_GL_TEXTURE0 + i);
                            kzsGlDisableClientState(KZS_GL_TEXTURE_COORD_ARRAY);
                        }
                    }
                }
            }
            else if(semantic == KZC_VERTEX_ATTRIBUTE_COLOR)
            {
                kzsGlDisableClientState(KZS_GL_COLOR_ARRAY);
            }
            else
            {
                kzsLog(KZS_LOG_LEVEL_WARNING, "Invalid vertex buffer semantic at ES1!");
            }
        }
        kzsGlClientActiveTexture(KZS_GL_TEXTURE0);
#endif

        if(vertexBuffer->isVBO)
        {
            kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);
        }
    }

    kzsSuccess();
}

void kzcVertexBufferGetMemorySize(const struct KzcVertexBuffer* vertexBuffer, kzUint* out_GPUMemory, kzUint* out_RAMMemory)
{
    kzUint memorySizeGPU;
    kzUint memorySizeRAM = 0;

    kzsAssert(vertexBuffer != KZ_NULL);

    if(vertexBuffer->isVBO)
    {
        memorySizeGPU = vertexBuffer->vertexSize * vertexBuffer->vertexCount;
        if(vertexBuffer->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM)
        {
            memorySizeRAM = memorySizeGPU;
        }
    }
    else
    {
        memorySizeGPU = 0;
        memorySizeRAM = vertexBuffer->vertexSize * vertexBuffer->vertexCount;
    }

    *out_GPUMemory = memorySizeGPU;
    *out_RAMMemory = memorySizeRAM;
}

kzsError kzcVertexBufferGetSemanticData(const struct KzcMemoryManager* memoryManager, const struct KzcVertexBuffer* vertexBuffer,
                                        enum KzcVertexAttributeSemantic semantic, void** out_data)
{
    kzsError result;

    struct KzcDynamicArrayIterator it;
    void* data = KZ_NULL;
    it = kzcDynamicArrayGetIterator(vertexBuffer->vertexAttributes);

    kzsErrorTest(vertexBuffer->data != KZ_NULL, KZS_ERROR_ILLEGAL_ARGUMENT, "Cannot get vertices from vertex buffer not stored in RAM.");

    /* Find position attribute. */
    while (kzcDynamicArrayIterate(it))
    {
        struct KzcVertexAttributeInternal* attributeInternal = (struct KzcVertexAttributeInternal*)kzcDynamicArrayIteratorGetValue(it);

        if(attributeInternal->semantic == semantic)
        {
            kzUint i;
            kzU8* byteData;
            kzU8* originalData = (kzU8*)vertexBuffer->data + attributeInternal->padding;

            /* TODO: kzFloat assumed as vertex size. */
            result = kzcMemoryAllocPointer(memoryManager, &byteData, vertexBuffer->vertexCount * 
                attributeInternal->dimension * sizeof(kzFloat), "SemanticData");
            kzsErrorForward(result);

            data = byteData;

            for(i = 0; i < vertexBuffer->vertexCount; ++i)
            {
                kzsMemcpy(byteData, originalData, sizeof(kzFloat) * attributeInternal->dimension);
                byteData += 4 * attributeInternal->dimension;
                originalData += vertexBuffer->vertexSize;
            }

            break;
        }
    }

    *out_data = data;
    kzsSuccess();
}

kzUint kzcVertexBufferGetVertexCount( const struct KzcVertexBuffer* vertexBuffer)
{
    kzsAssert(kzcIsValidPointer(vertexBuffer));
    return vertexBuffer->vertexCount;
}

kzUint kzcVertexBufferGetVertexSize(const struct KzcVertexBuffer* vertexBuffer)
{
    kzsAssert(kzcIsValidPointer(vertexBuffer));
    return vertexBuffer->vertexSize;
}

void kzcVertexBufferAddOverrideSemantic(struct KzcVertexBuffer* vertexBuffer, enum KzcVertexAttributeSemantic semantic,
                                        kzUint dimension, void* data)
{
    kzsAssert(kzcIsValidPointer(vertexBuffer));

    vertexBuffer->overrideSemantics[vertexBuffer->overrideSemanticCount].padding = 0;
    vertexBuffer->overrideSemantics[vertexBuffer->overrideSemanticCount].dimension = dimension;
    vertexBuffer->overrideSemantics[vertexBuffer->overrideSemanticCount].semantic = semantic;
    vertexBuffer->overrideSemanticDatas[vertexBuffer->overrideSemanticCount] = data;
    
    ++vertexBuffer->overrideSemanticCount;
}

void kzcVertexBufferRemoveOverrideSemantic(struct KzcVertexBuffer* vertexBuffer, enum KzcVertexAttributeSemantic semantic)
{
    kzUint i;

    for(i = 0; i < vertexBuffer->overrideSemanticCount; ++i)
    {
        if(vertexBuffer->overrideSemantics[i].semantic == semantic)
        {
            kzUint j;
            for(j = i; j < vertexBuffer->overrideSemanticCount - 1; ++j)
            {
                vertexBuffer->overrideSemantics[j] = vertexBuffer->overrideSemantics[j + 1];
                vertexBuffer->overrideSemanticDatas[j] = vertexBuffer->overrideSemanticDatas[j + 1];
            }

            --vertexBuffer->overrideSemanticCount;
            break;
        }
    }
}

void kzcVertexBufferRemoveOverrideSemantics(struct KzcVertexBuffer* vertexBuffer)
{
    vertexBuffer->overrideSemanticCount = 0;
}

kzsError kzcVertexBufferGetVertices(const struct KzcMemoryManager* memoryManager, const struct KzcVertexBuffer* vertexBuffer, 
                                    struct KzcDynamicArray** out_vertices)
{
    kzsError result;
    struct KzcByteReadBuffer byteBuffer;
    struct KzcDynamicArray* vertices;

    kzsAssert(kzcIsValidPointer(vertexBuffer));

    kzsErrorTest(vertexBuffer->data != KZ_NULL, KZS_ERROR_ILLEGAL_ARGUMENT, "Cannot get vertices from vertex buffer not stored in RAM.");

    result = kzcDynamicArrayCreate(memoryManager, &vertices);
    kzsErrorForward(result);

    /* Arrange vertex data if it's list of arrays. */
    kzcByteBufferInitializeRead(&byteBuffer, vertexBuffer->data, vertexBuffer->vertexSize * vertexBuffer->vertexCount);

    /* Find vertices. */
    {
        kzUint sizeOfElement = sizeof(kzFloat);
        kzUint elementsPerVertex = 0;
        kzUint paddingToPosition = 0;
        kzUint positionDimensions = 0;
        struct KzcDynamicArrayIterator it;
        it = kzcDynamicArrayGetIterator(vertexBuffer->vertexAttributes);

        /* Find position attribute. */
        while (kzcDynamicArrayIterate(it))
        {
            struct KzcVertexAttributeInternal* attributeInternal = (struct KzcVertexAttributeInternal*)kzcDynamicArrayIteratorGetValue(it);

            elementsPerVertex += attributeInternal->dimension;
            if(attributeInternal->semantic == KZC_VERTEX_ATTRIBUTE_POSITION)
            {
                paddingToPosition = attributeInternal->padding;
                positionDimensions = attributeInternal->dimension;
            }
        }

        /* Find the vertices from data. */
        {
            kzUint i;
            kzUint count = vertexBuffer->vertexCount;
            for(i = 0; i < count; ++i)
            {
                struct KzcVector3* vertex;
                result = kzcMemoryAllocVariable(memoryManager, vertex, "Vertex buffer vertex");
                kzsErrorForward(result);
                if(paddingToPosition > 0)
                {
                    kzcByteBufferSkip(&byteBuffer, paddingToPosition * sizeOfElement);
                }
                kzcByteBufferRead(&byteBuffer, *vertex);
                result = kzcDynamicArrayAdd(vertices, vertex);
                kzsErrorForward(result);
                kzcByteBufferSkip(&byteBuffer, (elementsPerVertex - paddingToPosition - positionDimensions) * sizeOfElement);
            }
        }
    }    

    *out_vertices = vertices;
    kzsSuccess();
}

kzBool kzcVertexBufferContainsShaderAttribute(const struct KzcVertexBuffer* vertexBuffer, kzString shaderAttributeName)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsAssert(kzcIsValidPointer(vertexBuffer));
    return kzcHashMapContains(vertexBuffer->currentAttributeMap, shaderAttributeName);
#else
    kzBool containsAttribute = KZ_FALSE;
    kzsAssert(kzcIsValidPointer(vertexBuffer));
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(vertexBuffer->vertexAttributes);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzcVertexAttributeInternal* internalVertexAttribute = (struct KzcVertexAttributeInternal*)kzcDynamicArrayIteratorGetValue(it);
            kzMutableString name = internalVertexAttribute->name;
            
            if (kzcStringIsEqual(shaderAttributeName, name))
            {
                containsAttribute = KZ_TRUE;
                break;
            }
        }
    }
    return containsAttribute;
#endif
}

/*lint -esym(528,kzcVertexBufferGetOverrideSemantic_internal) Not used in NO_GL */
static kzBool kzcVertexBufferGetOverrideSemantic_internal(const struct KzcVertexBuffer* vertexBuffer, enum KzcVertexAttributeSemantic semantic,
                                                          void** out_data, kzUint* out_size)
{
    kzUint i;
    kzBool found = KZ_FALSE;
    if(vertexBuffer->usage == KZC_VERTEX_BUFFER_USAGE_DYNAMIC)
    {
        for(i = 0; i < vertexBuffer->overrideSemanticCount; ++i)
        {
            if(vertexBuffer->overrideSemantics[i].semantic == semantic)
            {
                found = KZ_TRUE;
                *out_data = vertexBuffer->overrideSemanticDatas[i];
                *out_size = vertexBuffer->overrideSemantics[i].dimension * sizeof(kzFloat);
                break;
            }
        }
    }

    return found;
}

kzBool kzcVertexBufferIsHalfFloatSupported(void)
{
    if(g_vertexHalfFloatExtensionSupported == -1)
    {
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
#if KZ_OPENGL_PLATFORM == KZ_PLATFORM_DESKTOP
        g_vertexHalfFloatExtensionSupported = 0;
#else
        kzString extensionString;
        const kzString HALF_FLOAT_EXTENSION = "GL_OES_vertex_half_float";
        kzUint index;

        /* Get list of the extensions */
        extensionString = kzsGlGetString(KZS_GL_EXTENSIONS);

        g_vertexHalfFloatExtensionSupported = kzcStringFindSubstring(extensionString, HALF_FLOAT_EXTENSION, &index) ? 1 : 0;
#endif
#else
        g_vertexHalfFloatExtensionSupported = 0;
#endif
    }

    return g_vertexHalfFloatExtensionSupported == 1 ? KZ_TRUE : KZ_FALSE;
}

enum KzcResourceMemoryType kzcVertexBufferGetMemoryType(const struct KzcVertexBuffer* vertexBuffer)
{
    kzsAssert(kzcIsValidPointer(vertexBuffer));
    return vertexBuffer->memoryType;
}
