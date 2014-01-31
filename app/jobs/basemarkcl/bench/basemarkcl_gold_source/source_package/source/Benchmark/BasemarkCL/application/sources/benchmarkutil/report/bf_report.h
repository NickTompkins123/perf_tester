/**
* \file
* Benchmark timing and report utilities.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef BF_REPORT_H
#define BF_REPORT_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;

/**
* \struct BfReportGenerator
* Contains the utilities for storing frame times for generating report.
*/
struct BfReportLogger;


/** Creates report generator struct. */
kzsError bfReportLoggerCreate(const struct KzcMemoryManager* memoryManager, struct BfReportLogger** out_logger);

/** Deletes report generator struct. */
kzsError bfReportLoggerDelete(struct BfReportLogger* logger);

/** Resets a logger. */
kzsError bfReportLoggerReset(struct BfReportLogger* logger, kzString logName, kzUint frames, kzBool isPartOfOverallScore);

/** Updates logger pre frame. */
kzsError bfReportLoggerUpdatePreFrame(struct BfReportLogger* logger);
/** Updates logger post frame. */
kzsError bfReportLoggerUpdatePostFrame(struct BfReportLogger* logger);

/** Gets the frame count from report logger. */
kzUint bfReportLoggerGetFrameCount(const struct BfReportLogger* logger);
/** Gets the frame duration array from report logger. */
kzUint* bfReportLoggerGetFrameTimesArray(const struct BfReportLogger* logger);

/** Returns KZ_TRUE if current scene is part of overall score. False if not. */
kzBool bfReportLoggerIsPartOfOverallScore(const struct BfReportLogger* logger);


#endif
