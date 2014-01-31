/**
* \file
* Graphical user interface component generator.
* Provides helpers for generating common 3D user interface content.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_COMPONENT_CREATOR_H
#define KZU_UI_COMPONENT_CREATOR_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuUiComponentNode;
struct KzuUiComponentType;
struct KzuObjectNode;
struct KzuPropertyManager;
struct KzuMessageDispatcher;
struct KzuUiManager;
struct KzcMemoryManager;
struct KzcTrajectory;


/** 
* Creates a UI button node, component type and component. 
* The button mesh should be placed under componentNode and should have pickable property attached.
*/
kzsError kzuUiComponentCreateButton(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                    const struct KzuUiManager* uiManager, kzString nodeName, struct KzuUiComponentType** out_componentType, 
                                    struct KzuUiComponentNode** out_componentNode);


/** 
* Creates a UI slider node and required node hierarchy, component type and component. 
* The rail of the slider should be placed under railNode and the knob used to indicate value should be under knobNode.
*/
kzsError kzuUiComponentCreateSlider(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                    const struct KzuUiManager* uiManager, kzString nodeName, struct KzuUiComponentType** out_componentType, 
                                    struct KzuUiComponentNode** out_componentNode,
                                    struct KzuObjectNode** out_knobNode, struct KzuObjectNode** out_railNode);

/** 
* Creates a trajectory layout.
*/
kzsError kzuUiComponentCreateTrajectoryLayout(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                              const struct KzuUiManager* uiManager, kzString nodeName, struct KzcTrajectory* trajectory,
                                              struct KzuUiComponentType** out_componentType, struct KzuUiComponentNode** out_componentNode);


/** Adds a engine action to defined event. */
kzsError kzuUiComponentNodeCreateAndAddEventListener(const struct KzuUiComponentNode* componentNode, const struct KzcMemoryManager* memoryManager, 
                                                     const struct KzuUiManager* uiManager, kzString eventIdentifier, kzString actionIdentifier);


#endif
