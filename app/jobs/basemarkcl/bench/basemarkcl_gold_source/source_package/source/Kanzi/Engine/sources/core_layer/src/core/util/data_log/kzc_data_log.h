/**
* \file
* Data log.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_DATA_LOG_H
#define KZC_DATA_LOG_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzcDynamicArray;


/** Data format of dataset. */
enum KzcDataSetFormat
{
    KZC_LOG_DATA_FORMAT_INT,  /**< Data set contains ints. */
    KZC_LOG_DATA_FORMAT_UINT, /**< Data set contains uints. */
    KZC_LOG_DATA_FORMAT_FLOAT /**< Data set contains floats. */
};


/**
 * \struct KzcDataLog
 * Logs data.
 */
struct KzcDataLog;

/**
 * \struct KzcDataSet
 * Contains a set of data points.
 */
struct KzcDataSet;

/**
 * \struct KzcDataSetChunk
 * Contains an array of data points.
 */
struct KzcDataSetChunk;

/**
 * \struct KzcDataLogPoint
 * Contains single point of data.
 */
struct KzcDataLogPoint;

/** Iterator for a single dataset. */
struct KzcDataLogIterator
{
    struct KzcDataSet* dataSet_private; /**< DataSet to iterate through. */
    struct KzcDataLogPoint* point_private; /**< Current data point. */
    kzInt pointIndex_private; /**< Index of current data point. */
    struct KzcDataSetChunk* chunk_private; /**< Current chunk iterated through. */
};

/** Configuration for a single dataset. */
struct KzcDataSetConfiguration
{
    kzUint chunkArraySize;         /**< Number of elements to store for each internal chunk */
    enum KzcDataSetFormat format;  /**< Dataset format <int|uint|float>. */
};


/** Create DataLog */
kzsError kzcDataLogCreate(const struct KzcMemoryManager* memoryManager, struct KzcDataLog** out_dataLog);
/** Delete DataLog */
kzsError kzcDataLogDelete(struct KzcDataLog* dataLog);

/** Initialize a DataSetConfiguration. */
struct KzcDataSetConfiguration kzcDataSetConfigurationInitialize(enum KzcDataSetFormat format);

/** Add dataset with given data type */
kzsError kzcDataLogAddDataSet(const struct KzcDataLog* dataLog, kzString name, struct KzcDataSetConfiguration config);
/** Delete dataset. Return error if could not remove the dataset. */
kzsError kzcDataLogRemoveDataSet(const struct KzcDataLog* dataLog, kzString setName);

/** 
 * Set integer number 'value' for dataset 'name' in position 'key'.
 * \param[in] dataLog log container
 * \param[in] setName of dataset
 * \param[in] key to store value into
 * \param[in] value in integer format to store
 */
kzsError kzcDataLogAddInt(struct KzcDataLog* dataLog, kzString setName, kzUint key, kzInt value);

/** 
 * Set unsigned integer 'value' for dataset 'name' in position 'key'.
 * \param[in] dataLog log container
 * \param[in] setName of dataset
 * \param[in] key to store value into
 * \param[in] value in unsigned format to store
 */
kzsError kzcDataLogAddUint(struct KzcDataLog* dataLog, kzString setName, kzUint key, kzUint value);

/** 
 * Set floating point number 'value' for dataset 'name' in position 'key'.
 * \param[in] dataLog log container
 * \param[in] setName of dataset
 * \param[in] key to store value into
 * \param[in] value in floating point format to store
 */
kzsError kzcDataLogAddFloat(struct KzcDataLog* dataLog, kzString setName, kzUint key, kzFloat value);

/** Get integer value in data point 'key' */
kzsError kzcDataLogGetInt(const struct KzcDataLog* dataLog, kzString setName, kzUint key, kzInt* out_value);
/** Get unsigned value in data point 'key' */
kzsError kzcDataLogGetUint(const struct KzcDataLog* dataLog, kzString setName, kzUint key, kzUint* out_value);
/** Get floating point value in data point 'key' */
kzsError kzcDataLogGetFloat(const struct KzcDataLog* dataLog, kzString setName, kzUint key, kzFloat* out_value);

/** Does dataLog contain dataset 'name' */
kzBool kzcDataLogHasDataset(const struct KzcDataLog* dataLog, kzString setName);
/** How many key-value data points are stored to dataset 'name' */
kzUint kzcDataLogGetDataPointCount(const struct KzcDataLog* dataLog, kzString setName);
/** How many key-value data points are stored to dataset 'name' within interval startKey and endKey */
kzUint kzcDataLogGetDataPointCountInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endkey);

/** Get average of values within complete dataset. Throws error on wrong format or name.*/
kzsError kzcDataLogGetAverageFloat(const struct KzcDataLog* dataLog, kzString setName, kzFloat* out_average);
/** Get average of values within complete dataset.. Throws error on wrong format or name.*/
kzsError kzcDataLogGetAverageInt(const struct KzcDataLog* dataLog, kzString setName, kzInt* out_average);
/** Get average of values within complete dataset.. Throws error on wrong format or name.*/
kzsError kzcDataLogGetAverageUint(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_average);

/** Get average of values with keys within the range startKey and endKey. Throws error on wrong format or name.*/
kzsError kzcDataLogGetAverageFloatInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzFloat* out_average);
/** Get average of values with keys within the range startKey and endKey. Throws error on wrong format or name.*/
kzsError kzcDataLogGetAverageIntInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzInt* out_average);
/** Get average of values with keys within the range startKey and endKey. Throws error on wrong format or name.*/
kzsError kzcDataLogGetAverageUintInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzUint* out_average);

/** Get minimum floating point value in dataset. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMinFloat(const struct KzcDataLog* dataLog, kzString setName, kzFloat* out_average);
/** Get minimum integer value in dataset. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMinInt(const struct KzcDataLog* dataLog, kzString setName, kzInt* out_average);
/** Get minimum unsigned integer value in dataset. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMinUint(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_average);

/** Get minimum floating point value in dataset within range startKey and endKey. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMinFloatInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzFloat* out_average);
/** Get minimum integer point value in dataset within range startKey and endKey. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMinIntInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzInt* out_average);
/** Get minimum unsigned integer point value in dataset within range startKey and endKey. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMinUintInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzUint* out_average);

/** Get maximum floating point value in dataset. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMaxFloat(const struct KzcDataLog* dataLog, kzString setName, kzFloat* out_average);
/** Get maximum integer value in dataset. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMaxInt(const struct KzcDataLog* dataLog, kzString setName, kzInt* out_average);
/** Get maximum unsigned integer value in dataset. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMaxUint(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_average);

/** Get maximum floating point value in dataset within range startKey and endKey. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMaxFloatInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzFloat* out_average);
/** Get maximum integer value in dataset within range startKey and endKey. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMaxIntInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzInt* out_average);
/** Get maximum unsigned integer value in dataset within range startKey and endKey. Throws error on wrong format or name.*/
kzsError kzcDataLogGetMaxUintInRange(const struct KzcDataLog* dataLog, kzString setName, kzUint startKey, kzUint endKey, kzUint* out_average);

/** Get last floating point value entered in dataset. Throws error on wrong format or name.*/
kzsError kzcDataLogGetLastValueFloat(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_key, kzFloat* out_value);
/** Get last integer value entered in dataset. Throws error on wrong format or name.*/
kzsError kzcDataLogGetLastValueInt(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_key, kzInt* out_value);
/** Get last unsigned integer value entered in dataset. Throws error on wrong format or name.*/
kzsError kzcDataLogGetLastValueUint(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_key, kzUint* out_value);

/** Get maximum key value within dataset. */
kzsError kzcDataLogGetMaxKey(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_key);
/** Get minimum key value within dataset. */
kzsError kzcDataLogGetMinKey(const struct KzcDataLog* dataLog, kzString setName, kzUint* out_key);

/** Get format of dataset */
kzsError kzcDataLogGetFormat(const struct KzcDataLog* dataLog, kzString setName,enum KzcDataSetFormat* out_format);

/**
 * Get names for all of the DataSets contained within the datalog in an array.
 * The array must be freed after use with kzcMemoryFreeArray().
 */
kzsError kzcDataLogGetDataSetNames(const struct KzcDataLog* dataLog, kzString** out_dataSetNames);

/** Get iterator for dataset setName */
struct KzcDataLogIterator kzcDataLogGetIterator(const struct KzcDataLog* dataLog, kzString setName);

/** Iterator. Walks through data points in set in order which they were entered into the set.*/
#define kzcDataLogIterate(iterator_param) kzcDataLogIterate_private(&iterator_param)

/** Get dataset format. */
#define kzcDataLogIteratorGetFormat(iterator_param) kzcDataLogIteratorGetFormat_private(&iterator_param)
/** Get dataset name. */
#define kzcDataLogIteratorGetSetName(iterator_param) kzcDataLogIteratorGetSetName_private(&iterator_param)
/** Get current key. */
#define kzcDataLogIteratorGetKey(iterator_param) kzcDataLogIteratorGetKey_private(&iterator_param)
/** Get current floating point value. */
#define kzcDataLogIteratorGetFloatValue(iterator_param) kzcDataLogIteratorGetFloatValue_private(&iterator_param)
/** Get current integer value. */
#define kzcDataLogIteratorGetIntValue(iterator_param) kzcDataLogIteratorGetIntValue_private(&iterator_param)
/** Get current unsigned integer value. */
#define kzcDataLogIteratorGetUintValue(iterator_param) kzcDataLogIteratorGetUintValue_private(&iterator_param)

/** \see kzcDataLogIterate */
kzBool kzcDataLogIterate_private(struct KzcDataLogIterator* iterator);
/** \see kzcDataLogIteratorGetFormat */
enum KzcDataSetFormat kzcDataLogIteratorGetFormat_private(const struct KzcDataLogIterator* iterator);
/** \see kzcDataLogIteratorGetSetName */
kzString kzcDataLogIteratorGetSetName_private(const struct KzcDataLogIterator* iterator);
/** \see kzcDataLogIteratorGetKey */
kzUint kzcDataLogIteratorGetKey_private(const struct KzcDataLogIterator* iterator);
/** \see kzcDataLogIteratorGetFloatValue */
kzFloat kzcDataLogIteratorGetFloatValue_private(const struct KzcDataLogIterator* iterator);
/** \see kzcDataLogIteratorGetIntValue */
kzInt kzcDataLogIteratorGetIntValue_private(const struct KzcDataLogIterator* iterator);
/** \see kzcDataLogIteratorGetUintValue */
kzUint kzcDataLogIteratorGetUintValue_private(const struct KzcDataLogIterator* iterator);


#endif
