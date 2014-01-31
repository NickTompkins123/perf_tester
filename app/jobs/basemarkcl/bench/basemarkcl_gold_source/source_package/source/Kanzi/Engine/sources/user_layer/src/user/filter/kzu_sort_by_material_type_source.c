/**
* \file
* Specifies sorting object source for material type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_sort_by_material_type_source.h"

#include "kzu_object_source.h"
#include "kzu_object_source_private.h"

#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/material/kzu_material_type.h>
#include <user/material/kzu_material.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/collection/kzc_sort.h>


/** Comparator function for ordering object sources by their material type. */
static kzInt kzuSortByMaterialType_internal(const struct KzuTransformedObjectNode* first,
                                            const struct KzuTransformedObjectNode* second,
                                            const struct KzcMatrix4x4* cameraMatrix,
                                            const struct KzuObjectSourceRuntimeData* runtimeData);


/** Sort configuration for material type sorting */
static const struct KzuSortObjectSourceConfiguration KZU_MATERIAL_TYPE_SORT_CONFIGURATION =
{
    KZ_NULL, /* Start function */
    KZ_NULL, /* Delete function */
    kzuSortByMaterialType_internal /* Comparator function */
};


kzsError kzuSortByMaterialTypeObjectSourceCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                                 struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;

    result = kzuSortObjectSourceCreate(memoryManager, input, &KZU_MATERIAL_TYPE_SORT_CONFIGURATION, &objectSource);
    kzsErrorForward(result);

    *out_objectSource = objectSource;
    kzsSuccess();
}

KZ_CALLBACK static kzInt kzuSortByMaterialType_internal(const struct KzuTransformedObjectNode* first,
                                                        const struct KzuTransformedObjectNode* second,
                                                        const struct KzcMatrix4x4* cameraMatrix,
                                                        const struct KzuObjectSourceRuntimeData* runtimeData)
{
    struct KzuMaterialType* firstMaterialType = KZ_NULL;
    struct KzuMaterialType* secondMaterialType = KZ_NULL;
    struct KzuObjectNode* firstObjectNode = kzuTransformedObjectNodeGetObjectNode(first);
    struct KzuObjectNode* secondObjectNode = kzuTransformedObjectNodeGetObjectNode(second);

    if(kzuObjectNodeGetType(firstObjectNode) == KZU_OBJECT_TYPE_MESH)
    {
        struct KzuMesh* mesh = kzuMeshNodeGetMesh(kzuMeshNodeFromObjectNode(firstObjectNode));
        if(kzuMeshGetClusterCount(mesh) == 1)
        {
            struct KzuMaterial* material = kzuMeshGetMaterial(mesh);
            if(material != KZ_NULL)
            {
                firstMaterialType = kzuMaterialGetMaterialType(material);
            }
        }
    }
    if(kzuObjectNodeGetType(secondObjectNode) == KZU_OBJECT_TYPE_MESH)
    {
        struct KzuMesh* mesh = kzuMeshNodeGetMesh(kzuMeshNodeFromObjectNode(secondObjectNode));
        if(kzuMeshGetClusterCount(mesh) == 1)
        {
            struct KzuMaterial* material = kzuMeshGetMaterial(mesh);
            if(material != KZ_NULL)
            {
                secondMaterialType = kzuMaterialGetMaterialType(material);
            }
        }
    }
    
    return kzcComparePointers(firstMaterialType, secondMaterialType);
}
