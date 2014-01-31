/**
* \file
* Free camera utility.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_free_camera.h"


#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_quaternion.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>


/* Values that can & should be tuned. */

/** Roll movement speed. */
#define KZU_FREE_CAMERA_ROLL_MOVEMENT_SPEED     0.06f
/** FOV movement speed. */
#define KZU_FREE_CAMERA_FIELD_OF_VIEW_MOVEMENT_SPEED     0.03f
/** Value for lifting speed. */
#define KZU_FREE_CAMERA_LIFTING_SPEED           0.1f
/** Value for rotating speed. */
#define KZU_FREE_CAMERA_ROTATE_SPEED            0.005f
/** A value for defining how hard camera interpolates to target, between [0, 1]. */
#define KZU_FREE_CAMERA_ANGULAR_HARDNESS        0.3f
/** A value for defining how quickly camera speed adjustment accelerates / deaccelerates. */
#define KZU_FREE_CAMERA_SPEED_ACCELERATION_SPEED      3.0f
/** Orientation normalization speed. */
#define KZU_FREE_CAMERA_ORIENTATION_NORMALIZATION_SPEED      0.01f
/** Base movement speed for camera. */
#define KZU_FREE_CAMERA_BASE_MOVEMENT_SPEED     20.0f


struct KzuFreeCameraMouseAction
{
    enum KzuFreeCameraActionType actionType;        /**< Type of action. */
    kzU32 mouseButton;                              /**< Button that this action is mapped. */
};

struct KzuFreeCameraKeyAction
{
    enum KzuFreeCameraActionType actionType;        /**< Type of action. */
    enum KzsInputKey key;                           /**< Key that this action is mapped. */
};


/* Default key device actions for free camera. */
static const struct KzuFreeCameraKeyAction KZU_FREE_CAMERA_DEFAULT_KEY_MAPPINGS[] =
{
    { KZU_FREE_CAMERA_ACTION_FORWARD, KZS_KEY_W },
    { KZU_FREE_CAMERA_ACTION_BACKWARD, KZS_KEY_S },
    { KZU_FREE_CAMERA_ACTION_LEFT, KZS_KEY_A },
    { KZU_FREE_CAMERA_ACTION_RIGHT, KZS_KEY_D },
    { KZU_FREE_CAMERA_ACTION_SPEED_UP, KZS_KEY_3 },
    { KZU_FREE_CAMERA_ACTION_SPEED_DOWN, KZS_KEY_2 },
    { KZU_FREE_CAMERA_ACTION_DOUBLE_SPEED, KZS_KEY_MODIFIER1 },
    { KZU_FREE_CAMERA_ACTION_ROLLING_LEFT, KZS_KEY_E },
    { KZU_FREE_CAMERA_ACTION_ROLLING_RIGHT, KZS_KEY_Q },
    { KZU_FREE_CAMERA_ACTION_FIELD_OF_VIEW_UP, KZS_KEY_I },
    { KZU_FREE_CAMERA_ACTION_FIELD_OF_VIEW_DOWN, KZS_KEY_K },
    { KZU_FREE_CAMERA_ACTION_ROTATE_X_CLOCKWISE, KZS_KEY_Z },
    { KZU_FREE_CAMERA_ACTION_ROTATE_X_COUNTERCLOCKWISE, KZS_KEY_X },
    { KZU_FREE_CAMERA_ACTION_ROTATE_Y_CLOCKWISE, KZS_KEY_U },
    { KZU_FREE_CAMERA_ACTION_ROTATE_Y_COUNTERCLOCKWISE, KZS_KEY_J },
    { KZU_FREE_CAMERA_ACTION_TOGGLE_FREEZE, KZS_KEY_4 },
    { KZU_FREE_CAMERA_ACTION_UPDATE_TRANSFORMATION_TO_CAMERA, KZS_KEY_5 },
    { KZU_FREE_CAMERA_ACTION_LIFTING_SIDE, KZS_KEY_COMMA },
    { KZU_FREE_CAMERA_ACTION_LIFTING_DEPTH, KZS_KEY_PERIOD },
    { KZU_FREE_CAMERA_ACTION_STRAIGHTEN, KZS_KEY_MINUS },
    
};

#define KZU_FREE_CAMERA_DEFAULT_KEY_ACTION_COUNT (sizeof(KZU_FREE_CAMERA_DEFAULT_KEY_MAPPINGS) / sizeof(*KZU_FREE_CAMERA_DEFAULT_KEY_MAPPINGS))


/* Mouse controlled actions. */
static const struct KzuFreeCameraMouseAction KZU_FREE_CAMERA_DEFAULT_MOUSE_MAPPINGS[] =
{
    { KZU_FREE_CAMERA_ACTION_ROTATING, KZS_POINTING_DEVICE_BUTTON_PRIMARY },
    { KZU_FREE_CAMERA_ACTION_LIFTING, KZS_POINTING_DEVICE_BUTTON_SECONDARY }
    /*{ KZU_FREE_CAMERA_ACTION_FIELD_OF_VIEW_UP, KZS_POINTING_DEVICE_BUTTON_ROLL_UP },
    { KZU_FREE_CAMERA_ACTION_FIELD_OF_VIEW_DOWN, KZS_POINTING_DEVICE_BUTTON_ROLL_DOWN }*/
};

#define KZU_FREE_CAMERA_DEFAULT_MOUSE_ACTION_COUNT (sizeof(KZU_FREE_CAMERA_DEFAULT_MOUSE_MAPPINGS) / sizeof(*KZU_FREE_CAMERA_DEFAULT_MOUSE_MAPPINGS))


enum KzuFreeCameraActionState
{
    KZU_FREE_CAMERA_ACTION_STATE_ENABLED, /**< Action enabled. */
    KZU_FREE_CAMERA_ACTION_STATE_DISABLED, /**< Action disabled. */
    KZU_FREE_CAMERA_ACTION_STATE_NONE /**< Action in no state. */
};

struct KzuFreeCamera
{
    struct KzcQuaternion orientation;   /**< Camera orientation as quaternion. */
    struct KzcVector3 position;         /**< Camera position. */

    struct KzuCameraNode* originalCameraNode;   /**< Original camera object node of free camera. */
    struct KzuCameraNode* cameraNode;   /**< Camera object node of free camera. */
    KzuFreeCameraUpdateStateFunction updateStateFunction; /**< Function for updating camera state from free camera. */
    struct KzcMatrix4x4 parentWorldTransformation; /**< Parent world transformation. */
    void* callbackUserData;

    struct KzuFreeCameraKeyAction* keyActions;    /**< Free camera key actions. */
    struct KzuFreeCameraMouseAction* mouseActions; /**< Free camera mouse actions. */
    enum KzuFreeCameraActionState actionStates[KZU_FREE_CAMERA_ACTION_COUNT];   /**< States for free camera actions. */

    kzBool doubleSpeed; /**< Is double speed enabled or disabled. */
    kzBool deaccelerateForward; /**< Is camera deaccelerating to forward direction. */
    kzBool deaccelerateSide; /**< Is camera deaccelerating to side direction. */

    kzBool rotatingEnabled; /**< Is rotating enabled. */
    kzBool liftingEnabled; /**< Is lifting up/down enabled. */
    kzBool rollingEnabled; /**< Is rolling enabled. */
    kzFloat rotateXSpeed; /**< Rotate X speed. */
    kzFloat rotateYSpeed; /**< Rotate Y speed. */

    kzInt mouseX; /**< Current mouse X coordinate for free camera. */
    kzInt mouseY; /**< Current mouse Y coordinate for free camera. */
    kzInt previousMouseX; /**< Previous mouse X coordinate for free camera. */
    kzInt previousMouseY; /**< Previous mouse Y coordinate for free camera. */

    kzFloat mouseRotateTargetX; /**< Variable for animating rotation X. */
    kzFloat mouseRotateTargetY; /**< Variable for animating rotation Y. */
    kzFloat forwardMovement; /**< Speed of forward movement, from -1 to 1. */
    kzFloat sideMovement; /**< Speed of side movement, from -1 to 1. */
    kzFloat rollMovement; /**< Speed of roll movement. */
    kzFloat fieldOfViewMovementRoll; /**< Field of view movement for mouse roll. */
    kzFloat speedMovement;  /**< How much speed moves. */
    kzFloat speed; /**< Current speed of camera. */

    kzBool freezed; /**< Free camera freezed or not. */
};


static void kzuFreeCameraResetMovement_internal(struct KzuFreeCamera* freeCamera)
{
    kzUint i;

    freeCamera->doubleSpeed             = KZ_FALSE;
    freeCamera->deaccelerateForward     = KZ_FALSE;
    freeCamera->deaccelerateSide        = KZ_FALSE;

    freeCamera->forwardMovement = 0.0f;
    freeCamera->sideMovement = 0.0f;
    freeCamera->rollMovement = 0.0f;
    freeCamera->fieldOfViewMovementRoll = 0.0f;
    freeCamera->speedMovement = 0.0f;
    freeCamera->speed = 1.0f;

    freeCamera->rotatingEnabled       = KZ_FALSE;
    freeCamera->liftingEnabled      = KZ_FALSE;
    freeCamera->rollingEnabled        = KZ_FALSE;
    freeCamera->freezed = KZ_FALSE;
    freeCamera->parentWorldTransformation = KZC_MATRIX4X4_IDENTITY;

    freeCamera->mouseX = 0;
    freeCamera->previousMouseX = 0;
    freeCamera->mouseY = 0;
    freeCamera->previousMouseY = 0;

    freeCamera->mouseRotateTargetX = 0.0f;
    freeCamera->mouseRotateTargetY = 0.0f;

    for (i = 0; i < (kzUint)KZU_FREE_CAMERA_ACTION_COUNT/*lint -e{930}*/; ++i) 
    {
        freeCamera->actionStates[i] = KZU_FREE_CAMERA_ACTION_STATE_NONE;
    }
}

kzsError kzuFreeCameraCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                             struct KzuFreeCamera** out_freeCamera)
{
    kzsError result;
    struct KzuFreeCamera* freeCamera;
    kzUint i;

    result = kzcMemoryAllocVariable(memoryManager, freeCamera, "FreeCamera");
    kzsErrorForward(result);

    result = kzuCameraNodeCreate(memoryManager, propertyManager, messageDispatcher, "Free camera", &freeCamera->cameraNode);
    kzsErrorForward(result); 

    /* Map key actions. */
    result = kzcMemoryAllocArray(memoryManager, freeCamera->keyActions, KZU_FREE_CAMERA_DEFAULT_KEY_ACTION_COUNT, "FreeCamera key actions");
    kzsErrorForward(result);
    for (i = 0; i < kzcArrayLength(freeCamera->keyActions); ++i)
    {
        freeCamera->keyActions[i].actionType = KZU_FREE_CAMERA_DEFAULT_KEY_MAPPINGS[i].actionType;
        freeCamera->keyActions[i].key = KZU_FREE_CAMERA_DEFAULT_KEY_MAPPINGS[i].key;
    }

    /* Map pointer device actions. */
    result = kzcMemoryAllocArray(memoryManager, freeCamera->mouseActions, KZU_FREE_CAMERA_DEFAULT_MOUSE_ACTION_COUNT, "FreeCamera pointer actions");
    kzsErrorForward(result);
    for (i = 0; i < kzcArrayLength(freeCamera->mouseActions); ++i)
    {
        freeCamera->mouseActions[i].actionType = KZU_FREE_CAMERA_DEFAULT_MOUSE_MAPPINGS[i].actionType;
        freeCamera->mouseActions[i].mouseButton = KZU_FREE_CAMERA_DEFAULT_MOUSE_MAPPINGS[i].mouseButton;
    }

    kzuFreeCameraResetMovement_internal(freeCamera);
    freeCamera->updateStateFunction = KZ_NULL;
    freeCamera->originalCameraNode = KZ_NULL;

    result = kzuFreeCameraInitializeDefault(freeCamera);
    kzsErrorForward(result);

    *out_freeCamera = freeCamera;
    kzsSuccess();
}

kzsError kzuFreeCameraDelete(struct KzuFreeCamera* freeCamera)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(freeCamera));

    result = kzuCameraNodeDelete(freeCamera->cameraNode);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray(freeCamera->keyActions);
    kzsErrorForward(result);
    result = kzcMemoryFreeArray(freeCamera->mouseActions);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(freeCamera);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuFreeCameraInitializeDefault(struct KzuFreeCamera* freeCamera)
{
    kzsError result;
    struct KzcMatrix4x4 freeCameraMatrix;
    struct KzuCameraNode* cameraNode;

    kzsAssert(kzcIsValidPointer(freeCamera));

    cameraNode = freeCamera->cameraNode;

    kzcVector3Set(&freeCamera->position, 0.0f, 4.7f, 15.0f);
    kzcQuaternionInitializeFromVector(&KZC_VECTOR3_X_AXIS, -KZS_PI * 15.0f / 180.0f, &freeCamera->orientation);
    kzcQuaternionNormalize(&freeCamera->orientation);

    kzuFreeCameraInitializeMatrix(freeCamera, &freeCameraMatrix);

    result = kzuPropertyManagerRemoveProperties(kzuObjectNodeGetPropertyManager(kzuCameraNodeToObjectNode(cameraNode)), cameraNode);
    kzsErrorForward(result);

    result = kzuObjectNodeSetTransformation(kzuCameraNodeToObjectNode(cameraNode), &freeCameraMatrix);
    kzsErrorForward(result);

    result = kzuCameraNodeSetFov(cameraNode, 45.0f);
    kzsErrorForward(result);

    kzuFreeCameraResetMovement_internal(freeCamera);

    kzsSuccess();
}

kzsError kzuFreeCameraInitializeFromCameraNode(struct KzuFreeCamera* freeCamera, const struct KzuScene* scene,
                                               struct KzuCameraNode* cameraNode, kzBool setTransformation,
                                               const struct KzcMatrix4x4* parentWorldTransformation)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(freeCamera));

    freeCamera->originalCameraNode = cameraNode;

    result = kzuCameraNodeCloneData(cameraNode, freeCamera->cameraNode);
    kzsErrorForward(result);

    if (setTransformation)
    {
        struct KzcMatrix4x4 transformation;
        struct KzcMatrix4x4 inverseTransformation;
        kzBool objectFound;

        kzsAssert(kzcIsValidPointer(freeCamera));

        objectFound = kzuSceneCalculateWorldTransformation(scene, kzuCameraNodeToObjectNode(cameraNode), &transformation);
        kzsErrorTest(objectFound, KZS_ERROR_ILLEGAL_OPERATION, "Trying to get position from camera which is not in the scene.");

        kzcMatrix4x4GetPosition(&transformation, &freeCamera->position);
        kzcMatrix4x4SetPositionVector3(&transformation, &KZC_VECTOR3_ZERO);

        kzcMatrix4x4Inverse(&transformation, &inverseTransformation);

        kzcQuaternionInitializeFromMatrix4x4(&inverseTransformation, &freeCamera->orientation);
    }
    freeCamera->parentWorldTransformation = *parentWorldTransformation;

    kzuFreeCameraResetMovement_internal(freeCamera);

    kzsSuccess();
}

void kzuFreeCameraSetPosition(struct KzuFreeCamera* freeCamera, const struct KzcVector3* position)
{
    kzsAssert(kzcIsValidPointer(freeCamera));
    freeCamera->position = *position;
}

struct KzuCameraNode* kzuFreeCameraGetCameraNode(const struct KzuFreeCamera* freeCamera)
{
    kzsAssert(kzcIsValidPointer(freeCamera));

    return freeCamera->cameraNode;
}

void kzuFreeCameraInitializeMatrix(const struct KzuFreeCamera* freeCamera, struct KzcMatrix4x4* out_worldMatrix)
{
    struct KzcMatrix4x4 translationMatrix;
    struct KzcMatrix4x4 quaternionMatrix;
    struct KzcMatrix4x4 quaternionMatrixInverse;
    struct KzcMatrix4x4 result;

    kzsAssert(kzcIsValidPointer(freeCamera));

    kzcQuaternionConvertToMatrix4x4(&freeCamera->orientation, &quaternionMatrix);
    kzcMatrix4x4Inverse(&quaternionMatrix, &quaternionMatrixInverse);
    kzcMatrix4x4SetIdentity(&translationMatrix);
    kzcMatrix4x4TranslateVector3(&translationMatrix, &freeCamera->position);
    kzcMatrix4x4Multiply(&quaternionMatrixInverse, &translationMatrix, &result);

    *out_worldMatrix = result;
}

static kzsError kzuFreeCameraHandleActions_internal(struct KzuFreeCamera* freeCamera)
{
    kzUint i;

    kzsAssert(kzcIsValidPointer(freeCamera));

    /* Forward enable/disable. */
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_FORWARD] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->deaccelerateForward = KZ_FALSE;
        freeCamera->forwardMovement = -1.0f;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_FORWARD] == KZU_FREE_CAMERA_ACTION_STATE_DISABLED)
    {
        freeCamera->deaccelerateForward = KZ_TRUE;
    }

    /* Backward enable/disable. */
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_BACKWARD] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->deaccelerateForward = KZ_FALSE;
        freeCamera->forwardMovement = 1.0f;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_BACKWARD] == KZU_FREE_CAMERA_ACTION_STATE_DISABLED)
    {
        freeCamera->deaccelerateForward = KZ_TRUE;
    }

    /* Left enable/disable. */
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_LEFT]== KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->deaccelerateSide = KZ_FALSE;
        freeCamera->sideMovement = -1.0f;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_LEFT] == KZU_FREE_CAMERA_ACTION_STATE_DISABLED)
    {
        freeCamera->deaccelerateSide = KZ_TRUE;
    }

    /* Right enable/disable. */
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_RIGHT] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->deaccelerateSide = KZ_FALSE;
        freeCamera->sideMovement = 1.0f;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_RIGHT] == KZU_FREE_CAMERA_ACTION_STATE_DISABLED)
    {
        freeCamera->deaccelerateSide = KZ_TRUE;
    }

    /* Speed-up enable/disable. */
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_SPEED_UP] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->speedMovement = 1.0f;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_SPEED_DOWN] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->speedMovement = -1.0f;
    }
    else
    {
        freeCamera->speedMovement = 0.0f;
    }

    /* Rotate X. */
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROTATE_X_CLOCKWISE] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->rotateXSpeed = 1.0f;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROTATE_X_COUNTERCLOCKWISE] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->rotateXSpeed = -1.0f;
    }
    else
    {
        freeCamera->rotateXSpeed = 0.0f;
    }

    /* Rotate Y. */
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROTATE_Y_CLOCKWISE] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->rotateYSpeed = 1.0f;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROTATE_Y_COUNTERCLOCKWISE] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->rotateYSpeed = -1.0f;
    }
    else
    {
        freeCamera->rotateYSpeed = 0.0f;
    }

    /* Double speed enable/disable. */
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_DOUBLE_SPEED] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->doubleSpeed = KZ_TRUE;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_DOUBLE_SPEED] == KZU_FREE_CAMERA_ACTION_STATE_DISABLED)
    {
        freeCamera->doubleSpeed = KZ_FALSE;
    }
    
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_TOGGLE_FREEZE] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_TOGGLE_FREEZE] = KZU_FREE_CAMERA_ACTION_STATE_DISABLED;
        freeCamera->freezed = !freeCamera->freezed;
    }
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_UPDATE_TRANSFORMATION_TO_CAMERA] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_UPDATE_TRANSFORMATION_TO_CAMERA] = KZU_FREE_CAMERA_ACTION_STATE_DISABLED;
        if(freeCamera->updateStateFunction != KZ_NULL)
        {
            kzsError result;
            struct KzcMatrix4x4 inverseParent;
            struct KzcMatrix4x4 freeCameraTransformation;
            struct KzcMatrix4x4 transformation;
            kzuFreeCameraInitializeMatrix(freeCamera, &transformation);
            kzcMatrix4x4Inverse(&freeCamera->parentWorldTransformation, &inverseParent);
            kzcMatrix4x4Multiply(&transformation, &inverseParent, &freeCameraTransformation);

            result = freeCamera->updateStateFunction(&freeCameraTransformation, freeCamera->callbackUserData);
            kzsErrorIf(result)
            {
                kzsLog(KZS_LOG_LEVEL_INFO, "Error in calling callback of free camera");
            }
            kzsErrorForward(result);
        }
    }

    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROTATING] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->rotatingEnabled = KZ_TRUE;
        freeCamera->previousMouseX = freeCamera->mouseX;
        freeCamera->previousMouseY = freeCamera->mouseY;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROTATING] == KZU_FREE_CAMERA_ACTION_STATE_DISABLED)
    {
        freeCamera->rotatingEnabled = KZ_FALSE;
    }

    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROLLING] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->rollingEnabled = KZ_TRUE;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROLLING] == KZU_FREE_CAMERA_ACTION_STATE_DISABLED)
    {
        freeCamera->rollingEnabled = KZ_FALSE;
    }

    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_LIFTING] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->liftingEnabled = KZ_TRUE;
        freeCamera->previousMouseX = freeCamera->mouseX;
        freeCamera->previousMouseY = freeCamera->mouseY;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_LIFTING] == KZU_FREE_CAMERA_ACTION_STATE_DISABLED)
    {
        freeCamera->liftingEnabled = KZ_FALSE;
    }

    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_FIELD_OF_VIEW_UP] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->fieldOfViewMovementRoll = 1.0f;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_FIELD_OF_VIEW_DOWN] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->fieldOfViewMovementRoll = -1.0f;
    }
    else
    {
        freeCamera->fieldOfViewMovementRoll = 0.0f;
    }

    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROLLING_LEFT] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->rollMovement = -1.0f;
        freeCamera->rollingEnabled = KZ_TRUE;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROLLING_LEFT] == KZU_FREE_CAMERA_ACTION_STATE_DISABLED)
    {
        freeCamera->rollingEnabled = KZ_FALSE;
    }
    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROLLING_RIGHT] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        freeCamera->rollMovement = 1.0f;
        freeCamera->rollingEnabled = KZ_TRUE;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_ROLLING_RIGHT] == KZU_FREE_CAMERA_ACTION_STATE_DISABLED)
    {
        freeCamera->rollingEnabled = KZ_FALSE;
    }

    /* Mouse actions have to be reset to none as their actions are spawned differently that key actions. */
    for(i = 0; i < kzcArrayLength(freeCamera->mouseActions); ++i)
    {
        enum KzuFreeCameraActionType actionType = freeCamera->mouseActions[i].actionType;

        freeCamera->actionStates[actionType] = KZU_FREE_CAMERA_ACTION_STATE_NONE;
    }
    kzsSuccess();
}

static kzsError kzuFreeCameraUpdate_internal(struct KzuFreeCamera* freeCamera, kzFloat rotateDeltaX, kzFloat rotateDeltaY, 
                                         kzFloat forwardMovement, kzFloat sideMovement, kzFloat upMovement,
                                         kzFloat rollMovement, kzFloat fieldOfViewMovement)
{
    kzsError result;
    struct KzcQuaternion quaternion;
    struct KzcQuaternion tempOrientation;
    struct KzcQuaternion rotationAxis;
    struct KzcQuaternion rotationAxis2;
    struct KzcQuaternion rotationAxis3;
    struct KzcVector3 movementForward;
    struct KzcVector3 movementSide;
    struct KzcVector3 movementUp;
    struct KzcMatrix4x4 freeCameraMatrix;
    kzFloat fieldOfView;
    
    kzsAssert(kzcIsValidPointer(freeCamera));

    /* Update camera's orientation (rotateX, rotateY and roll). */
    if(!freeCamera->freezed)
    {
        tempOrientation = freeCamera->orientation;
        kzcQuaternionInitializeFromVector(&KZC_VECTOR3_Z_AXIS, rollMovement, &rotationAxis3);
        kzcQuaternionMultiply(&tempOrientation, &rotationAxis3, &quaternion);
        tempOrientation = quaternion;

        kzcQuaternionInitializeFromVector(&KZC_VECTOR3_Y_AXIS, rotateDeltaX, &rotationAxis);
        kzcQuaternionMultiply(&tempOrientation, &rotationAxis, &quaternion);
        tempOrientation = quaternion;

        kzcQuaternionInitializeFromVector(&KZC_VECTOR3_X_AXIS, rotateDeltaY, &rotationAxis2);
        kzcQuaternionMultiply(&tempOrientation, &rotationAxis2, &quaternion);

        freeCamera->orientation = quaternion;
    }

    /* Update camera's position (forward, side and up movement). */
    if(!freeCamera->freezed)
    {
        kzcQuaternionMultiplyVector3(&freeCamera->orientation, &KZC_VECTOR3_Z_AXIS, &movementForward);
        kzcVector3Scale(&movementForward, forwardMovement, &movementForward);
        kzcVector3Add(&freeCamera->position, &movementForward, &freeCamera->position);

        kzcQuaternionMultiplyVector3(&freeCamera->orientation, &KZC_VECTOR3_X_AXIS, &movementSide);
        kzcVector3Scale(&movementSide, sideMovement, &movementSide);
        kzcVector3Add(&freeCamera->position, &movementSide, &freeCamera->position);

        kzcQuaternionMultiplyVector3(&freeCamera->orientation, &KZC_VECTOR3_Y_AXIS, &movementUp);
        kzcVector3Scale(&movementUp, upMovement, &movementUp);
        kzcVector3Add(&freeCamera->position, &movementUp, &freeCamera->position);
    }

    kzuFreeCameraInitializeMatrix(freeCamera, &freeCameraMatrix);
    result = kzuObjectNodeSetTransformation(kzuCameraNodeToObjectNode(freeCamera->cameraNode), &freeCameraMatrix);
    kzsErrorForward(result);

    /* Update camera's field of view. */
    {
        fieldOfView = kzuCameraNodeGetFov(freeCamera->cameraNode);
        fieldOfView += fieldOfViewMovement;
        fieldOfView = kzsClampf(fieldOfView, 0.01f, 180.0f);
        result = kzuCameraNodeSetFov(freeCamera->cameraNode, fieldOfView);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuFreeCameraUpdate(struct KzuFreeCamera* freeCamera, kzUint deltaTime)
{
    kzsError result;
    kzFloat freeCameraSpeed;
    kzFloat mouseDeltaX;
    kzFloat mouseDeltaY;
    kzFloat mouseRotateX;
    kzFloat mouseRotateY;
    kzFloat liftX = 0.0f;
    kzFloat liftY = 0.0f;
    kzFloat liftZ = 0.0f;
    kzFloat deltaTimeSeconds = (kzFloat)deltaTime / 1000.0f;

    kzsAssert(kzcIsValidPointer(freeCamera));
    kzsAssert(freeCamera->cameraNode != KZ_NULL);

    mouseDeltaX = (kzFloat)(freeCamera->previousMouseX - freeCamera->mouseX);
    mouseDeltaY = (kzFloat)(freeCamera->previousMouseY - freeCamera->mouseY);

    freeCamera->speed += freeCamera->speedMovement * KZU_FREE_CAMERA_SPEED_ACCELERATION_SPEED * deltaTimeSeconds;
    freeCamera->speed = kzsMaxf(freeCamera->speed, 0.0f);
    freeCameraSpeed = freeCamera->speed;

    if (freeCamera->doubleSpeed)
    {
        freeCameraSpeed *= 2.0f;
    }

    freeCameraSpeed *= KZU_FREE_CAMERA_BASE_MOVEMENT_SPEED;

    result = kzuFreeCameraHandleActions_internal(freeCamera);
    kzsErrorForward(result);

    if (freeCamera->rotatingEnabled)
    {
        freeCamera->mouseRotateTargetX += mouseDeltaX * KZU_FREE_CAMERA_ROTATE_SPEED;
        freeCamera->mouseRotateTargetY += mouseDeltaY * KZU_FREE_CAMERA_ROTATE_SPEED;
    }

    freeCamera->mouseRotateTargetX += freeCamera->rotateXSpeed * KZU_FREE_CAMERA_ROTATE_SPEED * 2.0f;
    freeCamera->mouseRotateTargetY += freeCamera->rotateYSpeed * KZU_FREE_CAMERA_ROTATE_SPEED * 2.0f;

    mouseRotateX = freeCamera->mouseRotateTargetX * KZU_FREE_CAMERA_ANGULAR_HARDNESS;
    mouseRotateY = freeCamera->mouseRotateTargetY * KZU_FREE_CAMERA_ANGULAR_HARDNESS;
    freeCamera->mouseRotateTargetX -= mouseRotateX;
    freeCamera->mouseRotateTargetY -= mouseRotateY;

    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_LIFTING_SIDE] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        liftX = freeCamera->liftingEnabled ? mouseDeltaY * KZU_FREE_CAMERA_LIFTING_SPEED : 0.0f;
    }
    else if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_LIFTING_DEPTH] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        liftZ = freeCamera->liftingEnabled ? mouseDeltaY * KZU_FREE_CAMERA_LIFTING_SPEED : 0.0f;
    }
    else
    {
        liftY = freeCamera->liftingEnabled ? mouseDeltaY * KZU_FREE_CAMERA_LIFTING_SPEED : 0.0f;
    }

    if(freeCamera->actionStates[KZU_FREE_CAMERA_ACTION_STRAIGHTEN] == KZU_FREE_CAMERA_ACTION_STATE_ENABLED)
    {
        struct KzcMatrix4x4 matrix;
        struct KzcQuaternion temp;
        struct KzcQuaternion resultQuaternion;
        kzcQuaternionConvertToMatrix4x4(&freeCamera->orientation, &matrix);

        matrix.data[4] = 0.0f;
        matrix.data[5] = 1.0f;
        matrix.data[6] = 0.0f;

        kzcQuaternionInitializeFromMatrix4x4(&matrix, &temp);

        kzcQuaternionSlerp(&freeCamera->orientation, &temp, KZU_FREE_CAMERA_ORIENTATION_NORMALIZATION_SPEED * deltaTime, &resultQuaternion);
        freeCamera->orientation = resultQuaternion;

        kzcQuaternionNormalize(&freeCamera->orientation);
    }

    result = kzuFreeCameraUpdate_internal(freeCamera, mouseRotateX, mouseRotateY,
                                          freeCameraSpeed * freeCamera->forwardMovement * deltaTimeSeconds + liftZ,
                                          freeCameraSpeed * freeCamera->sideMovement * deltaTimeSeconds + liftX, liftY,
                                          freeCamera->rollMovement * KZU_FREE_CAMERA_ROLL_MOVEMENT_SPEED,
                                          freeCamera->fieldOfViewMovementRoll * KZU_FREE_CAMERA_FIELD_OF_VIEW_MOVEMENT_SPEED);
    kzsErrorForward(result);

    if (!freeCamera->rollingEnabled)
    {
        freeCamera->rollMovement -= freeCamera->rollMovement * KZU_FREE_CAMERA_ANGULAR_HARDNESS;
    }

    if(freeCamera->deaccelerateForward)
    {
        if(freeCamera->forwardMovement > KZ_FLOAT_EPSILON)
        {
            freeCamera->forwardMovement -= KZU_FREE_CAMERA_SPEED_ACCELERATION_SPEED * deltaTimeSeconds;
            if(freeCamera->forwardMovement < 0.0f)
            {
                freeCamera->forwardMovement = 0.0f;
            }
        }
        else if(freeCamera->forwardMovement < -KZ_FLOAT_EPSILON)
        {
            freeCamera->forwardMovement += KZU_FREE_CAMERA_SPEED_ACCELERATION_SPEED * deltaTimeSeconds;
            if(freeCamera->forwardMovement > 0.0f)
            {
                freeCamera->forwardMovement = 0.0f;
            }
        }
    }

    if(freeCamera->deaccelerateSide)
    {
        if(freeCamera->sideMovement > KZ_FLOAT_EPSILON)
        {
            freeCamera->sideMovement -= KZU_FREE_CAMERA_SPEED_ACCELERATION_SPEED * deltaTimeSeconds;
            if(freeCamera->sideMovement <= KZ_FLOAT_EPSILON)
            {
                freeCamera->sideMovement = 0.0f;
            }
        }
        else if(freeCamera->sideMovement < -KZ_FLOAT_EPSILON)
        {
            freeCamera->sideMovement += KZU_FREE_CAMERA_SPEED_ACCELERATION_SPEED * deltaTimeSeconds;
            if(freeCamera->sideMovement >= -KZ_FLOAT_EPSILON)
            {
                freeCamera->sideMovement = 0.0f;
            }
        }
    }

    freeCamera->previousMouseX = freeCamera->mouseX;
    freeCamera->previousMouseY = freeCamera->mouseY;

    if(kzuObjectNodeIsValid(/*lint -e{740}*/(struct KzuObjectNode*)freeCamera->originalCameraNode))
    {
        struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(kzuCameraNodeToObjectNode(freeCamera->originalCameraNode));

        result = kzuPropertyManagerCopyProperty(propertyManager, freeCamera->originalCameraNode, freeCamera->cameraNode,
                                                KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_EYE_DISTANCE);
        kzsErrorForward(result);

        result = kzuPropertyManagerCopyProperty(propertyManager, freeCamera->originalCameraNode, freeCamera->cameraNode,
                                                KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_FOCUS_DISTANCE);
        kzsErrorForward(result);

        /* TODO: Perhaps this should copy other properties as well */
    }

    kzsSuccess();
}

void kzuFreeCameraSetFreezed(struct KzuFreeCamera* freeCamera, kzBool freezed)
{
    kzsAssert(kzcIsValidPointer(freeCamera));
    freeCamera->freezed = freezed;
}

kzBool kzuFreeCameraIsFreezed(const struct KzuFreeCamera* freeCamera)
{
    kzsAssert(kzcIsValidPointer(freeCamera));
    return freeCamera->freezed;
}

kzsError kzuFreeCameraMapActionToKey(struct KzuFreeCamera* freeCamera, enum KzsInputKey key, enum KzuFreeCameraActionType actionType)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(freeCamera);
    kzUint i;
    struct KzuFreeCameraKeyAction* newActions;

    kzsAssert(kzcIsValidPointer(freeCamera));

    result = kzcMemoryAllocArray(memoryManager, newActions, kzcArrayLength(freeCamera->keyActions) + 1, "NewKeyActions");
    kzsErrorForward(result);

    for(i = 0; i < kzcArrayLength(freeCamera->keyActions); ++i)
    {
        newActions[i] = freeCamera->keyActions[i];
    }
    newActions[i].key = key;
    newActions[i].actionType = actionType;

    result = kzcMemoryFreeArray(freeCamera->keyActions);
    kzsErrorForward(result);
    freeCamera->keyActions = newActions;

    kzsSuccess();
}

kzsError kzuFreeCameraMapActionToPointerButton(struct KzuFreeCamera* freeCamera, kzU32 pointerButton, enum KzuFreeCameraActionType actionType)
{
    kzUint i;
    kzsError result;
    struct KzuFreeCameraMouseAction* newActions;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(freeCamera);

    kzsAssert(kzcIsValidPointer(freeCamera));

    result = kzcMemoryAllocArray(memoryManager, newActions, kzcArrayLength(freeCamera->mouseActions) + 1, "NewMouseActions");
    kzsErrorForward(result);

    for(i = 0; i < kzcArrayLength(freeCamera->mouseActions); ++i)
    {
        newActions[i] = freeCamera->mouseActions[i];
    }
    newActions[kzcArrayLength(freeCamera->mouseActions)].mouseButton = pointerButton;
    newActions[kzcArrayLength(freeCamera->mouseActions)].actionType = actionType;

    result = kzcMemoryFreeArray(freeCamera->mouseActions);
    kzsErrorForward(result);
    freeCamera->mouseActions = newActions;

    kzsSuccess();
}

void kzuFreeCameraHandleKeyEvents(struct KzuFreeCamera* freeCamera, enum KzsInputKey keyCode, enum KzsInputKeyDeviceState keyState)
{
    kzUint i;
    kzsAssert(kzcIsValidPointer(freeCamera));

    for(i = 0; i < kzcArrayLength(freeCamera->keyActions); ++i)
    {
        enum KzuFreeCameraActionType actionType = freeCamera->keyActions[i].actionType;
        if(keyCode == freeCamera->keyActions[i].key)
        {
            if(keyState == KZS_KEY_DEVICE_STATE_DOWN)
            {
                freeCamera->actionStates[actionType] = KZU_FREE_CAMERA_ACTION_STATE_ENABLED;
            }
            else if(keyState == KZS_KEY_DEVICE_STATE_UP)
            {
                freeCamera->actionStates[actionType] = KZU_FREE_CAMERA_ACTION_STATE_DISABLED;
            }
        }
    }
}

void kzuFreeCameraHandlePointerEvents(struct KzuFreeCamera* freeCamera, kzU32 buttons, enum KzsInputPointingDeviceState state, kzInt mouseX, kzInt mouseY)
{
    kzUint i;

    kzsAssert(kzcIsValidPointer(freeCamera));

    freeCamera->mouseX = mouseX;
    freeCamera->mouseY = mouseY;

    for(i = 0; i < kzcArrayLength(freeCamera->mouseActions); ++i)
    {
        enum KzuFreeCameraActionType actionType = freeCamera->mouseActions[i].actionType;

        if((buttons & freeCamera->mouseActions[i].mouseButton) != 0)
        {
            if(state == KZS_POINTING_DEVICE_STATE_DOWN)
            {
                freeCamera->previousMouseX = mouseX;
                freeCamera->previousMouseY = mouseY;
                freeCamera->actionStates[actionType] = KZU_FREE_CAMERA_ACTION_STATE_ENABLED;
            }
        }
        if(state == KZS_POINTING_DEVICE_STATE_UP)
        {
            freeCamera->actionStates[actionType] = KZU_FREE_CAMERA_ACTION_STATE_DISABLED;
        }
    }
}

void kzuFreeCameraSetActionEvent(struct KzuFreeCamera* freeCamera, enum KzuFreeCameraActionType actionType)
{
    kzsAssert(kzcIsValidPointer(freeCamera));
    freeCamera->actionStates[actionType] = KZU_FREE_CAMERA_ACTION_STATE_ENABLED;
}

void kzuFreeCameraSetUpdateStateFunction(struct KzuFreeCamera* freeCamera, KzuFreeCameraUpdateStateFunction updateStateFunction, void* userData)
{
    kzsAssert(kzcIsValidPointer(freeCamera));

    freeCamera->updateStateFunction = updateStateFunction;    
    freeCamera->callbackUserData = userData;
}
