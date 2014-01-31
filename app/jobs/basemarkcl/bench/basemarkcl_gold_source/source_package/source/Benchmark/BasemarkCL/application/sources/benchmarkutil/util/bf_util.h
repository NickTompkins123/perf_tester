/**
* \file
* Benchmark framework utility functions.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef BF_UTIL_H
#define BF_UTIL_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <stdlib.h>

/* Forward declarations. */
struct BfScene;
struct BenchmarkFramework;
struct BfSceneLogName;
struct KzcMemoryManager;


/** Compose output name for screenshot file. Does not prepend the output path. Use composeOutputFilename for complete path.*/
kzsError bfComposeScreenshotName(const struct BenchmarkFramework* framework, struct BfScene* scene, kzUint frameIndex,kzMutableString* out_name);

/** Get used OpenGL api name from framework. */
kzString bfGLApiName(void);

/** Get used OpenVG api name from framework. */
kzString bfVGApiName(void);

/** Get file path, either relative to the application execution directory or, if Kanzi's resource path is non-null, then relative to the resource path. */
kzsError bfGetResourcePath(const struct KzcMemoryManager* manager, kzString path, kzMutableString* out_path);

/** Reads local workgroup size from settings. */
kzsError bfUtilReadLocalWorkgroupSize(const struct BenchmarkFramework* framework, kzString propertyName, kzUint dimensions, size_t** out_workgroupSizes);
/** Frees structure allocated for workgroup size. */
kzsError bfUtilFreeLocalWorkgroupSize(size_t* workgroupSizes);


#endif
