/**
* \file
* Transformed scene object. Transformed scene is extracted from a scene object.
* After extracting it holds all the data needed to perform updates and to render the scene.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TRANSFORMED_SCENE_H
#define KZU_TRANSFORMED_SCENE_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzuScene;
struct KzuCameraNode;
struct KzuObjectSource;
struct KzuObjectSourceCache;
struct KzuObjectSourceRuntimeData;
struct KzcMemoryManager;


/**
* \struct KzuTransformedScene
* Structure to hold transformed scene state.
*/
struct KzuTransformedScene;


/** Create a new transformed scene object. */
kzsError kzuTransformedSceneCreate(const struct KzcMemoryManager* memoryManager, struct KzuTransformedScene** out_scene);
/** Delete a transformed scene object. */
kzsError kzuTransformedSceneDelete(struct KzuTransformedScene* transformedScene);

/** Generates a transformed scene from a scene. */
kzsError kzuTransformedSceneExtract(struct KzuTransformedScene* transformedScene, struct KzuScene* scene);

/** Applies pre-rendering properties for transformed scene such as face to camera. */
void kzuTransformedSceneApplyPreRenderingProperties(const struct KzuTransformedScene* transformedScene);

/** Get the scene that was used to generate the transformed scene. */
struct KzuScene* kzuTransformedSceneGetScene(const struct KzuTransformedScene* transformedScene);
/** Get the view camera. */
struct KzuCameraNode* kzuTransformedSceneGetViewCamera(const struct KzuTransformedScene* transformedScene);
/** Get the transformed object array. */
struct KzcDynamicArray* kzuTransformedSceneGetObjects(const struct KzuTransformedScene* transformedScene);
/** Get the root object source. */
struct KzuObjectSource* kzuTransformedSceneGetRootObjectSource(const struct KzuTransformedScene* transformedScene);

/** Get the object source runtime data. */
struct KzuObjectSourceRuntimeData* kzuTransformedSceneGetObjectSourceRuntimeData(const struct KzuTransformedScene* transformedScene);


#endif
