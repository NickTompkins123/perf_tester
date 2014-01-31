/**
 * \file
 * OpenCL Atomic tests
 * Copyright 2011 by Rightware. All rights reserved.
 */
#include "cl_loadscreen.h"

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>

#include <application/kza_application.h>

#include <core/memory/kzc_memory_manager.h>


kzsError loadScreenLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError loadScreenLoadNoGL(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError loadScreenLoadScreenshot(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError loadScreenUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError loadScreenUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);


KZ_CALLBACK kzsError loadScreenLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    bfSceneSetFrameCounter(scene, 2);
    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/IntervalSceneLoad");
    kzsErrorForward(result);
    kzsSuccess();
}

KZ_CALLBACK kzsError loadScreenLoadNoGL(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/IntervalSceneNonGlRender");
    kzsErrorForward(result);
    kzsSuccess();
}

KZ_CALLBACK kzsError loadScreenLoadScreenshot(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/IntervalSceneScreenShots");
    kzsErrorForward(result);
    kzsSuccess();
}

KZ_CALLBACK kzsError loadScreenUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsSuccess();
}

KZ_CALLBACK kzsError loadScreenUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    kzsSuccess();
}

kzsError clLoadScreenCreate(const struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;

    result = bfTestConfigurationInitialize(memoryManager, loadScreenLoad, loadScreenUpdate, KZ_NULL,
        loadScreenUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);
    configuration->isBenchmarkedScene = KZ_FALSE;
    result = bfSceneCreate(framework, configuration, "Load screen", "Intermediate", KZ_NULL, &scene);
    kzsErrorForward(result);

    *out_scene = scene;
    kzsSuccess();
}

kzsError clLoadScreenCreateScreenshot(const struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;

    result = bfTestConfigurationInitialize(memoryManager, loadScreenLoadScreenshot, loadScreenUpdate, KZ_NULL,
        loadScreenUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);
    configuration->isBenchmarkedScene = KZ_FALSE;
    result = bfSceneCreate(framework, configuration, "Load screen", "Intermediate", KZ_NULL, &scene);
    kzsErrorForward(result);

    *out_scene = scene;
    kzsSuccess();
}

kzsError clLoadScreenCreateNoGl(const struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;

    result = bfTestConfigurationInitialize(memoryManager, loadScreenLoadNoGL, loadScreenUpdate, KZ_NULL,
        loadScreenUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);
    configuration->isBenchmarkedScene = KZ_FALSE;
    result = bfSceneCreate(framework, configuration, "Load screen", "Intermediate", KZ_NULL, &scene);
    kzsErrorForward(result);

    *out_scene = scene;
    kzsSuccess();
}
