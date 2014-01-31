/**
* \file
* Specifies a color adjustment composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_COLOR_ADJUSTMENT_COMPOSER_H
#define KZU_COLOR_ADJUSTMENT_COMPOSER_H

#include <user/renderer/kzu_composer_private.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuMaterial;
struct KzuRenderer;
struct KzuTransformedScene;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzsWindow;

/**
 * \struct KzuColorAdjustmentComposer
 * Color adjustment composer to achieve HSL, brightness, contrast and gamma adjustments, inherited from KzuComposer.
 */
struct KzuColorAdjustmentComposer;


/** Creates new Color Adjustment composer. */
kzsError kzuColorAdjustmentComposerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuColorAdjustmentComposer** out_colorAdjustmentComposer);
/** Deletes color adjustment composer. */
kzsError kzuColorAdjustmentComposerDelete(struct KzuComposer* composer);
/** Initializes color adjustment composer.
* \param renderTargetTextureWidth Texture width for color adjustment target. Should be fetch from window size.
* \param renderTargetTextureHeight Texture height for color adjustment target. Should be fetch from window size.
* \param colorAdjustmentMaterial Color adjustment material, that modifies the input values by HSL, brightness, contrast and gamma correction.
*/
kzsError kzuColorAdjustmentComposerInitialize(const struct KzuColorAdjustmentComposer* colorAdjustmentComposer, struct KzcResourceManager* resourceManager,
                                              kzUint renderTargetTextureWidth, kzUint renderTargetTextureHeight, struct KzuMaterial* colorAdjustmentMaterial);
/** Applies color adjustment composer for rendering. Override for ComposerApply. */
kzsError kzuColorAdjustmentComposerApply(const struct KzuComposer* colorAdjustmentComposer, struct KzuRenderer* renderer,
                                      const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow);


/** Casts color adjustment composer to composer. */
struct KzuComposer* kzuColorAdjustmentComposerToComposer(struct KzuColorAdjustmentComposer* composer);
/** Casts composer to color adjustment composer. */
struct KzuColorAdjustmentComposer* kzuColorAdjustmentComposerFromComposer(const struct KzuComposer* composer);


#endif
