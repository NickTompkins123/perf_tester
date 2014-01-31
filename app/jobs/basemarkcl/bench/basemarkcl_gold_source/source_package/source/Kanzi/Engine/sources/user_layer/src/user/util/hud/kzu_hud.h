/**
* \file
* Heads up display for displaying debug information.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_HUD_H
#define KZU_HUD_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzuRenderer;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcBitmapFontSystem;
struct KzsWindow;

/**
* \struct KzuHud
* Structure to hold heads up display content.
*/
struct KzuHud;


/** Create a new hud. */
kzsError kzuHudCreate(const struct KzcMemoryManager* memoryManager, struct KzuHud** out_hud);
/** Initialize resources in hud. */
kzsError kzuHudCreateResources(struct KzuHud* hud, struct KzcResourceManager* resourceManager, struct KzcBitmapFontSystem* fontSystem);
/** Delete a hud context. */
kzsError kzuHudDelete(struct KzuHud* hud);

/** Updates hud. Should be called once per rendered frame. */
kzsError kzuHudUpdate(struct KzuHud* hud, kzUint fps, kzUint batchCount, kzUint triangleCount, kzUint textureSwitchCount, kzUint shaderSwitchCount,
                      kzString viewCameraPath, kzFloat animationPlayerTime);
/** Adds custom text line to HUD. Buffer of custom text lines is cleared after each kzuHudUpdate. */
kzsError kzuHudAddCustomTextLine(struct KzuHud* hud, kzString customString);

/** Render the hud. */
kzsError kzuHudRenderDebugInfo(const struct KzuHud* hud, struct KzuRenderer* renderer, const struct KzcMemoryManager* quickMemoryManager, const struct KzsWindow* window);

/** Render the log. */
kzsError kzuHudRenderLog(const struct KzuHud* hud, const struct KzuRenderer* renderer, const struct KzcMemoryManager* quickMemoryManager, const struct KzsWindow* window);

/** Gets the default font from hud. */
struct KzcFont* kzuHudGetFont(const struct KzuHud* hud);


#endif
