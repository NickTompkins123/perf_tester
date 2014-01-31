/**
* \file
* Specifies a stereoscopic composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_STEREOSCOPIC_COMPOSER_H
#define KZU_STEREOSCOPIC_COMPOSER_H

#include <user/renderer/kzu_composer_private.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations. */
struct KzuRenderer;
struct KzuMaterial;
struct KzuTransformedScene;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzsWindow;

/**
 * \struct KzuStereoscopicComposer
 * Stereoscopic rendering composer, inherited from KzuComposer.
 */
struct KzuStereoscopicComposer;


/** Creates new stereoscopic composer. */
kzsError kzuStereoscopicComposerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                       struct KzuStereoscopicComposer** out_stereoscopicComposer);
/** Deletes steroscopic composer. */
kzsError kzuStereoscopicComposerDelete(struct KzuComposer* composer);
/** Initializes stereoscopic composer. Only for loading anaglyphic material (if anaglyphic rendering mode is enabled).
* \param anaglyphicMaterial Anaglyphic material, for forcing red / cyan write only.
*/
void kzuStereoscopicComposerInitialize(const struct KzuStereoscopicComposer* stereoscopicComposer, struct KzcResourceManager* resourceManager,
                                       struct KzuMaterial* anaglyphicMaterial);
/** Applies stereoscopic composer for rendering. Override for ComposerApply. */
struct KzuPropertyManager* kzuStereoscopicComposerGetPropertyManager(const struct KzuStereoscopicComposer* stereoscopicComposer);

kzsError kzuStereoscopicComposerApply(const struct KzuComposer* renderPassComposer, struct KzuRenderer* renderer,
                                      const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow);


/** Casts stereoscopic composer to composer. */
struct KzuComposer* kzuStereoscopicComposerToComposer(struct KzuStereoscopicComposer* composer);
/** Casts composer to stereoscopic composer. */
struct KzuStereoscopicComposer* kzuStereoscopicComposerFromComposer(const struct KzuComposer* composer);


#endif
