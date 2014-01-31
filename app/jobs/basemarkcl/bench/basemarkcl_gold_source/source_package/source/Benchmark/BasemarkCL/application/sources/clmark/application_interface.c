/**
 * \file
 * Application interface implementation
 * 
 * Copyright 2011 by Rightware. All rights reserved.
 */
#include "application_interface.h"

#include <application/kza_application_interface.h>
#include <application/kza_application_events.h>
#include <application/kza_application.h>

#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader.h>
#include <user/scene_graph/kzu_object.h>
#include <user/ui/kzu_ui_manager.h>
#include <user/ui/kzu_ui_component.h>
#include <user/renderer/kzu_renderer.h>
#include <user/engine/kzu_engine.h>

#include <core/renderer/kzc_renderer.h>
#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/util/settings/kzc_settings.h>
#include <core/debug/kzc_log.h>

#include <system/file/kzs_file_base.h>
#include <system/wrappers/kzs_string.h>

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene_queue.h>
#include <benchmarkutil/settings/bf_settings.h>
#include <benchmarkutil/bf_version.h>
#include <benchmarkutil/report/bf_report_document.h>
#include <benchmarkutil/report/xml/bf_xml_document.h>
#include <benchmarkutil/report/xml/bf_xml_node.h>

#include "menu/cl_menu.h"
#include "menu/cl_loadscreen.h"

#include "tests/image/cl_image.h"
#include "tests/image/cl_video.h"
#include "tests/physics/cl_fluid.h"
#include "tests/physics/cl_soft_body.h"
#include "tests/physics/cl_sph.h"
#include "tests/physics/cl_wave.h"
#include "tests/feature/cl_mandelbulb.h"
#include "tests/feature/cl_julia.h"
#include "tests/feature/cl_compiler.h"


/* Input handling callbacks */
static kzsError keyDeviceHandler(struct KzaApplication* application, const struct KzsKeyDeviceInputData* inputData);
/* Pointing device handler callback. */
static kzsError pointingDeviceHandler(struct KzaApplication* application, const struct KzsPointingDeviceInputData* inputData);
/** Exit cl menu handler. */
KZ_CALLBACK kzsError clMenuExit(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters);
/** Launch test Kanzi event callback. */
KZ_CALLBACK kzsError clMenuStartTestHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters);
/** Launches test with given index. */
static kzsError launchTest_internal(struct KzaApplication* application, kzUint index);
/** Launches given test and places loading bar before the test. */
static kzsError launchTestWithLoadingBar_internal(const struct BenchmarkFramework* framework, struct BfScene* scene);


static void kzApplicationChangeState(struct ApplicationData* applicationData, enum ApplicationState nextState)
{
    applicationData->applicationStateNext = nextState;
}

KZ_CALLBACK void kzApplicationConfigure(const struct KzaSystemProperties* systemProperties, struct KzaApplicationProperties* configuration)
{
    kzcLogDebug("kzApplicationConfigure: Set file path and configure startup settings.");

#ifdef ANDROID
    {
        kzString resourcePath = "/sdcard/rightware/kanzi/basemarkcl";
        kzsLog(KZS_LOG_LEVEL_INFO, "Setting resource path:");
        kzsLog(KZS_LOG_LEVEL_INFO, resourcePath);
        kzsFileBaseSetResourceDirectory(resourcePath);
    }
#endif

    configuration->memoryPoolCount = 1;
    configuration->memoryPoolSize = 70 * 1024 * 1024;

#ifdef WIN32
    configuration->windowProperties.width = 800;
    configuration->windowProperties.height = 480;
    configuration->windowProperties.style = KZS_WINDOW_STYLE_NORMAL;
    configuration->surfaceProperties.antiAliasing = 4;
#else
    configuration->windowProperties.width = 0;
    configuration->windowProperties.height = 0;
    configuration->windowProperties.style = KZS_WINDOW_STYLE_FULL_SCREEN;
    configuration->surfaceProperties.antiAliasing = 0;

    configuration->windowProperties.width = 800;
    configuration->windowProperties.height = 480;
    configuration->windowProperties.style = KZS_WINDOW_STYLE_NORMAL;
    configuration->surfaceProperties.antiAliasing = 4;


#endif	

    configuration->binaryName = "binaries.cfg";
}

/** Kanzi application init function. Called from application framework. */
kzsError kzApplicationInitialize(struct KzaApplication* application)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct ApplicationData* applicationData;
    struct KzaSystemProperties* systemProperties;
    kzMutableString commandLineParameters;

    systemProperties = kzaApplicationGetSystemProperties(application);
    memoryManager = kzaApplicationGetApplicationMemoryManager(application);

    /* Create application data. */
    result = kzcMemoryAllocVariable(memoryManager, applicationData, "Application data");
    kzsErrorForward(result);
    kzaApplicationSetUserData(application, applicationData);

    kzaApplicationSetKeyDeviceHandler(application, keyDeviceHandler);
    kzaApplicationSetPointingDeviceHandler(application, pointingDeviceHandler);

    {
        kzMutableString title;
        kzString versionString = bfGetVersionString();
        result = kzcStringFormat(memoryManager, "Basemark CL '%s'", &title, versionString);
        kzsErrorForward(result);
        result = bfCreate(memoryManager, kzaApplicationGetEngine(application), application, kzaApplicationGetWindow(application), title, &applicationData->framework);
        kzsErrorForward(result);
        result = kzcStringDelete(title);
        kzsErrorForward(result);
    }
    
    applicationData->applicationState = APPLICATION_STATE_MENU;
    applicationData->applicationStateNext = APPLICATION_STATE_NONE;
    applicationData->testTerminationRequested = KZ_FALSE;

    result = kzcStringCreateEmpty(memoryManager, &commandLineParameters);
    kzsErrorForward(result);

    /* Parse command line parameters. */
    {
        kzUint i;
        kzBool invalidParameterPresent = KZ_FALSE;

        kzsLog(KZS_LOG_LEVEL_INFO, " ");
    
        kzsAssert(systemProperties != 0);

        /* First index is the application path. */
        for(i = 1; i < systemProperties->programArgumentCount; ++i)
        {
            kzString argument = systemProperties->programArguments[i];
            kzMutableString argumentLowerCase;
            kzMutableString* splitStrings;
            kzMutableString parameter1;
            kzUint stringCount;

            result = kzcStringAsciiToLowerCase(memoryManager, argument, &argumentLowerCase);
            kzsErrorForward(result);

            parameter1 = commandLineParameters;
            result = kzcStringConcatenateMultiple(memoryManager, 3, &commandLineParameters, commandLineParameters, " ", argumentLowerCase);
            kzsErrorForward(result);
            result = kzcStringDelete(parameter1);
            kzsErrorForward(result);

            result = kzcStringSplit(memoryManager, argumentLowerCase, "=", &stringCount, &splitStrings);
            kzsErrorForward(result);

            if(stringCount == 2)
            {
                if(kzsStrcmp(splitStrings[0], "--on-measure-start") == 0)
                {
                    result = bfSetTimingStartExecPath(applicationData->framework, splitStrings[1]);
                    kzsErrorForward(result);
                }
                else if(kzsStrcmp(splitStrings[0], "--on-measure-stop") == 0)
                {
                    result = bfSetTimingStopExecPath(applicationData->framework, splitStrings[1]);
                    kzsErrorForward(result);
                }
                else if(kzsStrcmp(splitStrings[0], "--tests") == 0)
                {
                    result = bfSetOverrideTests(applicationData->framework, splitStrings[1]);
                    kzsErrorForward(result);
                    bfSetUseAutorun(applicationData->framework, KZ_TRUE);
                }
                else if(kzsStrcmp(splitStrings[0], "--repeat") == 0)
                {
                    if(kzcStringCompare(splitStrings[1], "1") == 0)
                    {
                        bfSetRepeat(applicationData->framework, KZ_TRUE);
                    }
                }
                else
                {
                    result = kzcLog(bfGetMemoryManager(applicationData->framework), KZS_LOG_LEVEL_WARNING, "Unrecognized command line parameter: \"%s\".", argument);
                    kzsErrorForward(result);
                    invalidParameterPresent = KZ_TRUE;
                }
            }
            else
            {
                result = kzcLog(bfGetMemoryManager(applicationData->framework), KZS_LOG_LEVEL_WARNING, "Malformed command line parameter: \"%s\".", argument);
                kzsErrorForward(result);
                invalidParameterPresent = KZ_TRUE;
            }

            result = kzcStringDelete(argumentLowerCase);
            kzsErrorForward(result);
            {
                kzUint n; 
                for (n = 0; n < kzcArrayLength(splitStrings); ++n)
                {
                    result = kzcStringDelete(splitStrings[n]);
                    kzsErrorForward(result);
                }
            }
            result = kzcMemoryFreeArray(splitStrings);
            kzsErrorForward(result);
        }

        if(invalidParameterPresent)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, " ");
            kzsLog(KZS_LOG_LEVEL_INFO, "Accepted parameters: ");
            kzsLog(KZS_LOG_LEVEL_INFO, " --on-measure-start=\"c:\\start.bat\"");
            kzsLog(KZS_LOG_LEVEL_INFO, "   path to executable to run when starting timing test");
            kzsLog(KZS_LOG_LEVEL_INFO, " --on-measure-stop=\"c:\\stop.bat\"");
            kzsLog(KZS_LOG_LEVEL_INFO, "   path to executable to run when stopping timing test");
            kzsLog(KZS_LOG_LEVEL_INFO, " --tests=\"TestSphFluid, TestSoftBodySimulationButton\"");
            kzsLog(KZS_LOG_LEVEL_INFO, "   comma separated list of tests to run");
            kzsLog(KZS_LOG_LEVEL_INFO, " --repeat=1");
            kzsLog(KZS_LOG_LEVEL_INFO, "   repeat given tests");
            kzsLog(KZS_LOG_LEVEL_INFO, " ");
        }
    }

    /* Store command line parameters. */
    {
        struct BfReportDocument* document = bfGetReportDocument(applicationData->framework);
        struct XMLNode* node;
        result = bfReportDocumentGetNode(document, "xml/commandLineParameters", &node);
        kzsErrorForward(result);
        result = XMLNodeSetString(node, commandLineParameters);
        kzsErrorForward(result);
        result = kzcStringDelete(commandLineParameters);
        kzsErrorForward(result);
    }

    result = clMenuCreate(applicationData->framework, &applicationData->menu); 
    kzsErrorForward(result);

    /* Set callbacks for event handler. */
    {
        struct KzuProject* project = kzaApplicationGetProject(application);
        struct KzuUiManager* managerUi = kzuProjectGetUiProject(project);
        result = kzuUiManagerAddActionCallback(managerUi, "menuLaunchTest", clMenuStartTestHandler, application);
        kzsErrorForward(result);
        result = kzuUiManagerAddActionCallback(managerUi, "menuExit", clMenuExit, application);
        kzsErrorForward(result);
        result = kzuUiManagerAddActionCallback(managerUi, "menuHoverStart", clMenuHoverHandler, applicationData->framework);
        kzsErrorForward(result);
        result = kzuUiManagerAddActionCallback(managerUi, "menuHoverEnd", clMenuDehoverHandler, applicationData->framework);
        kzsErrorForward(result);
        result = kzuUiManagerAddActionCallback(managerUi, "menuBack", clMenuBackHandler, applicationData->menu);
        kzsErrorForward(result);
        result = kzuUiManagerAddActionCallback(managerUi, "menuSelect", clMenuSelectHandler, applicationData->menu);
        kzsErrorForward(result);
    }

    result = clMenuInitialize(applicationData->framework, applicationData->menu);
    kzsErrorForward(result);


    /* Check if menu skip is requested. */
    {
        kzInt platformId, deviceId;
        struct KzcSettingContainer* settings = bfGetSettings(applicationData->framework);
        result = settingGetInt(settings, "ForceOpenCLPlatform", &platformId);
        kzsErrorForward(result);
        result = settingGetInt(settings, "ForceOpenCLDevice", &deviceId);
        kzsErrorForward(result);
        
        {
            kzBool success;
            result = clMenuCreateContextAndSkipPlatformMenu(applicationData->menu, applicationData->framework, platformId, deviceId, &success);
            kzsErrorForward(result);
            if(success)
            {
                result = clMenuProcessAutoStart(applicationData, applicationData->framework, applicationData->menu, KZ_NULL);
                kzsErrorForward(result);
            }
        }
    }

    {
        kzInt resolutionX;
        kzInt resolutionY;
        const struct KzsWindow* window = kzaApplicationGetWindow(application);
        if(window != KZ_NULL)
        {
            struct KzcSettingContainer* settings = bfGetSettings(applicationData->framework);
            result = settingGetInt(settings, "WindowResolutionX", &resolutionX);
            kzsErrorForward(result);
            result = settingGetInt(settings, "WindowResolutionY", &resolutionY);
            kzsErrorForward(result);
            /*
            result = kzsWindowResize(window, resolutionX, resolutionY);
            kzsErrorForward(result);
            */
        }
    }


    kzsSuccess();
}

KZ_CALLBACK kzsError kzApplicationUpdate(struct KzaApplication* application, kzUint deltaTime)
{
    kzsError result;
    struct ApplicationData* applicationData = kzaApplicationGetUserData(application);
    kzsAssert(kzcIsValidPointer(applicationData));

    /* Handle state changes. */
    switch(applicationData->applicationStateNext)
    {
    case APPLICATION_STATE_NONE:
        {
            /* Do nothing. No state change is requested. */
            break;
        }
    case APPLICATION_STATE_MENU:
        {
            result = clMenuInitialize(applicationData->framework, applicationData->menu);
            kzsErrorForward(result);

        }
        /* Flow intentionally moves to next case. -e{825} -e{616} */
    case APPLICATION_STATE_QUIT:
    case APPLICATION_STATE_START_TEST_SUITE:
    case APPLICATION_STATE_TEST:
    default:
        {
            applicationData->applicationState = applicationData->applicationStateNext;
            applicationData->applicationStateNext = APPLICATION_STATE_NONE;
            break;
        }
    }

    switch(applicationData->applicationState)
    {
        case APPLICATION_STATE_MENU:
        {
            result = clMenuUpdate(applicationData, applicationData->framework, applicationData->menu, deltaTime);
            kzsErrorForward(result);
            break;
        }
        case APPLICATION_STATE_TEST:
        {
            kzBool finished;

            if(!applicationData->testTerminationRequested)
            {
                result = bfUpdate(applicationData->framework, deltaTime, &finished);
                kzsErrorForward(result);
            }
            else
            {
                struct BfSceneQueue* queue = bfGetSceneQueue(applicationData->framework);
                struct BfScene* test;

                result = bfSceneQueueGetCurrentScene(queue, &test);
                kzsErrorForward(result);               
                result = bfSceneUninitialize(applicationData->framework, test);
                kzsErrorForward(result);

                result = bfSceneQueueClear(applicationData->framework, queue);
                kzsErrorForward(result);

                finished = KZ_TRUE;
            }

            if(finished)
            {
                applicationData->testTerminationRequested = KZ_FALSE;

                result = kzaApplicationSetScene(application, KZ_NULL);
                kzsErrorForward(result);
                result = kzuProjectClear(bfGetProject(applicationData->framework));
                kzsErrorForward(result);
                result = kzuProjectClearCache(bfGetProject(applicationData->framework));
                kzsErrorForward(result);
                
                if(bfGetUseAutorun(applicationData->framework))
                {
                    kzApplicationChangeState(applicationData, APPLICATION_STATE_QUIT);
                }
                else
                {
                    kzApplicationChangeState(applicationData, APPLICATION_STATE_MENU);
                }
            }
            break;
        }
        case APPLICATION_STATE_START_TEST_SUITE:
        {
            result = clMenuUninitialize(applicationData->framework, applicationData->menu);
            kzsErrorForward(result);
            result = bfStartTestRun(applicationData->framework);
            kzsErrorForward(result);
            kzApplicationChangeState(applicationData, APPLICATION_STATE_TEST);
            break;
        }
        case APPLICATION_STATE_QUIT:
        {
            kzaApplicationQuit(application);
            break;
        }
        case APPLICATION_STATE_NONE:
        default:
        {
            break;
        }
    }
    kzsSuccess();
}

KZ_CALLBACK kzsError kzApplicationRender(const struct KzaApplication* application)
{
    kzsError result;
    struct ApplicationData* applicationData = kzaApplicationGetUserData(application);
    kzsAssert(kzcIsValidPointer(applicationData));

    switch(applicationData->applicationState)
    {
        case APPLICATION_STATE_MENU:
        {
            result = clMenuRender(applicationData->framework, applicationData->menu);
            kzsErrorForward(result);
            break;
        }
        case APPLICATION_STATE_TEST:
        {
            result = bfRender(applicationData->framework);
            kzsErrorForward(result);
            break;
        }
        case APPLICATION_STATE_START_TEST_SUITE:
        case APPLICATION_STATE_NONE:
        case APPLICATION_STATE_QUIT:
        default:
        {
            break;
        }
    }


    kzsSuccess();
}

KZ_CALLBACK kzsError kzApplicationUninitialize(struct KzaApplication* application)
{
    kzsError result;
    struct ApplicationData* applicationData = kzaApplicationGetUserData(application);
    kzsAssert(kzcIsValidPointer(applicationData));

    result = clMenuDelete(applicationData->framework, applicationData->menu);
    kzsErrorForward(result);

    result = bfDelete(applicationData->framework);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(applicationData);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzApplicationEventHandler(struct KzaApplication* application, enum KzsWindowEventType event)
{
    kzsSuccess();
}

KZ_CALLBACK kzsError pointingDeviceHandler(struct KzaApplication* application, const struct KzsPointingDeviceInputData* inputData)
{
    kzsError result;
    struct ApplicationData* applicationData = kzaApplicationGetUserData(application);
    kzsAssert(kzcIsValidPointer(applicationData));

    switch(applicationData->applicationState)
    {
        case APPLICATION_STATE_MENU:
        {
            result = clMenuPointingDeviceHandler(applicationData->framework, applicationData->menu, inputData);
            kzsErrorForward(result);
            break;
        }
        case APPLICATION_STATE_TEST:
        {
            result = bfPointingDeviceHandler(applicationData->framework, application, inputData);
            kzsErrorForward(result);
            break;
        }
        case APPLICATION_STATE_START_TEST_SUITE:
        case APPLICATION_STATE_QUIT:
        case APPLICATION_STATE_NONE:
        default:
        {
            break;
        }
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError keyDeviceHandler(struct KzaApplication* application, const struct KzsKeyDeviceInputData* inputData)
{
    kzsError result;
    enum KzsInputKeyDeviceState state = kzsInputGetKeyDeviceState(inputData);
    enum KzsInputKey button = kzsInputGetKeyDeviceButton(inputData);
    struct ApplicationData* applicationData = kzaApplicationGetUserData(application);
    kzsAssert(kzcIsValidPointer(applicationData));

    switch(applicationData->applicationState)
    {
        case APPLICATION_STATE_MENU:
        {
            result = clMenuKeyDeviceHandler(applicationData->framework, applicationData->menu, inputData);
            kzsErrorForward(result);
            break;
        }
        case APPLICATION_STATE_TEST:
        {
            result = bfKeyDeviceHandler(applicationData->framework, application, inputData);
            kzsErrorForward(result);
            break;
        }
        case APPLICATION_STATE_START_TEST_SUITE:
        case APPLICATION_STATE_QUIT:
        case APPLICATION_STATE_NONE:
        default:
        {
            break;
        }
    }


    
    if(state == KZS_KEY_DEVICE_STATE_DOWN && (button == KZS_KEY_ESC || button == KZS_KEY_Q))
    {
        if(applicationData->applicationState == APPLICATION_STATE_MENU)
        {
            if(clMenuEscapePressed(applicationData->framework, applicationData->menu))
            {
                kzApplicationChangeState(applicationData, APPLICATION_STATE_QUIT);
            }
        }
        else
        {
            if(bfGetUseAutorun(applicationData->framework))
            {
                struct BfSceneQueue* queue = bfGetSceneQueue(applicationData->framework);
                struct BfScene* test;

                kzsLog(KZS_LOG_LEVEL_INFO, "Test terminated. Quitting application.");

                result = bfSceneQueueGetCurrentScene(queue, &test);
                kzsErrorForward(result);               
                result = bfSceneUninitialize(applicationData->framework, test);
                kzsErrorForward(result);
                result = bfSceneQueueClear(applicationData->framework, queue);
                kzsErrorForward(result);
                kzApplicationChangeState(applicationData, APPLICATION_STATE_QUIT);
            }
            else
            {
                kzsLog(KZS_LOG_LEVEL_INFO, "Test terminated.");
                applicationData->testTerminationRequested = KZ_TRUE;
            }
        }
    }

    kzsSuccess();
}


KZ_CALLBACK kzsError clMenuExit(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    struct KzaApplication* application = userData;
    struct ApplicationData* applicationData = kzaApplicationGetUserData(application);
    kzApplicationChangeState(applicationData, APPLICATION_STATE_QUIT);
    kzsSuccess();
}

static kzsError launchTestWithLoadingBar_internal(const struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;

    /* Add load screen. */
    {
        struct BfScene* loadingScene;
        result = clLoadScreenCreate(framework, &loadingScene);
        kzsErrorForward(result);
        result = bfAddScene(framework, loadingScene);
        kzsErrorForward(result);
    }
    /* Add test. */
    {
        result = bfAddScene(framework, scene);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError addTestToQueue(struct BenchmarkFramework* framework, kzUint index, kzBool* out_testExists)
{
    kzsError result;
    kzBool exists = KZ_TRUE;

    switch(index)
    {
        /* Physics tests. */
        case 0:
        {
            struct BfScene* scene;
            result = softBodyCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 1:
        {
            struct BfScene* scene;
            result = fluidCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 2:
        {
            struct BfScene* scene;
            result = sphCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }


        case 3:
        {
            struct BfScene* scene;
            result = waveCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);    
            break;
        }
        /* Image tests. */
        case 10:
        {
            struct BfScene* scene;
            result = blurCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 11:
        {
            struct BfScene* scene;
            result = histogramCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 12:
        {
            struct BfScene* scene;
            result = medianCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 13:
        {
            struct BfScene* scene;
            result = bilateralCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 14:
        {
            struct BfScene* scene;
            result = sharpeningCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        /* Video tests. */

        case 20: /* Smoothing */
        {
            struct BfScene* scene;
            result = blurVideoCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }

        case 21: /* Color correction */
        {
            struct BfScene* scene;
            result = histogramVideoCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 22: /* Noise reduction */
        {
            struct BfScene* scene;
            result = medianVideoCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 23: /* Surface smoothing */
        {
            struct BfScene* scene;
            result = bilateralVideoCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 24: /* Sharpening */
        {
            struct BfScene* scene;
            result = sharpeningVideoCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }

        /* Feature tests. */
        case 30:
        {
            struct BfScene* scene;
            result = mandelbulbCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 31:
        {
            struct BfScene* scene;
            result = juliaCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }
        case 32:
        {
            struct BfScene* scene;
            result = compilerTestCreate(framework, &scene);
            kzsErrorForward(result);
            result = launchTestWithLoadingBar_internal(framework, scene);
            kzsErrorForward(result);
            break;
        }

        /* Run all tests. */
        case 40:
        {
            kzUint i;
            for(i = 0; i < 32; i++)
            {
                result = addTestToQueue(framework, i, KZ_NULL);
                kzsErrorForward(result);
            }
            break;
        }
        default: 
        {
            exists = KZ_FALSE;
            break;
        }
    }
    
    if(out_testExists != KZ_NULL)
    {
        *out_testExists = exists;
    }
    kzsSuccess();
}

static kzsError launchTest_internal(struct KzaApplication* application, kzUint index)
{
    kzsError result;
    struct ApplicationData* applicationData = kzaApplicationGetUserData(application);
    kzsAssert(kzcIsValidPointer(applicationData));

    {
        struct BenchmarkFramework* framework = applicationData->framework;
        kzBool launchTest;

        result = addTestToQueue(framework, index, &launchTest);
        kzsErrorForward(result);

        if(launchTest)
        {
            kzApplicationChangeState(applicationData, APPLICATION_STATE_START_TEST_SUITE);
        }
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError clMenuStartTestHandler(const struct KzuUiComponentNode* node, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;
    struct KzaApplication* application = userData;
    kzInt testIndex = clMenuGetIndex(node);
    
    if(testIndex >= 0)
    {
        result = launchTest_internal(application, testIndex);
        kzsErrorForward(result);
    }

    kzsSuccess();
}   

kzsError clMenuProcessAutoStart(struct ApplicationData* applicationData, struct BenchmarkFramework* framework, struct ClMenu* clMenu, kzBool* out_autoStartActivated)
{
    kzsError result;
    struct KzcSettingNode* testsNode;
    kzBool atleastOneTestAddedToQueue = KZ_FALSE;
    kzBool useAutorun;

    kzsLog(KZS_LOG_LEVEL_INFO, "");

    {
        kzInt autorunValue, repeatValue;
        struct KzcSettingNode* node;
        result = kzcSettingContainerGetNode(bfGetSettings(framework), "Autorun", &node);
        kzsErrorForward(result);

        /* Use autorun. */
        autorunValue = kzcSettingNodeGetIntegerDefault(node, "UseAutorun", 1);
        kzsErrorForward(result);
        if(autorunValue != 0)
        {
            bfSetUseAutorun(framework, KZ_TRUE);
        }
        useAutorun = bfGetUseAutorun(framework);
        result = kzcLog(bfGetQuickManager(framework), KZS_LOG_LEVEL_INFO, "Use autorun \"%s\".", (bfGetUseAutorun(framework) ? "true" : "false"));
        kzsErrorForward(result);

#if KZ_OPENGL_VERSION == KZ_OPENGL_NONE
        if(!useAutorun)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "");
            kzsLog(KZS_LOG_LEVEL_WARNING, "Enable autorun feature from application.cfg.");
            exit(0);
        }
#endif

        /* Repeat count. */
        repeatValue = kzcSettingNodeGetIntegerDefault(node, "Repeat", 1);
        kzsErrorForward(result);

        /* Check if override list is used. */
        if(bfGetOverrideTestsList(framework) != KZ_NULL)
        {
            useAutorun = KZ_TRUE;
            repeatValue = 0;
        }

        if(useAutorun)
        {
            bfSetUseAutorun(framework, KZ_TRUE);
            if(repeatValue != 0)
            {
                bfSetRepeat(framework, KZ_TRUE);
            }
            result = kzcLog(bfGetQuickManager(framework), KZS_LOG_LEVEL_INFO, "Autorun repeat \"%s\".", ((bfGetRepeat(framework) != KZ_FALSE) ? "infinitely" : "once"));
            kzsErrorForward(result);
        }
    }

    if(useAutorun)
    {
        /* Use the commad line parameter list. */
        if(bfGetOverrideTestsList(framework) != KZ_NULL)
        {
            kzString testNames = bfGetOverrideTestsList(framework);
            kzUint testCount;
            kzMutableString* testNamesArray;
            kzUint i;
            
            result = kzcStringSplit(bfGetMemoryManager(framework), testNames, ",", &testCount, &testNamesArray);
            kzsErrorForward(result);

            for(i = 0; i < testCount; ++i)
            {
                kzString name = kzcStringTrim(testNamesArray[i]);

                kzBool added;
                kzInt testIndex;
                result = clMenuGetTestIndexForName(bfGetQuickManager(framework), name, &testIndex);
                kzsErrorForward(result);
                result = addTestToQueue(framework, testIndex, &added);
                kzsErrorForward(result);
                if(added)
                {
                    atleastOneTestAddedToQueue = KZ_TRUE;
                    result = kzcLog(bfGetQuickManager(framework), KZS_LOG_LEVEL_INFO, "Added \"%s\" to autorun queue.", name);
                    kzsErrorForward(result);
                }
            }

            {
                kzUint n; 
                for (n = 0; n < kzcArrayLength(testNamesArray); ++n)
                {
                    result = kzcStringDelete(testNamesArray[n]);
                    kzsErrorForward(result);
                }
            }
            result = kzcMemoryFreeArray(testNamesArray);
            kzsErrorForward(result);
        }
        else /* Use the ones in settings. */
        {

            result = kzcSettingContainerGetNode(bfGetSettings(framework), "Autorun/IncludedTests", &testsNode);
            kzsErrorForward(result);

            if(testsNode != KZ_NULL)
            {
                struct KzcHashMap* settingDictionary = kzcSettingNodeGetDictionary(testsNode);
                if(settingDictionary != KZ_NULL)
                {
                    struct KzcHashMapIterator it = kzcHashMapGetIterator(settingDictionary);

                    while(kzcHashMapIterate(it))
                    {
                        kzString name = kzcHashMapIteratorGetKey(it);

                        kzInt intValue;
                        if(kzcSettingNodeGetInteger(testsNode, name, &intValue))
                        {
                            if(intValue != 0)
                            {
                                kzBool added;
                                kzInt testIndex;
                                result = clMenuGetTestIndexForName(bfGetQuickManager(framework), name, &testIndex);
                                kzsErrorForward(result);
                                result = addTestToQueue(framework, testIndex, &added);
                                kzsErrorForward(result);
                                if(added)
                                {
                                    atleastOneTestAddedToQueue = KZ_TRUE;
                                    result = kzcLog(bfGetQuickManager(framework), KZS_LOG_LEVEL_INFO, "Added \"%s\" to autorun queue.", name);
                                    kzsErrorForward(result);
                                }
                            }
                        }
                        else
                        {
                            result = kzcLog(bfGetQuickManager(framework), KZS_LOG_LEVEL_WARNING, "Setting \"Autorun/IncludedTests\" value for %s was not integer");
                            kzsErrorForward(result);
                        }
                    }
                }
            }
        }

        {
            kzBool started = KZ_FALSE;
            if(atleastOneTestAddedToQueue)
            {
                kzApplicationChangeState(applicationData, APPLICATION_STATE_START_TEST_SUITE);
                started = KZ_TRUE;
            }

            if(out_autoStartActivated != KZ_NULL)
            {
                *out_autoStartActivated = started;
            }
        }
    }
    else
    {
        if(out_autoStartActivated != KZ_NULL)
        {
            *out_autoStartActivated = KZ_FALSE;
        }
    }
    
    kzsLog(KZS_LOG_LEVEL_INFO, "");

    kzsSuccess();
}
