/**
* \file
* Benchmark framework data structure definition.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "bf_benchmark_framework.h"

#include <benchmarkutil/scene/bf_scene_queue.h>
#include <benchmarkutil/settings/bf_settings.h>
#include <benchmarkutil/report/bf_report_document.h>
#include <benchmarkutil/report/bf_logger.h>
#include <benchmarkutil/report/bf_report.h>
#include <benchmarkutil/report/bf_info.h>

#include <application/kza_application.h>

#include <user/engine/kzu_engine.h>
#include <user/project/kzu_project.h>

#include <core/util/string/kzc_string.h>
#include <core/util/string/kzc_string_buffer.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/util/io/kzc_file.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/debug/kzc_log.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_string.h>

#include <stdio.h>

#include <clutil/clu_platform.h>
#include <clutil/clu_util.h>

#ifdef WIN32
#include <windows.h>
#endif


struct BenchmarkFramework
{
    struct KzaApplication* application; /**< Application */
    struct KzuEngine* engine; /**< Bound engine. */
    struct KzsWindow* window; /**< Application main window. */

    struct KzcMemoryManager* quickManager; /**<  Quick manager. */

    struct BfSceneQueue* testQueue; /**< Queue of tests to be run next. */

    struct KzcSettingContainer* settingContainer; /**< Container for parsed setting file contents. */

    cl_context clContext; /**< OpenCL context. */

    struct BfLogger* logMessageLogger; /**< File Logger struct. */
    struct BfReportLogger* reportLogger; /**< Logger utilities for report generation. */

    struct CluInfo* cluInfo; /**< CL info. */

    struct BfReportDocument* reportDocument; /**< Report document for storing the benchmark results. */

    kzMutableString timingStartAppPath; /**< Path to timing start application. */
    kzMutableString timingStopAppPath; /**< Path to timing stop application. */
    kzMutableString overrideTestsList; /**< Comma separated list of test names for overriding the list of used tests. */

    kzBool autorun; /**< Use autorun feature or not. After autorun the application exits automatically. */
};


/** Loads next scene. */
kzsError bfLoadNextScene_internal(struct BenchmarkFramework* framework);


kzsError bfCreate(const struct KzcMemoryManager* memoryManager, struct KzuEngine* engine, struct KzaApplication* application, struct KzsWindow* window, kzString windowTitle, 
                  struct BenchmarkFramework** out_bf)
{
    kzsError result;
    struct BenchmarkFramework* framework;

    result = kzcMemoryAllocVariable(memoryManager, framework, "Benchmark framework");
    kzsErrorForward(result);

    framework->reportDocument = KZ_NULL;

    if(!kzcFileExists("application.cfg"))
    {
        kzcLogDebug("Could not locate application.cfg file. File should be placed in same folder with executable.");
#ifdef WIN32
        MessageBox(0, L"Could not locate application.cfg file.\nFile should be placed in same folder with executable.", L"Error", 0);
        exit(0);
#endif
    }
 
    result = settingsCreate(memoryManager, "application.cfg", &framework->settingContainer);
    kzsErrorForward(result);

    {
        kzInt outputLog;
        result = settingGetInt(bfGetSettings(framework), "OutputLog", &outputLog);
        kzsErrorForward(result);
        result = bfLoggerCreate(memoryManager, "log.txt", outputLog == 0 ? KZ_FALSE : KZ_TRUE, &framework->logMessageLogger);
        kzsErrorForward(result);
    }
    result = bfReportLoggerCreate(memoryManager, &framework->reportLogger);
    kzsErrorForward(result);

    kzsLog(KZS_LOG_LEVEL_INFO, "Starting OpenCL benchmark");
    kzsLog(KZS_LOG_LEVEL_INFO, "");

    if(window != KZ_NULL && windowTitle != KZ_NULL)
    {
        result = kzsWindowSetTitle(window, windowTitle);
        kzsErrorForward(result);
    }

    /* Set callback for event handler. */
    {
        struct KzuProject* project = kzaApplicationGetProject(application);
        struct KzuUiManager* managerUi = kzuProjectGetUiProject(project);
        result = kzuUiManagerAddActionCallback(managerUi, "MainHandler", bfUiEventHandler, framework);
        kzsErrorForward(result);
    }

    /* Allocate quick memory manager. */
    {
        kzUint quickSize = 1024 * 1024 * 5;
        result = kzcMemoryManagerCreateQuickManager(memoryManager, quickSize, &(framework->quickManager));
        kzsErrorForward(result);
    }

    result = cluInfoCreate(memoryManager, &framework->cluInfo);
    kzsErrorForward(result);

    result = bfSceneQueueCreate(memoryManager, &framework->testQueue);
    kzsErrorForward(result);

    framework->application = application;
    framework->engine = engine;
    framework->application = application;
    framework->window = window;
    framework->clContext = KZ_NULL;
    framework->reportDocument = KZ_NULL;
    framework->timingStartAppPath = KZ_NULL;
    framework->timingStopAppPath = KZ_NULL;
    framework->overrideTestsList = KZ_NULL;
    framework->autorun = KZ_FALSE;

    result = bfReportDocumentCreate(bfGetMemoryManager(framework), &framework->reportDocument);
    kzsErrorForward(result);

    *out_bf = framework;
    kzsSuccess();
}

kzsError bfDelete(struct BenchmarkFramework* framework)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(framework));

    kzsLog(KZS_LOG_LEVEL_INFO, "");
    kzsLog(KZS_LOG_LEVEL_INFO, "Closing down OpenCL benchmark");

    if(framework->timingStartAppPath != KZ_NULL)
    {
        result = kzcStringDelete(framework->timingStartAppPath);
        kzsErrorForward(result);
    }
    if(framework->timingStopAppPath != KZ_NULL)
    {
        result = kzcStringDelete(framework->timingStopAppPath);
        kzsErrorForward(result);
    }
    if(framework->overrideTestsList != KZ_NULL)
    {
        result = kzcStringDelete(framework->overrideTestsList);
        kzsErrorForward(result);
    }

    if(framework->reportDocument != KZ_NULL)
    {
        result = bfReportDocumentDelete(framework->reportDocument);
        kzsErrorForward(result);
    }

    result = bfSceneQueueDelete(framework->testQueue);
    kzsErrorForward(result);

    result = cluInfoDelete(framework->cluInfo);
    kzsErrorForward(result);

    result = kzcMemoryManagerDelete(framework->quickManager);
    kzsErrorForward(result);

    result = bfReportLoggerDelete(framework->reportLogger);
    kzsErrorForward(result);
    result = bfLoggerDelete(framework->logMessageLogger);
    kzsErrorForward(result);

    result = settingsDelete(framework->settingContainer);
    kzsErrorForward(result);

    if(framework->clContext != KZ_NULL)
    {
        cl_int clResult = clReleaseContext(framework->clContext);
        cluClErrorTest(clResult);
    }

    result = kzcMemoryFreeVariable(framework);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfStartTestRun(struct BenchmarkFramework* framework)
{
    kzsError result;
    struct BfScene* scene;

    result = bfSceneQueueGetCurrentScene(framework->testQueue, &scene);
    kzsErrorForward(result);

    if(scene != KZ_NULL)
    {
        result = bfSceneInitialize(framework, scene);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError bfUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, kzBool* out_finished)
{
    kzsError result;
    kzBool queueComplete;
    kzBool sceneFinished;
    struct BfScene* executedScene;

    kzsAssert(kzcIsValidPointer(framework));

    result = bfSceneQueueUpdate(framework->testQueue, framework, deltaTime,
       framework->reportLogger, &executedScene, &sceneFinished, &queueComplete);
    kzsErrorForward(result);

    result = kzcMemoryManagerResetQuickManager(framework->quickManager);
    kzsErrorForward(result);

    if(sceneFinished)
    {
        result = bfSceneUninitialize(framework, executedScene);
        kzsErrorForward(result);

        if(bfSceneGetRepeatMode(framework->testQueue) != BF_SCENE_QUEUE_REPEAT)
        {
            result = bfSceneDelete(executedScene);
            kzsErrorForward(result);
        }

        if(!queueComplete)
        {
            result = bfLoadNextScene_internal(framework);
            kzsErrorForward(result);
        }
    }

    *out_finished = queueComplete;
    kzsSuccess();
}

kzsError bfRender(struct BenchmarkFramework* framework)
{
    kzsError result;
    struct BfScene* scene;

    kzsAssert(kzcIsValidPointer(framework));

    result = bfSceneQueueGetCurrentScene(framework->testQueue, &scene);
    kzsErrorForward(result);

    if(scene != KZ_NULL)
    {
        result = bfSceneRender(framework, scene);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError bfKeyDeviceHandler(struct BenchmarkFramework* framework, struct KzaApplication* application, const struct KzsKeyDeviceInputData* inputData)
{
    kzsError result;
    struct BfScene* scene;

    kzsAssert(kzcIsValidPointer(framework));
    kzsAssert(kzcIsValidPointer(application));

    result = bfSceneQueueGetCurrentScene(framework->testQueue, &scene);
    kzsErrorForward(result);

    if(scene != KZ_NULL)
    {
        result = bfSceneHandleKeyInput(framework, scene, inputData);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError bfPointingDeviceHandler(struct BenchmarkFramework* framework, struct KzaApplication* application, const struct KzsPointingDeviceInputData* inputData)
{
    kzsError result;
    struct BfScene* scene;

    kzsAssert(kzcIsValidPointer(framework));
    kzsAssert(kzcIsValidPointer(application));

    result = bfSceneQueueGetCurrentScene(framework->testQueue, &scene);
    kzsErrorForward(result);

    if(scene != KZ_NULL)
    {
        result = bfSceneHandlePointerInput(framework, scene, inputData);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

void bfSetRepeat(const struct BenchmarkFramework* framework, kzBool repeat)
{
    kzsAssert(kzcIsValidPointer(framework));
    bfSceneSetRepeatMode(framework->testQueue, (repeat ? BF_SCENE_QUEUE_REPEAT : BF_SCENE_QUEUE_ONCE));
}

kzBool bfGetRepeat(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return (bfSceneGetRepeatMode(framework->testQueue) == BF_SCENE_QUEUE_REPEAT);
}

kzsError bfAddScene(const struct BenchmarkFramework* framework, struct BfScene* test)
{
    kzsError result;
    
    kzsAssert(kzcIsValidPointer(framework));
    kzsAssert(kzcIsValidPointer(test));

    result = bfSceneQueueAdd(framework->testQueue, test);
    kzsErrorForward(result);

    kzsSuccess();
}

void bfSetUseAutorun(struct BenchmarkFramework* framework, kzBool autorun)
{
    kzsAssert(kzcIsValidPointer(framework));
    framework->autorun = autorun;
}

kzBool bfGetUseAutorun(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->autorun;
}

KZ_CALLBACK kzsError bfUiEventHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;
    struct BenchmarkFramework* framework = (struct BenchmarkFramework*)userData;

    kzsAssert(kzcIsValidPointer(node));

    /* Forward event to corresponding scene, if handler is attached. */
    {
        struct BfScene* scene;
        result = bfSceneQueueGetCurrentScene(framework->testQueue, &scene);
        kzsErrorForward(result);

        if(scene != KZ_NULL)
        {
            KzuUiActionCallback action = bfSceneGetUiActionCallback(scene);
            if(action != KZ_NULL)
            {
                result = action(node, userData, event, eventType, parameters);
                kzsErrorForward(result);
            }
        }
    }

    kzsSuccess();
}

kzsError bfLoadNextScene_internal(struct BenchmarkFramework* framework)
{
    kzsError result;
    struct BfScene* scene;

    kzsAssert(kzcIsValidPointer(framework));

    result = bfSceneQueueGetCurrentScene(framework->testQueue, &scene);
    kzsErrorForward(result);

    result = kzuProjectClear(bfGetProject(framework));
    kzsErrorForward(result);
    result = kzuProjectClearCache(bfGetProject(framework));
    kzsErrorForward(result);

    result = kzaApplicationSetScene(bfGetApplication(framework), KZ_NULL);
    kzsErrorForward(result);

    result = bfSceneInitialize(framework, scene);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuEngine* bfGetEngine(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->engine;
}

struct KzuProject* bfGetProject(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return kzaApplicationGetProject(framework->application);
}

struct KzuBinaryDirectory* bfGetBinaryDirectory(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return kzaApplicationGetBinaryDirectory(framework->application);
}

struct KzsWindow* bfGetWindow(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->window;
}

struct KzcMemoryManager* bfGetMemoryManager(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return kzaApplicationGetApplicationMemoryManager(framework->application);
}

struct KzcMemoryManager* bfGetQuickManager(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->quickManager;
}

struct KzaApplication* bfGetApplication( const struct BenchmarkFramework* framework )
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->application;
}

struct KzcSettingContainer* bfGetSettings(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->settingContainer;
}

struct BfReportLogger* bfGetReportLogger(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->reportLogger;
}

cl_context bfGetClContext(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->clContext;
}

void bfSetClContext(struct BenchmarkFramework* framework, cl_context context)
{
    kzsAssert(kzcIsValidPointer(framework));
    framework->clContext = context;
}

struct CluInfo* bfGetCluInfo(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->cluInfo;
}

kzsError bfReportCreate(struct BenchmarkFramework* framework)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(framework));

    {
        struct XMLNode* node;
        result = bfReportDocumentGetNode(framework->reportDocument, "xml/surface", &node);
        kzsErrorForward(result);
        {
            const struct KzsWindow* window = bfGetWindow(framework);
            result = bfInfoUpdateSurface(node, window != KZ_NULL ? kzsWindowGetSurface(window) : KZ_NULL);
            kzsErrorForward(result);
        }
    }

    {
        struct XMLNode* node;
        result = bfReportDocumentGetNode(framework->reportDocument, "xml/opengl", &node);
        kzsErrorForward(result);
        result = bfInfoUpdateGL(node);
        kzsErrorForward(result);
    }
    {
        struct XMLNode* node;
        struct XMLNode* profileNode;        
        result = bfReportDocumentGetNode(framework->reportDocument, "xml/opencl", &node);
        kzsErrorForward(result);
        result = bfReportDocumentGetNode(framework->reportDocument, "xml/clProfile", &profileNode);
        kzsErrorForward(result);
        result = bfInfoUpdateCL(node, framework->cluInfo, profileNode);
        kzsErrorForward(result);
    }
    {
        struct XMLNode* node;
        result = bfReportDocumentGetNode(framework->reportDocument, "xml/benchmark/settings", &node);
        kzsErrorForward(result);
        result = bfInfoUpdateConfiguration(node, framework->settingContainer);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

struct BfReportDocument* bfGetReportDocument(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->reportDocument;
}

kzsError bfSetOverrideTests(struct BenchmarkFramework* framework, kzString overrideTests)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(framework));

    if(framework->overrideTestsList != KZ_NULL)
    {
        result = kzcStringDelete(framework->overrideTestsList);
        kzsErrorForward(result);
    }
    result = kzcStringCopy(bfGetMemoryManager(framework), overrideTests, &framework->overrideTestsList);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfSetTimingStartExecPath(struct BenchmarkFramework* framework, kzString startExecPath)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(framework));

    if(framework->timingStartAppPath != KZ_NULL)
    {
        result = kzcStringDelete(framework->timingStartAppPath);
        kzsErrorForward(result);
    }
    result = kzcStringCopy(bfGetMemoryManager(framework), startExecPath, &framework->timingStartAppPath);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfSetTimingStopExecPath(struct BenchmarkFramework* framework, kzString stopExecPath)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(framework));

    if(framework->timingStopAppPath != KZ_NULL)
    {
        result = kzcStringDelete(framework->timingStopAppPath);
        kzsErrorForward(result);
    }
    result = kzcStringCopy(bfGetMemoryManager(framework), stopExecPath, &framework->timingStopAppPath);
    kzsErrorForward(result);

    kzsSuccess();
}

kzString bfGetTimingStartExecPath(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->timingStartAppPath;
}

kzString bfGetTimingStopExecPath(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->timingStopAppPath;
}

kzString bfGetOverrideTestsList(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->overrideTestsList;
}

struct BfSceneQueue* bfGetSceneQueue(const struct BenchmarkFramework* framework)
{
    kzsAssert(kzcIsValidPointer(framework));
    return framework->testQueue;

}

kzsError bfGetCLFullProfile(const struct BenchmarkFramework* framework, kzBool* out_fullProfile)
{
    kzsError result;
    kzInt fullProfile;
    struct KzcSettingContainer* settings;
    kzsAssert(kzcIsValidPointer(framework));
    settings = bfGetSettings(framework);
    result = settingGetInt(settings, "OpenCLFullProfile", &fullProfile);
    kzsErrorForward(result);
    *out_fullProfile = (fullProfile == 1);
    kzsSuccess();
}
