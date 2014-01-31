/**
* \file
* Benchmark scene runtime data storage
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "bf_scene.h"

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/settings/bf_settings.h>
#include <benchmarkutil/report/bf_report_document.h>
#include <benchmarkutil/report/bf_report.h>
#include <benchmarkutil/report/bf_info.h>

#include <user/engine/kzu_engine.h>
#include <user/ui/kzu_ui_action.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/data_log/kzc_stopwatch.h>
#include <core/util/data_log/kzc_data_log.h>
#include <core/util/string/kzc_string.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_dynamic_array.h>

#include <system/time/kzs_time.h>
#include <system/wrappers/kzs_opengl.h>
#include <system/wrappers/kzs_openvg.h>
#include <clutil/clu_profiler.h>

struct BfScene
{
    struct BfSceneConfiguration* configuration; /**< Scene function pointers and options. */
    void* sceneData_private; /**< Scene data. */   
    kzMutableString sceneName; /**< Name of the scene. */
    kzMutableString sceneCategory; /**< Category of the scene. Used to separate tests in the generated report file. */
    kzBool isPartOfScore; /**< Is scene part of overall score. */
    kzUint frameCounter; /**< Number of frames to run the scene. */
    kzUint frameCounterInitialValue; /**< Number of frames to run the scene. */
    kzBool validData; /**< Does scene have valid configuration. Proper CL Kernels etc. */
    kzBool usingBinaryProgram; /**< Does scene use binary kernels. */
    struct CluProfiler *profiler; /* Profiler for this scene */
    kzFloat scoreWeightFactor; /**< How much weight does this test bring to overall score. */
};


KZ_CALLBACK KzuUiActionCallback bfSceneGetUiActionCallback(struct BfScene* sceneData)
{
    kzsAssert(kzcIsValidPointer(sceneData));
    return sceneData->configuration->uiAction;
}

kzsError bfTestConfigurationInitialize(const struct KzcMemoryManager* memoryManager, BenchmarkInitializeTestPtr load, BenchmarkUpdateTestPtr update, BenchmarkRenderTestPtr render, 
    BenchmarkUninitializeTestPtr finalize, BenchmarkTouchInputPtr touchInput, BenchmarkKeyInputPtr keyInput, KzuUiActionCallback uiAction, 
    BenchmarkPreUpdateTestPtr preUpdate, BenchmarkPostUpdateTestPtr postUpdate, struct BfSceneConfiguration** out_configuration)
{
    kzsError result;
    struct BfSceneConfiguration* configuration;

    result = kzcMemoryAllocVariable(memoryManager, configuration, "Scene configuration data");
    kzsErrorForward(result);

    configuration->load_private = load;
    configuration->update_private = update;
    configuration->render_private = render;
    configuration->free_private = finalize;
    configuration->touch_private = touchInput;
    configuration->key_private = keyInput;
    configuration->uiAction = uiAction;
    configuration->preUpdate_private = preUpdate;
    configuration->postUpdate_private = postUpdate;
    configuration->isBenchmarkedScene = KZ_TRUE;

    *out_configuration = configuration;
    kzsSuccess();
}

kzBool bfSceneConfigurationIsValid(const struct BfSceneConfiguration* configuration)
{
    /* Other callbacks can be left NULL. */
    return (configuration->load_private != KZ_NULL && configuration->update_private != KZ_NULL && configuration->free_private != KZ_NULL);
}

kzsError bfSceneCreate(const struct BenchmarkFramework* framework, struct BfSceneConfiguration* sceneConfiguration, kzString sceneName, kzString category, 
                       void* userData, struct BfScene** out_sceneData)
{
    kzsError result;
    struct BfScene* sceneData;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcIsValidPointer(framework));

    memoryManager = kzcMemoryGetManager(framework);

    result = kzcMemoryAllocVariable(memoryManager, sceneData, "Benchmark scene data");
    kzsErrorForward(result);

    /* Verify scene configuration */
    kzsAssertText(bfSceneConfigurationIsValid(sceneConfiguration), "Invalid configuration. Init, uninit and update must be set.");

    result = kzcStringCopy(memoryManager, sceneName, &sceneData->sceneName);
    kzsErrorForward(result);
    result = kzcStringCopy(memoryManager, category, &sceneData->sceneCategory);
    kzsErrorForward(result);
    {
        kzInt profilingEnabled;
        result = settingGetInt(bfGetSettings(framework), "EnableProfiling", &profilingEnabled);
        kzsErrorForward(result);
        result = cluProfilerCreate(memoryManager, 1024, (profilingEnabled == 1), &sceneData->profiler);
        kzsErrorForward(result);
    }


    sceneData->configuration = sceneConfiguration;
    sceneData->sceneData_private = userData;
    sceneData->frameCounter = 0;
    sceneData->frameCounterInitialValue = 0;
    sceneData->isPartOfScore = KZ_TRUE;
    sceneData->validData = KZ_TRUE;
    sceneData->usingBinaryProgram = KZ_FALSE;
    sceneData->scoreWeightFactor = 1.0f;

    *out_sceneData = sceneData;
    kzsSuccess();
}

kzsError bfSceneDelete(struct BfScene* sceneData)
{
    kzsError result;
    kzsAssert(sceneData != KZ_NULL);

    /* Loading screens do not have any user data stored. */
    if(bfSceneGetUserData(sceneData) != KZ_NULL)
    {
        result = kzcMemoryFreeVariable(bfSceneGetUserData(sceneData));    
        kzsErrorForward(result);
    }

    result = kzcStringDelete(sceneData->sceneCategory);
    kzsErrorForward(result);
    result = kzcStringDelete(sceneData->sceneName);
    kzsErrorForward(result);

    if(sceneData->profiler != KZ_NULL)
    {
        result = cluProfilerDelete(sceneData->profiler);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(sceneData->configuration);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(sceneData);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfSceneInitialize(struct BenchmarkFramework* framework, struct BfScene* sceneData)
{
    kzsError result;
    struct BfReportLogger* reportLogger;

    kzsAssert(kzcIsValidPointer(framework));
    kzsAssert(kzcIsValidPointer(sceneData));
    
    sceneData->frameCounterInitialValue = 0;
    sceneData->frameCounter = 0;
    sceneData->isPartOfScore = KZ_TRUE;

    reportLogger = bfGetReportLogger(framework);

    result = sceneData->configuration->load_private(framework, sceneData);
    kzsErrorForward(result);

    if(sceneData->configuration->isBenchmarkedScene)
    {
        result = bfReportLoggerReset(reportLogger, sceneData->sceneName, sceneData->frameCounter, sceneData->isPartOfScore);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError bfSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* sceneData, kzBool* out_sceneComplete)
{
    kzsError result;
    struct BfReportLogger* reportLogger;
    kzBool sceneComplete;

    kzsAssert(kzcIsValidPointer(framework));

    reportLogger = bfGetReportLogger(framework);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    glFinish();
#endif

    if(sceneData->configuration->isBenchmarkedScene)
    {
        /* Execute the preframe executable. */
        if(bfGetTimingStartExecPath(framework) != KZ_NULL)
        {
            kzInt value = (kzInt)system(bfGetTimingStartExecPath(framework));
            KZ_UNUSED_RETURN_VALUE(value);
        }

        if(sceneData->configuration->preUpdate_private != KZ_NULL)
        {
            result = sceneData->configuration->preUpdate_private(framework, sceneData);
            kzsErrorForward(result);
        }

        result = bfReportLoggerUpdatePreFrame(reportLogger);
        kzsErrorForward(result);
    }

    result = sceneData->configuration->update_private(framework, deltaTime, sceneData);
    kzsErrorForward(result);

    sceneData->frameCounter--;
    if(sceneData->frameCounter == 0)
    {
        sceneComplete = KZ_TRUE;
    }
    else
    {
        sceneComplete = KZ_FALSE;
    }

    result = cluProfilerLogEvents(sceneData->profiler);
    kzsErrorForward(result);

    if(sceneData->configuration->isBenchmarkedScene)
    {
        result = bfReportLoggerUpdatePostFrame(reportLogger);
        kzsErrorForward(result);

        if(sceneData->configuration->postUpdate_private != KZ_NULL)
        {
            result = sceneData->configuration->postUpdate_private(framework, sceneData);
            kzsErrorForward(result);
        }

        /* Execute the postframe executable. */
        if(bfGetTimingStopExecPath(framework) != KZ_NULL)
        {
            kzInt value = (kzInt)system(bfGetTimingStopExecPath(framework));
            KZ_UNUSED_RETURN_VALUE(value);
        }

        if(sceneComplete)
        {
            struct XMLNode* node;
            struct BfReportDocument* report = bfGetReportDocument(framework);
            result = bfReportDocumentGetNode(report, "xml/benchmark/tests", &node);
            kzsErrorForward(result);
            result = bfInfoUpdateSceneResults(node, reportLogger, sceneData->sceneName, sceneData->sceneCategory, sceneData->validData, sceneData->usingBinaryProgram, sceneData->scoreWeightFactor);
            kzsErrorForward(result);

            {
                
                kzInt outputReport;
                result = settingGetInt(bfGetSettings(framework), "OutputReport", &outputReport);
                kzsErrorForward(result);

                if(outputReport != 0)
                {
                    result = bfReportDocumentSaveXML(report);
                    kzsErrorForward(result);
                }
            }
        }

    }
    
    *out_sceneComplete = sceneComplete;
    kzsSuccess();
}

kzsError bfSceneRender(struct BenchmarkFramework* framework, struct BfScene* sceneData)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(framework));

    if(sceneData->configuration->render_private != KZ_NULL)
    {
        result = sceneData->configuration->render_private(framework, sceneData);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError bfSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* sceneData)
{
    kzsError result;

    kzsAssert(sceneData != KZ_NULL);

    result = sceneData->configuration->free_private(framework, sceneData);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfSceneHandlePointerInput(struct BenchmarkFramework* framework, struct BfScene* sceneData, const struct KzsPointingDeviceInputData* inputData)
{
    kzsError result;

    if(sceneData->configuration->touch_private != KZ_NULL)
    {
        result = sceneData->configuration->touch_private(framework, inputData, sceneData);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError bfSceneHandleKeyInput(struct BenchmarkFramework* framework, struct BfScene* sceneData, const struct KzsKeyDeviceInputData* inputData)
{
    kzsError result;

    if(sceneData->configuration->key_private != KZ_NULL)
    {
        result = sceneData->configuration->key_private(framework, inputData, sceneData);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

void* bfSceneGetUserData(const struct BfScene* scene)
{
    kzsAssert(kzcIsValidPointer(scene));
    return scene->sceneData_private;
}

struct CluProfiler* bfSceneGetProfiler(const struct BfScene* scene)
{
    kzsAssert(kzcIsValidPointer(scene));
    return scene->profiler;
}


void bfSceneSetFrameCounter(struct BfScene* sceneData, kzUint frameCount)
{
    kzsAssert(kzcIsValidPointer(sceneData));
    sceneData->frameCounter = frameCount;
    sceneData->frameCounterInitialValue = frameCount;
}

kzUint bfSceneGetFrameCounterValue(const struct BfScene* sceneData)
{
    kzsAssert(kzcIsValidPointer(sceneData));
    return sceneData->frameCounter;
}

kzUint bfSceneGetFrameInitialCounterValue(const struct BfScene* sceneData)
{
    kzsAssert(kzcIsValidPointer(sceneData));
    return sceneData->frameCounterInitialValue;
}

void bfSceneDisableFromOverallScore(struct BfScene* sceneData)
{
    kzsAssert(kzcIsValidPointer(sceneData));
    sceneData->isPartOfScore = KZ_FALSE;
}

kzBool bfSceneGetValidConfigurationData(const struct BfScene* sceneData)
{
    kzsAssert(kzcIsValidPointer(sceneData));
    return sceneData->validData;
}

void bfSceneSetValidConfigurationData(struct BfScene* sceneData, kzBool valid)
{
    kzsAssert(kzcIsValidPointer(sceneData));
    sceneData->validData = valid;
}

void bfSceneSetScoreWeightFactor(struct BfScene* sceneData, kzFloat sceneWeight)
{
    kzsAssert(kzcIsValidPointer(sceneData));
    sceneData->scoreWeightFactor = sceneWeight;
}

kzBool bfSceneGetUseProgramBinary(const struct BfScene* sceneData)
{
    kzsAssert(kzcIsValidPointer(sceneData));
    return sceneData->usingBinaryProgram;
}

void bfSceneSetUseProgramBinary(struct BfScene* sceneData, kzBool useBinary)
{
    kzsAssert(kzcIsValidPointer(sceneData));
    sceneData->usingBinaryProgram = useBinary;
}
