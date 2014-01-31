/**
* \file
* User interface project data.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_manager.h"

#include <user/engine/kzu_engine_message.h>
#include <user/engine/kzu_engine_message_queue.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/scene_graph/kzu_object.h>
#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_action.h>
#include <user/ui/kzu_ui_event.h>
#include <user/ui/components/kzu_ui_actions.h>
#include <user/ui/components/kzu_ui_label.h>
#include <user/ui/components/kzu_ui_button.h>
#include <user/ui/components/kzu_ui_toggle_button.h>
#include <user/ui/components/kzu_ui_dropdown.h>
#include <user/ui/components/kzu_ui_playlist_player.h>
#include <user/ui/components/kzu_ui_slider.h>
#include <user/ui/components/kzu_ui_stack_layout.h>
#include <user/ui/components/kzu_ui_grid_layout.h>
#include <user/ui/components/kzu_ui_dock_layout.h>
#include <user/ui/components/kzu_ui_trajectory_layout.h>
#include <user/ui/components/kzu_ui_viewbox.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/debug/kzc_log.h>


typedef void (*KzuGenericFunctionPointer)(void);


struct KzuUiManagerCallbackWrapper
{
    KzuGenericFunctionPointer callback;
};

struct KzuUiManager
{
    struct KzcHashMap* initializeCallbacks; /** Initialization callbacks. <kzString, KzuUiManagerCallbackWrapper>. */
    struct KzcHashMap* uninitializeCallbacks; /** Uninitialization callbacks. <kzString, KzuUiManagerCallbackWrapper>. */
    struct KzcHashMap* updateCallbacks; /** Update callbacks. <kzString, KzuUiManagerCallbackWrapper>. */
    struct KzcHashMap* handleEventsCallbacks; /** Handle events callbacks. <kzString, KzuUiManagerCallbackWrapper>. */
    struct KzcHashMap* actions; /** Actions. <kzString, KzuUiAction>. */
};


static kzsError kzuUiManagerAddAction_internal(const struct KzuUiManager* uiManager, kzBool engineAction, kzString key, 
                                               KzuUiActionCallback callback, void* userData)
{
    kzsError result;
    struct KzuUiAction* action;

    kzsAssert(kzcIsValidPointer(uiManager));
    kzsErrorTest(!kzcHashMapContains(uiManager->actions, key), KZS_ERROR_ILLEGAL_OPERATION, "An action with the given name already exists");

    result = kzuUiActionCreate(kzcMemoryGetManager(uiManager), callback, engineAction, userData, &action);
    kzsErrorForward(result);

    result = kzcHashMapPut(uiManager->actions, key, action);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuUiManagerCreateDefaultActions_internal(const struct KzuUiManager* uiManager)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(uiManager));

    /* Default engine actions. */
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "PlayAnimation", kzuUiCallbackPlayAnimation, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "ChangeScene", kzuUiCallbackChangeScene, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "SetProperty", kzuUiCallbackSetPropertyValue, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "PlaylistPlayNext", kzuUiCallbackPlaylistPlayerNext, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "PlaylistPlayPrevious", kzuUiCallbackPlaylistPlayerPrevious, KZ_NULL);
    kzsErrorForward(result);    
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "SwitchBooleanProperty", kzuUiCallbackSwitchBooleanProperty, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "SetBooleanProperty", kzuUiCallbackSetBooleanProperty, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "StartTransition", kzuUiCallbackStartTransition, KZ_NULL);
    kzsErrorForward(result);    
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "ChangeCamera", kzuUiCallbackChangeRenderPassCamera, KZ_NULL);
    kzsErrorForward(result);    
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "Change2DPickCamera", kzuUiCallbackChange2DPickingCamera, KZ_NULL);
    kzsErrorForward(result);    
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "SetMatrixToIdentity", kzuUiCallbackSetTransformationToIdentity, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "ExecuteScript", kzuUiCallbackExecuteScript, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "ChangeSceneCamera", kzuUiCallbackChangeSceneCamera, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "RenderPassSetEnabled", kzuUiCallbackRenderPassSetEnabled, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "RenderPassToggleEnabled", kzuUiCallbackRenderPassToggleEnabled, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "RenderPassRenderOneFrameAndDisable", kzuUiCallbackRenderPassRenderOnceAndDisable, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "ChangeComposer", kzuUiCallbackChangeComposer, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "SetTextureProperty", kzuUiCallbackSetTexturePropertyValue, KZ_NULL);
    kzsErrorForward(result);
    
    kzsSuccess();
}

static kzsError kzuUiManagerCreateDefaultContent_internal(const struct KzuUiManager* uiManager)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(uiManager));

    /* Default button callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "ButtonInitialize", kzuUiButtonInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "ButtonUninitialize", kzuUiButtonUninitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "ButtonUpdate", kzuUiButtonLogicUpdate);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "ButtonHandleEvents", kzuUiButtonHandleEvents);
    kzsErrorForward(result);

    /* Default toggle button callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "ToggleButtonInitialize", kzuUiToggleButtonInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "ToggleButtonUninitialize", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "ToggleButtonUpdate", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "ToggleButtonHandleEvents", kzuUiToggleButtonHandleEvents);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "ToggleButtonSwitchState", kzuUiCallbackToggleButtonSwitchState, KZ_NULL);
    kzsErrorForward(result);

    /* Default slider callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "SliderInitialize", kzuUiSliderInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "SliderUninitialize", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "SliderUpdate", kzuUiSliderLogicUpdate);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "SliderHandleEvents", kzuUiSliderHandleEvents);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "SliderSetValue", kzuUiCallbackSetSliderValue, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "SliderIncreaseValue", kzuUiCallbackIncreaseSliderValue, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "SliderDecreaseValue", kzuUiCallbackDecreaseSliderValue, KZ_NULL);
    kzsErrorForward(result);

    /* Default label callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "LabelInitialize", kzuUiLabelInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "LabelUninitialize", kzuUiLabelUninitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "LabelUpdate", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "LabelHandleEvents", KZ_NULL);
    kzsErrorForward(result);

    /* Default dropdown callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "DropdownInitialize", kzuUiDropdownInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "DropdownUninitialize", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "DropdownUpdate", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "DropdownHandleEvents", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "DropdownSetSelectedItem", kzuUiCallbackDropdownSetSelection, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "DropdownOpen", kzuUiCallbackDropdownOpen, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "DropdownClose", kzuUiCallbackDropdownClose, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddAction_internal(uiManager, KZ_TRUE, "DropdownToggleOpen", kzuUiCallbackDropdownToggleOpenState, KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "DropdownLayoutHandleEvents", kzuUiDropdownLayoutHandleEvents);
    kzsErrorForward(result);
    
    /* Default playlist player callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "PlaylistPlayerInitialize", kzuUiPlaylistPlayerInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "PlaylistPlayerUninitialize", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "PlaylistPlayerUpdate", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "PlaylistPlayerHandleEvents", KZ_NULL);
    kzsErrorForward(result);

    /* Default stack layout callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "StackLayoutInitialize", kzuUiStackLayoutInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "StackLayoutUninitialize", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "StackLayoutUpdate", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "StackLayoutHandleEvents", KZ_NULL);
    kzsErrorForward(result);

    /* Default grid layout callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "GridLayoutInitialize", kzuUiGridLayoutInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "GridLayoutUninitialize", kzuUiGridLayoutUninitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "GridLayoutUpdate", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "GridLayoutHandleEvents", KZ_NULL);
    kzsErrorForward(result);

    /* Default dock layout callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "DockLayoutInitialize", kzuUiDockLayoutInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "DockLayoutUninitialize", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "DockLayoutUpdate", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "DockLayoutHandleEvents", KZ_NULL);
    kzsErrorForward(result);

    /* Default trajectory layout callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "TrajectoryLayoutInitialize", kzuUiTrajectoryLayoutInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "TrajectoryLayoutUninitialize", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "TrajectoryLayoutUpdate", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "TrajectoryLayoutHandleEvents", KZ_NULL);
    kzsErrorForward(result);

    /* Default viewbox callbacks. */
    result = kzuUiManagerAddInitializeCallback(uiManager, "ViewBoxInitialize", kzuUiViewBoxInitialize);
    kzsErrorForward(result);
    result = kzuUiManagerAddUninitializeCallback(uiManager, "ViewBoxUninitialize", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddUpdateCallback(uiManager, "ViewBoxUpdate", KZ_NULL);
    kzsErrorForward(result);
    result = kzuUiManagerAddHandleEventsCallback(uiManager, "ViewBoxHandleEvents", KZ_NULL);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiManagerCreate(const struct KzcMemoryManager* memoryManager, struct KzuUiManager** out_uiManager)
{
    kzsError result;
    struct KzuUiManager* uiManager;

    result = kzcMemoryAllocVariable(memoryManager, uiManager, "UI project");
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &uiManager->initializeCallbacks);
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &uiManager->uninitializeCallbacks);
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &uiManager->updateCallbacks);
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &uiManager->handleEventsCallbacks);
    kzsErrorForward(result);

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &uiManager->actions);
    kzsErrorForward(result);
    
    result = kzuUiManagerCreateDefaultActions_internal(uiManager);
    kzsErrorForward(result);

    result = kzuUiManagerCreateDefaultContent_internal(uiManager);
    kzsErrorForward(result);

    *out_uiManager = uiManager;
    kzsSuccess();
}

kzsError kzuUiManagerDelete(struct KzuUiManager* uiManager)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(uiManager));

    /* Delete initialize callbacks. */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(uiManager->initializeCallbacks);
        while (kzcHashMapIterate(it))
        { 
            struct KzuUiManagerCallbackWrapper* callbackWrapper = (struct KzuUiManagerCallbackWrapper*)kzcHashMapIteratorGetValue(it);
            result = kzcMemoryFreeVariable(callbackWrapper);
            kzsErrorForward(result);
        }

        result = kzcHashMapDelete(uiManager->initializeCallbacks);
        kzsErrorForward(result);
    }

    /* Delete uninitialize callbacks. */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(uiManager->uninitializeCallbacks);
        while (kzcHashMapIterate(it))
        { 
            struct KzuUiManagerCallbackWrapper* callbackWrapper = (struct KzuUiManagerCallbackWrapper*)kzcHashMapIteratorGetValue(it);
            result = kzcMemoryFreeVariable(callbackWrapper);
            kzsErrorForward(result);
        }

        result = kzcHashMapDelete(uiManager->uninitializeCallbacks);
        kzsErrorForward(result);
    }

    /* Delete update callbacks. */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(uiManager->updateCallbacks);
        while (kzcHashMapIterate(it))
        { 
            struct KzuUiManagerCallbackWrapper* callbackWrapper = (struct KzuUiManagerCallbackWrapper*)kzcHashMapIteratorGetValue(it);
            result = kzcMemoryFreeVariable(callbackWrapper);
            kzsErrorForward(result);
        }

        result = kzcHashMapDelete(uiManager->updateCallbacks);
        kzsErrorForward(result);
    }

    /* Delete handle event callbacks. */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(uiManager->handleEventsCallbacks);
        while (kzcHashMapIterate(it))
        { 
            struct KzuUiManagerCallbackWrapper* callbackWrapper = (struct KzuUiManagerCallbackWrapper*)kzcHashMapIteratorGetValue(it);
            result = kzcMemoryFreeVariable(callbackWrapper);
            kzsErrorForward(result);
        }

        result = kzcHashMapDelete(uiManager->handleEventsCallbacks);
        kzsErrorForward(result);
    }

    /* Delete actions. */
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(uiManager->actions);
        while (kzcHashMapIterate(it))
        { 
            struct KzuUiAction* action = (struct KzuUiAction*)kzcHashMapIteratorGetValue(it);
            result = kzuUiActionDelete(action);
            kzsErrorForward(result);
        }

        result = kzcHashMapDelete(uiManager->actions);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(uiManager);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuUiAction* kzuUiManagerGetAction(const struct KzuUiManager* uiManager, kzString actionName)
{
    struct KzuUiAction* action;

    kzsAssert(kzcIsValidPointer(uiManager));

    if (!kzcHashMapGet(uiManager->actions, actionName, (void**)&action))
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Requested action was not found"); /* TODO: Print action name here directly when new log function is added. */
        kzsLog(KZS_LOG_LEVEL_WARNING, actionName);
        action = KZ_NULL;
    }

    return action;
}

KzuUiComponentInitializeCallback kzuUiManagerGetInitializeCallback(const struct KzuUiManager* uiManager, kzString callbackName)
{
    struct KzuUiManagerCallbackWrapper* wrapper;
    KzuUiComponentInitializeCallback callback;

    kzsAssert(kzcIsValidPointer(uiManager));

    if (kzcHashMapGet(uiManager->initializeCallbacks, callbackName, (void**)&wrapper))
    {
        callback = (KzuUiComponentInitializeCallback)wrapper->callback;
    }
    else
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Requested initialize callback was not found"); /* TODO: Print callback name here directly when new log function is added. */
        kzsLog(KZS_LOG_LEVEL_WARNING, callbackName);
        callback = KZ_NULL;
    }

    return callback;
}

KzuUiComponentUninitializeCallback kzuUiManagerGetUninitializeCallback(const struct KzuUiManager* uiManager, kzString callbackName)
{
    struct KzuUiManagerCallbackWrapper* wrapper;
    KzuUiComponentUninitializeCallback callback;

    kzsAssert(kzcIsValidPointer(uiManager));

    if (kzcHashMapGet(uiManager->uninitializeCallbacks, callbackName, (void**)&wrapper))
    {
        callback = (KzuUiComponentUninitializeCallback)wrapper->callback;
    }
    else
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Requested uninitialize callback was not found"); /* TODO: Print callback name here directly when new log function is added. */
        kzsLog(KZS_LOG_LEVEL_WARNING, callbackName);
        callback = KZ_NULL;
    }

    return callback;
}

KzuUiComponentUpdateCallback kzuUiManagerGetUpdateCallback(const struct KzuUiManager* uiManager, kzString callbackName)
{
    struct KzuUiManagerCallbackWrapper* wrapper;
    KzuUiComponentUpdateCallback callback;

    kzsAssert(kzcIsValidPointer(uiManager));

    if (kzcHashMapGet(uiManager->updateCallbacks, callbackName, (void**)&wrapper))
    {
        callback = (KzuUiComponentUpdateCallback)wrapper->callback;
    }
    else
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Requested update callback was not found"); /* TODO: Print callback name here directly when new log function is added. */
        kzsLog(KZS_LOG_LEVEL_WARNING, callbackName);
        callback = KZ_NULL;
    }

    return callback;
}

KzuUiComponentHandleEventsCallback kzuUiManagerGetHandleEventsCallback(const struct KzuUiManager* uiManager, kzString callbackName)
{
    struct KzuUiManagerCallbackWrapper* wrapper;
    KzuUiComponentHandleEventsCallback callback;

    kzsAssert(kzcIsValidPointer(uiManager));

    if (kzcHashMapGet(uiManager->handleEventsCallbacks, callbackName, (void**)&wrapper))
    {
        callback = (KzuUiComponentHandleEventsCallback)wrapper->callback;
    }
    else
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Requested handle events callback was not found"); /* TODO: Print callback name here directly when new log function is added. */
        kzsLog(KZS_LOG_LEVEL_WARNING, callbackName);
        callback = KZ_NULL;
    }

    return callback;
}

kzsError kzuUiManagerAddActionCallback(const struct KzuUiManager* uiManager, kzString key, KzuUiActionCallback callback, void* userData)
{
    kzsError result;
    
    result = kzuUiManagerAddAction_internal(uiManager, KZ_FALSE, key, callback, userData);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiManagerRemoveActionCallback(const struct KzuUiManager* uiManager, kzString callbackName)
{
    kzsError result;
    struct KzuUiAction* action;

    kzsAssert(kzcIsValidPointer(uiManager));
    kzsErrorTest(kzcHashMapContains(uiManager->actions, callbackName), KZS_ERROR_ILLEGAL_OPERATION, "An action with the given name doesn't exist");

    if (kzcHashMapGet(uiManager->actions, callbackName, (void**)&action))
    {
        result = kzuUiActionDelete(action);
        kzsErrorForward(result);

        result = kzcHashMapRemove(uiManager->actions, callbackName);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuUiManagerAddInitializeCallback(const struct KzuUiManager* uiManager, kzString key, KzuUiComponentInitializeCallback callback)
{
    kzsError result;
    struct KzuUiManagerCallbackWrapper* wrapper;

    kzsAssert(kzcIsValidPointer(uiManager));

    kzsErrorTest(!kzcHashMapContains(uiManager->initializeCallbacks, key), KZS_ERROR_ILLEGAL_OPERATION,
                 "An initialize callback with the given name already exists");

    result = kzcMemoryAllocVariable(kzcMemoryGetManager(uiManager), wrapper, "Wrapper struct for initialize callback");
    kzsErrorForward(result);

    wrapper->callback = (KzuGenericFunctionPointer)callback;

    result = kzcHashMapPut(uiManager->initializeCallbacks, key, wrapper);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiManagerAddUninitializeCallback(const struct KzuUiManager* uiManager, kzString key, KzuUiComponentUninitializeCallback callback)
{
    kzsError result;
    struct KzuUiManagerCallbackWrapper* wrapper;

    kzsAssert(kzcIsValidPointer(uiManager));

    kzsErrorTest(!kzcHashMapContains(uiManager->uninitializeCallbacks, key), KZS_ERROR_ILLEGAL_OPERATION,
                 "An uninitialize callback with the given name already exists");

    result = kzcMemoryAllocVariable(kzcMemoryGetManager(uiManager), wrapper, "Wrapper struct for uninitialize callback");
    kzsErrorForward(result);

    wrapper->callback = (KzuGenericFunctionPointer)callback;

    result = kzcHashMapPut(uiManager->uninitializeCallbacks, key, wrapper);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiManagerAddUpdateCallback(const struct KzuUiManager* uiManager, kzString key, KzuUiComponentUpdateCallback callback)
{
    kzsError result;
    struct KzuUiManagerCallbackWrapper* wrapper;

    kzsAssert(kzcIsValidPointer(uiManager));

    kzsErrorTest(!kzcHashMapContains(uiManager->updateCallbacks, key), KZS_ERROR_ILLEGAL_OPERATION,
                 "An update callback with the given name already exists");

    result = kzcMemoryAllocVariable(kzcMemoryGetManager(uiManager), wrapper, "Wrapper struct for update callback");
    kzsErrorForward(result);

    wrapper->callback = (KzuGenericFunctionPointer)callback;

    result = kzcHashMapPut(uiManager->updateCallbacks, key, wrapper);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiManagerAddHandleEventsCallback(const struct KzuUiManager* uiManager, kzString key, KzuUiComponentHandleEventsCallback callback)
{
    kzsError result;
    struct KzuUiManagerCallbackWrapper* wrapper;

    kzsAssert(kzcIsValidPointer(uiManager));

    kzsErrorTest(!kzcHashMapContains(uiManager->handleEventsCallbacks, key), KZS_ERROR_ILLEGAL_OPERATION,
                 "A handle events callback with the given name already exists");

    result = kzcMemoryAllocVariable(kzcMemoryGetManager(uiManager), wrapper, "Wrapper struct for handle events callback");
    kzsErrorForward(result);

    wrapper->callback = (KzuGenericFunctionPointer)callback;

    result = kzcHashMapPut(uiManager->handleEventsCallbacks, key, wrapper);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiManagerAddMessageToQueue(const struct KzcMemoryManager* memoryManager, void* userData, 
                                       const enum KzuEngineMessageType type, const struct KzcHashMap* parameters)/*lint -e{818}*/
{
    kzsError result;
    kzUint delay;
    struct KzuEngineMessage* message;
    struct KzuEngineMessageQueue* queue;
    struct KzuEngineMessageQueueHelper helper;
    void* userSetData;

    helper = *(struct KzuEngineMessageQueueHelper*)userData;
    queue = helper.queue;
    delay = helper.delayDuration;
    userSetData = helper.userData;

    kzsAssert(kzcIsValidPointer(queue));

    result = kzuEngineMessageCreate(memoryManager, type, delay, userSetData, &message);
    kzsErrorForward(result);

    if(parameters != KZ_NULL)
    {
        struct KzcHashMapIterator it = kzcHashMapGetIterator(parameters);
        while(kzcHashMapIterate(it))
        {
            kzString name = (kzString)kzcHashMapIteratorGetKey(it);
            void* data = kzcHashMapIteratorGetValue(it);

            result = kzuEngineMessageAddArgument(message, name, data);
            kzsErrorForward(result);
        }
    }

    result = kzuEngineMessageQueueAddItem(queue, message);
    kzsErrorForward(result);

    kzsSuccess();
}
