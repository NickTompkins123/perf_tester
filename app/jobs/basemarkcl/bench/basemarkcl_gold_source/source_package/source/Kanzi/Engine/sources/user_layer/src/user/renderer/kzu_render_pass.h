/**
* \file
* Specifies a render pass structure. Render pass specifies one stage of rendering with specified characteristics,
* defined by render pass properties (and filter). Sets of render passes are piped via composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_RENDER_PASS_H
#define KZU_RENDER_PASS_H


#include "kzu_composer_private.h"
#include "kzu_composer_base.h"

#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/color/kzc_color.h>

#include <system/debug/kzs_error.h>


/** Composer type identifier for render pass. */
extern const KzuComposerType KZU_COMPOSER_TYPE_RENDER_PASS;


/** Default value for renderpass depth clear. */
#define KZU_RENDER_PASS_DEFAULT_DEPTH_CLEAR_VALUE 1.0f


/** Enumeration for face culling mode. */
enum KzuCullMode
{
    KZU_CULL_NONE,  /**< No culling. */
    KZU_CULL_BACK,  /**< Cull back faces. */
    KZU_CULL_FRONT  /**< Cull front faces. */
};

/** Enumeration for color write mode. */
enum KzuColorWriteMode
{
    KZU_COLOR_WRITE_NONE,       /**< No write for color channels. */
    KZU_COLOR_WRITE_RGB,        /**< Write to RGB color channels. */
    KZU_COLOR_WRITE_RGBA,       /**< Write to RGBA color channels. */
    KZU_COLOR_WRITE_GRAYSCALE,  /**< Write gray scale. */
    KZU_COLOR_WRITE_RED,        /**< Write R (red) only. */
    KZU_COLOR_WRITE_GREEN,      /**< Write G (green) only. */
    KZU_COLOR_WRITE_BLUE,       /**< Write B (blue) only. */
    KZU_COLOR_WRITE_ALPHA       /**< Write A (alpha) only. */
};

/** Enumeration for stencil operation. */
enum KzuStencilOperation
{
    KZU_STENCIL_OPERATION_KEEP,               /**< No operations for stencil buffer. */
    KZU_STENCIL_OPERATION_INCREASE_ON_ZPASS,  /**< Increase stencil buffer value on depth test pass. */
    KZU_STENCIL_OPERATION_DECREASE_ON_ZPASS,  /**< Decrease stencil buffer value on depth test pass. */
    KZU_STENCIL_OPERATION_INCREASE_ON_ZFAIL,  /**< Increase stencil buffer value on depth test fail. */
    KZU_STENCIL_OPERATION_DECREASE_ON_ZFAIL,  /**< Decrease stencil buffer value on depth test fail. */
    KZU_STENCIL_OPERATION_ZERO                /**< Zero the stencil buffer. */
};

/** Enumeration for stencil function. */
enum KzuStencilFunction
{
    KZU_STENCIL_FUNCTION_ALWAYS,              /**< Always true. */
    KZU_STENCIL_FUNCTION_NEVER,               /**< Always false. */
    KZU_STENCIL_FUNCTION_LESS,                /**< ref < stencil. */
    KZU_STENCIL_FUNCTION_LEQUAL,              /**< ref <= stencil. */
    KZU_STENCIL_FUNCTION_GREATER,             /**< ref > stencil. */
    KZU_STENCIL_FUNCTION_GEQUAL,              /**< ref >= stencil. */
    KZU_STENCIL_FUNCTION_EQUAL,               /**< ref = stencil. */
    KZU_STENCIL_FUNCTION_NOT_EQUAL            /**< ref != stencil. */
};

/** Enumeration for render target */
enum KzuRenderTargetType
{
    KZU_RENDER_TARGET_SCREEN,   /**< Render target directly to screen (video memory). */
    KZU_RENDER_TARGET_TEXTURE   /**< Render target directly to texture. */
};


extern const struct KzuComposerClass KZU_COMPOSER_RENDER_PASS_CLASS;


/* Forward declaration structures */
struct KzuFixedPropertyTypes;
struct KzuTransformedScene;
struct KzuTransformedObjectNode;
struct KzuCameraNode;
struct KzuLightNode;
struct KzuComposer;
struct KzuRenderer;
struct KzuObjectSource;
struct KzuObjectSourceRuntimeData;
struct KzuProperty;
struct KzuPropertyType;
struct KzuMaterial;
struct KzcMemoryManager;
struct KzcRenderer;
struct KzcTexture;
struct KzcDynamicArray;
struct KzcColorRGBA;
struct KzcFrameBuffer;
struct KzsWindow;
struct KzcHashMap;
struct KzuPropertyManager;


/**
 * \struct KzuRenderPass
 * Render pass passes set of objects to be rendered down to renderer. Rendering
 * attributes are selected from render pass properties, that includes view port, clear color,
 * depth & stencil buffer and other characteristics. Inherited from KzuComposer.
 */
struct KzuRenderPass;


/** Creates a render pass. */
kzsError kzuRenderPassCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuRenderPass** out_renderPass);
/** Deletes render pass. */
kzsError kzuRenderPassDelete(struct KzuComposer* renderPassComposer);


/** Checks if the render pass is valid (pointer and the data are non-null). */
kzBool kzuRenderPassIsValid(const struct KzuRenderPass* renderPass);

/** Get the property manager of a composer. */
struct KzuPropertyManager* kzuRenderPassGetPropertyManager(const struct KzuRenderPass* renderPass);


/** Applies a render pass for rendering. */
kzsError kzuRenderPassApply(const struct KzuComposer* renderPassComposer, struct KzuRenderer* renderer,
                            const struct KzuTransformedScene* transformedScene, const struct KzsWindow* activeWindow);

/** Sets a camera for render pass. KZ_NULL for scene view camera. */
void kzuRenderPassSetCamera(const struct KzuRenderPass* renderPass, struct KzuCameraNode* camera);
/** Gets a camera from render pass. KZ_NULL for scene view camera. */
struct KzuCameraNode* kzuRenderPassGetCamera(const struct KzuRenderPass* renderPass);
/** Sets an object source for render pass. */
kzsError  kzuRenderPassSetObjectSource(const struct KzuRenderPass* renderPass, struct KzuObjectSource* objectSource);
/** Gets object source from render pass. */
struct KzuObjectSource* kzuRenderPassGetObjectSource(const struct KzuRenderPass* renderPass);
/** Sets a color buffer for render pass. */
kzsError kzuRenderPassSetColorBuffer(const struct KzuRenderPass* renderPass, kzBool clearEnabled, struct KzcColorRGBA clearColor, 
                                     enum KzuColorWriteMode writeMode);


/** Returns the current clear color. */
struct KzcColorRGBA kzuRenderPassGetClearColor(const struct KzuRenderPass* renderPass);
/** Returns the current clear color. */
enum KzuColorWriteMode kzuRenderPassGetColorWriteMode(const struct KzuRenderPass* renderPass);
/** Returns KZ_TRUE if color buffer clear is enabled. */
kzBool kzuRenderPassGetColorClearEnabled(const struct KzuRenderPass* renderPass);

/** Sets a depth buffer for render pass.
* \param renderPass renderPass to be applied.
* \param depthClearEnabled Is depth buffer cleared on beginning of this renderpass.
* \param clearValue clearValue clear value used if clearing enabled, use KZU_RENDER_PASS_DEFAULT_DEPTH_CLEAR_VALUE by default.
* \param depthTestEnabled Depth testing enabled / disabled.
* \param depthWriteEnabled Writing to depth buffer enabled / disabled.
*/
kzsError kzuRenderPassSetDepthBuffer(const struct KzuRenderPass* renderPass, kzBool depthClearEnabled, kzFloat clearValue,
                                 kzBool depthTestEnabled, kzBool depthWriteEnabled);
/** Sets a stencil buffer for render pass. */
kzsError kzuRenderPassSetStencilBuffer(const struct KzuRenderPass* renderPass, kzBool stencilClearEnabled, kzInt clearValue,
                                       kzBool stencilTestEnabled, enum KzuStencilOperation stencilMode,
                                       enum KzuStencilFunction stencilFunction);
/** Sets a cull mode for render pass. */
kzsError kzuRenderPassSetCullMode(const struct KzuRenderPass* renderPass, enum KzuCullMode mode);


/** Attach frame buffer to this render pass */
kzsError kzuRenderPassAttachFrameBuffer(const struct KzuRenderPass* renderPass, struct KzcTexture* frameBufferTexture);
/** Sets viewport calculated from window dimensions if frame buffer is attached. */
void kzuRenderPassSetFrameBufferViewportFromWindow(const struct KzuRenderPass* renderPass, kzBool isFromWindow);
/** Detaches currently bound frame buffer */
void kzuRenderPassUnattachFrameBuffer(const struct KzuRenderPass* renderPass);
/** Returns the currently attached frame buffer from given renderPass or KZ_NULL if none */
struct KzcTexture* kzuRenderPassGetAttachedFrameBuffer(const struct KzuRenderPass* renderPass);


/** Sets shadow pass enabled / disabled for render pass. */
void kzuRenderPassSetShadowPassEnabled(const struct KzuRenderPass* renderPass, kzBool enabled);
/** Sets shadow pass information for render pass. */
void kzuRenderPassSetShadowPassInformation(const struct KzuRenderPass* renderPass, struct KzuLightNode* lightNode,
                                           kzFloat nearPlane, kzFloat farPlane, kzFloat viewAngleInDegrees);


/** Sets the view port's size in absolute coordinates (pixels) */
void kzuRenderPassSetViewportAbsolute(const struct KzuRenderPass* renderPass, kzUint x, kzUint y, kzUint width, kzUint height);
/** Sets the view port's size in coordinates relative to screen size (0.f = left, 1.f = right) (0.f = top, 1.f = bottom) */
void kzuRenderPassSetViewportRelative(const struct KzuRenderPass* renderPass, kzFloat x, kzFloat y, kzFloat width, kzFloat height);


/** Enables or disables the render pass */
void kzuRenderPassSetEnabled(const struct KzuRenderPass* renderPass, kzBool value);
/** Gets the state of the render pass */
kzBool kzuRenderPassIsEnabled(const struct KzuRenderPass* renderPass);

/** Sets render pass to state where it gets rendered once and is disabled after that. */
void kzuRenderPassSetRenderOnce(const struct KzuRenderPass* renderPass);

/** Sets frustum culling enabled / disabled. */
kzsError kzuRenderPassSetFrustumCulling(const struct KzuRenderPass* renderPass, kzBool enabled);


/** Sets override material for render pass. 
* The material is applied to all objects rendered on this render pass and overrides the default material settings for the nodes and meshes. 
* Setting material to KZ_NULL disables render pass material override. 
*/
void kzuRenderPassSetMaterialOverride(const struct KzuRenderPass* renderPass, struct KzuMaterial* material);

/** Rendering step function prototype for render pass. */
typedef kzsError (*KzuRenderPassRenderStepFunction) (struct KzuRenderer* renderer,
                                                     const struct KzuTransformedScene* transformedScene,
                                                     struct KzcDynamicArray* transformedObjects,
                                                     const struct KzsWindow* activeWindow);

/** Sets pre-rendering step function for render pass. */
void kzuRenderPassSetPreRenderingStep(const struct KzuRenderPass* renderPass, KzuRenderPassRenderStepFunction preRenderFunction);
/** Sets post-rendering step function for render pass. Uses same bound frame buffer as render pass. */
void kzuRenderPassSetPostRenderingStep(const struct KzuRenderPass* renderPass, KzuRenderPassRenderStepFunction postRenderFunction);

/** Casts render pass to composer. */
struct KzuComposer* kzuRenderPassToComposer(struct KzuRenderPass* renderPass);
/** Casts composer to render pass */
struct KzuRenderPass* kzuRenderPassFromComposer(const struct KzuComposer* composer);


#endif
