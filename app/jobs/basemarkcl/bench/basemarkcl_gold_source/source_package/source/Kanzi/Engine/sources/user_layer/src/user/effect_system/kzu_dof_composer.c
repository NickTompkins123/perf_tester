/**
* \file
* Implements a depth of field composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_dof_composer.h"

#include <user/material/kzu_material.h>
#include <user/renderer/kzu_composer.h>
#include <user/renderer/kzu_renderer.h>
#include <user/renderer/kzu_renderer_util.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_vector2_property.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/renderer/kzc_renderer.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/frame_buffer/kzc_resource_frame_buffer.h>

#include <system/display/kzs_window.h>


const struct KzuComposerClass KZU_COMPOSER_DOF_CLASS =
{
    kzuDOFComposerApply,
    kzuDOFComposerDelete
};


struct KzuDOFComposer
{
    struct KzuComposer composer;
};

struct KzuDOFComposerData
{
    struct KzuComposerData composerData; /*lint -esym(754, KzuDOFComposerData::composerData)*/

    struct KzcTexture* screenContentsTexture;
    struct KzcTexture* screenContentsTextureTemp;
    struct KzcTexture* depthTexture;

    struct KzuMaterial* boxBlurMaterial;
    struct KzuMaterial* dofDepthMaterial;
    struct KzuMaterial* dofBlitMaterial;

    struct KzcResourceManager* resourceManager;
};

#define kzuDOFComposerGetData_private(composer_param) ((struct KzuDOFComposerData*)composer_param->composer.data)


kzsError kzuDOFComposerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuDOFComposer** out_bloomComposer)
{
    kzsError result;
    struct KzuDOFComposer* dofComposer;
    struct KzuDOFComposerData* dofComposerData;
    struct KzuComposer* composer;

    result = kzcMemoryAllocVariable(memoryManager, dofComposerData, "DOF composer data");
    kzsErrorForward(result);

    result = kzuComposerCreate_private(memoryManager, propertyManager, (struct KzuComposerData*)dofComposerData, 
        &KZU_COMPOSER_DOF_CLASS, &composer);
    kzsErrorForward(result);
    dofComposer = (struct KzuDOFComposer*)composer;

    dofComposerData->screenContentsTexture  = KZ_NULL;
    dofComposerData->screenContentsTextureTemp  = KZ_NULL;
    dofComposerData->boxBlurMaterial = KZ_NULL;
    dofComposerData->dofBlitMaterial = KZ_NULL;
    dofComposerData->dofDepthMaterial = KZ_NULL;
    dofComposerData->depthTexture = KZ_NULL;

    *out_bloomComposer = dofComposer;
    kzsSuccess();
}

kzsError kzuDOFComposerDelete(struct KzuComposer* composer)
{
    kzsError result;
    struct KzuDOFComposerData* dofComposerData;
    struct KzuDOFComposer* dofComposer;
    kzBool selfOwned;

    dofComposer = kzuDOFComposerFromComposer(composer);
    kzsAssert(dofComposer != KZ_NULL);

    dofComposerData = kzuDOFComposerGetData_private(dofComposer);
    selfOwned = composer->data->selfOwned;

    if(dofComposerData->screenContentsTexture != KZ_NULL)
    {
        result = kzcTextureDelete(dofComposerData->screenContentsTexture);
        kzsErrorForward(result);
        dofComposerData->screenContentsTexture = KZ_NULL;
    }
    if(dofComposerData->screenContentsTextureTemp != KZ_NULL)
    {
        result = kzcTextureDelete(dofComposerData->screenContentsTextureTemp);
        kzsErrorForward(result);
        dofComposerData->screenContentsTextureTemp = KZ_NULL;
    }
    if(dofComposerData->depthTexture != KZ_NULL)
    {
        result = kzcTextureDelete(dofComposerData->depthTexture);
        kzsErrorForward(result);
        dofComposerData->depthTexture = KZ_NULL;
    }

    result = kzuComposerDelete_private((struct KzuComposer*)dofComposer);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(dofComposer);
        kzsErrorForward(result);
    }
    else
    {
        composer->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuDOFComposerInitialize(const struct KzuDOFComposer* dofComposer, struct KzcResourceManager* resourceManager,
                                    kzUint renderTargetTextureWidth, kzUint renderTargetTextureHeight,
                                    struct KzuMaterial* boxBlurMaterial, struct KzuMaterial* dofDepthMaterial,
                                    struct KzuMaterial* dofBlitMaterial)
{
    kzsError result;
    struct KzuDOFComposerData* dofComposerData;

    kzsAssert(dofComposer != KZ_NULL);

    dofComposerData = kzuDOFComposerGetData_private(dofComposer);

    result = kzcTextureCreateFrameBufferTexture(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGB,
        renderTargetTextureWidth, renderTargetTextureHeight, KZC_TEXTURE_FILTER_BILINEAR, &dofComposerData->screenContentsTexture);
    kzsErrorForward(result);

    result = kzcTextureCreateFrameBufferTexture(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGB,
        renderTargetTextureWidth, renderTargetTextureHeight, KZC_TEXTURE_FILTER_BILINEAR, &dofComposerData->screenContentsTextureTemp);
    kzsErrorForward(result);

    dofComposerData->boxBlurMaterial = boxBlurMaterial;
    dofComposerData->dofDepthMaterial = dofDepthMaterial;
    dofComposerData->dofBlitMaterial = dofBlitMaterial;
    dofComposerData->resourceManager = resourceManager;

    kzsSuccess();
}

kzsError kzuDOFComposerApply(const struct KzuComposer* dofComposer, struct KzuRenderer* renderer,
                             const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow)
{
    kzsError result;
    struct KzuDOFComposerData* dofComposerData;
    struct KzuDOFComposer* composer;
    kzUint i;
    kzUint windowWidth, windowHeight;

    kzsAssert(dofComposer != KZ_NULL);
    composer = kzuDOFComposerFromComposer(dofComposer);
    kzsAssert(composer != KZ_NULL);
    dofComposerData = kzuDOFComposerGetData_private(composer);

    windowWidth = kzsWindowGetWidth(activeWindow);
    windowHeight = kzsWindowGetHeight(activeWindow);

    if(dofComposerData->depthTexture == KZ_NULL ||
        kzcTextureGetWidth(dofComposerData->depthTexture) != windowWidth ||
        kzcTextureGetHeight(dofComposerData->depthTexture) != windowHeight)
    {
        result = kzcRendererReset(kzuRendererGetCoreRenderer(renderer));
        kzsErrorForward(result);

        if(dofComposerData->depthTexture != KZ_NULL)
        {
            result = kzcTextureDelete(dofComposerData->depthTexture);
            kzsErrorForward(result);
        }

        result = kzcTextureCreateFrameBufferTexture(dofComposerData->resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGB,
            windowWidth, windowHeight, KZC_TEXTURE_FILTER_BILINEAR, &dofComposerData->depthTexture);
        kzsErrorForward(result);
    }

    if(kzuComposerGetChildCount(dofComposer) > 0)
    {
        result = kzuRendererPushOverrideScreenTargetTexture(renderer, dofComposerData->screenContentsTexture);
        kzsErrorForward(result);
        /* Iterate the render passes */
        result = kzuComposerIterateChildren(dofComposer, renderer, transformedScene, activeWindow);
        kzsErrorForward(result);
        result = kzuRendererPopOverrideScreenTargetTexture(renderer, KZ_NULL);
        kzsErrorForward(result);

        /* Blur the DOF target. */
        for(i = 0; i < 2; ++i)
        {
            struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
            struct KzcFrameBuffer* frameBuffer = kzcTextureGetFrameBufferReference(
                i == 0 ? dofComposerData->screenContentsTextureTemp : dofComposerData->screenContentsTexture);
            struct KzcTexture* sourceTexture = i == 0 ? dofComposerData->screenContentsTexture :
                dofComposerData->screenContentsTextureTemp;

            kzcFrameBufferBind(frameBuffer, coreRenderer);
            {
                struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(dofComposerData->boxBlurMaterial);
                const struct KzuPropertyType* blurDirectionProperty = kzuPropertyManagerFindPropertyType(propertyManager, "PostprocessingMaterialBlurDirection");
                const struct KzuPropertyType* blurSpreadProperty = kzuPropertyManagerFindPropertyType(propertyManager, "PostprocessingMaterialBlurSpread");
                struct KzcVector3 points[4];

                result = kzuPropertyManagerSetVoid(propertyManager, dofComposerData->boxBlurMaterial, KZU_PROPERTY_TYPE_TEXTURE, sourceTexture);
                kzsErrorForward(result);

                result = kzuPropertyManagerSetVector2(propertyManager, dofComposerData->boxBlurMaterial, blurDirectionProperty,
                                                      i == 0 ? kzcVector2(1.0f, 0.0f) : kzcVector2(0.0f, 1.0f));
                kzsErrorForward(result);
                    /* Blur spread normalized to 0...1, maximum spread 10% of screen (in 5 steps). */
                result = kzuPropertyManagerSetFloat(propertyManager, dofComposerData->boxBlurMaterial, blurSpreadProperty,
                                                    kzuPropertyManagerGetFloatDefault(propertyManager, dofComposer, KZU_PROPERTY_TYPE_POST_PROCESSING_DOFBLUR_SPREAD) * 0.1f / 5.0f);
                kzsErrorForward(result);

                kzcRendererSetViewport(coreRenderer, 0, 0, kzcTextureGetWidth(sourceTexture), kzcTextureGetHeight(sourceTexture));
                kzuRendererSetDefaultOrthoProjectionAndViewport(renderer);

                points[0] = kzcVector3(-1.0f, 1.0f, 0.0f);
                points[1] = kzcVector3( 1.0f, 1.0f, 0.0f);
                points[2] = kzcVector3( 1.0f, -1.0f, 0.0f);
                points[3] = kzcVector3(-1.0f, -1.0f, 0.0f);

                kzcRendererSetCullMode(coreRenderer, KZC_RENDERER_CULL_MODE_NONE);
                kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_DEPTH_TEST);

                result = kzuRendererDrawQuadWithMaterial(renderer, &points[0], &points[1], &points[2], &points[3],
                    dofComposerData->boxBlurMaterial, &KZC_MATRIX4X4_IDENTITY);
                kzsErrorForward(result);
            }

            kzcFrameBufferUnbind(frameBuffer, coreRenderer);
        }
        /* Fetch the depth */
        {
            result = kzuRendererPushOverrideScreenTargetTexture(renderer, dofComposerData->depthTexture);
            kzsErrorForward(result);
            /* Iterate the render passes */
            kzuRendererSetMaterialOverride(renderer, dofComposerData->dofDepthMaterial);
            kzuRendererSetClearColorOverride(renderer, &KZC_COLOR_WHITE);
            result = kzuComposerIterateChildren(dofComposer, renderer, transformedScene, activeWindow);
            kzsErrorForward(result);
            result = kzuRendererPopOverrideScreenTargetTexture(renderer, KZ_NULL);
            kzsErrorForward(result);
            kzuRendererSetClearColorOverride(renderer, KZ_NULL);
            kzuRendererSetMaterialOverride(renderer, KZ_NULL);
        }

        result = kzuComposerIterateChildren(dofComposer, renderer, transformedScene, activeWindow);
        kzsErrorForward(result);

        /* Apply DOF. */
        {
            struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(dofComposerData->dofBlitMaterial);
            const struct KzuPropertyType* propertyDepth = kzuPropertyManagerFindPropertyType(propertyManager, "PostProcessingEffectTextureDepth");
            struct KzcVector3 points[4];
            struct KzcTexture* targetTexture;

            result = kzuRendererPeekOverrideScreenTargetTexture(renderer, &targetTexture);
            kzsErrorForward(result);
            if(targetTexture != KZ_NULL)
            {
                kzcFrameBufferBind(kzcTextureGetFrameBufferReference(targetTexture), kzuRendererGetCoreRenderer(renderer));
            }

            result = kzuPropertyManagerSetVoid(propertyManager, dofComposerData->dofBlitMaterial, KZU_PROPERTY_TYPE_TEXTURE, dofComposerData->screenContentsTexture);
            kzsErrorForward(result);
            result = kzuPropertyManagerSetVoid(propertyManager, dofComposerData->dofBlitMaterial, propertyDepth, dofComposerData->depthTexture);
            kzsErrorForward(result);
               
            result = kzuPropertyManagerSetInt(propertyManager, dofComposerData->dofBlitMaterial, KZU_PROPERTY_TYPE_BLEND_MODE, /*lint -e{641}*/KZC_RENDERER_BLEND_MODE_ALPHA);
            kzsErrorForward(result);

            kzuRendererSetDefaultOrthoProjectionAndViewport(renderer);

            points[0] = kzcVector3(-1.0f, 1.0f, 0.0f);
            points[1] = kzcVector3( 1.0f, 1.0f, 0.0f);
            points[2] = kzcVector3( 1.0f, -1.0f, 0.0f);
            points[3] = kzcVector3(-1.0f, -1.0f, 0.0f);

            kzcRendererSetCullMode(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_CULL_MODE_NONE);
            kzcRendererDisableState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_TEST);
            kzcRendererDisableState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_WRITE);

            result = kzuRendererDrawQuadWithMaterial(renderer, &points[0], &points[1], &points[2], &points[3], dofComposerData->dofBlitMaterial, &KZC_MATRIX4X4_IDENTITY);
            kzsErrorForward(result);

            if(targetTexture != KZ_NULL)
            {
                kzcFrameBufferUnbind(kzcTextureGetFrameBufferReference(targetTexture), kzuRendererGetCoreRenderer(renderer));
            }
        }
    }

    kzsSuccess();
}

struct KzuComposer* kzuDOFComposerToComposer(struct KzuDOFComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));
    return &composer->composer;
}

struct KzuDOFComposer* kzuDOFComposerFromComposer(const struct KzuComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));
    return (struct KzuDOFComposer*)composer;
}

