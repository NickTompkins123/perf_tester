/**
* \file
* Specifies a bloom composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_bloom_composer.h"

#include <user/material/kzu_material.h>
#include <user/renderer/kzu_composer.h>
#include <user/renderer/kzu_renderer.h>
#include <user/renderer/kzu_renderer_util.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_vector2_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/renderer/kzc_renderer.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/color/kzc_color.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/frame_buffer/kzc_resource_frame_buffer.h>


const struct KzuComposerClass KZU_COMPOSER_BLOOM_CLASS =
{
    kzuBloomComposerApply,
    kzuBloomComposerDelete
};


struct KzuBloomComposer
{
    struct KzuComposer composer;
};

struct KzuBloomComposerData
{
    struct KzuComposerData composerData; /*lint -esym(754, KzuBloomComposerData::composerData)*/

    struct KzcTexture* screenContentsTexture;
    struct KzcTexture* screenContentsTextureTemp;
    struct KzuMaterial* boxBlurMaterial;
    struct KzuMaterial* blitMaterial;
};

#define kzuBloomComposerGetData_private(composer_param) ((struct KzuBloomComposerData*)composer_param->composer.data)


kzsError kzuBloomComposerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuBloomComposer** out_bloomComposer)
{
    kzsError result;
    struct KzuBloomComposer* bloomComposer;
    struct KzuBloomComposerData* bloomComposerData;
    struct KzuComposer* composer;

    result = kzcMemoryAllocVariable(memoryManager, bloomComposerData, "Bloom composer data");
    kzsErrorForward(result);

    result = kzuComposerCreate_private(memoryManager, propertyManager, (struct KzuComposerData*)bloomComposerData, 
        &KZU_COMPOSER_BLOOM_CLASS, &composer);
    kzsErrorForward(result);
    bloomComposer = (struct KzuBloomComposer*)composer;

    bloomComposerData->screenContentsTexture  = KZ_NULL;
    bloomComposerData->screenContentsTextureTemp  = KZ_NULL;
    bloomComposerData->boxBlurMaterial  = KZ_NULL;

    *out_bloomComposer = bloomComposer;
    kzsSuccess();
}

kzsError kzuBloomComposerDelete(struct KzuComposer* composer)
{
    kzsError result;
    struct KzuBloomComposerData* bloomComposerData;
    struct KzuBloomComposer* bloomComposer;
    kzBool selfOwned;

    bloomComposer = kzuBloomComposerFromComposer(composer);
    kzsAssert(bloomComposer != KZ_NULL);

    bloomComposerData = kzuBloomComposerGetData_private(bloomComposer);
    selfOwned = composer->data->selfOwned;

    if(bloomComposerData->screenContentsTexture != KZ_NULL)
    {
        result = kzcTextureDelete(bloomComposerData->screenContentsTexture);
        kzsErrorForward(result);
        bloomComposerData->screenContentsTexture = KZ_NULL;
    }
    if(bloomComposerData->screenContentsTextureTemp != KZ_NULL)
    {
        result = kzcTextureDelete(bloomComposerData->screenContentsTextureTemp);
        kzsErrorForward(result);
        bloomComposerData->screenContentsTextureTemp = KZ_NULL;
    }

    result = kzuComposerDelete_private((struct KzuComposer*)bloomComposer);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(bloomComposer);
        kzsErrorForward(result);
    }
    else
    {
        composer->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuBloomComposerInitialize(const struct KzuBloomComposer* bloomComposer, struct KzcResourceManager* resourceManager,
                                    kzUint renderTargetTextureWidth, kzUint renderTargetTextureHeight,
                                    struct KzuMaterial* bloomBlurMaterial, struct KzuMaterial* bloomBlitMaterial)
{
    kzsError result;
    struct KzuBloomComposerData* bloomComposerData;

    kzsAssert(bloomComposer != KZ_NULL);

    bloomComposerData = kzuBloomComposerGetData_private(bloomComposer);

    result = kzcTextureCreateFrameBufferTexture(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGB,
        renderTargetTextureWidth, renderTargetTextureHeight, KZC_TEXTURE_FILTER_BILINEAR, &bloomComposerData->screenContentsTexture);
    kzsErrorForward(result);

    result = kzcTextureCreateFrameBufferTexture(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGB,
        renderTargetTextureWidth, renderTargetTextureHeight, KZC_TEXTURE_FILTER_BILINEAR, &bloomComposerData->screenContentsTextureTemp);
    kzsErrorForward(result);

    bloomComposerData->boxBlurMaterial = bloomBlurMaterial;
    bloomComposerData->blitMaterial = bloomBlitMaterial;

    kzsSuccess();
}

kzsError kzuBloomComposerApply(const struct KzuComposer* bloomComposer, struct KzuRenderer* renderer,
                               const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow)
{
    kzsError result;
    struct KzuBloomComposerData* bloomComposerData;
    struct KzuBloomComposer* composer;
    struct KzuPropertyManager* propertyManager = kzuComposerGetPropertyManager(bloomComposer);
    kzUint i;

    kzsAssert(bloomComposer != KZ_NULL);
    composer = kzuBloomComposerFromComposer(bloomComposer);
    kzsAssert(composer != KZ_NULL);
    bloomComposerData = kzuBloomComposerGetData_private(composer);

    if(kzuComposerGetChildCount(bloomComposer) > 0)
    {
        result = kzuRendererPushOverrideScreenTargetTexture(renderer, bloomComposerData->screenContentsTexture);
        kzsErrorForward(result);
        /* Iterate the render passes */
        result = kzuComposerIterateChildren(bloomComposer, renderer, transformedScene, activeWindow);
        kzsErrorForward(result);
        result = kzuRendererPopOverrideScreenTargetTexture(renderer, KZ_NULL);
        kzsErrorForward(result);

        /* Bloom. */
        for(i = 0; i < 2; ++i)
        {
            struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
            struct KzcFrameBuffer* frameBuffer = kzcTextureGetFrameBufferReference(
                i == 0 ? bloomComposerData->screenContentsTextureTemp : bloomComposerData->screenContentsTexture);
            struct KzcTexture* sourceTexture = i == 0 ? bloomComposerData->screenContentsTexture :
                bloomComposerData->screenContentsTextureTemp;

            kzcFrameBufferBind(frameBuffer, coreRenderer);
            {
                const struct KzuPropertyType* blurDirectionPropertyType = kzuPropertyManagerFindPropertyType(propertyManager, "PostprocessingMaterialBlurDirection");
                const struct KzuPropertyType* blurSpreadPropertyType = kzuPropertyManagerFindPropertyType(propertyManager, "PostprocessingMaterialBlurSpread");
                struct KzcVector3 points[4];

                kzcRendererSetViewport(coreRenderer, 0, 0, kzcTextureGetWidth(sourceTexture), kzcTextureGetHeight(sourceTexture));
                kzuRendererSetDefaultOrthoProjectionAndViewport(renderer);
                result = kzuPropertyManagerSetVoid(propertyManager, bloomComposerData->boxBlurMaterial, KZU_PROPERTY_TYPE_TEXTURE, sourceTexture);
                kzsErrorForward(result);

                result = kzuPropertyManagerSetVector2(propertyManager, bloomComposerData->boxBlurMaterial, blurDirectionPropertyType,
                                                      i == 0 ? kzcVector2(1.0f, 0.0f) : kzcVector2(0.0f, 1.0f));
                kzsErrorForward(result);

                /* Bloom spread normalized to 0...1, maximum spread 10% of screen (in 5 steps). */
                result = kzuPropertyManagerSetFloat(propertyManager, bloomComposerData->boxBlurMaterial, blurSpreadPropertyType,
                                                    kzuPropertyManagerGetFloatDefault(propertyManager, bloomComposer, KZU_PROPERTY_TYPE_POST_PROCESSING_BLOOM_SPREAD) * 0.1f / 5.0f);
                kzsErrorForward(result);

                points[0] = kzcVector3(-1.0f, 1.0f, 0.0f);
                points[1] = kzcVector3( 1.0f, 1.0f, 0.0f);
                points[2] = kzcVector3( 1.0f, -1.0f, 0.0f);
                points[3] = kzcVector3(-1.0f, -1.0f, 0.0f);

                kzcRendererSetCullMode(coreRenderer, KZC_RENDERER_CULL_MODE_NONE);
                kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_STENCIL_TEST);
                kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_DEPTH_TEST);
                kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_DEPTH_WRITE);

                result = kzuRendererDrawQuadWithMaterial(renderer, &points[0], &points[1], &points[2], &points[3],
                    bloomComposerData->boxBlurMaterial, &KZC_MATRIX4X4_IDENTITY);
                kzsErrorForward(result);
            }

            kzcFrameBufferUnbind(frameBuffer, coreRenderer);
        }

        result = kzuComposerIterateChildren(bloomComposer, renderer, transformedScene, activeWindow);
        kzsErrorForward(result);

        /* Apply bloom. */
        {
            struct KzcVector3 points[4];
            struct KzcTexture* targetTexture;

            result = kzuRendererPeekOverrideScreenTargetTexture(renderer, &targetTexture);
            kzsErrorForward(result);
            if(targetTexture != KZ_NULL)
            {
                kzcFrameBufferBind(kzcTextureGetFrameBufferReference(targetTexture), kzuRendererGetCoreRenderer(renderer));
            }

            result = kzuPropertyManagerSetVoid(propertyManager, bloomComposerData->blitMaterial, KZU_PROPERTY_TYPE_TEXTURE, bloomComposerData->screenContentsTexture);
            kzsErrorForward(result);

            {
                const struct KzuPropertyType* bloomIntensityPropertyType = kzuPropertyManagerFindPropertyType(propertyManager, "ColorMultiply");

                result = kzuPropertyManagerSetInt(propertyManager, bloomComposerData->blitMaterial, KZU_PROPERTY_TYPE_BLEND_MODE, /*lint -e{641}*/KZC_RENDERER_BLEND_MODE_ADDITIVE);
                kzsErrorForward(result);

                {
                    struct KzcColorRGBA color = kzuPropertyManagerGetColorDefault(propertyManager, bloomComposer, KZU_PROPERTY_TYPE_POST_PROCESSING_BLOOM_INTENSITY);
                    result = kzuPropertyManagerSetColor(propertyManager, bloomComposerData->blitMaterial, bloomIntensityPropertyType, color);
                    kzsErrorForward(result);
                    result = kzuPropertyManagerSetFloat(propertyManager, bloomComposerData->blitMaterial, KZU_PROPERTY_TYPE_BLEND_INTENSITY, 1.0f);
                    kzsErrorForward(result);
                }
            }

            kzuRendererSetDefaultOrthoProjectionAndViewport(renderer);

            points[0] = kzcVector3(-1.0f, 1.0f, 0.0f);
            points[1] = kzcVector3( 1.0f, 1.0f, 0.0f);
            points[2] = kzcVector3( 1.0f, -1.0f, 0.0f);
            points[3] = kzcVector3(-1.0f, -1.0f, 0.0f);

            kzcRendererSetCullMode(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_CULL_MODE_NONE);
            kzcRendererDisableState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_TEST);
            kzcRendererDisableState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_WRITE);

            result = kzuRendererDrawQuadWithMaterial(renderer, &points[0], &points[1], &points[2], &points[3], bloomComposerData->blitMaterial, &KZC_MATRIX4X4_IDENTITY);
            kzsErrorForward(result);

            if(targetTexture != KZ_NULL)
            {
                kzcFrameBufferUnbind(kzcTextureGetFrameBufferReference(targetTexture), kzuRendererGetCoreRenderer(renderer));
            }
        }
    }

    kzsSuccess();
}

struct KzuComposer* kzuBloomComposerToComposer(struct KzuBloomComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));
    return &composer->composer;
}

struct KzuBloomComposer* kzuBloomComposerFromComposer(const struct KzuComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));
    return (struct KzuBloomComposer*)composer;
}
