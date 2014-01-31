/**
* \file
* User interface stack layout component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_STACK_LAYOUT_H
#define KZU_UI_STACK_LAYOUT_H


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


/** Stack layout direction enumeration. */
enum KzuStackLayoutDirection
{
    KZU_STACK_LAYOUT_DIRECTION_X = 0, /**< Layout direction X. */
    KZU_STACK_LAYOUT_DIRECTION_Y = 1, /**< Layout direction Y. */
    KZU_STACK_LAYOUT_DIRECTION_Z = 2  /**< Layout direction Z. */
};


/** Object type identifier for stack layout components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_STACK_LAYOUT;


/** Get the direction of the stack layout. */
enum KzuStackLayoutDirection kzuUiStackLayoutGetDirection(const struct KzuObjectNode* objectNode);
/** Get the reversed property of the stack layout. */
kzBool kzuUiStackLayoutIsReversed(const struct KzuObjectNode* objectNode);

/** Stack layout initialize. */
kzsError kzuUiStackLayoutInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);


#endif
