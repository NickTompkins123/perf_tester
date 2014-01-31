/**
* \file
* Benchmark framework screenshot utility.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef BF_SCREENSHOT_H
#define BF_SCREENSHOT_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


struct BenchmarkFramework;
struct KzcMemoryManager;
struct KzcImage;


/** Write KzcImage to a file. */
kzsError bfWriteImageToFile(const struct KzcMemoryManager* manager, kzString path,const  struct KzcImage* image);

/* Write current frame as screenshot. */
kzsError bfSaveScreenshot(const struct BenchmarkFramework* bf);

/* Write a file by the given name */
kzsError bfSaveScreenshotToFile(const struct BenchmarkFramework* bf, kzString fileName);


#endif
