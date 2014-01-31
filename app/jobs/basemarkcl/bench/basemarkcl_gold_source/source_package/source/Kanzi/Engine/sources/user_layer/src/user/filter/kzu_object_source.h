/**
* \file
* Object source. Object source that can be coupled with filters.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_OBJECT_SOURCE_H
#define KZU_OBJECT_SOURCE_H


#include <core/util/collection/kzc_comparator.h>

#include <system/debug/kzs_error.h>


enum KzuObjectSourceMeasurement
{
    KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_SET_START, /**< Object set start. */
    KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_SET_APPLY, /**< Object set apply. */

    KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_TYPE_APPLY, /**< Object type apply. */

    KZU_OBJECT_SOURCE_MEASUREMENT_FRUSTUM_CULL_START, /**< Frustum cull start. */
    KZU_OBJECT_SOURCE_MEASUREMENT_FRUSTUM_CULL_APPLY, /**< Frustum cull apply. */

    KZU_OBJECT_SOURCE_MEASUREMENT_SORT_START, /**< Sort by Z start. */
    KZU_OBJECT_SOURCE_MEASUREMENT_SORT_APPLY, /**< Sort by Z apply. */

    KZU_OBJECT_SOURCE_MEASUREMENT_LOD_START,/**< LOD start. */
    KZU_OBJECT_SOURCE_MEASUREMENT_LOD_APPLY /**< LOD apply. */
};

/* Forward declarations */
struct KzuSortObjectSource;
struct KzuObjectSource;
struct KzuObjectSourceData;
struct KzuTransformedObjectNode;
struct KzcMemoryManager;
struct KzcDynamicArray;


/**
 * \struct KzuObjectSourceCache
 * Storage for object source cache data.
 */
struct KzuObjectSourceCache;

/**
 * \struct KzuObjectSourceRuntimeData
 * Structure passed as parameter to object source reset.
 */
struct KzuObjectSourceRuntimeData;


/** Creates an object source for root of the scene graph. */
kzsError kzuRootObjectSourceCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource** out_objectSource);

/** Creates an object source, which combines several object sources together. */
kzsError kzuCombinerObjectSourceCreate(const struct KzcMemoryManager* memoryManager, kzUint inputCount,
                                       struct KzuObjectSource* const* inputs, struct KzuObjectSource** out_objectSource);

/** Deletes an object source. */
kzsError kzuObjectSourceDelete(struct KzuObjectSource* objectSource);

/**
 * Takes ownership of the object source container deletion. After this, when a object source is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuObjectSourceTakeOwnership(const struct KzuObjectSource* objectSource);
/** Checks if the object source is valid (pointer and the data are non-null). */
kzBool kzuObjectSourceIsValid(const struct KzuObjectSource* objectSource);
/** Moves the data from source object source to the target object source. */
void kzuObjectSourceTransferData(struct KzuObjectSource* targetObjectSource, struct KzuObjectSource* sourceObjectSource);


/** Gets transformed object node from object source. out_objects is filled with transformed object nodes. */
kzsError kzuObjectSourceFetchGraph(const struct KzuObjectSource* objectSource, const struct KzuObjectSourceRuntimeData* runtimeData,
                                   struct KzuTransformedObjectNode* camera, struct KzcDynamicArray** out_objects);

/** Gets the root object source by traversing object source hierarchy upwards from the given object source. */
struct KzuObjectSource* kzuObjectSourceGetRoot(struct KzuObjectSource* objectSource);

/** Sets input data for object source. */
void kzuFilterObjectSourceSetInput(const struct KzuObjectSource* objectSource, struct KzuObjectSource* input);
/** Gets an input from object source, KZ_NULL if not specified. */
struct KzuObjectSource* kzuObjectSourceGetInput(const struct KzuObjectSource* objectSource);

/** Creates an object for object source reset data. */
kzsError kzuObjectSourceRuntimeDataCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* rootObjectSource,
                                            struct KzcDynamicArray* allTransformedObjects,
                                            struct KzuObjectSourceRuntimeData** out_runtimeData);

/** Get the root object source from the reset data after the reset has completed. */
struct KzuObjectSource* kzuObjectSourceRuntimeDataGetRoot(const struct KzuObjectSourceRuntimeData* objectSourceRuntimeData);


/** Resets measurement info for object source. */
void kzuObjectSourceResetMeasurementInfo(const struct KzuObjectSource* objectSource);
/** Gets measurement info from object source. */
kzUint kzuObjectSourceGetMeasurementInfo(const struct KzuObjectSource* objectSource, enum KzuObjectSourceMeasurement measurement);

/** Adds cumulative time to object source measurement. */
void kzuObjectSourceAddCumulativeTime_private(const struct KzuObjectSource* objectSource, enum KzuObjectSourceMeasurement measurement, kzUint time);


#endif
