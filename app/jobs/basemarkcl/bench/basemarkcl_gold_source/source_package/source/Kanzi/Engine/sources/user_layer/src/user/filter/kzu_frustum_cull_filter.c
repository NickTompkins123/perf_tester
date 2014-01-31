/**
* \file
* Specifies frustum cull filter.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_frustum_cull_filter.h"

#include "kzu_object_source_private.h"
#include "kzu_object_source.h"

#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_plane.h>

#include <system/debug/kzs_error.h>
#include <system/time/kzs_tick.h>


/** Structure for frustum cull filter. */
struct KzuFrustumCullFilter
{
    struct KzuFilterObjectSource filterObjectSource; /**< Inheritance. */
    struct KzcPlane frustumPlanes[KZU_FRUSTUM_PLANE_COUNT]; /**< List of frustum planes. */
};


/** Call-back function for starting object frustum culling. */
static kzsError kzuFrustumCullFilterStart_internal(struct KzuFilterObjectSource* filterObjectSource,
                                                   const struct KzuObjectSourceRuntimeData* runtimeData,
                                                   const struct KzuTransformedObjectNode* camera,
                                                   const struct KzcDynamicArray* inputObjects);
/** Call-back function for applying object frustum cull. */
static kzBool kzuFrustumCullFilterApply_internal(const struct KzuFilterObjectSource* filterObjectSource,
                                                 const struct KzuObjectSourceRuntimeData* runtimeData,
                                                 const struct KzuTransformedObjectNode* transformedCameraNode,
                                                 const struct KzuTransformedObjectNode* transformedObject);


/** Filter configuration for frustum cull filters. */
static const struct KzuFilterConfiguration KZU_FRUSTUM_CULL_FILTER_CONFIGURATION =
{
    kzuFrustumCullFilterStart_internal, /* Start function */
    KZ_NULL, /* Delete function */
    kzuFrustumCullFilterApply_internal /* Apply function */
};


kzsError kzuFrustumCullFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                    struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;
    struct KzuFrustumCullFilter* frustumCullFilter;

    result = kzcMemoryAllocVariable(memoryManager, frustumCullFilter, "Frustum cull filter");
    kzsErrorForward(result);
 
    result = kzuFilterObjectSourceInitialize_private(memoryManager, &frustumCullFilter->filterObjectSource, input, KZ_FALSE,
                                                     &KZU_FRUSTUM_CULL_FILTER_CONFIGURATION, &objectSource);
    kzsErrorForward(result);

    *out_objectSource = objectSource;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuFrustumCullFilterStart_internal(struct KzuFilterObjectSource* filterObjectSource,
                                                               const struct KzuObjectSourceRuntimeData* runtimeData,
                                                               const struct KzuTransformedObjectNode* camera,
                                                               const struct KzcDynamicArray* inputObjects)
{
    struct KzcMatrix4x4 viewMatrix;
    struct KzuCameraNode* cameraNode;
    struct KzuFrustumCullFilter* frustumCullFilter = (struct KzuFrustumCullFilter*)filterObjectSource;
    struct KzcMatrix4x4 frustumPlaneMatrix;
    kzUint stamp = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(frustumCullFilter));

    /* Calculate frustum planes. */
    viewMatrix = kzuTransformedObjectNodeGetMatrix(camera);
    cameraNode = kzuCameraNodeFromObjectNode(kzuTransformedObjectNodeGetObjectNode(camera));
    kzcMatrix4x4SetToViewCoordinates(&viewMatrix, &frustumPlaneMatrix);
    kzuCameraNodeCalculateFrustumPlanes(cameraNode, &frustumPlaneMatrix, frustumCullFilter->frustumPlanes);

    kzuObjectSourceAddCumulativeTime_private(runtimeData->rootObjectSource, KZU_OBJECT_SOURCE_MEASUREMENT_FRUSTUM_CULL_START, kzsTimeGetCurrentTimestamp() - stamp);

    kzsSuccess();
}

KZ_CALLBACK static kzBool kzuFrustumCullFilterApply_internal(const struct KzuFilterObjectSource* filterObjectSource,
                                                             const struct KzuObjectSourceRuntimeData* runtimeData,
                                                             const struct KzuTransformedObjectNode* transformedCameraNode,
                                                             const struct KzuTransformedObjectNode* transformedObject)
{
    kzBool result = KZ_TRUE;
    struct KzuFrustumCullFilter* frustumCullFilter = (struct KzuFrustumCullFilter*)filterObjectSource;
    struct KzuObjectNode* objectNode;
    struct KzuBoundingVolume* boundingVolume;
    kzUint stamp = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(frustumCullFilter));

    objectNode = kzuTransformedObjectNodeGetObjectNode(transformedObject);

    boundingVolume = kzuObjectNodeGetBoundingVolume(objectNode);
    
    if (boundingVolume != KZ_NULL)
    {
        struct KzcMatrix4x4 objectMatrix = kzuTransformedObjectNodeGetMatrix(transformedObject);
        struct KzuTransformedBoundingVolume transformedBoundingVolume;
        kzUint i;

        kzuBoundingVolumeCalculateTransformedBoundingVolume(boundingVolume, &objectMatrix, &transformedBoundingVolume);

        for (i = 0; i < KZU_FRUSTUM_PLANE_COUNT; ++i)
        {
            if (!kzuTransformedBoundingVolumeIsAbovePlane(&transformedBoundingVolume, &frustumCullFilter->frustumPlanes[i]))
            {
                result = KZ_FALSE;
                break;
            }
        }
    }

    kzuObjectSourceAddCumulativeTime_private(runtimeData->rootObjectSource, KZU_OBJECT_SOURCE_MEASUREMENT_FRUSTUM_CULL_APPLY, kzsTimeGetCurrentTimestamp() - stamp);

    return result;
}
