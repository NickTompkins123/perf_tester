/**
 * \file
 * Application framework.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kza_application.h"

#if 0
#define KZA_APPLICATION_ENABLE_SPLASH_SCREEN /**< Enables the splash screen. */
#define KZA_APPLICATION_SPLASH_DURATION 2000 /**< Splash screen duration in milliseconds. */
#endif

/* TODO: Move below later. */
/* TODO: Currently defines ENABLE_LUA if in Windows */
#undef ENABLE_LUA
#ifdef ENABLE_LUA
#include <user/scripting/lua/kzu_lua.h>
#include <lua_wrapper/lua_bind.h> 
#endif

#include <application/kza_window.h>
#include <application/kza_application_interface.h>
#include <application/kza_splash.h>
#include <application/kza_application_events.h>
#include <application/transition/kza_transition.h>

#include <user/animation/kzu_animation_player.h>
#include <user/binary/kzu_binary_directory.h>
#include <user/binary/kzu_binary_loader.h>
#include <user/common/kzu_user.h>
#include <user/common/kzu_globals.h>
#include <user/engine/kzu_engine.h>
#include <user/filter/kzu_object_source.h>
#include <user/material/kzu_material.h>
#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader.h>
#include <user/project/kzu_project_patcher.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/renderer/kzu_renderer.h>
#include <user/renderer/kzu_renderer_util.h>
#include <user/scripting/kzu_script.h>
#include <user/scripting/lua/kzu_lua.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>
#include <user/ui/kzu_ui_screen.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/components/kzu_ui_grid_layout.h>
#include <user/ui/components/kzu_ui_stack_layout.h>
#include <user/ui/components/kzu_ui_trajectory_layout.h>
#include <user/ui/input_manager/kzu_input_manager.h>
#include <user/ui/message/kzu_message_dispatcher.h>
#include <user/util/kzu_free_camera.h>
#include <user/util/kzu_pick.h>
#include <user/util/kzu_transform_gizmo.h>
#include <user/util/hud/kzu_hud.h>

#include <core/debug/kzc_log.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_system.h>
#include <core/memory/kzc_memory_pooled.h>
#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/io/kzc_file.h>
#include <core/util/settings/kzc_settings.h>
#include <core/util/settings/kzc_settings_parser.h>
#include <core/util/string/kzc_string.h>

#include <system/kzs_system.h>
#include <system/debug/kzs_error.h>
#include <system/debug/kzs_log.h>
#include <system/display/kzs_display.h>
#include <system/display/kzs_window.h>
#include <system/display/kzs_surface.h>
#include <system/file/kzs_resource_file.h>
#include <system/time/kzs_sleep.h>
#include <system/wrappers/kzs_opengl.h>

#ifdef ANDROID
#include <system/time/kzs_tick.h>
#define KZA_APPLICATION_RELOAD_GPU_RESOURCES_ON_RESUME
#endif


/** Application main structure. */
struct KzaApplication
{
    /* common */
    enum KzaApplicationState        state;                      /**< Current state of the application. */

    /* memory managers */
    struct KzcMemoryManager*    systemMemoryManager;
    struct KzcMemoryManager*    applicationMemoryManager;       /**< Used by the application. */
    kzBool useSystemAsApplicationMemoryManager;                 /**< When application is initialized, use system memory manager instead of creating a new pooled memory manager. */

    struct KzaSystemProperties     systemProperties;  /**< System properties. */
    struct KzaApplicationProperties     applicationProperties;  /**< Application properties. */

    struct KzcDynamicArray*      displays; /**< List of application displays.  <struct KzsDisplay*>*/ 
    struct KzcDynamicArray*      windows;  /**< List of application windows.  <struct KzsWindow*> */
    struct KzcDynamicArray*      surfaces; /**< List of application surfaces. <struct KzsSurface*>  */

    void*                       userData;               /**< Custom data for the user. */

    struct KzaPlatform*         platform;               /**< Platform specific data. */

    /* Default handlers */
    KzaApplicationChangeHandler changeHandler;                   /**< Function pointer to handler that receives events from application. */
    KzaApplicationPointingDeviceHandler pointingDeviceHandler; /**< Function pointer to mouse handler. */
    KzaApplicationKeyDeviceHandler keyDeviceHandler;          /**< Function pointer to keyboard input handler. */
    KzaApplicationApplySceneHandler applySceneHandler;         /**< Function pointer to tool apply scene handler.*/
    KzaApplicationPatchProjectHandler patchProjectHandler;       /**< Function pointer to tool patch project handler.*/
    KzaApplicationReloadProjectHandler reloadProjectHandler;      /**< Function pointer to tool reload project handler.*/

    KzaApplicationWindowCreatorFunction windowCreatorFunction;
    
    KzaApplicationToolReloadHandler toolReloadFunction;

    KzaApplicationWindowEventHandler windowEventHandler;

    kzBool toolPresent;                         /**< Is tool used. */

    struct KzuPropertyManager* propertyManager; /**< Property manager. */
    struct KzuInputManager* inputManager;       /**< Input manager. */
    struct KzuMessageDispatcher* messageDispatcher; /**< Message dispatcher. */
    struct KzuBinaryDirectory* directory;       /**< Binary directory. */
    struct KzuBinaryLoader* binaryLoader;       /**< Binary loader. */
    struct KzuProjectPatcher* projectPatcher;   /**< Project patcher. */
    struct KzuProject* project;                 /**< Default project. */
    struct KzuEngine* engine;                   /**< Engine. */
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    struct KzsWindow* window;                   /**< Application main window. */
#endif
    struct KzuCameraNode* viewCamera;           /**< The actual view camera of the active scene. Stored here so it can be restored when disabling free camera. */
    struct KzuFreeCamera* freeCamera;           /**< Free camera. */
    kzBool freeCameraInUse;                     /**< KZ_TRUE when the free camera is added to the scene. This is different than freeCameraEnabled, as the free camera is added to the scene also if no view camera is specified for the scene. */
    struct KzuPick* pick;                       /**< Picking utility. */
    struct KzuTransformGizmo* transformGizmo;   /**< Transform gizmo utility. */
    struct KzuScene* currentScene;              /**< Currently active scene. */
    struct KzuTransformedScene* transformedScene; /**< Transformed current scene. */

    enum KzaToolPickMode toolPickMode;          /**< Tool object picking mode. */
    KzaPickCallback pickCallback;               /**< Picking callback. */
    void* pickCallbackUserData;                 /**< Picking callback user data. */

    kzBool applicationInitialized;               /**< Flag to indicate wether the application has been initialized or not. */
    
    kzBool gridEnabled;                         /**< Is grid enabled or not. */
    kzBool debugObjectsEnabled;                 /**< Are debug objects enabled. */
    kzBool freeCameraEnabled;                   /**< Is free camera enabled or not. */
    kzBool pickingEnabled;                      /**< Is picking enabled or not. */
    kzBool transformGizmoEnabled;               /**< Is transform gizmo enabled or not. */
    kzBool boundingBoxEnabled;                  /**< Is bounding box enabled or not. */
    kzBool layoutAllocatedVisualization;        /**< Is allocated layout visualization enabled or not. */
    kzBool layoutActualVisualization;           /**< Is actual layout visualization enabled or not. */
    kzBool isActualLayoutVisualizationRecursive; /**< Set whether or not the actual layout display works recursively down in layouts. */
    kzBool infoHudEnabled;                      /**< Is FPS info enabled. */
    kzBool logVisualizationEnabled;             /**< Is log visualization enabled. */
    kzBool userInterfaceEnabled;                /**< Is user interface enabled. */
    kzBool defaultKeyHandlingEnabled;           /**< Is default key handling enabled. */
    
    kzBool isDeltaTimeSkippedForFrame;          /**< Is delta time skipped for frame. */
    kzBool applicationResuming;                 /**< Application resuming after pause. */
    kzBool applicationPausing;                  /**< Application pausing in next update. */


    struct KzaSplashScreen* splash;             /**< Splash screen structure. */
    kzInt splashTimer;                          /**< Timer for splash screen. Negative value indicates that splash has finished. */

    struct KzaTransition* transition;           /**< Transition effect. */

    struct KzuHud* hud; /**< Heads up display for drawing fps and such data to screen. */

    struct KzuLua* lua; /**< Lua scripting. */
};


/** Creates and adds free camera. */
static kzsError addFreeCamera_internal(struct KzaApplication* application, kzBool modifyFreeCamera);
/** Removes free camera from current scene. */
static kzsError removeFreeCamera_internal(struct KzaApplication* application);
/** Patch callback. */
static kzsError patchCallback_internal(void* userData, kzString path, enum KzuProjectObjectType objectType, void* object);
/** Reads applicaiton configuration from given path. */
static kzsError kzaApplicationReadConfiguration_internal(const struct KzcMemoryManager* memoryManager,
                                                         struct KzaApplicationProperties* configuration, kzString configPath);
/** Visualize the allocated space if an object node. */
static kzsError kzaApplicationVisualizeLayout_internal(struct KzaApplication* application, struct KzcDynamicArray* objectList,
                                                       const struct KzuObjectNode* objectNode);
/** Handle all pointing device events. */
static kzsError kzaApplicationInputPointingDeviceHandler_internal(const struct KzsPointingDeviceInputData* inputData,
                                                                  void* userData);
/** Handle all key device events. */
static kzsError kzaApplicationInputKeyDeviceHandler_internal(const struct KzsKeyDeviceInputData* inputData,
                                                             void* userData);


kzsError kzaApplicationCreate(const struct KzaSystemProperties* systemProperties, struct KzaApplication** out_application)
{
    kzsError result;
    struct KzaApplication* application = KZ_NULL;

    /* If called again, reload resources and resume */
    if (kzcIsValidPointer(*out_application))
    {
        struct KzaApplicationChangeEvent event;

        kzsAssert(kzcIsValidPointer(application));

        event.type = KZA_APPLICATION_RESTARTED;
        
        if ((*out_application)->changeHandler != KZ_NULL)
        {
            result = (*out_application)->changeHandler(application, &event);
            kzsErrorForward(result);
        }

#ifdef ANDROID
        kzcLogDebug("Android restore resources");

        kzcResourceManagerFreeResourcesFromGPU(kzuProjectGetResourceManager(application->project));
        kzcLogDebug("Android free resources");
        kzcResourceManagerUploadResourcesToGPU(kzuProjectGetResourceManager(application->project));

        kzcLogDebug("Android resources done");
#endif
    }

    result = kzaApplicationCreateAdvanced(systemProperties, KZ_NULL, KZ_FALSE, &application);
    kzsErrorForward(result);

    *out_application = application;
    kzsSuccess();
}

kzsError kzaApplicationCreateAdvanced(const struct KzaSystemProperties* systemProperties,
                                      KzaApplicationWindowCreatorFunction windowCreatorFunction,
                                      kzBool useSystemAsApplicationMemoryManager, struct KzaApplication** out_application)
{
    kzsError result;
    struct KzaApplication* application;
    struct KzcMemoryManager* systemMemoryManager;
    struct KzcMemoryManager* memoryManager;

    struct KzaApplicationProperties applicationProperties;
    
    result = kzsSystemInitialize();
    kzsErrorForward(result);

    /* Initialize default application properties. */
    {
        applicationProperties.memoryPoolCount = 1;
        applicationProperties.memoryPoolSize = 2 * 1024 * 1024;

        applicationProperties.binaryName = KZ_NULL;
        applicationProperties.scenePath = KZ_NULL;

        applicationProperties.mainHandler = KZ_NULL;

#ifdef WIN32
        applicationProperties.windowProperties.width = 640;
        applicationProperties.windowProperties.height = 480;
        applicationProperties.windowProperties.style = KZS_WINDOW_STYLE_RESIZABLE;
        applicationProperties.surfaceProperties.antiAliasing = 4;
#else
        applicationProperties.windowProperties.width = 0;
        applicationProperties.windowProperties.height = 0;
        applicationProperties.windowProperties.style = KZS_WINDOW_STYLE_FULL_SCREEN;
        applicationProperties.surfaceProperties.antiAliasing = 0;
#endif

        /* Use automatic surface settings as default */
        applicationProperties.surfaceProperties.bitsColorR = KZS_SURFACE_PROPERTY_DONT_CARE;
        applicationProperties.surfaceProperties.bitsColorG = KZS_SURFACE_PROPERTY_DONT_CARE;
        applicationProperties.surfaceProperties.bitsColorB = KZS_SURFACE_PROPERTY_DONT_CARE;
        applicationProperties.surfaceProperties.bitsAlpha =  KZS_SURFACE_PROPERTY_DONT_CARE;

        applicationProperties.surfaceProperties.bitsDepthBuffer = 16;
        applicationProperties.surfaceProperties.bitsStencil = KZS_SURFACE_PROPERTY_DONT_CARE;

        applicationProperties.gridEnabled = KZ_FALSE;
        applicationProperties.debugObjectsEnabled = KZ_FALSE;
        applicationProperties.freeCameraEnabled = KZ_FALSE;
        applicationProperties.pickingEnabled = KZ_FALSE;
        applicationProperties.transformGizmoEnabled = KZ_FALSE;
        applicationProperties.boundingBoxEnabled = KZ_FALSE;
        applicationProperties.fpsInfoEnabled = KZ_FALSE;
        applicationProperties.logVisualizationEnabled = KZ_FALSE;
        applicationProperties.clearBackgroundEnabled = KZ_FALSE;
        applicationProperties.loadStartupScene = KZ_TRUE;
    }

    result = kzcMemoryManagerCreateSystemManager(&systemMemoryManager);
    kzsErrorForward(result);

    kzApplicationConfigure(systemProperties, &applicationProperties);

    result = kzaApplicationReadConfiguration_internal(systemMemoryManager, &applicationProperties, "application.cfg");
    kzsErrorForward(result);


    if (useSystemAsApplicationMemoryManager || applicationProperties.memoryPoolCount == 0)
    {
        memoryManager = systemMemoryManager;
    }
    else
    {
        result = kzcMemoryManagerCreatePooledManager(systemMemoryManager, applicationProperties.memoryPoolCount,
                                                     applicationProperties.memoryPoolSize, &memoryManager);
        kzsErrorForward(result);
    }

    /* Allocate memory for application struct. */
    result = kzcMemoryAllocVariable(memoryManager, application, "Application structure");
    kzsErrorForward(result);

    result = kzuHudCreate(memoryManager, &application->hud);
    kzsErrorForward(result);

    application->systemProperties = *systemProperties;
    application->applicationProperties = applicationProperties;

    application->changeHandler = KZ_NULL;
    application->keyDeviceHandler = KZ_NULL;
    application->pointingDeviceHandler = KZ_NULL;
    application->applySceneHandler = KZ_NULL;
    application->patchProjectHandler = KZ_NULL;
    application->reloadProjectHandler = KZ_NULL;

    application->state = KZA_APPLICATION_STATE_UNINITIALIZED;
    application->applicationResuming = KZ_FALSE;
    application->applicationPausing = KZ_FALSE;
    
    application->applicationMemoryManager = memoryManager;
    application->systemMemoryManager = systemMemoryManager;
    application->windowCreatorFunction = windowCreatorFunction;
    application->windowEventHandler = KZ_NULL;
    application->useSystemAsApplicationMemoryManager = useSystemAsApplicationMemoryManager;
    application->toolPresent = KZ_FALSE;
    application->toolReloadFunction = KZ_NULL;
    application->userData = KZ_NULL;
    application->pick = KZ_NULL;
    application->transformGizmo = KZ_NULL;
    application->currentScene = KZ_NULL;
    application->binaryLoader = KZ_NULL;

    application->pickCallback = KZ_NULL;
    application->pickCallbackUserData = KZ_NULL;
    application->toolPickMode = KZA_TOOL_PICK_MODE_NODE;

    application->applicationInitialized = KZ_FALSE;

    application->gridEnabled = applicationProperties.gridEnabled;
    application->debugObjectsEnabled = applicationProperties.debugObjectsEnabled;
    application->freeCameraEnabled = applicationProperties.freeCameraEnabled;
    application->pickingEnabled = applicationProperties.pickingEnabled;
    application->transformGizmoEnabled = applicationProperties.transformGizmoEnabled;
    application->boundingBoxEnabled = applicationProperties.boundingBoxEnabled;
    application->infoHudEnabled = applicationProperties.fpsInfoEnabled;
    application->logVisualizationEnabled = applicationProperties.logVisualizationEnabled;
    application->userInterfaceEnabled = KZ_TRUE;
    application->layoutAllocatedVisualization = KZ_FALSE;
    application->layoutActualVisualization = KZ_FALSE;
    application->defaultKeyHandlingEnabled = KZ_TRUE;

    application->isDeltaTimeSkippedForFrame = KZ_FALSE;

    application->splash = KZ_NULL;
    application->splashTimer = -1;

    result = kzuUserInitialize(memoryManager);
    kzsErrorForward(result);

    result = kzuPropertyManagerCreate(memoryManager, &application->propertyManager);
    kzsErrorForward(result);

    result = kzuInputManagerCreate(memoryManager, &application->inputManager);
    kzsErrorForward(result);

    result = kzuMessageDispatcherCreate(memoryManager, &application->messageDispatcher);
    kzsErrorForward(result);

    application->freeCameraInUse = KZ_FALSE;
    application->viewCamera = KZ_NULL;

    result = kzcDynamicArrayCreate(memoryManager, &application->displays);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &application->windows);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &application->surfaces);
    kzsErrorForward(result);

    /* Initialize platform specific components. */
    result = kzaApplicationPlatformCreate(application, &application->platform);
    kzsErrorForward(result);

    result = kzaTransitionCreate(memoryManager, &application->transition);
    kzsErrorForward(result);
#undef ENABLE_LUA
#ifdef ENABLE_LUA
    result = kzuLuaCreate(memoryManager, &application->lua);
    kzsErrorForward(result);
    result = kzuBindLua((lua_State*)kzuLuaGetLuaState(application->lua));
    kzsErrorForward(result);
    result = kzuGlobalsPut(KZU_GLOBALS_LUA_STATE, application->lua);
    kzsErrorForward(result);
#endif

    /* Initialize all available displays. */
    {
        kzUint displayCount;
        kzUint i;
        struct KzcDynamicArray* displayArray = kzaApplicationGetDisplayArray(application);

        result = kzsDisplayNativeGetNumDisplays(&displayCount);
        kzsErrorForward(result);

        for(i = 0; i < displayCount; ++i)
        {
            struct KzsDisplay* display;
            struct KzsDisplayConfiguration displayConfiguration;
            displayConfiguration.dummy = 0;

            result = kzsDisplayCreate(i, &displayConfiguration, &display);
            kzsErrorForward(result);

            result = kzcDynamicArrayAdd(displayArray, display);
            kzsErrorForward(result);
        }
    }

    *out_application = application;
    kzsSuccess();
}

kzsError kzaApplicationDelete(struct KzaApplication* application)
{
    kzsError result;
    struct KzcMemoryManager* systemMemoryManager;
    struct KzcMemoryManager* applicationMemoryManager;
    kzBool useSystemAsApplicationMemoryManager;

    kzsAssert(kzcIsValidPointer(application));

    systemMemoryManager = application->systemMemoryManager;
    applicationMemoryManager = application->applicationMemoryManager;
    useSystemAsApplicationMemoryManager = application->useSystemAsApplicationMemoryManager;

    /* Uninitialize and destroy all remaining surfaces. */
    result = kzaSurfaceDeleteAll(application);
    kzsErrorForward(result);

    /* Uninitialize and destroy all remaining windows. */
    result = kzaWindowDeleteAll(application);
    kzsErrorForward(result);

    /* Delete all displays. */
    result = kzaDisplayDeleteAll(application);
    kzsErrorForward(result);
   
    /*result = kzuLuaDelete(application->lua);
    kzsErrorForward(result);*/

    result = kzaTransitionDelete(application->transition);
    kzsErrorForward(result);

    result = kzaApplicationPlatformDelete(application, application->platform);
    kzsErrorForward(result);

    result = kzuMessageDispatcherDelete(application->messageDispatcher);
    kzsErrorForward(result);

    result = kzuInputManagerDelete(application->inputManager);
    kzsErrorForward(result);

    result = kzuPropertyManagerDelete(application->propertyManager);
    kzsErrorForward(result);

    result = kzuUserUninitialize();
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(application);
    kzsErrorForward(result);

    /* Delete memory managers. */
    if (!useSystemAsApplicationMemoryManager)
    {
        result = kzcMemoryManagerDelete(applicationMemoryManager);
        kzsErrorForward(result);
    }

    result = kzcMemoryManagerDelete(systemMemoryManager);
    kzsErrorForward(result);

    result = kzsSystemUninitialize();
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK static kzsError patchCallback_internal(void* userData, kzString path, enum KzuProjectObjectType objectType, void* object)
{
    kzsError result;
    struct KzaApplication* application = (struct KzaApplication*)userData;

    kzsAssert(kzcIsValidPointer(application));

    if (object == application->currentScene)
    {
        application->viewCamera = kzuSceneGetViewCamera(application->currentScene);

        result = removeFreeCamera_internal(application);
        kzsErrorForward(result);

        result = addFreeCamera_internal(application, KZ_FALSE);
        kzsErrorForward(result);
    }

    if (objectType == KZU_PROJECT_OBJECT_TYPE_SCRIPT)
    {
        result = kzaApplicationCompileLuaScript(application, object);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzaApplicationLoadBinary(struct KzaApplication* application, kzString binaryPath)
{
    kzsError result;

    result = kzuBinaryLoaderCreate(application->applicationMemoryManager, &application->binaryLoader);
    kzsErrorForward(result);

    result = kzuBinaryLoaderAddBinaryFileResource(application->binaryLoader, binaryPath);
    kzsErrorForward(result);

    application->directory = kzuBinaryLoaderGetDirectory(application->binaryLoader);

    /* Merge core assets. */
    result = kzuBinaryDirectoryMergeCoreAssets(application->directory);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzaApplicationLoadBinaryFromConfig(struct KzaApplication* application, kzString configPath)
{
    kzsError result;

    result = kzuBinaryLoaderCreateFromConfig(application->applicationMemoryManager, configPath, &application->binaryLoader);
    kzsErrorForward(result);

    application->directory = kzuBinaryLoaderGetDirectory(application->binaryLoader);

    kzsSuccess();
}

kzsError kzaApplicationInitialize(struct KzaApplication* application)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = application->applicationMemoryManager;

    if (application->applicationProperties.binaryName != KZ_NULL)
    {
        kzMutableString lowerCaseBinaryName;
        kzString binaryName = application->applicationProperties.binaryName;

        result = kzcStringAsciiToLowerCase(memoryManager, binaryName, &lowerCaseBinaryName);
        kzsErrorForward(result);

        if (kzcStringEndsWith(lowerCaseBinaryName, ".kzb"))
        {
            result = kzaApplicationLoadBinary(application, binaryName);
            kzsErrorForward(result);
        }
        else
        {
            result = kzaApplicationLoadBinaryFromConfig(application, binaryName);
            kzsErrorForward(result);
        }

        result = kzcStringDelete(lowerCaseBinaryName);
        kzsErrorForward(result);    
    }
    else
    {
        /* Create initial binary directory. */
        result = kzuBinaryDirectoryCreateEmpty(memoryManager, &application->directory);
        kzsErrorForward(result);

        /* Merge core assets. */
        result = kzuBinaryDirectoryMergeCoreAssets(application->directory);
        kzsErrorForward(result);
    }

    if (kzaToolIsPresent(application))
    {
        kzuBinaryDirectorySetFileFlagMask(application->directory, KZU_BINARY_DIRECTORY_DEFAULT_FILE_FLAG_MASK ^ KZU_BINARY_FILE_INFO_FLAG_LOAD_ON_DEMAND);
    }

    /* Create window. */
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    result = kzaWindowCreate(application, &application->applicationProperties.windowProperties, 
                             &application->applicationProperties.surfaceProperties, &application->window);
    kzsErrorForward(result);
#endif


    /* Create project. */
    result = kzuProjectCreate(memoryManager, application->propertyManager, application->messageDispatcher, application->directory, &application->project);
    kzsErrorForward(result);

    /* UI Manager. */
    if (application->applicationProperties.mainHandler != KZ_NULL)
    {
        struct KzuUiManager* uiManager = kzuProjectGetUiProject(application->project);
        result = kzuUiManagerAddActionCallback(uiManager, "MainHandler", application->applicationProperties.mainHandler, application);
        kzsErrorForward(result);
    }

    result = kzuFreeCameraCreate(memoryManager, application->propertyManager, application->messageDispatcher, &application->freeCamera);
    kzsErrorForward(result);

    /* Create the engine. */
    result = kzuEngineCreate(memoryManager, KZU_ENGINE_DEFAULT_QUICK_MEMORY_SIZE, kzuProjectGetPropertyManager(application->project), &application->engine);
    kzsErrorForward(result);
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    kzuEngineSetActiveWindow(application->engine, application->window);
    kzcRendererSetActiveWindow(kzuRendererGetCoreRenderer(kzuEngineGetRenderer(application->engine)), application->window);
#endif
    
    result = kzuProjectPatcherCreate(memoryManager, &application->projectPatcher);
    kzsErrorForward(result);
    kzuProjectPatcherSetCallback(application->projectPatcher, application, patchCallback_internal);

    /* Initialize picking utility. */
    if (application->pickingEnabled)
    {
        result = kzuPickCreate(memoryManager, application->propertyManager, kzaApplicationGetEngine(application), &application->pick);
        kzsErrorForward(result);
    }

    /* Initialize transform gizmo. */
    if (application->transformGizmoEnabled)
    {
        result = kzuTransformGizmoCreate(memoryManager, application->propertyManager, application->messageDispatcher, "TransformGizmo.kzb", &application->transformGizmo);
        kzsErrorForward(result);
    }

    result = kzuTransformedSceneCreate(memoryManager, &application->transformedScene);
    kzsErrorForward(result);

#ifdef KZA_APPLICATION_ENABLE_SPLASH_SCREEN
    if (!kzaToolIsPresent(application))
    {
        struct KzuEngine* engine = kzaApplicationGetEngine(application);
        struct KzcResourceManager* resourceManager = kzuEngineGetResourceManager(engine);
        struct KzcRenderer* renderer = kzuRendererGetCoreRenderer(kzuEngineGetRenderer(engine));

        result = kzaSplashScreenCreate(memoryManager, resourceManager, renderer, &application->splash);
        kzsErrorForward(result);

        application->splashTimer = KZA_APPLICATION_SPLASH_DURATION;
    }
#endif

    kzsSuccess();
}

static kzsError kzaApplicationInitializeProject_internal(struct KzaApplication* application)
{
    kzsError result;

    /* TODO: Remove kzApplicationInitialize last when the UI action system allows that */
    result = kzuProjectLoaderLoadProjectSettings(application->project);
    kzsErrorForward(result);

    result = kzuEngineApplyGlobalProperties(application->engine, kzuProjectGetPropertyManager(application->project), application->project);
    kzsErrorForward(result);

    result = kzApplicationInitialize(application);
    kzsErrorForward(result);

    result = kzuProjectLoaderLoadPropertyGroups(application->project);
    kzsErrorForward(result);

    /* Startup scene */
    if (application->applicationProperties.loadStartupScene && !kzaToolIsPresent(application) && application->currentScene == KZ_NULL)
    {
        if (application->applicationProperties.scenePath != KZ_NULL)
        {
            result = kzaApplicationSetScenePath(application, application->applicationProperties.scenePath);
            kzsErrorForward(result);
        }
        else
        {
            struct KzuScene* scene = (struct KzuScene*)kzuPropertyManagerGetVoidDefault(kzuProjectGetPropertyManager(application->project),
                                                                                        application->project, KZU_PROPERTY_TYPE_STARTUP_SCENE);

            if (scene != KZ_NULL)
            {
                result = kzaApplicationSetScene(application, scene);
                kzsErrorForward(result);
            }
        }
    }

    /* HUD */
    {
        struct KzcBitmapFontSystem* hudFontSystem;

        result = kzuProjectGetBitmapFontSystem(application->project, &hudFontSystem);
        kzsErrorForward(result);

        result = kzuHudCreateResources(application->hud, kzuEngineGetResourceManager(application->engine), hudFontSystem);
        kzsErrorForward(result);
    }

    result = kzaApplicationCompileLuaScripts(application);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzaApplicationVisualizeLayout_internal(struct KzaApplication* application, struct KzcDynamicArray* objectList,
                                                       const struct KzuObjectNode* objectNode)
{
    kzsError result;
    struct KzuTransformedObjectNode* objectInWorldCoordinates;
    
    objectInWorldCoordinates = kzuTransformedObjectListFindObject(objectList, objectNode);
    if (objectInWorldCoordinates != KZ_NULL && kzuTransformedObjectNodeHasUiData(objectInWorldCoordinates))
    {
        KzuObjectType objectType = kzuObjectNodeGetType(objectNode);
        KzuComponentTypeIdentifier identifier = (KzuComponentTypeIdentifier)KZ_NULL;
        struct KzuRenderer* renderer = kzuEngineGetRenderer(application->engine);
        struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
        struct KzcMatrix4x4 camera = kzcRendererGetMatrix(coreRenderer, KZC_RENDERER_MATRIX_CAMERA);
        struct KzuTransformedObjectNode* parent = kzuTransformedObjectNodeGetParent(objectInWorldCoordinates);
        struct KzuUiComponentNode* uiNode = NULL;
        struct KzcVector3 objectMin;
        struct KzcVector3 objectMax;
        kzBool isLayoutParent = kzuTransformedObjectNodeIsLayout(parent);
        kzBool isLayout = kzuTransformedObjectNodeIsLayout(objectInWorldCoordinates);

        if(objectType == KZU_OBJECT_TYPE_UI_COMPONENT)
        {
            struct KzuUiComponentType* uiType;

            uiNode = kzuUiComponentNodeFromObjectNode(objectNode);
            uiType = kzuUiComponentNodeGetType(uiNode);
            identifier = kzuUiComponentTypeGetTypeIdentifier(uiType);
        }

        if (application->layoutAllocatedVisualization)
        {
            struct KzcMatrix4x4 transform;

            if(isLayout)
            {
                /* TODO: ContentSize may change meaning later, this will break at that point. */
                kzuTransformedObjectNodeGetContentSize(objectInWorldCoordinates, &objectMin, &objectMax);

                if(isLayoutParent)
                {
                    struct KzcMatrix4x4 childTransform = kzuObjectNodeGetTransformation(objectNode);
                    struct KzcMatrix4x4 parentTransform = kzuTransformedObjectNodeGetMatrix(parent);
                    struct KzcVector3 layoutMin;
                    struct KzcVector3 layoutMax;
                    struct KzcVector3 center;

                    kzuTransformedObjectNodeGetLayoutSize(objectInWorldCoordinates, &layoutMin, &layoutMax);

                    center.data[0] = (layoutMin.data[0] + layoutMax.data[0]) * 0.5f;
                    center.data[1] = (layoutMin.data[1] + layoutMax.data[1]) * 0.5f;
                    center.data[2] = (layoutMin.data[2] + layoutMax.data[2]) * 0.5f;

                    kzcMatrix4x4SetPositionVector3(&childTransform, &center);

                    kzcMatrix4x4Multiply(&childTransform, &parentTransform, &transform);
                }
                else
                {
                    transform = kzuTransformedObjectNodeGetMatrix(objectInWorldCoordinates);
                }


            }
            else
            {
                kzuTransformedObjectNodeGetLayoutSize(objectInWorldCoordinates, &objectMin, &objectMax);

                if(isLayoutParent)
                {
                    transform = kzuTransformedObjectNodeGetMatrix(parent);
                }
                else
                {
                    transform = KZC_MATRIX4X4_IDENTITY;
                }
            }

            result = kzuRendererDrawTransformedBoundingVolumeAB(renderer, &objectMin, &objectMax, &transform, &KZC_COLOR_RED);
            kzsErrorForward(result);

            if(identifier == KZU_COMPONENT_TYPE_GRID_LAYOUT)
            {
                result = kzuRendererDrawGridLayoutCells(renderer, &objectMin, &objectMax, &transform, &camera, uiNode, &KZC_COLOR_RED);
                kzsErrorForward(result);              
            }
        }

        if (application->layoutActualVisualization)
        {
            struct KzcMatrix4x4 transform = kzuTransformedObjectNodeGetMatrix(objectInWorldCoordinates);


            if(isLayout)
            {
                if(application->isActualLayoutVisualizationRecursive)
                {
                    struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(objectNode);
                    while(kzcDynamicArrayIterate(it))
                    {
                        struct KzuObjectNode* child = kzcDynamicArrayIteratorGetValue(it);

                        result = kzaApplicationVisualizeLayout_internal(application, objectList, child);
                        kzsErrorForward(result);
                    }
                }

                /* TODO: ContentSize may change meaning later, this will break at that point. */
                kzuTransformedObjectNodeGetContentSize(objectInWorldCoordinates, &objectMin, &objectMax);

                result = kzuRendererDrawGridLayoutElementPositions(kzuEngineGetRenderer(application->engine), &objectMin, &objectMax, &transform, &camera, objectInWorldCoordinates, &KZC_COLOR_BLUE);
                kzsErrorForward(result);
            }
            else
            {
                kzuTransformedObjectNodeGetContentSize(objectInWorldCoordinates, &objectMin, &objectMax);

                if(isLayoutParent)
                {
                    struct KzcMatrix4x4 childTransform;
                    struct KzcMatrix4x4 parentTransform = kzuTransformedObjectNodeGetMatrix(parent);
                    struct KzcVector3 layoutMin;
                    struct KzcVector3 layoutMax;
                    struct KzcVector3 center;

                    kzuTransformedObjectNodeGetLayoutSize(objectInWorldCoordinates, &layoutMin, &layoutMax);

                    center.data[0] = (layoutMin.data[0] + layoutMax.data[0]) * 0.5f - (objectMin.data[0] + objectMax.data[0]) * 0.5f;
                    center.data[1] = (layoutMin.data[1] + layoutMax.data[1]) * 0.5f - (objectMin.data[1] + objectMax.data[1]) * 0.5f;
                    center.data[2] = (layoutMin.data[2] + layoutMax.data[2]) * 0.5f - (objectMin.data[2] + objectMax.data[2]) * 0.5f;

                    kzcMatrix4x4SetIdentity(&childTransform);
                    kzcMatrix4x4SetPositionVector3(&childTransform, &center);

                    kzcMatrix4x4Multiply(&childTransform, &parentTransform, &transform);
                }
            }
            result = kzuRendererDrawTransformedBoundingVolumeAB(kzuEngineGetRenderer(application->engine), &objectMin, &objectMax, &transform, &KZC_COLOR_BLUE);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzaApplicationUninitialize(struct KzaApplication* application)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));

    result = kzApplicationUninitialize(application);
    kzsErrorForward(result);

    result = kzuTransformedSceneDelete(application->transformedScene);
    kzsErrorForward(result);

    if (application->transformGizmo != KZ_NULL)
    {
        result = kzuTransformGizmoDelete(application->transformGizmo);
        kzsErrorForward(result);
    }

    if (application->pick != KZ_NULL)
    {
        result = kzuPickDelete(application->pick);
        kzsErrorForward(result);
    }

#ifdef KZA_APPLICATION_ENABLE_SPLASH_SCREEN
    result = kzaSplashScreenDelete(application->splash, kzuRendererGetCoreRenderer(kzuEngineGetRenderer(kzaApplicationGetEngine(application))));
    kzsErrorForward(result);
#endif

    result = kzuHudDelete(application->hud);
    kzsErrorForward(result);

    result = kzuFreeCameraDelete(application->freeCamera);
    kzsErrorForward(result);

    result = kzuEngineDelete(application->engine);
    kzsErrorForward(result);

    result = kzuProjectDelete(application->project);
    kzsErrorForward(result);

    result = kzuProjectPatcherDelete(application->projectPatcher);
    kzsErrorForward(result);

    if(application->binaryLoader != KZ_NULL)
    {
        result = kzuBinaryLoaderDelete(application->binaryLoader);
        kzsErrorForward(result);
    }
    else
    {
        result = kzuBinaryDirectoryDelete(application->directory);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzaApplicationUpdate(struct KzaApplication* application, kzUint deltaTime)
{
    kzsError result;

    /* Pause occurred. */
    if(application->applicationPausing)
    {
        application->applicationPausing = KZ_FALSE;
        application->state = KZA_APPLICATION_STATE_PAUSED;

#ifdef KZA_APPLICATION_RELOAD_GPU_RESOURCES_ON_RESUME
        result = kzaSurfaceDeleteAll(application);
        kzsErrorForward(result);
        result = kzaWindowDeleteAll(application);
        kzsErrorForward(result);
#endif
    }

    /* Resume occurred. */
    if(application->applicationResuming)
    {
        application->applicationResuming = KZ_FALSE;

        application->state = KZA_APPLICATION_STATE_RUNNING;

#ifdef KZA_APPLICATION_RELOAD_GPU_RESOURCES_ON_RESUME
        result = kzcDynamicArrayCreate(kzcMemoryGetManager(application), &application->windows);
        kzsErrorForward(result);

        result = kzcDynamicArrayCreate(kzcMemoryGetManager(application), &application->surfaces);
        kzsErrorForward(result);

        result = kzaWindowCreate(application, &application->applicationProperties.windowProperties, &application->applicationProperties.surfaceProperties,
            &application->window);
        kzsErrorForward(result);

        kzuEngineSetActiveWindow(application->engine, application->window);
        kzcRendererSetActiveWindow(kzuRendererGetCoreRenderer(kzuEngineGetRenderer(application->engine)), application->window);

        {
            kzUint startTime = kzsTimeGetCurrentTimestamp();
            result = kzcResourceManagerFreeResourcesFromGPU(kzuProjectGetResourceManager(application->project));
            kzsErrorForward(result);
            result = kzcResourceManagerFreeResourcesFromGPU(kzuEngineGetResourceManager(application->engine));
            kzsErrorForward(result);
            result = kzcResourceManagerUploadResourcesToGPU(kzuProjectGetResourceManager(application->project));
            kzsErrorForward(result);
            result = kzcResourceManagerUploadResourcesToGPU(kzuEngineGetResourceManager(application->engine));
            kzsErrorForward(result);
            kzcLogDebug("Resume time total for resources: %d", (kzsTimeGetCurrentTimestamp() - startTime));
        }
#endif

        result = kzuRendererReset(kzuEngineGetRenderer(application->engine));
        kzsErrorForward(result);
    }

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    result = kzsWindowSetActive(application->window, KZ_TRUE);
    kzsErrorForward(result);
#endif

    if (application->splashTimer >= 0)
    {
        application->splashTimer -= (kzInt)kzuEngineGetDeltaTime(application->engine);
    }
    
    if (application->splashTimer < 0)
    {
        if (!application->applicationInitialized)
        {
            result = kzaApplicationInitializeProject_internal(application);
            kzsErrorForward(result);

            application->applicationInitialized = KZ_TRUE;
        }

        /* Animate first. */
        if (kzuSceneIsValid(application->currentScene))
        {
            result = kzuSceneAnimate(application->currentScene, application->isDeltaTimeSkippedForFrame ? 0 : deltaTime);
            kzsErrorForward(result);
            application->isDeltaTimeSkippedForFrame = KZ_FALSE;
        }

        result = kzApplicationUpdate(application, deltaTime);
        kzsErrorForward(result);

        result = kzaTransitionUpdate(application->transition, application, deltaTime);
        kzsErrorForward(result);

        if (kzuSceneIsValid(application->currentScene))
        {
            kzuRendererSetBoundingBoxVisualizationEnabled(kzuEngineGetRenderer(application->engine), application->boundingBoxEnabled);

            /* Handle messages. */
            result = kzaApplicationHandleEventQueue(application, deltaTime);
            kzsErrorForward(result);

            /* Update free camera. */
            if (application->freeCameraEnabled)
            {
                result = kzuFreeCameraUpdate(application->freeCamera, kzuEngineGetDeltaTime(application->engine));
                kzsErrorForward(result);
            }

            {
                struct KzuCameraNode* cameraNode = kzuSceneGetViewCamera(application->currentScene);
                struct KzuRenderer* renderer = kzuEngineGetRenderer(application->engine);
                kzString cameraName = (cameraNode == KZ_NULL ? "" : kzuObjectNodeGetName(kzuCameraNodeToObjectNode(cameraNode)));
                result = kzuHudUpdate(application->hud, kzuEngineGetFramesPerSecond(application->engine), 
                                      kzuRendererGetBatchCount(renderer), kzuRendererGetTriangleCount(renderer),
                                      kzuRendererGetTextureSwitchCount(renderer), kzuRendererGetShaderSwitchCount(renderer),
                                      cameraName, kzuAnimationPlayerGetTime(kzuSceneGetAnimationPlayer(application->currentScene)));
                kzsErrorForward(result);
                if(application->infoHudEnabled)
                {
                    kzUint i;
                    kzMutableString texts[11];
                    kzUint measurements[5];
                    
                    result = kzcStringFormat(kzcMemoryGetManager(application), "Layouting time: %i ms", &texts[0], kzuSceneGetPerformanceMeasurement(kzaApplicationGetScene(application), KZU_SCENE_PERFORMANCE_MEASUREMENT_LAYOUT));
                    kzsErrorForward(result);
                    result = kzcStringFormat(kzcMemoryGetManager(application), "Extract time: %i ms", &texts[1], kzuSceneGetPerformanceMeasurement(kzaApplicationGetScene(application), KZU_SCENE_PERFORMANCE_MEASUREMENT_EXTRACT));
                    kzsErrorForward(result);
                    result = kzcStringFormat(kzcMemoryGetManager(application), "Transform time: %i ms", &texts[2], kzuSceneGetPerformanceMeasurement(kzaApplicationGetScene(application), KZU_SCENE_PERFORMANCE_MEASUREMENT_TRANSFORM));
                    kzsErrorForward(result);
                    result = kzcStringFormat(kzcMemoryGetManager(application), "Arrange time: %i ms", &texts[3], kzuSceneGetPerformanceMeasurement(kzaApplicationGetScene(application), KZU_SCENE_PERFORMANCE_MEASUREMENT_ARRANGE));
                    kzsErrorForward(result);
                    result = kzcStringFormat(kzcMemoryGetManager(application), "Constraints time: %i ms", &texts[4], kzuSceneGetPerformanceMeasurement(kzaApplicationGetScene(application), KZU_SCENE_PERFORMANCE_MEASUREMENT_CONSTRAINTS));
                    kzsErrorForward(result);
                    measurements[0] = kzuObjectSourceGetMeasurementInfo(kzuSceneGetRootObjectSource(kzaApplicationGetScene(application)), KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_SET_APPLY)
                        + kzuObjectSourceGetMeasurementInfo(kzuSceneGetRootObjectSource(kzaApplicationGetScene(application)), KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_SET_START);
                    measurements[1] = kzuObjectSourceGetMeasurementInfo(kzuSceneGetRootObjectSource(kzaApplicationGetScene(application)), KZU_OBJECT_SOURCE_MEASUREMENT_FRUSTUM_CULL_APPLY)
                        + kzuObjectSourceGetMeasurementInfo(kzuSceneGetRootObjectSource(kzaApplicationGetScene(application)), KZU_OBJECT_SOURCE_MEASUREMENT_FRUSTUM_CULL_START);
                    measurements[2] = kzuObjectSourceGetMeasurementInfo(kzuSceneGetRootObjectSource(kzaApplicationGetScene(application)), KZU_OBJECT_SOURCE_MEASUREMENT_SORT_APPLY)
                        + kzuObjectSourceGetMeasurementInfo(kzuSceneGetRootObjectSource(kzaApplicationGetScene(application)), KZU_OBJECT_SOURCE_MEASUREMENT_SORT_START);
                    measurements[3] = kzuObjectSourceGetMeasurementInfo(kzuSceneGetRootObjectSource(kzaApplicationGetScene(application)), KZU_OBJECT_SOURCE_MEASUREMENT_OBJECT_TYPE_APPLY);
                    measurements[4] = kzuObjectSourceGetMeasurementInfo(kzuSceneGetRootObjectSource(kzaApplicationGetScene(application)), KZU_OBJECT_SOURCE_MEASUREMENT_LOD_START)
                         + kzuObjectSourceGetMeasurementInfo(kzuSceneGetRootObjectSource(kzaApplicationGetScene(application)), KZU_OBJECT_SOURCE_MEASUREMENT_LOD_APPLY);
                    result = kzcStringFormat(kzcMemoryGetManager(application), "Object set time: %i ms", &texts[5], measurements[0]);
                    kzsErrorForward(result);
                    result = kzcStringFormat(kzcMemoryGetManager(application), "Frustum cull time: %i ms", &texts[6], measurements[1]);
                    kzsErrorForward(result);
                    result = kzcStringFormat(kzcMemoryGetManager(application), "Sorting time: %i ms", &texts[7], measurements[2]);
                    kzsErrorForward(result);
                    result = kzcStringFormat(kzcMemoryGetManager(application), "Object type time: %i ms", &texts[8], measurements[3]);
                    kzsErrorForward(result);

                    result = kzcStringFormat(kzcMemoryGetManager(application), "Animation time: %i ms", &texts[9], kzuSceneGetPerformanceMeasurement(kzaApplicationGetScene(application), KZU_SCENE_PERFORMANCE_MEASUREMENT_ANIMATION));
                    kzsErrorForward(result);

                    result = kzcStringFormat(kzcMemoryGetManager(application), "LOD time: %i ms", &texts[10], measurements[4]);
                    kzsErrorForward(result);

                    for(i = 0; i < 11; ++i)
                    {
                        result = kzuHudAddCustomTextLine(application->hud, texts[i]);
                        kzsErrorForward(result);

                        result = kzcStringDelete(texts[i]);
                        kzsErrorForward(result);
                    }
                }
            }

            result = kzuEngineStartUpdate(application->engine);
            kzsErrorForward(result);

            result = kzuEngineApplyGlobalProperties(application->engine, kzuProjectGetPropertyManager(application->project), application->project);
            kzsErrorForward(result);

            result = kzuEngineExtractScene(application->engine, application->currentScene, application->transformedScene);
            kzsErrorForward(result);

            /* Handle input. */
            {
                const struct KzsWindow* window = kzaApplicationGetWindow(application);
                if(window != KZ_NULL)
                {
                    struct KzsInputEventQueue* queue = kzsWindowGetInputEventQueue(window);
                    struct KzsInputEventQueueIterator it = kzsInputEventQueueGetIterator(queue);                

                    while (kzsInputEventQueueIteratorIterate(it))
                    {
                        const struct KzsInputEvent* inputEvent = kzsInputEventQueueIteratorGetValue(it);
                        /* These objects are accessed every iteration since they can change between calls. */
                        struct KzuEngine* engine = kzaApplicationGetEngine(application);
                        struct KzuRenderer* renderer = kzuEngineGetRenderer(engine);
                        struct KzsWindow* window = kzaApplicationGetWindow(application);
                        struct KzuTransformedScene* transformedScene = kzaApplicationGetTransformedScene(application);
                        struct KzuCameraNode* cameraNode = kzuTransformedSceneGetViewCamera(transformedScene);

                        result = kzuInputManagerProcessInputEvent(inputEvent, renderer, window, transformedScene, cameraNode);
                        kzsErrorForward(result);
                    }
                }
            }

            result = kzuMessageDispatcherDispatch(application->messageDispatcher);
            kzsErrorForward(result);

            if (application->userInterfaceEnabled)
            {
                result = kzuEngineUpdateUi(application->engine, application->transformedScene, deltaTime);
                kzsErrorForward(result); 
            }
        }
    }

    kzsSuccess();
}

kzsError kzaApplicationUpdateWindows(struct KzaApplication* application)
{
    kzsError result;
    struct KzcDynamicArray* windowArray;
    struct KzcDynamicArrayMutableIterator windowIterator;
 
    /* TODO: fix multi-windowing.. needs some more planning and refactoring. */

    result = kzuEngineBeginFrame(application->engine);
    kzsErrorForward(result);

    if (application->splashTimer >= 0)
    {
#ifdef KZA_APPLICATION_ENABLE_SPLASH_SCREEN
        result = kzaSplashScreenRender(application->splash, kzuRendererGetCoreRenderer(kzuEngineGetRenderer(kzaApplicationGetEngine(application))),
                                       kzaApplicationGetWindow(application));
        kzsErrorForward(result);
#endif
    }
    else
    {
        if (kzuSceneIsValid(application->currentScene))
        {
            /* Render. */
            result = kzuEngineRender(application->engine, application->transformedScene);
            kzsErrorForward(result);

            /* Set viewport and camera. */
            {
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
                struct KzuRenderer* renderer = kzuEngineGetRenderer(application->engine);
                struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);

                struct KzuCameraNode* sceneViewCamera = kzuSceneGetViewCamera(application->currentScene);
                kzFloat viewportX, viewportY, viewportWidth, viewportHeight;

                kzuRendererGetAdjustedViewport(kzuEngineGetRenderer(application->engine), application->window, &viewportX,
                    &viewportY, &viewportWidth, &viewportHeight);

                kzcRendererSetViewport(coreRenderer, kzsFloatToUint(viewportX), kzsFloatToUint(viewportY),
                    kzsFloatToUint(viewportWidth), kzsFloatToUint(viewportHeight));

                if(sceneViewCamera != KZ_NULL)
                {
                    struct KzuTransformedObjectNode* transformedCameraNode;
                    transformedCameraNode = kzuTransformedObjectListFindObject(kzuTransformedSceneGetObjects(application->transformedScene), kzuCameraNodeToObjectNode(sceneViewCamera));

                    if(transformedCameraNode != KZ_NULL)
                    {
                        result = kzuRendererApplyCameraNode(renderer, transformedCameraNode, viewportX, viewportY, viewportWidth, viewportHeight);
                        kzsErrorForward(result);
                    }
                }
#endif
            }

            /* Draw visualization grid. */
            if (application->gridEnabled)
            {
                result = kzuEngineDrawVisualizationGrid(application->engine, application->transformedScene,
                                                        kzaApplicationGetTransformGizmoGridSize(application));
                kzsErrorForward(result);
            }

            /* Debug objects enabled. */
            if (application->debugObjectsEnabled)
            {
                result = kzuEngineDrawDebugObjects(application->engine, application->transformedScene);
                kzsErrorForward(result);
            }

            /* Transform gizmo. */
            if (application->transformGizmoEnabled && application->transformGizmo != KZ_NULL)
            {
                kzString target = kzuTransformGizmoGetTarget(application->transformGizmo);

                result = kzuTransformGizmoRender(application->transformGizmo, kzaApplicationGetEngine(application),
                                                 kzaApplicationGetProject(application), application->transformedScene);
                kzsErrorForward(result);

                /* Draw layout visualizations. */
                if (target != KZ_NULL && (application->layoutActualVisualization || application->layoutAllocatedVisualization))
                {
                    struct KzcDynamicArray* objectList;
                    struct KzuObjectNode* objectNode = kzuProjectGetObject(kzaApplicationGetProject(application), KZU_PROJECT_OBJECT_TYPE_OBJECT_NODE, target);
                    result = kzuObjectSourceFetchGraph(kzuProjectGetRootSource(kzaApplicationGetProject(application)),
                                                       kzuTransformedSceneGetObjectSourceRuntimeData(kzaApplicationGetTransformedScene(application)),
                                                       KZ_NULL, &objectList);
                    kzsErrorForward(result);

                    result = kzaApplicationVisualizeLayout_internal(application, objectList, objectNode);
                    kzsErrorForward(result);
                }
            }
        }

        result = kzApplicationRender(application);
        kzsErrorForward(result);


#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
        result = kzuRendererDrawBorders(kzuEngineGetRenderer(application->engine), application->window);
        kzsErrorForward(result);
#endif
    }

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    if (application->infoHudEnabled)
    {
        result = kzuHudRenderDebugInfo(application->hud, kzuEngineGetRenderer(application->engine), 
            kzuRendererGetQuickMemoryManager(kzuEngineGetRenderer(application->engine)), application->window);
        kzsErrorForward(result);
    }

    if (application->logVisualizationEnabled)
    {
        result = kzuHudRenderLog(application->hud, kzuEngineGetRenderer(application->engine), 
            kzuRendererGetQuickMemoryManager(kzuEngineGetRenderer(application->engine)), application->window);
        kzsErrorForward(result);
    }
#endif

    result = kzuEngineEndFrame(application->engine);
    kzsErrorForward(result);

    /* Process all windows and update them. */
    windowArray = kzaApplicationGetWindowArray_private(application);
    windowIterator = kzcDynamicArrayGetMutableIterator(windowArray);
    while (kzcDynamicArrayMutableIterate(windowIterator))
    {
        struct KzsWindow* window = (struct KzsWindow*)kzcDynamicArrayMutableIteratorGetValue(windowIterator);

        /* Is it a valid window? */
        if(kzsWindowGetState(window) != KZS_WINDOW_STATE_UNINITIALIZED)
        {
            result = kzsWindowUpdate(window);
            kzsErrorForward(result);
        }

        if(kzsWindowIsClosed(window))
        {
            result = kzcDynamicArrayMutableIteratorRemove(windowIterator);
            kzsErrorForward(result);
            result = kzsWindowDelete(window);
            kzsErrorForward(result);
        }
    }

    if (kzcDynamicArrayIsEmpty(windowArray))
    {
        /* No windows created or last window closed, close the application. */
        kzaApplicationQuit(application);
    }

    kzsSuccess();
}

static kzsError kzaApplicationInputPointingDeviceHandler_internal(const struct KzsPointingDeviceInputData* inputData,
                                                                  void* userData)
{
    kzsError result;
    struct KzaApplication* application = (struct KzaApplication*)userData;
    enum KzsInputPointingDeviceState state = kzsInputGetPointingDeviceState(inputData);
    kzU32 buttons = kzsInputGetPointingDeviceButtons(inputData);
    kzInt x = kzsInputGetPointingDeviceX(inputData);
    kzInt y = kzsInputGetPointingDeviceY(inputData);
    kzBool gizmoHandledInput = KZ_FALSE;

    kzsAssert(kzcIsValidPointer(application));

    if (kzuSceneIsValid(application->currentScene) &&
        kzuSceneIsValid(kzuTransformedSceneGetScene(application->transformedScene)))
    {
        if (application->transformGizmoEnabled && application->transformGizmo != KZ_NULL)
        {
            result = kzuTransformGizmoProcessInput(application->transformGizmo, kzaApplicationGetEngine(application),
                                                   kzaApplicationGetProject(application), application->transformedScene,
                                                   application->pick, x, y, state, &gizmoHandledInput);
            kzsErrorForward(result);
        }

        if (application->pickingEnabled && application->pick != KZ_NULL &&
            application->pickCallback != KZ_NULL && !gizmoHandledInput)
        {
            if (state == KZS_POINTING_DEVICE_STATE_UP)
            {
                kzString name;

                if (application->toolPickMode == KZA_TOOL_PICK_MODE_NODE)
                {
                    struct KzuObjectNode* objectNode;
                    result = kzuPick(application->pick, kzaApplicationGetEngine(application), kzuProjectGetRootSource(kzaApplicationGetProject(application)),
                                     application->transformedScene, kzuSceneGetViewCamera(kzaApplicationGetScene(application)), (kzUint)x, (kzUint)y, &objectNode, KZ_NULL);
                    kzsErrorForward(result);
                    name = (objectNode != KZ_NULL ? kzuProjectGetObjectPath(application->project, objectNode) : KZ_NULL);
                }
                else
                {
                    struct KzuMaterial* material;
                    result = kzuPickMaterial(application->pick, kzaApplicationGetEngine(application), kzuProjectGetRootSource(kzaApplicationGetProject(application)),
                                             application->transformedScene, kzuSceneGetViewCamera(kzaApplicationGetScene(application)), (kzUint)x, (kzUint)y, &material);
                    kzsErrorForward(result);
                    name = (material != KZ_NULL ? kzuProjectGetObjectPath(application->project, material) : KZ_NULL);
                }

                if (name != KZ_NULL)
                {
                    result = application->pickCallback(name, application->pickCallbackUserData);
                    kzsErrorForward(result);
                }
            }
        }

        /* Handle input only if the gizmo didn't handle it. */
        if (!gizmoHandledInput)
        {
            kzuSceneHandlePointerEvents(application->currentScene, buttons, state, kzsInputGetPointingDeviceX(inputData), kzsInputGetPointingDeviceY(inputData));

            if (application->freeCameraEnabled && application->freeCamera != KZ_NULL)
            {
                kzuFreeCameraHandlePointerEvents(application->freeCamera, buttons, state, kzsInputGetPointingDeviceX(inputData), kzsInputGetPointingDeviceY(inputData));
            }
        }
    }

    if (application->pointingDeviceHandler != KZ_NULL)
    {
        if (application->state == KZA_APPLICATION_STATE_RUNNING ||
            application->state == KZA_APPLICATION_STATE_PAUSED)
        {
            result = application->pointingDeviceHandler(application, inputData);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

static kzsError kzaApplicationInputKeyDeviceHandler_internal(const struct KzsKeyDeviceInputData* inputData,
                                                             void* userData)
{
    kzsError result;
    struct KzaApplication* application = (struct KzaApplication*)userData;

    kzsAssert(kzcIsValidPointer(application));

    if (application->defaultKeyHandlingEnabled)
    {
        enum KzsInputKey button = kzsInputGetKeyDeviceButton(inputData);
        enum KzsInputKeyDeviceState state = kzsInputGetKeyDeviceState(inputData);

        kzuSceneHandleKeyEvents(application->currentScene, button, state);

        if (application->freeCameraEnabled)
        {
            kzuFreeCameraHandleKeyEvents(application->freeCamera, button, state);
        }
    }

    if (application->keyDeviceHandler != KZ_NULL)
    {
        result = application->keyDeviceHandler(application, inputData);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzaApplicationInputEventHandler(const struct KzsInputEventQueue* queue, void* userData)
{
    kzsError result;
    struct KzsInputEventQueueIterator it;

    kzsAssert(queue != NULL);

    it = kzsInputEventQueueGetIterator(queue);
    while(kzsInputEventQueueIteratorIterate(it))
    {
        const struct KzsInputEvent* event = kzsInputEventQueueIteratorGetValue(it);
        enum KzsInputEventType type = kzsInputEventGetType(event);

        if(type == KZS_INPUT_EVENT_POINTER)
        {
            const struct KzsPointingDeviceInputData* data = kzsInputEventGetPointingDeviceData((struct KzsInputEvent*)event);

            result = kzaApplicationInputPointingDeviceHandler_internal(data, userData);
            kzsErrorForward(result);
        }
        else
        {
            const struct KzsKeyDeviceInputData* data = kzsInputEventGetKeyDeviceData((struct KzsInputEvent*)event);

            result = kzaApplicationInputKeyDeviceHandler_internal(data, userData);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError kzaApplicationWindowEventHandler(struct KzsWindow* window, enum KzsWindowEventType type, void* userData)
{
    kzsError result;
    struct KzaApplication* application = (struct KzaApplication*)userData;

    /* TODO: Add any app framework specific updates here */

    /* Pass the event to the application. */
    if (application->windowEventHandler != KZ_NULL)
    {
        result = application->windowEventHandler(application, type, window);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzaApplicationPause(struct KzaApplication* application)
{
    kzsError result;
    application->applicationPausing = KZ_TRUE;

    if (application->changeHandler != KZ_NULL)
    {
        struct KzaApplicationChangeEvent event;
        event.type = KZA_APPLICATION_PAUSED;

        result = application->changeHandler(application, &event);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzaApplicationResume(struct KzaApplication* application)
{
    kzsError result;
    application->applicationResuming = KZ_TRUE;

    if (application->changeHandler != KZ_NULL)
    {
        struct KzaApplicationChangeEvent event;
        event.type = KZA_APPLICATION_RESUMED;

        result = application->changeHandler(application, &event);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

static kzsError kzaApplicationReadConfiguration_internal(const struct KzcMemoryManager* memoryManager,
                                                         struct KzaApplicationProperties* configuration, kzString configPath)
{
    kzsError result;

    if (kzcFileExists(configPath))
    {
        struct KzcSettingContainer* container;
        kzMutableString fileData;
        kzBool valueFound;

        result = kzcFileReadTextFile(memoryManager, configPath, &fileData);
        kzsErrorForward(result);

        result = kzcSettingContainerCreate(memoryManager, &container);
        kzsErrorForward(result);

        result = kzcSettingsParseString(container, fileData);
        kzsErrorForward(result);

        valueFound = kzcSettingNodeGetInteger(kzcSettingContainerGetRoot(container), "ApplicationMemoryManagerSize", (kzInt*)&configuration->memoryPoolSize);
        KZ_UNUSED_PARAMETER(valueFound);
        valueFound = kzcSettingNodeGetInteger(kzcSettingContainerGetRoot(container), "ApplicationMemoryPoolCount", (kzInt*)&configuration->memoryPoolCount);
        KZ_UNUSED_PARAMETER(valueFound);
        /* Surface properties. */
        valueFound = kzcSettingNodeGetInteger(kzcSettingContainerGetRoot(container), "SurfaceBitsStencil", (kzInt*)&configuration->surfaceProperties.bitsStencil);
        KZ_UNUSED_PARAMETER(valueFound);
        valueFound = kzcSettingNodeGetInteger(kzcSettingContainerGetRoot(container), "SurfaceBitsDepthBuffer", (kzInt*)&configuration->surfaceProperties.bitsDepthBuffer);
        KZ_UNUSED_PARAMETER(valueFound);
        valueFound = kzcSettingNodeGetInteger(kzcSettingContainerGetRoot(container), "SurfaceBitsRed", (kzInt*)&configuration->surfaceProperties.bitsColorR);
        KZ_UNUSED_PARAMETER(valueFound);
        valueFound = kzcSettingNodeGetInteger(kzcSettingContainerGetRoot(container), "SurfaceBitsGreen", (kzInt*)&configuration->surfaceProperties.bitsColorG);
        KZ_UNUSED_PARAMETER(valueFound);
        valueFound = kzcSettingNodeGetInteger(kzcSettingContainerGetRoot(container), "SurfaceBitsBlue", (kzInt*)&configuration->surfaceProperties.bitsColorB);
        KZ_UNUSED_PARAMETER(valueFound);
        valueFound = kzcSettingNodeGetInteger(kzcSettingContainerGetRoot(container), "SurfaceBitsAlpha", (kzInt*)&configuration->surfaceProperties.bitsAlpha);
        KZ_UNUSED_PARAMETER(valueFound);
        valueFound = kzcSettingNodeGetInteger(kzcSettingContainerGetRoot(container), "SurfaceSamplesAntialiasing", (kzInt*)&configuration->surfaceProperties.antiAliasing);
        KZ_UNUSED_PARAMETER(valueFound);

        result = kzcSettingContainerDelete(container);
        kzsErrorForward(result);
        result = kzcStringDelete(fileData);
        kzsErrorForward(result);
    }
    else
    {
        kzMutableString fileNotFoundString;

        result = kzcStringFormat(memoryManager, "Settings file %s not found, using default configuration.", &fileNotFoundString, configPath);
        kzsErrorForward(result);
        kzsLog(KZS_LOG_LEVEL_INFO, fileNotFoundString);
        result = kzcStringDelete(fileNotFoundString);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

KzaApplicationPointingDeviceHandler kzaApplicationGetPointingDeviceHandler(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->pointingDeviceHandler;
}

KzaApplicationKeyDeviceHandler kzaApplicationGetKeyDeviceHandler(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->keyDeviceHandler;
}

void kzaApplicationSetPointingDeviceHandler(struct KzaApplication* application, KzaApplicationPointingDeviceHandler handler)
{
    kzsAssert(kzcIsValidPointer(application));
    application->pointingDeviceHandler = handler;
}

void kzaApplicationSetKeyDeviceHandler(struct KzaApplication* application, KzaApplicationKeyDeviceHandler handler)
{
    kzsAssert(kzcIsValidPointer(application));
    application->keyDeviceHandler = handler;
}

KzaApplicationWindowEventHandler kzaApplicationGetWindowEventHandler(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->windowEventHandler;
}

void kzaApplicationSetWindowEventHandler(struct KzaApplication* application, KzaApplicationWindowEventHandler handler)
{
    kzsAssert(kzcIsValidPointer(application));
    application->windowEventHandler = handler;
}

enum KzaApplicationState kzaApplicationGetState(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->state;
}

void kzaApplicationSetState(struct KzaApplication* application, enum KzaApplicationState state)
{
    kzsAssert(kzcIsValidPointer(application));
    application->state = state;
}

void kzaApplicationQuit(struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    application->state = KZA_APPLICATION_STATE_QUITTING;
}

struct KzaPlatform* kzaApplicationGetPlatform(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->platform;
}

/* TODO: remove, rename or hide */
struct KzcDynamicArray* kzaApplicationGetWindowArray_private(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->windows;
}

struct KzcDynamicArray* kzaApplicationGetSurfaceArray(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->surfaces;
}

struct KzcDynamicArray* kzaApplicationGetDisplayArray(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->displays;
}

void* kzaApplicationGetUserData(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->userData;
}

void kzaApplicationSetUserData(struct KzaApplication* application, void* userData)
{
    kzsAssert(kzcIsValidPointer(application));
    application->userData = userData;
}

struct KzcMemoryManager* kzaApplicationGetSystemMemoryManager(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->systemMemoryManager;
}

struct KzcMemoryManager* kzaApplicationGetApplicationMemoryManager(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->applicationMemoryManager;
}

void kzaApplicationSetWindowCreator(struct KzaApplication* application, KzaApplicationWindowCreatorFunction windowCreatorFunction)
{
    kzsAssert(kzcIsValidPointer(application));
    application->windowCreatorFunction = windowCreatorFunction;
}

KzaApplicationWindowCreatorFunction kzaApplicationGetWindowCreator(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->windowCreatorFunction;
}

kzsError kzaApplicationSetToolData(struct KzaApplication* application, KzaApplicationToolReloadHandler toolReloadFunction)
{
    kzsAssert(kzcIsValidPointer(application));
    kzsErrorTest(!application->toolPresent, KZS_ERROR_ILLEGAL_OPERATION, "ApplicationSetToolData function must not be called multiple times for one instance of application.");
    application->toolPresent = KZ_TRUE;
    application->toolReloadFunction = toolReloadFunction;
    kzsSuccess();
}

kzBool kzaToolIsPresent(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->toolPresent;
}

void kzaToolRequestReload(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    if(application->toolReloadFunction != KZ_NULL)
    {
        application->toolReloadFunction();
    }
}

void kzaApplicationSetChangeHandler(struct KzaApplication* application, KzaApplicationChangeHandler changeHandler)
{
    kzsAssert(kzcIsValidPointer(application));
    application->changeHandler = changeHandler;
}

void kzaApplicationSetApplySceneHandler(struct KzaApplication* application, KzaApplicationApplySceneHandler handler)
{
    kzsAssert(kzcIsValidPointer(application));
    application->applySceneHandler = handler;
}

void kzaApplicationSetPatchProjectHandler(struct KzaApplication* application, KzaApplicationPatchProjectHandler handler)
{
    kzsAssert(kzcIsValidPointer(application));
    application->patchProjectHandler = handler;
}

void kzaApplicationSetReloadProjectHandler(struct KzaApplication* application, KzaApplicationReloadProjectHandler handler)
{
    kzsAssert(kzcIsValidPointer(application));
    application->reloadProjectHandler = handler;
}

kzsError kzaApplicationSetScene(struct KzaApplication* application, struct KzuScene* scene)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));

    if (application->currentScene != KZ_NULL)
    {
        result = removeFreeCamera_internal(application);
        kzsErrorForward(result);
    }

    application->currentScene = scene;

    if (scene != KZ_NULL)
    {
        struct KzuAnimationPlayer* animationPlayer = kzuSceneGetAnimationPlayer(scene);

        application->viewCamera = kzuSceneGetViewCamera(scene);

        result = addFreeCamera_internal(application, KZ_TRUE);
        kzsErrorForward(result);

        /* Reset UI screen state. */
        result = kzuUiScreenReset(kzuSceneGetUiScreen(scene));
        kzsErrorForward(result);

        /* Reset animation player. */
        kzuAnimationPlayerSetTime(animationPlayer, 0.0f);
        kzuAnimationPlayerSetPaused(animationPlayer, KZ_FALSE);
    }

    /* Reset renderer, due to different light count etc. */
    result = kzuRendererReset(kzuEngineGetRenderer(application->engine));
    kzsErrorForward(result);

    if(kzaToolIsPresent(application))
    {
        struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(kzuEngineGetRenderer(application->engine));

        kzcRendererSetClearColor(coreRenderer, KZC_COLOR_BLACK);
        kzcRendererAddClearTarget(coreRenderer, KZC_RENDERER_CLEAR_TARGET_COLOR);
        kzcRendererClear(coreRenderer);
    }

    kzsSuccess();
}

kzsError kzaApplicationSetScenePath(struct KzaApplication* application, kzString scenePath)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));

    if (application->applySceneHandler != KZ_NULL)
    {
        result = application->applySceneHandler(application, scenePath);
        kzsErrorForward(result);
    }
    else
    {
        struct KzuScene* oldScene = application->currentScene;
        struct KzuScene* scene;
        struct KzaApplicationChangeEvent event;

        if (scenePath != KZ_NULL)
        {
            result = kzuProjectLoaderLoadScene(application->project, scenePath, &scene);
            kzsErrorForward(result);
        }
        else
        {
            scene = KZ_NULL;
        }

        result = kzaApplicationSetScene(application, scene);
        kzsErrorForward(result);

        if (application->changeHandler != KZ_NULL)
        {
            event.type = KZA_APPLICATION_CHANGE_EVENT_SCENE_CHANGED;
            event.data.sceneChanged.oldScene = oldScene;
            event.data.sceneChanged.newScene = scene;

            result = application->changeHandler(application, &event);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

struct KzuScene* kzaApplicationGetScene(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->currentScene;
}

struct KzuFreeCamera* kzaApplicationGetFreeCamera(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->freeCamera;
}

struct KzuCameraNode* kzaApplicationGetViewCamera(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->viewCamera;
}

struct KzuProjectPatcher* kzaApplicationGetProjectPatcher(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->projectPatcher;
}

struct KzuEngine* kzaApplicationGetEngine(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->engine;
}

struct KzuTransformedScene* kzaApplicationGetTransformedScene(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->transformedScene;
}

void kzaApplicationSetCamera(const struct KzaApplication* application, struct KzuCameraNode* cameraNode)
{
    kzuSceneSetViewCamera(application->currentScene, cameraNode);
}

struct KzsWindow* kzaApplicationGetWindow(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    return application->window;
#else
    return KZ_NULL;
#endif
}

struct KzuBinaryDirectory* kzaApplicationGetBinaryDirectory(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->directory;
}

struct KzuProject* kzaApplicationGetProject(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->project;
}

void kzaApplicationSetDebugObjectsEnabled(struct KzaApplication* application, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(application));
    application->debugObjectsEnabled = enabled;
}

void kzaApplicationSetGridEnabled(struct KzaApplication* application, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(application));
    application->gridEnabled = enabled;
}

kzBool kzaApplicationIsGridEnabled(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->gridEnabled;
}

void kzaApplicationSetFpsInfoEnabled(struct KzaApplication* application, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(application));
    application->infoHudEnabled = enabled;
}

kzBool kzaApplicationIsFpsInfoEnabled(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->infoHudEnabled;
}

void kzaApplicationSetLogVisualizationEnabled(struct KzaApplication* application, kzBool enabled)
{ 
    kzsAssert(kzcIsValidPointer(application));
    application->logVisualizationEnabled = enabled;
}

kzBool kzaApplicationIsLogVisualizationEnabled(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->logVisualizationEnabled;
}

kzsError kzaApplicationEnablePicking(struct KzaApplication* application)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));

    application->pickingEnabled = KZ_TRUE;
    result = kzuPickCreate(application->applicationMemoryManager, kzuProjectGetPropertyManager(application->project), kzaApplicationGetEngine(application), &application->pick);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzaApplicationSetPickCallback(struct KzaApplication* application, KzaPickCallback callback, void* userData)
{
    kzsAssert(kzcIsValidPointer(application));
    application->pickCallback = callback;
    application->pickCallbackUserData = userData;
}

void kzaApplicationSetPickMode(struct KzaApplication* application, enum KzaToolPickMode mode)
{
    kzsAssert(kzcIsValidPointer(application));
    application->toolPickMode = mode;
}

kzsError kzaApplicationPick(const struct KzaApplication* application, kzUint x, kzUint y,  struct KzuObjectNode** out_node, kzUint* out_cluster)
{
    kzsError result;
    
    kzsAssert(kzcIsValidPointer(application));
    
    result = kzuPick(application->pick, kzaApplicationGetEngine(application), kzuProjectGetRootSource(kzaApplicationGetProject(application)),
                     application->transformedScene, kzuSceneGetViewCamera(kzaApplicationGetScene(application)), x, y, out_node, out_cluster);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzaApplicationEnableTransformGizmo(struct KzaApplication* application)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));

    application->transformGizmoEnabled = KZ_TRUE;
    result = kzuTransformGizmoCreate(application->applicationMemoryManager, application->propertyManager, application->messageDispatcher, "TransformGizmo.kzb", &application->transformGizmo);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzaApplicationSetTransformGizmoCallback(const struct KzaApplication* application, KzaTransformGizmoCallback callback, void* userData)
{
    kzsAssert(kzcIsValidPointer(application));

    if (application->transformGizmo != KZ_NULL)
    {
        kzuTransformGizmoSetCallback(application->transformGizmo, callback, userData); /*lint !e632 Assignment to strong type 'KzuTransformGizmoCallback' */
    }
}

kzsError kzaApplicationSetTransformGizmoTarget(const struct KzaApplication* application, kzString objectPath)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));
    if (application->transformGizmo != KZ_NULL)
    {
        result = kzuTransformGizmoSetTarget(application->transformGizmo, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzaApplicationSetTransformGizmoMode(const struct KzaApplication* application, enum KzuTransformGizmoMode mode)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));
    if (application->transformGizmo != KZ_NULL)
    {
        result = kzuTransformGizmoSetMode(application->transformGizmo, mode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzBool kzaApplicationIsTransformGizmoGridSnapEnabled(const struct KzaApplication* application)
{
    kzBool enabled = KZ_FALSE;
    struct KzuTransformGizmo* gizmo;

    kzsAssert(kzcIsValidPointer(application));

    gizmo = application->transformGizmo;
    if(gizmo != KZ_NULL)
    {
        enabled = kzuTransformGizmoIsGridSnapEnabled(gizmo);
    }

    return enabled;
}

void kzaApplicationSetTransformGizmoGridSnapEnabled(const struct KzaApplication* application, kzBool enabled)
{
    struct KzuTransformGizmo* gizmo;

    kzsAssert(kzcIsValidPointer(application));

    gizmo = application->transformGizmo;
    if(gizmo != KZ_NULL)
    {
        kzuTransformGizmoSetGridSnapEnabled(gizmo, enabled);
    }
}

kzFloat kzaApplicationGetTransformGizmoGridSize(const struct KzaApplication* application)
{
    kzFloat gridSize = 0.0f;
    struct KzuTransformGizmo* gizmo;

    kzsAssert(kzcIsValidPointer(application));

    gizmo = application->transformGizmo;
    if(gizmo != KZ_NULL)
    {
        gridSize = kzuTransformGizmoGetGridSize(gizmo);
    }

    return gridSize;
}

void kzaApplicationSetTransformGizmoGridSize(const struct KzaApplication* application, kzFloat size)
{
    struct KzuTransformGizmo* gizmo;

    kzsAssert(kzcIsValidPointer(application));

    gizmo = application->transformGizmo;
    if(gizmo != KZ_NULL)
    {
        kzuTransformGizmoSetGridSize(gizmo, size);
    }
}

enum KzuTransformGizmoGridSnapMode kzaApplicationGetTransformGizmoGridSnapMode(const struct KzaApplication* application)
{
    enum KzuTransformGizmoGridSnapMode gridSnapMode = KZU_TRANSFORM_GIZMO_GRID_SNAP_NONE;
    struct KzuTransformGizmo* gizmo;

    kzsAssert(kzcIsValidPointer(application));

    gizmo = application->transformGizmo;
    if(gizmo != KZ_NULL)
    {
        gridSnapMode = kzuTransformGizmoGetGridSnapMode(gizmo);
    }

    return gridSnapMode;
}

void kzaApplicationSetTransformGizmoGridSnapMode(const struct KzaApplication* application,
                                                 enum KzuTransformGizmoGridSnapMode mode)
{
    struct KzuTransformGizmo* gizmo;

    kzsAssert(kzcIsValidPointer(application));

    gizmo = application->transformGizmo;
    if(gizmo != KZ_NULL)
    {
        kzuTransformGizmoSetGridSnapMode(gizmo, mode);
    }
}

void kzaApplicationSetBoundingBoxVisualizationEnabled(struct KzaApplication* application, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(application));
    application->boundingBoxEnabled = enabled;
}

kzBool kzaApplicationIsBoundingBoxVisualizationEnabled(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->boundingBoxEnabled;
}

kzBool kzaApplicationIsDebugObjectsEnabled(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->debugObjectsEnabled;
}

void kzaApplicationSetAllocatedLayoutBoundingBoxVisualizationEnabled(struct KzaApplication* application, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(application));
    application->layoutAllocatedVisualization = enabled;
}

kzBool kzaApplicationIsAllocatedLayouBoundingBoxVisualizationEnabled(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->layoutAllocatedVisualization;
}

void kzaApplicationSetActualLayoutBoundingBoxVisualizationEnabled(struct KzaApplication* application, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(application));
    application->layoutActualVisualization = enabled;
}

kzBool kzaApplicationIsActualLayouBoundingBoxVisualizationEnabled(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->layoutActualVisualization;
}

kzBool kzaApplicationIsActualLayoutVisualizationReacursive(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->isActualLayoutVisualizationRecursive;
}

void kzaApplicationSetActualLayoutVisualizationRecursive(struct KzaApplication* application, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(application));
    application->isActualLayoutVisualizationRecursive = enabled;
}

kzsError kzaApplicationSetFreeCameraEnabled(struct KzaApplication* application, kzBool enabled)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));

    application->freeCameraEnabled = enabled;

    if (application->currentScene != KZ_NULL)
    {
        result = removeFreeCamera_internal(application);
        kzsErrorForward(result);

        result = addFreeCamera_internal(application, KZ_TRUE);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzBool kzaApplicationIsFreeCameraEnabled(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));

    return application->freeCameraEnabled;
}


void kzaApplicationSetFreeCameraCallback(const struct KzaApplication* application, KzuFreeCameraUpdateStateFunction updateStateFunction, void* userData)
{
    kzsAssert(kzcIsValidPointer(application));

    if (application->freeCamera != KZ_NULL)
    {
        kzuFreeCameraSetUpdateStateFunction(application->freeCamera, updateStateFunction, userData);
    }
}

/** Adds the free camera to the scene. If modifyFreeCamera is KZ_FALSE, the transformation of the free camera is not reset. */
static kzsError addFreeCamera_internal(struct KzaApplication* application, kzBool modifyFreeCamera)
{
    kzsError result;
    struct KzuScene* scene;
    struct KzuCameraNode* viewCameraNode;

    kzsAssert(kzcIsValidPointer(application));

    scene = application->currentScene;
    viewCameraNode = kzuSceneGetViewCamera(scene);

    /* Initialize free camera. */
    if (application->freeCameraEnabled || viewCameraNode == KZ_NULL)
    {
        struct KzuFreeCamera* freeCamera = application->freeCamera;

        if (viewCameraNode != KZ_NULL)
        {
            struct KzuObjectNode* viewCameraObjectNode = kzuCameraNodeToObjectNode(viewCameraNode);
            struct KzuObjectNode* parentObjectNode;
            struct KzcMatrix4x4 parentWorldTransformation = KZC_MATRIX4X4_IDENTITY;
            kzBool parentFound;
            
            parentObjectNode = kzuObjectNodeGetParent(viewCameraObjectNode);
            kzsAssert(parentObjectNode != KZ_NULL);

            parentFound = kzuObjectNodeCalculateRelativeTransformation(kzuSceneGetRootNode(scene), parentObjectNode, &parentWorldTransformation);
            kzsAssert(parentFound);

            result = kzuFreeCameraInitializeFromCameraNode(freeCamera, scene, viewCameraNode, modifyFreeCamera, &parentWorldTransformation);
            kzsErrorForward(result);
        }
        else
        {
            result = kzuFreeCameraInitializeDefault(freeCamera);
            kzsErrorForward(result);
        }

        /* Add free camera to scene */
        {
            struct KzuCameraNode* freeCameraNode = kzuFreeCameraGetCameraNode(freeCamera);
            struct KzuObjectNode* freeCameraObjectNode = kzuCameraNodeToObjectNode(freeCameraNode);

            result = kzuSceneAddObject(scene, freeCameraObjectNode);
            kzsErrorForward(result);

            kzuSceneSetViewCamera(scene, freeCameraNode);
        }

        application->freeCameraInUse = KZ_TRUE;
    }

    kzsSuccess();
}

static kzsError removeFreeCamera_internal(struct KzaApplication* application)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));

    if (application->freeCameraInUse)
    {
        struct KzuCameraNode* freeCameraNode = kzuFreeCameraGetCameraNode(application->freeCamera);

        result = kzuSceneRemoveObject(application->currentScene, kzuCameraNodeToObjectNode(freeCameraNode));
        kzsErrorForward(result);

        kzuSceneSetViewCamera(application->currentScene, application->viewCamera);

        application->freeCameraInUse = KZ_FALSE;
    }

    kzsSuccess();
}

void kzaApplicationSetDefaultKeyHandlingEnabled(struct KzaApplication* application, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(application));

    application->defaultKeyHandlingEnabled = enabled;
}


void kzaApplicationSetAnimationPlayerTime(struct KzaApplication* application, kzInt millisecondsFromStart)
{
    kzsAssert(kzcIsValidPointer(application));

    if (kzuSceneIsValid(application->currentScene))
    { 
        struct KzuAnimationPlayer* animationPlayer = kzuSceneGetAnimationPlayer(application->currentScene);
        if (animationPlayer != KZ_NULL)
        {
            kzFloat floatTime = millisecondsFromStart * 0.001f;
            kzuAnimationPlayerSetTime(animationPlayer, floatTime);
            application->isDeltaTimeSkippedForFrame = KZ_TRUE;
        }
    }
}

kzsError kzaApplicationReloadProject(struct KzaApplication* application, kzUint numBytes, const kzByte* bytes)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));

    if (application->reloadProjectHandler != KZ_NULL)
    {
        result = application->reloadProjectHandler(application, numBytes, bytes);
        kzsErrorForward(result);
    }
    else
    {
        if (application->currentScene != KZ_NULL)
        {
            result = kzaApplicationSetScene(application, KZ_NULL);
            kzsErrorForward(result);
        }

        /* Clear the project. */
        result = kzuProjectClear(application->project);
        kzsErrorForward(result);

        /* This will clear uniform cache. */
        result = kzuRendererReset(kzuEngineGetRenderer(application->engine));
        kzsErrorForward(result);

        /* Patch the project with the binary data. */
        result = kzuProjectLoaderLoadProject(application->project, numBytes, bytes);
        kzsErrorForward(result);

        result = kzaApplicationCompileLuaScripts(application);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzaApplicationPatchProject(struct KzaApplication* application, kzUint numBytes, const kzByte* bytes, kzBool* out_patchSuccess)
{
    kzsError result;
    struct KzuProjectPatcher* projectPatcher;
    struct KzuProject* project;
    kzBool patchSuccess;

    kzsAssert(kzcIsValidPointer(application));

    if (application->patchProjectHandler != KZ_NULL)
    {
        result = application->patchProjectHandler(application, numBytes, bytes, &patchSuccess);
        kzsErrorForward(result);
    }
    else
    {
        projectPatcher = application->projectPatcher;
        project = application->project;

        /* Patch the project with the binary data. */
        result = kzuProjectPatcherPatchProject(projectPatcher, project, numBytes, bytes, &patchSuccess);
        kzsErrorForward(result);

        /* This will clear uniform cache. */
        result = kzuRendererReset(kzuEngineGetRenderer(application->engine));
        kzsErrorForward(result);
    }

    *out_patchSuccess = patchSuccess;
    kzsSuccess();
}

kzsError kzaApplicationPrintRendererInfo(const struct KzaApplication* application)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(application));

    result = kzcRendererPrintInfo(kzuRendererGetCoreRenderer(kzuEngineGetRenderer(application->engine)));
    kzsErrorForward(result);

    kzsSuccess();
}

void kzaApplicationSetUserInterfaceEnabled(struct KzaApplication* application, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(application));
    application->userInterfaceEnabled = enabled;
}

kzBool kzaApplicationGetUserInterfaceEnabled(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->userInterfaceEnabled;
}

struct KzaTransition* kzaApplicationGetTransition(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->transition;
}

struct KzuLua* kzaApplicationGetLua(const struct KzaApplication* application)
{
    kzsAssert(kzcIsValidPointer(application));
    return application->lua;
}

kzsError kzaApplicationCompileLuaScripts(const struct KzaApplication* application)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(application));

    /* Load all script files. */
    {
        struct KzcDynamicArray* files;
        struct KzcDynamicArrayIterator it;

        result = kzcDynamicArrayCreate(application->applicationMemoryManager, &files);
        kzsErrorForward(result);
        result = kzuBinaryFolderInfoGetAllFiles(kzuBinaryDirectoryGetRootFolder(application->directory), files);
        kzsErrorForward(result);

        it = kzcDynamicArrayGetIterator(files);
        while(kzcDynamicArrayIterate(it))
        {
            struct KzuBinaryFileInfo* fileInfo = (struct KzuBinaryFileInfo*)kzcDynamicArrayIteratorGetValue(it);
            if(kzuBinaryFileInfoGetType(fileInfo) == KZU_BINARY_FILE_TYPE_SCRIPT)
            {
                struct KzuScript* script; /* The script is not used, only preloaded. */
                kzString path = kzuBinaryFileInfoGetPath(fileInfo);
                result = kzuProjectLoaderLoadScript(application->project, path, &script);
                kzsErrorForward(result);
            }
        }

        result = kzcDynamicArrayDelete(files);
        kzsErrorForward(result);
    }

    /* Compile script files. */
    {
        kzUint count = 0;
        struct KzuProjectObjectIterator it;

        it = kzuProjectGetObjectsOfType(application->project, KZU_PROJECT_OBJECT_TYPE_SCRIPT);
        while(kzuProjectObjectIterate(it))
        {
            struct KzuScript* script = kzuProjectObjectIteratorGetValue(it);
            result = kzaApplicationCompileLuaScript(application, script);
            kzsErrorForward(result);
            count++;
        }

        if(count > 0)
        {
            result = kzcLog(kzcMemoryGetManager(application->project), KZS_LOG_LEVEL_INFO, "Lua scripts: %d", count);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzaApplicationCompileLuaScript(const struct KzaApplication* application, const struct KzuScript* script)
{
    kzsError result;
    enum KzuScriptDataType dataType;
    kzsAssert(kzuScriptIsValid(script));

    dataType = kzuScriptGetDataType(script);
    switch(dataType)
    {
        case KZU_SCRIPT_DATA_TYPE_SOURCE:
        {
            kzString source;
            result = kzuScriptGetSource(script, &source);
            kzsErrorForward(result);
#undef ENABLE_LUA
#ifdef ENABLE_LUA
            result = kzuLuaLoadFromString(application->lua, source);
            kzsErrorForward(result);
#endif
            break;
        }

        case KZU_SCRIPT_DATA_TYPE_BINARY:
        {
            void* data;
            kzUint dataLength;
            result = kzuScriptGetBinary(script, &data, &dataLength);
            kzsErrorForward(result);
#undef ENABLE_LUA
#ifdef ENABLE_LUA
            /* TODO: Change name to project file name. */
            result = kzuLuaLoadFromMemory(application->lua, data, dataLength, "Temp");
            kzsErrorForward(result);
#else
            KZ_UNUSED_PARAMETER(application);
#endif
            break;
        }
    }

    kzsSuccess();
}

struct KzcFont* kzaApplicationGetDefaultFont(const struct KzaApplication* application)
{
    return kzuHudGetFont(application->hud);
}

struct KzaSystemProperties* kzaApplicationGetSystemProperties(const struct KzaApplication* application)
{
    return &application->systemProperties;
}
