/**
* \file
* User interface play list player.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_UI_PLAYLIST_PLAYER_H
#define KZU_UI_PLAYLIST_PLAYER_H

#include <user/ui/kzu_ui_component_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;
struct KzcHashMap;
struct KzuUiEvent;
struct KzuUiComponentNode;
struct KzuEngineMessageQueue;
struct KzuStringProperty;


/** Object type identifier for label components. */
extern const KzuComponentTypeIdentifier KZU_COMPONENT_TYPE_PLAYLIST_PLAYER;


/** Starts playing next item of the player. */
kzsError kzuUiCallbackPlaylistPlayerNext(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                    kzString eventType, struct KzcHashMap* parameters);

/** Starts playing previous item of the player. */
kzsError kzuUiCallbackPlaylistPlayerPrevious(const struct KzuUiComponentNode* componentNode, void* userData, struct KzuUiEvent* event, 
                                         kzString eventType, struct KzcHashMap* parameters);

/** Initializes playlist player. */
kzsError kzuUiPlaylistPlayerInitialize(const struct KzcMemoryManager* memoryManager, struct KzuUiComponentNode* componentNode);

/** Returns the animation count from the player.*/
kzUint kzuUiPlaylistPlayerGetAnimationCount(const struct KzuUiComponentNode* componentNode);

/** Returns the last played animation id.*/
kzUint kzuUiPlaylistPlayerGetLastAnimationId(const struct KzuUiComponentNode* componentNode);

/** Changes current animation id. Index is silently clamped to valid range. */
kzsError kzuUiPlaylistPlayerSetAnimationId(const struct KzuUiComponentNode* componentNode, kzUint index);


/** Gets project path of animation (clip or sequence) to be played with previous action. */
kzsError kzuUiPlaylistPlayerGetNext(const struct KzuUiComponentNode* componentNode, kzMutableString* out_path);
/** Gets project path of animation (clip or sequence) to be played with previous action. */
kzsError kzuUiPlaylistPlayerGetPrevious(const struct KzuUiComponentNode* componentNode, kzMutableString* out_path);


#endif
