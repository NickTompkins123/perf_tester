/**
* \file
* Specifies a user renderer. Renderer is the core component for assigning
* lights, meshes, cameras, skin animations etc. to lower level rendering.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_RENDERER_H
#define KZU_RENDERER_H


#include <user/effect_system/stereoscopic_rendering/kzu_stereoscopic_rendering.h>
#include <user/scene_graph/kzu_object_base.h>

#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/color/kzc_color.h>

#include <system/debug/kzs_error.h>
#include <system/wrappers/kzs_opengl_base.h>


/* Forward declarations. */
struct KzuProperty;
struct KzuLightProperty;
struct KzuPropertyType;
struct KzuFixedPropertyTypes;
struct KzuFixedLightPropertyTypes;
struct KzuScene;
struct KzuMaterial;
struct KzuCameraNode;
struct KzuLightNode;
struct KzuTransformedObjectNode;
struct KzuBoundingVolume;
struct KzuPropertyQuery;
struct KzuRenderPass;
struct KzuPropertyManager;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcTexture;
struct KzcDynamicArray;
struct KzcRenderer;
struct KzcMatrix4x4;
struct KzsWindow;


/* Callback function for override materials. */
typedef kzsError (*KzuRendererOverrideMaterialCallback)(const struct KzuTransformedObjectNode* transformedObjectNode, struct KzuMaterial* material, kzUint cluster,
                                                        void* userData, struct KzuMaterial** out_material);


/**
 * \struct KzuRenderer
 * Renderer is the core component for assigning lights, meshes, cameras, skin animations etc. to lower level rendering.
 */
struct KzuRenderer;


#define KZU_RENDERER_FLOAT_BUFFER_LENGTH 5000 /**< Length of float buffer, that is used for dynamic data of vertex arrays. */


/** Function pointer for applying specific type of object for rendering. */
typedef kzsError (*KzuRendererApplyObjectFunction)(struct KzuTransformedObjectNode* transformedObjectNode, struct KzuRenderer* renderer);


/** Creates user renderer. */
kzsError kzuRendererCreate(const struct KzcMemoryManager* memoryManager, kzUint quickMemorySize,
                           struct KzuPropertyManager* propertyManager, struct KzcResourceManager* resourceManager,
                           kzBool createDefaultMaterials, struct KzuRenderer** out_renderer);
/** Deletes user renderer. */
kzsError kzuRendererDelete(struct KzuRenderer* renderer);

/** Enables or disables debug log writing. */
void kzuRendererSetLogging(struct KzuRenderer* renderer, kzBool logging);

/** Resets renderer, that is, puts it to initial state. */
kzsError kzuRendererReset(struct KzuRenderer* renderer);
/** Resets user renderer for frame, that is, clear property stack and core renderer per-frame variables. */
kzsError kzuRendererResetFrame(struct KzuRenderer* renderer);
/** Resets pass for renderer, that is, per-render pass specific properties, such as lighting. */
kzsError kzuRendererResetPass(struct KzuRenderer* renderer);
/** Ends frame for renderer. */
kzsError kzuRendererPrintInfo(const struct KzuRenderer* renderer);


/** Adds light property to rendering pipeline. Lights should be reset each pass by calling kzuRendererResetPass. */
kzsError kzuRendererApplyLightProperty(const struct KzuRenderer* renderer, struct KzuLightNode* lightNode,
                                       const struct KzcMatrix4x4* worldTransformation);
/** Applies mesh object (2D & 3D) down in rendering pipeline, to core renderer. */
kzsError kzuRendererApplyMeshObject(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode);
/** Sets transformed object list for renderer, used for fetching bones for mesh. */
void kzuRendererSetTransformedObjectList(struct KzuRenderer* renderer, struct KzcDynamicArray* transformedObjectList);


/** Gets renderer property query. */
struct KzuPropertyQuery* kzuRendererGetPropertyQuery(const struct KzuRenderer* renderer);


/** Applies material to renderer, attaches the shader and uniforms. */
kzsError kzuRendererApplyMaterial(struct KzuRenderer* renderer, struct KzuMaterial* material);
/** Applies camera node to renderer, containing projection and camera matrices. */
kzsError kzuRendererApplyCameraNode(const struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedCameraNode,
                                    kzFloat viewportX, kzFloat viewportY, kzFloat viewportWidth, kzFloat viewportHeight);
/** Returns default material from renderer. */
struct KzuMaterial* kzuRendererGetDefaultMaterial(const struct KzuRenderer* renderer);
/** Returns error material from renderer. */
struct KzuMaterial* kzuRendererGetErrorMaterial(const struct KzuRenderer* renderer);
/** Gets textured material from renderer. */
struct KzuMaterial* kzuRendererGetTexturedMaterial(const struct KzuRenderer* renderer);
/** Returns solid color material. Uses color property "Emissive" to specify solid color. */
struct KzuMaterial* kzuRendererGetSolidColorMaterial(const struct KzuRenderer* renderer);
/** Sets current override material, which is used instead of all default materials. Setting material to KZ_NULL disables overrides. */
void kzuRendererSetMaterialOverride(struct KzuRenderer* renderer, struct KzuMaterial* material);
/** Sets clear color override for rendering. KZ_NULL to colorRGBA to disable. */
void kzuRendererSetClearColorOverride(struct KzuRenderer* renderer, const struct KzcColorRGBA* colorRGBA);
/** Gets clear color override from renderer. Returns KZ_TRUE if override enabled. */
kzBool kzuRendererGetClearColorOverride(const struct KzuRenderer* renderer, struct KzcColorRGBA* out_colorRGBA);


/** Sets viewport for renderer. */
void kzuRendererSetViewport(struct KzuRenderer* renderer, kzFloat x, kzFloat y, kzFloat width, kzFloat height, kzFloat aspectRatio,
                            kzBool relative);
/** Gets viewport from renderer. 
* \param out_x Viewport X coordinate in relative / absolute coordinates. KZ_NULL if not needed.
* \param out_y Viewport Y coordinate in relative / absolute coordinates. KZ_NULL if not needed.
* \param out_width Viewport Width in relative / absolute coordinates. KZ_NULL if not needed.
* \param out_height Viewport Height in relative / absolute coordinates. KZ_NULL if not needed.
* \param out_aspectRatio Aspect ratio for this viewport. Used only if viewport is relative. KZ_NULL if not needed.
* \param out_relative Is this window relative (absolute if not). KZ_NULL if not needed.
*/
void kzuRendererGetViewport(const struct KzuRenderer* renderer, kzFloat* out_x, kzFloat* out_y, kzFloat* out_width, kzFloat* out_height,
                            kzFloat* out_aspectRatio, kzBool* out_relative);

/** Gets adjusted viewport from renderer. Adjusted viewport calculates real viewport size according to window size and aspect ratio,
    if using relative coordinates. For example 800x600 window with 1.666 aspect ratio would result 800x480 viewport with borders on
    bottom and top, in this example (0, 60, 800, 480).

* \param out_x Viewport X coordinate in absolute (pixel) coordinates.
* \param out_y Viewport Y coordinate in absolute (pixel) coordinates.
* \param out_width Viewport Width in absolute (pixel) coordinates.
* \param out_height Viewport Height in absolute (pixel) coordinates.
*/
void kzuRendererGetAdjustedViewport(const struct KzuRenderer* renderer, const struct KzsWindow* window,
                                    kzFloat* out_x, kzFloat* out_y, kzFloat* out_width, kzFloat* out_height);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
/** Returns shadow map from renderer. */
struct KzuShadowMap* kzuRendererGetShadowMap(const struct KzuRenderer* renderer);
#endif

/** Returns core renderer from user renderer. */
struct KzcRenderer* kzuRendererGetCoreRenderer(const struct KzuRenderer* renderer);


/** Sets bounding box visualization enabled / disabled. */
void kzuRendererSetBoundingBoxVisualizationEnabled(struct KzuRenderer* renderer, kzBool enabled);
/** Returns if bounding box visualization is enabled. */
kzBool kzuRendererIsBoundingBoxVisualizationEnabled(const struct KzuRenderer* renderer);


/** Returns float buffer from renderer. Float buffer contains maximum of KZU_RENDERER_FLOAT_BUFFER_LENGTH elements,
    and it's used for dynamic vertex arrays. */
kzFloat* kzuRendererGetFloatBuffer(const struct KzuRenderer* renderer);


/** Set override material callback. Pass KZ_NULL to disable. */
kzsError kzuRendererSetOverrideMaterialCallback(struct KzuRenderer* renderer, KzuRendererOverrideMaterialCallback callbackFunction, void* userData);
/** Gets override material callback from renderer, KZ_NULL if not enabled. */
KzuRendererOverrideMaterialCallback kzuRendererGetOverrideMaterialCallback(const struct KzuRenderer* renderer);
/** Gets override material callback user data from renderer. */
void* kzuRendererGetOverrideMaterialCallbackUserData(const struct KzuRenderer* renderer);


/** Returns a quick memory manager of the renderer. */
struct KzcMemoryManager* kzuRendererGetQuickMemoryManager(const struct KzuRenderer* renderer);


/** Gets last frame rendered batch count from renderer. */
kzUint kzuRendererGetBatchCount(const struct KzuRenderer* renderer);
/** Gets last frame rendered triangle count from renderer. */
kzUint kzuRendererGetTriangleCount(const struct KzuRenderer* renderer);
/** Gets amount of shader switches per frame. If ES1 always returns zero. */
kzUint kzuRendererGetShaderSwitchCount(const struct KzuRenderer* renderer);
/** Gets number of texture switches per frame from renderer. */
kzUint kzuRendererGetTextureSwitchCount(const struct KzuRenderer* renderer);

/** Callback prototype for setActiveFrameBuffer. */
typedef void (*KzuRendererFrameBufferCallback)(kzUint frameBufferHandle);
/** Sets callback function for setActiveFrameBuffer. Called each time engine changes framebuffer. */
void kzuRendererSetFrameBufferCallback(struct KzuRenderer* renderer, KzuRendererFrameBufferCallback callbackFunction);

/** Enables stereoscopic rendering. */
void kzuRendererEnableStereoscopic(struct KzuRenderer* renderer, enum KzuStereoscopicRenderingMode mode, kzUint pass);
/** Disables stereoscopic rendering */
void kzuRendererDisableStereoscopic(struct KzuRenderer* renderer);
/** Gets stereoscopic properties from renderer. If out_enabled, out_mode, out_pass or out_flipped is KZ_NULL, they're not used. */
void kzuRendererGetStereoscopic(const struct KzuRenderer* renderer, kzBool* out_enabled, enum KzuStereoscopicRenderingMode* out_mode, kzUint* out_pass,
                                kzBool* out_flipped);
/** Sets clear once occurred for stereoscopic rendering. Used for right eye pass. */
void kzuRendererSetStereoscopicClearOnceOccurred(struct KzuRenderer* renderer);
/** Returns if clear once has occurred in stereoscopic rendering. Used for right eye pass. */
kzBool kzuRendererIsStereoscopicClearOnceOccurred(const struct KzuRenderer* renderer);

/** Pushes override screen target. Will override rendering to screen buffer. */
kzsError kzuRendererPushOverrideScreenTargetTexture(const struct KzuRenderer* renderer, struct KzcTexture* overrideScreenTargetTexture);
/** Pops override screen target. out_overrideScreenTargetTexture not assigned with value if KZ_NULL */
kzsError kzuRendererPopOverrideScreenTargetTexture(const struct KzuRenderer* renderer, struct KzcTexture** out_overrideScreenTargetTexture);
/** Peeks override screen target from top of stack. KZ_NULL if none. */
kzsError kzuRendererPeekOverrideScreenTargetTexture(const struct KzuRenderer* renderer, struct KzcTexture** out_overrideScreenTargetTexture);


#endif
