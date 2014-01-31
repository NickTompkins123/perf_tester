/**
* \file
* Core renderer ES1 version. Contains the graphics state machine.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_RENDERER_ES1_H
#define KZC_RENDERER_ES1_H


#include <system/wrappers/kzs_opengl_base.h>

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1 || KZ_OPENGL_VERSION == KZ_OPENGL_NONE

#include <core/resource_manager/index_buffer/kzc_index_buffer_type.h>
#include <core/resource_manager/texture/kzc_texture_descriptor.h>
#include <core/util/color/kzc_color.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_matrix4x4.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Maximum number of supported lights. */
#define KZC_RENDERER_MAX_LIGHTS         8
/** Maximum number of supported texture units. */
#define KZC_RENDERER_MAX_TEXTURE_UNITS  4


/** Extensions. */
enum KzcRendererExtension
{
    KZC_EXTENSION_FRAMEBUFFER_OBJECT = 0,   /**< Frame buffer object extension. */
    KZC_EXTENSION_TEXTURE_CUBE_MAP   = 1,   /**< Cube map extension. */
    KZC_EXTENSION_COUNT              = 2    /**< Number of extensions. */
};

/** Clear target options for renderer. */
enum KzcRendererClearTarget
{
    KZC_RENDERER_CLEAR_TARGET_COLOR,    /**< Clear target color. */
    KZC_RENDERER_CLEAR_TARGET_DEPTH,    /**< Clear target depth. */
    KZC_RENDERER_CLEAR_TARGET_STENCIL   /**< Clear target stencil. */
};

/** Renderer state modes. */
enum KzcRendererStateMode
{
    KZC_RENDERER_STATE_MODE_DISABLED,   /**< Mode disabled. */
    KZC_RENDERER_STATE_MODE_ENABLED,    /**< Mode enabled. */
    KZC_RENDERER_STATE_MODE_NOT_SET     /**< Mode not set. */
};

/** Renderer state enumerations. */
enum KzcRendererState
{
    KZC_RENDERER_STATE_DEPTH_TEST,          /**< Renderer state for depth testing. */
    KZC_RENDERER_STATE_CULL_FACE,           /**< Renderer state for cull face. */
    KZC_RENDERER_STATE_BLEND,               /**< Renderer state for blending. */
    KZC_RENDERER_STATE_DEPTH_WRITE,         /**< Renderer state for depth write. */
    KZC_RENDERER_STATE_STENCIL_TEST,        /**< Renderer state for stencil test. */
    KZC_RENDERER_STATE_STENCIL_WRITE,       /**< Renderer state for stencil write. */
    KZC_RENDERER_STATE_MULTISAMPLE,         /**< Renderer state for multi-sampling. */
    KZC_RENDERER_STATE_SCISSOR_TEST,     /**< Renderer state for scissor test. */

    KZC_RENDERER_STATE_LIGHTING,            /**< Renderer state for lighting. */
    KZC_RENDERER_STATE_FOG,                 /**< Renderer state for lighting. */

    KZC_RENDERER_MAX_STATES                 /**< Maximum count of renderer states. */
};

/** Renderer stencil modes. */
enum KzcRendererStencilFunction
{
    KZC_RENDERER_STENCIL_MODE_NEVER,    /**< Always fails. */
    KZC_RENDERER_STENCIL_MODE_LESS,     /**< (Ref & mask) < (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_LEQUAL,   /**< (Ref & mask) <= (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_GREATER,  /**< (Ref & mask) > (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_GEQUAL,   /**< (Ref & mask) >= (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_EQUAL,    /**< (Ref & mask) = (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_NOT_EQUAL,/**< (Ref & mask) != (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_ALWAYS    /**< Always passes. */
};

/** Renderer stencil operations. */
enum KzcRendererStencilOperation
{
    KZC_RENDERER_STENCIL_OPERATION_KEEP, /**< Keeps the current stencil buffer value. */
    KZC_RENDERER_STENCIL_OPERATION_ZERO, /**< Sets stencil buffer value to zero. */
    KZC_RENDERER_STENCIL_OPERATION_REPLACE, /**< Set stencil buffer value to reference value, specified by StencilFunction. */
    KZC_RENDERER_STENCIL_OPERATION_INCREASE_WRAP, /**< Increases stencil buffer value, wraps to zero when reaching maximum. */
    KZC_RENDERER_STENCIL_OPERATION_DECREASE_WRAP, /**< Decreases stencil buffer value, wraps to maximum - 1 when below 0. */
    KZC_RENDERER_STENCIL_OPERATION_INCREASE, /**< Increases current stencil buffer value. */
    KZC_RENDERER_STENCIL_OPERATION_DECREASE, /**< Decreases current stencil buffer value. */
    KZC_RENDERER_STENCIL_OPERATION_INVERT /**< Inverts current stencil buffer value. */
};

/** Renderer fog modes. */
enum KzcRendererFogMode
{
    KZC_RENDERER_FOG_MODE_NONE,         /**< No fog. */
    KZC_RENDERER_FOG_MODE_LINEAR,       /**< Linear fog. */
    KZC_RENDERER_FOG_MODE_DENSITY       /**< Density fog. */
};

/** List of active renderer colors, e.g. fixed function material colors. */
enum KzcRendererColor
{
    KZC_RENDERER_COLOR_AMBIENT,         /**< Active ambient. */
    KZC_RENDERER_COLOR_DIFFUSE,         /**< Active diffuse. */
    KZC_RENDERER_COLOR_SPECULAR,        /**< Active specular. */
    KZC_RENDERER_COLOR_EMISSIVE,        /**< Active emissive. */
    KZC_RENDERER_COLOR_FOG,             /**< Active fog color. */

    KZC_RENDERER_COLOR_GLOBAL_AMBIENT,  /**< Active global ambient color. */

    KZC_RENDERER_COLOR_CLEAR,           /**< Active clear color. */

    KZC_RENDERER_MAX_COLORS             /**< Maximum count of renderer colors. */
};

/** List of available cull modes. */
enum KzcRendererCullMode
{
    KZC_RENDERER_CULL_MODE_NONE,        /**< No culling. */
    KZC_RENDERER_CULL_MODE_BACK,        /**< Backface culling. */
    KZC_RENDERER_CULL_MODE_FRONT        /**< Frontface culling. */
};

/** List of available buffers. */
enum KzcRendererBuffer
{
    KZC_RENDERER_BUFFER_VERTEX,         /**< Vertex buffer. */
    KZC_RENDERER_BUFFER_INDEX,          /**< Index buffer. */

    KZC_RENDERER_MAX_BUFFERS            /** Maximum amount of buffers. */
};

/** Null value for buffer */
#define KZC_RENDERER_BUFFER_VALUE_NONE 0

/** List of active renderer scalars */
enum KzcRendererScalar
{
    KZC_RENDERER_SCALAR_SPECULAR_EXPONENT,      /**< Specular exponent. */
    KZC_RENDERER_SCALAR_CLEAR_DEPTH,            /**< Clear depth value. */

    KZC_RENDERER_SCALAR_FOG_START,              /**< Fog start. */
    KZC_RENDERER_SCALAR_FOG_END,                /**< Fog end. */
    KZC_RENDERER_SCALAR_FOG_MODE,               /**< Fog mode. */

    KZC_RENDERER_MAX_SCALARS                    /**< Maximum amount of scalars. */
};

/** List of active matrices renderer keeps track of. */
enum KzcRendererMatrix
{
    KZC_RENDERER_MATRIX_PROJECTION,     /**< Projection matrix. */
    KZC_RENDERER_MATRIX_CAMERA,         /**< Camera matrix. */
    KZC_RENDERER_MATRIX_WORLD,          /**< Current world matrix. */
    KZC_RENDERER_MATRIX_TEXTURE0,       /**< Current texture matrix unit 0. */
    KZC_RENDERER_MATRIX_TEXTURE1,       /**< Current texture matrix unit 1. */
    KZC_RENDERER_MATRIX_TEXTURE2,       /**< Current texture matrix unit 2. */
    KZC_RENDERER_MATRIX_TEXTURE3,       /**< Current texture matrix unit 3. */
    KZC_RENDERER_MATRIX_CAMERA_WORLD,   /**< Current camera x world matrix. */

    KZC_RENDERER_MAX_MATRICES           /**< Maximum amount of matrices. */
};

/** List of color write modes. */
enum KzcRendererColorWriteMode
{
    KZC_RENDERER_COLOR_WRITE_NONE,      /**< No color write. */
    KZC_RENDERER_COLOR_WRITE_RGB,       /**< Color write for RGB channels. */
    KZC_RENDERER_COLOR_WRITE_RGBA,      /**< Color write for RGBA channels. */
    KZC_RENDERER_COLOR_WRITE_RED,       /**< Color write for red channel. */
    KZC_RENDERER_COLOR_WRITE_GREEN,     /**< Color write for green channel. */
    KZC_RENDERER_COLOR_WRITE_BLUE,      /**< Color write for blue channel. */
    KZC_RENDERER_COLOR_WRITE_CYAN,      /**< Color write for cyan (green+blue). */
    KZC_RENDERER_COLOR_WRITE_ALPHA      /**< Color write for alpha channel. */
};

/** Possible light types for renderer. */
enum KzcRendererLightType
{
    KZC_RENDERER_LIGHT_TYPE_DIRECTIONAL,    /**< Directional light. */
    KZC_RENDERER_LIGHT_TYPE_POINT,          /**< Point light. */
    KZC_RENDERER_LIGHT_TYPE_SPOT            /**< Spot light. */
};

/** List of available blend modes. */
enum KzcRendererBlendMode
{
    KZC_RENDERER_BLEND_MODE_OPAQUE,     /**< Opaque blend mode. */
    KZC_RENDERER_BLEND_MODE_ALPHA,      /**< Alpha blend. */
    KZC_RENDERER_BLEND_MODE_ADDITIVE    /**< Additive blend mode. */
};

/** List of color formats. */
enum KzcRendererColorFormat
{
    KZC_RENDERER_COLOR_FORMAT_RGB           /**< RGB color. */
};

/** List of color types. */
enum KzcRendererColorType
{
    KZC_RENDERER_COLOR_TYPE_UNSIGNED_SHORT_5_6_5        /**< 16 bit 565 color. */
};

/** List of texture types. */
enum KzcRendererTextureType
{
    KZC_RENDERER_TEXTURE_TYPE_2D, /**< 2D Texture. */
    KZC_RENDERER_TEXTURE_TYPE_CUBEMAP /**< Cubemap texture. */
};

/* Vertex array types for vertex array usage. */
#define KZC_VERTEX_ARRAY_POSITION               (1 << 0)        /**< Positions for vertex array. */
#define KZC_VERTEX_ARRAY_NORMAL                 (1 << 1)        /**< Normals for vertex array. */
#define KZC_VERTEX_ARRAY_COLOR                  (1 << 2)        /**< Colors for vertex array. */
#define KZC_VERTEX_ARRAY_TEXTURE_COORDINATE     (1 << 3)        /**< UVs for vertex array. */


/* Forward declarations */
struct KzcMemoryManager;
struct KzcIndexBuffer;
struct KzcVertexBuffer;
struct KzcDynamicArray;
struct KzcTexture;
struct KzsWindow;


/**
 * \struct KzcRenderer
 * Structure for core layer renderer.
 */
struct KzcRenderer;


/** Creates a new renderer object. */
kzsError kzcRendererCreate(const struct KzcMemoryManager* memoryManager, struct KzcRenderer** out_renderer);
/** Deletes a renderer object. */
kzsError kzcRendererDelete(struct KzcRenderer* renderer);


/** Resets frame for core renderer. */
void kzcRendererResetFrame(struct KzcRenderer* renderer);
/** Resets some of the renderer specific data on fixed pipeline. Currently does not optimize anything specific. */
kzsError kzcRendererReset(struct KzcRenderer* renderer);
/** Resets lights from renderer. */
kzsError kzcRendererResetLights(struct KzcRenderer* renderer);


/** Sets an active window for renderer. */
void kzcRendererSetActiveWindow(struct KzcRenderer* renderer, struct KzsWindow* window);
/** Returns an active window from renderer. */
struct KzsWindow* kzcRendererGetActiveWindow(const struct KzcRenderer* renderer);


/** Sets a specific renderer state enabled. */
void kzcRendererSetState(struct KzcRenderer* renderer, enum KzcRendererState state);
/** Disables a specific renderer state enabled. */
void kzcRendererDisableState(struct KzcRenderer* renderer, enum KzcRendererState state);


/** Enables texturing for current unit. */
void kzcRendererEnableTexturing(struct KzcRenderer* renderer, enum KzcRendererTextureType textureType);
/** Disables texturing for current unit. */
void kzcRendererDisableTexturing(struct KzcRenderer* renderer);
/** Set texture operation. */
void kzcRendererSetTextureOperation(struct KzcRenderer* renderer, enum KzcTextureOperation operation);


/** Clears a context using specified flags. */
void kzcRendererClear(const struct KzcRenderer* renderer);
/** Resets a clear target for renderer. */
void kzcRendererResetClearTarget(struct KzcRenderer* renderer);
/** Sets a clearing target for renderer. */
void kzcRendererAddClearTarget(struct KzcRenderer* renderer, enum KzcRendererClearTarget target);
/** Removes a clearing target for renderer. */
void kzcRendererRemoveClearTarget(struct KzcRenderer* renderer, enum KzcRendererClearTarget target);
/** Sees if renderer has specific clear target. */
kzBool kzcRendererHasClearTarget(const struct KzcRenderer* renderer, enum KzcRendererClearTarget target);
/** See if clear is enabled. */
kzBool kzcRendererIsClearEnabled(const struct KzcRenderer* renderer);
/** Set clear color for renderer. */
void kzcRendererSetClearColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA);
/** Set a color write mode for renderer. */
void kzcRendererSetColorWriteMode(enum KzcRendererColorWriteMode colorWriteMode);
/** Sets scissor test for renderer. */
void kzcRendererSetScissorTest(struct KzcRenderer* renderer, kzBool enabled, kzUint x, kzUint y, kzUint width, kzUint height);


/** Set clear depth for renderer. */
void kzcRendererSetClearDepthValue(struct KzcRenderer* renderer, kzFloat clearDepthValue);
/** Set clear stencil for renderer. */
void kzcRendererSetClearStencil(struct KzcRenderer* renderer, kzInt stencilClearValue);


/** Sets an active viewport for renderer. */
void kzcRendererSetViewport(struct KzcRenderer* renderer, kzUint x, kzUint y, kzUint width, kzUint height);


/** Sets a buffer for renderer. */
void kzcRendererSetBuffer(struct KzcRenderer* renderer, enum KzcRendererBuffer buffer, kzUint value);
/** Render buffers that are currently bound. */
void kzcRendererDrawBuffers(struct KzcRenderer* renderer);
/** Sets vertex count for renderer. */
void kzcRendererSetVertexCount(struct KzcRenderer* renderer, kzUint vertexCount);
/** Sets an index count for renderer. */
void kzcRendererSetIndexCount(struct KzcRenderer* renderer, kzUint indexCount);
/** Sets an index data for renderer. */
void kzcRendererSetIndexData(struct KzcRenderer* renderer, void* indexData);
/** Sets the index type for renderer. */
void kzcRendererSetIndexType(struct KzcRenderer* renderer, enum KzcPrimitiveType indexType);

/** Sets a color for light. */
void kzcRendererSetLightColor(struct KzcRenderer* renderer, kzUint lightIndex, struct KzcColorRGBA colorRGBA);
/** Sets a light color for renderer engine. */
void kzcRendererSetLightColor_private(struct KzcRenderer* renderer, kzUint lightIndex, struct KzcColorRGBA colorRGBA);
/** Sets a directional light for specified index. */
void kzcRendererSetDirectionalLight(struct KzcRenderer* renderer, kzUint lightIndex, const struct KzcVector3* direction);
/** Sets a point light for specified index. */
void kzcRendererSetPointLight(struct KzcRenderer* renderer, kzUint lightIndex, const struct KzcVector3* lightPosition);
/** Sets a spot light for specified index. */
void kzcRendererSetSpotLight(struct KzcRenderer* renderer, kzUint lightIndex, const struct KzcVector3* lightPosition,
                             const struct KzcVector3* lightDirection, kzFloat exponent, kzFloat cutoffAngleInDegrees);
/** Sets attenuation coefficients for light. */
void kzcRendererSetLightAttenuation(struct KzcRenderer* renderer, kzUint lightIndex, struct KzcVector3 attenuationCoefficients);
/** Sets a type for light. */
void kzcRendererSetLightType(struct KzcRenderer* renderer, kzUint lightIndex, enum KzcRendererLightType lightType);
/** Gets a light color from light index. */
struct KzcColorRGBA kzcRendererGetLightColor(const struct KzcRenderer* renderer, kzUint lightIndex);


/** Sets an active color slot value for renderer. */
void kzcRendererSetActiveColor(struct KzcRenderer* renderer, enum KzcRendererColor color, struct KzcColorRGBA colorRGBA);
/** Gets a pointer value of active color slot from renderer. */
struct KzcColorRGBA kzcRendererGetActiveColor(const struct KzcRenderer* renderer, enum KzcRendererColor color);
/** Switches an scalar value for renderer if necessary, e.g. depth clear value. */
void kzcRendererSetActiveScalar(struct KzcRenderer* renderer, enum KzcRendererScalar scalar, kzFloat value);
/** Returns a current scalar value from renderer. */
kzFloat kzcRendererGetActiveScalar(const struct KzcRenderer* renderer, enum KzcRendererScalar scalar);


/** Sets blend mode for renderer. */
void kzcRendererSetBlendMode(struct KzcRenderer* renderer, enum KzcRendererBlendMode blendMode);
/** Sets blend intensity for renderer. */
void kzcRendererSetBlendIntensity(struct KzcRenderer* renderer, kzFloat intensity);

/** Sets a fog mode for renderer. */
void kzcRendererSetFogMode(struct KzcRenderer* renderer, enum KzcRendererFogMode fogMode);

/** Sets a cull mode for renderer. */
void kzcRendererSetCullMode(struct KzcRenderer* renderer, enum KzcRendererCullMode cullMode);
/** Sets a stencil mode for renderer. */
void kzcRendererSetStencilFunction(struct KzcRenderer* renderer, enum KzcRendererStencilFunction stencilFunction,
                                   kzUint referenceValue, kzUint maskValue);
/** Sets a stencil operation for renderer. */
void kzcRendererSetStencilOperation(struct KzcRenderer* renderer, enum KzcRendererStencilOperation stencilFailOperation,
                                    enum KzcRendererStencilOperation stencilPassDepthFailOperation,
                                    enum KzcRendererStencilOperation stencilPassDepthPassOperation);


/** Sets a matrix for renderer. */
void kzcRendererSetMatrix(struct KzcRenderer* renderer, enum KzcRendererMatrix matrixType, const struct KzcMatrix4x4* matrix);
/** Gets a matrix from renderer. */
struct KzcMatrix4x4 kzcRendererGetMatrix(const struct KzcRenderer* renderer, enum KzcRendererMatrix matrixType);
/** Sets a texture matrix for renderer. */
void kzcRendererSetTextureMatrix(struct KzcRenderer* renderer, const struct KzcMatrix4x4* matrix);
/** Applies model view transformation for renderer. */
void kzcRendererApplyModelviewTransformation(const struct KzcRenderer* renderer, const struct KzcMatrix4x4* matrix);
/** Applies transformations for renderer. */
kzsError kzcRendererApplyTransformation(const struct KzcRenderer* renderer);


/** Sets a global ambient color for renderer. */
void kzcRendererSetGlobalAmbientColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA);
/** Sets a diffuse color for renderer. */
void kzcRendererSetDiffuseColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA);
/** Sets an ambient color for renderer. */
void kzcRendererSetAmbientColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA);
/** Sets a specular color and exponent for renderer. */
void kzcRendererSetSpecular(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA, const kzFloat exponent);
/** Sets a specular color for renderer. */
void kzcRendererSetSpecularColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA);
/** Sets a specular exponent for renderer. */
void kzcRendererSetSpecularExponent(struct KzcRenderer* renderer, kzFloat exponent);
/** Sets an emissive color for renderer. */
void kzcRendererSetEmissiveColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA);


/** Sets fog color for renderer. */
void kzcRendererSetFogColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA);
/** Sets fog start for renderer (linear fog). */
void kzcRendererSetFogStart(struct KzcRenderer* renderer, kzFloat value);
/** Sets fog end for renderer (linear fog). */
void kzcRendererSetFogEnd(struct KzcRenderer* renderer, kzFloat value);


/** Sets active texture unit. */
void kzcRendererSetActiveTextureUnit(struct KzcRenderer* renderer, kzUint textureUnit);
/** Applies texture for renderer. */
kzsError kzcRendererApplyTexture(struct KzcRenderer* renderer, kzUint textureHandle,  enum KzcRendererTextureType textureType,
                                 kzUint textureWidth, kzUint textureHeight);

/** Detaches texture handle from all texture units it has been applied for. */
void kzcRendererDetachTextureHandle(struct KzcRenderer* renderer, kzUint textureHandle);
/** Detaches texture from rendering. */
void kzcRendererDetachTexture(struct KzcRenderer* renderer);

/** Binds texture for renderer. */
void kzcRendererBindTexture(struct KzcRenderer* renderer,  enum KzcRendererTextureType textureType, kzUint textureHandle);
/** Returns if texturing is enabled on given unit. */
kzBool kzcRendererIsTexturingEnabled(const struct KzcRenderer* renderer, kzUint textureUnit);


/** Sets active frame buffer for renderer. */
void kzcRendererSetActiveFrameBuffer(struct KzcRenderer* renderer, kzUint frameBufferHandle);
/** Gets active frame buffer handle from. */
kzUint kzcRendererGetActiveFrameBuffer(const struct KzcRenderer* renderer);

/** Reads the pixels from the current frame buffer. */
void kzcRendererReadPixels(struct KzcRenderer* renderer, kzInt x, kzInt y, kzInt width, kzInt height,
                           enum KzcRendererColorFormat format, enum KzcRendererColorType type, void* pixels);

/** Sets active vertex attribute map for renderer. */
void kzcRendererSetActiveShaderAttributeMap(struct KzcRenderer* renderer, struct KzcDynamicArray* vertexAttributeMap);
/** Gets active vertex attribute map from renderer. */
struct KzcDynamicArray* kzcRendererGetActiveShaderAttributeMap(const struct KzcRenderer* renderer);


/** Begins a material for renderer. */
void kzcRendererBeginMaterialSetup(struct KzcRenderer* renderer);
/** Ends a material for renderer. */
void kzcRendererEndMaterialSetup(struct KzcRenderer* renderer);


/** Sets line rendering width for renderer. */
void kzcRendererSetLineWidth(struct KzcRenderer* renderer, kzFloat lineWidth);


/** 
 * Begins set of vertex arrays.
 * \param renderer Core renderer object.
 * \param vertexArrayFlags Set of vertex arrays to be applied. Options: KZC_VERTEX_ARRAY_POSITION, KZC_VERTEX_ARRAY_NORMAL,
 * KZC_VERTEX_ARRAY_TEXTURE_COORDINATE and KZC_VERTEX_ARRAY_COLOR.
 */
void kzcRendererBeginVertexArray(struct KzcRenderer* renderer, kzUint vertexArrayFlags);
/** Sets vertex array data. */
void kzcRendererSetVertexArrayData(struct KzcRenderer* renderer, kzUint vertexArrayFlag, const void* data);
/** Ends vertex array. Renders the specified vertex lists and finishes the usage. */
void kzcRendererEndVertexArray(struct KzcRenderer* renderer, enum KzcPrimitiveType primitiveType, kzUint count);

/** Prints info about renderer capabilities. */
kzsError kzcRendererPrintInfo(const struct KzcRenderer* renderer);

/** Check supported extensions */
kzBool kzcRendererIsExtensionSupported(const struct KzcRenderer* renderer, enum KzcRendererExtension extension);

/** Render textured quad in orthogonal projection. x,y coordinates are window coordinates. */
kzsError kzcRendererDrawTexturedQuad2DAbsolute(struct KzcRenderer* renderer, const struct KzcTexture* texture, kzFloat x, kzFloat y, kzFloat width, kzFloat height, kzFloat u1, kzFloat v1, kzFloat u2, kzFloat v2);
/** Render quad with solid color in orthogonal projection. x,y coordinates are window coordinates. */
kzsError kzcRendererDrawSolidQuad2DAbsolute(struct KzcRenderer* renderer, kzFloat x, kzFloat y, kzFloat width,
                                            kzFloat height, struct KzcColorRGBA color);

/** Gets view port from renderer. If out_x, out_y, out_width or out_height are specified as KZ_NULL, they're not used. */
void kzcRendererGetViewport(const struct KzcRenderer* renderer, kzFloat* out_x, kzFloat* out_y, kzFloat* out_width, kzFloat* out_height);

/** Sets debug logging state for renderer. */
void kzcRendererSetDebugLoggingState(struct KzcRenderer* renderer, kzBool loggingEnabled);
/** Gets triangle count from renderer. */
kzUint kzcRendererGetTriangleCount(const struct KzcRenderer* renderer);
/** Gets number of texture switches from renderer. */
kzUint kzcRendererGetTextureSwitchCount(const struct KzcRenderer* renderer);


/** Sets GPU calls enabled / disabled. */
void kzcRendererSetGPUCallsEnabled(struct KzcRenderer* renderer, kzBool enabled);
/** Checks if GPU calls are enabled. */
kzBool kzcRendererGPUCallsIsEnabled(const struct KzcRenderer* renderer);


#endif

#endif
