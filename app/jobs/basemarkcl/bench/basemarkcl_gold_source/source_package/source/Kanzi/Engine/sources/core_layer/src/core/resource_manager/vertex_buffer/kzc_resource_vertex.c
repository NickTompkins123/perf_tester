/**
* \file
* Vertex attribute related functionality.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_resource_vertex.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex_buffer.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/string/kzc_string.h>
#include <core/debug/kzc_log.h>

#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_system.h>


/** Structure for vertex attribute. Contains size, type, dimension, data. */
struct KzcVertexAttribute
{
    kzMutableString name;                   /**< Vertex attribute name. */
    enum KzcVertexAttributeSemantic semantic;   /**< Vertex attribute semantic. */
    enum KzcVertexAttributeDataType dataType; /**< Vertex attribute data type. */
    kzUint elementSize;                 /**< Size of vertex attribute element. */
    kzUint dimension;                   /**< Dimension of vertex attribute. */
};

/** List of vertices. */
struct KzcVertexList
{
    struct KzcDynamicArray* vertexAttributes;    /**< List of vertex attributes, each are type of KzcVertexAttribute. */

    void* data;                                  /**< Vertex attribute data. */
    kzUint dataElementCount;                     /**< Number of data elements. */
    enum KzcVertexListFormat format;             /**< Format for this vertex list. */
};

kzsError kzcVertexListCreate(const struct KzcMemoryManager* memoryManager, struct KzcVertexList** out_vertexList)
{
    kzsError result;
    struct KzcVertexList* vertexList;
    
    /* Allocate memory for attribute list variable. */
    result = kzcMemoryAllocVariable(memoryManager, vertexList, "Vertex list");
    kzsErrorForward(result);
    
    /* Allocate memory for dynamic array (list of attributes) */
    result = kzcDynamicArrayCreate(memoryManager, &vertexList->vertexAttributes);
    kzsErrorForward(result);

    vertexList->data = KZ_NULL;
    vertexList->dataElementCount = 0;
    vertexList->format = KZC_VERTEX_LIST_FORMAT_LIST_OF_ARRAYS;

    *out_vertexList = vertexList;
    kzsSuccess();
}

kzsError kzcVertexListDelete(struct KzcVertexList* vertexList)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    kzsAssert(kzcIsValidPointer(vertexList));

    it = kzcDynamicArrayGetIterator(vertexList->vertexAttributes);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzcVertexAttribute* attribute = (struct KzcVertexAttribute*)kzcDynamicArrayIteratorGetValue(it);

        result = kzcStringDelete(attribute->name);
        kzsErrorForward(result);
        result = kzcMemoryFreeVariable(attribute);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayDelete(vertexList->vertexAttributes);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(vertexList);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcVertexListAddAttribute(const struct KzcVertexList* vertexList, kzString attributeName, enum KzcVertexAttributeSemantic type, 
                                   enum KzcVertexAttributeDataType dataType, kzUint elementSize, kzUint dimension)
{
    kzsError result;
    struct KzcVertexAttribute* attribute;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcIsValidPointer(vertexList));
    memoryManager = kzcMemoryGetManager(vertexList);

    result = kzcMemoryAllocVariable(memoryManager, attribute, "Vertex attribute");
    kzsErrorForward(result);

    /* Fill the vertex attribute structure. */
    attribute->semantic             = type;
    attribute->dataType             = dataType;
    attribute->elementSize          = elementSize;
    attribute->dimension            = dimension;
    result = kzcStringCopy(memoryManager, attributeName, &attribute->name);
    kzsErrorForward(result);

    /* Add attribute to list. */
    result = kzcDynamicArrayAdd(vertexList->vertexAttributes, attribute);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcVertexListConvertHalfFloatToFloat(struct KzcVertexList* vertexList, void** out_newData)
{
    kzsError result;
    kzBool convertVertexList = KZ_FALSE;
    kzUint newVertexSize = 0;
    kzUint oldVertexSize = 0;
    kzByte* newDataAllocated = KZ_NULL;
    struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(vertexList->vertexAttributes);
    kzUint* originalAttributeSize;
    kzUint currentAttributeIndex = 0;

    result = kzcMemoryAllocArray(kzcMemoryGetManager(vertexList), originalAttributeSize, kzcDynamicArrayGetSize(vertexList->vertexAttributes), "OriginalSizes");
    kzsErrorForward(result);

    while(kzcDynamicArrayIterate(it))
    {
        struct KzcVertexAttribute* attribute = kzcDynamicArrayIteratorGetValue(it);

        /* TODO: Don't access the members of iterators */
        originalAttributeSize[currentAttributeIndex] = kzcVertexAttributeGetElementSize(attribute);
        if(attribute->dataType == KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT16 && !kzcVertexBufferIsHalfFloatSupported())
        {
            attribute->dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32;
            attribute->elementSize = 4;

            newVertexSize += 4 * kzcVertexAttributeGetDimension(attribute);
            oldVertexSize += 2 * kzcVertexAttributeGetDimension(attribute);
            convertVertexList = KZ_TRUE;
        }
        else
        {
            newVertexSize += kzcVertexAttributeGetElementSize(attribute) * kzcVertexAttributeGetDimension(attribute);
            oldVertexSize += kzcVertexAttributeGetElementSize(attribute) * kzcVertexAttributeGetDimension(attribute);
        }

        ++currentAttributeIndex;
    }

    if(convertVertexList)
    {
        kzByte* newData;
        kzByte* originalData;
        kzUint originalOffset = 0;
        kzUint newOffset = 0;
        kzUint i;

        result = kzcMemoryAllocPointer(kzcMemoryGetManager(vertexList), &newDataAllocated, newVertexSize * vertexList->dataElementCount, "NewVertexList");
        kzsErrorForward(result);

        currentAttributeIndex = 0;
        it = kzcDynamicArrayGetIterator(vertexList->vertexAttributes);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzcVertexAttribute* attribute = kzcDynamicArrayIteratorGetValue(it);

            originalData = (kzByte*)vertexList->data + originalOffset;
            newData = (kzByte*)newDataAllocated + newOffset;

            if(originalAttributeSize[currentAttributeIndex] == 2)
            {
                for(i = 0; i < vertexList->dataElementCount; ++i)
                {
                    kzUint k;
                    kzFloat floatValues[4]; /* Vertex buffer maximum dimension is 4. */
                    for(k = 0; k < kzcVertexAttributeGetDimension(attribute); ++k)
                    {
                        kzU16 halfFloatValue;
                        if(kzsIsBigEndian())
                        {
                            halfFloatValue = /*lint -e{740}*/*(kzU16*)&originalData;
                        }
                        else
                        {
                            halfFloatValue = (kzU16)((kzByte)originalData[0] | ((kzByte)originalData[1] << 8));
                        }
                        
                        floatValues[k] = kzcVertexHalfFloatToFloat(halfFloatValue);
                        originalData += 2;
                    }

                    kzsMemcpy(newData, /*lint -e{772}*/floatValues, 4 * kzcVertexAttributeGetDimension(attribute));
                    originalData += oldVertexSize - 2 * kzcVertexAttributeGetDimension(attribute);
                    newData += newVertexSize;
                }
            }
            else
            {
                for(i = 0; i < vertexList->dataElementCount; ++i)
                {
                    kzsMemcpy(newData, originalData, 4);
                    newData += newVertexSize;
                    originalData += oldVertexSize;
                }
            }

            originalOffset += originalAttributeSize[currentAttributeIndex] * kzcVertexAttributeGetDimension(attribute);
            newOffset += kzcVertexAttributeGetMemorySize(attribute);
            ++currentAttributeIndex;
        }

        vertexList->data = newDataAllocated;

        kzsAssert(kzcIsValidPointer(vertexList->data));
    }

    result = kzcMemoryFreeArray(originalAttributeSize);
    kzsErrorForward(result);

    *out_newData = newDataAllocated;
    kzsSuccess();
}

kzsError kzcVertexListSetData(struct KzcVertexList* vertexList, void* data, kzUint numDataElements)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(vertexList));

    if(numDataElements > 65535)
    {
        result = kzcLog(kzcMemoryGetManager(vertexList), KZS_LOG_LEVEL_WARNING, "Currently vertex attribute maximum count is 65535 (unsigned short), tried to assign %d", numDataElements);
        kzsErrorForward(result);
    }

    vertexList->data = data;
    vertexList->dataElementCount = numDataElements;

    kzsSuccess();
}

enum KzcVertexAttributeSemantic kzcVertexAttributeGetSemantic(const struct KzcVertexAttribute* vertexAttribute)
{
    kzsAssert(kzcIsValidPointer(vertexAttribute));
    return vertexAttribute->semantic;
}

kzUint kzcVertexAttributeGetElementSize(const struct KzcVertexAttribute* attribute)
{
    kzsAssert(kzcIsValidPointer(attribute));
    return attribute->elementSize;
}

kzUint kzcVertexAttributeGetDimension(const struct KzcVertexAttribute* attribute)
{
    kzsAssert(kzcIsValidPointer(attribute));
    return attribute->dimension;
}

kzMutableString kzcVertexAttributeGetName(const struct KzcVertexAttribute* attribute)
{
    kzsAssert(kzcIsValidPointer(attribute));
    return attribute->name;
}

kzUint kzcVertexAttributeGetMemorySize(const struct KzcVertexAttribute* attribute)
{
    kzsAssert(kzcIsValidPointer(attribute));
    return (attribute->elementSize * attribute->dimension);
}

enum KzcVertexAttributeDataType kzcVertexAttributeGetDataType(const struct KzcVertexAttribute* attribute)
{
    kzsAssert(kzcIsValidPointer(attribute));
    return attribute->dataType;
}

kzUint kzcVertexListGetNumDataElements(const struct KzcVertexList* vertexList)
{
    kzsAssert(kzcIsValidPointer(vertexList));
    return vertexList->dataElementCount;
}

void* kzcVertexListGetData(const struct KzcVertexList* vertexList)
{
    kzsAssert(kzcIsValidPointer(vertexList));
    return vertexList->data;
}

struct KzcDynamicArray* kzcVertexListGetVertexAttributes(const struct KzcVertexList* vertexList)
{
    kzsAssert(kzcIsValidPointer(vertexList));
    return vertexList->vertexAttributes;
}

void kzcVertexListSetFormat(struct KzcVertexList* vertexList, enum KzcVertexListFormat format)
{
    kzsAssert(kzcIsValidPointer(vertexList));
    vertexList->format = format;
}

enum KzcVertexListFormat kzcVertexListGetFormat(const struct KzcVertexList* vertexList)
{
    return vertexList->format;
}

kzUint kzcVertexListGetVertexAttributesTotalSize(const struct KzcVertexList* vertexList)
{
    kzUint totalSize = 0;
    struct KzcDynamicArrayIterator it;

    it = kzcDynamicArrayGetIterator(vertexList->vertexAttributes);
    while(kzcDynamicArrayIterate(it))
    {
        struct KzcVertexAttribute* attribute = (struct KzcVertexAttribute*)kzcDynamicArrayIteratorGetValue(it);
        totalSize += kzcVertexAttributeGetMemorySize(attribute);
    }

    return totalSize;
}

kzUint kzcVertexListGetMemorySize(const struct KzcVertexList* vertexList)
{
    kzUint totalSize;

    totalSize = vertexList->dataElementCount * kzcVertexListGetVertexAttributesTotalSize(vertexList);

    return totalSize;
}

/* Based on OpenGL ES programming guide's article (Appendix A, Converting Float to Half float. */
#define KZU_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP  0x38000000
#define KZU_HALF_FLOAT_MAX_BIASED_EXP_AS_SINGLE_FP_EXP  0x47800000
#define KZU_FLOAT_MAX_BIASED_EXP                        (0xFF << 23)
#define KZU_HALF_FLOAT_MAX_BIASED_EXP                   (0x1F << 10)
#define KZU_FLOAT_MANTISSA_MASK                         ((1 << 23) - 1)
#define KZU_HALF_FLOAT_MANTISSA_MASK                    ((1 << 10) - 1)

kzU16 kzcVertexFloatToHalfFloat(kzFloat floatNumber)
{
    kzU32 bits = /*lint -e{740}*/*(kzU32*)&floatNumber;
    kzU16 sign = (kzU16)(bits >> 16) & 0x8000;
    kzU32 mantissa;
    kzU32 exponent;
    kzU16 halfFloat;

    mantissa = bits & KZU_FLOAT_MANTISSA_MASK;
    exponent = bits & KZU_FLOAT_MAX_BIASED_EXP;

    if(exponent >= KZU_HALF_FLOAT_MAX_BIASED_EXP_AS_SINGLE_FP_EXP)
    {
        if(mantissa != 0 && (exponent == KZU_FLOAT_MAX_BIASED_EXP))
        {
            mantissa = KZU_FLOAT_MANTISSA_MASK;
        }
        else
        {
            mantissa = 0;
        }
        halfFloat = sign | (kzU16)KZU_HALF_FLOAT_MAX_BIASED_EXP | (kzU16)(mantissa >> 13);
    }
    else if(exponent <= KZU_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP)
    {
        exponent = (KZU_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP - exponent) >> 23;
        mantissa >>= (14 + exponent);
        halfFloat = sign | (kzU16)mantissa;
    }
    else
    {
        halfFloat = sign | (kzU16)((exponent - KZU_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP) >> 13) | (kzU16)(mantissa >> 13);
    }

    return halfFloat;
}

kzFloat kzcVertexHalfFloatToFloat(kzU16 halfFloatNumber)
{
    kzU32 sign = (kzU32)(halfFloatNumber >> 15);
    kzU32 mantissa = (kzU32)(halfFloatNumber & KZU_HALF_FLOAT_MANTISSA_MASK);
    kzU32 exponent = (kzU32)(halfFloatNumber & KZU_HALF_FLOAT_MAX_BIASED_EXP);
    kzU32 bits;

    if(exponent == KZU_HALF_FLOAT_MAX_BIASED_EXP)
    {
        exponent = KZU_FLOAT_MAX_BIASED_EXP;
        if(mantissa != 0)
        {
            mantissa = KZU_FLOAT_MANTISSA_MASK;
        }
    }
    else if(exponent == 0)
    {
        if(mantissa != 0)
        {
            mantissa <<= 1;
            exponent = KZU_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
            while((mantissa & (1 << 10)) == 0)
            {
                mantissa <<= 1;
                exponent -= (1 << 23);
            }
            mantissa &= KZU_HALF_FLOAT_MANTISSA_MASK;
            mantissa <<= 13;
        }
    }
    else
    {
        mantissa <<= 13;
        exponent = (exponent << 13) + KZU_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
    }

    bits = (sign << 31) | exponent | mantissa;
    return /*lint -e{740}*/*((kzFloat*)&bits);
}

kzsError kzcVertexFloatArrayToHalfFloatArray(const struct KzcMemoryManager* memoryManager, const kzFloat* floatArray, kzUint floatValueCount,
                                             kzU16** out_halfFloatArray)
{
    kzsError result;
    kzU16* halfFloatArray;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, halfFloatArray, floatValueCount, "HalfFloatArray");
    kzsErrorForward(result);

    for(i = 0; i < floatValueCount; ++i)
    {
        halfFloatArray[i] = kzcVertexFloatToHalfFloat(floatArray[i]);
    }

    *out_halfFloatArray = halfFloatArray;
    kzsSuccess();
}
