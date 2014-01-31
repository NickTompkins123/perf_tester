/**
* \file
* Object source. Object source that can be coupled with filters.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_object_source.h"
#include "kzu_object_source_private.h"

#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_object.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_set.h>

#include <system/debug/kzs_counter.h>
#include <system/time/kzs_tick.h>


/** Root object source. This is the only possible root node for object source graphs. */
struct KzuRootObjectSource
{
    struct KzuObjectSourceData objectSource; /**< Object source. Used for inheritance. */
};

/** Combiner object source. This combines several input object sources by appending. */
struct KzuCombinerObjectSource
{
    struct KzuObjectSourceData objectSource; /**< Object source. Used for inheritance. */
    struct KzuObjectSource** inputs; /**< Array of input object sources. */
};


kzsError kzuObjectSourceCreate_private(const struct KzcMemoryManager* memoryManager, enum KzuObjectSourceType type,
                                       struct KzuObjectSourceData* objectSourceData, struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;

    result = kzcMemoryAllocVariable(memoryManager, objectSource, "Object source");
    kzsErrorForward(result);

    objectSource->data = objectSourceData;
    objectSourceData->selfOwned = KZ_TRUE;

    objectSourceData->type = type;

    *out_objectSource = objectSource;
    kzsSuccess();
}

kzsError kzuRootObjectSourceCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;
    struct KzuRootObjectSource* rootObjectSource;

    result = kzcMemoryAllocVariable(memoryManager, rootObjectSource, "Root object source");
    kzsErrorForward(result);

    result = kzuObjectSourceCreate_private(memoryManager, KZU_OBJECT_SOURCE_TYPE_ROOT, &rootObjectSource->objectSource, &objectSource);
    kzsErrorForward(result);

    *out_objectSource = objectSource;
    kzsSuccess();
}

static kzsError kzuRootObjectSourceDelete_internal(struct KzuRootObjectSource* rootObjectSource)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(rootObjectSource));

    result = kzcMemoryFreeVariable(rootObjectSource);
    kzsErrorForward(result);

    kzsSuccess();
}

static struct KzuRootObjectSource* kzuRootObjectSourceFromObjectSource_internal(const struct KzuObjectSource* objectSource)
{
    kzsAssert(kzuObjectSourceIsValid(objectSource));
    kzsAssert(objectSource->data->type == KZU_OBJECT_SOURCE_TYPE_ROOT);

    return (struct KzuRootObjectSource*)objectSource->data;
}

kzsError kzuCombinerObjectSourceCreate(const struct KzcMemoryManager* memoryManager, kzUint inputCount,
                                       struct KzuObjectSource* const* inputs, struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;
    struct KzuCombinerObjectSource* combinerObjectSource;
    kzUint i;

    result = kzcMemoryAllocVariable(memoryManager, combinerObjectSource, "Combiner object source");
    kzsErrorForward(result);

    result = kzuObjectSourceCreate_private(memoryManager, KZU_OBJECT_SOURCE_TYPE_COMBINER, &combinerObjectSource->objectSource, &objectSource);
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, combinerObjectSource->inputs, inputCount, "Object source combiner inputs");
    kzsErrorForward(result);

    for (i = 0; i < inputCount; ++i)
    {
        combinerObjectSource->inputs[i] = inputs[i];
    }

    *out_objectSource = objectSource;
    kzsSuccess();
}

static kzsError kzuCombinerObjectSourceDelete_internal(struct KzuCombinerObjectSource* combinerObjectSource)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(combinerObjectSource));

    result = kzcMemoryFreeArray(combinerObjectSource->inputs);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(combinerObjectSource);
    kzsErrorForward(result);

    kzsSuccess();
}

static struct KzuCombinerObjectSource* kzuCombinerObjectSourceFromObjectSource_internal(const struct KzuObjectSource* objectSource)
{
    kzsAssert(kzuObjectSourceIsValid(objectSource));
    kzsAssert(objectSource->data->type == KZU_OBJECT_SOURCE_TYPE_COMBINER);

    return (struct KzuCombinerObjectSource*)objectSource->data;
}

kzsError kzuSortObjectSourceCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                   const struct KzuSortObjectSourceConfiguration* configuration,
                                   struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;
    struct KzuSortObjectSource* sortObjectSource;

    result = kzcMemoryAllocVariable(memoryManager, sortObjectSource, "Sort object source");
    kzsErrorForward(result);

    result = kzuObjectSourceCreate_private(memoryManager, KZU_OBJECT_SOURCE_TYPE_SORT, &sortObjectSource->objectSource, &objectSource);
    kzsErrorForward(result);

    sortObjectSource->input = input;
    sortObjectSource->configuration = configuration;

    *out_objectSource = objectSource;
    kzsSuccess();
}

kzsError kzuSortObjectSourceInitialize_private(const struct KzcMemoryManager* memoryManager,
                                               struct KzuSortObjectSource* sortObjectSource,
                                               struct KzuObjectSource* input, 
                                               const struct KzuSortObjectSourceConfiguration* configuration,
                                               struct KzuObjectSource** out_objectSource)
{
   kzsError result;
   struct KzuObjectSource* objectSource;

   kzsAssert(kzcIsValidPointer(sortObjectSource));

   result = kzuObjectSourceCreate_private(memoryManager, KZU_OBJECT_SOURCE_TYPE_FILTER, &sortObjectSource->objectSource,
       &objectSource);
   kzsErrorForward(result);

   sortObjectSource->input = input;
   sortObjectSource->configuration = configuration;

   *out_objectSource = objectSource;
   kzsSuccess();
}

static kzsError kzuSortObjectSourceDelete_internal(struct KzuSortObjectSource* sortObjectSource)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(sortObjectSource));

    if (sortObjectSource->configuration->deleteFunction != KZ_NULL)
    {
        result = sortObjectSource->configuration->deleteFunction(sortObjectSource);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(sortObjectSource);
    kzsErrorForward(result);

    kzsSuccess();
}

static struct KzuSortObjectSource* kzuSortObjectSourceFromObjectSource_internal(const struct KzuObjectSource* objectSource)
{
    kzsAssert(kzuObjectSourceIsValid(objectSource));
    kzsAssert(objectSource->data->type == KZU_OBJECT_SOURCE_TYPE_SORT);

    return (struct KzuSortObjectSource*)objectSource->data;
}

kzsError kzuFilterObjectSourceInitialize_private(const struct KzcMemoryManager* memoryManager,
                                                 struct KzuFilterObjectSource* filterObjectSource,
                                                 struct KzuObjectSource* input, kzBool isExclusive,
                                                 const struct KzuFilterConfiguration* configuration,
                                                 struct KzuObjectSource** out_objectSource)
{
    kzsError result;
    struct KzuObjectSource* objectSource;

    kzsAssert(kzcIsValidPointer(filterObjectSource));

    result = kzuObjectSourceCreate_private(memoryManager, KZU_OBJECT_SOURCE_TYPE_FILTER, &filterObjectSource->objectSource, &objectSource);
    kzsErrorForward(result);

    filterObjectSource->input = input;
    filterObjectSource->isExclusive = isExclusive;
    filterObjectSource->configuration = configuration;

    *out_objectSource = objectSource;
    kzsSuccess();
}

static kzsError kzuFilterObjectSourceDelete_internal(struct KzuFilterObjectSource* filterObjectSource)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(filterObjectSource));

    if (filterObjectSource->configuration->deleteFunction != KZ_NULL)
    {
        result = filterObjectSource->configuration->deleteFunction(filterObjectSource);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(filterObjectSource);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuFilterObjectSource* kzuFilterObjectSourceFromObjectSource_private(const struct KzuObjectSource* objectSource)
{
    kzsAssert(kzuObjectSourceIsValid(objectSource));
    kzsAssert(objectSource->data->type == KZU_OBJECT_SOURCE_TYPE_FILTER);

    return (struct KzuFilterObjectSource*)objectSource->data;
}

kzsError kzuObjectSourceDelete(struct KzuObjectSource* objectSource)
{
    kzsError result;
    struct KzuObjectSourceData* objectSourceData;
    kzBool selfOwned;

    kzsAssert(kzuObjectSourceIsValid(objectSource));

    objectSourceData = objectSource->data;
    selfOwned = objectSourceData->selfOwned;

    switch (objectSourceData->type)
    {
        case KZU_OBJECT_SOURCE_TYPE_ROOT:
        {
            result = kzuRootObjectSourceDelete_internal(kzuRootObjectSourceFromObjectSource_internal(objectSource));
            kzsErrorForward(result);
            break;
        }

        case KZU_OBJECT_SOURCE_TYPE_COMBINER:
        {
            result = kzuCombinerObjectSourceDelete_internal(kzuCombinerObjectSourceFromObjectSource_internal(objectSource));
            kzsErrorForward(result);
            break;
        }

        case KZU_OBJECT_SOURCE_TYPE_FILTER:
        {
            result = kzuFilterObjectSourceDelete_internal(kzuFilterObjectSourceFromObjectSource_private(objectSource));
            kzsErrorForward(result);
            break;
        }

        case KZU_OBJECT_SOURCE_TYPE_SORT:
        {
            result = kzuSortObjectSourceDelete_internal(kzuSortObjectSourceFromObjectSource_internal(objectSource));
            kzsErrorForward(result);
            break;
        }
    }

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(objectSource);
        kzsErrorForward(result);
    }
    else
    {
        objectSource->data = KZ_NULL;
    }

    kzsSuccess();
}

void kzuObjectSourceTakeOwnership(const struct KzuObjectSource* objectSource)
{
    kzsAssert(kzuObjectSourceIsValid(objectSource));

    objectSource->data->selfOwned = KZ_FALSE;
}

kzBool kzuObjectSourceIsValid(const struct KzuObjectSource* objectSource)
{
    return objectSource != KZ_NULL && objectSource->data != KZ_NULL;
}

void kzuObjectSourceTransferData(struct KzuObjectSource* targetObjectSource, struct KzuObjectSource* sourceObjectSource)
{
    kzsAssert(kzcIsValidPointer(targetObjectSource));
    kzsAssert(targetObjectSource->data == KZ_NULL);
    kzsAssert(kzuObjectSourceIsValid(sourceObjectSource));

    targetObjectSource->data = sourceObjectSource->data;
    sourceObjectSource->data = KZ_NULL;
}

/** Internal function for fetching the data from combiner object source. */
static kzsError kzuCombinerObjectSourceFetchGraph_internal(const struct KzuCombinerObjectSource* combinerSource,
                                                           const struct KzuObjectSourceRuntimeData* runtimeData,
                                                           struct KzuTransformedObjectNode* camera,
                                                           struct KzcDynamicArray** out_objects)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(runtimeData);
    /* Hash map containing the keys of the new object list. */
    struct KzcHashSet* addedObjects;
    struct KzcDynamicArray* objects;
    kzUint i;

    result = kzcDynamicArrayCreate(memoryManager, &objects);
    kzsErrorForward(result);
    result = kzcHashSetCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &addedObjects);
    kzsErrorForward(result);

    /* Loop through the input object sources. */
    for (i = 0; i < kzcArrayLength(combinerSource->inputs); ++i)
    {
        struct KzuObjectSource* inputObjectSource = combinerSource->inputs[i];
        struct KzcDynamicArray* sourceObjects;
        struct KzcDynamicArrayIterator it;

        result = kzuObjectSourceFetchGraph(inputObjectSource, runtimeData, camera, &sourceObjects);
        kzsErrorForward(result);

        it = kzcDynamicArrayGetIterator(sourceObjects);
        /* Loop through the current object source elements. */
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuTransformedObjectNode* object = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);

            /* Element was actually found from new list, do not add! */
            if (!kzcHashSetContains(addedObjects, object))
            {
                result = kzcDynamicArrayAdd(objects, object);
                kzsErrorForward(result);
                result = kzcHashSetAdd(addedObjects, object);
                kzsErrorForward(result);
            }
        }
    }

    *out_objects = objects;
    kzsSuccess();
}

/** Internal function for fetching the data from filter object source. */
static kzsError kzuFilterObjectSourceFetchGraph_internal(struct KzuFilterObjectSource* filterSource,
                                                         const struct KzuObjectSourceRuntimeData* runtimeData,
                                                         struct KzuTransformedObjectNode* camera,
                                                         struct KzcDynamicArray** out_objects)
{
    kzsError result;
    struct KzcDynamicArray* objects;

    result = kzcDynamicArrayCreate(kzcMemoryGetManager(runtimeData), &objects);
    kzsErrorForward(result);

    {
        struct KzuObjectSource* inputObjectSource = filterSource->input;
        struct KzcDynamicArray* sourceObjects;
        struct KzcDynamicArrayIterator it;

        result = kzuObjectSourceFetchGraph(inputObjectSource, runtimeData, camera, &sourceObjects);
        kzsErrorForward(result);

        if (filterSource->configuration->startFunction != KZ_NULL)
        {
            result = filterSource->configuration->startFunction(filterSource, runtimeData, camera, sourceObjects);
            kzsErrorForward(result);
        }

        it = kzcDynamicArrayGetIterator(sourceObjects);
        /* Loop through the current object source elements. */
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuTransformedObjectNode* object = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);

            if (filterSource->configuration->applyFunction(filterSource, runtimeData, camera, object) != filterSource->isExclusive)
            {
                result = kzcDynamicArrayAdd(objects, object);
                kzsErrorForward(result);
            }
        }
    }

    *out_objects = objects;
    kzsSuccess();
}

struct KzuSortObjectSourceComparatorContext
{
    KzuSortObjectSourceComparatorFunction comparator;
    struct KzcMatrix4x4 camera;
    const struct KzuObjectSourceRuntimeData* runtimeData;
};

static kzInt kzuSortObjectSourceComparator_internal(const void* first, const void* second, const void* context)
{
    struct KzuSortObjectSourceComparatorContext* context2 = (struct KzuSortObjectSourceComparatorContext*)context;
    struct KzuTransformedObjectNode* firstObject = *(struct KzuTransformedObjectNode**)first;
    struct KzuTransformedObjectNode* secondObject = *(struct KzuTransformedObjectNode**)second;

    return context2->comparator(firstObject, secondObject, &context2->camera, context2->runtimeData);
}

/** Internal function for fetching the data from sort object source. */
static kzsError kzuSortObjectSourceFetchGraph_internal(struct KzuSortObjectSource* sortSource,
                                                       const struct KzuObjectSourceRuntimeData* runtimeData,
                                                       struct KzuTransformedObjectNode* camera,
                                                       struct KzcDynamicArray** out_objects)
{
    kzsError result;
    struct KzcDynamicArray* objects;

    result = kzcDynamicArrayCreate(kzcMemoryGetManager(runtimeData), &objects);
    kzsErrorForward(result);

    {
        struct KzuObjectSource* inputObjectSource = sortSource->input;
        struct KzcDynamicArray* sourceObjects;
        struct KzuSortObjectSourceComparatorContext context;
        kzUint stamp;

        result = kzuObjectSourceFetchGraph(inputObjectSource, runtimeData, camera, &sourceObjects);
        kzsErrorForward(result);

        stamp = kzsTimeGetCurrentTimestamp();
        if (sortSource->configuration->startFunction != KZ_NULL)
        {
            result = sortSource->configuration->startFunction(sortSource, runtimeData, camera, sourceObjects);
            kzsErrorForward(result);
        }
        kzuObjectSourceAddCumulativeTime_private(runtimeData->rootObjectSource, KZU_OBJECT_SOURCE_MEASUREMENT_SORT_START, kzsTimeGetCurrentTimestamp() - stamp);

        result = kzcDynamicArrayCopy(kzcMemoryGetManager(runtimeData), sourceObjects, &objects);
        kzsErrorForward(result);

        context.comparator = sortSource->configuration->comparator;

        if(camera != KZ_NULL)
        {
            struct KzcMatrix4x4 cameraMatrix = kzuTransformedObjectNodeGetMatrix(camera);
            kzcMatrix4x4SetToViewCoordinates(&cameraMatrix, &context.camera);
        }
        else
        {
            kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "No camera specified for sort object source, invalid operation");
        }
        
        context.runtimeData = runtimeData;

        

        stamp = kzsTimeGetCurrentTimestamp();
        kzcDynamicArraySortWithContext(objects, kzuSortObjectSourceComparator_internal, &context);
        kzuObjectSourceAddCumulativeTime_private(runtimeData->rootObjectSource, KZU_OBJECT_SOURCE_MEASUREMENT_SORT_APPLY, kzsTimeGetCurrentTimestamp() - stamp);
    }

    *out_objects = objects;
    kzsSuccess();
}

                                                       
static kzsError kzuObjectSourceSetRuntimeOutput_internal(const struct KzuObjectSource* objectSource,
                                                         const struct KzuObjectSourceRuntimeData* runtimeData,
                                                         struct KzcDynamicArray* objects)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(runtimeData));

    result = kzcHashMapPut(runtimeData->objectSourceOutputs, objectSource, objects);
    kzsErrorForward(result);

    kzsSuccess();
}

static struct KzcDynamicArray* kzuObjectSourceGetRuntimeOutput_internal(const struct KzuObjectSource* objectSource,
                                                                        const struct KzuObjectSourceRuntimeData* runtimeData)
{
    struct KzcDynamicArray* objects;

    kzsAssert(kzcIsValidPointer(runtimeData));

    if (!kzcHashMapGet(runtimeData->objectSourceOutputs, objectSource, (void**)&objects))
    {
        objects = KZ_NULL;
    }

    return objects;
}

kzsError kzuObjectSourceFetchGraph(const struct KzuObjectSource* objectSource, const struct KzuObjectSourceRuntimeData* runtimeData,
                                   struct KzuTransformedObjectNode* camera, struct KzcDynamicArray** out_objects)
{
    kzsError result;
    struct KzcDynamicArray* objects;

    kzsAssert(kzuObjectSourceIsValid(objectSource));

    objects = kzuObjectSourceGetRuntimeOutput_internal(objectSource, runtimeData);

    /* Calculate object source if not set. */
    if (objects == KZ_NULL)
    {
        switch (objectSource->data->type)
        {
            case KZU_OBJECT_SOURCE_TYPE_ROOT:
            {
                objects = runtimeData->allTransformedObjects;
                break;
            }

            case KZU_OBJECT_SOURCE_TYPE_FILTER:
            {
                struct KzuFilterObjectSource* filterSource = kzuFilterObjectSourceFromObjectSource_private(objectSource);

                kzsAssert(kzcIsValidPointer(filterSource));

                result = kzuFilterObjectSourceFetchGraph_internal(filterSource, runtimeData, camera, &objects);
                kzsErrorForward(result);
                break;
            }

            case KZU_OBJECT_SOURCE_TYPE_COMBINER:
            {
                struct KzuCombinerObjectSource* combinerSource = kzuCombinerObjectSourceFromObjectSource_internal(objectSource);

                kzsAssert(kzcIsValidPointer(combinerSource));

                result = kzuCombinerObjectSourceFetchGraph_internal(combinerSource, runtimeData, camera, &objects);
                kzsErrorForward(result);
                break;
            }

            case KZU_OBJECT_SOURCE_TYPE_SORT:
            {
                struct KzuSortObjectSource* sortSource = kzuSortObjectSourceFromObjectSource_internal(objectSource);

                kzsAssert(kzcIsValidPointer(sortSource));

                result = kzuSortObjectSourceFetchGraph_internal(sortSource, runtimeData, camera, &objects);
                kzsErrorForward(result);
                break;
            }

            default:
            {
                kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid value for object source type");
            }
        }

        result = kzuObjectSourceSetRuntimeOutput_internal(objectSource, runtimeData, objects);
        kzsErrorForward(result);
    }

    kzsCounterIncrease("kzuObjectSourceFetchGraph");
    *out_objects = objects;
    kzsSuccess();
}
    
struct KzuObjectSource* kzuObjectSourceGetRoot(struct KzuObjectSource* objectSource)
{
    struct KzuObjectSource* traverseObjectSource = objectSource;

    kzsAssert(kzuObjectSourceIsValid(traverseObjectSource));

    while (traverseObjectSource->data->type != KZU_OBJECT_SOURCE_TYPE_ROOT)
    {
        switch (traverseObjectSource->data->type)
        {
            case KZU_OBJECT_SOURCE_TYPE_ROOT:
            {
                break;
            }

            case KZU_OBJECT_SOURCE_TYPE_FILTER:
            {
                struct KzuFilterObjectSource* filterSource = kzuFilterObjectSourceFromObjectSource_private(traverseObjectSource);

                kzsAssert(kzcIsValidPointer(filterSource));

                traverseObjectSource = filterSource->input;
                break;
            }

            case KZU_OBJECT_SOURCE_TYPE_COMBINER:
            {
                struct KzuCombinerObjectSource* combinerSource = kzuCombinerObjectSourceFromObjectSource_internal(traverseObjectSource);

                kzsAssert(kzcIsValidPointer(combinerSource));
                kzsAssert(kzcArrayLength(combinerSource->inputs) > 0);

                traverseObjectSource = combinerSource->inputs[0];
                break;
            }

            case KZU_OBJECT_SOURCE_TYPE_SORT:
            {
                struct KzuSortObjectSource* sortSource = kzuSortObjectSourceFromObjectSource_internal(traverseObjectSource);

                kzsAssert(kzcIsValidPointer(sortSource));

                traverseObjectSource = sortSource->input;
                break;
            }
        }
    }

    return traverseObjectSource;
}

void kzuFilterObjectSourceSetInput(const struct KzuObjectSource* objectSource, struct KzuObjectSource* inputSource)
{
    struct KzuFilterObjectSource* filterObjectSource = kzuFilterObjectSourceFromObjectSource_private(objectSource);

    kzsAssert(kzcIsValidPointer(filterObjectSource));

    filterObjectSource->input = inputSource;
}

struct KzuObjectSource* kzuObjectSourceGetInput(const struct KzuObjectSource* objectSource)
{
    struct KzuObjectSource* input = KZ_NULL;
    
    switch (objectSource->data->type)
    {
        case KZU_OBJECT_SOURCE_TYPE_FILTER:
        {
            struct KzuFilterObjectSource* filterObjectSource = kzuFilterObjectSourceFromObjectSource_private(objectSource);
            kzsAssert(kzcIsValidPointer(filterObjectSource));
            input = filterObjectSource->input;
            break;
        }

        case KZU_OBJECT_SOURCE_TYPE_SORT:
        {
            struct KzuSortObjectSource* sortObjectSource = kzuSortObjectSourceFromObjectSource_internal(objectSource);
            kzsAssert(kzcIsValidPointer(sortObjectSource));
            input = sortObjectSource->input;
            break;
        }

        case KZU_OBJECT_SOURCE_TYPE_COMBINER:
        case KZU_OBJECT_SOURCE_TYPE_ROOT:
        {
            input = KZ_NULL;
            break;
        }
    }

    return input;
}

kzsError kzuObjectSourceRuntimeDataCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* rootObjectSource,
                                          struct KzcDynamicArray* allTransformedObjects,
                                          struct KzuObjectSourceRuntimeData** out_runtimeData)
{
    kzsError result;
    struct KzuObjectSourceRuntimeData* runtimeData;

    result = kzcMemoryAllocVariable(memoryManager, runtimeData, "Object source reset data");
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &runtimeData->objectSourceOutputs);
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &runtimeData->objectSourceCacheDatas);
    kzsErrorForward(result);

    runtimeData->allTransformedObjects = allTransformedObjects;
    runtimeData->rootObjectSource = rootObjectSource;

    *out_runtimeData = runtimeData;
    kzsSuccess();
}

struct KzuObjectSource* kzuObjectSourceRuntimeDataGetRoot(const struct KzuObjectSourceRuntimeData* objectSourceRuntimeData)
{
    kzsAssert(kzcIsValidPointer(objectSourceRuntimeData));

    return objectSourceRuntimeData->rootObjectSource;
}

kzsError kzuObjectSourceSetRuntimeCacheData(const struct KzuObjectSourceData* objectSource,
                                            const struct KzuObjectSourceRuntimeData* runtimeData,
                                            void* data)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(runtimeData));

    result = kzcHashMapPut(runtimeData->objectSourceCacheDatas, objectSource, data);
    kzsErrorForward(result);

    kzsSuccess();
}

void* kzuObjectSourceGetRuntimeCacheData(const struct KzuObjectSourceData* objectSource,
                                         const struct KzuObjectSourceRuntimeData* runtimeData)
{
    void* data;

    kzsAssert(kzcIsValidPointer(runtimeData));

    if (!kzcHashMapGet(runtimeData->objectSourceCacheDatas, objectSource, &data))
    {
        data = KZ_NULL;
    }

    return data;
}

void kzuObjectSourceResetMeasurementInfo(const struct KzuObjectSource* objectSource)
{
    kzsAssert(objectSource->data->type == KZU_OBJECT_SOURCE_TYPE_ROOT);

    objectSource->data->measurement.objectSetApplyTime = 0;
    objectSource->data->measurement.objectSetStartTime = 0;
    objectSource->data->measurement.frustumCullApplyTime = 0;
    objectSource->data->measurement.frustumCullStartTime = 0;
    objectSource->data->measurement.sortApplyTime = 0;
    objectSource->data->measurement.sortStartTime = 0;
    objectSource->data->measurement.objectTypeApplyTime = 0;
    objectSource->data->measurement.lodApplyTime = 0;
    objectSource->data->measurement.lodStartTime = 0;
}

/** Gets measurement info from object source. */
kzUint kzuObjectSourceGetMeasurementInfo(const struct KzuObjectSource* objectSource, enum KzuObjectSourceMeasurement measurement)
{
    kzUint time = 0;
    kzsAssert(objectSource->data->type == KZU_OBJECT_SOURCE_TYPE_ROOT);

    switch(measurement)
    {
        case KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_SET_APPLY: time = objectSource->data->measurement.objectSetApplyTime; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_SET_START: time = objectSource->data->measurement.objectSetStartTime; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_FRUSTUM_CULL_APPLY: time = objectSource->data->measurement.frustumCullApplyTime; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_FRUSTUM_CULL_START: time = objectSource->data->measurement.frustumCullStartTime; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_SORT_APPLY: time = objectSource->data->measurement.sortApplyTime; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_SORT_START: time = objectSource->data->measurement.sortStartTime; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_TYPE_APPLY: time = objectSource->data->measurement.objectTypeApplyTime; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_LOD_APPLY: time = objectSource->data->measurement.lodApplyTime; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_LOD_START:  time = objectSource->data->measurement.lodStartTime; break;
        default: kzsAssertText(KZ_FALSE, "Invalid measurement constant");
    }
    return time;
}

void kzuObjectSourceAddCumulativeTime_private(const struct KzuObjectSource* objectSource, enum KzuObjectSourceMeasurement measurement, kzUint time)
{
    kzsAssert(objectSource->data->type == KZU_OBJECT_SOURCE_TYPE_ROOT);

    switch(measurement)
    {
        case KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_SET_APPLY: objectSource->data->measurement.objectSetApplyTime += time; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_SET_START: objectSource->data->measurement.objectSetStartTime += time; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_FRUSTUM_CULL_APPLY: objectSource->data->measurement.frustumCullApplyTime += time; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_FRUSTUM_CULL_START: objectSource->data->measurement.frustumCullStartTime += time; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_SORT_APPLY: objectSource->data->measurement.sortApplyTime += time; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_SORT_START: objectSource->data->measurement.sortStartTime += time; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_TYPE_APPLY: objectSource->data->measurement.objectTypeApplyTime += time; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_LOD_APPLY: objectSource->data->measurement.lodApplyTime += time; break;
        case KZU_OBJECT_SOURCE_MEASUREMENT_LOD_START: objectSource->data->measurement.lodStartTime += time; break;
        default: kzsAssertText(KZ_FALSE, "Invalid measurement constant");
    }
}
