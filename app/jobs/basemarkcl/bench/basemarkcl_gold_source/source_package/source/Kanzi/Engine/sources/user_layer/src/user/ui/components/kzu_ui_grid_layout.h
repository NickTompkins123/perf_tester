/**
* \file
* User interface grid layout component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_GRID_LAYOUT_H
#define KZU_UI_GRID_LAYOUT_H


#include <user/scene_graph/kzu_object.h>
#include <user/ui/kzu_ui_component_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcHashMap;
struct KzuUiEvent;
struct KzuUiComponentNode;
struct KzuEngineMessageQueue;


/** Object type identifier for grid layout components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_GRID_LAYOUT;


/** Grid layout initialize. */
kzsError kzuUiGridLayoutInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);

/** Grid layout uninitialize. */
kzsError kzuUiGridLayoutUninitialize(struct KzuUiComponentNode* componentNode);

/** Get the number of rows. */
kzUint kzuUiGridLayoutGetRowCount(const struct KzuUiComponentNode* componentNode);

/** Get the number of columns. */
kzUint kzuUiGridLayoutGetColumnCount(const struct KzuUiComponentNode* componentNode);

/** Get the calculated size of a row. */
kzFloat kzuUiGridLayoutGetActualRowSize(const struct KzuUiComponentNode* componentNode, kzUint rowIndex);

/** Get the calculated size of a column. */
kzFloat kzuUiGridLayoutGetActualColumnSize(const struct KzuUiComponentNode* componentNode, kzUint columnIndex);

/** Set a row for an object in a grid. */
kzsError kzuUiGridLayoutSetObjectRow(const struct KzuObjectNode* objectNode, kzUint row);

/** Get a row for an object in a grid. */
kzUint kzuUiGridLayoutGetObjectRow(const struct KzuObjectNode* objectNode);

/** Set row span for an object in a grid. */
kzsError kzuUiGridLayoutSetObjectRowSpan(const struct KzuObjectNode* objectNode, kzUint rowSpan);

/** Get row span for an object in a grid. */
kzUint kzuUiGridLayoutGetObjectRowSpan(const struct KzuObjectNode* objectNode);

/** Set a column for an object in a grid. */
kzsError kzuUiGridLayoutSetObjectColumn(const struct KzuObjectNode* objectNode, kzUint column);

/** Get a column for an object in a grid. */
kzUint kzuUiGridLayoutGetObjectColumn(const struct KzuObjectNode* objectNode);

/** Set column span for an object in a grid. */
kzsError kzuUiGridLayoutSetObjectColumnSpan(const struct KzuObjectNode* objectNode, kzUint columnSpan);

/** Get column span for an object in a grid. */
kzUint kzuUiGridLayoutGetObjectColumnSpan(const struct KzuObjectNode* objectNode);

#endif
