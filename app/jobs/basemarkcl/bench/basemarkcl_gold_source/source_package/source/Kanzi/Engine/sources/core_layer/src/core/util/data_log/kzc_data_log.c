/**
 * \file
 * Data logger.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 *
 * DataLog stores named datasets. Each dataset is a collection of key-value pairs where the
 * key is an unsigned integer and the value can be an unsigned or signed integer or a floating 
 * point number.
 *
 * Internally, the datasets are composed of a linked list of chunks where each chunk contains
 * an array of DataLogPoints. When a chunk is filled, a new chunk is reserved and placed after it.
*/
#include "kzc_data_log.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>


#define KZC_LOG_DEFAULT_CHUNK_ARRAY_SIZE 1024


struct KzcDataLogPoint
{
    kzUint key; /**< Key of data point. */
    union
    {
        kzUint uintValue;   /**< Value in unsigned integer format. */
        kzInt intValue;     /**< Value in signed integer format. */
        kzFloat floatValue; /**< Value in floating point format. */
    } data;                 /**< Union to hold different types of data values. */
};

/** Container for an array of DataLogPoints. Also a linked list node. */
struct KzcDataSetChunk
{
    kzUint count; /**< Number of points stored in buffer*/
    kzUint size; /**< Size of points array*/
    struct KzcDataLogPoint* points; /**< Data point array. */

    struct KzcDataSetChunk* next; /**< Next allocated DataBufferNode or KZ_NULL. */
};

/** Container for statistics */
struct KzcDataSetStatistics
{
    kzDouble minValue; /**< Minimum value*/
    kzDouble maxValue; /**< Maximum value*/
    kzDouble averageTotal; /**< Sum of scores*/
    kzDouble pointCount;  /**< Number of stored total */
    kzUint maxKey;
    kzUint minKey;
};

struct KzcDataSet
{
    struct KzcDataSetChunk* root; /**< Root chunk of the chunk list. */
    struct KzcDataSetChunk* last; /**< Last chunk of the chunk list. */

    kzUint size; /**< Number of data points stored in this set. */
    kzString name; /**< Name of dataSet. */
    struct KzcDataSetConfiguration configuration; /**< Data set configuration. Datatype and memory usage rules. */
    kzUint chunkCount; /**< Number of chunks allocated. */
    struct KzcDataSetStatistics statistics;
};

struct KzcDataLog
{
    struct KzcHashMap* dataSets; /**< <kzString, KzcDataSet>*/
    kzUint pointCount; /** Total number of points stored in log. */
};


/* Forward declarations */

/** Delete all datasets from dataLog. */
static kzsError kzcDataLogDeleteSets_internal(const struct KzcDataLog* dataLog);
/** Create a new dataset. */
static kzsError kzcDataSetCreate_internal(const struct KzcDataLog* dataLog, struct KzcDataSetConfiguration configuration, kzString name, struct KzcDataSet** out_set);
/** Delete a dataset from dataLog. */
static kzsError kzcDataSetDelete_internal(struct KzcDataSet* set);
/** Create a new chunk for DataSet */
static kzsError kzcDataSetChunkCreate_internal(const struct KzcMemoryManager* manager, kzUint arraySize, struct KzcDataSetChunk** out_chunk);
/** Delete chunk */
static kzsError kzcDataSetChunkDelete_internal(struct KzcDataSetChunk* chunk);


/** Create a new chunk for DataSet */
static kzsError kzcDataSetChunkCreate_internal(const struct KzcMemoryManager* manager, kzUint arraySize, struct KzcDataSetChunk** out_chunk)
{
    kzsError result;
    struct KzcDataSetChunk* chunk;
    kzUint i;
    kzUint totalChunkSize = sizeof(struct KzcDataSetChunk) + sizeof(struct KzcDataLogPoint) * arraySize;

    /* Allocate chunk and array at the same time. */
    result = kzcMemoryAllocPointer(manager, &chunk, totalChunkSize, "DataSetChunk and array");
    kzsErrorForward(result);

    /* Reset internal data. */
    chunk->count = 0;
    chunk->size = arraySize;
    chunk->next = KZ_NULL;

    /* Get array address*/
    chunk->points = (struct KzcDataLogPoint*)((void*)(chunk + 1));

    /* Initialize array. */
    for(i = 0; i < chunk->size; ++i)
    {
        chunk->points[i].key = 0;
        chunk->points[i].data.uintValue = 0;
    }

    *out_chunk = chunk;

    kzsSuccess();
}

static kzsError kzcDataSetChunkDelete_internal(struct KzcDataSetChunk* chunk)
{
    kzsError result;
    result = kzcMemoryFreePointer(chunk);
    kzsErrorForward(result);
    kzsSuccess();
}

static kzsError kzcDataSetCreate_internal(const struct KzcDataLog* dataLog, struct KzcDataSetConfiguration configuration, kzString name, struct KzcDataSet** out_set)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzcDataSet* dataSet;

    kzsAssert(kzcIsValidPointer(dataLog));

    memoryManager = kzcMemoryGetManager(dataLog);

    result = kzcMemoryAllocVariable(memoryManager, dataSet, "DataSet");
    kzsErrorForward(result);

    dataSet->configuration = configuration;
    dataSet->size = 0;
    dataSet->name = name;
    dataSet->chunkCount = 0;

    /* Allocate initial chunk */
    result = kzcDataSetChunkCreate_internal(memoryManager, configuration.chunkArraySize,&(dataSet->root));
    kzsErrorForward(result);

    dataSet->last = dataSet->root;

    dataSet->statistics.averageTotal = 0.0;
    dataSet->statistics.minValue = 1e100;
    dataSet->statistics.maxValue = -1e100;
    dataSet->statistics.pointCount = 0.0;
    dataSet->statistics.maxKey = 0;
    dataSet->statistics.minKey = 0xffffffff;

    *out_set = dataSet;

    kzsSuccess();
}

static kzsError kzcDataSetDelete_internal(struct KzcDataSet* set)
{
    kzsError result;
    struct KzcDataSetChunk* currentChunk;
    struct KzcDataSetChunk* nextChunk;

    kzsAssert(kzcIsValidPointer(set));

    /* Make sure no circular references exist. */
    set->last->next = KZ_NULL;

    /* Travel through all the chunks and free them. */
    currentChunk = set->root;
    nextChunk = set->root;

    while(currentChunk != KZ_NULL)
    {
        nextChunk = currentChunk->next;

        result = kzcDataSetChunkDelete_internal(currentChunk);
        kzsErrorForward(result);

        currentChunk = nextChunk;
    }

    result = kzcMemoryFreeVariable(set);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzcDataLogDeleteSets_internal(const struct KzcDataLog* dataLog)
{
    kzsError result;
    struct KzcHashMapIterator setIterator;

    kzsAssert(kzcIsValidPointer(dataLog));
    kzsAssert(kzcIsValidPointer(dataLog->dataSets));

    setIterator = kzcHashMapGetIterator(dataLog->dataSets);
    while(kzcHashMapIterate(setIterator))
    {
        result = kzcDataSetDelete_internal((struct KzcDataSet*)kzcHashMapIteratorGetValue(setIterator));
        kzsErrorForward(result);

        result = kzcHashMapRemove(dataLog->dataSets, kzcHashMapIteratorGetKey(setIterator));
        kzsErrorForward(result);

        result = kzcStringDelete((kzMutableString)kzcHashMapIteratorGetKey(setIterator));
        kzsErrorForward(result);
    }
    kzsSuccess();
}

kzsError kzcDataLogCreate(const struct KzcMemoryManager* memoryManager, struct KzcDataLog** out_dataLog)
{
    kzsError result;
    struct KzcDataLog* dataLog;

    result = kzcMemoryAllocVariable(memoryManager, dataLog, "DataLog");
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING,&dataLog->dataSets);
    kzsErrorForward(result);

    dataLog->pointCount = 0;

    *out_dataLog = dataLog;

    kzsSuccess();
}

kzsError kzcDataLogDelete(struct KzcDataLog* dataLog)
{
    kzsError result;

    /* Free memory from sets and their keys */
    result = kzcDataLogDeleteSets_internal(dataLog);
    kzsErrorForward(result);

    result = kzcHashMapDelete(dataLog->dataSets);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(dataLog);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcDataSetConfiguration kzcDataSetConfigurationInitialize(enum KzcDataSetFormat format)
{
    struct KzcDataSetConfiguration config;
    config.format = format;
    config.chunkArraySize = KZC_LOG_DEFAULT_CHUNK_ARRAY_SIZE;
    return config;
}

kzsError kzcDataLogAddDataSet(const struct KzcDataLog* dataLog, kzString name, struct KzcDataSetConfiguration config)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzcDataSet* dataSet;
    kzMutableString setName;

    kzsAssert(kzcIsValidPointer(dataLog));

    memoryManager = kzcMemoryGetManager(dataLog);

    /* Copy and allocate name */
    result = kzcStringCopy(memoryManager, name, &setName);
    kzsErrorForward(result);

    /* Allocate new datSset */
    result = kzcDataSetCreate_internal(dataLog, config, setName, &dataSet);
    kzsErrorForward(result);

    /* Push to hashMap */
    result = kzcHashMapPut(dataLog->dataSets, setName, dataSet);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcDataLogRemoveDataSet(const struct KzcDataLog* dataLog, kzString setName)
{
    kzsError result;
    struct KzcDataSet* dataSet;

    kzsAssert(dataLog != KZ_NULL);

    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        result = kzcDataSetDelete_internal(dataSet);
        kzsErrorForward(result);
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Could not find the specified dataset to remove.");
    }

    

    kzsSuccess();
}

static kzsError kzcDataLogGetSet_internal(const struct KzcDataLog* dataLog, kzString setName, enum KzcDataSetFormat format, struct KzcDataSet** out_dataSet)
{
    struct KzcDataSet* dataSet;

    kzsAssert(kzcIsValidPointer(dataLog));
    kzsAssert(kzcIsValidPointer(dataLog->dataSets));

    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        if(dataSet->configuration.format != format)
        {
            kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "DataSet format mismatch. Only values in initial type may be passed to DataSet.");
        }
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Could not find requested DataSet in DataLog.");
    }

    *out_dataSet = dataSet;

    kzsSuccess();
}

static kzsError kzcDataLogGetSetStatistics_internal(const struct KzcDataLog* dataLog, kzString setName, enum KzcDataSetFormat format, struct KzcDataSetStatistics** out_statistics)
{
    kzsError result;
    struct KzcDataSet* dataSet;

    result = kzcDataLogGetSet_internal(dataLog, setName, format, &dataSet);
    kzsErrorForward(result);

    *out_statistics = &dataSet->statistics;

    kzsSuccess();
}

static kzsError kzcDataSetAddPoint_internal(struct KzcDataSet* dataSet, struct KzcDataLogPoint** out_point)
{
    kzsError result;
    struct KzcDataSetChunk* chunk;
    struct KzcDataLogPoint* point;

    kzsAssert(kzcIsValidPointer(dataSet));
    kzsAssert(kzcIsValidPointer(dataSet->last));

    chunk = dataSet->last;

    /* Check if current chunk is full.*/
    if(chunk->count >= chunk->size)
    {
        /* Allocate new chunk. */
        struct KzcDataSetChunk* newChunk;
        struct KzcMemoryManager* manager = kzcMemoryGetManager(dataSet);

        result = kzcDataSetChunkCreate_internal(manager, dataSet->configuration.chunkArraySize, &newChunk);
        kzsErrorForward(result);

        ++dataSet->chunkCount;

        dataSet->last->next = newChunk;
        dataSet->last = newChunk;

        chunk = newChunk;
    }

    point = &(chunk->points[chunk->count]);
    ++chunk->count;

    *out_point = point;
    kzsSuccess();
}

static void kzcDataSetUpdateStatistics_internal(struct KzcDataSet* dataSet, kzUint key, kzDouble value)
{
    struct KzcDataSetStatistics* statistics = &dataSet->statistics;

    if(statistics->minValue > value) statistics->minValue = value;
    if(statistics->maxValue < value) statistics->maxValue = value;
    statistics->averageTotal += value;
    statistics->pointCount += 1.0;

    if(statistics->minKey > key) statistics->minKey = key;
    if(statistics->maxKey < key) statistics->maxKey = key;
}

static kzsError kzcDataLogAddDataPoint_internal(struct KzcDataLog* dataLog,  enum KzcDataSetFormat format, kzString setName, 
                                                kzUint key, struct KzcDataSet** out_dataSet, struct KzcDataLogPoint** out_point)
{
    kzsError result;
    struct KzcDataSet* dataSet;
    struct KzcDataLogPoint* point;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetSet_internal(dataLog, setName, format, &dataSet);
    kzsErrorForward(result);

    result = kzcDataSetAddPoint_internal(dataSet, &point);
    kzsErrorForward(result);

    ++dataLog->pointCount;

    point->key = key;
    *out_point = point;
    *out_dataSet = dataSet;
    kzsSuccess();
}

kzsError kzcDataLogAddInt(struct KzcDataLog* dataLog, kzString setName, kzUint key, kzInt value)
{
    kzsError result;
    struct KzcDataLogPoint* point;
    struct KzcDataSet* dataSet;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogAddDataPoint_internal(dataLog, KZC_LOG_DATA_FORMAT_INT, setName, key, &dataSet, &point);
    kzsErrorForward(result);

    point->data.intValue = value;
    kzcDataSetUpdateStatistics_internal(dataSet, key, (kzDouble)value);
    kzsSuccess();
}

kzsError kzcDataLogAddUint(struct KzcDataLog* dataLog, kzString setName, kzUint key, kzUint value)
{
    kzsError result;
    struct KzcDataLogPoint* point;
    struct KzcDataSet* dataSet;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogAddDataPoint_internal(dataLog, KZC_LOG_DATA_FORMAT_UINT, setName, key, &dataSet, &point);
    kzsErrorForward(result);

    point->data.uintValue = value;
    kzcDataSetUpdateStatistics_internal(dataSet,key, (kzDouble)value);
    kzsSuccess();
}

kzsError kzcDataLogAddFloat(struct KzcDataLog* dataLog, kzString setName, kzUint key, kzFloat value)
{
    kzsError result;
    struct KzcDataLogPoint* point;
    struct KzcDataSet* dataSet;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogAddDataPoint_internal(dataLog, KZC_LOG_DATA_FORMAT_FLOAT, setName, key, &dataSet, &point);
    kzsErrorForward(result);

    point->data.floatValue = value;
    kzcDataSetUpdateStatistics_internal(dataSet,key, (kzDouble)value);
    kzsSuccess();
}

static kzsError kzcDataSetGetPoint_internal(const struct KzcDataSet* dataSet, kzUint key, struct KzcDataLogPoint** out_point)
{
    struct KzcDataLogPoint* point;

    kzsAssert(kzcIsValidPointer(dataSet));

    point = KZ_NULL;

    /* First check if it possible at all to find the key */
    if(key < dataSet->statistics.minKey || key > dataSet->statistics.maxKey)
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Data key out of range");
    }
    else
    {
        struct KzcDataSetChunk* chunk = dataSet->root;
        while(chunk != KZ_NULL && point == KZ_NULL)
        {
            struct KzcDataLogPoint* points = chunk->points;
            kzUint size = chunk->size;
            kzUint i;

            for(i = 0; i < size; ++i)
            {
                if(points[i].key == key)
                {
                    point = &points[i];
                    break;
                }
            }
            chunk = chunk->next;
        }
        if(point == KZ_NULL)
        {
            kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Could not find key");
        }
    }

    *out_point = point;
    kzsSuccess();
}

static kzsError kzcDataLogGetPoint_internal(const struct KzcDataLog* dataLog, kzString setName, kzUint key, struct KzcDataLogPoint** out_point)
{
    kzsError result;
    struct KzcDataSet* dataSet;
    struct KzcDataLogPoint* point = KZ_NULL;

    /* Find if set exists*/
    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        /* Try to find key from set */
        result = kzcDataSetGetPoint_internal(dataSet, key, &point);
        kzsErrorForward(result);
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Could not find requested set");
    }

    *out_point = point;
    kzsSuccess();
}

kzsError kzcDataLogGetInt(const struct KzcDataLog* dataLog, kzString setName, kzUint key, kzInt* out_value)
{
    kzsError result;
    struct KzcDataLogPoint* point;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetPoint_internal(dataLog, setName,key, &point);
    kzsErrorForward(result);

    *out_value = point->data.intValue;

    kzsSuccess();
}

kzsError kzcDataLogGetUint(const struct KzcDataLog* dataLog, kzString setName, kzUint key, kzUint* out_value)
{
    kzsError result;
    struct KzcDataLogPoint* point;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetPoint_internal(dataLog, setName,key, &point);
    kzsErrorForward(result);

    *out_value = point->data.uintValue;

    kzsSuccess();
}

kzsError kzcDataLogGetFloat(const struct KzcDataLog* dataLog, kzString setName, kzUint key, kzFloat* out_value)
{
    kzsError result;
    struct KzcDataLogPoint* point;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetPoint_internal(dataLog, setName,key, &point);
    kzsErrorForward(result);

    *out_value = point->data.floatValue;

    kzsSuccess();
}

kzBool kzcDataLogHasDataset(const struct KzcDataLog* dataLog, kzString setName)
{
    kzBool result;
    struct KzcDataSet* dataSet;

    kzsAssert(kzcIsValidPointer(dataLog));
    kzsAssert(kzcIsValidPointer(dataLog->dataSets));

    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        result = KZ_TRUE;
    }
    else
    {
        result = KZ_FALSE;
    }

    return result;
}

kzUint kzcDataLogGetDataPointCount(const struct KzcDataLog* dataLog, kzString setName)
{
    struct KzcDataSet* dataSet;
    kzUint pointCount = 0;

    kzsAssert(kzcIsValidPointer(dataLog));
    kzsAssert(kzcIsValidPointer(dataLog->dataSets));

    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        pointCount = (kzUint) dataSet->statistics.pointCount;
    }
    return pointCount;
}

kzUint kzcDataLogGetDataPointCountInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey)
{
    struct KzcDataSet* dataSet;
    kzUint pointCount = 0;

    kzsAssert(kzcIsValidPointer(dataLog));
    kzsAssert(kzcIsValidPointer(dataLog->dataSets));

    kzsAssert(startKey <= endKey);

    /* Find set */
    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        /* Go through each chunk */
        struct KzcDataSetChunk* chunk = dataSet->root;

        while(chunk != KZ_NULL)
        {
            kzUint i;
            struct KzcDataLogPoint* points = chunk->points;
            kzUint size = chunk->count;
            for(i = 0; i < size; ++i)
            {
                if(points[i].key >= startKey && points[i].key <= endKey)
                {
                    ++pointCount;
                }
            }
            chunk = chunk->next;
        }
    }
    return pointCount;
}

kzsError kzcDataLogGetAverageFloat(const struct KzcDataLog* dataLog, kzString setName, kzFloat* out_average)
{
    kzsError result;
    struct KzcDataSetStatistics* statistics;
    kzFloat average;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetSetStatistics_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_FLOAT, &statistics);
    kzsErrorForward(result);

    average = (kzFloat)(statistics->averageTotal / statistics->pointCount);

    *out_average = average;

    kzsSuccess();
}

kzsError kzcDataLogGetAverageInt(const struct KzcDataLog* dataLog, kzString setName, kzInt* out_average)
{
    kzsError result;
    struct KzcDataSetStatistics* statistics;
    kzInt average;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetSetStatistics_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_INT, &statistics);
    kzsErrorForward(result);

    average = (kzInt)(statistics->averageTotal / statistics->pointCount);

    *out_average = average;

    kzsSuccess();
}

kzsError kzcDataLogGetAverageUint(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_average)
{
    kzsError result;
    struct KzcDataSetStatistics* statistics;
    kzUint average;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetSetStatistics_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_UINT, &statistics);
    kzsErrorForward(result);

    average = (kzUint)(statistics->averageTotal / statistics->pointCount);

    *out_average = average;

    kzsSuccess();
}

struct KzcDataPointIterator
{
    struct KzcDataLogPoint* point;
    struct KzcDataSetChunk* chunk;
    kzUint pointsVisited;
    kzUint i;
    kzUint size;
};

static kzsError kzcDataSetGetPointIterator_internal(const struct KzcDataLog* dataLog, kzString setName, enum KzcDataSetFormat format , struct KzcDataPointIterator* out_iterator)
{
    struct KzcDataPointIterator iter;
    struct KzcDataSet* dataSet;


    kzsAssert(kzcIsValidPointer(dataLog));
    kzsAssert(kzcIsValidPointer(dataLog->dataSets));

    /* Find set */
    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        kzsErrorTest(dataSet->configuration.format == format, KZS_ERROR_ILLEGAL_ARGUMENT, "Found DataSet in wrong format");
        /* Initialize iterator. */
        iter.point = KZ_NULL;
        iter.chunk = dataSet->root;
        iter.size = iter.chunk->count;
        iter.i = 0;
        iter.pointsVisited = 0;
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Did not find specified DataSet");
    }

    *out_iterator = iter;
    kzsSuccess();
}

#define kzcDataPointIteratorIntValue(iter_param)(iter_param.point->data.intValue)
#define kzcDataPointIteratorUintValue(iter_param)(iter_param.point->data.uintValue)
#define kzcDataPointIteratorFloatValue(iter_param)(iter_param.point->data.floatValue)

static kzBool kzcDataLogPointIteratePointsInRange_internal(struct KzcDataPointIterator* iter, kzUint startKey, kzUint endKey)
{
    kzBool result = KZ_FALSE;
    kzBool usable = KZ_TRUE;

    while (!result)
    {
        /* Do we need the next chunk */
        if(iter->i >= iter->size)
        {
            /* Try to get next chunk */
            iter->i = 0;
            iter->chunk = iter->chunk->next;
            if(iter->chunk != KZ_NULL)
            {
                iter->size = iter->chunk->count;
            }
            else
            {
                usable = KZ_FALSE;
            }
        }

        if(iter->i < iter->size && usable)
        {
            iter->point = &iter->chunk->points[iter->i];
            ++iter->i;
            if(iter->point->key >= startKey && iter->point->key <= endKey)
            {
                ++iter->pointsVisited;
                result = KZ_TRUE;
            }
        }
        else
        {
            /* No more points to find. */
            break;
        }
    }
    return result;
}


kzsError kzcDataLogGetAverageFloatInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzFloat* out_average)
{
    kzsError result;
    struct KzcDataPointIterator iter;
    kzDouble pointCount = 0.0;
    kzDouble average = 0.0;

    result = kzcDataSetGetPointIterator_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_FLOAT, &iter);
    kzsErrorForward(result);

    while(kzcDataLogPointIteratePointsInRange_internal(&iter, startKey, endKey))
    {
        pointCount += 1.0;
        average += (kzDouble)kzcDataPointIteratorFloatValue(iter);
    }

    if(pointCount > 0.0)
    {
        average /= pointCount;
    }

    *out_average = (kzFloat)average;
    kzsSuccess();
}

kzsError kzcDataLogGetAverageIntInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzInt* out_average)
{
    kzsError result;
    struct KzcDataPointIterator iter;
    kzDouble pointCount = 0.0;
    kzDouble average = 0.0;

    result = kzcDataSetGetPointIterator_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_INT, &iter);
    kzsErrorForward(result);

    while(kzcDataLogPointIteratePointsInRange_internal(&iter, startKey, endKey))
    {
        pointCount += 1.0;
        average += (kzDouble)kzcDataPointIteratorIntValue(iter);
    }

    if(pointCount > 0.0)
    {
        average /= pointCount;
    }

    *out_average = (kzInt)average;
    kzsSuccess();
}

kzsError kzcDataLogGetAverageUintInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzUint* out_average)
{
    kzsError result;
    struct KzcDataPointIterator iter;
    kzDouble pointCount = 0.0;
    kzDouble average = 0.0;

    result = kzcDataSetGetPointIterator_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_UINT, &iter);
    kzsErrorForward(result);

    while(kzcDataLogPointIteratePointsInRange_internal(&iter, startKey, endKey))
    {
        pointCount += 1.0;
        average += (kzDouble)kzcDataPointIteratorUintValue(iter);
    }

    if(pointCount > 0.0)
    {
        average /= pointCount;
    }

    *out_average = (kzUint)average;
    kzsSuccess();
}

kzsError kzcDataLogGetMinFloat(const struct KzcDataLog* dataLog, kzString setName, kzFloat* out_min)
{
    kzsError result;
    struct KzcDataSetStatistics* statistics;
    kzFloat min;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetSetStatistics_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_FLOAT, &statistics);
    kzsErrorForward(result);

    min = (kzFloat)statistics->minValue;

    *out_min = min;

    kzsSuccess();
}

kzsError kzcDataLogGetMinInt(const struct KzcDataLog* dataLog, kzString setName, kzInt* out_min)
{
    kzsError result;
    struct KzcDataSetStatistics* statistics;
    kzInt min;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetSetStatistics_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_INT, &statistics);
    kzsErrorForward(result);

    min = (kzInt)statistics->minValue;

    *out_min = min;

    kzsSuccess();
}

kzsError kzcDataLogGetMinUint(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_min)
{
    kzsError result;
    struct KzcDataSetStatistics* statistics;
    kzUint min;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetSetStatistics_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_UINT, &statistics);
    kzsErrorForward(result);

    min = (kzUint)statistics->minValue;

    *out_min = min;

    kzsSuccess();
}

kzsError kzcDataLogGetMinFloatInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzFloat* out_min)
{
    kzsError result;
    struct KzcDataPointIterator iter;
    kzFloat min = 0.f;

    result = kzcDataSetGetPointIterator_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_FLOAT, &iter);
    kzsErrorForward(result);

    while(kzcDataLogPointIteratePointsInRange_internal(&iter, startKey, endKey))
    {
        kzFloat value = kzcDataPointIteratorFloatValue(iter);
        if(iter.pointsVisited > 1)
        {
            if(value < min) min = value;
        }
        else
        {
            min = value;
        }
    }

    *out_min = min;
    kzsSuccess();
}

kzsError kzcDataLogGetMinIntInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzInt* out_min)
{
    kzsError result;
    struct KzcDataPointIterator iter;
    kzInt min = 0;

    result = kzcDataSetGetPointIterator_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_INT, &iter);
    kzsErrorForward(result);

    while(kzcDataLogPointIteratePointsInRange_internal(&iter, startKey, endKey))
    {
        kzInt value = kzcDataPointIteratorIntValue(iter);
        if(iter.pointsVisited > 1)
        {
            if(value < min) min = value;
        }
        else
        {
            min = value;
        }
    }

    *out_min = min;
    kzsSuccess();
}

kzsError kzcDataLogGetMinUintInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzUint* out_min)
{
    kzsError result;
    struct KzcDataPointIterator iter;
    kzUint min = 0;

    result = kzcDataSetGetPointIterator_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_UINT, &iter);
    kzsErrorForward(result);

    while(kzcDataLogPointIteratePointsInRange_internal(&iter, startKey, endKey))
    {
        kzUint value = kzcDataPointIteratorUintValue(iter);
        if(iter.pointsVisited > 1)
        {
            if(value < min) min = value;
        }
        else
        {
            min = value;
        }
    }

    *out_min = min;
    kzsSuccess();
}

kzsError kzcDataLogGetMaxFloat(const struct KzcDataLog* dataLog, kzString setName, kzFloat* out_max)
{
    kzsError result;
    struct KzcDataSetStatistics* statistics;
    kzFloat max;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetSetStatistics_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_FLOAT, &statistics);
    kzsErrorForward(result);

    max = (kzFloat)statistics->maxValue;

    *out_max = max;

    kzsSuccess();
}

kzsError kzcDataLogGetMaxInt(const struct KzcDataLog* dataLog, kzString setName, kzInt* out_max)
{
    kzsError result;
    struct KzcDataSetStatistics* statistics;
    kzInt max;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetSetStatistics_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_INT, &statistics);
    kzsErrorForward(result);

    max = (kzInt)statistics->maxValue;

    *out_max = max;

    kzsSuccess();
}

kzsError kzcDataLogGetMaxUint(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_max)
{
    kzsError result;
    struct KzcDataSetStatistics* statistics;
    kzUint max;

    kzsAssert(kzcIsValidPointer(dataLog));

    result = kzcDataLogGetSetStatistics_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_UINT, &statistics);
    kzsErrorForward(result);

    max = (kzUint)statistics->maxValue;

    *out_max = max;

    kzsSuccess();
}

kzsError kzcDataLogGetMaxFloatInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzFloat* out_max)
{
    kzsError result;
    struct KzcDataPointIterator iter;
    kzFloat max = 0.f;

    result = kzcDataSetGetPointIterator_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_FLOAT, &iter);
    kzsErrorForward(result);

    while(kzcDataLogPointIteratePointsInRange_internal(&iter, startKey, endKey))
    {
        kzFloat value = kzcDataPointIteratorFloatValue(iter);
        if(iter.pointsVisited > 1)
        {
            if(value > max) max = value;
        }
        else
        {
            max = value;
        }
    }

    *out_max = max;
    kzsSuccess();
}

kzsError kzcDataLogGetMaxIntInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzInt* out_max)
{
    kzsError result;
    struct KzcDataPointIterator iter;
    kzInt max = 0;

    result = kzcDataSetGetPointIterator_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_INT, &iter);
    kzsErrorForward(result);

    while(kzcDataLogPointIteratePointsInRange_internal(&iter, startKey, endKey))
    {
        kzInt value = kzcDataPointIteratorIntValue(iter);
        if(iter.pointsVisited > 1)
        {
            if(value > max) max = value;
        }
        else
        {
            max = value;
        }
    }

    *out_max = max;
    kzsSuccess();
}

kzsError kzcDataLogGetMaxUintInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzUint* out_max)
{
    kzsError result;
    struct KzcDataPointIterator iter;
    kzUint max = 0;

    result = kzcDataSetGetPointIterator_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_UINT, &iter);
    kzsErrorForward(result);

    while(kzcDataLogPointIteratePointsInRange_internal(&iter, startKey, endKey))
    {
        kzUint value = kzcDataPointIteratorUintValue(iter);
        if(iter.pointsVisited > 1)
        {
            if(value > max) max = value;
        }
        else
        {
            max = value;
        }
    }

    *out_max = max;
    kzsSuccess();
}

static kzsError kzcDataLogGetLastPoint_internal(const struct KzcDataLog* dataLog, kzString setName, enum KzcDataSetFormat format , struct KzcDataLogPoint** out_lastPoint)
{
    kzsError result;
    struct KzcDataSet* dataSet;
    struct KzcDataSetChunk* lastChunk;
    struct KzcDataLogPoint* lastPoint;

    result = kzcDataLogGetSet_internal(dataLog, setName, format, &dataSet);
    kzsErrorForward(result);

    lastChunk = dataSet->last;
    lastPoint = & lastChunk->points[lastChunk->count - 1];

    *out_lastPoint = lastPoint;

    kzsSuccess();
}

kzsError kzcDataLogGetLastValueFloat(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_key, kzFloat* out_value)
{
    kzsError result;
    struct KzcDataLogPoint* lastPoint;

    result = kzcDataLogGetLastPoint_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_FLOAT, &lastPoint);
    kzsErrorForward(result);

    *out_key = lastPoint->key;
    *out_value = lastPoint->data.floatValue;

    kzsSuccess();
}

kzsError kzcDataLogGetLastValueInt(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_key, kzInt* out_value)
{
    kzsError result;
    struct KzcDataLogPoint* lastPoint;

    result = kzcDataLogGetLastPoint_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_INT, &lastPoint);
    kzsErrorForward(result);

    *out_key = lastPoint->key;
    *out_value = lastPoint->data.intValue;

    kzsSuccess();
}

kzsError kzcDataLogGetLastValueUint(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_key, kzUint* out_value)
{
    kzsError result;
    struct KzcDataLogPoint* lastPoint;

    result = kzcDataLogGetLastPoint_internal(dataLog, setName, KZC_LOG_DATA_FORMAT_UINT, &lastPoint);
    kzsErrorForward(result);

    *out_key = lastPoint->key;
    *out_value = lastPoint->data.uintValue;

    kzsSuccess();
}

kzsError kzcDataLogGetMaxKey(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_key)
{
    struct KzcDataSet* dataSet;
    kzUint key = 0;

    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        key = (kzUint)dataSet->statistics.maxKey;
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Could not find DataSet in DataLog with the given name parameter.");
    }

    *out_key = key;
    kzsSuccess();
}

kzsError kzcDataLogGetMinKey(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_key)
{
    struct KzcDataSet* dataSet;
    kzUint key = 0;

    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        key = (kzUint)dataSet->statistics.minKey;
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Could not find DataSet in DataLog with the given name parameter.");
    }

    *out_key = key;
    kzsSuccess();
}

kzsError kzcDataLogGetFormat(const struct KzcDataLog* dataLog, kzString setName, enum KzcDataSetFormat* out_format)
{
    struct KzcDataSet* dataSet;
    enum KzcDataSetFormat format;

    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        format = dataSet->configuration.format;
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_ARGUMENT, "Could not find DataSet in DataLog with the given name parameter.");
    }

    *out_format = format;
    kzsSuccess();
}

kzsError kzcDataLogGetDataSetNames(const struct KzcDataLog* dataLog, kzString** out_dataSetNames)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    kzUint dataSetCount;
    kzString* dataSetNames;

    kzsAssert(dataLog != KZ_NULL);

    memoryManager = kzcMemoryGetManager(dataLog);

    dataSetCount = kzcHashMapGetSize(dataLog->dataSets);

    result = kzcMemoryAllocArray(memoryManager, dataSetNames, dataSetCount, "Data set names");
    kzsErrorForward(result);

    /* Add key strings to array */
    {
        kzUint i = 0;
        struct KzcHashMapIterator iterator = kzcHashMapGetIterator(dataLog->dataSets);

        while (kzcHashMapIterate(iterator))
        {
            dataSetNames[i++] = (kzString)kzcHashMapIteratorGetKey(iterator);
        }
    }

    *out_dataSetNames = dataSetNames;
    kzsSuccess();
}

struct KzcDataLogIterator kzcDataLogGetIterator(const struct KzcDataLog* dataLog, kzString setName)
{
    struct KzcDataLogIterator iterator;
    struct KzcDataSet* dataSet;

    kzsAssert(kzcIsValidPointer(dataLog));

    iterator.point_private = KZ_NULL;
    iterator.dataSet_private = KZ_NULL;

    if(kzcHashMapGet(dataLog->dataSets, setName, (void**)&dataSet))
    {
        iterator.dataSet_private = dataSet;
        iterator.chunk_private = dataSet->root;
        iterator.pointIndex_private = -1;
    }
    else
    {
        iterator.dataSet_private = KZ_NULL;
        iterator.chunk_private = KZ_NULL;
        iterator.pointIndex_private = -1;
        kzsAssertText(KZ_FALSE, "Could not initialize DataLogIterator - named DataSet not found.");
    }

    return iterator;
}

kzBool kzcDataLogIterate_private(struct KzcDataLogIterator* iterator)
{
    kzBool result = KZ_FALSE;

    kzsAssert(kzcIsValidPointer(iterator->dataSet_private));

    /* Next index in current chunk. */
    ++iterator->pointIndex_private;

    if((kzUint)iterator->pointIndex_private >= iterator->chunk_private->count)
    {
        iterator->chunk_private = iterator->chunk_private->next;
        if(iterator->chunk_private != KZ_NULL)
        {
            iterator->pointIndex_private = 0;
            if(iterator->chunk_private->count > 0)
            {
                iterator->point_private = 
                    &iterator->chunk_private->points[iterator->pointIndex_private];
                result = KZ_TRUE;
            }
        }
    }
    else
    {
        iterator->point_private = 
                    &iterator->chunk_private->points[iterator->pointIndex_private];
        result = KZ_TRUE;
    }

    return result;
}

enum KzcDataSetFormat kzcDataLogIteratorGetFormat_private(const struct KzcDataLogIterator* iterator)
{
    kzsAssert(kzcIsValidPointer(iterator->dataSet_private));
    return iterator->dataSet_private->configuration.format;
}

kzString kzcDataLogIteratorGetSetName_private(const struct KzcDataLogIterator* iterator)
{
    kzsAssert(kzcIsValidPointer(iterator->dataSet_private));
    return iterator->dataSet_private->name;
}

kzUint kzcDataLogIteratorGetKey_private(const struct KzcDataLogIterator* iterator)
{
    return iterator->point_private->key;
}

kzFloat kzcDataLogIteratorGetFloatValue_private(const struct KzcDataLogIterator* iterator)
{
    kzsAssert(iterator->dataSet_private->configuration.format == KZC_LOG_DATA_FORMAT_FLOAT);
    return iterator->point_private->data.floatValue;
}

kzInt kzcDataLogIteratorGetIntValue_private(const struct KzcDataLogIterator* iterator)
{
    kzsAssert(iterator->dataSet_private->configuration.format == KZC_LOG_DATA_FORMAT_INT);
    return iterator->point_private->data.intValue;
}

kzUint kzcDataLogIteratorGetUintValue_private(const struct KzcDataLogIterator* iterator)
{
    kzsAssert(iterator->dataSet_private->configuration.format == KZC_LOG_DATA_FORMAT_UINT);
    return iterator->point_private->data.uintValue;
}

