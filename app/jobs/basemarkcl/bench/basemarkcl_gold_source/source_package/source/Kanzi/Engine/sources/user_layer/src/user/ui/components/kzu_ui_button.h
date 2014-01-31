/**
* \file
* User interface button component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_BUTTON_H
#define KZU_UI_BUTTON_H

#include <user/ui/kzu_ui_component_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcHashMap;
struct KzuObjectNode;
struct KzuUiEvent;
struct KzuUiComponentNode;
struct KzuEngineMessageQueue;


extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_BUTTON; /**< Object type identifier for button components. */
extern const kzString KZU_UI_BUTTON_PRESSED_EVENT_STRING; /**< Button pressed event name. */
extern const kzString KZU_UI_BUTTON_LONG_RELEASED_EVENT_STRING; /**< Button long released event name. */
extern const kzString KZU_UI_BUTTON_RELEASED_EVENT_STRING; /**< Button released event name. */
extern const kzString KZU_UI_BUTTON_POINTER_ENTER_EVENT_STRING; /**< Pointer entered event name. */
extern const kzString KZU_UI_BUTTON_POINTER_LEAVE_EVENT_STRING; /**< Pointer left event name. */


/** Get the long release interval property of a button. */
kzFloat kzuUiButtonGetLongReleaseInterval(const struct KzuObjectNode* button);
/** Get the repeat interval property of a button. */
kzFloat kzuUiButtonGetRepeatInterval(const struct KzuObjectNode* button);

/** Button initialize. */
kzsError kzuUiButtonInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);
/** Button uninitialize. */
kzsError kzuUiButtonUninitialize(struct KzuUiComponentNode* componentNode);

/** Button event handler. */
kzsError kzuUiButtonHandleEvents(const struct KzuUiComponentNode* componentNode, kzString eventType, struct KzuUiEvent* event, 
                                 struct KzcHashMap* customParameters, struct KzuEngineMessageQueue* queue);
/** Button logic handler. */
kzsError kzuUiButtonLogicUpdate(const struct KzuUiComponentNode* componentNode, kzFloat deltaTime, struct KzuEngineMessageQueue* queue);


#endif
