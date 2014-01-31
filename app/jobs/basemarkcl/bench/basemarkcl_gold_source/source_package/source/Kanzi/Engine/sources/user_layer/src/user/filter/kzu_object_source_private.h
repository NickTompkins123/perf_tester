/**
* \file
* Object source private members.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_OBJECT_SOURCE_PRIVATE_H
#define KZU_OBJECT_SOURCE_PRIVATE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuCameraNode;
struct KzuFilterObjectSource;
struct KzuSortObjectSource;
struct KzuTransformedObjectNode;
struct KzuObjectSourceRuntimeData;
struct KzcMatrix4x4;
struct KzcDynamicArray;
struct KzcMemoryManager;


/* Filters */

/** Function type for per frame filter initialization. */
typedef kzsError (*KzuFilterResetFunction)(struct KzuFilterObjectSource* filterObjectSource,
                                           const struct KzuObjectSourceRuntimeData* runtimeData);
/** Function type for filter start function. This is called just before apply, but only once per frame */
typedef kzsError (*KzuFilterStartFunction)(struct KzuFilterObjectSource* filterObjectSource,
                                           const struct KzuObjectSourceRuntimeData* runtimeData,
                                           const struct KzuTransformedObjectNode* camera,
                                           const struct KzcDynamicArray* inputObjects);
/** Function type for cleaning up filter. */
typedef kzsError (*KzuFilterDeleteFunction)(struct KzuFilterObjectSource* filterObjectSource);
/** Function type for determining whether to accept given object or not. */
typedef kzBool(*KzuFilterApplyFunction)(struct KzuFilterObjectSource* filterObjectSource,
                                        const struct KzuObjectSourceRuntimeData* runtimeData,
                                        const struct KzuTransformedObjectNode* camera,
                                        const struct KzuTransformedObjectNode* object);


/** Configuration for filter types. */
struct KzuFilterConfiguration
{
    KzuFilterStartFunction startFunction; /**< Function to call every frame in the beginning of apply. Can be KZ_NULL. */
    KzuFilterDeleteFunction deleteFunction; /**< Function which is called when the filter is deleted. Can be KZ_NULL. */
    KzuFilterApplyFunction applyFunction; /**< Function to use for evaluation whether a specific object will be included or not. */
};


/* Sort */

/** Function type for per frame sort object source initialization. */
typedef kzsError (*KzuSortObjectSourceResetFunction)(struct KzuSortObjectSource* sortObjectSource,
                                                     const struct KzuObjectSourceRuntimeData* runtimeData);
/** Function type for sort start function. This is called just before sorting. */
typedef kzsError (*KzuSortObjectSourceStartFunction)(struct KzuSortObjectSource* sortObjectSource,
                                                     const struct KzuObjectSourceRuntimeData* runtimeData,
                                                     const struct KzuTransformedObjectNode* camera,
                                                     const struct KzcDynamicArray* inputObjects);
/** Function type for cleaning up sort object source. */
typedef kzsError (*KzuSortObjectSourceDeleteFunction)(struct KzuSortObjectSource* sortObjectSource);
/** Function type for sorting transformed object nodes with sort object sources. */
typedef kzInt (*KzuSortObjectSourceComparatorFunction)(const struct KzuTransformedObjectNode* first,
                                                       const struct KzuTransformedObjectNode* second,
                                                       const struct KzcMatrix4x4* cameraMatrix,
                                                       const struct KzuObjectSourceRuntimeData* runtimeData);


/** Configuration for sort object sources. */
struct KzuSortObjectSourceConfiguration
{
    KzuSortObjectSourceStartFunction startFunction; /**< Function to call just before sorting. Can be KZ_NULL. */
    KzuSortObjectSourceDeleteFunction deleteFunction; /**< Function which is called when the sort object source is deleted. Can be KZ_NULL. */
    KzuSortObjectSourceComparatorFunction comparator; /**< Comparator function to use for sorting objects. */
};

/** Root object source measurement. TODO: Move to root object source only. */
struct KzuObjectSourceMeasurementInfo
{
    kzUint objectSetApplyTime; /**< Time for object set apply. */
    kzUint objectSetStartTime; /**< Time for object set start. */
    kzUint frustumCullApplyTime; /**< Time for object set apply. */
    kzUint frustumCullStartTime; /**< Time for object set start. */
    kzUint sortApplyTime; /**< Time for sort apply. Includes all types of sorting filters. */
    kzUint sortStartTime;  /**< Time for sort start. Includes all types of sorting filters. */
    kzUint objectTypeApplyTime; /**< Object type apply time. */

    kzUint lodApplyTime; /**< LOD apply time. */
    kzUint lodStartTime; /**< LOD start time. */
};

/** Object source type. */
enum KzuObjectSourceType
{
    KZU_OBJECT_SOURCE_TYPE_ROOT, /**< Use root node as object source. */
    KZU_OBJECT_SOURCE_TYPE_FILTER, /**< Use filter as object source. */
    KZU_OBJECT_SOURCE_TYPE_COMBINER, /**< Object source combiner. */
    KZU_OBJECT_SOURCE_TYPE_SORT /**< Sorting object source. */
};

/** Structure for object source. */
struct KzuObjectSource
{
    struct KzuObjectSourceData* data;
};

/** Patchable data of object source. */
struct KzuObjectSourceData
{
    kzBool selfOwned; /**< Indicates that the object source container will be deleted when this data is deleted. */
    enum KzuObjectSourceType type;  /**< Type of object source, either root or filter. */
    struct KzuObjectSourceMeasurementInfo measurement; /**< Measurement data. TODO: Remove to root object source. */
};

/** Structure for filter object source. */
struct KzuFilterObjectSource
{
    struct KzuObjectSourceData objectSource; /**< Object source. Used for inheritance. */
    struct KzuObjectSource* input; /**< Object source that goes in filter. */
    kzBool isExclusive; /**< Is this filter exclusive. */
    
    const struct KzuFilterConfiguration* configuration; /**< Filter type configuration. */
};

/** Sort object source. This sorts the object list according to specified comparator. */
struct KzuSortObjectSource
{
    struct KzuObjectSourceData objectSource; /**< Object source. Used for inheritance. */
    struct KzuObjectSource* input; /**< Input object source. */
    const struct KzuSortObjectSourceConfiguration* configuration; /**< Sort configuration. */
};

struct KzuObjectSourceRuntimeData
{
    struct KzcDynamicArray* allTransformedObjects; /**< All transformed objects of the scene. */
    struct KzcHashMap* objectSourceOutputs; /**< Cached outputs for object sources. <KzuObjectSource,KzcDynamicArray<KzuObjectNode>>. */
    struct KzcHashMap* objectSourceCacheDatas; /**< Arbitrary object source specific data. <KzuObjectSourceData,void>. */
    struct KzcHashMap* objectSourceCachedTypes; /**< Cached types for objects. <KzuObjectNode, KzcDynamicArray<KzuObjectNode>>. */
    struct KzuObjectSource* rootObjectSource; /**< Root object source. The root object source will assign itself here for later use. */
};


/** Creates an object source. */
kzsError kzuObjectSourceCreate_private(const struct KzcMemoryManager* memoryManager, enum KzuObjectSourceType type,
                                       struct KzuObjectSourceData* objectSourceData, struct KzuObjectSource** out_objectSource);

/** Creates a filter object source. */
kzsError kzuFilterObjectSourceInitialize_private(const struct KzcMemoryManager* memoryManager,
                                                 struct KzuFilterObjectSource* filterObjectSource,
                                                 struct KzuObjectSource* input, kzBool isExclusive,
                                                 const struct KzuFilterConfiguration* configuration,
                                                 struct KzuObjectSource** out_objectSource);

/** Converts an object source into a filter object source */
struct KzuFilterObjectSource* kzuFilterObjectSourceFromObjectSource_private(const struct KzuObjectSource* objectSource);

/**
 * Creates an object source which performs sorting according to the given sorting configuration.
 * \param context Arbitrary data passed to the sorting comparator function.
 */
kzsError kzuSortObjectSourceCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                   const struct KzuSortObjectSourceConfiguration* configuration,
                                   struct KzuObjectSource** out_objectSource);

/** Creates a sort object source. */
kzsError kzuSortObjectSourceInitialize_private(const struct KzcMemoryManager* memoryManager,
                                               struct KzuSortObjectSource* sortObjectSource,
                                               struct KzuObjectSource* input, 
                                               const struct KzuSortObjectSourceConfiguration* configuration,
                                               struct KzuObjectSource** out_objectSource);


/** Store the cache data for an object source. */
kzsError kzuObjectSourceSetRuntimeCacheData(const struct KzuObjectSourceData* objectSource,
                                            const struct KzuObjectSourceRuntimeData* runtimeData,
                                            void* data);

/** Get the cache data of an object source. */
void* kzuObjectSourceGetRuntimeCacheData(const struct KzuObjectSourceData* objectSource,
                                         const struct KzuObjectSourceRuntimeData* runtimeData);


#endif
