/**
* \file
* Free camera utility. A utility for freely moving camera with pointing and key devices.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_FREE_CAMERA_H
#define KZU_FREE_CAMERA_H 1


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>


/** Free camera action types. */
enum KzuFreeCameraActionType
{
    KZU_FREE_CAMERA_ACTION_FORWARD,         /**< Action forward. */
    KZU_FREE_CAMERA_ACTION_BACKWARD,        /**< Action backward. */
    KZU_FREE_CAMERA_ACTION_LEFT,            /**< Action left. */
    KZU_FREE_CAMERA_ACTION_RIGHT,           /**< Action right. */
    KZU_FREE_CAMERA_ACTION_SPEED_UP,        /**< Action speed up. */
    KZU_FREE_CAMERA_ACTION_SPEED_DOWN,      /**< Action speed down. */
    KZU_FREE_CAMERA_ACTION_DOUBLE_SPEED,    /**< Action double speed. */
    KZU_FREE_CAMERA_ACTION_ROTATING,        /**< Action rotating on. */
    KZU_FREE_CAMERA_ACTION_LIFTING,         /**< Action lifting on. */
    KZU_FREE_CAMERA_ACTION_ROLLING,         /**< Action rolling on. */
    KZU_FREE_CAMERA_ACTION_FIELD_OF_VIEW_UP, /**< Action field of view up. */
    KZU_FREE_CAMERA_ACTION_FIELD_OF_VIEW_DOWN, /**< Action field of view down */
    KZU_FREE_CAMERA_ACTION_ROLLING_LEFT,    /**< Action rolling left. */
    KZU_FREE_CAMERA_ACTION_ROLLING_RIGHT,   /**< Action rolling right. */
    KZU_FREE_CAMERA_ACTION_LIFTING_SIDE,   /**< Action lifting to side direction. */
    KZU_FREE_CAMERA_ACTION_LIFTING_DEPTH,   /**< Action lifting to depth direction. */
    KZU_FREE_CAMERA_ACTION_STRAIGHTEN,       /**< Straightens the free camera to look horizontally. */
    KZU_FREE_CAMERA_ACTION_ROTATE_X_CLOCKWISE, /**< Rotate X clockwise. */
    KZU_FREE_CAMERA_ACTION_ROTATE_X_COUNTERCLOCKWISE, /**< Rotate X counterclockwise. */
    KZU_FREE_CAMERA_ACTION_ROTATE_Y_CLOCKWISE, /**< Rotate Y clockwise. */
    KZU_FREE_CAMERA_ACTION_ROTATE_Y_COUNTERCLOCKWISE, /**< Rotate Y counterclockwise. */

    KZU_FREE_CAMERA_ACTION_TOGGLE_FREEZE, /**< Toggles free camera freeze on/off. */
    KZU_FREE_CAMERA_ACTION_UPDATE_TRANSFORMATION_TO_CAMERA, /**< Update free camera transformation to original camera. */

    KZU_FREE_CAMERA_ACTION_COUNT            /**< Number of actions for free camera. */
};


/* Forward declarations */
struct KzuCameraNode;
struct KzuCamera;
struct KzuProject;
struct KzuScene;
struct KzcMemoryManager;
struct KzuPropertyManager;
struct KzuMessageDispatcher;
struct KzcMatrix4x4;
struct KzcVector3;


/** Free camera callback prototype for updating camera state from free camera. */
typedef kzsError (*KzuFreeCameraUpdateStateFunction)(struct KzcMatrix4x4* transformation, void* userData);


/**
* \struct KzuFreeCamera
* Structure for free camera object, that is, freely controllable camera utility.
*/
struct KzuFreeCamera;


/** Creates a new free camera object with a default camera in origin. */
kzsError kzuFreeCameraCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                             struct KzuFreeCamera** out_freeCamera);

/** Deletes free camera object. */
kzsError kzuFreeCameraDelete(struct KzuFreeCamera* freeCamera);

/** Initializes free camera with default camera settings at origin. */
kzsError kzuFreeCameraInitializeDefault(struct KzuFreeCamera* freeCamera);

/** Initializes free camera using the given camera and scene. */
kzsError kzuFreeCameraInitializeFromCameraNode(struct KzuFreeCamera* freeCamera, const struct KzuScene* scene,
                                               struct KzuCameraNode* cameraNode, kzBool setTransformation,
                                               const struct KzcMatrix4x4* parentWorldTransformation);
/** Sets position for free camera. */
void kzuFreeCameraSetPosition(struct KzuFreeCamera* freeCamera, const struct KzcVector3* position);
/** Returns world matrix from free camera position and orientation. */
void kzuFreeCameraInitializeMatrix(const struct KzuFreeCamera* freeCamera, struct KzcMatrix4x4* out_worldMatrix);


/** Returns camera node from free camera. */
struct KzuCameraNode* kzuFreeCameraGetCameraNode(const struct KzuFreeCamera* freeCamera);


/** Updates free camera, delta time in milliseconds. */
kzsError kzuFreeCameraUpdate(struct KzuFreeCamera* freeCamera, kzUint deltaTime);


/** Sets free camera freezed / unfreezed. */
void kzuFreeCameraSetFreezed(struct KzuFreeCamera* freeCamera, kzBool freezed);
/** Checks if free camera is freezed. */
kzBool kzuFreeCameraIsFreezed(const struct KzuFreeCamera* freeCamera);


/** Maps new action to key device key. Same key can have several actions. */
kzsError kzuFreeCameraMapActionToKey(struct KzuFreeCamera* freeCamera, enum KzsInputKey key, enum KzuFreeCameraActionType actionType);
/** Maps new action to pointer device button. Same pointer device button can be mapped with several actions. */
kzsError kzuFreeCameraMapActionToPointerButton(struct KzuFreeCamera* freeCamera, kzU32 pointerButton, enum KzuFreeCameraActionType actionType);


/** Handles free camera key events. */
void kzuFreeCameraHandleKeyEvents(struct KzuFreeCamera* freeCamera, enum KzsInputKey keyCode, enum KzsInputKeyDeviceState keyState);
/** Handles free camera mouse events. */
void kzuFreeCameraHandlePointerEvents(struct KzuFreeCamera* freeCamera, kzU32 buttons, enum KzsInputPointingDeviceState state, kzInt mouseX, kzInt mouseY);
/** Sets free camera action event. */
void kzuFreeCameraSetActionEvent(struct KzuFreeCamera* freeCamera, enum KzuFreeCameraActionType actionType);


/** Sets a callback for free camera. Pass KZ_NULL to disable. */
void kzuFreeCameraSetUpdateStateFunction(struct KzuFreeCamera* freeCamera, KzuFreeCameraUpdateStateFunction callback, void* userData);


#endif
