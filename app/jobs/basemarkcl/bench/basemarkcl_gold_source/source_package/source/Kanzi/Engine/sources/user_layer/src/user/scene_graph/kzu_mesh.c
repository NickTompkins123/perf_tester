/**
* \file
* Mesh node and data. Mesh contains 3D geometry data and material. Node inherited from KzuObjectNode.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_mesh.h"

#include "kzu_object_private.h"
#include "bounding_volume/kzu_bounding_volume.h"

#include <user/material/kzu_material.h>
#include <user/material/kzu_material_type.h>
#include <user/scene_graph/kzu_object.h>
#include <user/renderer/kzu_renderer.h>

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector2.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/resource_manager/index_buffer/kzc_resource_index_buffer.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex_buffer.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/wrappers/kzs_memory.h>
#include <system/debug/kzs_log.h>


/** Object node class for mesh. */
extern const struct KzuObjectNodeClass KZU_MESH_NODE_CLASS;

const KzuObjectType KZU_OBJECT_TYPE_MESH = (void*)&KZU_MESH_NODE_CLASS;


struct KzuMeshNode
{
    struct KzuObjectNode objectNode;            /**< Object node data. */
};

struct KzuMeshNodeData
{
    struct KzuObjectNodeData    objectNodeData; /**< Inherited from object node, that contains transformation and child data. */ /*lint -esym(754, KzuMeshNodeData::objectNodeData) Suppressed as this member is required but not referenced. */ 
    struct KzuMesh*             mesh;           /**< Object data. */
};

struct KzuMeshCluster
{
    struct KzcIndexBuffer* indexBuffer;     /**< Index buffer for this cluster. */
    struct KzuMaterial* material;           /**< Material for this cluster. */
};

struct KzuMeshMorphTargets
{
    struct KzuMesh** morphTargets; /**< Reference to morph targets. */
    kzUint morphTargetCount; /**< Number of morph targets. */
};

struct KzuMesh
{
    struct KzuMeshData* data;
};

struct KzuMeshData
{
    kzBool selfOwned;
    struct KzcVertexBuffer*     vertexBuffer;   /**< Reference to mesh vertex buffer */
    struct KzuBoundingVolume*   boundingVolume; /**< Bounding volume for mesh. */

    struct KzuSkeleton       skeleton;      /**< Reference to skeleton (skinned meshes) */
    struct KzcMatrix4x4      bindShapeMatrix;   /**< Bind shape matrix used for mesh skinning. */

    struct KzuMeshCluster*      clusters;       /**< Single cluster in mesh. */
    kzUint clusterCount;                        /**< Number of clusters in mesh. */

    struct KzuMeshMorphTargets morphTargets; /**< Mesh morph targets. */

    enum KzcResourceMemoryType memoryType;      /**< Holds information where mesh data is stored in. GPU_ONLY by default. */
};


kzsError kzuMeshCreate(const struct KzcMemoryManager* memoryManager, enum KzcResourceMemoryType memoryType, struct KzuMesh** out_mesh)
{
    kzsError result;
    struct KzuMesh* mesh;
    struct KzuMeshData* meshData;

    result = kzcMemoryAllocVariable(memoryManager, mesh, "Mesh");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, meshData, "Mesh data");
    kzsErrorForward(result);

    mesh->data = meshData;
    meshData->memoryType = memoryType;
    meshData->selfOwned = KZ_TRUE;
    meshData->vertexBuffer = KZ_NULL;
    meshData->skeleton.bones = KZ_NULL;
    meshData->boundingVolume = KZ_NULL;
    meshData->clusters = KZ_NULL;
    meshData->clusterCount = 0;
    meshData->bindShapeMatrix = KZC_MATRIX4X4_IDENTITY;

    meshData->morphTargets.morphTargetCount = 0;
    meshData->morphTargets.morphTargets = KZ_NULL;

    *out_mesh = mesh;
    kzsSuccess();
}

static kzsError kzuMeshCreateObjectNodeData_internal(const struct KzcMemoryManager* memoryManager, struct KzuMesh* mesh,
                                                     struct KzuObjectNodeData** out_meshNodeData)
{
    kzsError result;
    struct KzuMeshNodeData* meshNodeData;

    result = kzcMemoryAllocVariable(memoryManager, meshNodeData, "Mesh node data");
    kzsErrorForward(result);

    kzsAssert(mesh == KZ_NULL || kzuMeshIsValid(mesh));

    meshNodeData->mesh = mesh;

    *out_meshNodeData = &meshNodeData->objectNodeData;
    kzsSuccess();
}

kzsError kzuMeshNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                           kzString name, struct KzuMesh* mesh, struct KzuMeshNode** out_meshNode)
{
    kzsError result;
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;
    struct KzuMeshNode* meshNode;

    result = kzuMeshCreateObjectNodeData_internal(memoryManager, mesh, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCreate_private(memoryManager, propertyManager, messageDispatcher, &KZU_MESH_NODE_CLASS, name, objectNodeData, &objectNode);
    kzsErrorForward(result);

    meshNode = (struct KzuMeshNode*)objectNode;

    *out_meshNode = meshNode;
    kzsSuccess();
}

kzsError kzuMeshDelete(struct KzuMesh* mesh)
{
    kzsError result;
    struct KzuMeshData* meshData;
    kzBool selfOwned;
    kzUint i;

    kzsAssert(kzuMeshIsValid(mesh));

    meshData = mesh->data;
    selfOwned = meshData->selfOwned;

    for(i = 0; i < meshData->clusterCount; ++i)
    {
        result = kzcIndexBufferFree(meshData->clusters[i].indexBuffer);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeArray(meshData->clusters);
    kzsErrorForward(result);

    if(meshData->vertexBuffer != KZ_NULL)
    {
        result = kzcVertexBufferFree(meshData->vertexBuffer);
        kzsErrorForward(result);
    }

    if(meshData->boundingVolume != KZ_NULL)
    {
        result = kzuBoundingVolumeDelete(meshData->boundingVolume);
        kzsErrorForward(result);
        meshData->boundingVolume = KZ_NULL;
    }

    if(meshData->skeleton.bones != KZ_NULL)
    {
        result = kzcMemoryFreeArray(meshData->skeleton.bones);
        kzsErrorForward(result);
    }
    if(mesh->data->morphTargets.morphTargets != KZ_NULL)
    {
        result = kzcMemoryFreeArray(mesh->data->morphTargets.morphTargets);
        kzsErrorForward(result);
        mesh->data->morphTargets.morphTargets = KZ_NULL;
    }

    result = kzcMemoryFreeVariable(meshData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(mesh);
        kzsErrorForward(result);
    }
    else
    {
        mesh->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuMeshNodeDelete(struct KzuMeshNode* meshNode)
{
    kzsError result;

    result = kzuObjectNodeDeleteBase_private(&meshNode->objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuMeshTakeOwnership(const struct KzuMesh* mesh)
{
    kzsAssert(kzuMeshIsValid(mesh));

    mesh->data->selfOwned = KZ_FALSE;
}

kzBool kzuMeshIsValid(const struct KzuMesh* mesh)
{
    return mesh != KZ_NULL && mesh->data != KZ_NULL;
}

void kzuMeshTransferData(struct KzuMesh* targetMesh, struct KzuMesh* sourceMesh)
{
    kzsAssert(kzcIsValidPointer(targetMesh));
    kzsAssert(targetMesh->data == KZ_NULL);
    kzsAssert(kzuMeshIsValid(sourceMesh));

    targetMesh->data = sourceMesh->data;
    sourceMesh->data = KZ_NULL;
}

kzsError kzuMeshAssignData(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager, struct KzcVertexList* vertexList,
                           const struct KzuClusterDefinition* clusterDefinitions, kzUint clusterCount,
                           const struct KzcHashMap* vertexAttributeMapping)
{
    kzsError result;
    kzUint i;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzuMeshIsValid(mesh));

    memoryManager = kzcMemoryGetManager(mesh);

    mesh->data->clusterCount = clusterCount;

    result = kzcMemoryAllocArray(memoryManager, mesh->data->clusters, mesh->data->clusterCount, "MeshClusters");
    kzsErrorForward(result);
    
    if(mesh->data->morphTargets.morphTargetCount > 0)
    {
        mesh->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;
    }

    for(i = 0; i < mesh->data->clusterCount; ++i)
    {
        mesh->data->clusters[i].material = KZ_NULL;
        result = kzcIndexBufferCreate(resourceManager, mesh->data->memoryType, clusterDefinitions[i].indexData,
                                      clusterDefinitions[i].indexCount, KZC_PRIMITIVE_TYPE_TRIANGLES,
                                      &mesh->data->clusters[i].indexBuffer);
        kzsErrorForward(result);
    }

    if(mesh->data->morphTargets.morphTargetCount > 0)
    {
        result = kzcVertexBufferCreate(resourceManager, mesh->data->memoryType, KZ_FALSE, KZC_VERTEX_BUFFER_USAGE_DYNAMIC, vertexList, &mesh->data->vertexBuffer);
        kzsErrorForward(result);
    }
    else
    {
        result = kzcVertexBufferCreate(resourceManager, mesh->data->memoryType, KZ_TRUE, KZC_VERTEX_BUFFER_USAGE_STATIC, vertexList, &mesh->data->vertexBuffer);
        kzsErrorForward(result);
    }


    if(mesh->data->boundingVolume == KZ_NULL)
    {
        result = kzuBoundingVolumeCreate(kzcMemoryGetManager(mesh), vertexList, &mesh->data->boundingVolume);
        kzsErrorForward(result);
    }

    /* See if this mesh data contains valid vertex attribute mapping data. Assign attribute mappings for mesh in case. */
    if(vertexAttributeMapping != KZ_NULL)
    {
        struct KzcHashMapIterator materialTypeIterator = kzcHashMapGetIterator(vertexAttributeMapping);

        while(kzcHashMapIterate(materialTypeIterator))
        {
            struct KzuMaterialType* materialType = (struct KzuMaterialType*)kzcHashMapIteratorGetKey(materialTypeIterator);
            struct KzcHashMap* vertexAttributeMappings = (struct KzcHashMap*)kzcHashMapIteratorGetValue(materialTypeIterator);
            struct KzcDynamicArray* shaderAttributes = KZ_NULL;

            if(kzuMaterialTypeIsValid(materialType))
            {
                shaderAttributes = kzuMaterialTypeGetVertexAttributes(materialType);
            }

            result = kzcVertexBufferAssignAttributeMapping(mesh->data->vertexBuffer, materialType, vertexAttributeMappings,
                                                           shaderAttributes);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzuMeshCopy(const struct KzcMemoryManager* memoryManager, const struct KzuMesh* sourceMesh, struct KzuMesh** out_targetMesh)
{
    kzsError result;
    struct KzuMesh* mesh;
    kzUint i;

    result = kzuMeshCreate(memoryManager, sourceMesh->data->memoryType, &mesh);
    kzsErrorForward(result);

    /* Copy bones. */
    if(sourceMesh->data->skeleton.bones != KZ_NULL && kzcArrayLength(sourceMesh->data->skeleton.bones) > 0)
    {
        struct KzuObjectNode** objectNodes;
        struct KzcMatrix4x4* invBindMatrices;
        result = kzcMemoryAllocArray(memoryManager, objectNodes, kzcArrayLength(sourceMesh->data->skeleton.bones), "ObjectNodes");
        kzsErrorForward(result);
        result = kzcMemoryAllocArray(memoryManager, invBindMatrices, kzcArrayLength(sourceMesh->data->skeleton.bones), "InvBindMatrices");
        kzsErrorForward(result);
        for(i = 0; i < kzcArrayLength(sourceMesh->data->skeleton.bones); ++i)
        {
            objectNodes[i] = sourceMesh->data->skeleton.bones[i].bone;
            invBindMatrices[i] = sourceMesh->data->skeleton.bones[i].invBindMatrix;
        }

        result = kzuMeshAssignBoneData(mesh, kzcArrayLength(sourceMesh->data->skeleton.bones), objectNodes, invBindMatrices);
        kzsErrorForward(result);
    }

    result = kzuBoundingVolumeCopy(memoryManager, sourceMesh->data->boundingVolume, &mesh->data->boundingVolume);
    kzsErrorForward(result);

    mesh->data->clusterCount = sourceMesh->data->clusterCount;

    result = kzcVertexBufferCopy(sourceMesh->data->vertexBuffer, &mesh->data->vertexBuffer);
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, mesh->data->clusters, mesh->data->clusterCount, "MeshCopyClusters");
    kzsErrorForward(result);
    for(i = 0; i < mesh->data->clusterCount; ++i)
    {
        mesh->data->clusters[i].material = sourceMesh->data->clusters[i].material;
        result = kzcIndexBufferCopy(sourceMesh->data->clusters[i].indexBuffer, &mesh->data->clusters[i].indexBuffer);
        kzsErrorForward(result);
    }

    *out_targetMesh = mesh;
    kzsSuccess();
}

kzsError kzuMeshSetMorphTargets(const struct KzuMesh* mesh, kzUint numMorphTargets, struct KzuMesh* const* morphTargets)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(mesh));

    mesh->data->morphTargets.morphTargetCount = numMorphTargets;
    if(numMorphTargets > 0)
    {
        kzUint i;
        if(mesh->data->morphTargets.morphTargets != KZ_NULL)
        {
            result = kzcMemoryFreeArray(mesh->data->morphTargets.morphTargets);
            kzsErrorForward(result);
        }
        result = kzcMemoryAllocArray(kzcMemoryGetManager(mesh), mesh->data->morphTargets.morphTargets, numMorphTargets, "MorphTargets");
        kzsErrorForward(result);
        for(i = 0; i < numMorphTargets; ++i)
        {
            mesh->data->morphTargets.morphTargets[i] = morphTargets[i];
        }
    }

    kzsSuccess();
}

void kzuMeshGetMorphTargets(const struct KzuMesh* mesh, kzUint* out_numMorphTargets, struct KzuMesh*** out_morphTargets)
{
    kzsAssert(kzcIsValidPointer(mesh));

    *out_numMorphTargets = mesh->data->morphTargets.morphTargetCount;
    *out_morphTargets = mesh->data->morphTargets.morphTargets;
}

kzsError kzuMeshInterpolateMorphTargets(const struct KzcMemoryManager* memoryManager, const struct KzuMesh* mesh, void** out_positions,
                                        void** out_normals, void** out_tangents)
{
    kzsError result;
    kzFloat* basePositions = KZ_NULL;
    kzFloat* baseNormals = KZ_NULL;
    kzFloat* baseTangents = KZ_NULL;

    /* TODO: Not finished. Fetch weights from properties. */
    /* Interpolate positions, normals and tangents. */
    if(mesh->data->memoryType == KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM && mesh->data->morphTargets.morphTargetCount >= 1)
    {
        kzUint i, j;
        kzFloat weights[5] = {0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
        kzFloat** vertices;
        kzFloat** normals;
        kzFloat** tangents;
        kzUint elementCount = kzcVertexBufferGetVertexCount(mesh->data->vertexBuffer) * 3;

        result = kzcMemoryAllocArray(memoryManager, vertices, mesh->data->morphTargets.morphTargetCount, "MorphTargets");
        kzsErrorForward(result);
        result = kzcMemoryAllocArray(memoryManager, normals, mesh->data->morphTargets.morphTargetCount, "MorphTargets");
        kzsErrorForward(result);
        result = kzcMemoryAllocArray(memoryManager, tangents, mesh->data->morphTargets.morphTargetCount, "MorphTargets");
        kzsErrorForward(result);

        for(i = 0; i < mesh->data->morphTargets.morphTargetCount + 1; ++i)
        {
            if(weights[i] > 0.0f || i == 0)
            {
                if(i == 0)
                {
                    result = kzcVertexBufferGetSemanticData(memoryManager, mesh->data->vertexBuffer, KZC_VERTEX_ATTRIBUTE_POSITION,
                        (void**)&basePositions);
                    kzsErrorForward(result);

                    result = kzcVertexBufferGetSemanticData(memoryManager, mesh->data->vertexBuffer, KZC_VERTEX_ATTRIBUTE_NORMAL,
                        (void**)&baseNormals);
                    kzsErrorForward(result);

                    result = kzcVertexBufferGetSemanticData(memoryManager, mesh->data->vertexBuffer, KZC_VERTEX_ATTRIBUTE_TANGENT,
                        (void**)&baseTangents);
                    kzsErrorForward(result);
                }
                else
                {
                    struct KzuMesh* morphMesh = mesh->data->morphTargets.morphTargets[i - 1];
                    result = kzcVertexBufferGetSemanticData(memoryManager, morphMesh->data->vertexBuffer, KZC_VERTEX_ATTRIBUTE_POSITION,
                        (void**)&vertices[i - 1]);
                    kzsErrorForward(result);
                    result = kzcVertexBufferGetSemanticData(memoryManager, morphMesh->data->vertexBuffer, KZC_VERTEX_ATTRIBUTE_NORMAL,
                        (void**)&normals[i - 1]);
                    kzsErrorForward(result);
                    result = kzcVertexBufferGetSemanticData(memoryManager, morphMesh->data->vertexBuffer, KZC_VERTEX_ATTRIBUTE_TANGENT,
                        (void**)&tangents[i - 1]);
                    kzsErrorForward(result);
                }

                kzsAssert(basePositions != KZ_NULL);

                if(i == 0)
                {
                    for(j = 0; j < elementCount; ++j)
                    {
                        basePositions[j] *= weights[i];
                    }
                    if(baseNormals != KZ_NULL)
                    {
                        for(j = 0; j < elementCount; ++j)
                        {
                            baseNormals[j] *= weights[i];
                        }
                    }
                    if(baseTangents != KZ_NULL)
                    {
                        for(j = 0; j < elementCount; ++j)
                        {
                            baseTangents[j] *= weights[i];
                        }
                    }
                }
                else if(weights[i] > 0.0f)
                {
                    kzsAssert(vertices[i] != KZ_NULL);
                    
                    for(j = 0; j < elementCount; ++j)
                    {
                        basePositions[j] += vertices[i - 1][j] * weights[i];
                    }
                    if(baseNormals != KZ_NULL)
                    {
                        kzsAssert(normals[i] != KZ_NULL);
                        for(j = 0; j < elementCount; ++j)
                        {
                            baseNormals[j] += normals[i - 1][j] * weights[i];
                        }
                    }
                    if(baseTangents != KZ_NULL)
                    {
                        kzsAssert(tangents[i] != KZ_NULL);
                        for(j = 0; j < elementCount; ++j)
                        {
                            baseTangents[j] += tangents[i - 1][j] * weights[i];
                        }
                    }
                }
                if(i > 0)
                {
                    if(vertices[i - 1] != KZ_NULL)
                    {
                        result = kzcMemoryFreePointer(vertices[i - 1]);
                        kzsErrorForward(result);
                    }
                    if(normals[i - 1] != KZ_NULL)
                    {
                        result = kzcMemoryFreePointer(normals[i - 1]);
                        kzsErrorForward(result);
                    }
                    if(tangents[i - 1] != KZ_NULL)
                    {
                        result = kzcMemoryFreePointer(tangents[i - 1]);
                        kzsErrorForward(result);
                    }
                }
            }
        }
        
        result = kzcMemoryFreeArray(vertices);
        kzsErrorForward(result);
        result = kzcMemoryFreeArray(normals);
        kzsErrorForward(result);
        result = kzcMemoryFreeArray(tangents);
        kzsErrorForward(result);
    }

    *out_positions = basePositions;
    *out_normals = baseNormals;
    *out_tangents = baseTangents;
    kzsSuccess();
}

void kzuMeshNodeSetMesh(const struct KzuMeshNode* meshNode, struct KzuMesh* mesh)
{
    kzsAssert(kzcIsValidPointer(meshNode));
    ((struct KzuMeshNodeData*)meshNode->objectNode.data)->mesh = mesh;
}

struct KzuMesh* kzuMeshNodeGetMesh(const struct KzuMeshNode* meshNode)
{
    struct KzuMesh* mesh;
    kzsAssert(kzcIsValidPointer(meshNode));

    mesh = ((struct KzuMeshNodeData*)meshNode->objectNode.data)->mesh;
    return mesh;
}

struct KzuMeshNode* kzuMeshNodeFromObjectNode(const struct KzuObjectNode* objectNode)
{
    struct KzuMeshNode* meshNode;

    kzsAssert(kzcIsValidPointer(objectNode));
    kzsAssert(objectNode->data->objectNodeClass == &KZU_MESH_NODE_CLASS);

    meshNode = (struct KzuMeshNode*)objectNode;
    return meshNode;
}

struct KzuObjectNode* kzuMeshNodeToObjectNode(struct KzuMeshNode* meshNode)
{
    return (struct KzuObjectNode*)meshNode;
}

struct KzcVertexBuffer* kzuMeshGetVertexBuffer(const struct KzuMesh* mesh)
{
    kzsAssert(kzuMeshIsValid(mesh));
    return mesh->data->vertexBuffer;
}

struct KzcIndexBuffer* kzuMeshGetIndexBuffer(const struct KzuMesh* mesh)
{
    kzsAssert(kzuMeshIsValid(mesh));
    kzsAssert(mesh->data->clusterCount > 0);
    return mesh->data->clusters[0].indexBuffer;
}

struct KzcIndexBuffer* kzuMeshClusterGetIndexBuffer(const struct KzuMeshCluster* meshCluster)
{
    /* kzsAssert(kzcIsValidPointer(meshCluster)); */
    return meshCluster->indexBuffer;
}

void kzuMeshSetBoundingVolume(const struct KzuMesh* mesh, struct KzuBoundingVolume* boundingVolume)
{
    kzsAssert(kzuMeshIsValid(mesh));
    mesh->data->boundingVolume = boundingVolume;
}

struct KzuBoundingVolume* kzuMeshGetBoundingVolume(const struct KzuMesh* mesh)
{
    kzsAssert(kzuMeshIsValid(mesh));
    return mesh->data->boundingVolume;
}

kzsError kzuMeshAssignBoneData(const struct KzuMesh* mesh, kzUint boneCount, struct KzuObjectNode* const* bones,
                               const struct KzcMatrix4x4* invBindMatrices)
{
    kzsError result;
    kzUint i;

    kzsAssert(kzcIsValidPointer(mesh));
    kzsAssert(boneCount < 100);

    result = kzcMemoryAllocArray(kzcMemoryGetManager(mesh), mesh->data->skeleton.bones, boneCount, "MeshBones");
    kzsErrorForward(result);

    for(i = 0; i < boneCount; ++i)
    {
        mesh->data->skeleton.bones[i].bone = bones[i];
        mesh->data->skeleton.bones[i].invBindMatrix = invBindMatrices[i];
        mesh->data->skeleton.bones[i].childJoint = KZ_NULL;
    }

    kzsAssert(kzcArrayLength(mesh->data->skeleton.bones) < 100); /* TODO: Why is this check here? */

    for(i = 0; i < kzcArrayLength(mesh->data->skeleton.bones); ++i)
    {
        struct KzcDynamicArrayIterator it;
        it = kzuObjectNodeGetChildren(mesh->data->skeleton.bones[i].bone);
        while(kzcDynamicArrayIterate(it))
        {
            kzUint j;
            struct KzuObjectNode* childObjectNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);

            for(j = 0; j < boneCount; ++j)
            {
                if(childObjectNode == mesh->data->skeleton.bones[j].bone)
                {
                    mesh->data->skeleton.bones[i].childJoint = childObjectNode;
                    break;
                }
            }
        }
    }

    kzsSuccess();
}

struct KzuMeshBone* kzuMeshGetSkeletonBones(const struct KzuMesh* mesh)
{
    kzsAssert(kzcIsValidPointer(mesh));
    return mesh->data->skeleton.bones;
}

struct KzuSkeleton* kzuMeshGetSkeleton(const struct KzuMesh* mesh)
{
    kzsAssert(kzcIsValidPointer(mesh));
    return &mesh->data->skeleton;
}

void kzuMeshSetBindShapeMatrix(const struct KzuMesh* mesh, const struct KzcMatrix4x4* bindShapeMatrix)
{
    kzsAssert(kzcIsValidPointer(mesh));
    mesh->data->bindShapeMatrix = *bindShapeMatrix;
}

struct KzcMatrix4x4 kzuMeshGetBindShapeMatrix(const struct KzuMesh* mesh)
{
    kzsAssert(kzcIsValidPointer(mesh));
    return mesh->data->bindShapeMatrix;
}

kzsError kzuMeshGeneratePlaneWithOffset(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager,
                                        kzFloat xOffset, kzFloat yOffset, kzFloat width, kzFloat height, enum KzuPlaneAxes planeAxes, 
                                        kzBool invertU, kzBool invertV)
{
    kzsError result;
    kzUint vertexCount = 4;
    kzUint indexCount =  6;
    kzU16* indices;
    struct KzcVector3* vertexPositions;
    struct KzcVector2* vertexUV;
    struct KzcVector3* vertexNormal;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(mesh);
    struct KzcVertexList* vertexAttributes;

    result = kzcMemoryAllocPointer(memoryManager, &indices, indexCount * sizeof(*indices), "Indices");
    kzsErrorForward(result);

    result = kzcVertexListCreate(memoryManager, &vertexAttributes);
    kzsErrorForward(result);

    /* Create vertex position list for mesh. */
    result = kzcMemoryAllocPointer(memoryManager, &vertexPositions, vertexCount * sizeof(*vertexPositions), "VertexPos");
    kzsErrorForward(result);
    result = kzcMemoryAllocPointer(memoryManager, &vertexUV, vertexCount * sizeof(*vertexUV), "VertexUV");
    kzsErrorForward(result);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    /* See plane's orientation and generate data according to it. */
    {
        kzFloat xp1 = planeAxes == KZU_PLANE_YZ ? 0.0f : width + xOffset;
        kzFloat xp2 = planeAxes == KZU_PLANE_YZ ? 0.0f : -width + xOffset;
        kzFloat yp1 = planeAxes == KZU_PLANE_XZ ? 0.0f : height + yOffset;
        kzFloat yp2 = planeAxes == KZU_PLANE_XZ ? 0.0f : -height + yOffset;
        kzFloat zp1 = 0.0f;
        kzFloat zp2 = 0.0f;
        kzFloat zp3 = 0.0f;
        kzFloat zp4 = 0.0f;

        if(planeAxes == KZU_PLANE_XZ)
        {
            zp1 = -height + yOffset;
            zp2 = -height + yOffset;
            zp3 = height + yOffset;
            zp4 = height + yOffset;
        }
        else if(planeAxes == KZU_PLANE_YZ)
        {
            zp1 = width + xOffset;
            zp2 = -width + xOffset;
            zp3 = -width + xOffset;
            zp4 = width + xOffset;
        }

        kzcVector3Set(&vertexPositions[0], xp1, yp1, zp1);
        kzcVector3Set(&vertexPositions[1], xp2, yp1, zp2);
        kzcVector3Set(&vertexPositions[2], xp2, yp2, zp3);
        kzcVector3Set(&vertexPositions[3], xp1, yp2, zp4);
    }

    kzcVector2Set(&vertexUV[0], 0.0f, 0.0f);
    kzcVector2Set(&vertexUV[1], 1.0f, 0.0f);
    kzcVector2Set(&vertexUV[2], 1.0f, 1.0f);
    kzcVector2Set(&vertexUV[3], 0.0f, 1.0f);


    /* Invert texture coordinates. */
    {
        kzUint i;
        for(i = 0; i < 4; ++i)
        {
            if(invertU)
            {
                kzcVector2SetX(&vertexUV[i], 1.0f - kzcVector2GetX(&vertexUV[i]));
            }
            if(invertV)
            {
                kzcVector2SetY(&vertexUV[i], 1.0f - kzcVector2GetY(&vertexUV[i]));
            }
        }
    }

    result = kzcVertexListAddAttribute(vertexAttributes, "kzPosition", KZC_VERTEX_ATTRIBUTE_POSITION, 
        KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32, sizeof(kzFloat), 3);
    kzsErrorForward(result);
    result = kzcVertexListAddAttribute(vertexAttributes, "kzTextureCoordinate0", KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE, 
        KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32, sizeof(kzFloat), 2);
    kzsErrorForward(result);

    result = kzuMeshCalculateNormals(memoryManager, indices, indexCount, (kzFloat*)vertexPositions->data, indexCount, &vertexNormal);
    kzsErrorForward(result);
    result = kzcVertexListAddAttribute(vertexAttributes, "kzNormal", KZC_VERTEX_ATTRIBUTE_NORMAL, 
        KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32, sizeof(kzFloat), 3);
    kzsErrorForward(result);

    /* Rearrange data. */
    {
        void* data;
        void* originalData;
        struct KzuClusterDefinition cluster[1];

        result = kzcMemoryAllocPointer(memoryManager, &data, (sizeof(*vertexPositions) + sizeof(*vertexUV) + sizeof(*vertexNormal)) * vertexCount, "CombinedVertexData");
        kzsErrorForward(result);

        originalData = data;
        kzsMemcpy(data, vertexPositions, sizeof(*vertexPositions) * vertexCount);
        data = (kzU8*)data + sizeof(*vertexPositions) * vertexCount;
        kzsMemcpy(data, vertexUV, sizeof(*vertexUV) * vertexCount);
        data = (kzU8*)data + sizeof(*vertexUV) * vertexCount;
        kzsMemcpy(data, vertexNormal, sizeof(*vertexNormal) * vertexCount);
        data = originalData;

        result = kzcVertexListSetData(vertexAttributes, data, vertexCount);
        kzsErrorForward(result);

        cluster[0].indexCount = indexCount;
        cluster[0].indexData = indices;

        result = kzuMeshAssignData(mesh, resourceManager, vertexAttributes, cluster, 1, KZ_NULL);
        kzsErrorForward(result);

        result = kzcMemoryFreePointer(data);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreePointer(vertexPositions);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(vertexUV);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(vertexNormal);
    kzsErrorForward(result);
    result = kzcVertexListDelete(vertexAttributes);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(indices);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuMeshGeneratePlane(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager, kzFloat width, kzFloat height,
                              enum KzuPlaneAxes planeAxes, kzBool invertU, kzBool invertV)
{
    kzsError result;

    result = kzuMeshGeneratePlaneWithOffset(mesh, resourceManager, 0.0f, 0.0f, width, height, planeAxes, invertU, invertV);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuMeshGenerateSphere(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager,
                               kzFloat radius, const kzUint segmentCount, const kzUint rowCount)
{
    kzsError result;
    kzUint i, j, offset;
    kzUint vertexCount = segmentCount * rowCount;
    /* Sphere has segment count amount of faces per segment, and one quad is 2 triangles, one triangle consists of 3 indices. */
    kzUint indexCount = (segmentCount) * (rowCount - 2) * 2 * 3;

    kzU16* indices;
    struct KzcVector3* vertexPosition;
    struct KzcVector3* vertexNormal;
    struct KzcVector2* vertexUV;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(mesh);
    struct KzcVertexList* vertexList;

    kzsAssert(kzuMeshIsValid(mesh));
    
    if(segmentCount < 3 || rowCount < 2)
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Sphere should have at least 3 segments and 2 rows!");
    }
    
    /* Create index buffer for mesh. */
    result = kzcMemoryAllocPointer(memoryManager, &indices, indexCount * sizeof(*indices), "Indices");
    kzsErrorForward(result);

    result = kzcVertexListCreate(memoryManager, &vertexList);
    kzsErrorForward(result);

    offset = 0;

    /* Generate vertex indices for sphere. */
    for(j = 0; j < rowCount - 1; ++j)
    {
        for(i = 0; i < segmentCount; ++i)
        {
            /* Assign 2 faces per loop. */
            kzU16 vertex1 = (kzU16)(i + (j * segmentCount));
            kzU16 vertex2 = (kzU16)(((i + 1) % segmentCount) + (j * segmentCount));
            kzU16 vertex3 = (kzU16)(((i + 1) % segmentCount) + ((j + 1) * segmentCount));
            kzU16 vertex4 = (kzU16)(i + ((j + 1) * segmentCount));
            if(j == 0 || j == rowCount - 2)
            {
                if(j == 0)
                {
                    vertex1 = 0;
                    indices[offset++] = vertex1;
                    indices[offset++] = vertex3;
                    indices[offset++] = vertex4;
                }
                else
                {
                    vertex3 = (kzU16)(vertexCount - 1);
                    indices[offset++] = vertex1;
                    indices[offset++] = vertex2;
                    indices[offset++] = vertex3;
                }
            }
            else
            {
                indices[offset++] = vertex1;
                indices[offset++] = vertex2;
                indices[offset++] = vertex4;
                indices[offset++] = vertex2;
                indices[offset++] = vertex3;
                indices[offset++] = vertex4;
            }
        }
    }

    /* Create vertex position list for mesh. */
    result = kzcMemoryAllocPointer(memoryManager, &vertexPosition, vertexCount * sizeof(*vertexPosition), "VertexPos");
    kzsErrorForward(result);
    /* Create vertex UV list for mesh. */
    result = kzcMemoryAllocPointer(memoryManager, &vertexUV, vertexCount * sizeof(*vertexUV), "VertexUV");
    kzsErrorForward(result);

    /* Generate vertices for sphere. */
    for(j = 0; j < rowCount; ++j)
    {
        for(i = 0; i < segmentCount; ++i)
        {
            kzFloat y = radius - (kzFloat)j * radius * 2.0f / (kzFloat)(rowCount - 1);
            kzFloat innerRadius = kzsSqrtf(radius * radius - y * y);    /* Pythagoras */
            kzFloat x = kzsSinf(i * KZS_PI * 2.0f / (kzFloat)segmentCount) * innerRadius;
            kzFloat z = -kzsCosf(i * KZS_PI * 2.0f / (kzFloat)segmentCount) * innerRadius;
            struct KzcVector3 sphericalCoordinate;

            kzcVector3Set(&vertexPosition[j * segmentCount + i], x, y, z);

            sphericalCoordinate = kzcVector3(x, y, z);
            kzcVector3Normalize(&sphericalCoordinate);

            kzcVector2Set(&vertexUV[j * segmentCount + i],
                          kzcVector3GetX(&sphericalCoordinate) * 0.5f + 0.5f,
                          -kzcVector3GetY(&sphericalCoordinate) * 0.5f + 0.5f);
        }
    }

    {
        kzUint elementSize;
        enum KzcVertexAttributeDataType dataType;

        elementSize = 4;
        dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32;

        result = kzcVertexListAddAttribute(vertexList, "kzPosition", KZC_VERTEX_ATTRIBUTE_POSITION, 
            dataType, elementSize, 3);
        kzsErrorForward(result);

        result = kzcVertexListAddAttribute(vertexList, "kzTextureCoordinate0", KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE, 
            dataType, elementSize, 2);
        kzsErrorForward(result);

        /* Calculate normals for mesh and apply to attributes. */
        result = kzuMeshCalculateNormals(kzcMemoryGetManager(mesh), indices, indexCount, vertexPosition->data, vertexCount, &vertexNormal);
        kzsErrorForward(result);
        result = kzcVertexListAddAttribute(vertexList, "kzNormal", KZC_VERTEX_ATTRIBUTE_NORMAL, dataType,
            elementSize, 3);
        kzsErrorForward(result);
    }

    {
        void* data;
        void* originalData;
        struct KzuClusterDefinition clusters[1];

        result = kzcMemoryAllocPointer(memoryManager, &data, (sizeof(*vertexPosition) + sizeof(*vertexUV) + sizeof(*vertexNormal)) * vertexCount, "CombinedVertexData");
        kzsErrorForward(result);

        originalData = data;

        kzsMemcpy(data, vertexPosition, sizeof(*vertexPosition) * vertexCount);
        data = (kzU8*)data + sizeof(*vertexPosition) * vertexCount;
        kzsMemcpy(data, vertexUV, sizeof(*vertexUV) * vertexCount);
        data = (kzU8*)data + sizeof(*vertexUV) * vertexCount;
        kzsMemcpy(data, vertexNormal, sizeof(*vertexNormal) * vertexCount);
        data = originalData;

        result = kzcVertexListSetData(vertexList, data, vertexCount);
        kzsErrorForward(result);

        clusters[0].indexData = indices;
        clusters[0].indexCount = indexCount;

        result = kzuMeshAssignData(mesh, resourceManager, vertexList, clusters, 1, KZ_NULL);
        kzsErrorForward(result);

        result = kzcMemoryFreePointer(data);
        kzsErrorForward(result);
    }

    /* After the data has been deployed to GPU, no need to keep in memory. */
    result = kzcMemoryFreePointer(vertexPosition);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(vertexNormal);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(vertexUV);
    kzsErrorForward(result);
    result = kzcVertexListDelete(vertexList);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(indices);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuMeshGenerateMorphSphere(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager, kzFloat radius, const kzUint numSegments, const kzUint numRows)
{
    kzsError result;
    kzUint i, j, offset;
    kzUint vertexCount = numSegments * numRows;
    /* Sphere has segment count amount of faces per segment, and one quad is 2 triangles, one triangle consists of 3 indices. */
    kzUint indexCount = (numSegments) * (numRows - 2) * 2 * 3;

    kzU16* indices;
    struct KzcVector3* vertexPosition;
    struct KzcVector3* vertexNormal;
    struct KzcVector2* vertexUV;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(mesh);
    struct KzcVertexList* vertexList;

    mesh->data->memoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM;

    kzsAssert(kzuMeshIsValid(mesh));

    if(numSegments < 3 || numRows < 2)
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Sphere should have at least 3 segments and 2 rows!");
    }

    /* Create index buffer for mesh. */
    result = kzcMemoryAllocPointer(memoryManager, &indices, indexCount * sizeof(*indices), "Indices");
    kzsErrorForward(result);

    result = kzcVertexListCreate(memoryManager, &vertexList);
    kzsErrorForward(result);

    offset = 0;

    /* Generate vertex indices for sphere. */
    for(j = 0; j < numRows - 1; ++j)
    {
        for(i = 0; i < numSegments; ++i)
        {
            /* Assign 2 faces per loop. */
            kzU16 vertex1 = (kzU16)(i + (j * numSegments));
            kzU16 vertex2 = (kzU16)(((i + 1) % numSegments) + (j * numSegments));
            kzU16 vertex3 = (kzU16)(((i + 1) % numSegments) + ((j + 1) * numSegments));
            kzU16 vertex4 = (kzU16)(i + ((j + 1) * numSegments));
            if(j == 0 || j == numRows - 2)
            {
                if(j == 0)
                {
                    vertex1 = 0;
                    indices[offset++] = vertex1;
                    indices[offset++] = vertex3;
                    indices[offset++] = vertex4;
                }
                else
                {
                    vertex3 = (kzU16)(vertexCount - 1);
                    indices[offset++] = vertex1;
                    indices[offset++] = vertex2;
                    indices[offset++] = vertex3;
                }
            }
            else
            {
                indices[offset++] = vertex1;
                indices[offset++] = vertex2;
                indices[offset++] = vertex4;
                indices[offset++] = vertex2;
                indices[offset++] = vertex3;
                indices[offset++] = vertex4;
            }
        }
    }

    /* Create vertex position list for mesh. */
    result = kzcMemoryAllocPointer(memoryManager, &vertexPosition, vertexCount * sizeof(*vertexPosition), "VertexPos");
    kzsErrorForward(result);
    /* Create vertex UV list for mesh. */
    result = kzcMemoryAllocPointer(memoryManager, &vertexUV, vertexCount * sizeof(*vertexUV), "VertexUV");
    kzsErrorForward(result);

    /* Generate vertices for sphere. */
    for(j = 0; j < numRows; ++j)
    {
        for(i = 0; i < numSegments; ++i)
        {
            kzFloat y = radius - (kzFloat)j * radius * 2.0f / (kzFloat)(numRows - 1);
            kzFloat innerRadius = kzsSqrtf(radius * radius - y * y);    /* Pythagoras */
            kzFloat x = kzsSinf(i * KZS_PI * 2.0f / (kzFloat)numSegments) * innerRadius;
            kzFloat z = -kzsCosf(i * KZS_PI * 2.0f / (kzFloat)numSegments) * innerRadius;
            struct KzcVector3 sphericalCoordinate;
            
            if(x > 0.0f)
            {
                x *= 0.01f;
            }
            if(y > 0.0f)
            {
                y *= 0.01f;
            }
            if(z > 0.0f)
            {
                z *= 0.01f;
            }

            kzcVector3Set(&vertexPosition[j * numSegments + i], x, y, z);

            sphericalCoordinate = kzcVector3(x, y, z);
            kzcVector3Normalize(&sphericalCoordinate);

            kzcVector2Set(&vertexUV[j * numSegments + i],
                kzcVector3GetX(&sphericalCoordinate) * 0.5f + 0.5f,
                -kzcVector3GetY(&sphericalCoordinate) * 0.5f + 0.5f);
        }
    }

    {
        kzUint elementSize;
        enum KzcVertexAttributeDataType dataType;

        elementSize = 4;
        dataType = KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32;

        result = kzcVertexListAddAttribute(vertexList, "kzPosition", KZC_VERTEX_ATTRIBUTE_POSITION, 
            dataType, elementSize, 3);
        kzsErrorForward(result);

        result = kzcVertexListAddAttribute(vertexList, "kzTextureCoordinate0", KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE, 
            dataType, elementSize, 2);
        kzsErrorForward(result);

        /* Calculate normals for mesh and apply to attributes. */
        result = kzuMeshCalculateNormals(kzcMemoryGetManager(mesh), indices, indexCount, vertexPosition->data, vertexCount, &vertexNormal);
        kzsErrorForward(result);
        result = kzcVertexListAddAttribute(vertexList, "kzNormal", KZC_VERTEX_ATTRIBUTE_NORMAL, dataType,
            elementSize, 3);
        kzsErrorForward(result);
    }

    
    {
        struct KzuMesh* meshArray[1];

        result = kzuMeshCreate(memoryManager, KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM, &meshArray[0]);
        kzsErrorForward(result);
        result = kzuMeshGenerateSphere(meshArray[0], resourceManager, radius, numSegments, numRows);
        kzsErrorForward(result);
        result = kzuMeshSetMorphTargets(mesh, 1, meshArray);
        kzsErrorForward(result);
    }
    

    {
        void* data;
        void* originalData;
        struct KzuClusterDefinition clusters[1];

        result = kzcMemoryAllocPointer(memoryManager, &data, (sizeof(*vertexPosition) + sizeof(*vertexUV) + sizeof(*vertexNormal)) * vertexCount, "CombinedVertexData");
        kzsErrorForward(result);

        originalData = data;

        kzsMemcpy(data, vertexPosition, sizeof(*vertexPosition) * vertexCount);
        data = (kzU8*)data + sizeof(*vertexPosition) * vertexCount;
        kzsMemcpy(data, vertexUV, sizeof(*vertexUV) * vertexCount);
        data = (kzU8*)data + sizeof(*vertexUV) * vertexCount;
        kzsMemcpy(data, vertexNormal, sizeof(*vertexNormal) * vertexCount);
        data = originalData;

        result = kzcVertexListSetData(vertexList, data, vertexCount);
        kzsErrorForward(result);

        clusters[0].indexData = indices;
        clusters[0].indexCount = indexCount;

        result = kzuMeshAssignData(mesh, resourceManager, vertexList, clusters, 1, KZ_NULL);
        kzsErrorForward(result);

        result = kzcMemoryFreePointer(data);
        kzsErrorForward(result);
    }

    /* After the data has been deployed to GPU, no need to keep in memory. */
    result = kzcMemoryFreePointer(vertexPosition);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(vertexNormal);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(vertexUV);
    kzsErrorForward(result);
    result = kzcVertexListDelete(vertexList);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(indices);
    kzsErrorForward(result);

    kzsSuccess();

}

kzsError kzuMeshGenerateBoxFreeSize(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager,
                                    kzFloat sizeX, kzFloat sizeY, kzFloat sizeZ)
{
    kzUint i, j, k;
    kzsError result;
    const kzUint faceCount = 12;
    const kzUint indexCount = faceCount * 3;
    kzFloat x, y, z;
    struct KzcVector3* vertexPosition;
    struct KzcVector3* vertexNormal;
    struct KzcVector4* vertexColor;
    struct KzcVector2* vertexUV;
    kzU16* indices;
    struct KzcVertexList* vertexList;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(mesh);

    /* Positions of the cube corners */
    const kzFloat verticesPositions[] = 
    {
        1.f, 1.f, 1.f,  
        -1.f, 1.f, 1.f,  
        -1.f,-1.f, 1.f,  
        1.f,-1.f, 1.f,
        1.f,-1.f,-1.f,  
        1.f, 1.f,-1.f,  
        -1.f, 1.f,-1.f,  
        -1.f,-1.f,-1.f
    };

    /* UV mapping for one face */
    const kzFloat UVs[] =
    {
        0.f, 1.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        1.f, 0.f,
        0.f, 0.f
    };

    /* VerticePositions indices for Faces */
    const kzUint faceIndices[]= 
    {
        0, 1, 2,
        0, 2, 3,
        5, 0, 3,
        5, 3, 4,
        6, 5, 4,
        6, 4, 7,
        1, 6, 7,
        1, 7, 2,
        5, 6, 1,
        5, 1, 0,
        3, 2, 7,
        3, 7, 4
    };

    kzsAssert(kzuMeshIsValid(mesh));

    result = kzcMemoryAllocPointer(memoryManager, &indices, indexCount * sizeof(*indices), "Indices");
    kzsErrorForward(result);

    result = kzcVertexListCreate(memoryManager, &vertexList);
    kzsErrorForward(result);

    /* fill indices */
    for(j = 0; j < indexCount; ++j)
    {
        indices[j] = (kzU16)j;
    }

    result = kzcMemoryAllocPointer(memoryManager, &vertexPosition, indexCount * sizeof(*vertexPosition), "VertexPos");
    kzsErrorForward(result);

    result = kzcMemoryAllocPointer(memoryManager, &vertexColor, indexCount * sizeof(*vertexColor), "VertexColor");
    kzsErrorForward(result);

    result = kzcMemoryAllocPointer(memoryManager, &vertexUV, indexCount * sizeof(*vertexUV), "VertexUV");
    kzsErrorForward(result);

    for(j = 0; j < indexCount; ++j)
    {
        i = (kzUint)faceIndices[j];
        x = verticesPositions[i * 3];
        y = verticesPositions[i * 3 + 1];
        z = verticesPositions[i * 3 + 2];
        kzcVector3Set(&vertexPosition[j], x * sizeX * 0.5f, y * sizeY * 0.5f, z * sizeZ * 0.5f);
        kzcVector4Set(&vertexColor[j], 1.f, 1.f, 1.f, 1.f);
        k = j % 6;
        kzcVector2Set(&vertexUV[j], UVs[k * 2], UVs[k * 2 + 1]);
    }

    result = kzcVertexListAddAttribute(vertexList, "kzPosition", KZC_VERTEX_ATTRIBUTE_POSITION, 
        KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32, sizeof(kzFloat), 3);
    kzsErrorForward(result);

    result = kzcVertexListAddAttribute(vertexList, "kzTextureCoordinate0", KZC_VERTEX_ATTRIBUTE_TEXTURE_COORDINATE, 
        KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32, sizeof(kzFloat), 2);
    kzsErrorForward(result);

    result = kzuMeshCalculateNormals(kzcMemoryGetManager(mesh), indices, indexCount, (kzFloat*)vertexPosition->data, indexCount, &vertexNormal);
    kzsErrorForward(result);
    result = kzcVertexListAddAttribute(vertexList, "kzNormal", KZC_VERTEX_ATTRIBUTE_NORMAL, 
        KZC_VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT32, sizeof(kzFloat), 3);
    kzsErrorForward(result);

    {
        void* data;
        void* originalData;
        struct KzuClusterDefinition clusters[1];

        result = kzcMemoryAllocPointer(memoryManager, &data, (sizeof(*vertexPosition) + sizeof(*vertexUV) + sizeof(*vertexNormal)) * indexCount, "CombinedVertexData");
        kzsErrorForward(result);

        originalData = data;
        kzsMemcpy(data, vertexPosition, sizeof(*vertexPosition) * indexCount);
        data = (kzU8*)data + sizeof(*vertexPosition) * indexCount;
        kzsMemcpy(data, vertexUV, sizeof(*vertexUV) * indexCount);
        data = (kzU8*)data + sizeof(*vertexUV) * indexCount;
        kzsMemcpy(data, vertexNormal, sizeof(*vertexNormal) * indexCount);
        data = originalData;

        result = kzcVertexListSetData(vertexList, data, indexCount);
        kzsErrorForward(result);

        clusters[0].indexData = indices;
        clusters[0].indexCount = indexCount;

        result = kzuMeshAssignData(mesh, resourceManager, vertexList, clusters, 1, KZ_NULL);
        kzsErrorForward(result);

        result = kzcMemoryFreePointer(data);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreePointer(vertexColor);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(vertexPosition);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(vertexNormal);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(vertexUV);
    kzsErrorForward(result);
    result = kzcVertexListDelete(vertexList);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(indices);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuMeshGenerateBox(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager, kzFloat size)
{
    kzsError result;

    result = kzuMeshGenerateBoxFreeSize(mesh, resourceManager, size, size, size);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuMeshCalculateNormals(const struct KzcMemoryManager* memoryManager, const kzU16* indices, const kzUint indiceCount, const kzFloat* vertices, 
                                 const kzUint verticeCount, struct KzcVector3** out_normal)
{
    kzsError result;
    kzUint i;
    kzUint faceCount;
    struct KzcVector3* normals;

    /* Allocate memory for vertices, 3 x float per each. */
    result = kzcMemoryAllocPointer(memoryManager, &normals, sizeof(*normals) * verticeCount, "Normal allocation");
    kzsErrorForward(result);

    /* Initialize vertex normals as zero. */
    for(i = 0; i < verticeCount; ++i)
    {
        normals[i] = KZC_VECTOR3_ZERO;
    }
    
    faceCount = indiceCount / 3;
    for(i = 0; i < faceCount; ++i)
    {
        struct KzcVector3 vertex1, vertex2, vertex3, deltaA, deltaB;
        struct KzcVector3 faceNormal;
        kzUint index1, index2, index3;
        index1 = (kzUint)indices[i * 3];
        index2 = (kzUint)indices[i * 3 + 1];
        index3 = (kzUint)indices[i * 3 + 2];

        kzcVector3Set(&vertex1, vertices[index1 * 3], vertices[index1 * 3 + 1], vertices[index1 * 3 + 2]);
        kzcVector3Set(&vertex2, vertices[index2 * 3], vertices[index2 * 3 + 1], vertices[index2 * 3 + 2]);
        kzcVector3Set(&vertex3, vertices[index3 * 3], vertices[index3 * 3 + 1], vertices[index3 * 3 + 2]);

        kzcVector3Subtract(&vertex3, &vertex1, &deltaA);
        kzcVector3Normalize(&deltaA);
        kzcVector3Subtract(&vertex2, &vertex1, &deltaB);
        kzcVector3Normalize(&deltaB);
        
        kzcVector3Cross(&deltaB, &deltaA, &faceNormal);
        kzcVector3Normalize(&faceNormal);

        kzcVector3Add(&normals[index1], &faceNormal, &normals[index1]);
        kzcVector3Add(&normals[index2], &faceNormal, &normals[index2]);
        kzcVector3Add(&normals[index3], &faceNormal, &normals[index3]);
    }
    for(i = 0; i < verticeCount; ++i)
    {
        kzcVector3Normalize(&normals[i]);
    }

    *out_normal = normals;
    kzsSuccess();
}

kzUint kzuMeshGetClusterCount(const struct KzuMesh* mesh)
{
    kzsAssert(kzuMeshIsValid(mesh));
    return mesh->data->clusterCount;
}

struct KzuMeshCluster* kzuMeshGetClusterAtIndex(const struct KzuMesh* mesh, kzUint clusterIndex)
{
    kzsAssert(kzuMeshIsValid(mesh));

    return &mesh->data->clusters[clusterIndex];
}

void kzuMeshSetMaterial(const struct KzuMesh* mesh, struct KzuMaterial* material)
{
    kzUint i;

    kzsAssert(kzuMeshIsValid(mesh));
    kzsAssert(material == KZ_NULL || kzcIsValidPointer(material));

    for(i = 0; i < mesh->data->clusterCount; ++i)
    {
        kzuMeshClusterSetMaterial(&mesh->data->clusters[i], material);
    }
}

void kzuMeshClusterSetMaterial(struct KzuMeshCluster* meshCluster, struct KzuMaterial* material)
{
    kzsAssert(meshCluster != KZ_NULL);
    meshCluster->material = material;
}

struct KzuMaterial* kzuMeshGetMaterial(const struct KzuMesh* mesh)
{
    kzsAssert(kzuMeshIsValid(mesh));
    kzsAssert(mesh->data->clusterCount > 0);

    return mesh->data->clusters[0].material;
}

struct KzuMaterial* kzuMeshClusterGetMaterial(const struct KzuMeshCluster* meshCluster)
{
    kzsAssert(meshCluster != KZ_NULL);
    return meshCluster->material;
}

kzsError kzuMeshGetVertices(const struct KzcMemoryManager* memoryManager, const struct KzuMesh* mesh, struct KzcDynamicArray** out_vertices)
{
    kzsError result;
    kzsAssert(kzuMeshIsValid(mesh));
    kzsAssert(mesh->data->clusterCount > 0);

    result = kzcVertexBufferGetVertices(memoryManager, mesh->data->vertexBuffer, out_vertices);
    kzsErrorForward(result);

    kzsSuccess();
}



KZ_CALLBACK static kzsError kzuMeshNodeDeleteImplementation_internal(struct KzuObjectNode* objectNode)
{
    kzsError result = kzuMeshNodeDelete(kzuMeshNodeFromObjectNode(objectNode));
    kzsErrorForward(result);
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuMeshNodeCopyImplementation_internal(const struct KzcMemoryManager* memoryManager,
                                                                   const struct KzuObjectNode* sourceObjectNode,
                                                                   struct KzuObjectNode** out_objectNode)
{
    kzsError result;
    struct KzuMeshNodeData* sourceMeshNodeData = ((struct KzuMeshNodeData*)sourceObjectNode->data);
    struct KzuObjectNodeData* objectNodeData;
    struct KzuObjectNode* objectNode;

    result = kzuMeshCreateObjectNodeData_internal(memoryManager, sourceMeshNodeData->mesh, &objectNodeData);
    kzsErrorForward(result);

    result = kzuObjectNodeCopy_private(memoryManager, sourceObjectNode, objectNodeData, &objectNode);
    kzsErrorForward(result);

    *out_objectNode = objectNode;
    kzsSuccess();
}

KZ_CALLBACK static struct KzuBoundingVolume* kzuMeshNodeGetBoundingVolume_internal(const struct KzuObjectNode* objectNode)
{
    return kzuMeshGetBoundingVolume(kzuMeshNodeGetMesh(kzuMeshNodeFromObjectNode(objectNode)));
}

const struct KzuObjectNodeClass KZU_MESH_NODE_CLASS =
{
    kzuMeshNodeDeleteImplementation_internal,
    kzuMeshNodeCopyImplementation_internal,
    kzuRendererApplyMeshObject,
    kzuMeshNodeGetBoundingVolume_internal
};
