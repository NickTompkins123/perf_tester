/**
* \file
* Graphical user interface object generator
* Provides helpers for user interface items within code.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_component_creator.h"

#include <user/ui/kzu_ui_event_listener.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_manager.h>
#include <user/ui/components/kzu_ui_slider.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/scene_graph/kzu_object.h>

#include <core/memory/kzc_memory_manager.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/debug/kzs_log.h>


kzsError kzuUiComponentCreateButton(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                    const struct KzuUiManager* uiManager, kzString nodeName, struct KzuUiComponentType** out_componentType, 
                                    struct KzuUiComponentNode** out_componentNode)
{
    kzsError result;
    struct KzuUiComponentType* componentType;
    struct KzuUiComponentNode* componentNode;

    result = kzuUiComponentTypeCreate(memoryManager, &componentType);
    kzsErrorForward(result);

    kzuUiComponentTypeSetInitializeCallback(componentType, kzuUiManagerGetInitializeCallback(uiManager, "ButtonInitialize"));
    kzuUiComponentTypeSetLogicUpdateCallback(componentType, kzuUiManagerGetUpdateCallback(uiManager, "ButtonUpdate"));
    kzuUiComponentTypeSetUninitializeCallback(componentType, kzuUiManagerGetUninitializeCallback(uiManager, "ButtonUninitialize"));
    kzuUiComponentTypeSetHandleEventsCallback(componentType, kzuUiManagerGetHandleEventsCallback(uiManager, "ButtonHandleEvents"));

    result = kzuUiComponentNodeCreate(memoryManager, propertyManager, messageDispatcher, nodeName, componentType, &componentNode);
    kzsErrorForward(result);

    result = kzuUiComponentNodeInitialize(componentNode, memoryManager);
    kzsErrorForward(result);

    *out_componentType = componentType;
    *out_componentNode = componentNode;
    kzsSuccess();
}


kzsError kzuUiComponentCreateSlider(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                    const struct KzuUiManager* uiManager, kzString nodeName, struct KzuUiComponentType** out_componentType, 
                                    struct KzuUiComponentNode** out_componentNode,
                                    struct KzuObjectNode** out_knobNode, struct KzuObjectNode** out_railNode)
{
    kzsError result;
    struct KzuUiComponentType* componentType;
    struct KzuUiComponentNode* componentNode;
    struct KzuObjectNode* knobNode;
    struct KzuObjectNode* railNode;

    result = kzuUiComponentTypeCreate(memoryManager, &componentType);
    kzsErrorForward(result);

    kzuUiComponentTypeSetInitializeCallback(componentType, kzuUiManagerGetInitializeCallback(uiManager, "SliderInitialize"));
    kzuUiComponentTypeSetLogicUpdateCallback(componentType, kzuUiManagerGetUpdateCallback(uiManager, "SliderUpdate"));
    kzuUiComponentTypeSetUninitializeCallback(componentType, kzuUiManagerGetUninitializeCallback(uiManager, "SliderUninitialize"));
    kzuUiComponentTypeSetHandleEventsCallback(componentType, kzuUiManagerGetHandleEventsCallback(uiManager, "SliderHandleEvents"));

    result = kzuUiComponentNodeCreate(memoryManager, propertyManager, messageDispatcher, nodeName, componentType, &componentNode);
    kzsErrorForward(result);

    result = kzuEmptyNodeCreate(memoryManager, propertyManager, messageDispatcher, "Rail", &railNode);
    kzsErrorForward(result);
    result = kzuObjectNodeAddChild(kzuUiComponentNodeToObjectNode(componentNode), railNode);
    kzsErrorForward(result);
    result = kzuEmptyNodeCreate(memoryManager, propertyManager, messageDispatcher, "Knob", &knobNode);
    kzsErrorForward(result);
    result = kzuObjectNodeAddChild(kzuUiComponentNodeToObjectNode(componentNode), knobNode);
    kzsErrorForward(result);

    result = kzuUiComponentNodeInitialize(componentNode, memoryManager);
    kzsErrorForward(result);

    *out_componentType = componentType;
    *out_componentNode = componentNode;
    *out_knobNode = knobNode;
    *out_railNode = railNode;
    kzsSuccess();
}

kzsError kzuUiComponentCreateTrajectoryLayout(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                              const struct KzuUiManager* uiManager, kzString nodeName, struct KzcTrajectory* trajectory,
                                              struct KzuUiComponentType** out_componentType, struct KzuUiComponentNode** out_componentNode)
{
    kzsError result;
    struct KzuUiComponentType* componentType;
    struct KzuUiComponentNode* componentNode;
    struct KzuObjectNode* objectNode;

    result = kzuUiComponentTypeCreate(memoryManager, &componentType);
    kzsErrorForward(result);

    kzuUiComponentTypeSetInitializeCallback(componentType, kzuUiManagerGetInitializeCallback(uiManager, "TrajectoryLayoutInitialize"));
    kzuUiComponentTypeSetLogicUpdateCallback(componentType, kzuUiManagerGetUpdateCallback(uiManager, "TrajectoryLayoutUpdate"));
    kzuUiComponentTypeSetUninitializeCallback(componentType, kzuUiManagerGetUninitializeCallback(uiManager, "TrajectoryLayoutUninitialize"));
    kzuUiComponentTypeSetHandleEventsCallback(componentType, kzuUiManagerGetHandleEventsCallback(uiManager, "TrajectoryLayoutHandleEvents"));

    result = kzuUiComponentNodeCreate(memoryManager, propertyManager, messageDispatcher, nodeName, componentType, &componentNode);
    kzsErrorForward(result);

    objectNode = kzuUiComponentNodeToObjectNode(componentNode);

    result = kzuObjectNodeSetVoidProperty(objectNode, KZU_PROPERTY_TYPE_TRAJECTORY, trajectory);
    kzsErrorForward(result);

    result = kzuUiComponentNodeInitialize(componentNode, memoryManager);
    kzsErrorForward(result);

    *out_componentType = componentType;
    *out_componentNode = componentNode;
    kzsSuccess();
}

kzsError kzuUiComponentNodeCreateAndAddEventListener(const struct KzuUiComponentNode* componentNode, const struct KzcMemoryManager* memoryManager, 
                                                     const struct KzuUiManager* uiManager, kzString eventIdentifier, kzString actionIdentifier)
{
    kzsError result;
    struct KzuUiEventListener* eventListener;

    result = kzuUiEventListenerCreate(memoryManager, eventIdentifier, &eventListener);
    kzsErrorForward(result);

    {
        struct KzuUiAction* baseAction = kzuUiManagerGetAction(uiManager, actionIdentifier);

        if (baseAction != KZ_NULL)
        {
            struct KzuUiAction* action;

            result = kzuUiActionCopy(memoryManager, baseAction, &action);
            kzsErrorForward(result);

            kzuUiEventListenerSetAction(eventListener, action);
        }
        else
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, actionIdentifier);
            kzsLog(KZS_LOG_LEVEL_WARNING, "kzuUiComponentCreateAndAddEventListener(): Action with given identifier not found.");
        }
    }

    /* TODO: Implement argument forward. */
    
    result = kzuUiComponentNodeAddEventListener(componentNode, eventIdentifier, eventListener);
    kzsErrorForward(result);

    kzsSuccess();
}
