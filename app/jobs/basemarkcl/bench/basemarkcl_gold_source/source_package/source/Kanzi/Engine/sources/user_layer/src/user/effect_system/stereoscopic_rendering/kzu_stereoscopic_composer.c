/**
* \file
* Specifies a stereoscopic composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_stereoscopic_composer.h"

#include <user/material/kzu_material.h>
#include <user/renderer/kzu_composer.h>
#include <user/renderer/kzu_renderer.h>
#include <user/renderer/kzu_renderer_util.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/properties/kzu_vector3_property.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property_query.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/renderer/kzc_renderer.h>
#include <core/util/math/kzc_vector3.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>

#include <system/display/kzs_window.h>


const struct KzuComposerClass KZU_COMPOSER_STEREOSCOPIC_CLASS =
{
    kzuStereoscopicComposerApply,
    kzuStereoscopicComposerDelete
};


struct KzuStereoscopicComposer
{
    struct KzuComposer composer;
};

struct KzuStereoscopicComposerData
{
    struct KzuComposerData composerData; /*lint -esym(754, KzuStereoscopicComposerData::composerData)*/
    struct KzcResourceManager* resourceManager;

    struct KzuMaterial* anaglyphicMaterial; /**< Material for red-cyan anaglyphic rendering. */
    struct KzcTexture* stereoscopicEyeLeftTexture; /**< Left texture for red-cyan anaglyphic glasses. */
    struct KzcTexture* stereoscopicEyeRightTexture; /**< Right texture for red-cyan anaglyphic glasses. */
};

#define kzuStereoscopicComposerGetData_private(composer_param) ((struct KzuStereoscopicComposerData*)composer_param->composer.data)


kzsError kzuStereoscopicComposerCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager,
                                       struct KzuStereoscopicComposer** out_stereoscopicComposer)
{
    kzsError result;
    struct KzuStereoscopicComposer* stereoscopicComposer;
    struct KzuStereoscopicComposerData* stereoscopicComposerData;
    struct KzuComposer* composer;

    result = kzcMemoryAllocVariable(memoryManager, stereoscopicComposerData, "Stereoscopic composer data");
    kzsErrorForward(result);

    result = kzuComposerCreate_private(memoryManager, propertyManager, (struct KzuComposerData*)stereoscopicComposerData, 
        &KZU_COMPOSER_STEREOSCOPIC_CLASS, &composer);
    kzsErrorForward(result);
    stereoscopicComposer = (struct KzuStereoscopicComposer*)composer;

    stereoscopicComposerData->stereoscopicEyeLeftTexture = KZ_NULL;
    stereoscopicComposerData->stereoscopicEyeRightTexture = KZ_NULL;
    stereoscopicComposerData->anaglyphicMaterial = KZ_NULL;

    *out_stereoscopicComposer = stereoscopicComposer;
    kzsSuccess();
}

kzsError kzuStereoscopicComposerDelete(struct KzuComposer* composer)
{
    kzsError result;
    struct KzuStereoscopicComposerData* stereoscopicComposerData;
    struct KzuStereoscopicComposer* stereoscopicComposer;
    kzBool selfOwned;

    stereoscopicComposer = kzuStereoscopicComposerFromComposer(composer);
    kzsAssert(stereoscopicComposer != KZ_NULL);

    stereoscopicComposerData = kzuStereoscopicComposerGetData_private(stereoscopicComposer);
    selfOwned = composer->data->selfOwned;

    if(stereoscopicComposerData->stereoscopicEyeLeftTexture != KZ_NULL)
    {
        result = kzcTextureDelete(stereoscopicComposerData->stereoscopicEyeLeftTexture);
        kzsErrorForward(result);
        stereoscopicComposerData->stereoscopicEyeLeftTexture = KZ_NULL;
    }
    if(stereoscopicComposerData->stereoscopicEyeRightTexture != KZ_NULL)
    {
        result = kzcTextureDelete(stereoscopicComposerData->stereoscopicEyeRightTexture);
        kzsErrorForward(result);
        stereoscopicComposerData->stereoscopicEyeRightTexture = KZ_NULL;
    }

    result = kzuComposerDelete_private((struct KzuComposer*)stereoscopicComposer);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(stereoscopicComposer);
        kzsErrorForward(result);
    }
    else
    {
        composer->data = KZ_NULL;
    }

    kzsSuccess();
}

void kzuStereoscopicComposerInitialize(const struct KzuStereoscopicComposer* stereoscopicComposer, struct KzcResourceManager* resourceManager,
                                       struct KzuMaterial* anaglyphicMaterial)
{
    struct KzuStereoscopicComposerData* stereoscopicComposerData;
    kzsAssert(stereoscopicComposer != KZ_NULL);

    stereoscopicComposerData = kzuStereoscopicComposerGetData_private(stereoscopicComposer);
    stereoscopicComposerData->resourceManager = resourceManager;
    stereoscopicComposerData->anaglyphicMaterial = anaglyphicMaterial;
}

static enum KzuStereoscopicRenderingMode kzuStereoscopicModeFromProperty_internal(kzInt modeProperty)
{
    enum KzuStereoscopicRenderingMode mode = KZU_STEREOSCOPIC_RENDERING_DISABLED;
    kzInt value = modeProperty;
    switch(value)
    {
        case -1: mode = KZU_STEREOSCOPIC_RENDERING_DISABLED; break;
        case 0: mode = KZU_STEREOSCOPIC_RENDERING_ANAGLYPHIC_RED_CYAN; break;
        case 1: mode = KZU_STEREOSCOPIC_RENDERING_VERTICAL_LINE_INTERLACED; break;
        case 2: mode = KZU_STEREOSCOPIC_RENDERING_HORIZONTAL_LINE_INTERLACED; break;
        case 3: mode = KZU_STEREOSCOPIC_RENDERING_TOP_BOTTOM; break;
        case 4: mode = KZU_STEREOSCOPIC_RENDERING_BOTTOM_TOP; break;
        case 5: mode = KZU_STEREOSCOPIC_RENDERING_LEFT_RIGHT; break;
        case 6: mode = KZU_STEREOSCOPIC_RENDERING_RIGHT_LEFT; break;
        default: kzsAssertText(KZ_FALSE, "Invalid stereoscopic rendering mode in binary");
    }
    return mode;
}

struct KzuPropertyManager* kzuStereoscopicComposerGetPropertyManager(const struct KzuStereoscopicComposer* stereoscopicComposer)
{
    return kzuComposerGetPropertyManager(kzuStereoscopicComposerToComposer((struct KzuStereoscopicComposer*)stereoscopicComposer));
}

kzsError kzuStereoscopicComposerApply(const struct KzuComposer* stereoscopicComposer, struct KzuRenderer* renderer,
                                      const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow)
{
    kzsError result;
    struct KzuCameraNode* cameraNode;
    struct KzuStereoscopicComposerData* stereoscopicComposerData;
    struct KzuStereoscopicComposer* composer;
    struct KzuPropertyManager* propertyManager = kzuComposerGetPropertyManager(stereoscopicComposer);
    enum KzuStereoscopicRenderingMode renderingMode;
    struct KzuPropertyQuery* query;

    kzsAssert(stereoscopicComposer != KZ_NULL);

    composer = kzuStereoscopicComposerFromComposer(stereoscopicComposer);
    kzsAssert(composer != KZ_NULL);
    stereoscopicComposerData = kzuStereoscopicComposerGetData_private(composer);

    cameraNode = kzuSceneGetViewCamera(kzuTransformedSceneGetScene(transformedScene));

    renderingMode = kzuStereoscopicModeFromProperty_internal(kzuPropertyManagerGetIntDefault(propertyManager, stereoscopicComposer, KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_RENDERING_MODE));

    if(renderingMode == KZU_STEREOSCOPIC_RENDERING_ANAGLYPHIC_RED_CYAN)
    {
        if(stereoscopicComposerData->stereoscopicEyeLeftTexture == KZ_NULL)
        {
            kzUint renderingAreaWidth = kzsWindowGetWidth(activeWindow);
            kzUint renderingAreaHeight = kzsWindowGetHeight(activeWindow);

            result = kzcTextureCreateFrameBufferTexture(stereoscopicComposerData->resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGBA,
                renderingAreaWidth, renderingAreaHeight, KZC_TEXTURE_FILTER_BILINEAR, &stereoscopicComposerData->stereoscopicEyeLeftTexture);
            kzsErrorForward(result);
            result = kzcTextureCreateFrameBufferTexture(stereoscopicComposerData->resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGBA,
                renderingAreaWidth, renderingAreaHeight, KZC_TEXTURE_FILTER_BILINEAR, &stereoscopicComposerData->stereoscopicEyeRightTexture);
            kzsErrorForward(result);
        }
    }

    query = kzuRendererGetPropertyQuery(renderer);

    if(cameraNode != KZ_NULL)
    {
        struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
        kzFloat eyeDistance;
        kzFloat focusDistance;
        struct KzuTransformedObjectNode* transformedCameraNode = kzuTransformedObjectListFindObject(kzuTransformedSceneGetObjects(transformedScene), kzuCameraNodeToObjectNode(cameraNode));
        struct KzcMatrix4x4 originalMatrix = kzuTransformedObjectNodeGetMatrix(transformedCameraNode);
        struct KzcMatrix4x4 transformedMatrix = originalMatrix;
        kzFloat rotateAngle;

        result = kzuPropertyQueryPushObject(query, cameraNode);
        kzsErrorForward(result);

        eyeDistance = kzuPropertyQueryGetFloat(query, KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_EYE_DISTANCE);
        focusDistance = kzuPropertyQueryGetFloat(query, KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_FOCUS_DISTANCE);
        rotateAngle = kzsTanf(eyeDistance / focusDistance) / 2.0f;

        if (renderingMode == KZU_STEREOSCOPIC_RENDERING_ANAGLYPHIC_RED_CYAN)
        {
            kzcMatrix4x4Translate(&transformedMatrix, -eyeDistance, 0.0f, 0.0f);
            kzcMatrix4x4Rotate(&transformedMatrix, rotateAngle, 0.0f, 1.0f, 0.0f);
            kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &transformedMatrix);

            kzuRendererEnableStereoscopic(renderer, KZU_STEREOSCOPIC_RENDERING_ANAGLYPHIC_RED_CYAN, 0);
            result = kzuRendererPushOverrideScreenTargetTexture(renderer, stereoscopicComposerData->stereoscopicEyeLeftTexture);
            kzsErrorForward(result);

            /* Iterate the render passes */
            result = kzuComposerIterateChildren(stereoscopicComposer, renderer, transformedScene, activeWindow);
            kzsErrorForward(result);
            result = kzuRendererPopOverrideScreenTargetTexture(renderer, KZ_NULL);
            kzsErrorForward(result);

            transformedMatrix = originalMatrix;
            kzcMatrix4x4Translate(&transformedMatrix, eyeDistance, 0.0f, 0.0f);
            kzcMatrix4x4Rotate(&transformedMatrix, -rotateAngle, 0.0f, 1.0f, 0.0f);
            kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &transformedMatrix);

            kzuRendererEnableStereoscopic(renderer, KZU_STEREOSCOPIC_RENDERING_ANAGLYPHIC_RED_CYAN, 1);
            result = kzuRendererPushOverrideScreenTargetTexture(renderer, stereoscopicComposerData->stereoscopicEyeRightTexture);
            kzsErrorForward(result);
            /* Iterate the render passes */
            result = kzuComposerIterateChildren(stereoscopicComposer, renderer, transformedScene, activeWindow);
            kzsErrorForward(result);
            result = kzuRendererPopOverrideScreenTargetTexture(renderer, KZ_NULL);
            kzsErrorForward(result);

            kzuRendererDisableStereoscopic(renderer);
            kzcRendererSetColorWriteMode(KZC_RENDERER_COLOR_WRITE_RGBA);
            kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &originalMatrix);
        }
        else if(renderingMode == KZU_STEREOSCOPIC_RENDERING_VERTICAL_LINE_INTERLACED ||
            renderingMode == KZU_STEREOSCOPIC_RENDERING_HORIZONTAL_LINE_INTERLACED)
        {
            kzUint i;
            kzUint offset;
            kzFloat lineX = 0.0f;
            kzFloat lineY = 0.0f;
            kzFloat lineWidth = 1.0f;
            kzFloat lineHeight = 1.0f;
            kzUint lineAreaSize = 0;
            kzFloat* data;

            kzcRendererSetCullMode(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_CULL_MODE_NONE);
            kzcRendererDisableState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_TEST);

            {
                struct KzcMatrix4x4 projectionMatrix;
                struct KzcMatrix4x4 cameraMatrix;
                kzFloat x, y, width, height;

                kzuRendererGetAdjustedViewport(renderer, activeWindow, &x, &y, &width, &height);
                kzcRendererSetViewport(coreRenderer, kzsFloatToUint(x), kzsFloatToUint(y),
                    kzsFloatToUint(width + 0.5f), kzsFloatToUint(height + 0.5f));

                if(renderingMode == KZU_STEREOSCOPIC_RENDERING_VERTICAL_LINE_INTERLACED)
                {
                    lineAreaSize = kzsFloatToUint(width + 0.5f);
                    lineHeight = height;
                }
                else if(renderingMode == KZU_STEREOSCOPIC_RENDERING_HORIZONTAL_LINE_INTERLACED)
                {
                    lineAreaSize = kzsFloatToUint(height + 0.5f);
                    lineWidth = width;
                    lineY = (kzFloat)kzsFloatToUint(height + 0.5f);
                }

                kzuCameraNodeCalculateOrthogonalProjectionFromValues(0.0f, (kzFloat)kzsFloatToUint(width + 0.5f),
                                                                     (kzFloat)kzsFloatToUint(height + 0.5f), 0.0f,
                                                                     KZU_ORTHOGONAL_COORDINATE_SYSTEM_RELATIVE,
                                                                     KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL,
                                                                     1.0f, 1.0f, &projectionMatrix);
                kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_PROJECTION, &projectionMatrix);

                cameraMatrix = KZC_MATRIX4X4_IDENTITY;
                cameraMatrix.data[14] = 0.5f;

                kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_CAMERA, &cameraMatrix);
                kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD, &KZC_MATRIX4X4_IDENTITY);
            }

            kzcRendererSetState(coreRenderer, KZC_RENDERER_STATE_STENCIL_WRITE);
            kzcRendererResetClearTarget(coreRenderer);
            kzcRendererAddClearTarget(coreRenderer, KZC_RENDERER_CLEAR_TARGET_STENCIL);
            kzcRendererAddClearTarget(coreRenderer, KZC_RENDERER_CLEAR_TARGET_DEPTH);
            kzcRendererSetClearStencil(coreRenderer, 0);
            kzcRendererClear(coreRenderer);

            result = kzcMemoryAllocArray(kzuRendererGetQuickMemoryManager(renderer), data, lineAreaSize * 3 * 3 * 2 * sizeof(kzFloat), "InterlaceData");
            kzsErrorForward(result);

            result = kzuRendererApplyMaterial(renderer, kzuRendererGetSolidColorMaterial(renderer));
            kzsErrorForward(result);

            offset = 0;
            for(i = 0; i < lineAreaSize; i += 2)
            {
                data[offset++] = lineX;
                data[offset++] = lineY;
                data[offset++] = 0.0f;
                data[offset++] = lineX;
                data[offset++] = lineY + lineHeight;
                data[offset++] = 0.0f;
                data[offset++] = lineX + lineWidth;
                data[offset++] = lineY;
                data[offset++] = 0.0f;
                data[offset++] = lineX;
                data[offset++] = lineY + lineHeight;
                data[offset++] = 0.0f;
                data[offset++] = lineX + lineWidth;
                data[offset++] = lineY + lineHeight;
                data[offset++] = 0.0f;
                data[offset++] = lineX + lineWidth;
                data[offset++] = lineY;
                data[offset++] = 0.0f;

                if(renderingMode == KZU_STEREOSCOPIC_RENDERING_VERTICAL_LINE_INTERLACED)
                {
                    lineX += 2.0f;
                }
                else
                {
                    lineY -= 2.0f;
                }

            }
            kzcRendererSetColorWriteMode(KZC_RENDERER_COLOR_WRITE_NONE);
            kzcRendererSetStencilFunction(coreRenderer, KZC_RENDERER_STENCIL_MODE_NEVER, 1, 1);
            kzcRendererSetStencilOperation(coreRenderer, KZC_RENDERER_STENCIL_OPERATION_REPLACE, KZC_RENDERER_STENCIL_OPERATION_REPLACE, KZC_RENDERER_STENCIL_OPERATION_REPLACE);
            kzcRendererSetState(coreRenderer, KZC_RENDERER_STATE_STENCIL_WRITE);
            kzcRendererSetState(coreRenderer, KZC_RENDERER_STATE_STENCIL_TEST);
            kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_DEPTH_WRITE);
            kzcRendererBeginVertexArray(coreRenderer, KZC_VERTEX_ARRAY_POSITION);
            kzcRendererSetVertexArrayData(coreRenderer, KZC_VERTEX_ARRAY_POSITION, data);
            kzcRendererEndVertexArray(coreRenderer, KZC_PRIMITIVE_TYPE_TRIANGLES, offset / 3);

            {
                kzcMatrix4x4Translate(&transformedMatrix, -eyeDistance, 0.0f, 0.0f);
                kzcMatrix4x4Rotate(&transformedMatrix, rotateAngle, 0.0f, 1.0f, 0.0f);
                kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &transformedMatrix);

                kzuRendererEnableStereoscopic(renderer, renderingMode, 0);
                result = kzuComposerIterateChildren(stereoscopicComposer, renderer, transformedScene, activeWindow);
                kzsErrorForward(result);

                transformedMatrix = originalMatrix;
                kzcMatrix4x4Translate(&transformedMatrix, eyeDistance, 0.0f, 0.0f);
                kzcMatrix4x4Rotate(&transformedMatrix, -rotateAngle, 0.0f, 1.0f, 0.0f);
                kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &transformedMatrix);

                kzuRendererEnableStereoscopic(renderer, renderingMode, 1);
                result = kzuComposerIterateChildren(stereoscopicComposer, renderer, transformedScene, activeWindow);
                kzsErrorForward(result);
                kzuRendererDisableStereoscopic(renderer);
                kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_STENCIL_TEST);

                kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &originalMatrix);

            }
        }
        else if(renderingMode == KZU_STEREOSCOPIC_RENDERING_TOP_BOTTOM ||
            renderingMode == KZU_STEREOSCOPIC_RENDERING_BOTTOM_TOP ||
            renderingMode == KZU_STEREOSCOPIC_RENDERING_LEFT_RIGHT ||
            renderingMode == KZU_STEREOSCOPIC_RENDERING_RIGHT_LEFT)
        {
            kzcMatrix4x4Translate(&transformedMatrix, -eyeDistance, 0.0f, 0.0f);
            kzcMatrix4x4Rotate(&transformedMatrix, rotateAngle, 0.0f, 1.0f, 0.0f);
            kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &transformedMatrix);

            kzuRendererEnableStereoscopic(renderer, renderingMode, 0);
            result = kzuComposerIterateChildren(stereoscopicComposer, renderer, transformedScene, activeWindow);
            kzsErrorForward(result);

            transformedMatrix = originalMatrix;
            kzcMatrix4x4Translate(&transformedMatrix, eyeDistance, 0.0f, 0.0f);
            kzcMatrix4x4Rotate(&transformedMatrix, -rotateAngle, 0.0f, 1.0f, 0.0f);
            kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &transformedMatrix);

            kzuRendererEnableStereoscopic(renderer, renderingMode, 1);
            result = kzuComposerIterateChildren(stereoscopicComposer, renderer, transformedScene, activeWindow);
            kzsErrorForward(result);
            kzuRendererDisableStereoscopic(renderer);

            kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &originalMatrix);
        }
        else if(renderingMode == KZU_STEREOSCOPIC_RENDERING_DISABLED)
        {
            result = kzuComposerIterateChildren(stereoscopicComposer, renderer, transformedScene, activeWindow);
            kzsErrorForward(result);
        }

        result = kzuPropertyQueryPopObject(query);
        kzsErrorForward(result);
    }

    if(renderingMode == KZU_STEREOSCOPIC_RENDERING_ANAGLYPHIC_RED_CYAN &&
        kzuComposerGetChildCount(stereoscopicComposer) > 0)
    {
        struct KzcVector3 points[4];
#ifdef KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS
        const struct KzuPropertyType* anaglyphicColorIntensity = kzuPropertyManagerFindPropertyType(propertyManager, "AnaglyphicColorIntensity");
#endif

        result = kzuPropertyManagerSetVoid(propertyManager, stereoscopicComposerData->anaglyphicMaterial, KZU_PROPERTY_TYPE_TEXTURE, stereoscopicComposerData->stereoscopicEyeLeftTexture);
        kzsErrorForward(result);

        kzuRendererSetDefaultOrthoProjectionAndViewport(renderer);
        {
            kzFloat x, y, width, height;
            kzuRendererGetAdjustedViewport(renderer, activeWindow, &x, &y, &width, &height);
            kzcRendererSetViewport(kzuRendererGetCoreRenderer(renderer), kzsFloatToUint(x), kzsFloatToUint(y),
                kzsFloatToUint(width + 0.5f), kzsFloatToUint(height + 0.5f));
        }

        points[0] = kzcVector3(-1.0f, 1.0f, 0.0f);
        points[1] = kzcVector3( 1.0f, 1.0f, 0.0f);
        points[2] = kzcVector3( 1.0f, -1.0f, 0.0f);
        points[3] = kzcVector3(-1.0f, -1.0f, 0.0f);

        kzcRendererSetCullMode(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_CULL_MODE_NONE);
        kzcRendererDisableState(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_STATE_DEPTH_TEST);

        result = kzuPropertyManagerSetVoid(propertyManager, stereoscopicComposerData->anaglyphicMaterial, KZU_PROPERTY_TYPE_TEXTURE, stereoscopicComposerData->stereoscopicEyeLeftTexture);
        kzsErrorForward(result);
#ifdef KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS
        result = kzuPropertyManagerSetVector3(propertyManager, stereoscopicComposerData->anaglyphicMaterial, anaglyphicColorIntensity, kzcVector3(1.0f, 0.0f, 0.0f));
        kzsErrorForward(result);
#else
        result = kzuPropertyManagerSetColor(propertyManager, stereoscopicComposerData->anaglyphicMaterial, KZU_PROPERTY_TYPE_EMISSIVE, KZC_COLOR_RED);
        kzsErrorForward(result);
#endif

        result = kzuPropertyManagerSetInt(propertyManager, stereoscopicComposerData->anaglyphicMaterial, KZU_PROPERTY_TYPE_BLEND_MODE,
            /*lint -e{641}*/KZC_RENDERER_BLEND_MODE_OPAQUE);
        kzsErrorForward(result);

        result = kzuRendererDrawQuadWithMaterial(renderer, &points[0], &points[1], &points[2], &points[3], stereoscopicComposerData->anaglyphicMaterial, &KZC_MATRIX4X4_IDENTITY);
        kzsErrorForward(result);

        result = kzuPropertyManagerSetVoid(propertyManager, stereoscopicComposerData->anaglyphicMaterial, KZU_PROPERTY_TYPE_TEXTURE, stereoscopicComposerData->stereoscopicEyeRightTexture);
        kzsErrorForward(result);
#ifdef KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS
        result = kzuPropertyManagerSetVector3(propertyManager, stereoscopicComposerData->anaglyphicMaterial, anaglyphicColorIntensity, kzcVector3(0.0f, 1.0f, 1.0f));
        kzsErrorForward(result);
#else
        result = kzuPropertyManagerSetColor(propertyManager, stereoscopicComposerData->anaglyphicMaterial, KZU_PROPERTY_TYPE_EMISSIVE, KZC_COLOR_CYAN);
        kzsErrorForward(result);
#endif
        result = kzuPropertyManagerSetInt(propertyManager, stereoscopicComposerData->anaglyphicMaterial, KZU_PROPERTY_TYPE_BLEND_MODE,
            /*lint -e{641}*/KZC_RENDERER_BLEND_MODE_ADDITIVE);
        kzsErrorForward(result);

        result = kzuRendererDrawQuadWithMaterial(renderer, &points[0], &points[1], &points[2], &points[3], stereoscopicComposerData->anaglyphicMaterial, &KZC_MATRIX4X4_IDENTITY);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

struct KzuComposer* kzuStereoscopicComposerToComposer(struct KzuStereoscopicComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));
    return &composer->composer;
}

struct KzuStereoscopicComposer* kzuStereoscopicComposerFromComposer(const struct KzuComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));
    return (struct KzuStereoscopicComposer*)composer;
}
