/**
* \file
* Structure for engine. Contains pipeline specific assets (current scene, renderer), timer, etc.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_ENGINE_H
#define KZU_ENGINE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Amount of quick memory used by engine. */
#define KZU_ENGINE_DEFAULT_QUICK_MEMORY_SIZE    (1 << 21)


/** Engine loading time measurement constants. */
enum KzuEngineLoadingMeasurement
{
    KZU_ENGINE_LOADING_MEASUREMENT_RENDERER, /**< Renderer loading time. */
    KZU_ENGINE_LOADING_MEASUREMENT_RESOURCE_MANAGER /**< Resource manager loading time. */
};


/* Forward declaration structures. */
struct KzuScene;
struct KzuTransformedObjectNode;
struct KzuObjectNode;
struct KzuObjectSource;
struct KzuMaterial;
struct KzuRenderer;
struct KzuFixedPropertyTypes;
struct KzuPropertyCollection;
struct KzuPropertyGroup;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcDynamicArray;
struct KzcStopwatch;
struct KzsWindow;
struct KzuTransformedScene;
struct KzuPropertyManager;


/**
* \struct KzuEngine
* Structure for graphics engine core functionality. Handles the state machine, pipeline and storage specific functionality.
*/
struct KzuEngine;


/** Create a new engine. */
kzsError kzuEngineCreate(const struct KzcMemoryManager* memoryManager, kzUint quickMemorySize, struct KzuPropertyManager* propertyManager, struct KzuEngine** out_engine);
/** Deletes an engine. */
kzsError kzuEngineDelete(struct KzuEngine* engine);
/** Creates lightweight version of engine, which does only necessary initialization to operate engine (no default materials created). */
kzsError kzuEngineCreateLightweight(const struct KzcMemoryManager* memoryManager, kzUint quickMemorySize, struct KzuPropertyManager* propertyManager, struct KzuEngine** out_engine);


/** Sets active window for engine. */
void kzuEngineSetActiveWindow(struct KzuEngine* engine, struct KzsWindow* window);
/** Gets active window that engine has. */
struct KzsWindow* kzuEngineGetActiveWindow(const struct KzuEngine* engine);


/** Applies global properties for engine. */
kzsError kzuEngineApplyGlobalProperties(const struct KzuEngine* engine, const struct KzuPropertyManager* propertyManager, const void* object);
/** Start engine update, property groups reset. */
kzsError kzuEngineStartUpdate(struct KzuEngine* engine);
/** Starts a frame for engine rendering. */
kzsError kzuEngineBeginFrame(struct KzuEngine* engine);
/** Ends a frame for engine rendering . */
kzsError kzuEngineEndFrame(struct KzuEngine* engine);
/** Generates a transformed scene from a scene by using object sources from an engine. */
kzsError kzuEngineExtractScene(const struct KzuEngine* engine, struct KzuScene* scene, struct KzuTransformedScene* transformedScene);
/** Render a scene. */
kzsError kzuEngineRender(const struct KzuEngine* engine, const struct KzuTransformedScene* transformedScene);


/** Updates UI for engine. That is, UI interaction (picking) and logic. */
kzsError kzuEngineUpdateUi(const struct KzuEngine* engine, const struct KzuTransformedScene* transformedScene, kzUint deltaTime);


/** Gets timer from engine. */
struct KzcStopwatch* kzuEngineGetTimer(struct KzuEngine* engine);
/** Gets delta time from engine. */
kzUint kzuEngineGetDeltaTime(const struct KzuEngine* engine);
/** Gets frames per second from engine (FPS), measuring the overall frames per second. */
kzUint kzuEngineGetFramesPerSecond(const struct KzuEngine* engine);
/** Sets GPU performance measurement enabled / disabled for engine. */
void kzuEngineSetGPUPerformanceMeasurementEnabled(struct KzuEngine* engine, kzBool enabled);
/** Returns if GPU performance measurement is enabled. */
kzBool kzuEngineIsGPUPerformanceMeasurementEnabled(const struct KzuEngine* engine);
/** Returns if this frame had GPU enabled / disabled. */
kzBool kzuEngineIsCurrentFrameGPUEnabled(const struct KzuEngine* engine);
/** Gets GPU performance percentage from total performance, e.g. 0.5 would be 50% of time was consumed for GPU. */
kzFloat kzuEngineGetGPUPerformancePercentage(const struct KzuEngine* engine);
/** Sets GPU calls enabled / disabled. */
void kzuEngineSetGPUCallsEnabled(struct KzuEngine* engine, kzBool enabled);


/** Gets a resource manager from an engine. Most of the resources should be created to project, so this function should not be called outside engine functionality. */
struct KzcResourceManager* kzuEngineGetResourceManager(const struct KzuEngine* engine);
/** Returns user renderer from engine. */
struct KzuRenderer* kzuEngineGetRenderer(const struct KzuEngine* engine);


/** Gets loading time measurement from engine. */
kzUint kzuEngineGetLoadingTime(const struct KzuEngine* engine, enum KzuEngineLoadingMeasurement measurement);


/** Draws visualization grid. */
kzsError kzuEngineDrawVisualizationGrid(const struct KzuEngine* engine, const struct KzuTransformedScene* transformedScene,
                                        kzFloat spacing);
/** Draws debug objects (cameras & lights). */
kzsError kzuEngineDrawDebugObjects(const struct KzuEngine* engine, const struct KzuTransformedScene* transformedScene);

/** Gets engine message queue. */
struct KzuEngineMessageQueue* kzuEngineGetMessageQueue(const struct KzuEngine* engine);


#endif
