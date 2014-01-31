/**
* \file
* User interface label.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_ACTIONS_H
#define KZU_UI_ACTIONS_H

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcHashMap;
struct KzuUiEvent;
struct KzuUiComponentNode;


/** Set property value callback. */
kzsError kzuUiCallbackSetPropertyValue(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                       kzString eventType, struct KzcHashMap* parameters);

/** Switch boolean property callback. */
kzsError kzuUiCallbackSwitchBooleanProperty(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                       kzString eventType, struct KzcHashMap* parameters);

/** Set boolean property callback. */
kzsError kzuUiCallbackSetBooleanProperty(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                            kzString eventType, struct KzcHashMap* parameters);

/** Play animation callback. */
kzsError kzuUiCallbackPlayAnimation(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                    kzString eventType, struct KzcHashMap* parameters);

/** Start transition callback. */
kzsError kzuUiCallbackStartTransition(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                      kzString eventType, struct KzcHashMap* parameters);

/** Change scene callback. */
kzsError kzuUiCallbackChangeScene(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                  kzString eventType, struct KzcHashMap* parameters);

/** Change render pass camera. */
kzsError kzuUiCallbackChangeRenderPassCamera(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                             kzString eventType, struct KzcHashMap* parameters);

/** Change 2d picking camera. */
kzsError kzuUiCallbackChange2DPickingCamera(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                             kzString eventType, struct KzcHashMap* parameters);
/** Set matrix to identity. */
kzsError kzuUiCallbackSetTransformationToIdentity(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                                  kzString eventType, struct KzcHashMap* parameters);

/** Execute script. */
kzsError kzuUiCallbackExecuteScript(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                                  kzString eventType, struct KzcHashMap* parameters);

/** Change scene camera. */
kzsError kzuUiCallbackChangeSceneCamera(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                            kzString eventType, struct KzcHashMap* parameters);

/** Render pass set enabled or disabled. */
kzsError kzuUiCallbackRenderPassSetEnabled(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                        kzString eventType, struct KzcHashMap* parameters);

/** Render pass set toggle enabled. */
kzsError kzuUiCallbackRenderPassToggleEnabled(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                        kzString eventType, struct KzcHashMap* parameters);

/** Render pass render once. */
kzsError kzuUiCallbackRenderPassRenderOnceAndDisable(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                        kzString eventType, struct KzcHashMap* parameters);

/** Change scene composer. */
kzsError kzuUiCallbackChangeComposer(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                     kzString eventType, struct KzcHashMap* parameters);

/** Change texture property. */
kzsError kzuUiCallbackSetTexturePropertyValue(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                     kzString eventType, struct KzcHashMap* parameters);


#endif
