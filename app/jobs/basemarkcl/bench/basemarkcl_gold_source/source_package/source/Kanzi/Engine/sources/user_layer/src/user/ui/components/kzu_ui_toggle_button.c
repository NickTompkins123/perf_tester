/**
* \file
* User interface toggle button component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_toggle_button.h"
#include "kzu_ui_component_common.h"

#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_event.h>
#include <user/ui/kzu_ui_manager.h>
#include <user/ui/components/kzu_ui_button.h>
#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/engine/kzu_engine_message_queue.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_TOGGLE_BUTTON = (void*)&KZU_COMPONENT_TYPE_TOGGLE_BUTTON;


const kzString KZU_TOGGLE_BUTTON_ENABLED = "ToggleButtonToggledOn";
const kzString KZU_TOGGLE_BUTTON_DISABLED = "ToggleButtonToggledOff";


KZ_CALLBACK kzsError kzuUiToggleButtonInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_TOGGLE_BUTTON);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiToggleButtonHandleEvents(const struct KzuUiComponentNode* componentNode, kzString eventType,
                                                   struct KzuUiEvent* event, struct KzcHashMap* customParameters,
                                                   struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuObjectNode* objectNode;

    kzsAssert(kzcIsValidPointer(componentNode));

    objectNode = kzuUiComponentNodeToObjectNode(componentNode);

    if(kzuObjectNodeGetChildCount(objectNode) > 0)
    {
        kzBool componentPressed = kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED);
        struct KzuObjectNode* pickedObject = kzuUiEventGetPickedObjectNode(event);
        struct KzuObjectNode* dragStartObject = kzuUiEventGetDragStartPickedObjectNode(event);
        kzBool buttonHit = pickedObject != KZ_NULL;

        /* Check the pressed item is part of buttons hierarchy. */
        if (buttonHit)
        {
            buttonHit = kzuUiFindChildNode(objectNode, pickedObject);
        }


        if (kzuUiEventIsPointerPressed(event) && !kzuUiEventIsPointerMoved(event) && !componentPressed && buttonHit &&
            kzcStringIsEqual(eventType, "PointerPressedEvent") && pickedObject != dragStartObject)
        {
            componentPressed = KZ_TRUE;
            result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED, KZ_TRUE);
            kzsErrorForward(result);
            result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_UI_BUTTON_POINTER_ENTER_EVENT_STRING, queue);
            kzsErrorForward(result);
        }

        if (buttonHit && kzuUiEventIsPointerPressed(event) && kzcStringIsEqual(eventType, "PointerPressedEvent") && !componentPressed)
        {
            result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED, KZ_TRUE);
            kzsErrorForward(result);
            result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_UI_BUTTON_PRESSED_EVENT_STRING, queue);
            kzsErrorForward(result);
        }
        if (buttonHit && componentPressed && kzuUiEventIsPointerReleased(event) && kzcStringIsEqual(eventType, "PointerReleasedEvent"))
        {
            kzBool oldState = kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_TOGGLE_BUTTON_STATE);

            result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_TOGGLE_BUTTON_STATE, !oldState);
            kzsErrorForward(result);
            result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED, KZ_FALSE);
            kzsErrorForward(result);
            result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_UI_BUTTON_RELEASED_EVENT_STRING, queue);
            kzsErrorForward(result);

            result = kzuUiComponentNodeHandleEvents(componentNode, event, oldState ? KZU_TOGGLE_BUTTON_DISABLED : KZU_TOGGLE_BUTTON_ENABLED, queue);
            kzsErrorForward(result);
        }

        if (kzuUiEventIsPointerPressed(event) && componentPressed && !buttonHit && kzcStringIsEqual(eventType, "PointerPressedEvent"))
        {
            result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED, KZ_FALSE);
            kzsErrorForward(result);
            result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_UI_BUTTON_POINTER_LEAVE_EVENT_STRING, queue);
            kzsErrorForward(result);
        }
    }
    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackToggleButtonSwitchState(const struct KzuUiComponentNode* componentNode, void* userData,
                                                          struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_SWITCH_TOGGLE_BUTTON_STATE, parameters);
    kzsErrorForward(result); 

    kzsSuccess();
}

kzsError kzuUiToggleButtonSetStateAction(const struct KzuUiComponentNode* componentNode, kzBool newState, void* userData,
                                         struct KzuUiEvent* event, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);
    
    KZ_UNUSED_PARAMETER(userData); /* TODO: Remove unused parameter */

    {
        kzBool oldState = kzuObjectNodeGetBoolPropertyDefault(node, KZU_PROPERTY_TYPE_TOGGLE_BUTTON_STATE);

        if(oldState != newState)
        {
            result = kzuUiToggleButtonSetState(componentNode, newState);
            kzsErrorForward(result);
            result = kzuUiComponentNodeHandleEvents(componentNode, event, oldState ? KZU_TOGGLE_BUTTON_DISABLED : KZU_TOGGLE_BUTTON_ENABLED, queue);
            kzsErrorForward(result);
        }
    }
    kzsSuccess();
}

kzsError kzuUiToggleButtonSetState(const struct KzuUiComponentNode* componentNode, kzBool enabled)
{
    kzsError result;
    struct KzuObjectNode* componentObjectNode = kzuUiComponentNodeToObjectNode(componentNode);

    result = kzuObjectNodeSetBoolProperty(componentObjectNode, KZU_PROPERTY_TYPE_TOGGLE_BUTTON_STATE, enabled);
    kzsErrorForward(result);

    kzsSuccess();
}

kzBool kzuUiToggleButtonGetState(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* componentObjectNode = kzuUiComponentNodeToObjectNode(componentNode);

    return kzuObjectNodeGetBoolPropertyDefault(componentObjectNode, KZU_PROPERTY_TYPE_TOGGLE_BUTTON_STATE);
}
