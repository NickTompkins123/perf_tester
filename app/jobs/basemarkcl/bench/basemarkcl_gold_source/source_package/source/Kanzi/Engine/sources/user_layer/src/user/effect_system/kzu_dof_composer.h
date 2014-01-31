/**
* \file
* Specifies a depth of field composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_DOF_COMPOSER_H
#define KZU_DOF_COMPOSER_H

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
 * \struct KzuDOFComposer
 * DOF composer to achieve depth of field effect, inherited from KzuComposer.
 */
struct KzuDOFComposer;


/** Creates new Depth of Field composer. */
kzsError kzuDOFComposerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuDOFComposer** out_bloomComposer);
/** Deletes Depth of Field composer. */
kzsError kzuDOFComposerDelete(struct KzuComposer* composer);
/** Initializes depth of field composer.
* \param renderTargetTextureWidth Texture width for DOF blur image.
* \param renderTargetTextureHeight Texture width for DOF blur image.
* \param boxBlurMaterial Directional blur material, applied twice, once horizontally and once vertically.
* \param dofDepthMaterial DOF depth material, used in depth pass.
* \param dofBlitMaterial DOF blit material, used to combine screen and blur target by depth.
*/
kzsError kzuDOFComposerInitialize(const struct KzuDOFComposer* dofComposer, struct KzcResourceManager* resourceManager,
                                  kzUint renderTargetTextureWidth, kzUint renderTargetTextureHeight,
                                  struct KzuMaterial* boxBlurMaterial, struct KzuMaterial* dofDepthMaterial,
                                  struct KzuMaterial* dofBlitMaterial);
/** Applies DOF composer for rendering. Override for ComposerApply. */
kzsError kzuDOFComposerApply(const struct KzuComposer* bloomComposer, struct KzuRenderer* renderer,
                               const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow);


/** Casts DOF composer to composer. */
struct KzuComposer* kzuDOFComposerToComposer(struct KzuDOFComposer* composer);
/** Casts composer to bloom composer. */
struct KzuDOFComposer* kzuDOFComposerFromComposer(const struct KzuComposer* composer);


#endif
