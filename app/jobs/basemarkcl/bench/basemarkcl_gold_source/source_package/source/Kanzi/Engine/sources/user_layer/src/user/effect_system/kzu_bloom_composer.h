/**
* \file
* Specifies a bloom composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_BLOOM_COMPOSER_H
#define KZU_BLOOM_COMPOSER_H

#include <user/renderer/kzu_composer_private.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuRenderer;
struct KzuTransformedScene;
struct KzuMaterial;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzsWindow;

/**
 * \struct KzuBloomComposer
 * Bloom composer to achieve bloom effect, inherited from KzuComposer.
 */
struct KzuBloomComposer;


/** Creates new bloom composer. */
kzsError kzuBloomComposerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuBloomComposer** out_bloomComposer);
/** Deletes bloom composer. */
kzsError kzuBloomComposerDelete(struct KzuComposer* composer);
/** Initializes bloom composer.
 * \param renderTargetTextureWidth Texture width for blur target.
 * \param renderTargetTextureHeight Texture height for blur target.
 * \param bloomBlurMaterial Directional blur material, applied twice, once horizontally and once vertically.
 * \param bloomBlitMaterial Bloom blit materials, blits the texture over screen.
 */
kzsError kzuBloomComposerInitialize(const struct KzuBloomComposer* bloomComposer, struct KzcResourceManager* resourceManager,
                                    kzUint renderTargetTextureWidth, kzUint renderTargetTextureHeight, struct KzuMaterial* bloomBlurMaterial, struct KzuMaterial* bloomBlitMaterial);
/** Applies bloom composer for rendering. Override for ComposerApply. */
kzsError kzuBloomComposerApply(const struct KzuComposer* bloomComposer, struct KzuRenderer* renderer,
                                      const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow);


/** Casts bloom composer to composer. */
struct KzuComposer* kzuBloomComposerToComposer(struct KzuBloomComposer* composer);
/** Casts composer to bloom composer. */
struct KzuBloomComposer* kzuBloomComposerFromComposer(const struct KzuComposer* composer);


#endif
