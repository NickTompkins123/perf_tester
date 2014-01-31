/**
* \file
* User interface events.
* Events are used to deliver pointer and key events from application to GUI framework.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_EVENT_H
#define KZU_UI_EVENT_H


#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_ray.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>


/** Button identifier. */
enum KzuUiEventButton
{
    KZU_UI_EVENT_BUTTON_1,         /**< Button 1 for UI. */
    KZU_UI_EVENT_BUTTON_2          /**< Button 2 for UI. */
};


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcRay;
struct KzcVector3;
struct KzuProperty;
struct KzuPropertyCollection;


/**
 * \struct KzuUiEvent
 * Structure for delivering graphical user interface events.
 */
struct KzuUiEvent;


/** Allocates memory for UI event structure. */
kzsError kzuUiEventCreate(const struct KzcMemoryManager* memoryManager, struct KzuUiEvent** out_uiEvent);
/** Frees memory allocated for UI event structure. */
kzsError kzuUiEventDelete(struct KzuUiEvent* uiEvent);


/** Resets GUI event structure data. */
kzsError kzuUiEventResetEvent(struct KzuUiEvent* uiEvent);
/** Resets GUI event structure. Called at end of each frame. */
kzsError kzuUiEventResetFrame(struct KzuUiEvent* uiEvent);


/** Returns true if pointer is pressed. */
kzBool kzuUiEventIsPointerPressed(const struct KzuUiEvent* uiEvent);
/** Returns true if pointer is released. */
kzBool kzuUiEventIsPointerReleased(const struct KzuUiEvent* uiEvent);
/** Returns true if pointer is moved. */
kzBool kzuUiEventIsPointerMoved(const struct KzuUiEvent* uiEvent);
/** Returns true if pointer is dragged. */
kzBool kzuUiEventIsPointerDragged(const struct KzuUiEvent* uiEvent);
/** Returns pointer position x. */
kzInt kzuUiEventGetPointerX(const struct KzuUiEvent* uiEvent);
/** Returns pointer position y. */
kzInt kzuUiEventGetPointerY(const struct KzuUiEvent* uiEvent);
/** Returns x position where pointer was pressed. */
kzInt kzuUiEventGetPointerDragStartX(const struct KzuUiEvent* uiEvent);
/** Returns y position where pointer was pressed. */
kzInt kzuUiEventGetPointerDragStartY(const struct KzuUiEvent* uiEvent);


/** Pointer was pressed. */
void kzuUiEventPointerPressed(struct KzuUiEvent* uiEvent, enum KzuUiEventButton button, kzInt x, kzInt y);
/** Pointer released. */
void kzuUiEventPointerReleased(struct KzuUiEvent* uiEvent, enum KzuUiEventButton button, kzInt x, kzInt y);
/** Pointer moved. */
void kzuUiEventPointerMoved(struct KzuUiEvent* uiEvent, kzInt x, kzInt y);
/** Pointer dragged. */
void kzuUiEventPointerDragged(struct KzuUiEvent* uiEvent, enum KzuUiEventButton button, kzInt x, kzInt y);


/** UI event key pressed. */
void kzuUiEventKeyPressed(struct KzuUiEvent* uiEvent, enum KzsInputKey key);
/** UI event key released. */
void kzuUiEventKeyReleased(struct KzuUiEvent* uiEvent, enum KzsInputKey key);
/** Internal function for left and right reset. */
void kzuUiEventResetLeftAndRight_private(struct KzuUiEvent* uiEvent);
/** Gets left key pressed status */
kzBool kzuUiEventIsLeftPressed(const struct KzuUiEvent* uiEvent);
/** Gets right key pressed status */
kzBool kzuUiEventIsRightPressed(const struct KzuUiEvent* uiEvent);
/** Gets tabulator key pressed status */
kzBool kzuUiEventIsTabPressed(const struct KzuUiEvent* uiEvent);
/** Gets action key pressed status */
kzBool kzuUiEventIsActionPressed(const struct KzuUiEvent* uiEvent);
/** Resets action key pressed status */
void kzuUiEventResetActionPressed_private(struct KzuUiEvent* uiEvent);
/** Gets shift key pressed status */
kzBool kzuUiEventIsShiftPressed(const struct KzuUiEvent* uiEvent);


/** Returns the object node stored in this event. */
struct KzuObjectNode* kzuUiEventGetPickedObjectNode(const struct KzuUiEvent* uiEvent);
/** Sets the object stored with this event hit. */
void kzuUiEventSetPickedObjectNode(struct KzuUiEvent* uiEvent, struct KzuObjectNode* node);

/** Sets the offset from hit position to the origin of the stored object. Used to find the pointer to object ray position in objects local space. */
void kzuUiEventSetPickedObjectNodeOffset(struct KzuUiEvent* uiEvent, const struct KzcVector3* offset);
/** Gets the offset from hit position to the origin of the stored object. */
struct KzcVector3 kzuUiEventGetPickedObjectNodeOffset(const struct KzuUiEvent* uiEvent);

/** Returns the object node stored in this event. This object was hit when drag started. */
struct KzuObjectNode* kzuUiEventGetDragStartPickedObjectNode(const struct KzuUiEvent* uiEvent);

/** Sets the picking ray used to find the last item. */
void kzuUiEventSetStoredPickingRay(struct KzuUiEvent* uiEvent, const struct KzcRay* ray);
/** Gets the picking ray used to find the last item. */
struct KzcRay kzuUiEventGetStoredPickingRay(const struct KzuUiEvent* uiEvent);

/** Get property collection from event. */
struct KzuPropertyCollection* kzuUiEventGetEventPropertyCollection(const struct KzuUiEvent* uiEvent);
/** Adds a property to property collection. */
kzsError kzuUiEventAddProperty(const struct KzuUiEvent* uiEvent, struct KzuProperty* property);


#endif
