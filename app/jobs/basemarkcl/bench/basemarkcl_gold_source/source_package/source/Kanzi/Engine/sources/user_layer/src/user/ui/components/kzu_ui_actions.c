/**
* \file
* User interface label.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_actions.h"

#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_event.h>
#include <user/ui/kzu_ui_manager.h>
#include <user/engine/kzu_engine_message_queue.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>


KZ_CALLBACK kzsError kzuUiCallbackSetPropertyValue(const struct KzuUiComponentNode* componentNode, void* userData,
                                                   struct KzuUiEvent* event,  kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;
    
    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_SET_PROPERTY_VALUE, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackSwitchBooleanProperty(const struct KzuUiComponentNode* componentNode, void* userData,
                                                        struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_SWITCH_BOOLEAN_PROPERTY, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackSetBooleanProperty(const struct KzuUiComponentNode* componentNode, void* userData,
                                                     struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_SET_BOOLEAN_PROPERTY, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackPlayAnimation(const struct KzuUiComponentNode* componentNode, void* userData,
                                                struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_START_ANIMATION, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackStartTransition(const struct KzuUiComponentNode* componentNode, void* userData,
                                                  struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_START_TRANSITION, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackChangeScene(const struct KzuUiComponentNode* componentNode, void* userData,
                                              struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;
    
    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_TYPE_CHANGE_SCENE, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackChangeRenderPassCamera(const struct KzuUiComponentNode* componentNode, void* userData,
                                                         struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_CHANGE_RENDER_PASS_CAMERA, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackChange2DPickingCamera(const struct KzuUiComponentNode* componentNode, void* userData,
                                                        struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_CHANGE_2D_PICKING_CAMERA, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackSetTransformationToIdentity(const struct KzuUiComponentNode* componentNode, void* userData,
                                                              struct KzuUiEvent* event, kzString eventType,
                                                              struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_SET_TRANSFORMATION_TO_IDENTITY, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackExecuteScript(const struct KzuUiComponentNode* componentNode, void* userData,
                                                struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_EXECUTE_SCRIPT, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackChangeSceneCamera(const struct KzuUiComponentNode* componentNode, void* userData,
                                                    struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_CHANGE_SCENE_CAMERA, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackRenderPassSetEnabled(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                                        kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_RENDER_PASS_SET_ENABLED_STATE, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackRenderPassToggleEnabled(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                                        kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_RENDER_PASS_TOGGLE_ENABLED_STATE, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackRenderPassRenderOnceAndDisable(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                                        kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_RENDER_PASS_RENDER_FOR_ONE_FRAME_AND_DISABLE, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackChangeComposer(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                     kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_CHANGE_SCENE_COMPOSER, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackSetTexturePropertyValue(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                              kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_SET_TEXTURE_PROPERTY, parameters);
    kzsErrorForward(result);

    kzsSuccess();
}
