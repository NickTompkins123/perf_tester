/**
* \file
* User interface data.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_MANAGER_H
#define KZU_UI_MANAGER_H


#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_action.h>
#include <user/engine/kzu_engine_message.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>



/* Forward declarations. */
struct KzcMemoryManager;
struct KzcVector3;

/**
 * \struct KzuUiManager
 * Structure for storing user interface default callbacks.
 */
struct KzuUiManager;


/** Allocates memory for UI manager structure. */
kzsError kzuUiManagerCreate(const struct KzcMemoryManager* memoryManager, struct KzuUiManager** out_uiManager);
/** Frees memory allocated for UI manager structure. */
kzsError kzuUiManagerDelete(struct KzuUiManager* uiManager);

/** Gets action from the UI manager. */
struct KzuUiAction* kzuUiManagerGetAction(const struct KzuUiManager* uiManager, kzString actionName);
/** Gets initialize callback from the UI manager. */
KzuUiComponentInitializeCallback kzuUiManagerGetInitializeCallback(const struct KzuUiManager* uiManager, kzString callbackName);
/** Gets uninitialize callback from the UI manager. */
KzuUiComponentUninitializeCallback kzuUiManagerGetUninitializeCallback(const struct KzuUiManager* uiManager, kzString callbackName);
/** Gets update callback from the UI manager. */
KzuUiComponentUpdateCallback kzuUiManagerGetUpdateCallback(const struct KzuUiManager* uiManager, kzString callbackName);
/** Gets handle events callback from the UI manager. */
KzuUiComponentHandleEventsCallback kzuUiManagerGetHandleEventsCallback(const struct KzuUiManager* uiManager, kzString callbackName);

/** Adds new action callback to the UI manager. */
kzsError kzuUiManagerAddActionCallback(const struct KzuUiManager* uiManager, kzString callbackName, KzuUiActionCallback callback, void* userData);
/** Removes action callback from the UI manager. */
kzsError kzuUiManagerRemoveActionCallback(const struct KzuUiManager* uiManager, kzString callbackName);
/** Adds new handle events callback to the UI manager. */
kzsError kzuUiManagerAddHandleEventsCallback(const struct KzuUiManager* uiManager, kzString callbackName, KzuUiComponentHandleEventsCallback callback);
/** Adds new update callback to the UI manager. */
kzsError kzuUiManagerAddUpdateCallback(const struct KzuUiManager* uiManager, kzString callbackName, KzuUiComponentUpdateCallback callback);
/** Adds new uninitialize callback to the UI manager. */
kzsError kzuUiManagerAddUninitializeCallback(const struct KzuUiManager* uiManager, kzString callbackName, KzuUiComponentUninitializeCallback callback);
/** Adds new initialize callback to the UI manager. */
kzsError kzuUiManagerAddInitializeCallback(const struct KzuUiManager* uiManager, kzString callbackName, KzuUiComponentInitializeCallback callback);

/** Adds a message to message queue. */
kzsError kzuUiManagerAddMessageToQueue(const struct KzcMemoryManager* memoryManager, void* userData, 
                                       const enum KzuEngineMessageType type, const struct KzcHashMap* parameters);


#endif
