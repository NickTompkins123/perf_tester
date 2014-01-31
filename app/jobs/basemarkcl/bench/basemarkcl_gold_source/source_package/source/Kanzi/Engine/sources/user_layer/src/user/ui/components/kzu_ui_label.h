/**
* \file
* User interface label.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_LABEL_H
#define KZU_UI_LABEL_H

#include <user/ui/kzu_ui_component_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcHashMap;
struct KzuUiEvent;
struct KzuUiComponentNode;
struct KzuEngineMessageQueue;


/** Object type identifier for label components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_LABEL;


/** Label initialize. */
kzsError kzuUiLabelInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);

/** Label uninitialize. */
kzsError kzuUiLabelUninitialize(struct KzuUiComponentNode* componentNode);

/** Label update. */
kzsError kzuUiLabelUpdate(const struct KzuUiComponentNode* componentNode, kzFloat deltaTime);


#endif
