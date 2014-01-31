/**
* \file
* Specifies a composer structure, which is set of child composers that are processed in order.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_composer.h"

#include "kzu_composer_private.h"

#include <user/renderer/kzu_renderer.h>
#include <user/renderer/kzu_renderer_util.h>
#include <user/filter/kzu_object_source.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_query.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/scene_graph/kzu_transformed_object.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/frame_buffer/kzc_resource_frame_buffer.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>

#include <system/debug/kzs_log.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_opengl_base.h>


/** Internal function for composer delete. */
static kzsError kzuComposerComposerDelete_internal(struct KzuComposer* composer);


const struct KzuComposerClass KZU_COMPOSER_COMPOSER_CLASS =
{
    KZ_NULL,
    kzuComposerComposerDelete_internal
};


kzsError kzuComposerCreate_private(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuComposerData* composerData,
                                   const struct KzuComposerClass* composerClass, struct KzuComposer** out_composer)
{
    kzsError result;
    struct KzuComposer* composer;

    result = kzcMemoryAllocVariable(memoryManager, composer, "Composer");
    kzsErrorForward(result);

    composer->data = composerData;
    composerData->selfOwned = KZ_TRUE;
    composerData->propertyManager = propertyManager;
    composerData->composerClass = composerClass;

    /* Allocate memory for list of render passes. */
    result = kzcDynamicArrayCreate(memoryManager, &composerData->children);
    kzsErrorForward(result);

    *out_composer = composer;
    kzsSuccess();
}

kzsError kzuComposerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuComposer** out_composer)
{
    kzsError result;
    struct KzuComposer* composer;
    struct KzuComposerData* composerData;

    result = kzcMemoryAllocVariable(memoryManager, composerData, "Composer data");
    kzsErrorForward(result);

    result = kzuComposerCreate_private(memoryManager, propertyManager, composerData, &KZU_COMPOSER_COMPOSER_CLASS, &composer);
    kzsErrorForward(result);

    *out_composer = composer;
    kzsSuccess();
}

kzsError kzuComposerDelete_private(const struct KzuComposer* composer)
{
    kzsError result;
    struct KzuComposerData* composerData;

    kzsAssert(kzuComposerIsValid(composer));

    composerData = composer->data;

    result = kzuPropertyManagerRemoveProperties(kzuComposerGetPropertyManager(composer), composer);
    kzsErrorForward(result);

    result = kzcDynamicArrayDelete(composerData->children);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(composerData);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuComposerComposerDelete_internal(struct KzuComposer* composer)
{
    kzsError result;
    kzBool selfOwned;
    selfOwned = composer->data->selfOwned;

    result = kzuComposerDelete_private(composer);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(composer);
        kzsErrorForward(result);
    }
    else
    {
        composer->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuComposerDelete(struct KzuComposer* composer)
{
    kzsError result;

    if(composer->data->composerClass->kzuComposerDelete != KZ_NULL)
    {
        result = composer->data->composerClass->kzuComposerDelete(composer);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

void kzuComposerTakeOwnership(const struct KzuComposer* composer)
{
    kzsAssert(kzuComposerIsValid(composer));

    composer->data->selfOwned = KZ_FALSE;
}

kzBool kzuComposerIsValid(const struct KzuComposer* composer)
{
    return composer != KZ_NULL && composer->data != KZ_NULL;
}

void kzuComposerTransferData(struct KzuComposer* targetComposer, struct KzuComposer* sourceComposer)
{
    kzsAssert(kzcIsValidPointer(targetComposer));
    kzsAssert(targetComposer->data == KZ_NULL);
    kzsAssert(kzuComposerIsValid(sourceComposer));

    targetComposer->data = sourceComposer->data;
    sourceComposer->data = KZ_NULL;
}

KzuComposerType kzuComposerGetType(const struct KzuComposer* composer)
{
    kzsAssert(kzuComposerIsValid(composer));

    return (KzuComposerType)composer->data->composerClass;
}

struct KzuPropertyManager* kzuComposerGetPropertyManager(const struct KzuComposer* composer)
{
    kzsAssert(kzuComposerIsValid(composer));

    return composer->data->propertyManager;
}

kzsError kzuComposerAddChild(const struct KzuComposer* composer, struct KzuComposer*  childComposer)
{
    kzsError result;

    kzsAssert(kzuComposerIsValid(composer));
    kzsAssert(kzcIsValidPointer(childComposer));
    
    result = kzcDynamicArrayAdd(composer->data->children, childComposer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuComposerRemoveChild(const struct KzuComposer* composer, const struct KzuComposer* childComposer)
{
    kzsError result;
    kzsAssert(kzuComposerIsValid(composer));
    kzsAssert(kzcIsValidPointer(childComposer));

    result = kzcDynamicArrayRemove(composer->data->children, childComposer);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcDynamicArray* kzuComposerGetChildren(const struct KzuComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));

    return composer->data->children;
}

struct KzuComposer* kzuComposerGetChildComposer(const struct KzuComposer* composer, kzUint index)
{
    struct KzuComposer* childComposer;
    kzsAssert(kzcIsValidPointer(composer));

    if(index < kzcDynamicArrayGetSize(composer->data->children))
    {
        childComposer = kzcDynamicArrayGet(composer->data->children, index);
    }
    else
    {
        childComposer = KZ_NULL;
    }
    return childComposer;
}

kzUint kzuComposerGetChildCount(const struct KzuComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));

    return kzcDynamicArrayGetSize(composer->data->children);
}

kzsError kzuComposerIterateChildren(const struct KzuComposer* composer, struct KzuRenderer* renderer,
                                    const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;

    kzsAssert(kzuComposerIsValid(composer));

    it = kzcDynamicArrayGetIterator(composer->data->children);
    /* Iterate the render passes */
    while (kzcDynamicArrayIterate(it))
    {
        struct KzuComposer* childComposer = (struct KzuComposer*)kzcDynamicArrayIteratorGetValue(it);

        result = kzuComposerApply(childComposer, renderer, transformedScene, activeWindow);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuComposerApply(const struct KzuComposer* composer, struct KzuRenderer* renderer,
                          const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow)
{
    kzsError result;

    kzsAssert(kzuComposerIsValid(composer));

    /* Push active properties to engine. */
    result = kzuPropertyQueryPushObject(kzuRendererGetPropertyQuery(renderer), composer);
    kzsErrorForward(result);

    if(composer->data->composerClass->kzuComposerApply != KZ_NULL)
    {
        result = composer->data->composerClass->kzuComposerApply(composer, renderer, transformedScene, activeWindow);
        kzsErrorForward(result);
    }
    else
    {
        /* Iterate the render passes */
        result = kzuComposerIterateChildren(composer, renderer, transformedScene, activeWindow);
        kzsErrorForward(result);
    }

    /* Pop active properties from engine. */
    result = kzuPropertyQueryPopObject(kzuRendererGetPropertyQuery(renderer));
    kzsErrorForward(result);

    kzsSuccess();
}
