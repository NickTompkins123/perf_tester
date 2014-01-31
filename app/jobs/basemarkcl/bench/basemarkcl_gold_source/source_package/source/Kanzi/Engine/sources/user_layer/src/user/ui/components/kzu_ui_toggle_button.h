/**
* \file
* User interface toggle button component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_TOGGLE_BUTTON_H
#define KZU_UI_TOGGLE_BUTTON_H

#include <user/ui/kzu_ui_component_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcHashMap;
struct KzuUiEvent;
struct KzuUiComponentNode;
struct KzuEngineMessageQueue;


/** Object type identifier for button components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_TOGGLE_BUTTON;

extern const kzString KZU_TOGGLE_BUTTON_ENABLED; /**< Event identifier toggle button becoming enabled. */
extern const kzString KZU_TOGGLE_BUTTON_DISABLED; /**< Event identifier toggle button becoming disabled. */


/** Toggle button initialize. */
kzsError kzuUiToggleButtonInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);

/** Toggle button event handler. */
kzsError kzuUiToggleButtonHandleEvents(const struct KzuUiComponentNode* componentNode, kzString eventType, struct KzuUiEvent* event, 
                                       struct KzcHashMap* customParameters, struct KzuEngineMessageQueue* queue);

/** Action for setting toggle button state on or off. */
kzsError kzuUiToggleButtonSetStateAction(const struct KzuUiComponentNode* componentNode, kzBool newState, void* userData, struct KzuUiEvent* event, 
                                       struct KzuEngineMessageQueue* queue);

/** Sets toggle button on. */
kzsError kzuUiCallbackToggleButtonSwitchState(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                        kzString eventType, struct KzcHashMap* parameters);

/** Sets toggle button state. */
kzsError kzuUiToggleButtonSetState(const struct KzuUiComponentNode* componentNode, kzBool enabled); 
/** Gets toggle button state. */
kzBool kzuUiToggleButtonGetState(const struct KzuUiComponentNode* componentNode);


#endif
