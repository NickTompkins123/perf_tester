/**
* \file
* Structure for engine. Contains pipeline specific assets (current scene, renderer), timer, etc.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_engine.h"

#include "kzu_engine_message_queue.h"

#include <user/filter/kzu_object_source.h>
#include <user/material/kzu_material.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_light.h>
#include <user/renderer/kzu_renderer_util.h>
#include <user/renderer/kzu_render_pass.h>
#include <user/renderer/kzu_renderer.h>
#include <user/renderer/kzu_composer.h>
#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_property_group.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property_query.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/ui/kzu_ui_screen.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_hash_set.h>
#include <core/util/string/kzc_string.h>
#include <core/util/data_log/kzc_stopwatch.h>

#include <system/debug/kzs_log.h>


/** Structure for engine performance measurement, i.e. fps / GPU fps. */
struct KzuEnginePerformance
{
    kzUint fpsMilliseconds; /**< FPS timer in milliseconds, looping between [0, 1000[. */
    kzUint framesPerSecondCounter; /**< Helper counter for FPS. */
    kzUint framesPerSecond; /**< Current frames per second. */

    kzBool measureGPUFps; /**< Flag for determining whether GPU FPS measuring should be on / off. */
    kzBool gpuFrame; /**< Flipflop for marking if current frame to be rendered is GPU enabled frame - used in GPU fps measurement. */
    kzUint gpuFrameTimer; /**< Timer for GPU frames. */
    kzUint frameTimer; /**< Timer for total frames. */
    kzUint frameTimerStart; /**< Timer for total frames. */
    kzUint normalFrameTimerStart; /**< Timer for total frames. */
    kzFloat gpuPercentage; /**< GPU percentage from total frame duration. */
};

/** Structure for engine loading time measurement. */
struct KzuEngineLoading
{
    kzUint resourceManagerCreationTime; /**< Resource manager creation time. */
    kzUint rendererCreationTime; /**< Renderer creation time. */
};

/** Structure for graphics engine core functionality. Handles the state machine, pipeline and storage specific functionality. */
struct KzuEngine
{
    struct KzcResourceManager* resourceManager; /**< Resource manager for engine. */
    struct KzuRenderer* renderer; /**< Render manager. */

    struct KzsWindow* activeWindow; /**< Engine's active window. */

    struct KzcStopwatch timer; /**< Timer for engine. */
    kzUint deltaTime; /**< Delta time for engine. */
    struct KzuEnginePerformance performance; /**< Engine performance specific values. */
    struct KzuEngineLoading loadingMeasurement; /**< Engine loading time measurement values. */

    struct KzuEngineMessageQueue* messageQueue; /**< Message queue for delivering commands from ui to engine. */
    kzBool enableGPUCalls;
};


/** Internal function for updating scene (timers). */
void kzuEngineUpdate_internal(struct KzuEngine* engine);


static kzsError kzuEngineCreate_internal(const struct KzcMemoryManager* memoryManager, kzBool lightWeight, kzUint quickMemorySize, struct KzuPropertyManager* propertyManager, struct KzuEngine** out_engine)
{
    kzsError result;
    struct KzuEngine* engine;
    kzUint measurementStart = kzsTimeGetCurrentTimestamp();

    /* Allocate memory for engine. */
    result = kzcMemoryAllocVariable(memoryManager, engine, "Engine");
    kzsErrorForward(result);

    /* Creates a new resource manager. */
    result = kzcResourceManagerCreate(memoryManager, &engine->resourceManager);
    kzsErrorForward(result);

    engine->loadingMeasurement.resourceManagerCreationTime = (kzsTimeGetCurrentTimestamp() - measurementStart);

    /* Creates a render manager for engine (user renderer). */
    result = kzuRendererCreate(memoryManager, quickMemorySize, propertyManager, engine->resourceManager,
        !lightWeight, &engine->renderer);
    kzsErrorForward(result);

    engine->loadingMeasurement.rendererCreationTime = (kzsTimeGetCurrentTimestamp() - measurementStart - engine->loadingMeasurement.resourceManagerCreationTime);

    result = kzuEngineMessageQueueCreate(memoryManager, &engine->messageQueue);
    kzsErrorForward(result);

    engine->timer = kzcStopwatchInitialize();
    kzcStopwatchStart(&engine->timer);
    engine->deltaTime = 0;

    engine->performance.fpsMilliseconds = 0;
    engine->performance.framesPerSecond = 0;
    engine->performance.framesPerSecondCounter = 0;
    engine->performance.measureGPUFps = KZ_FALSE;
    engine->performance.gpuFrame = KZ_TRUE;
    engine->performance.gpuFrameTimer = 0;
    engine->performance.gpuPercentage = 0.0f;
    engine->performance.normalFrameTimerStart = 0;
    engine->performance.frameTimer = 0;
    engine->activeWindow = KZ_NULL;
    engine->enableGPUCalls = KZ_TRUE;

    *out_engine = engine;
    kzsSuccess();
}

kzsError kzuEngineCreate(const struct KzcMemoryManager* memoryManager, kzUint quickMemorySize, struct KzuPropertyManager* propertyManager, struct KzuEngine** out_engine)
{
    kzsError result;
    struct KzuEngine* engine;

    result = kzuEngineCreate_internal(memoryManager, KZ_FALSE, quickMemorySize, propertyManager, &engine);
    kzsErrorForward(result);

    *out_engine = engine;
    kzsSuccess();
}

kzsError kzuEngineCreateLightweight(const struct KzcMemoryManager* memoryManager, kzUint quickMemorySize, struct KzuPropertyManager* propertyManager, struct KzuEngine** out_engine)
{
    kzsError result;
    struct KzuEngine* engine;

    result = kzuEngineCreate_internal(memoryManager, KZ_TRUE, quickMemorySize, propertyManager, &engine);
    kzsErrorForward(result);

    *out_engine = engine;
    kzsSuccess();
}

kzsError kzuEngineDelete(struct KzuEngine* engine)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(engine));

    result = kzuEngineMessageQueueDelete(engine->messageQueue);
    kzsErrorForward(result);

    result = kzuRendererDelete(engine->renderer);
    kzsErrorForward(result);
    result = kzcResourceManagerDelete(engine->resourceManager);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(engine);
    kzsErrorForward(result);
    
    kzsSuccess();
}

void kzuEngineSetActiveWindow(struct KzuEngine* engine, struct KzsWindow* window)
{
    kzsAssert(kzcIsValidPointer(engine));
    engine->activeWindow = window;
    kzcRendererSetActiveWindow(kzuRendererGetCoreRenderer(kzuEngineGetRenderer(engine)), window);
}

struct KzsWindow* kzuEngineGetActiveWindow(const struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));
    return engine->activeWindow;
}

void kzuEngineUpdate_internal(struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));

    {
        kzUint currentTime = kzcStopwatchGetCurrentTime(&engine->timer);
        engine->deltaTime = kzcStopwatchSampleDeltaTime(&engine->timer);

        if(engine->performance.gpuFrame)
        {
            engine->performance.gpuFrameTimer = currentTime - engine->performance.normalFrameTimerStart;
            engine->performance.frameTimer = currentTime - engine->performance.frameTimerStart;
        }

        if(engine->performance.measureGPUFps && engine->performance.gpuFrame)
        {
            kzUint cpuTime = engine->performance.frameTimer - engine->performance.gpuFrameTimer;
            kzFloat percentageCPU = kzsClampf((kzFloat)cpuTime / (kzFloat)engine->performance.gpuFrameTimer, 0.0f, 1.0f);
            kzFloat percentageGPU = 1.0f - percentageCPU;

            engine->performance.gpuPercentage = percentageGPU;
        }

        /* Update FPS. */
        engine->performance.normalFrameTimerStart = currentTime;
        engine->performance.fpsMilliseconds += engine->deltaTime;
        ++engine->performance.framesPerSecondCounter;
        if(engine->performance.fpsMilliseconds >= 1000)
        {
            engine->performance.fpsMilliseconds -= ((engine->performance.fpsMilliseconds / 1000) * 1000);
            engine->performance.framesPerSecond = engine->performance.framesPerSecondCounter;
            engine->performance.framesPerSecondCounter = 0;
        }

        if(!engine->enableGPUCalls)
        {
            struct KzcRenderer* renderer = kzuRendererGetCoreRenderer(kzuEngineGetRenderer(engine));
            kzcRendererSetGPUCallsEnabled(renderer, KZ_FALSE);
        }
        else
        {
            struct KzcRenderer* renderer = kzuRendererGetCoreRenderer(kzuEngineGetRenderer(engine));
            kzcRendererSetGPUCallsEnabled(renderer, engine->performance.gpuFrame);
        }

        /* See the GPU frame status. */
        if(engine->performance.measureGPUFps)
        {
            engine->performance.gpuFrame = !engine->performance.gpuFrame;
        }
        else
        {
            engine->performance.gpuFrame = KZ_TRUE;
        }

        if(!engine->performance.gpuFrame)
        {
            engine->performance.frameTimerStart = currentTime;
        }
    }
}

kzsError kzuEngineApplyGlobalProperties(const struct KzuEngine* engine, const struct KzuPropertyManager* propertyManager, const void* object)
{
    struct KzuRenderer* userRenderer = kzuEngineGetRenderer(engine);

    kzuRendererSetViewport(userRenderer,
                           kzuPropertyManagerGetFloatDefault(propertyManager, object, KZU_PROPERTY_TYPE_WINDOW_X),
                           kzuPropertyManagerGetFloatDefault(propertyManager, object, KZU_PROPERTY_TYPE_WINDOW_Y),
                           kzuPropertyManagerGetFloatDefault(propertyManager, object, KZU_PROPERTY_TYPE_WINDOW_WIDTH),
                           kzuPropertyManagerGetFloatDefault(propertyManager, object, KZU_PROPERTY_TYPE_WINDOW_HEIGHT),
                           kzuPropertyManagerGetFloatDefault(propertyManager, object, KZU_PROPERTY_TYPE_WINDOW_ASPECT_RATIO),
                           kzuPropertyManagerGetIntDefault(propertyManager, object, KZU_PROPERTY_TYPE_WINDOW_METRICS_TYPE) == 1);

    kzsSuccess();
}

kzsError kzuEngineStartUpdate(struct KzuEngine* engine)
{
    KZ_UNUSED_PARAMETER(engine);
    kzsSuccess();
}

kzsError kzuEngineBeginFrame(struct KzuEngine* engine)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(engine));
    /* Reset render manager frame. */
    result = kzuRendererResetFrame(engine->renderer);
    kzsErrorForward(result);
    
    kzuEngineUpdate_internal(engine);

    kzsSuccess();
}

kzsError kzuEngineEndFrame(struct KzuEngine* engine)
{
    KZ_UNUSED_PARAMETER(engine);
    kzsSuccess();
}

kzsError kzuEngineExtractScene(const struct KzuEngine* engine, struct KzuScene* scene, struct KzuTransformedScene* transformedScene)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(engine));

    result = kzuTransformedSceneExtract(transformedScene, scene);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuEngineRender(const struct KzuEngine* engine, const struct KzuTransformedScene* transformedScene)
{
    kzsError result;
    struct KzuComposer* composer;
    struct KzuRenderer* renderer;
    struct KzuScene* scene;
    struct KzuPropertyQuery* propertyQuery;

    kzsAssert(kzcIsValidPointer(engine));
    kzsAssert(kzcIsValidPointer(transformedScene));

    if(engine->activeWindow == KZ_NULL)
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Engine may not be rendered as active window is not specified for it!");
    }

    scene = kzuTransformedSceneGetScene(transformedScene);

    renderer = engine->renderer;
    composer = kzuSceneGetComposer(scene);
    propertyQuery = kzuRendererGetPropertyQuery(renderer);

    /* Push scene properties. */
    result = kzuPropertyQueryPushObject(propertyQuery, scene);
    kzsErrorForward(result);

    if (composer != KZ_NULL)
    {
        result = kzuComposerApply(composer, renderer, transformedScene, engine->activeWindow);
        kzsErrorForward(result);
    }

    /* Pop scene properties. */
    result = kzuPropertyQueryPopObject(propertyQuery);
    kzsErrorForward(result);

    result = kzuRendererPrintInfo(renderer);
    kzsErrorForward(result);
    
    kzsSuccess();
}

struct KzcStopwatch* kzuEngineGetTimer(struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));
    return &engine->timer;
}

kzUint kzuEngineGetDeltaTime(const struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));
    return engine->deltaTime;
}

kzUint kzuEngineGetFramesPerSecond(const struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));
    return engine->performance.framesPerSecond;
}

void kzuEngineSetGPUPerformanceMeasurementEnabled(struct KzuEngine* engine, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(engine));
    engine->performance.measureGPUFps = enabled;
}

kzBool kzuEngineIsGPUPerformanceMeasurementEnabled(const struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));
    return engine->performance.measureGPUFps;
}

kzFloat kzuEngineGetGPUPerformancePercentage(const struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));
    return engine->performance.gpuPercentage;
}

void kzuEngineSetGPUCallsEnabled(struct KzuEngine* engine, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(engine));
    engine->enableGPUCalls = enabled;
}

kzBool kzuEngineIsCurrentFrameGPUEnabled(const struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));
    return engine->performance.gpuFrame;
}

kzsError kzuEngineUpdateUi(const struct KzuEngine* engine, const struct KzuTransformedScene* transformedScene, kzUint deltaTime)
{
    kzsError result;
    struct KzuScene* scene;
    struct KzuComposer* composer;

    kzsAssert(kzcIsValidPointer(engine));
    kzsAssert(kzcIsValidPointer(transformedScene));

    scene = kzuTransformedSceneGetScene(transformedScene);
    composer = kzuSceneGetComposer(scene);

    if(composer != KZ_NULL)
    {
        struct KzuUiScreen* screen = kzuSceneGetUiScreen(scene);

        result = kzuUiScreenLogicUpdate(screen, kzuSceneGetRootNode(scene), (kzFloat)deltaTime / 1000.0f, kzuEngineGetMessageQueue(engine));
        kzsErrorForward(result);

        result = kzuUiScreenHandleEvents(screen, kzuSceneGetRootNode(scene), transformedScene, engine->renderer,
                                         engine->activeWindow, kzuEngineGetMessageQueue(engine));
        kzsErrorForward(result);
    }

    kzsSuccess();
}

struct KzcResourceManager* kzuEngineGetResourceManager(const struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));
    return engine->resourceManager;
}

struct KzuRenderer* kzuEngineGetRenderer(const struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));
    return engine->renderer;
}

kzUint kzuEngineGetLoadingTime(const struct KzuEngine* engine, enum KzuEngineLoadingMeasurement measurement)
{
    kzUint time = 0;
    switch(measurement)
    {
        case KZU_ENGINE_LOADING_MEASUREMENT_RENDERER: time = engine->loadingMeasurement.rendererCreationTime; break;
        case KZU_ENGINE_LOADING_MEASUREMENT_RESOURCE_MANAGER: time = engine->loadingMeasurement.resourceManagerCreationTime; break;
        default: kzsAssert(KZ_FALSE);
    }
 
    return time;
}

kzsError kzuEngineDrawVisualizationGrid(const struct KzuEngine* engine, const struct KzuTransformedScene* transformedScene,
                                        kzFloat spacing)
{
    kzsError result;
    struct KzuScene* scene;
    struct KzuComposer* composer;

    kzsAssert(kzcIsValidPointer(engine));
    kzsAssert(kzcIsValidPointer(transformedScene));

    scene = kzuTransformedSceneGetScene(transformedScene);
    composer = kzuSceneGetComposer(scene);

    if(kzuComposerGetChildren(composer) != KZ_NULL && !kzcDynamicArrayIsEmpty(kzuComposerGetChildren(composer)))
    {
        result = kzuRendererDrawWireframeGrid(engine->renderer, spacing);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuEngineDrawDebugObjects(const struct KzuEngine* engine, const struct KzuTransformedScene* transformedScene)
{
    kzsError result;
    struct KzuScene* scene;
    struct KzuComposer* composer;

    kzsAssert(kzcIsValidPointer(engine));

    scene = kzuTransformedSceneGetScene(transformedScene);
    composer = kzuSceneGetComposer(scene);

    if(composer != KZ_NULL && !kzcDynamicArrayIsEmpty(kzuComposerGetChildren(composer)))
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(kzuTransformedSceneGetObjects(transformedScene));

        /* Iterate all scene objects. */
        while(kzcDynamicArrayIterate(it))
        {
            struct KzuTransformedObjectNode* transformedObjectNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
            struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedObjectNode);
            struct KzcMatrix4x4 matrix = kzuTransformedObjectNodeGetMatrix(transformedObjectNode);
            KzuObjectType objectType = kzuObjectNodeGetType(objectNode);

            /* Draw light. */
            if (objectType == KZU_OBJECT_TYPE_LIGHT)
            {
                struct KzuLightNode* lightNode = kzuLightNodeFromObjectNode(objectNode);
                
                result = kzuRendererDrawLight(engine->renderer, &matrix, lightNode);
                kzsErrorForward(result);
            }

            /* Draw camera. */
            else if (objectType == KZU_OBJECT_TYPE_CAMERA)
            {
                struct KzuCameraNode* cameraNode = kzuCameraNodeFromObjectNode(objectNode);
                /** If this object is not actual viewer, visualize camera. */
                if(kzuCameraNodeFromObjectNode(objectNode) != kzuSceneGetViewCamera(scene))
                {
                    result = kzuRendererDrawCamera(engine->renderer, &matrix, cameraNode);
                    kzsErrorForward(result);
                }
            }
            else if(objectType == KZU_OBJECT_TYPE_EMPTY)
            {
                result = kzuRendererDrawEmptyObject(engine->renderer, &matrix);
                kzsErrorForward(result);
            }
        }
    }

    kzsSuccess();
}

struct KzuEngineMessageQueue* kzuEngineGetMessageQueue(const struct KzuEngine* engine)
{
    kzsAssert(kzcIsValidPointer(engine));
    return engine->messageQueue;
}
