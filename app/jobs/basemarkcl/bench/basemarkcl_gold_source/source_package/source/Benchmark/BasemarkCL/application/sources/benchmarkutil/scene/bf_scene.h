/**
* \file
* Benchmark scene.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef BENCHMARK_SCENE_DATA_H
#define BENCHMARK_SCENE_DATA_H


#include <user/ui/kzu_ui_action.h>

#include <core/util/data_log/kzc_stopwatch.h>
#include <core/util/collection/kzc_dynamic_array.h>


/* Forward declarations */
struct KzcDataLog;
struct KzcStopwatch;
struct KzcMemoryManager;
struct BenchmarkFramework;
struct BfTestLogNameCollection;
struct BfReportLogger;


/**
 * \struct BfTest
 * Test runtime data.
 */
struct BfScene;


/** Load scene function prototype. */
typedef kzsError (*BenchmarkInitializeTestPtr)(struct BenchmarkFramework* framework, struct BfScene* scene);
/** Update scene function prototype. */
typedef kzsError (*BenchmarkUpdateTestPtr)(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
/** Render scene function prototype. */
typedef kzsError (*BenchmarkRenderTestPtr)(struct BenchmarkFramework* framework, struct BfScene* scene);
/** Free scene function prototype. */
typedef kzsError (*BenchmarkUninitializeTestPtr)(struct BenchmarkFramework* framework, struct BfScene* scene);
/** Handle scene pointer input function prototype. */
typedef kzsError (*BenchmarkTouchInputPtr)(struct BenchmarkFramework* framework, const struct KzsPointingDeviceInputData* inputData, struct BfScene* scene);
/** Handle scene key input function prototype. */
typedef kzsError (*BenchmarkKeyInputPtr)(struct BenchmarkFramework* framework, const struct KzsKeyDeviceInputData* inputData, struct BfScene* scene);
/** Preupdate scene function prototype. This is called before scene is updated. */
typedef kzsError (*BenchmarkPreUpdateTestPtr)(struct BenchmarkFramework* framework, struct BfScene* scene);
/** Postupdate scene function prototype. This is called after scene is updated. */
typedef kzsError (*BenchmarkPostUpdateTestPtr)(struct BenchmarkFramework* framework, struct BfScene* scene);


/** Scene configuration struct */
struct BfSceneConfiguration
{
    BenchmarkInitializeTestPtr load_private; /**< Scene loader function.*/
    BenchmarkUpdateTestPtr update_private; /**< Scene update function.*/
    BenchmarkRenderTestPtr render_private; /**< Scene render function.*/
    BenchmarkUninitializeTestPtr free_private; /**< Scene finalization function.*/
    BenchmarkTouchInputPtr touch_private; /**< Scene touch input handler. */
    BenchmarkKeyInputPtr key_private; /**< Scene key input handler. */
    KzuUiActionCallback uiAction; /**< Kanzi User Interface action callback, if any. */
    BenchmarkPreUpdateTestPtr preUpdate_private; /**< Scene prepare before update. */ 
    BenchmarkPostUpdateTestPtr postUpdate_private; /**< Scene clean up after update. */
    kzBool isBenchmarkedScene; /**< Indicates that benchmarking is enabled for the scene. Disable this for loading bars etc. */
};


/** Returns the ui action callback associated with the scene. */
KzuUiActionCallback bfSceneGetUiActionCallback(struct BfScene* sceneData);

/** Initialize a BenchmarkConfiguration struct. */
kzsError bfTestConfigurationInitialize(const struct KzcMemoryManager* memoryManager, BenchmarkInitializeTestPtr load, BenchmarkUpdateTestPtr update, 
    BenchmarkRenderTestPtr render, BenchmarkUninitializeTestPtr finalize, BenchmarkTouchInputPtr touchInput, BenchmarkKeyInputPtr keyInput, KzuUiActionCallback uiAction,
    BenchmarkPreUpdateTestPtr preUpdate, BenchmarkPostUpdateTestPtr postUpdate, struct BfSceneConfiguration** out_configuration);

/** Create BfScene. */
kzsError bfSceneCreate(const struct BenchmarkFramework* framework, struct BfSceneConfiguration* sceneConfiguration, kzString testName, kzString type,
                       void* userData, struct BfScene** out_sceneData);
/** Delete BfScene. */
kzsError bfSceneDelete(struct BfScene* sceneData);

/** Checks that all the function pointers have been set. */
kzBool bfSceneConfigurationIsValid(const struct BfSceneConfiguration* configuration);


/** Initialize scene.*/
kzsError bfSceneInitialize(struct BenchmarkFramework* framework, struct BfScene* scene);
/** Update scene data.*/
kzsError bfSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene, kzBool* out_sceneComplete);
/** Render scene. */
kzsError bfSceneRender(struct BenchmarkFramework* framework, struct BfScene* scene);
/** Uninitialize scene.*/
kzsError bfSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);
/** Handle pointer input. */
kzsError bfSceneHandlePointerInput(struct BenchmarkFramework* framework, struct BfScene* scene, const struct KzsPointingDeviceInputData* inputData);
/** Handle key input. */
kzsError bfSceneHandleKeyInput(struct BenchmarkFramework* framework, struct BfScene* scene, const struct KzsKeyDeviceInputData* inputData);

/** Returns the scene data from the scene struct. */
void* bfSceneGetUserData(const struct BfScene* scene);

/* Returns the profiler for this scene */
struct CluProfiler* bfSceneGetProfiler(const struct BfScene* scene);

/** Initialize frame counter. Test will run this amount of frames. */
void bfSceneSetFrameCounter(struct BfScene* sceneData, kzUint frameCount);
/** Get current remaining frames count. */
kzUint bfSceneGetFrameCounterValue(const struct BfScene* sceneData);
/** Get frame counter initial value. How many frames is the scene set to run in total. */
kzUint bfSceneGetFrameInitialCounterValue(const struct BfScene* sceneData);
/** Sets scene to not be included in overall benchmark score. */
void bfSceneDisableFromOverallScore(struct BfScene* sceneData);

/** Is valid configuration. */
kzBool bfSceneGetValidConfigurationData(const struct BfScene* sceneData);
/** Set configuration to valid or invalid. */
void bfSceneSetValidConfigurationData(struct BfScene* sceneData, kzBool valid);

/** Sets scene score weight factor for final score calculation. */
void bfSceneSetScoreWeightFactor(struct BfScene* sceneData, kzFloat sceneWeight);

/** Uses binary kernels. */
kzBool bfSceneGetUseProgramBinary(const struct BfScene* sceneData);
/** Set binary kernels usage. */
void bfSceneSetUseProgramBinary(struct BfScene* sceneData, kzBool useBinary);


#endif
