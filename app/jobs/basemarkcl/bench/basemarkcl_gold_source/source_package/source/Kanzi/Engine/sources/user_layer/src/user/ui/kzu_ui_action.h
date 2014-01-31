/**
* \file
* User interface action.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_ACTION_H
#define KZU_UI_ACTION_H

#include <user/ui/kzu_ui_event.h>

#include <core/util/collection/kzc_hash_map.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzuUiComponentNode;
struct KzuUiEvent;


/** Callback for action. */
typedef kzsError (*KzuUiActionCallback)(const struct KzuUiComponentNode* component, void* userData, struct KzuUiEvent* event, kzString eventType, struct KzcHashMap* parameters);

/**
* \struct KzuUiAction
* Structure containing data for user interface action.
*/
struct KzuUiAction;


/** Creates new user interface action with callback. */
kzsError kzuUiActionCreate(const struct KzcMemoryManager* memoryManager, KzuUiActionCallback actionCallback, 
                           kzBool requiresMessageQueue, void* userData, struct KzuUiAction** out_action);

/** Deletes action. */
kzsError kzuUiActionDelete(struct KzuUiAction* action);

/** Creates a copy of a user interface action. */
kzsError kzuUiActionCopy(const struct KzcMemoryManager* memoryManager, const struct KzuUiAction* sourceAction,
                         struct KzuUiAction** out_action);

/** Returns true if attached action requires message queue as userData */
kzBool kzuUiActionGetRequiresMessageQueue(const struct KzuUiAction* action);

/** Calls given user interface action. */
kzsError kzuUiActionCall(const struct KzuUiAction* action, const struct KzuUiComponentNode* component, void* userData, 
                         kzString eventType, struct KzuUiEvent* event, struct KzcHashMap* customParameters);

/** Gets user data bound to this callback. */
void* kzuUiActionGetUserData(const struct KzuUiAction* action);


#endif
