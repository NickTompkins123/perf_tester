/**
* \file
* Specifies distance filter.
*
* This filter stores the transformedObjectNode of the target object node in reset to the object source cache.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_distance_filter.h"

#include "kzu_object_source_private.h"

#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_transformed_object.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/math/kzc_vector3.h>


/** Structure for distance filter. */
struct KzuDistanceFilter
{
    struct KzuFilterObjectSource filterObjectSource; /**< Inheritance. */
    struct KzuObjectNode* targetObjectNode; /**< Object node that is compared against. */
    kzFloat maxDistance; /**< Maximum distance for the compared object pivot. */
};


/** Call-back function for per-frame initialization of distance filter. */
static kzsError kzuDistanceFilterStart_internal(struct KzuFilterObjectSource* filterObjectSource,
                                                const struct KzuObjectSourceRuntimeData* runtimeData,
                                                const struct KzuTransformedObjectNode* camera,
                                                const struct KzcDynamicArray* inputObjects);
/** Call-back function for applying distance filter. */
static kzBool kzuDistanceFilterApply_internal(const struct KzuFilterObjectSource* filterObjectSource,
                                              const struct KzuObjectSourceRuntimeData* runtimeData,
                                              const struct KzuTransformedObjectNode* camera,
                                              const struct KzuTransformedObjectNode* transformedObject);


/** Filter configuration for distance filters. */
static const struct KzuFilterConfiguration KZU_DISTANCE_FILTER_CONFIGURATION =
{
    kzuDistanceFilterStart_internal, /* Start function */
    KZ_NULL, /* Delete function */
    kzuDistanceFilterApply_internal /* Apply function */
};


kzsError kzuDistanceFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                 struct KzuObjectNode* object, const kzFloat maxDistance, kzBool isExclusive,
                                 struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;
    struct KzuDistanceFilter* distanceFilter;

    result = kzcMemoryAllocVariable(memoryManager, distanceFilter, "Distance filter");
    kzsErrorForward(result);

    result = kzuFilterObjectSourceInitialize_private(memoryManager, &distanceFilter->filterObjectSource, input, isExclusive,
                                                     &KZU_DISTANCE_FILTER_CONFIGURATION, &objectSource);
    kzsErrorForward(result);

    distanceFilter->targetObjectNode = object;
    distanceFilter->maxDistance = maxDistance;
    
    *out_objectSource = objectSource;
    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuDistanceFilterStart_internal(struct KzuFilterObjectSource* filterObjectSource,
                                                            const struct KzuObjectSourceRuntimeData* runtimeData,
                                                            const struct KzuTransformedObjectNode* camera,
                                                            const struct KzcDynamicArray* inputObjects)
{
    kzsError result;
    struct KzuDistanceFilter* distanceFilter = (struct KzuDistanceFilter*)filterObjectSource;
    struct KzuTransformedObjectNode* transformedTargetNode;

    kzsAssert(kzcIsValidPointer(distanceFilter));

    transformedTargetNode = kzuTransformedObjectListFindObject(runtimeData->allTransformedObjects, distanceFilter->targetObjectNode);

    result = kzuObjectSourceSetRuntimeCacheData(&filterObjectSource->objectSource, runtimeData, transformedTargetNode);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK static kzBool kzuDistanceFilterApply_internal(const struct KzuFilterObjectSource* filterObjectSource,
                                                          const struct KzuObjectSourceRuntimeData* runtimeData,
                                                          const struct KzuTransformedObjectNode* camera,
                                                          const struct KzuTransformedObjectNode* transformedObject)
{
    kzBool result;
    const struct KzuDistanceFilter* distanceFilter = (const struct KzuDistanceFilter*)filterObjectSource;
    struct KzuTransformedObjectNode* transformedTargetNode;
    struct KzcVector3 targetPosition;
    struct KzcVector3 objectPosition;
    struct KzcVector3 deltaVector;
    kzFloat objectDistance;

    kzsAssert(kzcIsValidPointer(distanceFilter));

    transformedTargetNode = kzuObjectSourceGetRuntimeCacheData(&filterObjectSource->objectSource, runtimeData);
    kzuTransformedObjectNodeGetPosition(transformedTargetNode, &targetPosition);
    kzuTransformedObjectNodeGetPosition(transformedObject, &objectPosition);

    kzcVector3Subtract(&objectPosition, &targetPosition, &deltaVector);
    objectDistance = kzcVector3GetLength(&deltaVector);

    result = (objectDistance <= distanceFilter->maxDistance);

    return result;
}
