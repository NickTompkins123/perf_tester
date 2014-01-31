/**
* \file
* User interface viewbox component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_VIEWBOX_H
#define KZU_UI_VIEWBOX_H

#include <user/ui/kzu_ui_component_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzuUiComponentNode;


/** Object type identifier for viewbox components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_VIEWBOX;


/** Viewbox initialize. */
kzsError kzuUiViewBoxInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);


#endif
