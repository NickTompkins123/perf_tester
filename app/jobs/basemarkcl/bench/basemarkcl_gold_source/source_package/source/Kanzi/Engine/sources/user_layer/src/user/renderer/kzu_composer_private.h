/**
* \file
* Specifies a composer structure, which is set of render passes that are processed in order.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_COMPOSER_PRIVATE_H
#define KZU_COMPOSER_PRIVATE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuComposer;
struct KzuRenderer;
struct KzuTransformedScene;
struct KzuComposerEffectData;
struct KzcMemoryManager;
struct KzcDynamicArray;
struct KzsWindow;
struct KzuPropertyManager;


/** Defines functions for different object node types. */
struct KzuComposerClass
{
    /** Apply function for the composer */
    kzsError (*kzuComposerApply)(const struct KzuComposer* composer, struct KzuRenderer* renderer,
                                 const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow);
    kzsError (*kzuComposerDelete)(struct KzuComposer* composer);
};


/** Structure for composer. */
struct KzuComposer
{
    struct KzuComposerData* data;
};

struct KzuComposerData
{
    kzBool selfOwned;
    struct KzcDynamicArray* children;   /**< Children of this composer*/
    struct KzuPropertyManager* propertyManager; /** Property manager of this composer. */

    const struct KzuComposerClass* composerClass; /**< Composer class, containing functions needed and specifies the type. */
};


/** Creates a new composer. */
kzsError kzuComposerCreate_private(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuComposerData* composerData,
                                   const struct KzuComposerClass* composerClass, struct KzuComposer** out_composer);
/** Deletes a composer. */
kzsError kzuComposerDelete_private(const struct KzuComposer* composer);


#endif
