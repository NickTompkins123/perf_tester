/**
* \file
* User interface events functionality.
* Events are used to deliver pointer and key events from application to gui framework.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_ui_event.h"

#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_property.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_ray.h>

#include <system/debug/kzs_log.h>


struct KzuUiEvent
{
    kzBool pointerPressed;  /**< Pointer was pressed. */
    kzBool pointerMoved;    /**< Pointer has moved. */
    kzBool pointerDragged;  /**< Pointer was dragged. */
    kzBool pointerReleased; /**< Pointer was released. */
    kzInt pointerX;         /**< Horizontal position of the pointer. */
    kzInt pointerY;         /**< Vertical position of the pointer. */

    struct KzcRay pickingRay; /**< Ray used to find current picked object. */

    struct KzuObjectNode* hitObjectNode; /**< Object found with ray picking. */
    struct KzcVector3 hitObjectNodeOffset; /**< Offset from objects origin to the hit position. */

    kzBool dragStartObjectSet; /** Set to true if drag start object is set. */
    struct KzuObjectNode* dragStartHitObjectNode; /**< Object found with ray picking when drag started. */

    kzInt pointerDragStartX;   /**< Horizontal position where user pressed the mouse button down. */
    kzInt pointerDragStartY;   /**< Vertical position where user pressed the mouse button down. */

    kzBool actionPressed;   /**< Action is pressed. */
    kzBool shiftPressed;    /**< Shift is pressed. */
    kzBool tabPressed;      /**< Tabulator is pressed. */
    kzBool leftPressed;     /**< Left key is pressed. */
    kzBool rightPressed;    /**< Right key is pressed. */

    struct KzuPropertyCollection* properties; /**< Properties attached to event. */
    /* TODO: Make most of the variables here properties and add to propertycollection. Also allow binding those in tool. */
};


kzsError kzuUiEventCreate(const struct KzcMemoryManager* memoryManager, struct KzuUiEvent** out_uiEvent)
{
    kzsError result;
    struct KzuUiEvent* uiEvent;

    result = kzcMemoryAllocVariable(memoryManager, uiEvent, "GUI screen event struct");
    kzsErrorForward(result);

    result = kzuPropertyCollectionCreate(memoryManager, &uiEvent->properties);
    kzsErrorForward(result);

    result = kzuUiEventResetEvent(uiEvent);
    kzsErrorForward(result);

    *out_uiEvent = uiEvent;
    kzsSuccess();
}

kzsError kzuUiEventDelete(struct KzuUiEvent* uiEvent)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(uiEvent));

    result = kzuPropertyCollectionDelete(uiEvent->properties);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(uiEvent);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuUiEventResetFrame(struct KzuUiEvent* uiEvent)
{
    kzsError result;
    struct KzcDynamicArrayMutableIterator it;
    kzsAssert(kzcIsValidPointer(uiEvent));

    it = kzuPropertyCollectionGetMutableIterator(uiEvent->properties);
    while (kzcDynamicArrayMutableIterate(it))
    {
        struct KzuProperty* property = (struct KzuProperty*)kzcDynamicArrayMutableIteratorGetValue(it);

        result = kzcDynamicArrayMutableIteratorRemove(it);
        kzsErrorForward(result);

        result = kzuPropertyDelete(property);
        kzsErrorForward(result);
    }

    uiEvent->pointerMoved = KZ_FALSE;
    uiEvent->pointerDragged = KZ_FALSE;
    uiEvent->pointerReleased = KZ_FALSE;

    uiEvent->actionPressed = KZ_FALSE;
    uiEvent->tabPressed = KZ_FALSE;  
    kzsSuccess();
}

kzsError kzuUiEventResetEvent(struct KzuUiEvent* uiEvent)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(uiEvent));
    result = kzuUiEventResetFrame(uiEvent);
    kzsErrorForward(result);

    uiEvent->pointerPressed = KZ_FALSE;
    uiEvent->pointerReleased = KZ_FALSE;
    uiEvent->pointerDragStartX = 0;
    uiEvent->pointerDragStartY = 0;
    uiEvent->pointerX = 0;
    uiEvent->pointerY = 0;

    uiEvent->leftPressed = KZ_FALSE;
    uiEvent->rightPressed = KZ_FALSE;
    uiEvent->shiftPressed = KZ_FALSE;

    uiEvent->hitObjectNode = KZ_NULL;
    uiEvent->dragStartObjectSet = KZ_FALSE;
    uiEvent->dragStartHitObjectNode = KZ_NULL;
    
    kzcRaySet(kzcVector3(0.0f, 0.0f, 0.0f), kzcVector3(0.0f, 0.0f, 1.0f), &uiEvent->pickingRay);

    kzsSuccess();
}

kzBool kzuUiEventIsPointerPressed(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->pointerPressed;
}

kzBool kzuUiEventIsPointerReleased(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->pointerReleased;
}

kzBool kzuUiEventIsPointerMoved(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->pointerMoved;
}

kzBool kzuUiEventIsPointerDragged(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->pointerDragged;
}

kzInt kzuUiEventGetPointerX(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->pointerX;
}

kzInt kzuUiEventGetPointerY(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->pointerY;
}

kzInt kzuUiEventGetPointerDragStartX(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->pointerDragStartX;
}

kzInt kzuUiEventGetPointerDragStartY(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->pointerDragStartY;
}

struct KzuObjectNode* kzuUiEventGetPickedObjectNode(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->hitObjectNode;
}

void kzuUiEventSetPickedObjectNode(struct KzuUiEvent* uiEvent, struct KzuObjectNode* objectNode)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    uiEvent->hitObjectNode = objectNode;
    if(!uiEvent->dragStartObjectSet && uiEvent->pointerPressed)
    {
        uiEvent->dragStartHitObjectNode = objectNode;
        uiEvent->dragStartObjectSet = KZ_TRUE;
    }
}

struct KzuObjectNode* kzuUiEventGetDragStartPickedObjectNode(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->dragStartHitObjectNode;
}

void kzuUiEventSetPickedObjectNodeOffset(struct KzuUiEvent* uiEvent, const struct KzcVector3* offset)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    uiEvent->hitObjectNodeOffset = *offset;
}

struct KzcVector3 kzuUiEventGetPickedObjectNodeOffset(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->hitObjectNodeOffset;
}

void kzuUiEventPointerPressed(struct KzuUiEvent* uiEvent, enum KzuUiEventButton button, kzInt x, kzInt y)
{
    KZ_UNUSED_PARAMETER(button);

    kzsAssert(kzcIsValidPointer(uiEvent));

    /* Store starting position for drag. */
    if(!uiEvent->pointerPressed)
    {
        uiEvent->pointerDragStartX = x;
        uiEvent->pointerDragStartY = y;
    }

    uiEvent->pointerX = x;
    uiEvent->pointerY = y;
    uiEvent->pointerPressed = KZ_TRUE;
}

void kzuUiEventPointerReleased(struct KzuUiEvent* uiEvent, enum KzuUiEventButton button, kzInt x, kzInt y)
{
    KZ_UNUSED_PARAMETER(button);

    kzsAssert(kzcIsValidPointer(uiEvent));

    uiEvent->pointerX = x;
    uiEvent->pointerY = y;
    uiEvent->pointerPressed = KZ_FALSE;
    uiEvent->pointerReleased = KZ_TRUE;
    uiEvent->pointerMoved = KZ_FALSE;

    uiEvent->dragStartObjectSet = KZ_FALSE;
    uiEvent->dragStartHitObjectNode = KZ_NULL;
}

void kzuUiEventPointerMoved(struct KzuUiEvent* uiEvent, kzInt x, kzInt y)
{
    kzsAssert(kzcIsValidPointer(uiEvent));

    uiEvent->pointerMoved = KZ_TRUE;
    uiEvent->pointerX = x;
    uiEvent->pointerY = y;
}

void kzuUiEventPointerDragged(struct KzuUiEvent* uiEvent, enum KzuUiEventButton button, kzInt x, kzInt y)
{
    KZ_UNUSED_PARAMETER(button);

    kzsAssert(kzcIsValidPointer(uiEvent));

    uiEvent->pointerDragged = KZ_TRUE;
    uiEvent->pointerX = x;
    uiEvent->pointerY = y;
}

void kzuUiEventKeyPressed(struct KzuUiEvent* uiEvent, enum KzsInputKey key)
{  
    /* Shift */
    if(key == KZS_KEY_MODIFIER1) 
    {
        uiEvent->shiftPressed = KZ_TRUE;
    }
    /* Tab */
    if(key == KZS_KEY_TAB)
    {
        uiEvent->tabPressed = KZ_TRUE;
    }
    /* Left / Down */
    if(key == KZS_KEY_ARROW_LEFT || key == KZS_KEY_ARROW_DOWN)
    {
        uiEvent->leftPressed = KZ_TRUE;
    }
    /* Right / Up */
    if(key == KZS_KEY_ARROW_RIGHT || key == KZS_KEY_ARROW_UP)
    {
        uiEvent->rightPressed = KZ_TRUE;
    }
    if(key == KZS_KEY_SPACE || key == KZS_KEY_ENTER)
    {
        uiEvent->actionPressed = KZ_TRUE;
    }
}

void kzuUiEventKeyReleased(struct KzuUiEvent* uiEvent, enum KzsInputKey key)
{

    if(key == KZS_KEY_MODIFIER1)
    {
        uiEvent->shiftPressed = KZ_FALSE;
    }

    if(key == KZS_KEY_TAB)
    {
        uiEvent->tabPressed = KZ_FALSE;
    }

    if(key == KZS_KEY_ARROW_LEFT || key == KZS_KEY_ARROW_DOWN)
    {
        uiEvent->leftPressed = KZ_FALSE;
    }

    if(key == KZS_KEY_ARROW_RIGHT || key == KZS_KEY_ARROW_UP)
    {
        uiEvent->rightPressed = KZ_FALSE;
    }

    if(key == KZS_KEY_SPACE || key == KZS_KEY_ENTER)
    {
        uiEvent->actionPressed = KZ_FALSE;
    }
}
void kzuUiEventResetLeftAndRight_private(struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    uiEvent->leftPressed = KZ_FALSE;
    uiEvent->rightPressed = KZ_FALSE;
}
kzBool kzuUiEventIsLeftPressed(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->leftPressed;
}
kzBool kzuUiEventIsRightPressed(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->rightPressed;
}
kzBool kzuUiEventIsTabPressed(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->tabPressed;
}
kzBool kzuUiEventIsActionPressed(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->actionPressed;
}

void kzuUiEventResetActionPressed_private(struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    uiEvent->actionPressed = KZ_FALSE;
}
kzBool kzuUiEventIsShiftPressed(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->shiftPressed;
}

void kzuUiEventSetStoredPickingRay(struct KzuUiEvent* uiEvent, const struct KzcRay* ray)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    uiEvent->pickingRay = *ray;
}

struct KzcRay kzuUiEventGetStoredPickingRay(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->pickingRay;
}

struct KzuPropertyCollection* kzuUiEventGetEventPropertyCollection(const struct KzuUiEvent* uiEvent)
{
    kzsAssert(kzcIsValidPointer(uiEvent));
    return uiEvent->properties;
}

kzsError kzuUiEventAddProperty(const struct KzuUiEvent* uiEvent, struct KzuProperty* property)
{
    kzsError result;

    result = kzuPropertyCollectionAddProperty(uiEvent->properties, property);
    kzsErrorForward(result);

    kzsSuccess();
}
