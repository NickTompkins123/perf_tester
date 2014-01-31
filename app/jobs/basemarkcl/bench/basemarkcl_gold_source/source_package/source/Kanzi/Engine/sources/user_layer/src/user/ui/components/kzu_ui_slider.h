/**
* \file
* User interface slider.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_SLIDER_H
#define KZU_UI_SLIDER_H

#include <user/ui/kzu_ui_component_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcHashMap;
struct KzuUiEvent;
struct KzuUiComponentNode;
struct KzuEngineMessageQueue;


/** Object type identifier for slider components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_SLIDER;

extern const kzString KZU_SLIDER_VALUE_CHANGED; /**< Identifier slider value change. */
extern const kzString KZU_SLIDER_PRESSED; /**< Identifier slider pressed. */
extern const kzString KZU_SLIDER_RELEASED; /**< Identifier slider released. */


/** Slider direction. */
enum KzuUiSliderDirection
{
    KZU_UI_SLIDER_DIRECTION_X = 0, /** Slider direction X. */
    KZU_UI_SLIDER_DIRECTION_Y = 1, /** Slider direction Y. */
    KZU_UI_SLIDER_DIRECTION_Z = 2 /** Slider direction Z. */
};

/** Slider expand direction. */
enum KzuUiSliderExpandDirection
{
    KZU_UI_SLIDER_EXPAND_DIRECTION_X = 0, /** Slider expand direction x. */
    KZU_UI_SLIDER_EXPAND_DIRECTION_Y = 1, /** Slider expand direction y. */
    KZU_UI_SLIDER_EXPAND_DIRECTION_Z = 2, /** Slider expand direction z. */
    KZU_UI_SLIDER_EXPAND_DIRECTION_ONLY_SLIDER_DIRECTION = 3, /** Slider expand only in direction of sliders movement. */
    KZU_UI_SLIDER_EXPAND_DIRECTION_NONE = 4 /** Slider does not expand on drag. */
};


/** Initializes slider. */
kzsError kzuUiSliderInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);

/** Updates slider. */
kzsError kzuUiSliderLogicUpdate(const struct KzuUiComponentNode* componentNode, kzFloat deltaTime, struct KzuEngineMessageQueue* queue);

/** Sets slider value action callback. */
kzsError kzuUiSliderSetValueAction(const struct KzuUiComponentNode* componentNode, kzFloat value, void* userData, struct KzuUiEvent* event, 
                             struct KzuEngineMessageQueue* queue);

/** Set slider value callback. */
kzsError kzuUiCallbackSetSliderValue(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                      kzString eventType, struct KzcHashMap* parameters);
/** Increase slider value callback. */
kzsError kzuUiCallbackIncreaseSliderValue(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                          kzString eventType, struct KzcHashMap* parameters);
/** Decrease slider value callback. */
kzsError kzuUiCallbackDecreaseSliderValue(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                          kzString eventType, struct KzcHashMap* parameters);

/** Slider event handler. */
kzsError kzuUiSliderHandleEvents(const struct KzuUiComponentNode* componentNode, kzString eventType, 
                                 struct KzuUiEvent* event, struct KzcHashMap* customParameters, struct KzuEngineMessageQueue* queue);

/** Gets the empty object node under which the slider knob must be placed. */
struct KzuObjectNode* kzuUiSliderGetKnob(const struct KzuUiComponentNode* componentNode);

/** Gets the empty object node under which the slider rail must be placed. */
struct KzuObjectNode* kzuUiSliderGetBar(const struct KzuUiComponentNode* componentNode); 

/** Gets slider value. */
kzFloat kzuUiSliderGetValue(const struct KzuUiComponentNode* componentNode);
/** Sets slider value. */
kzsError kzuUiSliderSetValue(const struct KzuUiComponentNode* componentNode, kzFloat value); 

/** Gets slider expand direction on drag. This is the hidden axis where the objects collision box is expanded when user drags slider. */
enum KzuUiSliderExpandDirection kzuUiSliderGetExpandOnDragDirection(const struct KzuUiComponentNode* componentNode);
/** Sets slider expand direction on drag. This is the hidden axis where the objects collision box is expanded when user drags slider. */
kzsError kzuUiSliderSetExpandOnDragDirection(const struct KzuUiComponentNode* componentNode, enum KzuUiSliderExpandDirection direction); 

/** Gets slider direction. */
enum KzuUiSliderDirection kzuUiSliderGetDirection(const struct KzuUiComponentNode* componentNode);
/** Sets slider direction. */
kzsError kzuUiSliderSetDirection(const struct KzuUiComponentNode* componentNode, enum KzuUiSliderDirection direction); 


#endif
