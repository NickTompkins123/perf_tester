/**
* \file
* Mesh node and data. Mesh contains 3D geometry data and material. Node inherited from KzuObjectNode.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_MESH_H
#define KZU_MESH_H


#include <user/scene_graph/kzu_object_base.h>

#include <core/resource_manager/kzc_resource_memory_type.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declaration structures. */
struct KzuObjectNode;
struct KzuMaterial;
struct KzuBoundingVolume;
struct KzcMemoryManager;
struct KzcVertexBuffer;
struct KzcIndexBuffer;
struct KzcVertexList;
struct KzcResourceManager;
struct KzcHashMap;
struct KzcDynamicArray;
struct KzuPropertyManager;
struct KzuMessageDispatcher;


/**
* \struct KzuMeshNode
* Structure for the mesh node, inherited from KzuObjectNode.
*/
struct KzuMeshNode;
/**
* \struct KzuMesh
* Structure for the mesh, that is geometrical data of 3D object and reference to materials. Volumetric info in bounding volume.
*/
struct KzuMesh;
/**
* \struct KzuMeshCluster
* Represents single cluster in mesh, that is, index buffer & material.
*/
struct KzuMeshCluster;


/** Definition of mesh cluster, that is, data for indices. */
struct KzuClusterDefinition
{
    void* indexData;            /**< Index data, that is, list of indices. Data assumed in u16 format. */
    kzUint indexCount;          /**< Index count. */
};

/** Plane axes for generated planes. */
enum KzuPlaneAxes
{
    KZU_PLANE_XY,    /**< XY axes. */
    KZU_PLANE_XZ,    /**< XZ axes. */
    KZU_PLANE_YZ     /**< YZ axes. */
};

/** Definition of mesh bone, that is used for mesh skinning. */
struct KzuMeshBone
{
    struct KzuObjectNode* bone; /**< Reference to bone object node. */
    struct KzuObjectNode* childJoint; /**< Reference to child bone object node. */

    struct KzcMatrix4x4 invBindMatrix; /**< Inverse bind matrix for the bone, that is needed to calculate skin matrices for shader. */
};

/** Skeleton container. */
struct KzuSkeleton
{
    struct KzuMeshBone* bones; /**< List of bones. */
};


/** Object type identifier for mesh objects. */
extern const KzuObjectType KZU_OBJECT_TYPE_MESH;


/** Creates a new mesh. */
kzsError kzuMeshCreate(const struct KzcMemoryManager* memoryManager, enum KzcResourceMemoryType memoryType, struct KzuMesh** out_mesh);
/** Creates a new mesh node. */
kzsError kzuMeshNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                           kzString name, struct KzuMesh* mesh, struct KzuMeshNode** out_meshNode);
/** Deletes a mesh. */
kzsError kzuMeshDelete(struct KzuMesh* mesh);
/** Deletes a mesh node. */
kzsError kzuMeshNodeDelete(struct KzuMeshNode* meshNode);
/** Copies mesh to another. */
kzsError kzuMeshCopy(const struct KzcMemoryManager* memoryManager, const struct KzuMesh* sourceMesh, struct KzuMesh** out_targetMesh);


/**
 * Takes ownership of the mesh container deletion. After this, when a mesh is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuMeshTakeOwnership(const struct KzuMesh* mesh);
/** Checks if the mesh is valid (pointer and the data are non-null). */
kzBool kzuMeshIsValid(const struct KzuMesh* mesh);
/** Moves the data from source mesh to the target mesh. */
void kzuMeshTransferData(struct KzuMesh* targetMesh, struct KzuMesh* sourceMesh);

/** Assigns data for mesh. 
* \param mesh Destination mesh.
* \param resourceManager Resource manager that mesh buffers are created.
* \param vertexList List of vertices.
* \param clusterDefinitions Cluster definitions.
* \param clusterCount Number of clusters in mesh.
* \param vertexAttributeMapping Vertex attribute mapping. HashMap<KzuMaterialType(materialTypeToBeMapped),
*  HashMap<string(shaderAttributeName), string(vertexBufferAttributeName)>>.
*/
kzsError kzuMeshAssignData(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager, struct KzcVertexList* vertexList,
                           const struct KzuClusterDefinition* clusterDefinitions, kzUint clusterCount,
                           const struct KzcHashMap* vertexAttributeMapping);
/** Sets morph targets for mesh. */
kzsError kzuMeshSetMorphTargets(const struct KzuMesh* mesh, kzUint numMorphTargets, struct KzuMesh* const* morphTargets);
/** Gets morph targets from mesh. */
void kzuMeshGetMorphTargets(const struct KzuMesh* mesh, kzUint* out_numMorphTargets, struct KzuMesh*** out_morphTargets);
/** Interpolates mesh morph targets. */
kzsError kzuMeshInterpolateMorphTargets(const struct KzcMemoryManager* memoryManager, const struct KzuMesh* mesh, void** out_positions,
                                        void** out_normals, void** out_tangents);

/** Sets mesh for mesh node. */
void kzuMeshNodeSetMesh(const struct KzuMeshNode* meshNode, struct KzuMesh* mesh);
/** Gets a mesh from mesh node. */
struct KzuMesh* kzuMeshNodeGetMesh(const struct KzuMeshNode* meshNode);
/** Returns mesh node from object node. */
struct KzuMeshNode* kzuMeshNodeFromObjectNode(const struct KzuObjectNode* objectNode);
/** Returns an object node from mesh node. */
struct KzuObjectNode* kzuMeshNodeToObjectNode(struct KzuMeshNode* meshNode);
/** Sets bone data for mesh. */
kzsError kzuMeshAssignBoneData(const struct KzuMesh* mesh, kzUint boneCount, struct KzuObjectNode* const* bones,
                               const struct KzcMatrix4x4* invBindMatrices);
/** Gets bone data from mesh. */
struct KzuMeshBone* kzuMeshGetSkeletonBones(const struct KzuMesh* mesh);
/** Gets skeleton from mesh. */
struct KzuSkeleton* kzuMeshGetSkeleton(const struct KzuMesh* mesh);
/** Sets bind shape matrix for mesh. */
void kzuMeshSetBindShapeMatrix(const struct KzuMesh* mesh, const struct KzcMatrix4x4* bindShapeMatrix);
/** Gets bind shape matrix from mesh. */
struct KzcMatrix4x4 kzuMeshGetBindShapeMatrix(const struct KzuMesh* mesh);


/** Return a vertex buffer that mesh contains. */
struct KzcVertexBuffer* kzuMeshGetVertexBuffer(const struct KzuMesh* mesh);
/** Return an index buffer that mesh contains. If mesh contains more than 1 cluster, this should not be called. */
struct KzcIndexBuffer* kzuMeshGetIndexBuffer(const struct KzuMesh* mesh);
/** Gets index buffer from mesh cluster. */
struct KzcIndexBuffer* kzuMeshClusterGetIndexBuffer(const struct KzuMeshCluster* meshCluster);


/** Sets bounding volume for mesh. */
void kzuMeshSetBoundingVolume(const struct KzuMesh* mesh, struct KzuBoundingVolume* boundingVolume);
/** Gets bounding volume from mesh. */
struct KzuBoundingVolume* kzuMeshGetBoundingVolume(const struct KzuMesh* mesh);


/** Helper function for generating sphere mesh. */
kzsError kzuMeshGenerateSphere(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager, kzFloat radius, const kzUint numSegments, const kzUint numRows);
/** Helper function for generating sphere mesh. */
kzsError kzuMeshGenerateMorphSphere(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager, kzFloat radius, const kzUint numSegments, const kzUint numRows);
/** Generates 2D quad with offset. */
kzsError kzuMeshGeneratePlaneWithOffset(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager,
                                        kzFloat xOffset, kzFloat yOffset, kzFloat width, kzFloat height, enum KzuPlaneAxes planeAxes,
                                        kzBool invertU, kzBool invertV);
/** Helper function for generating 2D quad mesh. */
kzsError kzuMeshGeneratePlane(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager, kzFloat width, kzFloat height,
                              enum KzuPlaneAxes planeAxes, kzBool invertU, kzBool invertV);
/** Helper function for generating box mesh. */
kzsError kzuMeshGenerateBox(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager, kzFloat size);
/** Helper function for generating rectangle mesh. */
kzsError kzuMeshGenerateBoxFreeSize(const struct KzuMesh* mesh, struct KzcResourceManager* resourceManager, kzFloat sizeX, kzFloat sizeY, kzFloat sizeZ);
/** Calculate normals according to list of vertices and indices. */
kzsError kzuMeshCalculateNormals(const struct KzcMemoryManager* memoryManager, const kzU16* indices, const kzUint indiceCount, const kzFloat* vertices, 
                                 const kzUint verticeCount, struct KzcVector3** out_normal);



/** Gets cluster count from mesh. */
kzUint kzuMeshGetClusterCount(const struct KzuMesh* mesh);
/** Gets cluster from mesh at specified index (from 0 to clusterCount - 1). */
struct KzuMeshCluster* kzuMeshGetClusterAtIndex(const struct KzuMesh* mesh, kzUint clusterIndex);


/** Sets a material for mesh. Affects on all clusters*/
void kzuMeshSetMaterial(const struct KzuMesh* mesh, struct KzuMaterial* material);
/** Sets a material for mesh cluster. */
void kzuMeshClusterSetMaterial(struct KzuMeshCluster* meshCluster, struct KzuMaterial* material);
/** Gets a material from mesh. If mesh contains more than 1 cluster, this should not be called. */
struct KzuMaterial* kzuMeshGetMaterial(const struct KzuMesh* mesh);
/** Gets material for mesh cluster. */
struct KzuMaterial* kzuMeshClusterGetMaterial(const struct KzuMeshCluster* meshCluster);

/** Gets vertices for given mesh. Allocates memory for new dynamic array which will contain the list of KzcVector3 positions of the vertices. */
kzsError kzuMeshGetVertices(const struct KzcMemoryManager* memoryManager, const struct KzuMesh* mesh, struct KzcDynamicArray** out_vertices);


#endif
