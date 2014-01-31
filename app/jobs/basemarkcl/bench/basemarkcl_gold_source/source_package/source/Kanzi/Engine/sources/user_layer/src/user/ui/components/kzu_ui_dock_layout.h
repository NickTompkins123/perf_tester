/**
* \file
* User interface dock layout component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_DOCK_LAYOUT_H
#define KZU_UI_DOCK_LAYOUT_H

#include <user/ui/kzu_ui_component_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcHashMap;
struct KzuUiEvent;
struct KzuUiComponentNode;
struct KzuEngineMessageQueue;


/** Object type identifier for dock layout components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_DOCK_LAYOUT;


/** Dock layout initialize. */
kzsError kzuUiDockLayoutInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);


#endif
