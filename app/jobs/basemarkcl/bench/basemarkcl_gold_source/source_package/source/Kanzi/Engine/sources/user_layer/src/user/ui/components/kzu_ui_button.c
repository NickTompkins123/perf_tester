/**
* \file
* User interface button component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_button.h"
#include "kzu_ui_component_common.h"

#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_event.h>
#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_BUTTON = (void*)&KZU_COMPONENT_TYPE_BUTTON;


const kzString KZU_UI_BUTTON_PRESSED_EVENT_STRING = "ButtonPressedEvent";
const kzString KZU_UI_BUTTON_RELEASED_EVENT_STRING = "ButtonReleasedEvent";
const kzString KZU_UI_BUTTON_LONG_RELEASED_EVENT_STRING = "ButtonLongReleasedEvent";
const kzString KZU_UI_BUTTON_POINTER_ENTER_EVENT_STRING = "ButtonPointerEnterEvent";
const kzString KZU_UI_BUTTON_POINTER_LEAVE_EVENT_STRING = "ButtonPointerLeaveEvent";


kzFloat kzuUiButtonGetLongReleaseInterval(const struct KzuObjectNode* button)
{
    return kzuObjectNodeGetFloatPropertyDefault(button, KZU_PROPERTY_TYPE_BUTTON_LONG_RELEASE_INTERVAL);
}

kzFloat kzuUiButtonGetRepeatInterval(const struct KzuObjectNode* button)
{
    return kzuObjectNodeGetFloatPropertyDefault(button, KZU_PROPERTY_TYPE_BUTTON_REPEAT_INTERVAL);
}

KZ_CALLBACK kzsError kzuUiButtonInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_BUTTON);

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiButtonUninitialize(struct KzuUiComponentNode* componentNode)
{
    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiButtonHandleEvents(const struct KzuUiComponentNode* componentNode, kzString eventType,
                                             struct KzuUiEvent* event, struct KzcHashMap* customParameters,
                                             struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);

    if(kzuObjectNodeGetChildCount(objectNode) > 0)
    {
        kzBool componentPressed = kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED);
        struct KzuObjectNode* pickedObject = kzuUiEventGetPickedObjectNode(event);
        struct KzuObjectNode* dragStartObject = kzuUiEventGetDragStartPickedObjectNode(event);
        kzBool buttonHit = pickedObject != KZ_NULL;
        kzFloat longReleaseInterval = kzuUiButtonGetLongReleaseInterval(objectNode);

        /* Check the pressed item is part of buttons hierarchy. */
        if (buttonHit)
        {
            buttonHit = kzuUiFindChildNode(objectNode, pickedObject);
        }

        if (kzuUiEventIsPointerPressed(event) && !kzuUiEventIsPointerMoved(event) &&
            !componentPressed && buttonHit && kzcStringIsEqual(eventType, "PointerPressedEvent") &&
            pickedObject != dragStartObject)
        {
            componentPressed = KZ_TRUE;
            result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED, KZ_TRUE);
            kzsErrorForward(result);
            result = kzuObjectNodeSetFloatProperty(objectNode, KZU_PROPERTY_TYPE_BUTTON_PRESS_DURATION, 0.0f);
            kzsErrorForward(result);
            result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_UI_BUTTON_POINTER_ENTER_EVENT_STRING, queue);
            kzsErrorForward(result);
        }
        
        if (buttonHit && kzuUiEventIsPointerPressed(event) && kzcStringIsEqual(eventType, "PointerPressedEvent") && !componentPressed)
        {
            result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED, KZ_TRUE);
            kzsErrorForward(result);
            result = kzuObjectNodeSetFloatProperty(objectNode, KZU_PROPERTY_TYPE_BUTTON_PRESS_DURATION, 0.0f);
            kzsErrorForward(result);
            result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_UI_BUTTON_PRESSED_EVENT_STRING, queue);
            kzsErrorForward(result);
        }
        if (buttonHit && componentPressed && kzuUiEventIsPointerReleased(event) && kzcStringIsEqual(eventType, "PointerReleasedEvent"))
        {
            kzFloat duration = kzuObjectNodeGetFloatPropertyDefault(objectNode, KZU_PROPERTY_TYPE_BUTTON_PRESS_DURATION);
            result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED, KZ_FALSE);

            if (longReleaseInterval > 0.0f && duration >= longReleaseInterval)
            {
                result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_UI_BUTTON_LONG_RELEASED_EVENT_STRING, queue);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_UI_BUTTON_RELEASED_EVENT_STRING, queue);
                kzsErrorForward(result);
            }
        }

        if (kzuUiEventIsPointerPressed(event) && componentPressed && !buttonHit &&
            kzcStringIsEqual(eventType, "PointerPressedEvent"))
        {
            result = kzuObjectNodeSetBoolProperty(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED, KZ_FALSE);
            kzsErrorForward(result);
            result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_UI_BUTTON_POINTER_LEAVE_EVENT_STRING, queue);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiButtonLogicUpdate(const struct KzuUiComponentNode* componentNode, kzFloat deltaTime, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuObjectNode* objectNode = kzuUiComponentNodeToObjectNode(componentNode);

    if (kzuObjectNodeGetBoolPropertyDefault(objectNode, KZU_PROPERTY_TYPE_COMPONENT_PRESSED))
    {
        kzFloat value = kzuObjectNodeGetFloatPropertyDefault(objectNode, KZU_PROPERTY_TYPE_BUTTON_PRESS_DURATION);
        value += deltaTime;
        result = kzuObjectNodeSetFloatProperty(objectNode, KZU_PROPERTY_TYPE_BUTTON_PRESS_DURATION, value);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
