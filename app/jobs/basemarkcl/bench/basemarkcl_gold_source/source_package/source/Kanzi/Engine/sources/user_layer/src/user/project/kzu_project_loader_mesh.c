/**
* \file
* Project loader for mesh files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_mesh.h"

#include "kzu_project_loader_object_node.h"
#include "kzu_project_loader_object_node_private.h"
#include "kzu_project_gpu_memory_type.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_material.h"
#include "kzu_project.h"

#include <user/scene_graph/kzu_object.h>
#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_util.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>
#include <user/material/kzu_material_type.h>
#include <user/material/kzu_material.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex_buffer.h>
#include <core/util/string/kzc_string.h>
#include <core/util/math/kzc_volume.h>
#include <core/memory/kzc_byte_buffer.h>
#include <core/debug/kzc_log.h>

#include <system/wrappers/kzs_opengl_base.h>
#include <system/file/kzs_file.h>
#include <system/time/kzs_tick.h>


/* TODO: Comment */
enum KzuBinaryVertexAttributeSemantic
{
    KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_POSITION = 0,
    KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_NORMAL = 1,
    KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_TANGENT = 2,
    KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_TEXTURE_COORDINATE = 3,
    KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_COLOR = 4,
    KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_WEIGHT = 5,
    KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_MATRIX_PALETTE = 6,
    KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_CUSTOM = 7
};

/* TODO: Comment */
enum KzuBinaryVertexAttributeDataType
{
    KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S8 = 0,
    KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U8 = 1,
    KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S16 = 2,
    KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U16 = 3,
    KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S32 = 4,
    KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U32 = 5,
    KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT16 = 6,
    KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32 = 7
};


/* TODO: Comment struct and members */
struct KzuBinaryVertexAttribute
{
    kzMutableString name;
    enum KzuBinaryVertexAttributeSemantic semantic;
    enum KzuBinaryVertexAttributeDataType dataType;
    kzUint dimension;
};


/** Internal function for assigning bones to mesh. */
static kzsError kzuProjectLoaderMeshAssignBones_internal(struct KzuProject* project, const struct KzuMesh* mesh,
                                                         kzString* bonePaths, kzUint boneCount,
                                                         const struct KzcMatrix4x4* bindShapeMatrix,
                                                         struct KzcMatrix4x4* invBindMatrices)
{
    kzsError result;
    kzUint i;
    struct KzuObjectNode** boneControllers = KZ_NULL;

    if(boneCount > 0)
    {
        result = kzcMemoryAllocArray(kzcMemoryGetManager(project), boneControllers, boneCount, "BoneControllers");
        kzsErrorForward(result);
    }

    for (i = 0; i < boneCount; ++i)
    {
        struct KzuObjectNode* objectNode;

        kzsAssert(bonePaths != KZ_NULL);
        kzsAssert(boneControllers != KZ_NULL);
        kzsAssert(bonePaths[i] != KZ_NULL);

        result = kzuProjectLoaderLoadObjectNode(project, bonePaths[i], &objectNode);
        kzsErrorForward(result);

        kzsAssert(kzuObjectNodeIsValid(objectNode));

        boneControllers[i] = objectNode;
    }

    if(boneCount > 0)
    {
        result = kzuMeshAssignBoneData(mesh, boneCount, boneControllers, invBindMatrices);
        kzsErrorForward(result);

        kzuMeshSetBindShapeMatrix(mesh, bindShapeMatrix);

        result = kzcMemoryFreeArray(boneControllers);
        kzsErrorForward(result);
    }

    if(invBindMatrices != KZ_NULL)
    {
        result = kzcMemoryFreeArray(invBindMatrices);
        kzsErrorForward(result);
    }

    if(bonePaths != KZ_NULL)
    {
        result = kzcMemoryFreeArray((kzMutableString*)bonePaths);
        kzsErrorForward(result);
    }

    kzsSuccess();
}


/** Gets data size for vertex attribute element in bytes. */
static kzUint kzuProjectLoaderGetVertexAttributeDataSize_internal(enum KzuBinaryVertexAttributeDataType attributeDataType)
{
    kzUint dataSize = 0;

    switch (attributeDataType)
    {
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S8: dataSize = 1; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U8: dataSize = 1; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S16: dataSize = 2; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U16: dataSize = 2; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S32: dataSize = 4; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U32: dataSize = 4; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT16: dataSize = 2; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32: dataSize = 4; break;
        default: kzsAssertText(KZ_FALSE, "Invalid vertex attribute data type");
    }

    return dataSize;
}

/** Returns vertex attribute semantic from vinary attribute semantic. */
static kzsError kzuProjectLoaderGetVertexAttributeSemantic_internal(enum KzuBinaryVertexAttributeSemantic binarySemantic,
                                                                    enum KzcVertexAttributeSemantic* out_semantic)
{
    enum KzcVertexAttributeSemantic semantic;

    switch (binarySemantic)
    {
        case KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_POSITION: semantic = KZC_VERTEX_ATTRIBUTE_POSITION; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_NORMAL: semantic = KZC_VERTEX_ATTRIBUTE_NORMAL; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_TANGENT: semantic = KZC_VERTEX_ATTRIBUTE_TANGENT; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_TEXTURE_COORDINATE: semantic = KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_COLOR: semantic = KZC_VERTEX_ATTRIBUTE_COLOR; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_WEIGHT: semantic = KZC_VERTEX_ATTRIBUTE_WEIGHT; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_MATRIX_PALETTE: semantic = KZC_VERTEX_ATTRIBUTE_MATRIX_INDEX; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_SEMANTIC_CUSTOM: semantic = KZC_VERTEX_ATTRIBUTE_CUSTOM; break;
        default: kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid vertex attribute semantic");
    }

    *out_semantic = semantic;
    kzsSuccess();
}

/** Returns vertex attribute data type from binary vertex attribute type. */
static kzsError kzuProjectLoaderGetVertexAttributeDataType_internal(enum KzuBinaryVertexAttributeDataType binaryDataType,
                                                                    enum KzcVertexAttributeDataType* out_dataType)
{
    enum KzcVertexAttributeDataType dataType;

    switch (binaryDataType)
    {
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S8: dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_S8; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U8: dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_U8; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S16: dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_S16; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U16: dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_U16; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S32: dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_S32; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U32: dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_U32; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT16: dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT16; break;
        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32: dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32; break;
        default: kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid vertex attribute data type");
    }

    *out_dataType = dataType;
    kzsSuccess();
}

/** Verifies mesh vertex attributes if they're valid with material type. */
static kzsError kzuProjectLoaderMeshVerifyAttributes_internal(const struct KzcMemoryManager* memoryManager, const struct KzuMesh* mesh,
                                                              kzString path, kzString materialPath)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
    struct KzuMaterial* meshMaterial = kzuMeshGetMaterial(mesh);

    if(meshMaterial != KZ_NULL)
    {
        struct KzcDynamicArray* vertexAttributes = kzuMaterialTypeGetVertexAttributes(kzuMaterialGetMaterialType(meshMaterial));
        struct KzcDynamicArrayIterator attributeIterator = kzcDynamicArrayGetIterator(vertexAttributes);

        while(kzcDynamicArrayIterate(attributeIterator))
        {
            kzString attribute = (kzString)kzcDynamicArrayIteratorGetValue(attributeIterator);

            result = kzcVertexBufferSetActiveShader(kzuMeshGetVertexBuffer(mesh), kzuMaterialGetMaterialType(kzuMeshGetMaterial(mesh)), 
                kzuMaterialTypeGetVertexAttributes(kzuMaterialGetMaterialType(kzuMeshGetMaterial(mesh))));
            kzsErrorForward(result);

            if(!kzcVertexBufferContainsShaderAttribute(kzuMeshGetVertexBuffer(mesh), attribute))
            {
                kzMutableString warningString;

                result = kzcStringFormat(memoryManager, "Vertex attribute %s not found from mesh %s but was defined for material %s", &warningString,
                                         attribute, path, materialPath);
                kzsErrorForward(result);

                kzsLog(KZS_LOG_LEVEL_WARNING, warningString);

                result = kzcStringDelete(warningString);
                kzsErrorForward(result);
            }
        }
    }

#else
    KZ_UNUSED_PARAMETER(memoryManager);
    KZ_UNUSED_PARAMETER(mesh);
    KZ_UNUSED_PARAMETER(path);
    KZ_UNUSED_PARAMETER(materialPath);
#endif
    kzsSuccess();
}

/* TODO: Split into smaller functions */
kzsException kzuProjectLoaderLoadMesh(struct KzuProject* project, kzString path, struct KzuMesh** out_mesh)
{
    kzsError result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuMesh* mesh;
    kzUint measurementStart = kzsTimeGetCurrentTimestamp();

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load mesh with null path");

    /* Check if the mesh is already loaded. */
    mesh = (struct KzuMesh*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_MESH, path);

    if (!kzuMeshIsValid(mesh))
    {
        /* Get file info from directory corresponding the given path. */
        /* result = kzuBinaryDirectoryGetFile(directory, path, &file);
        kzsErrorForward(result); */
        result = kzuBinaryDirectoryGetFileFromMeshLibrary(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_MESH, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load mesh file.");

        /* Load the mesh of the file info. */
        {
            kzUint numVertices;
            kzByte* vertexData;
            struct KzcVertexList* vertexList;
            struct KzuClusterDefinition* clusterDefinitions;
            kzUint clusterCount;
            kzUint attributeMappingGroupCount;
            struct KzcHashMap* attributeMappings = KZ_NULL;
            enum KzuBinaryGpuMemoryType memoryType;
            enum KzcResourceMemoryType resourceMemoryType;
            struct KzcVolume boundingVolume;

            /* References */
            kzString* materialPaths;
            kzString* bonePaths = KZ_NULL;
            struct KzcMatrix4x4* invBindMatrices = KZ_NULL;
            struct KzcMatrix4x4 bindShapeMatrix;
            kzUint boneCount;

            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                /* Vertex buffer */
                {
                    kzUint numVertexAttributes;
                    struct KzuBinaryVertexAttribute* vertexAttributes;

                    /* Vertex attributes */
                    {
                        kzUint i;

                        result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&memoryType);
                        kzsErrorForward(result);
                        result = kzuProjectLoaderGetMemoryType_private(memoryType, &resourceMemoryType);
                        kzsErrorForward(result);

                        result = kzcInputStreamReadU32Int(inputStream, &numVertexAttributes);
                        kzsErrorForward(result);

                        result = kzcMemoryAllocArray(memoryManager, vertexAttributes, numVertexAttributes, "Vertex attributes");
                        kzsErrorForward(result);

                        for (i = 0; i < numVertexAttributes; ++i)
                        {
                            kzMutableString attributeName;
                            enum KzuBinaryVertexAttributeSemantic attributeSemantic;
                            enum KzuBinaryVertexAttributeDataType attributeDataType;
                            kzUint attributeDimension;

                            result = kzuBinaryReadString(memoryManager, inputStream, &attributeName);
                            kzsErrorForward(result);

                            result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&attributeSemantic);
                            kzsErrorForward(result);

                            result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&attributeDataType);
                            kzsErrorForward(result);

                            result = kzcInputStreamReadU32Int(inputStream, &attributeDimension);
                            kzsErrorForward(result);

                            vertexAttributes[i].name = attributeName;
                            vertexAttributes[i].semantic = attributeSemantic;
                            vertexAttributes[i].dataType = attributeDataType;
                            vertexAttributes[i].dimension = attributeDimension;
                        }
                    }

                    /* Vertices */
                    {
                        kzUint i;
                        kzUint vertexDataSize = 0;
                        struct KzcByteWriteBuffer vertexDataBuffer;

                        result = kzcInputStreamReadU32Int(inputStream, &numVertices);
                        kzsErrorForward(result);

                        for (i = 0; i < numVertexAttributes; ++i)
                        {
                            struct KzuBinaryVertexAttribute* vertexAttribute = &vertexAttributes[i];
                            enum KzuBinaryVertexAttributeDataType attributeDataType = vertexAttribute->dataType;
                            kzUint attributeDimension = vertexAttribute->dimension;

                            vertexDataSize += kzuProjectLoaderGetVertexAttributeDataSize_internal(attributeDataType) * attributeDimension;
                        }
                        vertexDataSize *= numVertices;

                        result = kzcMemoryAllocPointer(memoryManager, &vertexData, vertexDataSize, "Vertex data");
                        kzsErrorForward(result);

                        kzcByteBufferInitializeWrite(&vertexDataBuffer, vertexData, vertexDataSize);

#if 1
                        /* TODO: Assuming that all attributes are type of float, as it's currently only way to export vertex buffer data. */
                        {
#if 0
                            kzFloat* data = /*lint -e{826}*/(kzFloat*)vertexData;

                            result = kzcInputStreamReadFloatArray(inputStream, vertexDataSize / sizeof(kzFloat), data);
                            kzsErrorForward(result);
#else
                            result = kzcInputStreamReadBytes(inputStream, vertexDataSize, vertexData);
                            kzsErrorForward(result);
#endif

                        }
#if 0
                        for (i = 0; i < numVertices; ++i)
                        {
                            kzUint j;

                            for (j = 0; j < numVertexAttributes; ++j)
                            {
                                struct KzuBinaryVertexAttribute* vertexAttribute = &vertexAttributes[j];
                                enum KzuBinaryVertexAttributeDataType attributeDataType = vertexAttribute->dataType;
                                kzUint attributeDimension = vertexAttribute->dimension;
                                kzUint k;

                                for (k = 0; k < attributeDimension; ++k)
                                {
                                    switch (attributeDataType)
                                    {
                                        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S8:
                                        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U8:
                                        {
                                            kzU8 value;
                                            result = kzcInputStreamReadU8(inputStream, &value);
                                            kzsErrorForward(result);

                                            kzcByteBufferWrite(&vertexDataBuffer, value);
                                            break;
                                        }

                                        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S16:
                                        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U16:
                                        {
                                            kzU16 value;
                                            result = kzcInputStreamReadU16(inputStream, &value);
                                            kzsErrorForward(result);

                                            kzcByteBufferWrite(&vertexDataBuffer, value);
                                            break;
                                        }

                                        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_S32:
                                        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_U32:
                                        {
                                            kzU32 value;
                                            result = kzcInputStreamReadU32(inputStream, &value);
                                            kzsErrorForward(result);

                                            kzcByteBufferWrite(&vertexDataBuffer, value);
                                            break;
                                        }

                                        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT16:
                                        {
                                            KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Attribute data type float16 property not supported yet.");
                                        }

                                        case KZU_BINARY_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32:
                                        {
                                            kzFloat value;
                                            result = kzcInputStreamReadFloat(inputStream, &value);
                                            kzsErrorForward(result);

                                            kzcByteBufferWrite(&vertexDataBuffer, value);
                                            break;
                                        }
                                        default:
                                        {
                                            kzsErrorThrow(KZU_ERROR_INVALID_BINARY_DATA, "Invalid vertex attribute data type!");
                                        }
                                    }
                                }
                            }
                        }
                        kzsAssert(kzcByteBufferIsFinished(&vertexDataBuffer));
#endif
                    }
#endif

                    /* Create vertex buffer */
                    {
                        kzUint i;

                        result = kzcVertexListCreate(memoryManager, &vertexList);
                        kzsErrorForward(result);
                        kzcVertexListSetFormat(vertexList, KZC_VERTEX_LIST_FORMAT_LIST_OF_ATTRIBUTES);

                        for (i = 0; i < numVertexAttributes; ++i)
                        {
                            struct KzuBinaryVertexAttribute* vertexAttribute = &vertexAttributes[i];
                            kzString name = vertexAttribute->name;
                            enum KzcVertexAttributeSemantic semantic;
                            enum KzuBinaryVertexAttributeDataType dataType = vertexAttribute->dataType;
                            enum KzcVertexAttributeDataType vertexDataType;
                            kzUint dimension = vertexAttribute->dimension;

                            result = kzuProjectLoaderGetVertexAttributeSemantic_internal(vertexAttribute->semantic, &semantic);
                            kzsErrorForward(result);

                            result = kzuProjectLoaderGetVertexAttributeDataType_internal(dataType, &vertexDataType);
                            kzsErrorForward(result);

                            result = kzcVertexListAddAttribute(vertexList, name, semantic, vertexDataType, kzuProjectLoaderGetVertexAttributeDataSize_internal(dataType), dimension);
                            kzsErrorForward(result);

                            result = kzcStringDelete((kzMutableString)name);
                            kzsErrorForward(result);
                        }

                        result = kzcVertexListSetData(vertexList, vertexData, numVertices);
                        kzsErrorForward(result);
                    }

                    result = kzcMemoryFreeArray(vertexAttributes);
                    kzsErrorForward(result);
                }

                /* Clusters. */
                {
                    kzUint j;
                    kzUint indexSize;
                    
                    /* Number of clusters. */
                    result = kzcInputStreamReadU32Int(inputStream, &clusterCount);
                    kzsErrorForward(result);

                    result = kzcMemoryAllocArray(memoryManager, materialPaths, clusterCount, "Mesh cluster material reference paths");
                    kzsErrorForward(result);

                    result = kzcMemoryAllocArray(memoryManager, clusterDefinitions, clusterCount, "Mesh cluster definitions");
                    kzsErrorForward(result);

                    for (j = 0; j < clusterCount; ++j)
                    {
                        result = kzuBinaryDirectoryReadReference(inputStream, file, &materialPaths[j]);
                        kzsErrorForward(result);

                        result = kzcInputStreamReadU32Int(inputStream, &clusterDefinitions[j].indexCount);
                        kzsErrorForward(result);

                        result = kzcInputStreamReadU32Int(inputStream, &indexSize);
                        kzsErrorForward(result);

                        /* TODO: Handle 32 bit indices */
                        if (indexSize == 2)
                        {
                            kzU16* indices;

                            result = kzcMemoryAllocArray(memoryManager, indices, clusterDefinitions[j].indexCount,
                                                         "Index buffer indices");
                            kzsErrorForward(result);

                            clusterDefinitions[j].indexData = indices;

                            result = kzcInputStreamReadU16Array(inputStream, clusterDefinitions[j].indexCount, indices);
                            kzsErrorForward(result);

                            /* 32bit padding. */
                            if ((clusterDefinitions[j].indexCount % 2) == 1)
                            {
                                result = kzcInputStreamSkip(inputStream, 2);
                                kzsErrorForward(result);
                            }

                        }
                        else if (indexSize == 4)
                        {
                            KZS_NOT_IMPLEMENTED_YET_ERROR_TEXT("Cluster index size 4 not supported yet.");
                        }
                        else
                        {
                            kzsErrorThrow(KZU_ERROR_INVALID_BINARY_DATA, "Invalid index size");
                        }
                    }
                }

                /* Bones */
                result = kzcInputStreamReadU32Int(inputStream, &boneCount);
                kzsErrorForward(result);

                if(boneCount > 0)
                {
                    kzUint i;

                    result = kzcMemoryAllocArray(memoryManager, bonePaths, boneCount, "Mesh bone reference paths");
                    kzsErrorForward(result);
                    result = kzcMemoryAllocArray(memoryManager, invBindMatrices, boneCount, "Bone inv bind matrices");
                    kzsErrorForward(result);

                    result = kzcInputStreamReadFloatArray(inputStream, 16, bindShapeMatrix.data);
                    kzsErrorForward(result);

                    for (i = 0; i < boneCount; ++i)
                    {
                        result = kzuBinaryDirectoryReadReference(inputStream, file, &bonePaths[i]);
                        kzsErrorForward(result);

                        result = kzcInputStreamReadFloatArray(inputStream, 16, invBindMatrices[i].data);
                        kzsErrorForward(result);
                    }
                }

                kzuProjectAddMeasurementCumulativeTime_private(project, measurementStart, KZU_PROJECT_MEASUREMENT_MESH);

                /* Attribute mappings. */
                {
                    kzUint i;

                    result = kzcInputStreamReadU32Int(inputStream, &attributeMappingGroupCount);
                    kzsErrorForward(result);

                    if (attributeMappingGroupCount > 0)
                    {
                        result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &attributeMappings);
                        kzsErrorForward(result);
                    }

                    /* Loop all material types assigned for this geometry and assign values. */
                    for (i = 0; i < attributeMappingGroupCount; ++i)
                    {
                        kzUint j;
                        kzString materialTypePath;
                        struct KzuMaterialType* materialType = KZ_NULL;
                        struct KzcHashMap* materialTypeMappings;
                        kzUint attributeMappingCount;

                        result = kzuBinaryDirectoryReadReference(inputStream, file, &materialTypePath);
                        kzsErrorForward(result);

                        if (materialTypePath != KZ_NULL)
                        {
                            result = kzuProjectLoaderLoadMaterialType(project, materialTypePath, &materialType);
                            kzsErrorForward(result);
                        }

                        result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &materialTypeMappings);
                        kzsErrorForward(result);

                        result = kzcInputStreamReadU32Int(inputStream, &attributeMappingCount);
                        kzsErrorForward(result);

                        for (j = 0; j < attributeMappingCount; ++j)
                        {
                            kzMutableString vertexShaderAttribute;
                            kzMutableString vertexBufferAttribute;

                            result = kzuBinaryReadString(memoryManager, inputStream, &vertexShaderAttribute);
                            kzsErrorForward(result);

                            result = kzuBinaryReadString(memoryManager, inputStream, &vertexBufferAttribute);
                            kzsErrorForward(result);

                            result = kzcHashMapPut(materialTypeMappings, vertexShaderAttribute, vertexBufferAttribute);
                            kzsErrorForward(result);
                        }

                        if(materialType != KZ_NULL)
                        {
                            result = kzcHashMapPut(attributeMappings, materialType, materialTypeMappings);
                            kzsErrorForward(result);
                        }
                        else
                        {
                            kzsErrorThrow(KZU_ERROR_INVALID_BINARY_DATA, "Null material type for attribute mapping occurred!");
                        }
                    }
                }

                /* Bounding volume. */
                {
                    kzFloat boundingVolumeValues[6];
                    result = kzcInputStreamReadFloatArray(inputStream, 6, boundingVolumeValues);
                    kzsErrorForward(result);

                    boundingVolume = kzcVolume(boundingVolumeValues[0], boundingVolumeValues[1], boundingVolumeValues[2],
                        boundingVolumeValues[3] - boundingVolumeValues[0], boundingVolumeValues[4] - boundingVolumeValues[1],
                        boundingVolumeValues[5] - boundingVolumeValues[2]);
                }

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            measurementStart = kzsTimeGetCurrentTimestamp();

            result = kzuMeshCreate(memoryManager, resourceMemoryType, &mesh);
            kzsErrorForward(result);

            {
                struct KzuBoundingVolume* meshBoundingVolume;
                result = kzuBoundingVolumeCreateEmpty(memoryManager, &meshBoundingVolume);
                kzsErrorForward(result);
                kzuBoundingVolumeSetVolume(meshBoundingVolume, &boundingVolume);

                kzuMeshSetBoundingVolume(mesh, meshBoundingVolume);
            }

            result = kzuMeshAssignData(mesh, kzuProjectGetResourceManager(project), vertexList,
                                       clusterDefinitions, clusterCount, attributeMappings);
            kzsErrorForward(result);

            {
                kzUint i;

                for(i = 0; i < clusterCount; ++i)
                {
                    result = kzcMemoryFreeArray(clusterDefinitions[i].indexData);
                    kzsErrorForward(result);
                }
            }

            result = kzcVertexListDelete(vertexList);
            kzsErrorForward(result);

            result = kzcMemoryFreePointer(vertexData);
            kzsErrorForward(result);

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_MESH, mesh, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&mesh);
            kzsErrorForward(result);

            /* Delete attribute mappings. */
            if (attributeMappings != KZ_NULL)
            {
                struct KzcHashMapIterator materialTypes;

                materialTypes = kzcHashMapGetIterator(attributeMappings);

                while(kzcHashMapIterate(materialTypes))
                {
                    struct KzcHashMap* mappings = (struct KzcHashMap*)kzcHashMapIteratorGetValue(materialTypes);
                    struct KzcHashMapIterator mappingsIterator = kzcHashMapGetIterator(mappings);

                    /* Iterate mappings and delete strings and finally hash map. */
                    while(kzcHashMapIterate(mappingsIterator))
                    {
                        kzMutableString key = (kzMutableString)kzcHashMapIteratorGetKey(mappingsIterator);
                        kzMutableString value = (kzMutableString)kzcHashMapIteratorGetValue(mappingsIterator);

                        result = kzcStringDelete(key);
                        kzsErrorForward(result);

                        result = kzcStringDelete(value);
                        kzsErrorForward(result);
                    }

                    result = kzcHashMapDelete(mappings);
                    kzsErrorForward(result);
                }

                result = kzcHashMapDelete(attributeMappings);
                kzsErrorForward(result);
            }

            kzuProjectAddMeasurementCumulativeTime_private(project, measurementStart, KZU_PROJECT_MEASUREMENT_MESH);

            /* Load references */
            {
                kzUint i;
                struct KzuMaterial* material;

                /* Material path can be NULL if, e.g., mesh's material is one of the unlinked materials that are not exported. */ 
                for(i = 0; i < clusterCount; ++i)
                {
                    if (materialPaths[i] != KZ_NULL)
                    {
                        struct KzuMeshCluster* cluster;

                        result = kzuProjectLoaderLoadMaterial(project, materialPaths[i], &material);
                        kzsErrorForward(result);

                        cluster = kzuMeshGetClusterAtIndex(mesh, i);
                        kzuMeshClusterSetMaterial(cluster, material);

                        result = kzuProjectLoaderMeshVerifyAttributes_internal(memoryManager, mesh, kzuBinaryFileInfoGetPath(file), materialPaths[i]);
                        kzsErrorForward(result);
                    }
                    else
                    {
                        material = KZ_NULL;
                    }
                }

                result = kzcMemoryFreeArray(clusterDefinitions);
                kzsErrorForward(result);

                result = kzcMemoryFreeArray((kzMutableString*)materialPaths);
                kzsErrorForward(result);
            }

            /* TODO: bindShapeMatrix is not always initialized. Initialize it and remove the lint comment */
            result = kzuProjectLoaderMeshAssignBones_internal(project, mesh, bonePaths, boneCount,
                                                              /*lint -e{645}*/&bindShapeMatrix, invBindMatrices);
            kzsErrorForward(result);
        }
        
    }

    *out_mesh = mesh;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadBox(struct KzuProject* project, kzString path, struct KzuMesh** out_mesh)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuMesh* mesh;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load box with null path");

    /* Check if the mesh is already loaded. */
    mesh = (struct KzuMesh*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_MESH, path);

    if (!kzuMeshIsValid(mesh))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_MESH_BOX, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load mesh file.");

        /* Create mesh. */
        {
            kzFloat width, height, length;
            kzString materialPath;
            struct KzuMaterial* material = KZ_NULL;
            enum KzcResourceMemoryType resourceMemoryType;

            /* Read data */
            {
                enum KzuBinaryGpuMemoryType memoryType;
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                /* Material. */
                result = kzuBinaryDirectoryReadReference(inputStream, file, &materialPath);
                kzsErrorForward(result);

                result = kzcInputStreamReadFloat(inputStream, &width);
                kzsErrorForward(result);

                result = kzcInputStreamReadFloat(inputStream, &height);
                kzsErrorForward(result);

                result = kzcInputStreamReadFloat(inputStream, &length);
                kzsErrorForward(result);

                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&memoryType);
                kzsErrorForward(result);
                result = kzuProjectLoaderGetMemoryType_private(memoryType, &resourceMemoryType);
                kzsErrorForward(result);

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzuMeshCreate(memoryManager, resourceMemoryType, &mesh);
            kzsErrorForward(result);

            {
                struct KzcResourceManager* resourceManager = kzuProjectGetResourceManager(project);

                result = kzuMeshGenerateBoxFreeSize(mesh, resourceManager, width, height, length);
                kzsErrorForward(result);
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_MESH, mesh, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&mesh);
            kzsErrorForward(result);

            if (materialPath != KZ_NULL)
            {
                result = kzuProjectLoaderLoadMaterial(project, materialPath, &material);
                kzsErrorForward(result);

                kzuMeshSetMaterial(mesh, material);

                result = kzuProjectLoaderMeshVerifyAttributes_internal(memoryManager, mesh, kzuBinaryFileInfoGetPath(file), materialPath);
                kzsErrorForward(result);
            }
        }
    }

    *out_mesh = mesh;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadSphere(struct KzuProject* project, kzString path, struct KzuMesh** out_mesh)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuMesh* mesh;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load sphere with null path");

    /* Check if the mesh is already loaded. */
    mesh = (struct KzuMesh*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_MESH, path);

    if (!kzuMeshIsValid(mesh))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_MESH_SPHERE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load mesh file.");

        /* Create mesh. */
        {
            kzFloat radius;
            kzString materialPath;
            struct KzuMaterial* material = KZ_NULL;

            enum KzcResourceMemoryType resourceMemoryType;

            /* Read data */
            {
                enum KzuBinaryGpuMemoryType memoryType;
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                /* Material. */
                result = kzuBinaryDirectoryReadReference(inputStream, file, &materialPath);
                kzsErrorForward(result);

                result = kzcInputStreamReadFloat(inputStream, &radius);
                kzsErrorForward(result);

                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&memoryType);
                kzsErrorForward(result);
                result = kzuProjectLoaderGetMemoryType_private(memoryType, &resourceMemoryType);
                kzsErrorForward(result);

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzuMeshCreate(memoryManager, resourceMemoryType, &mesh);
            kzsErrorForward(result);

            {
                struct KzcResourceManager* resourceManager = kzuProjectGetResourceManager(project);

                result = kzuMeshGenerateSphere(mesh, resourceManager, radius, 32, 32);
                kzsErrorForward(result);
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_MESH, mesh, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&mesh);
            kzsErrorForward(result);

            if (materialPath != KZ_NULL)
            {
                result = kzuProjectLoaderLoadMaterial(project, materialPath, &material);
                kzsErrorForward(result);

                kzuMeshSetMaterial(mesh, material);

                result = kzuProjectLoaderMeshVerifyAttributes_internal(memoryManager, mesh, kzuBinaryFileInfoGetPath(file), materialPath);
                kzsErrorForward(result);
            }
        }
    }

    *out_mesh = mesh;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadPlane(struct KzuProject* project, kzString path, struct KzuMesh** out_mesh)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuMesh* mesh;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load plane with null path");

    /* Check if the mesh is already loaded. */
    mesh = (struct KzuMesh*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_MESH, path);

    if (!kzuMeshIsValid(mesh))
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_MESH_PLANE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load mesh file.");

        /* Create mesh. */
        {
            kzFloat width, height;
            kzBool invertU, invertV;
            kzString materialPath;
            struct KzuMaterial* material = KZ_NULL;
            enum KzuBinaryGpuMemoryType memoryType;
            enum KzcResourceMemoryType resourceMemoryType;

            /* Read data */
            {
                struct KzcInputStream* inputStream;

                result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
                kzsErrorForward(result);

                /* Material. */
                result = kzuBinaryDirectoryReadReference(inputStream, file, &materialPath);
                kzsErrorForward(result);

                result = kzcInputStreamReadFloat(inputStream, &width);
                kzsErrorForward(result);

                result = kzcInputStreamReadFloat(inputStream, &height);
                kzsErrorForward(result);

                /* UV inverts */
                result = kzcInputStreamReadBoolean(inputStream, &invertU);
                kzsErrorForward(result);
                result = kzcInputStreamReadBoolean(inputStream, &invertV);
                kzsErrorForward(result);
                result = kzcInputStreamSkip(inputStream, 2);
                kzsErrorForward(result);

                result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&memoryType);
                kzsErrorForward(result);
                result = kzuProjectLoaderGetMemoryType_private(memoryType, &resourceMemoryType);
                kzsErrorForward(result);

                result = kzcInputStreamDelete(inputStream);
                kzsErrorForward(result);
            }

            result = kzuMeshCreate(memoryManager, resourceMemoryType, &mesh);
            kzsErrorForward(result);

            {
                struct KzcResourceManager* resourceManager = kzuProjectGetResourceManager(project);

                result = kzuMeshGeneratePlane(mesh, resourceManager, width, height, KZU_PLANE_XY, invertU, invertV);
                kzsErrorForward(result);
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_MESH, mesh, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&mesh);
            kzsErrorForward(result);

            if (materialPath != KZ_NULL)
            {
                result = kzuProjectLoaderLoadMaterial(project, materialPath, &material);
                kzsErrorForward(result);

                kzuMeshSetMaterial(mesh, material);

                result = kzuProjectLoaderMeshVerifyAttributes_internal(memoryManager, mesh, kzuBinaryFileInfoGetPath(file), materialPath);
                kzsErrorForward(result);
            }

        }
    }

    *out_mesh = mesh;
    kzsSuccess();
}

kzsException kzuProjectLoaderLoadMeshNode(struct KzuProject* project, kzString path, struct KzuMeshNode** out_meshNode)
{
    kzsException result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuMeshNode* meshNode;
    struct KzuObjectNode* objectNode;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load mesh node with null path");

    /* Check if the object node is already loaded. */
    objectNode = (struct KzuObjectNode*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, path);

    if (kzuObjectNodeIsValid(objectNode))
    {
        meshNode = kzuMeshNodeFromObjectNode(objectNode);
    }
    else
    {
        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsExceptionForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_MESH_NODE, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
                     "Wrong file type encountered while trying to load mesh node file.");

        /* Create mesh node. */
        result = kzuMeshNodeCreate(memoryManager, kzuProjectGetPropertyManager(project), kzuProjectGetMessageDispatcher(project), kzuBinaryFileInfoGetName(file), KZ_NULL, &meshNode);
        kzsErrorForward(result);

        result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, kzuMeshNodeToObjectNode(meshNode),
                                        kzuBinaryFileInfoGetPath(file), (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&objectNode);
        kzsErrorForward(result);

        meshNode = kzuMeshNodeFromObjectNode(objectNode);

        {
            struct KzcInputStream* inputStream;

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            /* Load object node base. */
            result = kzuProjectLoaderLoadObjectNodeBase_private(project, inputStream, file, objectNode);
            kzsErrorForward(result);

            /* Load mesh node specific data. */
            {
                kzString meshPath;
                struct KzuBinaryFileInfo* meshFile;
                struct KzuMesh* mesh;

                result = kzuBinaryDirectoryReadReference(inputStream, file, &meshPath);
                kzsErrorForward(result);

                result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), meshPath, &meshFile);
                kzsErrorForward(result);

                /*lint -e{788} Suppress warning about not listing all possible enum values. */
                switch (kzuBinaryFileInfoGetType(meshFile))
                {
                    case KZU_BINARY_FILE_TYPE_MESH:
                    {
                        result = kzuProjectLoaderLoadMesh(project, meshPath, &mesh);
                        kzsErrorForward(result);
                        break;
                    }

                    case KZU_BINARY_FILE_TYPE_MESH_PLANE:
                    {
                        result = kzuProjectLoaderLoadPlane(project, meshPath, &mesh);
                        kzsErrorForward(result);
                        break;
                    }

                    case KZU_BINARY_FILE_TYPE_MESH_SPHERE:
                    {
                        result = kzuProjectLoaderLoadSphere(project, meshPath, &mesh);
                        kzsErrorForward(result);
                        break;
                    }

                    case KZU_BINARY_FILE_TYPE_MESH_BOX:
                    {
                        result = kzuProjectLoaderLoadBox(project, meshPath, &mesh);
                        kzsErrorForward(result);
                        break;
                    }

                    default:
                    {
                        kzsErrorThrow(KZU_ERROR_WRONG_BINARY_FILE_TYPE, "Wrong binary file type for mesh");
                    }
                }

                kzuMeshNodeSetMesh(meshNode, mesh);
            }

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);
        }
    }

    *out_meshNode = meshNode;
    kzsSuccess();
}
