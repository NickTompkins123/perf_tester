/**
* \file
* Specifies object type filter.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_object_type_filter.h"

#include "kzu_object_source.h"
#include "kzu_object_source_private.h"

#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_object.h>

#include <core/memory/kzc_memory_manager.h>

#include <system/time/kzs_tick.h>


/** Structure for object type filter. */
struct KzuObjectTypeFilter
{
    struct KzuFilterObjectSource filterObjectSource; /**< Inheritance. */
    KzuObjectType type; /**< Object type. */
};


/** Call-back function for applying subset filter. */
static kzBool kzuObjectTypeFilterApply_internal(const struct KzuFilterObjectSource* filterObjectSource,
                                                const struct KzuObjectSourceRuntimeData* runtimeData,
                                                const struct KzuTransformedObjectNode* camera,
                                                const struct KzuTransformedObjectNode* transformedObject);


/** Filter configuration for frustum cull filters. */
static const struct KzuFilterConfiguration KZU_OBJECT_TYPE_FILTER_CONFIGURATION =
{
    KZ_NULL, /* Start function */
    KZ_NULL, /* Delete function */
    kzuObjectTypeFilterApply_internal /* Apply function */
};


kzsError kzuObjectTypeFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                   KzuObjectType objectType, kzBool isExclusive, struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;
    struct KzuObjectTypeFilter* objectTypeFilter;

    result = kzcMemoryAllocVariable(memoryManager, objectTypeFilter, "Object set filter");
    kzsErrorForward(result);

    result = kzuFilterObjectSourceInitialize_private(memoryManager, &objectTypeFilter->filterObjectSource, input, isExclusive,
                                                     &KZU_OBJECT_TYPE_FILTER_CONFIGURATION, &objectSource);
    kzsErrorForward(result);

    objectTypeFilter->type = objectType;

    *out_objectSource = objectSource;
    kzsSuccess();
}

KZ_CALLBACK static kzBool kzuObjectTypeFilterApply_internal(const struct KzuFilterObjectSource* filterObjectSource,
                                                            const struct KzuObjectSourceRuntimeData* runtimeData,
                                                            const struct KzuTransformedObjectNode* camera,
                                                            const struct KzuTransformedObjectNode* transformedObject)
{
    const struct KzuObjectTypeFilter* objectTypeFilter = (const struct KzuObjectTypeFilter*)filterObjectSource;
    kzBool result;
    struct KzuObjectNode* object = kzuTransformedObjectNodeGetObjectNode(transformedObject);
    KzuObjectType objectType = kzuObjectNodeGetType(object);
    kzUint stamp = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(objectTypeFilter));

    if (objectTypeFilter->type == KZU_OBJECT_TYPE_RENDERABLE)
    {
        result = kzuObjectTypeIsRenderable(objectType);
    }
    else
    {
        result = (objectType == objectTypeFilter->type);
    }

    kzuObjectSourceAddCumulativeTime_private(runtimeData->rootObjectSource, KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_TYPE_APPLY, kzsTimeGetCurrentTimestamp() - stamp);
    return result != filterObjectSource->isExclusive;
}
