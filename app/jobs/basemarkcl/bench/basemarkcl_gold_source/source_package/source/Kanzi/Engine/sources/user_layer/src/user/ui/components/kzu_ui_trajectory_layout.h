/**
* \file
* User interface trajectory layout component.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_TRAJECTORY_LAYOUT_H
#define KZU_UI_TRAJECTORY_LAYOUT_H

#include <user/ui/kzu_ui_component_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcHashMap;
struct KzcTrajectory;
struct KzuUiEvent;
struct KzuUiComponentNode;
struct KzuEngineMessageQueue;


/** Object type identifier for trajectory layout components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_TRAJECTORY_LAYOUT;


/** Trajectory layout initialize. */
kzsError kzuUiTrajectoryLayoutInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);


/** Trajectory layout set offset. The offset value works differently based on the type of used trajectory. */
kzsError kzuUiTrajectoryLayoutSetOffset(const struct KzuUiComponentNode* componentNode, kzFloat offset);
/** Trajectory layout get offset. */
kzFloat kzuUiTrajectoryLayoutGetOffset(const struct KzuUiComponentNode* componentNode);

/** Trajectory layout get attached trajectory. */
struct KzcTrajectory* kzuUiTrajectoryGetTrajectory(const struct KzuUiComponentNode* componentNode);


#endif
