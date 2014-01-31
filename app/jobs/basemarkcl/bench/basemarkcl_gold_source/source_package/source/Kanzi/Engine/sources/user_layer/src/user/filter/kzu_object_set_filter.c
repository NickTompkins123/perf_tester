/**
* \file
* Specifies object set filter.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_object_set_filter.h"

#include "kzu_object_source.h"
#include "kzu_object_source_private.h"

#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_object.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_hash_set.h>

#include <system/time/kzs_tick.h>


/** Structure for object set filter. Object set selects well-defined set of objects from source. */
struct KzuObjectSetFilter
{
    struct KzuFilterObjectSource filterObjectSource; /**< Inheritance. */
    struct KzcHashSet* objects; /**< Set of objects that this filter contains. <KzuObjectNode>. */
};


/** Callback function for starting object set filter. */
static kzsError kzuObjectSetFilterStart_internal(struct KzuFilterObjectSource* filterObjectSource,
                                                 const struct KzuObjectSourceRuntimeData* runtimeData,
                                                 const struct KzuTransformedObjectNode* camera,
                                                 const struct KzcDynamicArray* inputObjects);
/** Call-back function for deleting additional data of object set filter. */
static kzsError kzuObjectSetFilterDelete_internal(struct KzuFilterObjectSource* filterObjectSource);
/** Call-back function for applying object set filter. */
static kzBool kzuObjectSetFilterApply_internal(struct KzuFilterObjectSource* filterObjectSource,
                                               const struct KzuObjectSourceRuntimeData* runtimeData,
                                               const struct KzuTransformedObjectNode* camera,
                                               const struct KzuTransformedObjectNode* transformedObject);


/** Filter configuration for object set filters. */
static const struct KzuFilterConfiguration KZU_OBJECT_SET_FILTER_CONFIGURATION =
{
    kzuObjectSetFilterStart_internal, /* Start function */
    kzuObjectSetFilterDelete_internal, /* Delete function */
    kzuObjectSetFilterApply_internal /* Apply function */
};


kzsError kzuObjectSetFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                  kzUint objectCount, struct KzuObjectNode* const* objects, kzBool isExclusive,
                                  struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;
    struct KzuObjectSetFilter* objectSetFilter;
    kzUint i;

    result = kzcMemoryAllocVariable(memoryManager, objectSetFilter, "Object set filter");
    kzsErrorForward(result);

    result = kzuFilterObjectSourceInitialize_private(memoryManager, &objectSetFilter->filterObjectSource, input, isExclusive,
                                                     &KZU_OBJECT_SET_FILTER_CONFIGURATION, &objectSource);
    kzsErrorForward(result);

    result = kzcHashSetCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &objectSetFilter->objects);
    kzsErrorForward(result);
    for (i = 0; i < objectCount; ++i)
    {
        kzsAssert(kzcIsValidPointer(objects[i]));
        result = kzcHashSetAdd(objectSetFilter->objects, objects[i]);
        kzsErrorForward(result);
    }

    *out_objectSource = objectSource;
    kzsSuccess();
}

kzsError kzuObjectSetFilterAddObject(const struct KzuObjectSource* objectSource, const struct KzuObjectNode* objectNode)
{
    kzsError result;
    struct KzuObjectSetFilter* objectSetFilter;
    
    kzsAssert(kzcIsValidPointer(objectSource));
    objectSetFilter = (struct KzuObjectSetFilter*)kzuFilterObjectSourceFromObjectSource_private(objectSource);
    kzsAssert(kzcIsValidPointer(objectSetFilter));

    result = kzcHashSetAdd(objectSetFilter->objects, (void*)objectNode);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Adds object and its ancestors to object set hash set. */
static kzsError kzuObjectSetFilterAddAncestors_internal(struct KzcHashSet* allTransformedObjects, struct KzuTransformedObjectNode* transformedObject)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzcIsValidPointer(transformedObject));
    kzsAssert(kzcIsValidPointer(allTransformedObjects));

    result = kzcHashSetAdd(allTransformedObjects, transformedObject);
    kzsErrorForward(result);

    it = kzuTransformedObjectNodeGetChildren(transformedObject);
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuTransformedObjectNode* childTransformedObject = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        result = kzuObjectSetFilterAddAncestors_internal(allTransformedObjects, childTransformedObject);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuObjectSetFilterStart_internal(struct KzuFilterObjectSource* filterObjectSource,
                                                             const struct KzuObjectSourceRuntimeData* runtimeData,
                                                             const struct KzuTransformedObjectNode* camera,
                                                             const struct KzcDynamicArray* inputObjects)
{
    kzsError result;
    struct KzuObjectSetFilter* objectSetFilter = (struct KzuObjectSetFilter*)filterObjectSource;
    kzUint stamp = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(objectSetFilter));

    /* Create hash set, where all ancestors of specified objects are stored. */
    {
        struct KzcDynamicArrayIterator it;
        /* Set of objects to be included by this filter. This includes transformed version of specified objects and
           their ancestors. This is created and filled in start. <KzuTransformedObjectNode>. */
        struct KzcHashSet* includedObjects;

        result = kzcHashSetCreate(kzcMemoryGetManager(runtimeData), KZC_HASH_MAP_CONFIGURATION_POINTER, &includedObjects);
        kzsErrorForward(result);

        result = kzuObjectSourceSetRuntimeCacheData(&filterObjectSource->objectSource, runtimeData, includedObjects);
        kzsErrorForward(result);

        it = kzcDynamicArrayGetIterator(inputObjects);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuTransformedObjectNode* transformedObject = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            struct KzuObjectNode* object = kzuTransformedObjectNodeGetObjectNode(transformedObject);

            if (kzcHashSetContains(objectSetFilter->objects, object))
            {
                result = kzuObjectSetFilterAddAncestors_internal(includedObjects, transformedObject);
                kzsErrorForward(result);
            }
        }
    }

    kzuObjectSourceAddCumulativeTime_private(runtimeData->rootObjectSource, 
        KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_SET_START, kzsTimeGetCurrentTimestamp() - stamp);

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuObjectSetFilterDelete_internal(struct KzuFilterObjectSource* filterObjectSource)
{
    kzsError result;
    struct KzuObjectSetFilter* objectSetFilter = (struct KzuObjectSetFilter*)filterObjectSource;

    kzsAssert(kzcIsValidPointer(objectSetFilter));

    result = kzcHashSetDelete(objectSetFilter->objects);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK static kzBool kzuObjectSetFilterApply_internal(struct KzuFilterObjectSource* filterObjectSource,
                                                           const struct KzuObjectSourceRuntimeData* runtimeData,
                                                           const struct KzuTransformedObjectNode* camera,
                                                           const struct KzuTransformedObjectNode* transformedObject)
{
    kzBool result;
    struct KzcHashSet* includedObjects;
    kzUint stamp = kzsTimeGetCurrentTimestamp();

    kzsAssert(kzcIsValidPointer(filterObjectSource));

    includedObjects = kzuObjectSourceGetRuntimeCacheData(&filterObjectSource->objectSource, runtimeData);

    result = kzcHashSetContains(includedObjects, transformedObject);

    kzuObjectSourceAddCumulativeTime_private(runtimeData->rootObjectSource,
        KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_SET_APPLY, kzsTimeGetCurrentTimestamp() - stamp);

    return result;
}
