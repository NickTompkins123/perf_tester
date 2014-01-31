/**
* \file
* Core renderer ES2 version. Contains the graphics state machine.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_RENDERER_GL21_H
#define KZC_RENDERER_GL21_H


#include <system/wrappers/kzs_opengl_base.h>

#if KZ_OPENGL_VERSION == KZ_OPENGL_2_1

#include <core/resource_manager/index_buffer/kzc_index_buffer_type.h>
#include <core/util/color/kzc_color.h>
#include <core/util/math/kzc_matrix4x4.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>
#include <system/wrappers/kzs_math.h>


/** Size of the uniform cache. */
#define KZC_RENDERER_UNIFORM_CACHE_SIZE     150000

/* Maximum number of supported texture units. */
#define KZC_RENDERER_MAX_TEXTURE_UNITS      5


/** Extensions. */
enum KzcRendererExtension
{
    KZC_GL_ARB_framebuffer_object, /**< Framebuffer object. */
    KZC_EXTENSION_COUNT /**< Number of extensions. */
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
    KZC_RENDERER_STATE_DEPTH_TEST,      /**< Renderer state for depth testing. */
    KZC_RENDERER_STATE_CULL_FACE,       /**< Renderer state for cull face. */
    KZC_RENDERER_STATE_BLEND,           /**< Renderer state for blending. */
    KZC_RENDERER_STATE_DEPTH_WRITE,     /**< Renderer state for depth write. */
    KZC_RENDERER_STATE_STENCIL_TEST,   /**< Renderer state for stencil test. */
    KZC_RENDERER_STATE_STENCIL_WRITE,   /**< Renderer state for stencil write. */
    KZC_RENDERER_STATE_MULTISAMPLE,     /**< Renderer state for multi-sampling. */
    KZC_RENDERER_STATE_SCISSOR_TEST,     /**< Renderer state for scissor test. */

    KZC_RENDERER_MAX_STATES             /**< Maximum count of renderer states. */
};

/** List of active renderer colors for fixed functionality */
enum KzcRendererColor
{
    KZC_RENDERER_COLOR_CLEAR,       /**< Active clear color. */

    KZC_RENDERER_MAX_COLORS         /**< Maximum number of renderer colors. */
};

/** List of available cull modes. */
enum KzcRendererCullMode
{
    KZC_RENDERER_CULL_MODE_NONE,    /**< No culling. */
    KZC_RENDERER_CULL_MODE_BACK,    /**< Backface culling. */
    KZC_RENDERER_CULL_MODE_FRONT    /**< Frontface culling. */
};

/** List of active renderer scalars for fixed functionality. */
enum KzcRendererScalar
{
    KZC_RENDERER_SCALAR_CLEAR_DEPTH,    /**< Clear depth value. */
    
    KZC_RENDERER_MAX_SCALARS            /**< Maximum amount of scalars. */
};

/** List of available buffers. */
enum KzcRendererBuffer
{
    KZC_RENDERER_BUFFER_VERTEX,     /**< Vertex buffer. */
    KZC_RENDERER_BUFFER_INDEX,      /**< Index buffer. */

    KZC_RENDERER_MAX_BUFFERS        /**< Maximum amount of buffers. */
};

/** Renderer stencil modes. */
enum KzcRendererStencilFunction
{
    KZC_RENDERER_STENCIL_MODE_NEVER, /**< Always fails. */
    KZC_RENDERER_STENCIL_MODE_LESS, /**< (Ref & mask) < (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_LEQUAL, /**< (Ref & mask) <= (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_GREATER, /**< (Ref & mask) > (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_GEQUAL, /**< (Ref & mask) >= (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_EQUAL, /**< (Ref & mask) = (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_NOT_EQUAL,/**< (Ref & mask) != (stencil & mask). */
    KZC_RENDERER_STENCIL_MODE_ALWAYS /**< Always passes. */
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

#define KZC_RENDERER_BUFFER_VALUE_NONE 0    /**< Null value for buffers, that is, vertex & index buffer. */

/** List of active matrices renderer keeps track of. */
enum KzcRendererMatrix
{
    KZC_RENDERER_MATRIX_PROJECTION,                 /**< Projection matrix. */
    KZC_RENDERER_MATRIX_CAMERA,                     /**< Camera matrix. */
    KZC_RENDERER_MATRIX_WORLD,                      /**< Current world matrix. */
    KZC_RENDERER_MATRIX_CAMERA_WORLD,               /**< Current camera x world matrix. */
    KZC_RENDERER_MATRIX_PROJECTION_CAMERA_WORLD,   /**< Current projection x camera x world matrix. */

    KZC_RENDERER_MAX_MATRICES                       /**< Maximum amount of matrices. */
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

/** List of renderer blend modes. */
enum KzcRendererBlendMode
{
    KZC_RENDERER_BLEND_MODE_OPAQUE,         /**< Opaque blend mode. */
    KZC_RENDERER_BLEND_MODE_ALPHA,          /**< Alpha blend. */
    KZC_RENDERER_BLEND_MODE_ADDITIVE        /**< Additive blend mode. */
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


#define KZC_RENDERER_UNIFORM_WORLD_MATRIX                       "kzWorldMatrix"         /**< Uniform string for world matrix. */
#define KZC_RENDERER_UNIFORM_CAMERA_MATRIX                      "kzCameraMatrix"        /**< Uniform string for camera matrix. */
#define KZC_RENDERER_UNIFORM_CAMERA_WORLD_MATRIX                "kzCameraWorldMatrix"   /**< Uniform string for camera x world matrix. */
#define KZC_RENDERER_UNIFORM_PROJECTION_CAMERA_WORLD_MATRIX     "kzProjectionCameraWorldMatrix" /**< Uniform string for projection x camera x world matrix. */
#define KZC_RENDERER_UNIFORM_PROJECTION_MATRIX                  "kzProjectionMatrix"            /**< Uniform string for projection matrix. */
#define KZC_RENDERER_UNIFORM_NORMAL_MATRIX                      "kzNormalMatrix"                /**< Uniform string for normal matrix. */

#define KZC_RENDERER_UNIFORM_CAMERA_POSITION                    "kzCameraPosition"              /**< Uniform string for camera world position. */

/* Fixed shader attribute names. */
extern const kzString KZC_RENDERER_ATTRIBUTE_POSITION;        /**< Attribute string for vertex positions. */
extern const kzString KZC_RENDERER_ATTRIBUTE_NORMAL;          /**< Attribute string for vertex normals. */
extern const kzString KZC_RENDERER_ATTRIBUTE_TANGENT;         /**< Attribute string for vertex tangents. */
extern const kzString KZC_RENDERER_ATTRIBUTE_WEIGHT;          /**< Attribute string for vertex weight (skinning). */
extern const kzString KZC_RENDERER_ATTRIBUTE_MATRIX_PALETTE;  /**< Attribute string for matrix palette (skinning). */
extern const kzString KZC_RENDERER_ATTRIBUTE_TEXTURE_COORDINATE[];  /**< List of attribute strings for texture coordinates. */
extern const kzString KZC_RENDERER_ATTRIBUTE_COLOR[];       /**< List of attribute strings for vertex colors. */
extern const kzString KZC_RENDERER_ATTRIBUTE_CUSTOM[];  /**< List of attribute strings for custom attributes. */


/* Forward declaration structures. */
struct KzcMemoryManager;
struct KzcHashMap;
struct KzcIndexBuffer;
struct KzcVertexBuffer;
struct KzcShader;
struct KzcVector2;
struct KzcVector3;
struct KzcVector4;
struct KzcMatrix2x2;
struct KzcMatrix3x3;
struct KzcMatrix4x4;
struct KzcTexture;
struct KzcFrameBuffer;
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
/** Sets default shaders for core renderer. */
void kzcRendererSetDefaultShaders(struct KzcRenderer* renderer, struct KzcShader* solidColorShader, struct KzcShader* texturedShader);
/** Resets frame for core renderer, that is, counters for logging rendering infos (triangle count etc.). */
void kzcRendererResetFrame(struct KzcRenderer* renderer);
/** Resets renderer, e.g. resets its caches and states. */
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


/** Sets an active view port for renderer. */
void kzcRendererSetViewport(struct KzcRenderer* renderer, kzUint x, kzUint y, kzUint width, kzUint height);


/** Sets a buffer for renderer. */
void kzcRendererSetBuffer(struct KzcRenderer* renderer, enum KzcRendererBuffer buffer, kzUint value);
/** Render buffers that are currently bound. */
void kzcRendererDrawBuffers(struct KzcRenderer* renderer);
/** Sets vertex count for renderer. */
void kzcRendererSetVertexCount(struct KzcRenderer* renderer, kzUint vertexCount);
/** Sets the index count for renderer. */
void kzcRendererSetIndexCount(struct KzcRenderer* renderer, kzUint indexCount);
/** Sets index data for renderer. */
void kzcRendererSetIndexData(struct KzcRenderer* renderer, void* indexData);
/** Sets the index type for renderer. */
void kzcRendererSetIndexType(struct KzcRenderer* renderer, enum KzcPrimitiveType indexType);


/** Finds uniform location for given uniform from active shader. The data will be stored to uniform cache and fetched from in subsequent calls. */
kzsError kzcRendererGetUniformLocation(const struct KzcRenderer* renderer, kzString uniformName, kzInt* out_uniformLocation);
/** Sets integer uniform for renderer, using currently bound shader. */
kzsError kzcRendererSetUniformInteger(struct KzcRenderer* renderer, kzString uniformName, kzInt value);
/** Sets float uniform for renderer, using currently bound shader. */
kzsError kzcRendererSetUniformFloat(struct KzcRenderer* renderer, kzString uniformName, kzFloat value);
/** Sets uniform float array for renderer. */
kzsError kzcRendererSetUniformFloatArray(struct KzcRenderer* renderer, kzString uniformName, kzUint count, const kzFloat* value);
/** Sets uniform vec2 for renderer, using currently bound shader. */
kzsError kzcRendererSetUniformVec2(struct KzcRenderer* renderer, kzString uniformName, const struct KzcVector2* value);
/** Sets uniform vec3 for renderer, using currently bound shader. */
kzsError kzcRendererSetUniformVec3(struct KzcRenderer* renderer, kzString uniformName, const struct KzcVector3* value);
/** Sets uniform vec3 array for renderer. */
kzsError kzcRendererSetUniformVec3Array(struct KzcRenderer* renderer, kzString uniformName, kzUint count, const struct KzcVector3* value);
/** Sets uniform vec4 for renderer, using currently bound shader. */
kzsError kzcRendererSetUniformVec4(struct KzcRenderer* renderer, kzString uniformName, const struct KzcVector4* value);
/** Sets arbitrary vector array with dimension to renderer, using currently bound shader. */
kzsError kzcRendererSetUniformVecArray(struct KzcRenderer* renderer, kzString uniformName, kzUint count, kzUint dimension, const kzFloat* values);
/** Sets uniform colorRGBA for renderer, using currently bound shader. */
kzsError kzcRendererSetUniformColorRGBA(struct KzcRenderer* renderer, kzString uniformName, const struct KzcColorRGBA* value);
/** Sets uniform vec4 array for renderer. */
kzsError kzcRendererSetUniformVec4Array(struct KzcRenderer* renderer, kzString uniformName, kzUint count, const struct KzcVector4* value);
/** Sets matrix2x2 uniform for renderer, using currently bound shader. */
kzsError kzcRendererSetUniformMatrix2x2(struct KzcRenderer* renderer, kzString matrixName, const struct KzcMatrix2x2* value);
/** Sets matrix3x3 uniform for renderer, using currently bound shader. */
kzsError kzcRendererSetUniformMatrix3x3(struct KzcRenderer* renderer, kzString matrixName, const struct KzcMatrix3x3* value);
/** Sets matrix4x4 uniform for renderer, using currently bound shader. */
kzsError kzcRendererSetUniformMatrix4x4(struct KzcRenderer* renderer, kzString matrixName, const struct KzcMatrix4x4* value);
/** Sets texture uniform for renderer, using currently bound shader. */
void kzcRendererSetUniformTexture(struct KzcRenderer* renderer, kzString textureName);


/** Sets an active color slot value for renderer. */
void kzcRendererSetActiveColor(struct KzcRenderer* renderer, enum KzcRendererColor color, struct KzcColorRGBA colorRGBA);
/** Gets a pointer value of active color slot from renderer. */
struct KzcColorRGBA kzcRendererGetActiveColor(const struct KzcRenderer* renderer, enum KzcRendererColor color);
/** Switches an scalar value for renderer if necessary, e.g. depth clear value. */
void kzcRendererSetActiveScalar(struct KzcRenderer* renderer, enum KzcRendererScalar scalar, kzFloat value);
/** Returns a current scalar value from renderer. */
kzFloat kzcRendererGetActiveScalar(const struct KzcRenderer* renderer, enum KzcRendererScalar scalar);


/** Sets an blend mode for renderer. */
void kzcRendererSetBlendMode(struct KzcRenderer* renderer, enum KzcRendererBlendMode blendMode);
/** Sets a cull mode for renderer. */
void kzcRendererSetCullMode(struct KzcRenderer* renderer, enum KzcRendererCullMode cullMode);
/** Sets a stencil function for renderer. */
void kzcRendererSetStencilFunction(struct KzcRenderer* renderer, enum KzcRendererStencilFunction stencilFunction,
                                   kzUint referenceValue, kzUint maskValue);
/** Sets a stencil operation for renderer. */
void kzcRendererSetStencilOperation(struct KzcRenderer* renderer, enum KzcRendererStencilOperation stencilFailOperation,
                                    enum KzcRendererStencilOperation stencilPassDepthFailOperation,
                                    enum KzcRendererStencilOperation stencilPassDepthPassOperation);


/** Sets matrix for a renderer. */
void kzcRendererSetMatrix(struct KzcRenderer* renderer, enum KzcRendererMatrix matrixType, const struct KzcMatrix4x4* matrix);
/** Gets a matrix from renderer. */
struct KzcMatrix4x4 kzcRendererGetMatrix(const struct KzcRenderer* renderer, enum KzcRendererMatrix matrixType);
/** Calculates projection * camera * world matrix. */
void kzcRendererCalculateProjectionCameraWorldMatrix(struct KzcRenderer* renderer);
/** Applies transformations from renderer to GPU. */
kzsError kzcRendererApplyTransformation(struct KzcRenderer* renderer);


/** Sets texture unit active. */
void kzcRendererSetActiveTextureUnit(struct KzcRenderer* renderer, kzUint textureUnit);
/** Applies texture for renderer using currently bound shader (required) and texture uniform (required). */
kzsError kzcRendererApplyTexture(struct KzcRenderer* renderer, kzUint textureHandle, enum KzcRendererTextureType textureType,
                                 kzUint textureWidth, kzUint textureHeight);

/** Detaches texture handle from all texture units it has been applied for. */
void kzcRendererDetachTextureHandle(struct KzcRenderer* renderer, kzUint textureHandle);
/** Detaches texture from rendering. */
void kzcRendererDetachTexture(struct KzcRenderer* renderer);

/** Binds texture for renderer. */
void kzcRendererBindTexture(struct KzcRenderer* renderer, enum KzcRendererTextureType textureType, kzUint textureHandle);


/** Sets active shader for renderer. */
kzsError kzcRendererSetActiveShaderHandle(struct KzcRenderer* renderer, kzUint activeShaderHandle);
/** Sets active vertex attribute map for renderer. */
void kzcRendererSetActiveVertexAttributeMap(struct KzcRenderer* renderer, struct KzcHashMap* vertexAttributeMap);
/** Gets active vertex attribute map from renderer. */
struct KzcHashMap* kzcRendererGetActiveShaderAttributeMap(const struct KzcRenderer* renderer);


/** Sets active frame buffer for renderer. */
void kzcRendererSetActiveFrameBuffer(struct KzcRenderer* renderer, kzUint frameBufferHandle);
/** Callback prototype for setActiveFrameBuffer. */
typedef void (*KzcRendererFrameBufferCallback)(kzUint frameBufferHandle);
/** Sets callback function that is called after setActiveFrameBuffer. */
void kzcRendererSetActiveFrameBufferCallback(struct KzcRenderer* renderer, KzcRendererFrameBufferCallback callbackFunction);
/** Gets active frame buffer handle from. */
kzUint kzcRendererGetActiveFrameBuffer(const struct KzcRenderer* renderer);

/** Reads the pixels from the current frame buffer. */
void kzcRendererReadPixels(struct KzcRenderer* renderer, kzInt x, kzInt y, kzInt width, kzInt height,
                           enum KzcRendererColorFormat format, enum KzcRendererColorType type, void* pixels);

/** Begins a material for renderer. */
void kzcRendererBeginMaterialSetup(struct KzcRenderer* renderer);
/** Ends a material for renderer. */
void kzcRendererEndMaterialSetup(struct KzcRenderer* renderer);

/** 
* Begins set of vertex arrays.
* \param renderer Core renderer object.
* \param vertexArrayFlags Set of vertex arrays to be applied. Options: KZC_VERTEX_ARRAY_POSITION, KZC_VERTEX_ARRAY_NORMAL,
* KZC_VERTEX_ARRAY_TEXTURE_COORDINATE and KZC_VERTEX_ARRAY_COLOR.
*/
void kzcRendererBeginVertexArray(struct KzcRenderer* renderer, kzUint vertexArrayFlags);
/** Sets vertex array data. */
void kzcRendererSetVertexArrayData(const struct KzcRenderer* renderer, kzUint vertexArrayFlag, const void* data);
/** Ends vertex array. Renders the specified vertex lists and finishes the usage. */
void kzcRendererEndVertexArray(struct KzcRenderer* renderer, enum KzcPrimitiveType primitiveType, kzUint count);


/** Prints info about renderer capabilities. */
kzsError kzcRendererPrintInfo(const struct KzcRenderer* renderer);

/** Check supported extensions */
kzBool kzcRendererIsExtensionSupported(const struct KzcRenderer* renderer, enum KzcRendererExtension extension);


/** Sets line rendering width for renderer. */
void kzcRendererSetLineWidth(struct KzcRenderer* renderer, kzFloat lineWidth);

/** Render textured quad in orthogonal projection. x,y coordinates are window coordinates. Assumes that shader has been bound. */
kzsError kzcRendererDrawTexturedQuad2DAbsolute(struct KzcRenderer* renderer, const struct KzcTexture* texture, kzFloat x, kzFloat y,
                                               kzFloat width, kzFloat height, kzFloat u1, kzFloat v1, kzFloat u2, kzFloat v2);

/** Render quad with solid color in orthogonal projection. x,y coordinates are window coordinates. Assumes that correct shader has been bound. */
kzsError kzcRendererDrawSolidQuad2DAbsolute(struct KzcRenderer* renderer, kzFloat x, kzFloat y, kzFloat width,
                                            kzFloat height, struct KzcColorRGBA color);

/** Gets view port from renderer. */
void kzcRendererGetViewport(const struct KzcRenderer* renderer, kzFloat* x, kzFloat* y, kzFloat* width, kzFloat* height);

/** Sets debug logging state for renderer. */
void kzcRendererSetDebugLoggingState(struct KzcRenderer* renderer, kzBool loggingEnabled);
/** Gets triangle count from renderer. */
kzUint kzcRendererGetTriangleCount(const struct KzcRenderer* renderer);
/** Gets shader switches. */
kzUint kzcRendererGetShaderSwitchCount(const struct KzcRenderer* renderer);
/** Gets uniform sending count from renderer. */
kzUint kzcRendererGetUniformSendCount(const struct KzcRenderer* renderer);
/** Gets number of texture switches from renderer. */
kzUint kzcRendererGetTextureSwitchCount(const struct KzcRenderer* renderer);


/** Sets GPU calls enabled / disabled. */
void kzcRendererSetGPUCallsEnabled(struct KzcRenderer* renderer, kzBool enabled);
/** Checks if GPU calls are enabled. */
kzBool kzcRendererGPUCallsIsEnabled(const struct KzcRenderer* renderer);


#endif

#endif
