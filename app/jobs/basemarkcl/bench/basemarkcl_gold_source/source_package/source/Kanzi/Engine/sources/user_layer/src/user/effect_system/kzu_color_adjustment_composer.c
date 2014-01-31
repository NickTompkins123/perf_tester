/**
* \file
* Specifies a color adjustment composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_color_adjustment_composer.h"


#include <user/material/kzu_material.h>
#include <user/renderer/kzu_composer.h>
#include <user/renderer/kzu_renderer.h>
#include <user/renderer/kzu_renderer_util.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/renderer/kzc_renderer.h>
#include <core/util/math/kzc_vector3.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/frame_buffer/kzc_resource_frame_buffer.h>

#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_opengl_base.h>


const struct KzuComposerClass KZU_COMPOSER_COLOR_ADJUST_CLASS =
{
    kzuColorAdjustmentComposerApply,
    kzuColorAdjustmentComposerDelete
};


struct KzuColorAdjustmentComposer
{
    struct KzuComposer composer;
};

struct KzuColorAdjustmentComposerData
{
    struct KzuComposerData composerData; /*lint -esym(754, KzuColorAdjustmentComposerData::composerData)*/

    struct KzcTexture* screenContentsTexture;
    struct KzuMaterial* colorAdjustmentMaterial;
    struct KzcResourceManager* resourceManager;
};

#define kzuColorAdjustmentComposerGetData_private(composer_param) ((struct KzuColorAdjustmentComposerData*)composer_param->composer.data)


kzsError kzuColorAdjustmentComposerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuColorAdjustmentComposer** out_colorAdjustmentComposer)
{
    kzsError result;
    struct KzuColorAdjustmentComposer* colorAdjustmentComposer;
    struct KzuColorAdjustmentComposerData* colorAdjustmentComposerData;
    struct KzuComposer* composer;

    result = kzcMemoryAllocVariable(memoryManager, colorAdjustmentComposerData, "ColorAdjustment composer data");
    kzsErrorForward(result);

    result = kzuComposerCreate_private(memoryManager, propertyManager, (struct KzuComposerData*)colorAdjustmentComposerData, 
        &KZU_COMPOSER_COLOR_ADJUST_CLASS, &composer);
    kzsErrorForward(result);
    colorAdjustmentComposer = (struct KzuColorAdjustmentComposer*)composer;

    colorAdjustmentComposerData->screenContentsTexture  = KZ_NULL;
    colorAdjustmentComposerData->colorAdjustmentMaterial  = KZ_NULL;

    *out_colorAdjustmentComposer = colorAdjustmentComposer;
    kzsSuccess();
}

kzsError kzuColorAdjustmentComposerDelete(struct KzuComposer* composer)
{
    kzsError result;
    struct KzuColorAdjustmentComposerData* colorAdjustmentComposerData;
    struct KzuColorAdjustmentComposer* colorAdjustmentComposer;
    kzBool selfOwned;

    colorAdjustmentComposer = kzuColorAdjustmentComposerFromComposer(composer);
    kzsAssert(colorAdjustmentComposer != KZ_NULL);

    colorAdjustmentComposerData = kzuColorAdjustmentComposerGetData_private(colorAdjustmentComposer);
    selfOwned = composer->data->selfOwned;

    if(colorAdjustmentComposerData->screenContentsTexture != KZ_NULL)
    {
        result = kzcTextureDelete(colorAdjustmentComposerData->screenContentsTexture);
        kzsErrorForward(result);
        colorAdjustmentComposerData->screenContentsTexture = KZ_NULL;
    }

    result = kzuComposerDelete_private((struct KzuComposer*)colorAdjustmentComposer);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(colorAdjustmentComposer);
        kzsErrorForward(result);
    }
    else
    {
        composer->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuColorAdjustmentComposerInitialize(const struct KzuColorAdjustmentComposer* colorAdjustmentComposer, struct KzcResourceManager* resourceManager,
                                               kzUint renderTargetTextureWidth, kzUint renderTargetTextureHeight, struct KzuMaterial* colorAdjustmentMaterial)
{
    kzsError result;
    struct KzuColorAdjustmentComposerData* colorAdjustmentComposerData;

    kzsAssert(colorAdjustmentComposer != KZ_NULL);
    colorAdjustmentComposerData = kzuColorAdjustmentComposerGetData_private(colorAdjustmentComposer);

    if(renderTargetTextureWidth != 0 && renderTargetTextureHeight != 0)
    {
        if(colorAdjustmentComposerData->screenContentsTexture != KZ_NULL)
        {
            result = kzcTextureDelete(colorAdjustmentComposerData->screenContentsTexture);
            kzsErrorForward(result);
            colorAdjustmentComposerData->screenContentsTexture = KZ_NULL;
        }

        result = kzcTextureCreateFrameBufferTexture(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGBA,
            renderTargetTextureWidth, renderTargetTextureHeight, KZC_TEXTURE_FILTER_BILINEAR, &colorAdjustmentComposerData->screenContentsTexture);
        kzsErrorForward(result);
    }
    colorAdjustmentComposerData->resourceManager = resourceManager;
    colorAdjustmentComposerData->colorAdjustmentMaterial = colorAdjustmentMaterial;

    kzsSuccess();
}

kzsError kzuColorAdjustmentComposerApply(const struct KzuComposer* colorAdjustmentComposer, struct KzuRenderer* renderer,
                               const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow)
{
    kzsError result;
    struct KzuColorAdjustmentComposerData* colorAdjustmentComposerData;
    struct KzuColorAdjustmentComposer* composer;
    struct KzuPropertyManager* propertyManager = kzuComposerGetPropertyManager(colorAdjustmentComposer);

    kzsAssert(colorAdjustmentComposer != KZ_NULL);
    composer = kzuColorAdjustmentComposerFromComposer(colorAdjustmentComposer);
    kzsAssert(composer != KZ_NULL);
    colorAdjustmentComposerData = kzuColorAdjustmentComposerGetData_private(composer);

    if(colorAdjustmentComposerData->screenContentsTexture == KZ_NULL ||
        kzcTextureGetWidth(colorAdjustmentComposerData->screenContentsTexture) != kzsWindowGetWidth(activeWindow) ||
        kzcTextureGetHeight(colorAdjustmentComposerData->screenContentsTexture) != kzsWindowGetHeight(activeWindow))
    {
        result = kzcRendererReset(kzuRendererGetCoreRenderer(renderer));
        kzsErrorForward(result);

        result = kzuColorAdjustmentComposerInitialize(composer, colorAdjustmentComposerData->resourceManager,
            kzsWindowGetWidth(activeWindow), kzsWindowGetHeight(activeWindow), colorAdjustmentComposerData->colorAdjustmentMaterial);
        kzsErrorForward(result);
    }

    if(colorAdjustmentComposerData->screenContentsTexture != KZ_NULL)
    {
        result = kzuRendererPushOverrideScreenTargetTexture(renderer, colorAdjustmentComposerData->screenContentsTexture);
        kzsErrorForward(result);
        /* Iterate the render passes */
        result = kzuComposerIterateChildren(colorAdjustmentComposer, renderer, transformedScene, activeWindow);
        kzsErrorForward(result);
        result = kzuRendererPopOverrideScreenTargetTexture(renderer, KZ_NULL);
        kzsErrorForward(result);
    }

    if(colorAdjustmentComposerData->colorAdjustmentMaterial != KZ_NULL && kzuComposerGetChildCount(colorAdjustmentComposer) > 0)
    {
        struct KzcVector3 points[4];
        struct KzcTexture* targetTexture;

        result = kzuRendererPeekOverrideScreenTargetTexture(renderer, &targetTexture);
        kzsErrorForward(result);

        kzuRendererSetDefaultOrthoProjectionAndViewport(renderer);

        if(targetTexture != KZ_NULL)
        {
            kzcFrameBufferBind(kzcTextureGetFrameBufferReference(targetTexture), kzuRendererGetCoreRenderer(renderer));
            kzcRendererSetViewport(kzuRendererGetCoreRenderer(renderer), 0, 0, kzcTextureGetWidth(targetTexture), kzcTextureGetHeight(targetTexture));
        }
        else
        {
            kzFloat x, y, width, height;
            kzuRendererGetAdjustedViewport(renderer, activeWindow, &x, &y, &width, &height);
            kzcRendererSetViewport(kzuRendererGetCoreRenderer(renderer), kzsFloatToUint(x), kzsFloatToUint(y),
                kzsFloatToUint(width + 0.5f), kzsFloatToUint(height + 0.5f));
        }

        result = kzuPropertyManagerSetVoid(propertyManager, colorAdjustmentComposerData->colorAdjustmentMaterial, KZU_PROPERTY_TYPE_TEXTURE, colorAdjustmentComposerData->screenContentsTexture);
        kzsErrorForward(result);

        points[0] = kzcVector3(-1.0f, 1.0f, 0.0f);
        points[1] = kzcVector3( 1.0f, 1.0f, 0.0f);
        points[2] = kzcVector3( 1.0f, -1.0f, 0.0f);
        points[3] = kzcVector3(-1.0f, -1.0f, 0.0f);

        kzcRendererSetCullMode(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_CULL_MODE_NONE);
        kzcRendererDisableState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_TEST);

        result = kzuRendererDrawQuadWithMaterial(renderer, &points[0], &points[1], &points[2], &points[3], 
            colorAdjustmentComposerData->colorAdjustmentMaterial, &KZC_MATRIX4X4_IDENTITY);
        kzsErrorForward(result);

        if(targetTexture != KZ_NULL)
        {
            kzcFrameBufferUnbind(kzcTextureGetFrameBufferReference(targetTexture), kzuRendererGetCoreRenderer(renderer));
        }
    }

    kzsSuccess();
}

struct KzuComposer* kzuColorAdjustmentComposerToComposer(struct KzuColorAdjustmentComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));
    return &composer->composer;
}

struct KzuColorAdjustmentComposer* kzuColorAdjustmentComposerFromComposer(const struct KzuComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));
    return (struct KzuColorAdjustmentComposer*)composer;
}
