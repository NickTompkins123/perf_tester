/**
* \file
* Benchmark framework data structure and callbacks definition.
*
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef BF_H
#define BF_H


#include <benchmarkutil/scene/bf_scene.h>

#include <system/debug/kzs_error.h>

#include <clutil/clu_opencl_base.h>


/* Forward declarations */
struct KzsWindow;
struct KzcMemoryManager;
struct KzuEngine;
struct KzaApplication;
struct BfScene;
struct BfInputState;
struct CluInfo;

/**
 * \struct BenchmarkFramework
 * benchmark framework application data.
 */
struct BenchmarkFramework;


/** Create benchmark framework. */
kzsError bfCreate(const struct KzcMemoryManager* memoryManager, struct KzuEngine* engine, struct KzaApplication* application, struct KzsWindow* window, kzString windowTitle, struct BenchmarkFramework** out_bf);
/** Delete benchmark framework. */
kzsError bfDelete(struct BenchmarkFramework* bf);

/** Start benchmark test run. */
kzsError bfStartTestRun(struct BenchmarkFramework* framework);

/** Update benchmark framework. */
kzsError bfUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, kzBool* out_finished);

/** Render benchmark framework. */
kzsError bfRender(struct BenchmarkFramework* framework);

/** Kanzi UI event callback. */
KZ_CALLBACK kzsError bfUiEventHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters);

/** Handles key presses. */
KZ_CALLBACK kzsError bfKeyDeviceHandler(struct BenchmarkFramework* framework, struct KzaApplication* application, const struct KzsKeyDeviceInputData* inputData);
/** Handles touch events. */
KZ_CALLBACK kzsError bfPointingDeviceHandler(struct BenchmarkFramework* framework, struct KzaApplication* application, const struct KzsPointingDeviceInputData* inputData);

/** Pushes a test to queue. */
kzsError bfAddScene(const struct BenchmarkFramework* framework, struct BfScene* test);
/** Set use autorun. */
void bfSetUseAutorun(struct BenchmarkFramework* framework, kzBool autorun);
/** Get use autorun. */
kzBool bfGetUseAutorun(const struct BenchmarkFramework* framework);
/** Sets repeat count for scene queue. */
void bfSetRepeat(const struct BenchmarkFramework* framework, kzBool repeat);
/** Gets scene queue from framework. */
kzBool bfGetRepeat(const struct BenchmarkFramework* framework);


/** Gets memory manager from bf framework. */
struct KzcMemoryManager* bfGetMemoryManager(const struct BenchmarkFramework* framework);
/** Gets quick memory manager from bf framework. */
struct KzcMemoryManager* bfGetQuickManager(const struct BenchmarkFramework* framework);
/** Get application from benchmark framework. */
struct KzaApplication* bfGetApplication(const struct BenchmarkFramework* framework);
/** Get engine from benchmark framework. */
struct KzuEngine* bfGetEngine(const struct BenchmarkFramework* framework);
/** Get project from benchmark framework. */
struct KzuProject* bfGetProject(const struct BenchmarkFramework* framework);
/** Get binary directory from benchmark framework. */
struct KzuBinaryDirectory* bfGetBinaryDirectory(const struct BenchmarkFramework* framework);
/** Get window from benchmark framework. */
struct KzsWindow* bfGetWindow(const struct BenchmarkFramework* framework);

/** Get setting container from benchmark framework. */
struct KzcSettingContainer* bfGetSettings(const struct BenchmarkFramework* framework);
/** Get report logger from framework. */
struct BfReportLogger* bfGetReportLogger(const struct BenchmarkFramework* framework);

/** Gets the cl context from benchmark framework. */
cl_context bfGetClContext(const struct BenchmarkFramework* framework);
/** Sets the cl context for benchmark framework. This function should only be called by cl_menu. */
void bfSetClContext(struct BenchmarkFramework* framework, cl_context context);

/** Get CluInfo structure from benchmark framework. */
struct CluInfo* bfGetCluInfo(const struct BenchmarkFramework* framework);

/** Initializes the report document. */
kzsError bfReportCreate(struct BenchmarkFramework* framework);
/** Get bf report document. */
struct BfReportDocument* bfGetReportDocument(const struct BenchmarkFramework* framework);

/** Set path for applications which will be executed when test timing starts. Setting this to KZ_NULL disables the calls. */
kzsError bfSetTimingStartExecPath(struct BenchmarkFramework* framework, kzString startExecPath);
/** Returns the timing start test path. */
kzString bfGetTimingStartExecPath(const struct BenchmarkFramework* framework);

/** Set path for applications which will be executed when test timing stops. Setting this to KZ_NULL disables the calls. */
kzsError bfSetTimingStopExecPath(struct BenchmarkFramework* framework, kzString stopExecPath);
/** Returns the timing stop test path. */
kzString bfGetTimingStopExecPath(const struct BenchmarkFramework* framework);

/** Sets comma separated list for benchmark framework which is used to override the list of used tests. */
kzsError bfSetOverrideTests(struct BenchmarkFramework* framework, kzString overrideTests);
/** Returns the comma separated list of tests. */
kzString bfGetOverrideTestsList(const struct BenchmarkFramework* framework);

/** Gets scene queue. */
struct BfSceneQueue* bfGetSceneQueue(const struct BenchmarkFramework* framework);

/** BF full profile. */
kzsError bfGetCLFullProfile(const struct BenchmarkFramework* framework, kzBool* out_fullProfile);


#endif
