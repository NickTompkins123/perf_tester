/**
* \file
* Input manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_input_manager.h"

#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/ui/message/kzu_message_dispatcher.h>
#include <user/ui/message/kzu_general_messages.h>
#include <user/util/kzu_raypick.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_ray.h>

#include <system/input/kzs_input.h>


/** Input manager. */
struct KzuInputManager
{
    kzInt unused;
};


kzsError kzuInputManagerCreate(const struct KzcMemoryManager* memoryManager, struct KzuInputManager** out_inputManager)
{
    kzsError result;
    struct KzuInputManager* inputManager;

    result = kzcMemoryAllocVariable(memoryManager, inputManager, "Input manager");
    kzsErrorForward(result);

    KZ_UNUSED_PARAMETER(inputManager->unused);

    *out_inputManager = inputManager;
    kzsSuccess();
}

kzsError kzuInputManagerDelete(struct KzuInputManager* inputManager)
{
    kzsError result;

    result = kzcMemoryFreeVariable(inputManager);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuInputManagerProcessInputEvent(const struct KzsInputEvent* inputEvent,
                                          const struct KzuRenderer* renderer, const struct KzsWindow* window,
                                          const struct KzuTransformedScene* transformedScene, const struct KzuCameraNode* cameraNode)
{
    kzsError result;
    enum KzsInputEventType eventType = kzsInputEventGetType(inputEvent);

    if (eventType == KZS_INPUT_EVENT_POINTER)
    {
        struct KzsPointingDeviceInputData* data = kzsInputEventGetPointingDeviceData((struct KzsInputEvent*)inputEvent);
        struct KzuObjectSource* pickingSource = kzuTransformedSceneGetRootObjectSource(transformedScene);
        kzInt pointerX = kzsInputGetPointingDeviceX(data);
        kzInt pointerY = kzsInputGetPointingDeviceY(data);
        enum KzsInputPointingDeviceState state = kzsInputGetPointingDeviceState(data);
        struct KzuObjectNode* objectNode;
        struct KzuTransformedObjectNode* transformedNode;
        struct KzcVector3 offset;
        struct KzcRay ray;
        kzFloat distance;

        result = kzuRayPick(renderer, window, pickingSource, transformedScene, cameraNode, pointerX, pointerY, KZ_FALSE, &distance, &objectNode, &transformedNode, &offset, &ray);
        kzsErrorForward(result);

        if (objectNode != KZ_NULL)
        {
            struct KzuMessageDispatcher* dispatcher = kzuObjectNodeGetMessageDispatcher(objectNode);

            if (state == KZS_POINTING_DEVICE_STATE_DOWN)
            {
                struct KzuMessage* message;
                result = kzuMessageDispatcherCreateMessage(dispatcher, KZU_MESSAGE_POINTER_DOWN, objectNode, &message);
                kzsErrorForward(result);

                result = kzuMessageDispatcherSendMessage(dispatcher, message);
                kzsErrorForward(result);
            }
            else if (state == KZS_POINTING_DEVICE_STATE_UP)
            {
                struct KzuMessage* message;
                result = kzuMessageDispatcherCreateMessage(dispatcher, KZU_MESSAGE_POINTER_UP, objectNode, &message);
                kzsErrorForward(result);

                result = kzuMessageDispatcherSendMessage(dispatcher, message);
                kzsErrorForward(result);
            }
            
        }
    }

    kzsSuccess();
}
