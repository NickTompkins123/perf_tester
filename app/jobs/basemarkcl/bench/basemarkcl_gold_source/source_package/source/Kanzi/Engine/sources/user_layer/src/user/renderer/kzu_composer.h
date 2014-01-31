/**
* \file
* Specifies a composer structure, which is set of render passes that are processed in order.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_COMPOSER_H
#define KZU_COMPOSER_H


#include "kzu_composer_base.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declaration structures needed to implement composer. */
struct KzuRenderer;
struct KzuTransformedScene;
struct KzuObjectSourceRuntimeData;
struct KzuPropertyType;
struct KzuProperty;
struct KzcTexture;
struct KzcMemoryManager;
struct KzcDynamicArray;
struct KzcResourceManager;
struct KzsWindow;
struct KzuPropertyManager;


/** 
 * \struct KzuComposer
 * Composer combines set of render passes which are rendered sequentially.
 */
struct KzuComposer;


/** Creates a new composer object. */
kzsError kzuComposerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuComposer** out_composer);
/** Deletes a composer. */
kzsError kzuComposerDelete(struct KzuComposer* composer);

/**
 * Takes ownership of the composer container deletion. After this, when a composer is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzuComposerTakeOwnership(const struct KzuComposer* composer);
/** Checks if the composer is valid (pointer and the data are non-null). */
kzBool kzuComposerIsValid(const struct KzuComposer* composer);
/** Moves the data from source composer to the target composer. */
void kzuComposerTransferData(struct KzuComposer* targetComposer, struct KzuComposer* sourceComposer);


/** Returns composer's type */
KzuComposerType kzuComposerGetType(const struct KzuComposer* composer);

/** Get the property manager of a composer. */
struct KzuPropertyManager* kzuComposerGetPropertyManager(const struct KzuComposer* composer);

/** Adds a child to composer. */
kzsError kzuComposerAddChild(const struct KzuComposer* composer, struct KzuComposer* childComposer);
/** Removes a child from composer. */
kzsError kzuComposerRemoveChild(const struct KzuComposer* composer, const struct KzuComposer* childComposer);
/** Gets render passes from composer. */
struct KzcDynamicArray* kzuComposerGetChildren(const struct KzuComposer* composer);

/** Gets a child composer from composer at given index. If index is out of range KZ_NULL is returned. */
struct KzuComposer* kzuComposerGetChildComposer(const struct KzuComposer* composer, kzUint index);
/** Returns the amount of render passes in composer. */
kzUint kzuComposerGetChildCount(const struct KzuComposer* composer);

/** Applies a composer for rendering. */
kzsError kzuComposerApply(const struct KzuComposer* composer, struct KzuRenderer* renderer,
                          const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow);

/** Iterates composer children. */
kzsError kzuComposerIterateChildren(const struct KzuComposer* composer, struct KzuRenderer* renderer,
                                    const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow);


#endif
