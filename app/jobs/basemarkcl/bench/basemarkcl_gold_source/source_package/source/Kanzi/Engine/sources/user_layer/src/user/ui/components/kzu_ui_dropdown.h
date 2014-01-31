/**
* \file
* User interface dropdown component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_DROPDOWN_H
#define KZU_UI_DROPDOWN_H

#include <user/ui/kzu_ui_component_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcHashMap;
struct KzuUiEvent;
struct KzuUiComponentNode;
struct KzuEngineMessageQueue;


/** Object type identifier for dropdown components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_DROPDOWN;


extern const kzString KZU_DROPDOWN_SELECTED_ITEM_CHANGED; /**< Identifier for dropdown item selected event. */
extern const kzString KZU_DROPDOWN_OPENED; /**< Identifier for dropdown open event. */
extern const kzString KZU_DROPDOWN_CLOSED; /**< Identifier for dropdown closed event. */


/** Dropdown initialize. */
kzsError kzuUiDropdownInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);

/** Dropdown layout event handler. */
kzsError kzuUiDropdownLayoutHandleEvents(const struct KzuUiComponentNode* componentNode, kzString eventType, struct KzuUiEvent* event, 
                                         struct KzcHashMap* customParameters, struct KzuEngineMessageQueue* queue);

/** Set dropdown selected value callback. */
kzsError kzuUiCallbackDropdownSetSelection(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                           kzString eventType, struct KzcHashMap* parameters); /*lint -e{818}*/
/** Dropdown open. */
kzsError kzuUiCallbackDropdownOpen(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                   kzString eventType, struct KzcHashMap* parameters); /*lint -e{818}*/
/** Dropdown close. */
kzsError kzuUiCallbackDropdownClose(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                    kzString eventType, struct KzcHashMap* parameters); /*lint -e{818}*/
/** Dropdown change state. */
kzsError kzuUiCallbackDropdownToggleOpenState(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                    kzString eventType, struct KzcHashMap* parameters); /*lint -e{818}*/

/** Dropdown set selected value action. */
kzsError kzuUiDropdownSetSelectedItemAction(const struct KzuUiComponentNode* componentNode, kzInt selectedItemIndex, void* userData, struct KzuUiEvent* event, 
    struct KzuEngineMessageQueue* queue);

/** Dropdown set open action. */
kzsError kzuUiDropdownSetOpenAction(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                    struct KzuEngineMessageQueue* queue);
/** Dropdown set closed action. */
kzsError kzuUiDropdownSetClosedAction(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                      struct KzuEngineMessageQueue* queue);

/** Gets the button object node which represents the button which opens the dropdown. */
struct KzuObjectNode* kzuUiDropdownGetButton(const struct KzuUiComponentNode* componentNode);
/** Gets the stack layout under which the items are located. */
struct KzuObjectNode* kzuUiDropdownGetLayout(const struct KzuUiComponentNode* componentNode); 

/** Sets dropdown selected item index. out_set is set to KZ_TRUE if selection was changed. Selection is silently clamped to valid range. */
kzsError kzuUiDropdownSetSelectedItem(const struct KzuUiComponentNode* componentNode, kzUint index, kzBool* out_set);
/** Gets dropdown selected item index. */
kzUint kzuUiDropdownGetSelectedItem(const struct KzuUiComponentNode* componentNode);

/** Gets dropdown item count. */
kzUint kzuUiDropdownGetItemCount(const struct KzuUiComponentNode* componentNode);
/** Gets item at given index in dropdown. */
struct KzuObjectNode* kzuUiDropdownGetItemAtIndex(const struct KzuUiComponentNode* componentNode, kzUint index); 

/** Gets dropdown open / close state. */
kzBool kzuUiDropdownIsOpen(const struct KzuUiComponentNode* componentNode);



#endif
