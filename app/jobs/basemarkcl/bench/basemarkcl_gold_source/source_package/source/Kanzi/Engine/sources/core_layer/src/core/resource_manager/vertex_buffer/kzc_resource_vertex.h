/**
* \file
* Attribute-specific vertex structures. Contains type data (position, color, uv, weight) and
* helper functions for constructing vertex attribute lists.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_RESOURCE_VERTEX_H
#define KZC_RESOURCE_VERTEX_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcDynamicArray;
struct KzcMemoryManager;


/** Type of vertex attribute. */
enum KzcVertexAttributeSemantic
{
    /* Fixed function attributes. */
    KZC_VERTEX_ATTRIBUTE_POSITION,              /**< Vertex attribute for position. */
    KZC_VERTEX_ATTRIBUTE_COLOR,                 /**< Vertex attribute for color. */
    KZC_VERTEX_ATTRIBUTE_NORMAL,                /**< Vertex attribute for normal. */
    KZC_VERTEX_ATTRIBUTE_TANGENT,               /**< Vertex attribute for tangent. */
    KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE,    /**< Vertex attribute for texture coordinate. */
    KZC_VERTEX_ATTRIBUTE_WEIGHT,                /**< Vertex attribute for weight. */
    KZC_VERTEX_ATTRIBUTE_MATRIX_INDEX,          /**< Vertex attribute for matrix indices used in skinning. */

    KZC_VERTEX_ATTRIBUTE_CUSTOM                 /**< Custom vertex attribute (can be used on programmable pipeline). */
};

/** Vertex attribute data type. */
enum KzcVertexAttributeDataType
{
    KZC_VERTEX_ATTRIBUTE_DATA_TYPE_S8,          /**< Signed 8bit integer. */
    KZC_VERTEX_ATTRIBUTE_DATA_TYPE_U8,          /**< Unsigned 8bit integer. */
    KZC_VERTEX_ATTRIBUTE_DATA_TYPE_S16,         /**< Signed 16bit integer. */
    KZC_VERTEX_ATTRIBUTE_DATA_TYPE_U16,         /**< Unsigned 16bit integer. */
    KZC_VERTEX_ATTRIBUTE_DATA_TYPE_S32,         /**< Signed 32bit integer. */
    KZC_VERTEX_ATTRIBUTE_DATA_TYPE_U32,         /**< Unsigned 32bit integer. */
    KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT16,     /**< 16 bit floating point (half float). */
    KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32      /**< 32 bit floating point. */
};

/** Vertex list format. */
enum KzcVertexListFormat
{
    KZC_VERTEX_LIST_FORMAT_LIST_OF_ARRAYS,      /**< List of arrays. A1_0,A1_1,A1_2...A2_0,A2_1,A2_2... */
    KZC_VERTEX_LIST_FORMAT_LIST_OF_ATTRIBUTES   /**< List of attributes. A1_0,A2_0,A1_1,A2_1...A1_N,A2_N. */
};


struct KzcVertexAttributeInternal;

/**
* \struct KzcVertexAttribute
* Structure for single vertex attribute. Contains size, type, dimension and data.
*/
struct KzcVertexAttribute;

/**
* \struct KzcVertexList
* List of vertices, which is the actual data and attribute information.
*/
struct KzcVertexList;


/** Creates new vertex attribute list. */
kzsError kzcVertexListCreate(const struct KzcMemoryManager* memoryManager, struct KzcVertexList** out_attributeList);
/** Deletes vertex attribute list. */
kzsError kzcVertexListDelete(struct KzcVertexList* attributeList);
/** Adds new vertex attribute vertex list. */
kzsError kzcVertexListAddAttribute(const struct KzcVertexList* vertexAttributes, kzString attributeName, enum KzcVertexAttributeSemantic type, 
                                   enum KzcVertexAttributeDataType dataType, kzUint elementSize, kzUint dimension);
/** Adds data for vertex list, data specified as a1a1a1a1a1a2a2a2a2....aNaNaN where aN is Nth vertex attribute value. */
kzsError kzcVertexListSetData(struct KzcVertexList* vertexList, void* data, kzUint numDataElements);
/** Converts vertex list half float to float, out_newData containing converted data, KZ_NULL if not converted. */
kzsError kzcVertexListConvertHalfFloatToFloat(struct KzcVertexList* vertexList, void** out_newData);

/** Gets number of data elements (vertex count) in vertex list. */
kzUint kzcVertexListGetNumDataElements(const struct KzcVertexList* vertexList);
/** Gets data from vertex list. */
void* kzcVertexListGetData(const struct KzcVertexList* vertexList);
/** Gets vertex attribute array from vertex list. */
struct KzcDynamicArray* kzcVertexListGetVertexAttributes(const struct KzcVertexList* vertexList);
/** Sets format for vertex list. */
void kzcVertexListSetFormat(struct KzcVertexList* vertexList, enum KzcVertexListFormat format);
/** Gets format from vertex list. */
enum KzcVertexListFormat kzcVertexListGetFormat(const struct KzcVertexList* vertexList);
/** Gets total size of vertex attributes in bytes. */
kzUint kzcVertexListGetVertexAttributesTotalSize(const struct KzcVertexList* vertexList);
/** Gets total memory size of vertex list data. */
kzUint kzcVertexListGetMemorySize(const struct KzcVertexList* vertexList);

/** Gets vertex attribute type. */
enum KzcVertexAttributeSemantic kzcVertexAttributeGetSemantic(const struct KzcVertexAttribute* attribute);
/** Gets vertex attribute element size. */
kzUint kzcVertexAttributeGetElementSize(const struct KzcVertexAttribute* attribute);
/** Gets dimension from vertex attribute. */
kzUint kzcVertexAttributeGetDimension(const struct KzcVertexAttribute* attribute);
/** Gets name for vertex attribute. */
kzMutableString kzcVertexAttributeGetName(const struct KzcVertexAttribute* attribute);
/** Gets vertex attributes's memory size in bytes. */
kzUint kzcVertexAttributeGetMemorySize(const struct KzcVertexAttribute* attribute);
/** Gets vertex attribute data type. */
enum KzcVertexAttributeDataType kzcVertexAttributeGetDataType(const struct KzcVertexAttribute* attribute);


/** Converts float number to 16bit half float number. */
kzU16 kzcVertexFloatToHalfFloat(kzFloat floatNumber);
/** Converts half float number to full precision floating point number. */
kzFloat kzcVertexHalfFloatToFloat(kzU16 halfFloatNumber);
/** Converts float array to half float array. */
kzsError kzcVertexFloatArrayToHalfFloatArray(const struct KzcMemoryManager* memoryManager, const kzFloat* floatArray, kzUint floatValueCount,
                                             kzU16** out_halfFloatArray);


#endif
