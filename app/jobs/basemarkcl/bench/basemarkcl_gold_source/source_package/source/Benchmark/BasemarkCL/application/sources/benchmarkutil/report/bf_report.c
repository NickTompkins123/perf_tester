/**
* \file
* Benchmark timing and report utilities.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "bf_report.h"

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/debug/kzc_log.h>

#include <system/time/kzs_tick.h>
#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_math.h>

#include "bf_timer.h"


struct BfReportLogger
{
    kzUint frameIndex; /**< Frame index. */
    kzUint lastTime; /**< Last time stored. */
    kzUint maximumFrameTimes; /**< Maximum frame times. */
    kzUint* frameTimes; /**< Frame times array. */
    kzBool isPartOfScore; /**< Is this scene calculated into the overall score. */
    struct BfTimer* timer;
};


kzsError bfReportLoggerCreate(const struct KzcMemoryManager* memoryManager, struct BfReportLogger** out_logger)
{
    kzsError result;
    struct BfReportLogger* logger;

    result = kzcMemoryAllocVariable(memoryManager, logger, "Report Logger");
    kzsErrorForward(result);
    logger->frameTimes = KZ_NULL;
    logger->maximumFrameTimes = 0;
    logger->isPartOfScore = KZ_TRUE;

    result = bfTimerCreate(memoryManager, &logger->timer);
    kzsErrorForward(result);

    *out_logger = logger;
    kzsSuccess();
}

kzsError bfReportLoggerDelete(struct BfReportLogger* logger)
{
    kzsError result;

    result = bfTimerDelete(logger->timer);
    kzsErrorForward(result);

    if(logger->maximumFrameTimes > 0)
    {
        result = kzcMemoryFreeArray(logger->frameTimes);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(logger);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfReportLoggerReset(struct BfReportLogger* logger, kzString logName, kzUint frames, kzBool isPartOfOverallScore)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    
    kzsAssert(kzcIsValidPointer(logger));

    memoryManager = kzcMemoryGetManager(logger);

    if(logger->maximumFrameTimes > 0)
    {
        result = kzcMemoryFreeArray(logger->frameTimes);
        kzsErrorForward(result);
    }
    if(frames > 0)
    {
        result = kzcMemoryAllocArray(memoryManager, logger->frameTimes, frames, "Frame times array");
        kzsErrorForward(result);
    }
    else
    {
        logger->frameTimes = KZ_NULL;
    }
    logger->maximumFrameTimes = frames;
    logger->isPartOfScore = isPartOfOverallScore;

    kzsLog(KZS_LOG_LEVEL_INFO, "");
    kzcLogDebug("Running test: %s", logName);

    logger->frameIndex = 0;
    kzsSuccess();
}

kzsError bfReportLoggerUpdatePreFrame(struct BfReportLogger* logger)
{
    logger->lastTime = (kzUint)bfTimerGetElapsedTimeInMicroSeconds(logger->timer);
    kzsSuccess();
}

kzsError bfReportLoggerUpdatePostFrame(struct BfReportLogger* logger)
{
    kzUint timeNow = (kzUint)bfTimerGetElapsedTimeInMicroSeconds(logger->timer);
    kzUint delta = kzsMaxU(timeNow - logger->lastTime, 1);

    kzsErrorTest(logger->frameIndex < logger->maximumFrameTimes, KZS_ERROR_ARRAY_OUT_OF_BOUNDS, "Array out of bounds for frametime logger.");
    logger->frameTimes[logger->frameIndex] = delta;

    logger->frameIndex++;
    kzcLogDebug("Frame %d duration %d (us)", logger->frameIndex, delta);

    kzsSuccess();
}

kzUint bfReportLoggerGetFrameCount(const struct BfReportLogger* logger)
{
    kzsAssert(kzcIsValidPointer(logger));
    return logger->maximumFrameTimes;    
}

kzUint* bfReportLoggerGetFrameTimesArray(const struct BfReportLogger* logger)
{
    kzsAssert(kzcIsValidPointer(logger));
    return logger->frameTimes;
}

kzBool bfReportLoggerIsPartOfOverallScore(const struct BfReportLogger* logger)
{
    kzsAssert(kzcIsValidPointer(logger));
    return logger->isPartOfScore;
}
