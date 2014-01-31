/**
* \file
* User interface screen.
* Screen is the root of a graphical user interface for single scene.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_SCREEN_H
#define KZU_UI_SCREEN_H

#include <user/ui/kzu_ui_screen.h>

#include <system/input/kzs_input.h>
#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuUiEvent;
struct KzuUiComponentNode;
struct KzuRenderer;
struct KzuCameraNode;
struct KzuObjectNode;
struct KzuProject;
struct KzuScene;
struct KzuTransformedScene;
struct KzuObjectSource;
struct KzuEngineMessageQueue;
struct KzcMemoryManager;
struct KzsWindow;


/**
 * \struct KzuUiScreen
 * Structure containing graphical user interface screen data.
 */
struct KzuUiScreen;


/** Creates the screen. */
kzsError kzuUiScreenCreate(const struct KzcMemoryManager* memoryManager, struct KzuUiScreen** out_screen);
/** Deletes screen. */
kzsError kzuUiScreenDelete(struct KzuUiScreen* screen);


/** Sets the default object source used for picking. Can also be KZ_NULL, which uses root as the default object source. */
void kzuUiScreenSetObjectSource(struct KzuUiScreen* screen, struct KzuObjectSource* objectSource);
/** Sets picking camera for this screen. By default the default camera is used. */
void kzuUiScreenSetPickingCamera(struct KzuUiScreen* screen, struct KzuCameraNode* pickingCamera);
/** Sets 2D picking camera for this screen. By default the 2D picking is disabled. */
void kzuUiScreenSet2DPickingCamera(struct KzuUiScreen* screen, struct KzuCameraNode* pickingCamera);
/** Enables or disables mouse picking from this screen. */
void kzuUiScreenEnablePicking(struct KzuUiScreen* screen, kzBool value);
/** Returns the state of picking. True if enabled. */
kzBool kzuUiScreenGetPickingEnabled(const struct KzuUiScreen* screen);


/** Handles key events. */
void kzuUiScreenHandleKeyEvents(const struct KzuUiScreen* screen, enum KzsInputKey keyCode, enum KzsInputKeyDeviceState keyState);
/** Handles pointer events. Bitmask of pointing device buttons, defined as KZS_POINTING_DEVICE_BUTTON_ */
void kzuUiScreenHandlePointerEvents(const struct KzuUiScreen* screen, kzU32 buttons, enum KzsInputPointingDeviceState state, kzInt mouseX, kzInt mouseY);


/** Handles events and informs all elements in the hierarchy of this screen. */
kzsError kzuUiScreenHandleEvents(struct KzuUiScreen* screen, const struct KzuObjectNode* root,
                                 const struct KzuTransformedScene* transformedScene,
                                 const struct KzuRenderer* renderer, const struct KzsWindow* window,
                                 struct KzuEngineMessageQueue* queue);

/** Updates logic. */
kzsError kzuUiScreenLogicUpdate(const struct KzuUiScreen* screen, const struct KzuObjectNode* root, kzFloat deltaTime, struct KzuEngineMessageQueue* queue);


/** Sets the new focused control. */
void kzuUiScreenSetFocusedControl(struct KzuUiScreen* screen, struct KzuUiComponentNode* componentNode);
/** Returns the focused element. */
struct KzuUiComponentNode* kzuUiScreenGetFocusedControl(const struct KzuUiScreen* screen);
/** Changes the focus to the next element and returns the new focused element. */
kzsError kzuUiScreenFocusNextControl(struct KzuUiScreen* screen);
/** Changes the focus to the previous element and returns the new focused element. */
kzsError kzuUiScreenFocusPreviousControl(struct KzuUiScreen* screen);


/** Reset screen. */
kzsError kzuUiScreenReset(const struct KzuUiScreen* screen);

/** Gets the event structure from screen. */
struct KzuUiEvent* kzuUiScreenGetEvent(const struct KzuUiScreen* screen);


#endif
