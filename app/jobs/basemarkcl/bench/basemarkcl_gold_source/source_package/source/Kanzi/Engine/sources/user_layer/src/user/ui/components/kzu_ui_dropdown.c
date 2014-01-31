/**
* \file
* User interface dropdown component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_dropdown.h"

#include <user/ui/kzu_ui_component_type.h>
#include <user/ui/kzu_ui_component_base.h>
#include <user/ui/kzu_ui_component.h>
#include <user/ui/kzu_ui_event.h>
#include <user/ui/kzu_ui_manager.h>
#include <user/scene_graph/kzu_object.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_void_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/engine/kzu_engine_message.h>
#include <user/engine/kzu_engine_message_queue.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>

#include <system/debug/kzs_log.h>

#include "stdio.h"


/** Index of dropdown button. */
#define DROPDOWN_BUTTON_CHILD_INDEX 0
/** Index of dropdown layout. */
#define DROPDOWN_LAYOUT_CHILD_INDEX 1


const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_DROPDOWN = (void*)&KZU_COMPONENT_TYPE_DROPDOWN;


const kzString KZU_DROPDOWN_SELECTED_ITEM_CHANGED = "DropdownSelectedItemChanged";
const kzString KZU_DROPDOWN_OPENED = "DropdownOpened";
const kzString KZU_DROPDOWN_CLOSED = "DropdownClosed";


/* TODO: Rename functions to start with lowercase */
KZ_CALLBACK kzsError kzuUiDropdownInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode)
{
    kzuUiComponentTypeSetTypeIdentifier(kzuUiComponentNodeGetType(componentNode), KZU_COMPONENT_TYPE_DROPDOWN);
    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiDropdownLayoutHandleEvents(const struct KzuUiComponentNode* componentNode, kzString eventType, struct KzuUiEvent* event, 
                                 struct KzcHashMap* customParameters, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuObjectNode* objectNode;
    struct KzuObjectNode* pickedObject = kzuUiEventGetPickedObjectNode(event);
    struct KzuObjectNode* dragStartObject = kzuUiEventGetDragStartPickedObjectNode(event);

    kzsAssert(kzcIsValidPointer(componentNode));

    objectNode = kzuUiComponentNodeToObjectNode(componentNode);

    if (kzcStringIsEqual(eventType, "PointerPressedEvent") && (pickedObject == dragStartObject) && pickedObject != KZ_NULL)
    {
        kzUint index = 0;
        struct KzcDynamicArrayIterator it = kzuObjectNodeGetChildren(objectNode);
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuObjectNode* childNode = (struct KzuObjectNode*)kzcDynamicArrayIteratorGetValue(it);

            kzBool buttonHit = ((pickedObject == childNode) || kzuUiFindChildNode(childNode, pickedObject));
            
            if (buttonHit)
            {
                struct KzuObjectNode* parent = kzuObjectNodeGetParent(objectNode);
                struct KzuUiComponentNode* dropdownNode = kzuUiComponentNodeFromObjectNode(parent);
                kzUint oldSelectionIndex = kzuUiDropdownGetSelectedItem(dropdownNode);
                kzBool valueSet;

                result = kzuUiDropdownSetSelectedItem(dropdownNode, index, &valueSet);
                kzsErrorForward(result);

                if(valueSet)
                {
                    {
                        struct KzuVoidProperty* oldItemProperty;
                        struct KzuObjectNode* oldItem = kzuUiDropdownGetItemAtIndex(dropdownNode, oldSelectionIndex);
                        result = kzuVoidPropertyCreate(kzcMemoryGetManager(event), kzuVoidPropertyTypeFromPropertyType((struct KzuPropertyType*)KZU_PROPERTY_TYPE_DROPDOWN_OLD_SELECTED_ITEM), oldItem, &oldItemProperty);
                        kzsErrorForward(result);
                        result = kzuUiEventAddProperty(event, kzuVoidPropertyToProperty(oldItemProperty));
                        kzsErrorForward(result);
                    }

                    {
                        struct KzuVoidProperty* newItemProperty;
                        struct KzuObjectNode* newItem = kzuUiDropdownGetItemAtIndex(dropdownNode, index);
                        result = kzuVoidPropertyCreate(kzcMemoryGetManager(event), kzuVoidPropertyTypeFromPropertyType((struct KzuPropertyType*)KZU_PROPERTY_TYPE_DROPDOWN_NEW_SELECTED_ITEM), newItem, &newItemProperty);
                        kzsErrorForward(result);
                        result = kzuUiEventAddProperty(event, kzuVoidPropertyToProperty(newItemProperty));
                        kzsErrorForward(result);
                    }

                    result = kzuUiComponentNodeHandleEvents(dropdownNode, event, KZU_DROPDOWN_SELECTED_ITEM_CHANGED, queue);
                    kzsErrorForward(result);
                }

                break;
            }

            index++;
        }
    }

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackDropdownSetSelection(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
    kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_DROPDOWN_SET_SELECTED_ITEM, parameters);
    kzsErrorForward(result); 

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackDropdownOpen(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
    kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_DROPDOWN_OPEN, parameters);
    kzsErrorForward(result); 

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackDropdownClose(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
    kzString eventType, struct KzcHashMap* parameters) 
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_DROPDOWN_CLOSE, parameters);
    kzsErrorForward(result); 

    kzsSuccess();
}

KZ_CALLBACK kzsError kzuUiCallbackDropdownToggleOpenState(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                         kzString eventType, struct KzcHashMap* parameters)
{
    kzsError result;

    result = kzuUiManagerAddMessageToQueue(kzcMemoryGetManager(componentNode), userData, KZU_ENGINE_MESSAGE_DROPDOWN_SWITCH_OPEN_STATE, parameters);
    kzsErrorForward(result); 

    kzsSuccess();
}

kzsError kzuUiDropdownSetSelectedItemAction(const struct KzuUiComponentNode* componentNode, kzInt selectedItemIndex,
                                            void* userData, struct KzuUiEvent* event, struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);
    
    KZ_UNUSED_PARAMETER(userData); /* TODO: Remove this parameter if not used. */

    {
        kzUint oldSelectedItem = (kzUint)kzuObjectNodeGetIntPropertyDefault(node, KZU_PROPERTY_TYPE_DROPDOWN_SELECTED_ITEM);
        kzUint setValue = kzsMinU((kzUint)selectedItemIndex, kzuUiDropdownGetItemCount(componentNode) - 1);

        if(oldSelectedItem != setValue)
        {
            result = kzuObjectNodeSetIntProperty(node, KZU_PROPERTY_TYPE_DROPDOWN_SELECTED_ITEM, (kzInt)setValue);
            kzsErrorForward(result);

            {
                struct KzuVoidProperty* oldItemProperty;
                struct KzuObjectNode* oldItem = kzuUiDropdownGetItemAtIndex(componentNode, oldSelectedItem);
                result = kzuVoidPropertyCreate(kzcMemoryGetManager(event), kzuVoidPropertyTypeFromPropertyType((struct KzuPropertyType*)KZU_PROPERTY_TYPE_DROPDOWN_OLD_SELECTED_ITEM), oldItem, &oldItemProperty);
                kzsErrorForward(result);
                result = kzuUiEventAddProperty(event, kzuVoidPropertyToProperty(oldItemProperty));
                kzsErrorForward(result);
            }

            {
                struct KzuVoidProperty* newItemProperty;
                struct KzuObjectNode* newItem = kzuUiDropdownGetItemAtIndex(componentNode, setValue);
                result = kzuVoidPropertyCreate(kzcMemoryGetManager(event), kzuVoidPropertyTypeFromPropertyType((struct KzuPropertyType*)KZU_PROPERTY_TYPE_DROPDOWN_NEW_SELECTED_ITEM), newItem, &newItemProperty);
                kzsErrorForward(result);
                result = kzuUiEventAddProperty(event, kzuVoidPropertyToProperty(newItemProperty));
                kzsErrorForward(result);
            }

            result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_DROPDOWN_SELECTED_ITEM_CHANGED, queue);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

kzsError kzuUiDropdownSetOpenAction(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
    struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);

    KZ_UNUSED_PARAMETER(userData); /* TODO: Remove this parameter if not needed */

    {
        kzBool oldOpenState = kzuObjectNodeGetBoolPropertyDefault(node, KZU_PROPERTY_TYPE_DROPDOWN_OPEN_STATE);

        if (!oldOpenState)
        {
            result = kzuObjectNodeSetBoolProperty(node, KZU_PROPERTY_TYPE_DROPDOWN_OPEN_STATE, KZ_TRUE);
            kzsErrorForward(result);

            result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_DROPDOWN_OPENED, queue);
            kzsErrorForward(result);
        }
    }
    kzsSuccess();
}

kzsError kzuUiDropdownSetClosedAction(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
    struct KzuEngineMessageQueue* queue)
{
    kzsError result;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);

    KZ_UNUSED_PARAMETER(userData);

    {
        kzBool oldOpenState = kzuObjectNodeGetBoolPropertyDefault(node, KZU_PROPERTY_TYPE_DROPDOWN_OPEN_STATE);

        if (oldOpenState)
        {
            result = kzuObjectNodeSetBoolProperty(node, KZU_PROPERTY_TYPE_DROPDOWN_OPEN_STATE, KZ_FALSE);
            kzsErrorForward(result);

            result = kzuUiComponentNodeHandleEvents(componentNode, event, KZU_DROPDOWN_CLOSED, queue);
            kzsErrorForward(result);
        }
    }
    kzsSuccess();
}

kzBool kzuUiDropdownIsOpen(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);

    return kzuObjectNodeGetBoolPropertyDefault(node, KZU_PROPERTY_TYPE_DROPDOWN_OPEN_STATE);
}

kzUint kzuUiDropdownGetItemCount(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* dropdownNode;
    kzsAssert(kzcIsValidPointer(componentNode));

    dropdownNode = kzuUiDropdownGetLayout(componentNode);

    return kzuObjectNodeGetChildCount(dropdownNode);
}

struct KzuObjectNode* kzuUiDropdownGetItemAtIndex(const struct KzuUiComponentNode* componentNode, kzUint index)
{
    struct KzuObjectNode* dropdownNode;
    struct KzuObjectNode* resultNode;
    kzsAssert(kzcIsValidPointer(componentNode));

    dropdownNode = kzuUiDropdownGetLayout(componentNode);
    resultNode = kzuObjectNodeGetChildAtIndex(dropdownNode, index);

    return resultNode;
}

kzUint kzuUiDropdownGetSelectedItem(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);

    return (kzUint)kzuObjectNodeGetIntPropertyDefault(node, KZU_PROPERTY_TYPE_DROPDOWN_SELECTED_ITEM);
}

kzsError kzuUiDropdownSetSelectedItem(const struct KzuUiComponentNode* componentNode, kzUint index, kzBool* out_set)
{
    kzsError result;
    kzBool set = KZ_FALSE;
    struct KzuObjectNode* node = kzuUiComponentNodeToObjectNode(componentNode);
    kzUint oldValue = kzuUiDropdownGetSelectedItem(componentNode); 
    kzUint setValue = kzsMinU(index, kzuUiDropdownGetItemCount(componentNode) - 1);

    if(oldValue != setValue)
    {
        result = kzuObjectNodeSetIntProperty(node, KZU_PROPERTY_TYPE_DROPDOWN_SELECTED_ITEM, (kzInt)index);
        kzsErrorForward(result);

        set = KZ_TRUE;
    }

    *out_set = set;
    kzsSuccess();
}

struct KzuObjectNode* kzuUiDropdownGetButton(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* node;
    struct KzuObjectNode* buttonNode;

    kzsAssert(kzcIsValidPointer(componentNode));

    node = kzuUiComponentNodeToObjectNode(componentNode);
    buttonNode = kzuObjectNodeGetChildAtIndex(node, DROPDOWN_BUTTON_CHILD_INDEX);
    return buttonNode;
}

struct KzuObjectNode* kzuUiDropdownGetLayout(const struct KzuUiComponentNode* componentNode)
{
    struct KzuObjectNode* node;
    struct KzuObjectNode* layoutNode;

    kzsAssert(kzcIsValidPointer(componentNode));

    node = kzuUiComponentNodeToObjectNode(componentNode);
    layoutNode = kzuObjectNodeGetChildAtIndex(node, DROPDOWN_LAYOUT_CHILD_INDEX);
    return layoutNode;
}
