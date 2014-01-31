/**
 * \file
 * Application framework events.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZA_APPLICATION_EVENTS_H
#define KZA_APPLICATION_EVENTS_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzaApplication;
struct KzuCameraNode;
struct KzuTransition;
struct KzuScene;


/** Starts transition effect. */
kzsError kzaApplicationStartTransition(struct KzaApplication* application, struct KzuTransition* transition, 
                                       kzUint duration, struct KzuScene* sourceScene, struct KzuScene* targetScene,
                                       struct KzuCameraNode* cameraNode);

/** Handles events in queue for application to handle. */
kzsError kzaApplicationHandleEventQueue(struct KzaApplication* application, kzUint deltaTime);


#endif
