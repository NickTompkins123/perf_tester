/**
* \file
* Specifies sorting object source.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_sort_by_z_object_source.h"

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


/** Comparator function for ordering object sources by their z value front to back. */
static kzInt kzuSortByZFrontToBack_internal(const struct KzuTransformedObjectNode* first, const struct KzuTransformedObjectNode* second,
                                            const struct KzcMatrix4x4* cameraMatrix, const struct KzuObjectSourceRuntimeData* runtimeData);
/** Comparator function for ordering object sources by their z value back to front. */
static kzInt kzuSortByZBackToFront_internal(const struct KzuTransformedObjectNode* first, const struct KzuTransformedObjectNode* second,
                                            const struct KzcMatrix4x4* cameraMatrix, const struct KzuObjectSourceRuntimeData* runtimeData);


/** Sort configuration for front to back sorting. */
static const struct KzuSortObjectSourceConfiguration KZU_FRONT_TO_BACK_SORT_CONFIGURATION =
{
    KZ_NULL, /* Start function */
    KZ_NULL, /* Delete function */
    kzuSortByZFrontToBack_internal /* Comparator function */
};

/** Sort configuration for back to front sorting. */
static const struct KzuSortObjectSourceConfiguration KZU_BACK_TO_FRONT_SORT_CONFIGURATION =
{
    KZ_NULL, /* Start function */
    KZ_NULL, /* Delete function */
    kzuSortByZBackToFront_internal /* Comparator function */
};

kzsError kzuSortByZObjectSourceCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                      kzBool frontToBack, struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    const struct KzuSortObjectSourceConfiguration* configuration;
    struct KzuObjectSource* objectSource;

    if (frontToBack)
    {
        configuration = &KZU_FRONT_TO_BACK_SORT_CONFIGURATION;
    }
    else
    {
        configuration = &KZU_BACK_TO_FRONT_SORT_CONFIGURATION;
    }

    result = kzuSortObjectSourceCreate(memoryManager, input, configuration, &objectSource);
    kzsErrorForward(result);

    *out_objectSource = objectSource;
    kzsSuccess();
}

static kzInt kzuSortByZ_internal(const struct KzuTransformedObjectNode* first, const struct KzuTransformedObjectNode* second,
                                 const struct KzcMatrix4x4* cameraMatrix)
{
    struct KzcMatrix4x4 firstTransformation = kzuTransformedObjectNodeGetMatrix(first);
    struct KzcMatrix4x4 secondTransformation = kzuTransformedObjectNodeGetMatrix(second);
    kzFloat currentZ1;
    kzFloat currentZ2;

    currentZ1 = kzcMatrix4x4MultiplyAffineGetTranslationZ(&firstTransformation, cameraMatrix);
    currentZ2 = kzcMatrix4x4MultiplyAffineGetTranslationZ(&secondTransformation, cameraMatrix);

    return ((currentZ1 < currentZ2) ? 1 : ((currentZ1 > currentZ2) ? -1 : 0));
}

KZ_CALLBACK static kzInt kzuSortByZFrontToBack_internal(const struct KzuTransformedObjectNode* first,
                                                        const struct KzuTransformedObjectNode* second,
                                                        const struct KzcMatrix4x4* cameraMatrix,
                                                        const struct KzuObjectSourceRuntimeData* runtimeData)
{
    return kzuSortByZ_internal(first, second, cameraMatrix);
}

KZ_CALLBACK static kzInt kzuSortByZBackToFront_internal(const struct KzuTransformedObjectNode* first,
                                                        const struct KzuTransformedObjectNode* second,
                                                        const struct KzcMatrix4x4* cameraMatrix,
                                                        const struct KzuObjectSourceRuntimeData* runtimeData)
{
    return -kzuSortByZ_internal(first, second, cameraMatrix);
}
