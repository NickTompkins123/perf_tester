/**
* \file
* Scene element contains root node of scene graph, composer, camera and set of properties.
* Element that constructs the scene visually.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_SCENE_H
#define KZU_SCENE_H


#include <core/util/collection/kzc_dynamic_array.h>

#include <system/input/kzs_input.h>
#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/** Scene performance measurement constants. */
enum KzuScenePerformanceMeasurement
{
    KZU_SCENE_PERFORMANCE_MEASUREMENT_LAYOUT, /**< Scene measurement for layouting. */
    KZU_SCENE_PERFORMANCE_MEASUREMENT_ARRANGE, /**< Scene measurement for arranging nodes. */
    KZU_SCENE_PERFORMANCE_MEASUREMENT_TRANSFORM, /**< Scene measurement for transforming nodes. */
    KZU_SCENE_PERFORMANCE_MEASUREMENT_EXTRACT, /**< Scene measurement for extracting nodes. */
    KZU_SCENE_PERFORMANCE_MEASUREMENT_CONSTRAINTS, /**< Scene measurement for applying constraints. */
    KZU_SCENE_PERFORMANCE_MEASUREMENT_ANIMATION /**< Scene measurement for applying animation. */
};


/* Forward declaration structures */
struct KzuObjectNode;
struct KzuCameraNode;
struct KzuTransformedObjectNode;
struct KzuComposer;
struct KzuRenderer;
struct KzuProperty;
struct KzuUiScreen;
struct KzcMemoryManager;
struct KzcMatrix4x4;
struct KzuPropertyManager;
struct KzuMessageDispatcher;


/**
 * \struct KzuScene
 * Scene contains composer, view camera, properties and reference to root node of scene graph.
 */
struct KzuScene;


/** Creates a new scene object. */
kzsError kzuSceneCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                        kzString name, struct KzuScene** out_scene);
/** Creates scene with custom root node. */
kzsError kzuSceneCreateWithRoot(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                kzString name, struct KzuObjectNode* rootNode, struct KzuScene** out_scene);
/** Deletes a scene object. */
kzsError kzuSceneDelete(struct KzuScene* scene);

/**
 * Takes ownership of the scene container deletion. After this, when a scene is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuSceneTakeOwnership(const struct KzuScene* scene);
/** Checks if the scene is valid (pointer and the data are non-null). */
kzBool kzuSceneIsValid(const struct KzuScene* scene);
/** Moves the data from source scene to the target scene. */
void kzuSceneTransferData(struct KzuScene* targetScene, struct KzuScene* sourceScene);


/** Applies animations for scene. */
kzsError kzuSceneAnimate(const struct KzuScene* scene, kzUint deltaTime);

/**
 * Extracts scene graph, i.e. creates copies from the instance nodes.
 * Traverses the object nodes one by one and copies to corresponding extracted hierarchy.
 * Also transforms the nodes to world coordinates.
 */
kzsError kzuSceneExtract(struct KzcMemoryManager* memoryManager, const struct KzuScene* scene,
                         struct KzcDynamicArray* extractedObjectList, struct KzuTransformedObjectNode** out_extractedRootNode);


/** Set composer for scene. */
void kzuSceneSetComposer(const struct KzuScene* scene, struct KzuComposer* composer);
/** Gets composer of the scene. */
struct KzuComposer* kzuSceneGetComposer(const struct KzuScene* scene);

/** Returns a scene's root node */
kzsError kzuSceneSetRootNode(const struct KzuScene* scene, struct KzuObjectNode* rootNode);
/** Returns a scene's root node */
struct KzuObjectNode* kzuSceneGetRootNode(const struct KzuScene* scene);
/** Sets view camera for scene. */
void kzuSceneSetViewCamera(const struct KzuScene* scene, struct KzuCameraNode* camera);
/** Sets view camera 2D for scene. */
void kzuSceneSetViewCamera2D(const struct KzuScene* scene, struct KzuCameraNode* camera2D);
/** Returns view camera from scene. */
struct KzuCameraNode* kzuSceneGetViewCamera(const struct KzuScene* scene);
/** Returns view camera 2D from scene. */
struct KzuCameraNode* kzuSceneGetViewCamera2D(const struct KzuScene* scene);


/** Adds object to scene's root. */
kzsError kzuSceneAddObject(const struct KzuScene* scene, struct KzuObjectNode* objectNode);
/** Removes object from scene root. */
kzsError kzuSceneRemoveObject(const struct KzuScene* scene, const struct KzuObjectNode* objectNode);


/** Searches current scene for given node */
kzBool kzuSceneFindObjectNode(const struct KzuScene* scene, const struct KzuObjectNode* objectToFind);

/** Gets the root object source by traversing object source hierarchy upwards from the scene composer render passes. */
struct KzuObjectSource* kzuSceneGetRootObjectSource(const struct KzuScene* scene);

/**
 * Calculates world transformation for single object in scene. If the object exists in scene graph multiple times,
 * the result is one arbitrarily chosen correct transformation.
 * If the object exists in the scene, true is returned and the world transformation is written to out_worldTransformation.
 * Otherwise, false is returned and identity matrix is written to out_worldTransformation.
 */
kzBool kzuSceneCalculateWorldTransformation(const struct KzuScene* scene,
                                            const struct KzuObjectNode* objectNode,
                                            struct KzcMatrix4x4* out_worldTransformation);

/** Gets animation player from scene. */
struct KzuAnimationPlayer* kzuSceneGetAnimationPlayer(const struct KzuScene* scene);

/** Gets user interface screen from scene. */
struct KzuUiScreen* kzuSceneGetUiScreen(const struct KzuScene* scene);
/** Forwards key events to ui attached to scene. */
void kzuSceneHandleKeyEvents(const struct KzuScene* scene, enum KzsInputKey keyCode, enum KzsInputKeyDeviceState keyState);
/** Forwards pointer events to ui attached to scene. buttons is bitmask of pointing device buttons, defined as KZS_POINTING_DEVICE_BUTTON_ *.*/
void kzuSceneHandlePointerEvents(const struct KzuScene* scene, kzU32 buttons, enum KzsInputPointingDeviceState state, kzInt mouseX, kzInt mouseY);

/** Perform extracting on a node. */
kzsError kzuSceneExtractNode(const struct KzuScene* scene, struct KzcMemoryManager* memoryManager, struct KzcDynamicArray* extractedObjectList,
                             struct KzuObjectNode* node, struct KzuTransformedObjectNode** out_transformedNode);

/** Perform extracting and layout measurement on a node. */
kzsError kzuSceneMeasureNode(const struct KzuScene* scene, struct KzcMemoryManager* memoryManager, struct KzcDynamicArray* extractedObjectList,
                             struct KzuObjectNode* node, struct KzuTransformedObjectNode** out_transformedNode);

/** Perform transformation on an extracted node. */
kzsError kzuSceneTransformNode(const struct KzuScene* scene, struct KzuTransformedObjectNode* transformedNode, struct KzuTransformedObjectNode* transformedParentNode);
/** Perform layout arrangement on an extracted node. */
kzsError kzuSceneArrangeNode(const struct KzuScene* scene, struct KzuTransformedObjectNode* transformedNode, struct KzuTransformedObjectNode* transformedParentNode);
/** Applies scene constraints. */
kzsError kzuSceneApplyConstraints(const struct KzuScene* scene, const struct KzcMemoryManager* quickMemoryManager);


/** Gets per-frame performance measurement form scene. */
kzUint kzuSceneGetPerformanceMeasurement(const struct KzuScene* scene, enum KzuScenePerformanceMeasurement measurement);


/** Sets name for the scene. */
kzString kzuSceneGetName(const struct KzuScene* scene);
/** Gets name from the scene. */
kzsError kzuSceneSetName(const struct KzuScene* scene, kzString name);


#endif
