/**
* \file
* Input manager.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_INPUT_MANAGER_H
#define KZU_INPUT_MANAGER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzuObjectSource;
struct KzuTransformedScene;
struct KzuCameraNode;
struct KzuRenderer;
struct KzcMemoryManager;
struct KzsWindow;
struct KzsInputEvent;


/** Input manager. */
struct KzuInputManager;


/** Create an input manager. */
kzsError kzuInputManagerCreate(const struct KzcMemoryManager* memoryManager, struct KzuInputManager** out_inputManager);
/** Delete an input manager. */
kzsError kzuInputManagerDelete(struct KzuInputManager* inputManager);


/** Process an event from the low level input system. */
kzsError kzuInputManagerProcessInputEvent(const struct KzsInputEvent* inputEvent,
                                          const struct KzuRenderer* renderer, const struct KzsWindow* window,
                                          const struct KzuTransformedScene* transformedScene, const struct KzuCameraNode* cameraNode);

#endif
