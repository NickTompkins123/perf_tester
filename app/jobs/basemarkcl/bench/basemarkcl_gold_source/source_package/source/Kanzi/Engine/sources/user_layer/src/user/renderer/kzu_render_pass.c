/**
* \file
* Specifies a render pass structure. Render pass specifies one stage of rendering with specified charasteristics,
* defined by render pass properties (and filter). Sets of renderpasses are piped via composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
/* FIXME: Remove all void* casts. */
#include "kzu_render_pass.h"

#include "kzu_composer_private.h"
#include "kzu_composer.h"

#include <user/renderer/kzu_renderer.h>
#include <user/renderer/kzu_renderer_util.h>
#include <user/renderer/kzu_shadow_map.h>
#include <user/filter/kzu_object_type_filter.h>
#include <user/filter/kzu_frustum_cull_filter.h>
#include <user/filter/kzu_lod_filter.h>
#include <user/filter/kzu_object_source.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/material/kzu_material.h>
#include <user/material/kzu_material_type.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_light.h>
#include <user/scene_graph/kzu_lod_selector.h>
#include <user/properties/kzu_property_query.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_light_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/kzu_error_codes.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/frame_buffer/kzc_resource_frame_buffer.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/color/kzc_color.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/string/kzc_string.h>
#include <core/renderer/kzc_renderer.h>

#include <system/debug/kzs_log.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_opengl_base.h>


const struct KzuComposerClass KZU_COMPOSER_RENDER_PASS_CLASS =
{
    kzuRenderPassApply,
    kzuRenderPassDelete
};


const KzuComposerType KZU_COMPOSER_TYPE_RENDER_PASS = &KZU_COMPOSER_RENDER_PASS_CLASS;


/** Enumeration for viewport coordinate unit type (exact pixel coordinates / percentages from window) */
enum KzuViewportCoordinateType
{
    /* Note that these values must be synchronized with the corresponding enums in the tool. */
    KZU_VIEWPORT_COORDINATE_ABSOLUTE = 0, /**< Absolute viewport coordinates, i.e. pixel values. */
    KZU_VIEWPORT_COORDINATE_RELATIVE = 1  /**< Relative viewport coordinates, i.e. percentages of render target width & height. */
};

/** Structure for viewport. */
struct KzuViewport
{
    kzFloat x; /**< X coordinate of view port */
    kzFloat y; /**< Y coordinate of view port */
    kzFloat width; /**< Width of view port */
    kzFloat height; /**< Height of view port */
    enum KzuViewportCoordinateType coordinateType;  /**< Type of viewport coordinates */
};

struct KzuRenderPass
{
    struct KzuComposer composer; /**< Inherited composer. */ /*lint -esym(754, KzuRenderPass::composer)*/
};

struct KzuRenderPassData
{
    struct KzuComposerData composerData; /*lint -esym(754, KzuRenderPassData::composerData)*/

    struct KzuCameraNode* cameraNode; /**< Camera for this render pass. */
    struct KzuViewport viewport; /**< View port for this render pass. */
    kzBool cameraErrorLogged; /**< Camera error logged for this render pass. */

    struct KzuObjectSource* objectSource; /**< Object source for render pass. */

    struct KzcTexture* frameBufferTexture; /**< Frame buffer bound to this screen or KZ_NULL if none. */
    kzBool frameBufferViewportCalculatedFromWindow; /**< Is frame buffer viewport calculated from window or framebuffer dimensions. */

    kzBool enabled; /**< Render pass is not in use at the moment. */

    kzBool renderOnce; /**< If true render pass is in state where it is rendered once and disabled after that. */

    struct KzuObjectSource* lodObjectSource; /**< LOD object source. */
    struct KzuObjectSource* lightObjectSource; /**< Light object source. */
    struct KzuObjectSource* renderableObjectSource; /**< Renderable object source. */
    struct KzuObjectSource* frustumCullObjectSource; /**< Frustum cull object source for renderables. */

    struct
    {
        struct KzuLightNode* lightNode;
        kzFloat nearClip;
        kzFloat farClip;
        kzFloat viewAngle;
        kzBool enabled;
    } shadowPass;

    struct KzuMaterial* overrideMaterial;       /**< Override material for render pass. KZ_NULL if not set. */

    KzuRenderPassRenderStepFunction preStepFunction; /**< Pre-step function for render pass. */
    KzuRenderPassRenderStepFunction postStepFunction; /**< Post-step function for render pass. */
};


/** Applies color, depth and stencil buffers of the render pass. */
static kzsError kzuRenderPassApplyBufferSettingsAndCulling_internal(struct KzuRenderPass* renderPass, const struct KzuRenderer* userRenderer,
                                                                    const struct KzcTexture* targetTexture);

/** Down samples a render target to the size specified in the target itself. */
static kzsError kzuRenderPassRenderTargetDownsample_internal(struct KzcTexture* bufferTexture, struct KzcRenderer* coreRenderer);


#define kzuRenderPassGetData_private(renderPass_param) ((struct KzuRenderPassData*)renderPass_param->composer.data)

kzsError kzuRenderPassCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuRenderPass** out_renderPass)
{
    kzsError result;
    struct KzuRenderPass* renderPass;
    struct KzuRenderPassData* renderPassData;
    struct KzuComposer* composer;

    result = kzcMemoryAllocVariable(memoryManager, renderPassData, "Render pass data");
    kzsErrorForward(result);

    result = kzuComposerCreate_private(memoryManager, propertyManager, (struct KzuComposerData*)renderPassData, &KZU_COMPOSER_RENDER_PASS_CLASS, &composer);
    kzsErrorForward(result);
    renderPass = (struct KzuRenderPass*)composer;

    kzsAssert(kzcIsValidPointer(renderPass));

    renderPassData = kzuRenderPassGetData_private(renderPass);

    /* Default camera. */
    renderPassData->cameraNode = KZ_NULL;
    renderPassData->objectSource = KZ_NULL;
    renderPassData->cameraErrorLogged = KZ_FALSE;

    renderPassData->overrideMaterial = KZ_NULL;

    renderPassData->enabled = KZ_TRUE;
    renderPassData->renderOnce = KZ_FALSE;

    /* Setup default frame buffer to none */
    renderPassData->frameBufferTexture = KZ_NULL;
    renderPassData->frameBufferViewportCalculatedFromWindow = KZ_FALSE;

    result = kzuObjectTypeFilterCreate(memoryManager, KZ_NULL, KZU_OBJECT_TYPE_LIGHT, KZ_FALSE, &renderPassData->lightObjectSource);
    kzsErrorForward(result);
    result = kzuLODFilterCreate(memoryManager, KZ_NULL, &renderPassData->lodObjectSource);
    kzsErrorForward(result);
    result = kzuObjectTypeFilterCreate(memoryManager, renderPassData->lodObjectSource, KZU_OBJECT_TYPE_RENDERABLE,
                                       KZ_FALSE, &renderPassData->renderableObjectSource);
    kzsErrorForward(result);
    result = kzuFrustumCullFilterCreate(memoryManager, renderPassData->renderableObjectSource, &renderPassData->frustumCullObjectSource);
    kzsErrorForward(result);

    renderPassData->objectSource = KZ_NULL;

    renderPassData->shadowPass.enabled = KZ_FALSE;
    renderPassData->shadowPass.lightNode = KZ_NULL;
    renderPassData->shadowPass.nearClip = 1.0f;
    renderPassData->shadowPass.farClip = 100.0f;
    renderPassData->shadowPass.viewAngle = 90.0f;

    /* Pre/post step functions. */
    renderPassData->preStepFunction = KZ_NULL;
    renderPassData->postStepFunction = KZ_NULL;

    *out_renderPass = renderPass;
    kzsSuccess();
}

kzsError kzuRenderPassDelete(struct KzuComposer* renderPassComposer)
{
    kzsError result;
    struct KzuRenderPassData* renderPassData;
    struct KzuRenderPass* renderPass;
    kzBool selfOwned;

    renderPass = kzuRenderPassFromComposer(renderPassComposer);
    kzsAssert(renderPass != KZ_NULL);

    renderPassData = kzuRenderPassGetData_private(renderPass);
    selfOwned = renderPassComposer->data->selfOwned;

    result = kzuObjectSourceDelete(renderPassData->lightObjectSource);
    kzsErrorForward(result);
    result = kzuObjectSourceDelete(renderPassData->renderableObjectSource);
    kzsErrorForward(result);
    result = kzuObjectSourceDelete(renderPassData->frustumCullObjectSource);
    kzsErrorForward(result);
    result = kzuObjectSourceDelete(renderPassData->lodObjectSource);
    kzsErrorForward(result);

    result = kzuComposerDelete_private((struct KzuComposer*)renderPass);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(renderPassComposer);
        kzsErrorForward(result);
    }
    else
    {
        renderPassComposer->data = KZ_NULL;
    }

    kzsSuccess();
}

kzBool kzuRenderPassIsValid(const struct KzuRenderPass* renderPass)
{
    return renderPass != KZ_NULL && kzuRenderPassGetData_private(renderPass) != KZ_NULL;
}

struct KzuPropertyManager* kzuRenderPassGetPropertyManager(const struct KzuRenderPass* renderPass)
{
    return kzuComposerGetPropertyManager(kzuRenderPassToComposer((struct KzuRenderPass*)renderPass));
}

static kzsError kzuRenderPassApplyCameraAndViewport_internal(const struct KzuRenderPass* renderPass, const struct KzuRenderer* userRenderer,
                                                             const struct KzuTransformedObjectNode* transformedCameraNode,
                                                             const struct KzsWindow* activeWindow,
                                                             struct KzuViewport viewport, const struct KzcTexture* attachedTexture)
{
    kzsError result;
    struct KzuCameraNode* cameraNode = kzuCameraNodeFromObjectNode(kzuTransformedObjectNodeGetObjectNode(transformedCameraNode));
    struct KzcRenderer* renderer = kzuRendererGetCoreRenderer(userRenderer);
    struct KzuRenderPassData* renderPassData = kzuRenderPassGetData_private(renderPass);
    kzFloat viewportX, viewportY, viewportWidth, viewportHeight;
    kzFloat adjustedViewportX, adjustedViewportY, adjustedViewportWidth, adjustedViewportHeight;

    kzsAssert(kzuRenderPassIsValid(renderPass));

    kzuRendererGetAdjustedViewport(userRenderer, activeWindow, &adjustedViewportX, &adjustedViewportY, &adjustedViewportWidth, &adjustedViewportHeight);

    if(attachedTexture != KZ_NULL && !renderPassData->frameBufferViewportCalculatedFromWindow)
    {
        viewportX = 0.0f;
        viewportY = 0.0f;
        viewportWidth = (kzFloat)kzcTextureGetWidth(attachedTexture);
        viewportHeight = (kzFloat)kzcTextureGetHeight(attachedTexture);
    }
    else
    {
        viewportX = adjustedViewportX;
        viewportY = adjustedViewportY;
        viewportWidth = adjustedViewportWidth;
        viewportHeight = adjustedViewportHeight;

        {
            kzBool stereoscopicEnabled;
            enum KzuStereoscopicRenderingMode mode;
            kzUint pass;
            kzBool flipped;

            kzuRendererGetStereoscopic(userRenderer, &stereoscopicEnabled, &mode, &pass, &flipped);

            if(stereoscopicEnabled)
            {
                if(mode == KZU_STEREOSCOPIC_RENDERING_TOP_BOTTOM)
                {
                    if(pass == 0)
                    {
                        viewportY += flipped ? 0 : viewportHeight / 2.0f;
                        viewportHeight /= 2.0f;
                    }
                    else if(pass == 1)
                    {
                        viewportHeight /= 2.0f;
                    }
                }
                else if(mode == KZU_STEREOSCOPIC_RENDERING_BOTTOM_TOP)
                {
                    if(pass == 1)
                    {
                        viewportY += flipped ? 0 : viewportHeight / 2.0f;
                        viewportHeight /= 2.0f;
                    }
                    else if(pass == 0)
                    {
                        viewportHeight /= 2.0f;
                    }
                }
                else if(mode == KZU_STEREOSCOPIC_RENDERING_LEFT_RIGHT)
                {
                    if(pass == 0)
                    {
                        viewportWidth /= 2.0f;
                    }
                    else if(pass == 1)
                    {
                        viewportX += viewportWidth / 2.0f;
                        viewportWidth /= 2.0f;
                    }
                }
                else if(mode == KZU_STEREOSCOPIC_RENDERING_RIGHT_LEFT)
                {
                    if(pass == 0)
                    {
                        viewportX += viewportWidth / 2.0f;
                        viewportWidth /= 2.0f;
                    }
                    else if(pass == 1)
                    {
                        viewportWidth /= 2.0f;
                    }
                }
            }
        }
    }

    if(kzuCameraNodeGetDisableAspectRatio(cameraNode))
    {
        kzuCameraNodeSetInheritedAspectRatio(cameraNode, adjustedViewportWidth / adjustedViewportHeight);
    }

    /* Setup viewport. */
    switch(viewport.coordinateType)
    {
        /* Set viewport, relative coordinates - calculate from window. */
        case KZU_VIEWPORT_COORDINATE_RELATIVE:
        {
            kzFloat x, y, width, height;

            x = viewportWidth * viewport.x + viewportX;
            y = viewportHeight * viewport.y + viewportY;
            width = viewportWidth * viewport.width;
            height = viewportHeight * viewport.height;

            kzcRendererSetViewport(renderer, kzsFloatToUint(x), kzsFloatToUint(y),
                                             kzsFloatToUint(width + 0.5f), kzsFloatToUint(height + 0.5f));
            break;
        }
        /* Set viewport, absolute coordinates - set as-is. */
        case KZU_VIEWPORT_COORDINATE_ABSOLUTE:
        {
            kzcRendererSetViewport(renderer,
                kzsFloatToUint(viewportX + viewport.x), 
                kzsFloatToUint(viewportY + viewport.y),
                kzsFloatToUint(viewport.width), 
                kzsFloatToUint(viewport.height));
            break;
        }
    }

    result = kzuRendererApplyCameraNode(userRenderer, transformedCameraNode, viewportX, viewportY, viewportWidth, viewportHeight);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuRenderPassApplyRenderable_internal(struct KzuTransformedObjectNode* transformedObjectNode, struct KzuRenderer* renderer)
{
    kzsError result;
    kzBool visible;
    struct KzuObjectNode* objectNode = kzuTransformedObjectNodeGetObjectNode(transformedObjectNode);

    visible = kzuPropertyQueryGetBool(kzuRendererGetPropertyQuery(renderer), KZU_PROPERTY_TYPE_VISIBLE);

    if (visible)
    {
        struct KzcMatrix4x4 objectWorldMatrix = kzuTransformedObjectNodeGetMatrix(transformedObjectNode);

        kzcRendererSetMatrix(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_MATRIX_WORLD, &objectWorldMatrix);

        result = kzuObjectNodeRender(renderer, transformedObjectNode);
        kzsErrorForward(result);

        /* Bounding box visualization. */
        if (kzuRendererIsBoundingBoxVisualizationEnabled(renderer))
        {
            struct KzuBoundingVolume* boundingVolume = kzuObjectNodeGetBoundingVolume(objectNode);

            result = kzuRendererDrawBoundingVolume(renderer, boundingVolume, objectWorldMatrix);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

KZ_CALLBACK static kzsError kzuRenderPassApplyLight_internal(struct KzuTransformedObjectNode* transformedObjectNode,
                                                             struct KzuRenderer* renderer)
{
    kzsError result;
    struct KzuLightNode* lightNode;
    struct KzuObjectNode* objectNode;
    struct KzcMatrix4x4 objectWorldMatrix;

    /* Get the original object node from transformed object node. */
    objectNode = kzuTransformedObjectNodeGetObjectNode(transformedObjectNode);
    lightNode = kzuLightNodeFromObjectNode(objectNode);

    objectWorldMatrix = kzuTransformedObjectNodeGetMatrix(transformedObjectNode);

    result = kzuRendererApplyLightProperty(renderer, lightNode, &objectWorldMatrix);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuRenderPassIterateTransformedObjects_internal(struct KzuRenderer* renderer,
                                                                const struct KzcDynamicArray* transformedObjects, 
                                                                KzuRendererApplyObjectFunction applyFunction)
{
    struct KzcDynamicArrayIterator it;
    struct KzuTransformedObjectNode* transformedObjectNode;
    struct KzuPropertyQuery* propertyQuery = kzuRendererGetPropertyQuery(renderer);
    kzsError result;

    it = kzcDynamicArrayGetIterator(transformedObjects);
    /* Loop the mesh list and send them to renderer. */
    while (kzcDynamicArrayIterate(it))
    {
        transformedObjectNode = (struct KzuTransformedObjectNode*)kzcDynamicArrayIteratorGetValue(it);
        kzsAssert(kzcIsValidPointer(transformedObjectNode));

        /* TODO: make a filter for this. */
        if (kzuTransformedObjectNodeIsClipped(transformedObjectNode))
        {
            continue;
        }

        /* Push object properties. */
        result = kzuPropertyQueryPushObjectNode(propertyQuery, kzuTransformedObjectNodeGetObjectNode(transformedObjectNode));
        kzsErrorForward(result);
        
        result = applyFunction(transformedObjectNode, renderer);
        kzsErrorForward(result);

        /* Pop object properties. */
        result = kzuPropertyQueryPopObject(propertyQuery);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuRenderPassApply(const struct KzuComposer* renderPassComposer, struct KzuRenderer* renderer,
                            const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow)
{
    kzsError result;
    struct KzcDynamicArray* objectList;
    struct KzuTransformedObjectNode* transformedCameraNode;
    struct KzuCameraNode* cameraNode;
    struct KzcMatrix4x4 originalCameraTransformation;
    kzFloat originalFieldOfView = 0.0f;
    kzFloat originalNearClip = 0.0f;
    kzFloat originalFarClip = 0.0f;
    const struct KzuRenderPass* renderPass;
    struct KzuRenderPassData* renderPassData;
    struct KzcDynamicArray* transformedObjects = kzuTransformedSceneGetObjects(transformedScene);

    renderPass = kzuRenderPassFromComposer(renderPassComposer);
    kzsAssert(renderPass != KZ_NULL);
    kzsAssert(kzuRenderPassIsValid(renderPass));

    renderPassData = kzuRenderPassGetData_private(renderPass);

    if(renderPassData->enabled)
    {
        /** Render pass functionality:

            1) Set viewport according to render pass settings, apply to renderer.
            2) Apply buffers (buffer means color/depth/stencil buffers).
            3) Apply camera for renderer (calculate camera & projection matrices, set to renderer).
            4) Apply render pass properties for property pipeline.
            5) Apply render target.
            6) Fetch all lights from object source. For each light:
                - Apply lights for user renderer.
            7) Fetch all renderables from object source. For each renderable:
               - Apply object properties (properties are hierarchically inherited in scene graph extract phase).
               - Apply material properties.
               - Apply material for core renderer (fixed functionality = set states, 
                 programmable pipeline = apply shader + set states such as blend).
               - Set object's world matrix for renderer.
               - Apply renderer transformations.
               - Fetch mesh index and vertex buffers, apply for renderer for rendering purposes.
               - Detach material properties.
               - Detach object properties.
            9) Pop render pass properties.
        */

        /* Set material override for render pass. Also KZ_NULL must be passed since we also disable overrides if changed. */
        if(renderPassData->overrideMaterial != KZ_NULL)
        {
            kzuRendererSetMaterialOverride(renderer, renderPassData->overrideMaterial);
        }

        result = kzuRendererResetPass(renderer);
        kzsErrorForward(result);

        if(renderPassData->preStepFunction != KZ_NULL)
        {
            result = renderPassData->preStepFunction(renderer, transformedScene, transformedObjects, activeWindow);
            kzsErrorForward(result);
        }

        /* Apply camera. */
        if (renderPassData->cameraNode == KZ_NULL)
        {
            struct KzuScene* scene = kzuTransformedSceneGetScene(transformedScene);
            if (kzuPropertyManagerGetBoolDefault(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_RENDER_PASS_IS_2D))
            {
                cameraNode = kzuSceneGetViewCamera2D(scene);
            }
            else
            {
                cameraNode = kzuSceneGetViewCamera(scene);
            }
            kzsErrorTest(cameraNode != KZ_NULL, KZU_ERROR_INVALID_SCENE_DATA, "No valid camera assigned for scene");
        }
        else
        {
            cameraNode = renderPassData->cameraNode;
        }

        /* Set shadow pass enabled for renderer and store previous camera values. */
        if(renderPassData->shadowPass.enabled && renderPassData->shadowPass.lightNode != KZ_NULL)
        {
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
            kzuShadowMapSetDepthPass(kzuRendererGetShadowMap(renderer), KZ_TRUE);
#endif

            originalFieldOfView = kzuCameraNodeGetFov(cameraNode);
            originalNearClip = kzuCameraNodeGetZNear(cameraNode);
            originalFarClip = kzuCameraNodeGetZFar(cameraNode);
            result = kzuCameraNodeSetFov(cameraNode, renderPassData->shadowPass.viewAngle);
            kzsErrorForward(result);
            result = kzuCameraNodeSetZNear(cameraNode, renderPassData->shadowPass.nearClip);
            kzsErrorForward(result);
            result = kzuCameraNodeSetZFar(cameraNode, renderPassData->shadowPass.farClip);
            kzsErrorForward(result);
        }

        transformedCameraNode = kzuTransformedObjectListFindObject(transformedObjects, kzuCameraNodeToObjectNode(cameraNode));

        if(transformedCameraNode != KZ_NULL)
        {
            kzBool overrideUsed = KZ_FALSE;
            struct KzcTexture* renderPassBufferTexture;
            struct KzcTexture* targetBufferTexture;
            struct KzcTexture* overrideRenderTargetTexture;
            struct KzuViewport viewport = renderPassData->viewport;
            kzBool stereoscopicEnabled;
            enum KzuStereoscopicRenderingMode stereoscopicMode;
            kzFloat originalX, originalY, originalWidth, originalHeight;
            enum KzuViewportCoordinateType originalType;
            kzUint dummy;
            kzBool bDummy;

            viewport.x = kzuPropertyManagerGetFloatDefault(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_VIEWPORT_X);
            viewport.y = kzuPropertyManagerGetFloatDefault(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_VIEWPORT_Y);
            viewport.width = kzuPropertyManagerGetFloatDefault(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_VIEWPORT_WIDTH);
            viewport.height = kzuPropertyManagerGetFloatDefault(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_VIEWPORT_HEIGHT);
            viewport.coordinateType = (enum KzuViewportCoordinateType)kzuPropertyManagerGetIntDefault(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_VIEWPORT_TYPE);/*lint !e930*/

            originalX = viewport.x;
            originalY = viewport.y;
            originalWidth = viewport.width;
            originalHeight = viewport.height;
            originalType = viewport.coordinateType;

            kzuRendererGetStereoscopic(renderer, &stereoscopicEnabled, &stereoscopicMode, &dummy, &bDummy);

            renderPassBufferTexture = kzuRenderPassGetAttachedFrameBuffer(renderPass);

            result = kzuRendererPeekOverrideScreenTargetTexture(renderer, &overrideRenderTargetTexture);
            kzsErrorForward(result);

            /* Find the overridden framebuffer texture. */
            if(overrideRenderTargetTexture != KZ_NULL && renderPassBufferTexture == KZ_NULL)
            {
                targetBufferTexture = overrideRenderTargetTexture;
                overrideUsed = KZ_TRUE;
            }
            else
            {
                targetBufferTexture = renderPassBufferTexture;
            }

            if(targetBufferTexture != KZ_NULL)
            {
                struct KzcFrameBuffer* attachedFrameBuffer = kzcTextureGetFrameBufferReference(targetBufferTexture);
                kzcFrameBufferBind(attachedFrameBuffer, kzuRendererGetCoreRenderer(renderer));
                /* Override the default viewport coordinates. */
                if(overrideUsed)
                {
                    viewport.coordinateType = KZU_VIEWPORT_COORDINATE_ABSOLUTE;
                    viewport.x = 0.0f;
                    viewport.y = 0.0f;
                    viewport.width = (kzFloat)kzcFrameBufferGetWidth(attachedFrameBuffer);
                    viewport.height = (kzFloat)kzcFrameBufferGetHeight(attachedFrameBuffer);
                }
            }

            originalCameraTransformation = kzuTransformedObjectNodeGetMatrix(transformedCameraNode);

            if(renderPassData->shadowPass.enabled && renderPassData->shadowPass.lightNode != KZ_NULL)
            {
                struct KzcMatrix4x4 lightTransformation;
                struct KzuTransformedObjectNode* transformedLightNode = kzuTransformedObjectListFindObject(transformedObjects, kzuLightNodeToObjectNode(
                    renderPassData->shadowPass.lightNode));

                if(transformedLightNode != KZ_NULL)
                {
                    lightTransformation = kzuTransformedObjectNodeGetMatrix(transformedLightNode);
                    kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &lightTransformation);
                }
            }

            result = kzuRenderPassApplyCameraAndViewport_internal(renderPass, renderer, transformedCameraNode, activeWindow, viewport, targetBufferTexture);
            kzsErrorForward(result);

            kzuTransformedObjectNodeSetMatrix(transformedCameraNode, &originalCameraTransformation);

            /* Apply buffers. */
            result = kzuRenderPassApplyBufferSettingsAndCulling_internal((struct KzuRenderPass*)renderPass, renderer,
                overrideRenderTargetTexture);
            kzsErrorForward(result);

#ifdef KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS
            result = kzuShadowMapPrepare(kzuRendererGetShadowMap(renderer), renderer);
            kzsErrorForward(result);
#endif

            kzuRendererSetTransformedObjectList(renderer, transformedObjects);

            /* Fetch light object source. */
            {
                result = kzuObjectSourceFetchGraph(renderPassData->lightObjectSource,
                                                   kzuTransformedSceneGetObjectSourceRuntimeData(transformedScene),
                                                   transformedCameraNode, &objectList);
                kzsErrorForward(result);

                result = kzuRenderPassIterateTransformedObjects_internal(renderer, objectList, kzuRenderPassApplyLight_internal);
                kzsErrorForward(result);
            }

            /* Fetch renderable object source, apply for rendering. */
            /* See if frustum cull is enabled. */
            if(kzuPropertyManagerGetBoolDefault(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_FRUSTUM_CULLING_ON))
            {
                result = kzuObjectSourceFetchGraph(renderPassData->frustumCullObjectSource,
                                                   kzuTransformedSceneGetObjectSourceRuntimeData(transformedScene),
                                                   transformedCameraNode, &objectList);
                kzsErrorForward(result);
            }
            else
            {
                result = kzuObjectSourceFetchGraph(renderPassData->renderableObjectSource,
                                                   kzuTransformedSceneGetObjectSourceRuntimeData(transformedScene),
                                                   transformedCameraNode, &objectList);
                kzsErrorForward(result);
            }

            /* Iterate renderable objects. */
            result = kzuRenderPassIterateTransformedObjects_internal(renderer, objectList, kzuRenderPassApplyRenderable_internal);
            kzsErrorForward(result);

            /* Restore shadow pass information. */
            if(renderPassData->shadowPass.enabled)
            {
                result = kzuCameraNodeSetFov(cameraNode, originalFieldOfView);
                kzsErrorForward(result);
                result = kzuCameraNodeSetZNear(cameraNode, originalNearClip);
                kzsErrorForward(result);
                result = kzuCameraNodeSetZFar(cameraNode, originalFarClip);
                kzsErrorForward(result);
            }

            viewport.x = originalX;
            viewport.y = originalY;
            viewport.width = originalWidth;
            viewport.height = originalHeight;
            viewport.coordinateType = originalType;

            if(renderPassData->postStepFunction != KZ_NULL)
            {
                result = renderPassData->postStepFunction(renderer, transformedScene, transformedObjects, activeWindow);
                kzsErrorForward(result);
            }

            /* Unbind frame buffer if needed */
            if(targetBufferTexture != KZ_NULL)
            {
                struct KzuMaterial* texturedMaterial = kzuRendererGetTexturedMaterial(renderer);
                struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(texturedMaterial);
                struct KzcFrameBuffer* attachedFrameBuffer = kzcTextureGetFrameBufferReference(targetBufferTexture);
                result = kzuPropertyManagerSetVoid(propertyManager, texturedMaterial, KZU_PROPERTY_TYPE_TEXTURE, targetBufferTexture);
                kzsErrorForward(result); 
                kzcFrameBufferUnbind(attachedFrameBuffer, kzuRendererGetCoreRenderer(renderer));
                
                result = kzuRendererApplyMaterial(renderer, kzuRendererGetTexturedMaterial(renderer));
                kzsErrorForward(result); 
                result = kzuRenderPassRenderTargetDownsample_internal(targetBufferTexture, kzuRendererGetCoreRenderer(renderer));
                kzsErrorForward(result); 
            } 
        }
        else
        {
            if(!renderPassData->cameraErrorLogged)
            {
                renderPassData->cameraErrorLogged = KZ_TRUE;
                if(renderPassData->cameraNode == KZ_NULL)
                {
                    kzsLog(KZS_LOG_LEVEL_WARNING, "Scene view camera not defined while using scene view camera in render pass");
                }
                else
                {
                    kzMutableString logString;
                    kzString cameraName = kzuObjectNodeGetName(kzuCameraNodeToObjectNode(renderPassData->cameraNode));
                    if (cameraName == KZ_NULL)
                    {
                        cameraName = "Unnamed";
                    }
                    result = kzcStringFormat(kzcMemoryGetManager(renderPass), "Camera node %s not found from active scene, but was set for render pass", &logString, cameraName);
                    kzsErrorForward(result);
                    kzsLog(KZS_LOG_LEVEL_WARNING, logString);
                    result = kzcStringDelete(logString);
                    kzsErrorForward(result);
                }
            }
        }

        if(renderPassData->overrideMaterial != KZ_NULL)
        {
            kzuRendererSetMaterialOverride(renderer, KZ_NULL);
        }

        if(renderPassData->renderOnce)
        {
            renderPassData->enabled = KZ_FALSE;
            renderPassData->renderOnce = KZ_FALSE;
        }
    }

    result = kzuComposerIterateChildren(renderPassComposer, renderer, transformedScene, activeWindow);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuRenderPassRenderTargetDownsample_internal(struct KzcTexture* bufferTexture, struct KzcRenderer* coreRenderer)
{
    kzsError result;
    kzUint sample = 0;
    struct KzcTexture* downSample;
    struct KzcTexture* lastTexture;

    kzsAssert(kzcIsValidPointer(bufferTexture));

    downSample = kzcTextureGetFrameBufferDownscaleTarget(bufferTexture, 0);
    lastTexture = bufferTexture;

    while(downSample != KZ_NULL)
    {
        struct KzcFrameBuffer* downsampleFrameBuffer = kzcTextureGetFrameBufferReference(downSample);

        kzcFrameBufferBind(downsampleFrameBuffer, coreRenderer);

        kzcRendererSetViewport(coreRenderer, 0, 0, kzcTextureGetWidth(downSample), kzcTextureGetHeight(downSample));

        result = kzcRendererDrawTexturedQuad2DAbsolute(coreRenderer, lastTexture, 0.0f, 0.0f, 
            (kzFloat)kzcTextureGetWidth(downSample), (kzFloat)kzcTextureGetHeight(downSample), 0.0f, 0.0f, 1.0f, 1.0f);
        kzsErrorForward(result);

        kzcFrameBufferUnbind(downsampleFrameBuffer, coreRenderer);

        ++sample;
        lastTexture = downSample;
        downSample = kzcTextureGetFrameBufferDownscaleTarget(bufferTexture, sample);
    }

    kzsSuccess();
}

static kzsError kzuRenderPassApplyBufferSettingsAndCulling_internal(struct KzuRenderPass* renderPass, const struct KzuRenderer* userRenderer,
                                                                    const struct KzcTexture* targetTexture)
{
    struct KzcRenderer* renderer = kzuRendererGetCoreRenderer(userRenderer);
    struct KzuComposer* composer = kzuRenderPassToComposer(renderPass);
    kzBool clearEnabled;
    struct KzcColorRGBA clearColor;
    enum KzuCullMode cullMode;
    enum KzuColorWriteMode colorWrite;

    kzBool depthClear;
    kzBool depthTest;
    kzBool depthWrite;
    kzFloat depthClearValue;

    kzBool stencilClearEnabled;
    kzBool stencilTestEnabled;
    kzInt stencilClearValue;
    enum KzuStencilOperation stencilOperation;
    enum KzuStencilFunction stencilFunction;

    clearEnabled = kzuPropertyManagerGetBoolDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_ENABLED);
    clearColor = kzuPropertyManagerGetColorDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_COLOR);
    cullMode = (enum KzuCullMode)kzuPropertyManagerGetIntDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_CULL_MODE);/*lint !e930*/
    colorWrite = (enum KzuColorWriteMode)kzuPropertyManagerGetIntDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_COLOR_WRITE_MODE);/*lint !e930*/

    depthClear = kzuPropertyManagerGetBoolDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_DEPTH_BUFFER_CLEAR_ENABLED);
    depthTest = kzuPropertyManagerGetBoolDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_DEPTH_BUFFER_TEST_ENABLED);
    depthWrite = kzuPropertyManagerGetBoolDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_DEPTH_BUFFER_WRITE_ENABLED);
    depthClearValue = kzuPropertyManagerGetFloatDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_DEPTH_BUFFER_CLEAR_VALUE);

    stencilClearEnabled = kzuPropertyManagerGetBoolDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_STENCIL_BUFFER_CLEAR_ENABLED);
    stencilTestEnabled = kzuPropertyManagerGetBoolDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_STENCIL_BUFFER_TEST_ENABLED);
    stencilClearValue = kzuPropertyManagerGetIntDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_STENCIL_BUFFER_CLEAR_VALUE);
    stencilOperation = (enum KzuStencilOperation)kzuPropertyManagerGetIntDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_STENCIL_WRITE_MODE);/*lint !e930*/
    stencilFunction = (enum KzuStencilFunction)kzuPropertyManagerGetIntDefault(kzuComposerGetPropertyManager(composer), composer, KZU_PROPERTY_TYPE_STENCIL_FUNCTION);/*lint !e930*/

    kzcRendererResetClearTarget(renderer);

    /* Apply color buffer settings. */
    if(clearEnabled)
    {
        struct KzcColorRGBA overrideClearColor;
        kzcRendererAddClearTarget(renderer, KZC_RENDERER_CLEAR_TARGET_COLOR);
        if(kzuRendererGetClearColorOverride(userRenderer, &overrideClearColor))
        {
            kzcRendererSetClearColor(renderer, overrideClearColor);
        }
        else
        {
            kzcRendererSetClearColor(renderer, clearColor);
        }
        
    }

    /* Apply culling mode. */
    switch(cullMode)
    {
        case KZU_CULL_NONE:
        {
            kzcRendererSetCullMode(renderer, KZC_RENDERER_CULL_MODE_NONE);
            break;
        }
        case KZU_CULL_BACK:
        {
            kzcRendererSetCullMode(renderer, KZC_RENDERER_CULL_MODE_BACK);
            break;
        }
        case KZU_CULL_FRONT:
        {
            kzcRendererSetCullMode(renderer, KZC_RENDERER_CULL_MODE_FRONT);
            break;
        }
    }

    switch(colorWrite)
    {
        case KZU_COLOR_WRITE_NONE:
        {
            kzcRendererSetColorWriteMode(KZC_RENDERER_COLOR_WRITE_NONE);
            break;
        }
        case KZU_COLOR_WRITE_RGB:
        {
            kzcRendererSetColorWriteMode(KZC_RENDERER_COLOR_WRITE_RGB);
            break;
        }
        case KZU_COLOR_WRITE_RGBA:
        {
            kzcRendererSetColorWriteMode(KZC_RENDERER_COLOR_WRITE_RGBA);
            break;
        }
        case KZU_COLOR_WRITE_ALPHA:
        {
            kzcRendererSetColorWriteMode(KZC_RENDERER_COLOR_WRITE_ALPHA);
            break;
        }
        case KZU_COLOR_WRITE_RED:
        {
            kzcRendererSetColorWriteMode(KZC_RENDERER_COLOR_WRITE_RED);
            break;
        }
        case KZU_COLOR_WRITE_GREEN:
        {
            kzcRendererSetColorWriteMode(KZC_RENDERER_COLOR_WRITE_GREEN);
            break;
        }
        case KZU_COLOR_WRITE_BLUE:
        {
            kzcRendererSetColorWriteMode(KZC_RENDERER_COLOR_WRITE_BLUE);
            break;
        }
        case KZU_COLOR_WRITE_GRAYSCALE:
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown color buffer color write mode!");
        }
    }

    /* Apply depth buffer settings. */
    if(depthClear)
    {
        kzcRendererAddClearTarget(renderer, KZC_RENDERER_CLEAR_TARGET_DEPTH);
        kzcRendererSetClearDepthValue(renderer, depthClearValue);
    }

    /* Depth testing state. */
    if(depthTest)
    {
        kzcRendererSetState(renderer, KZC_RENDERER_STATE_DEPTH_TEST);
    }
    else
    {
        kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_TEST);
    }

    /* Set depth writing on / off. */
    if(depthWrite)
    {
        kzcRendererSetState(renderer, KZC_RENDERER_STATE_DEPTH_WRITE);
    }
    else
    {
        kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_WRITE);
    }

    /* Apply stencil buffer settings. */
    if(stencilClearEnabled)
    {
        kzcRendererAddClearTarget(renderer, KZC_RENDERER_CLEAR_TARGET_STENCIL);
        kzcRendererSetClearStencil(renderer, stencilClearValue);
    }

    if(stencilTestEnabled)
    {
        kzcRendererSetState(renderer, KZC_RENDERER_STATE_STENCIL_TEST);
    }
    else
    {
        kzcRendererDisableState(renderer, KZC_RENDERER_STATE_STENCIL_TEST);
    }

    /* Stencil operation. */
    switch(stencilOperation)
    {
        case KZU_STENCIL_OPERATION_KEEP:
        {
            kzcRendererSetStencilOperation(renderer, KZC_RENDERER_STENCIL_OPERATION_KEEP, KZC_RENDERER_STENCIL_OPERATION_KEEP, KZC_RENDERER_STENCIL_OPERATION_KEEP);
            break;
        }
        case KZU_STENCIL_OPERATION_INCREASE_ON_ZPASS:
        {
            kzcRendererSetStencilOperation(renderer, KZC_RENDERER_STENCIL_OPERATION_KEEP, KZC_RENDERER_STENCIL_OPERATION_KEEP, KZC_RENDERER_STENCIL_OPERATION_INCREASE_WRAP);
            break;
        }
        case KZU_STENCIL_OPERATION_DECREASE_ON_ZPASS:
        {
            kzcRendererSetStencilOperation(renderer, KZC_RENDERER_STENCIL_OPERATION_KEEP, KZC_RENDERER_STENCIL_OPERATION_KEEP, KZC_RENDERER_STENCIL_OPERATION_DECREASE_WRAP);
            break;
        }
        case KZU_STENCIL_OPERATION_INCREASE_ON_ZFAIL:
        {
            kzcRendererSetStencilOperation(renderer, KZC_RENDERER_STENCIL_OPERATION_KEEP, KZC_RENDERER_STENCIL_OPERATION_INCREASE_WRAP, KZC_RENDERER_STENCIL_OPERATION_KEEP);
            break;
        }
        case KZU_STENCIL_OPERATION_DECREASE_ON_ZFAIL:
        {
            kzcRendererSetStencilOperation(renderer, KZC_RENDERER_STENCIL_OPERATION_KEEP, KZC_RENDERER_STENCIL_OPERATION_DECREASE_WRAP, KZC_RENDERER_STENCIL_OPERATION_KEEP);
            break;
        }
        case KZU_STENCIL_OPERATION_ZERO:
        {
            kzcRendererSetStencilOperation(renderer, KZC_RENDERER_STENCIL_OPERATION_ZERO, KZC_RENDERER_STENCIL_OPERATION_ZERO, KZC_RENDERER_STENCIL_OPERATION_ZERO);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown stencil buffer operation!");
        }
    }

    /* Stencil function. */
    switch(stencilFunction)
    {
        /* TODO: Move the function call out from the switch/case */
        case KZU_STENCIL_FUNCTION_ALWAYS:
        {
            kzcRendererSetStencilFunction(renderer, KZC_RENDERER_STENCIL_MODE_ALWAYS, 0, 0xFF);
            break;
        }
        case KZU_STENCIL_FUNCTION_NEVER:
        {
            kzcRendererSetStencilFunction(renderer, KZC_RENDERER_STENCIL_MODE_NEVER, 0, 0xFF);
            break;
        }
        case KZU_STENCIL_FUNCTION_LESS:
        {
            kzcRendererSetStencilFunction(renderer, KZC_RENDERER_STENCIL_MODE_LESS, 0, 0xFF);
            break;
        }
        case KZU_STENCIL_FUNCTION_LEQUAL:
        {
            kzcRendererSetStencilFunction(renderer, KZC_RENDERER_STENCIL_MODE_LEQUAL, 0, 0xFF);
            break;
        }
        case KZU_STENCIL_FUNCTION_GREATER:
        {
            kzcRendererSetStencilFunction(renderer, KZC_RENDERER_STENCIL_MODE_GREATER, 0, 0xFF);
            break;
        }
        case KZU_STENCIL_FUNCTION_GEQUAL:
        {
            kzcRendererSetStencilFunction(renderer, KZC_RENDERER_STENCIL_MODE_GEQUAL, 0, 0xFF);
            break;
        }
        case KZU_STENCIL_FUNCTION_EQUAL:
        {
            kzcRendererSetStencilFunction(renderer, KZC_RENDERER_STENCIL_MODE_EQUAL, 0, 0xFF);
            break;
        }
        case KZU_STENCIL_FUNCTION_NOT_EQUAL:
        {
            kzcRendererSetStencilFunction(renderer, KZC_RENDERER_STENCIL_MODE_NOT_EQUAL, 0, 0xFF);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unknown stencil buffer function!");
        }
    }

    /* Stereoscopic modes. */
    if(targetTexture == KZ_NULL)
    {
        kzBool stereoscopicEnabled;
        enum KzuStereoscopicRenderingMode mode;
        kzUint pass;
        kzBool flipped;
        kzuRendererGetStereoscopic(userRenderer, &stereoscopicEnabled, &mode, &pass, &flipped);

        if(stereoscopicEnabled)
        {
            if(mode == KZU_STEREOSCOPIC_RENDERING_ANAGLYPHIC_RED_CYAN)
            {
                if(!flipped)
                {
                    kzcRendererSetColorWriteMode(pass == 0 ? KZC_RENDERER_COLOR_WRITE_RED : KZC_RENDERER_COLOR_WRITE_CYAN);
                }
                else
                {
                    kzcRendererSetColorWriteMode(pass == 1 ? KZC_RENDERER_COLOR_WRITE_RED : KZC_RENDERER_COLOR_WRITE_CYAN);
                }
            }
            else if(mode == KZU_STEREOSCOPIC_RENDERING_VERTICAL_LINE_INTERLACED ||
                    mode == KZU_STEREOSCOPIC_RENDERING_HORIZONTAL_LINE_INTERLACED)
            {
                kzcRendererDisableState(renderer, KZC_RENDERER_STATE_STENCIL_WRITE);
                kzcRendererSetState(renderer, KZC_RENDERER_STATE_STENCIL_TEST);
                kzcRendererSetStencilFunction(renderer, KZC_RENDERER_STENCIL_MODE_EQUAL, flipped ? 1 - pass : pass, 0xFF);
                kzcRendererSetStencilOperation(renderer, KZC_RENDERER_STENCIL_OPERATION_KEEP, KZC_RENDERER_STENCIL_OPERATION_KEEP, KZC_RENDERER_STENCIL_OPERATION_KEEP);

                if(pass == 1)
                {
                    kzcRendererRemoveClearTarget(renderer, KZC_RENDERER_CLEAR_TARGET_COLOR);
                }
            }
            else if(mode == KZU_STEREOSCOPIC_RENDERING_TOP_BOTTOM ||
                    mode == KZU_STEREOSCOPIC_RENDERING_BOTTOM_TOP ||
                    mode == KZU_STEREOSCOPIC_RENDERING_LEFT_RIGHT ||
                    mode == KZU_STEREOSCOPIC_RENDERING_RIGHT_LEFT)
            {
                if(pass == 1)
                {
                    kzFloat x, y, width, height;
                    kzcRendererGetViewport(renderer, &x, &y, &width, &height);
                    kzcRendererSetScissorTest(renderer, KZ_TRUE, (kzUint)x, (kzUint)y, (kzUint)width, (kzUint)height);
                }
            }
        }
    }

    /* Apply buffer clear. */
    if(kzcRendererIsClearEnabled(renderer))
    {
        kzcRendererClear(renderer);
    }
    kzcRendererSetScissorTest(renderer, KZ_FALSE, 0, 0, 0, 0);

    kzsSuccess();
}

void kzuRenderPassSetCamera(const struct KzuRenderPass* renderPass, struct KzuCameraNode* camera)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    kzsAssert(kzcIsValidPointer(camera));
    kzuRenderPassGetData_private(renderPass)->cameraNode = camera;
}

struct KzuCameraNode* kzuRenderPassGetCamera(const struct KzuRenderPass* renderPass)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    return kzuRenderPassGetData_private(renderPass)->cameraNode;
}

kzsError kzuRenderPassSetObjectSource(const struct KzuRenderPass* renderPass, struct KzuObjectSource* objectSource)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    kzsAssert(kzcIsValidPointer(objectSource));
    
    kzuRenderPassGetData_private(renderPass)->objectSource = objectSource;

    /* Add input data for renderpass specific object sources. */
    kzuFilterObjectSourceSetInput(kzuRenderPassGetData_private(renderPass)->lodObjectSource, kzuRenderPassGetData_private(renderPass)->objectSource);
    kzuFilterObjectSourceSetInput(kzuRenderPassGetData_private(renderPass)->lightObjectSource, kzuRenderPassGetData_private(renderPass)->objectSource);

    /* Add render pass' object sources to object source manager if it was changed. */
    kzsSuccess();
}

struct KzuObjectSource* kzuRenderPassGetObjectSource(const struct KzuRenderPass* renderPass)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    return kzuRenderPassGetData_private(renderPass)->objectSource;
}

kzsError kzuRenderPassSetColorBuffer(const struct KzuRenderPass* renderPass, kzBool clearEnabled, struct KzcColorRGBA clearColor, 
                                     enum KzuColorWriteMode writeMode)
{
    kzsError result;

    kzsAssert(kzuRenderPassIsValid(renderPass));
    
    result = kzuPropertyManagerSetColor(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_COLOR, clearColor);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetBool(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_ENABLED, clearEnabled);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetInt(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_COLOR_WRITE_MODE, /*lint -e{930}*/(kzInt)writeMode);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcColorRGBA kzuRenderPassGetClearColor(const struct KzuRenderPass* renderPass)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));

    return kzuPropertyManagerGetColorDefault(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_COLOR);
}

enum KzuColorWriteMode kzuRenderPassGetColorWriteMode(const struct KzuRenderPass* renderPass)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));

    return /*lint -e{930}*/(enum KzuColorWriteMode)kzuPropertyManagerGetIntDefault(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_COLOR_WRITE_MODE);
}

kzBool kzuRenderPassGetColorClearEnabled(const struct KzuRenderPass* renderPass)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));

    return kzuPropertyManagerGetBoolDefault(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_ENABLED);
}

kzsError kzuRenderPassSetDepthBuffer(const struct KzuRenderPass* renderPass, kzBool depthClearEnabled, kzFloat clearValue,
                                     kzBool depthTestEnabled, kzBool depthWriteEnabled)
{
    kzsError result;

    kzsAssert(kzuRenderPassIsValid(renderPass));

    result = kzuPropertyManagerSetBool(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_DEPTH_BUFFER_CLEAR_ENABLED, depthClearEnabled);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetFloat(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_DEPTH_BUFFER_CLEAR_VALUE, clearValue);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetBool(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_DEPTH_BUFFER_TEST_ENABLED, depthTestEnabled);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetBool(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_DEPTH_BUFFER_WRITE_ENABLED, depthWriteEnabled);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuRenderPassSetStencilBuffer(const struct KzuRenderPass* renderPass, kzBool stencilClearEnabled, kzInt clearValue,
                                       kzBool stencilTestEnabled, enum KzuStencilOperation stencilOperation,
                                       enum KzuStencilFunction stencilFunction)
{
    kzsError result;

    kzsAssert(kzuRenderPassIsValid(renderPass));

    result = kzuPropertyManagerSetBool(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_STENCIL_BUFFER_CLEAR_ENABLED, stencilClearEnabled);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetInt(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_STENCIL_BUFFER_CLEAR_VALUE, clearValue);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetBool(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_STENCIL_BUFFER_TEST_ENABLED, stencilTestEnabled);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetInt(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_STENCIL_WRITE_MODE, /*lint -e{930}*/(kzInt)stencilOperation);
    kzsErrorForward(result);
    result = kzuPropertyManagerSetInt(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_STENCIL_FUNCTION, /*lint -e{930}*/(kzInt)stencilFunction);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuRenderPassSetCullMode(const struct KzuRenderPass* renderPass, enum KzuCullMode mode)
{
    kzsError result;

    kzsAssert(kzuRenderPassIsValid(renderPass));

    result = kzuPropertyManagerSetInt(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_CULL_MODE, /*lint -e{930}*/(kzInt)mode);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuRenderPassAttachFrameBuffer(const struct KzuRenderPass* renderPass, struct KzcTexture* frameBufferTexture)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    kzsAssert(kzcIsValidPointer(frameBufferTexture));

    kzsErrorTest(kzcTextureGetTextureType(frameBufferTexture) == KZC_TEXTURE_TYPE_RENDERTARGET, KZS_ERROR_ILLEGAL_ARGUMENT, 
        "Render pass attachment texture must be of type KZC_TEXTURE_TYPE_RENDERTARGET");
    kzsErrorTest(kzcIsValidPointer(kzcTextureGetFrameBufferReference(frameBufferTexture)), KZS_ERROR_ILLEGAL_ARGUMENT, 
        "Render pass attachment texture did not contain valid framebuffer.");

    kzuRenderPassGetData_private(renderPass)->frameBufferTexture = frameBufferTexture;
    kzsSuccess();
}

void kzuRenderPassSetFrameBufferViewportFromWindow(const struct KzuRenderPass* renderPass, kzBool isFromWindow)
{
    kzsAssert(kzcIsValidPointer(renderPass));
    kzuRenderPassGetData_private(renderPass)->frameBufferViewportCalculatedFromWindow = isFromWindow;
}

void kzuRenderPassUnattachFrameBuffer(const struct KzuRenderPass* renderPass)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));

    kzuRenderPassGetData_private(renderPass)->frameBufferTexture = KZ_NULL;
}

struct KzcTexture* kzuRenderPassGetAttachedFrameBuffer(const struct KzuRenderPass* renderPass)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));

    return kzuRenderPassGetData_private(renderPass)->frameBufferTexture;
}

void kzuRenderPassSetShadowPassEnabled(const struct KzuRenderPass* renderPass, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(renderPass));
    kzuRenderPassGetData_private(renderPass)->shadowPass.enabled = enabled;
}

void kzuRenderPassSetShadowPassInformation(const struct KzuRenderPass* renderPass, struct KzuLightNode* lightNode, kzFloat nearPlane,
                                           kzFloat farPlane, kzFloat viewAngleInDegrees)
{
    kzsAssert(kzcIsValidPointer(renderPass));
    kzuRenderPassGetData_private(renderPass)->shadowPass.lightNode = lightNode;
    kzuRenderPassGetData_private(renderPass)->shadowPass.nearClip = nearPlane;
    kzuRenderPassGetData_private(renderPass)->shadowPass.farClip = farPlane;
    kzuRenderPassGetData_private(renderPass)->shadowPass.viewAngle = viewAngleInDegrees;
}

void kzuRenderPassSetViewportAbsolute(const struct KzuRenderPass* renderPass, kzUint x, kzUint y, kzUint width, kzUint height)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));

    kzuRenderPassSetViewportRelative(renderPass, (kzFloat)x, (kzFloat)y, (kzFloat)width, (kzFloat)height);
    kzuRenderPassGetData_private(renderPass)->viewport.coordinateType = KZU_VIEWPORT_COORDINATE_ABSOLUTE;
}

void kzuRenderPassSetViewportRelative(const struct KzuRenderPass* renderPass, kzFloat x, kzFloat y, kzFloat width, kzFloat height)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    kzsAssert(width >= 0);
    kzsAssert(height >= 0);

    kzuRenderPassGetData_private(renderPass)->viewport.coordinateType = KZU_VIEWPORT_COORDINATE_RELATIVE;
    kzuRenderPassGetData_private(renderPass)->viewport.x = (kzFloat)x;
    kzuRenderPassGetData_private(renderPass)->viewport.y = (kzFloat)y;
    kzuRenderPassGetData_private(renderPass)->viewport.width = (kzFloat)width;
    kzuRenderPassGetData_private(renderPass)->viewport.height = (kzFloat)height;
}

void kzuRenderPassSetEnabled(const struct KzuRenderPass* renderPass, kzBool value)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    kzuRenderPassGetData_private(renderPass)->enabled = value;
}

kzBool kzuRenderPassIsEnabled(const struct KzuRenderPass* renderPass)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    return kzuRenderPassGetData_private(renderPass)->enabled;
}

void kzuRenderPassSetRenderOnce(const struct KzuRenderPass* renderPass)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    kzuRenderPassGetData_private(renderPass)->renderOnce = KZ_TRUE;
}

kzsError kzuRenderPassSetFrustumCulling(const struct KzuRenderPass* renderPass, kzBool enabled)
{
    kzsError result;
    kzsAssert(kzuRenderPassIsValid(renderPass));
   
    result = kzuPropertyManagerSetBool(kzuRenderPassGetPropertyManager(renderPass), renderPass, KZU_PROPERTY_TYPE_FRUSTUM_CULLING_ON, enabled);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuRenderPassSetMaterialOverride(const struct KzuRenderPass* renderPass, struct KzuMaterial* material)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    kzsAssert(kzcIsValidPointer(material) || material == KZ_NULL); /* KZ_NULL is allowed. */
    kzuRenderPassGetData_private(renderPass)->overrideMaterial = material;
}

void kzuRenderPassSetPreRenderingStep(const struct KzuRenderPass* renderPass, KzuRenderPassRenderStepFunction preRenderFunction)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    kzuRenderPassGetData_private(renderPass)->preStepFunction = preRenderFunction;
}

void kzuRenderPassSetPostRenderingStep(const struct KzuRenderPass* renderPass, KzuRenderPassRenderStepFunction postRenderFunction)
{
    kzsAssert(kzuRenderPassIsValid(renderPass));
    kzuRenderPassGetData_private(renderPass)->postStepFunction = postRenderFunction;
}

struct KzuComposer* kzuRenderPassToComposer(struct KzuRenderPass* renderPass)
{
    kzsAssert(kzcIsValidPointer(renderPass));
    return &renderPass->composer;
}

struct KzuRenderPass* kzuRenderPassFromComposer(const struct KzuComposer* composer)
{
    kzsAssert(kzcIsValidPointer(composer));
    return (struct KzuRenderPass*)composer;
}
