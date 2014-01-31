/**
* \file
* Core renderer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_renderer_es2.h"


#if KZ_OPENGL_VERSION ==  KZ_OPENGL_ES_2_0

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/resource_manager/index_buffer/kzc_resource_index_buffer.h>
#include <core/resource_manager/index_buffer/kzc_index_buffer_type.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex_buffer.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex.h>
#include <core/resource_manager/frame_buffer/kzc_resource_frame_buffer.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/shader/kzc_resource_shader.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/color/kzc_color.h>
#include <core/util/math/kzc_matrix2x2.h>
#include <core/util/math/kzc_matrix3x3.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector4.h>
#include <core/util/string/kzc_string.h>
#include <core/debug/kzc_log.h>
#include <core/kzc_error_codes.h>

#include <system/display/kzs_window.h>
#include <system/debug/kzs_log.h>
#include <system/kzs_opengl_extensions.h>
#include <system/wrappers/kzs_string.h>
#include <system/wrappers/kzs_opengles20.h>
#include <system/wrappers/kzs_memory.h>
#include <system/time/kzs_tick.h>


/* Maximum number of supported vertex colors. */
#define KZC_RENDERER_MAX_VERTEX_COLORS      4
/* Maximum number of supported custom attributes. */
#define KZC_RENDERER_MAX_CUSTOM_ATTRIBUTES  4
/* Enables uniform cache. */
#define KZC_RENDERER_ENABLE_UNIFORM_CACHE


const kzString KZC_RENDERER_ATTRIBUTE_POSITION = "kzPosition";
const kzString KZC_RENDERER_ATTRIBUTE_NORMAL = "kzNormal";
const kzString KZC_RENDERER_ATTRIBUTE_TANGENT = "kzTangent";
const kzString KZC_RENDERER_ATTRIBUTE_WEIGHT = "kzWeight";
const kzString KZC_RENDERER_ATTRIBUTE_MATRIX_PALETTE = "kzMatrixIndices";

const kzString KZC_RENDERER_TEXTURE_WIDTH[KZC_RENDERER_MAX_TEXTURE_UNITS] = {
    "kzTextureWidth0",
    "kzTextureWidth1",
    "kzTextureWidth2",
    "kzTextureWidth3",
    "kzTextureWidth4"
};

const kzString KZC_RENDERER_TEXTURE_HEIGHT[KZC_RENDERER_MAX_TEXTURE_UNITS] = {
    "kzTextureHeight0",
    "kzTextureHeight1",
    "kzTextureHeight2",
    "kzTextureHeight3",
    "kzTextureHeight4"
};

const kzString KZC_RENDERER_ATTRIBUTE_TEXTURE_COORDINATE[KZC_RENDERER_MAX_TEXTURE_UNITS] = {
    "kzTextureCoordinate0",
    "kzTextureCoordinate1",
    "kzTextureCoordinate2",
    "kzTextureCoordinate3",
    "kzTextureCoordinate4"
};

const kzString KZC_RENDERER_ATTRIBUTE_COLOR[KZC_RENDERER_MAX_VERTEX_COLORS] = {
    "kzColor0",
    "kzColor1",
    "kzColor2",
    "kzColor3"
};

const kzString KZC_RENDERER_ATTRIBUTE_CUSTOM[KZC_RENDERER_MAX_CUSTOM_ATTRIBUTES] = {
    "kzCustomAttribute0",
    "kzCustomAttribute1",
    "kzCustomAttribute2",
    "kzCustomAttribute3"
};


/** Structure for debugging informations. */
struct KzcRendererDebugInfo
{
    kzBool loggingEnabled;      /**< Logging enabled / disabled. */
    kzUint triangleCount;       /**< Triangle count. */
    kzUint shaderSwitches;      /**< Shader switches count. */
    kzUint textureSwitches;     /**< Texture switches count. */
    kzUint uniformSendings;     /**< Number of uniform sendings for frame. */
};

/** Defines viewport for renderer. */
struct KzcRendererViewport
{
    kzUint x;   /**< Absolute X coordinate of view port top-left. */
    kzUint y;   /**< Absolute Y coordinate of view port top-left. */
    kzUint width;   /** Absolute width of view port. */
    kzUint height;  /** Absolute width of view port. */
};

/** Fixed function material states. */
struct KzcRendererMaterialState
{
    kzBool blendApplied;    /**< Boolean for if blend is applied. */
};


/** Uniform cache element data for vec2's. */
struct KzcUniformVec2Value
{
    struct KzcVector2 value; /**< Data value. */
};

/** Uniform cache element data for vec3's. */
struct KzcUniformVec3Value
{
    struct KzcVector3 value; /**< Data value. */
};

/** Uniform cache element data for vec4's. */
struct KzcUniformVec4Value
{
    struct KzcVector4 value; /**< Data value. */
};

/** Uniform cache element data for mat2x2's. */
struct KzcUniformMatrix2x2Value
{
    struct KzcMatrix2x2 value; /**< Data value. */
};

/** Uniform cache element data for mat3x3's. */
struct KzcUniformMatrix3x3Value
{
    struct KzcMatrix3x3 value; /**< Data value. */
};

/** Uniform cache element data for mat4x4's. */
struct KzcUniformMatrix4x4Value
{
    struct KzcMatrix4x4 value; /**< Data value. */
};

/** Uniform cache element data for texture2D / textureCube. */
struct KzcUniformSamplerValue
{
    kzUint value; /**< Data value. */
};

/** Uniform cache element data for integer uniforms. */
struct KzcUniformIntegerValue
{
    kzInt value; /**< Data value. */
};

/** Uniform cache element data for arrays; Used in lights. */
struct KzcUniformFloatArray
{
    kzUint size; /**< Size of array element, i.e. list of floats, vec2's, vec3's or vec4's. */
    kzUint floatValueCount; /**< Number of array elements. */
    kzFloat* values; /** Values. */
};

#define KZC_RENDERER_UNIFORM_CACHE_FLOAT        0
#define KZC_RENDERER_UNIFORM_CACHE_VEC2         1
#define KZC_RENDERER_UNIFORM_CACHE_VEC3         2
#define KZC_RENDERER_UNIFORM_CACHE_VEC4         3
#define KZC_RENDERER_UNIFORM_CACHE_MATRIX4X4    4
#define KZC_RENDERER_UNIFORM_CACHE_SAMPLER      5
#define KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY  6
#define KZC_RENDERER_UNIFORM_CACHE_INTEGER      7
#define KZC_RENDERER_UNIFORM_CACHE_MATRIX3X3    8
#define KZC_RENDERER_UNIFORM_CACHE_MATRIX2X2    9
#define KZC_RENDERER_UNIFORM_CACHE_COUNT        10

struct KzcUniformCache
{
    struct KzcHashMap* uniformCache[KZC_RENDERER_UNIFORM_CACHE_COUNT];
    struct KzcHashMap* uniformLocationCache;
};


/** Structure for renderer. Contains rendering state machine, extension handling, choosing code path etc. */
struct KzcRenderer
{
    kzInt renderState[KZC_RENDERER_MAX_STATES]; /**< Render states .*/

    struct KzcColorRGBA activeColor[KZC_RENDERER_MAX_COLORS]; /**< Active colors for renderer, e.g. current diffuse material or clear color value. */
    struct KzcMatrix4x4 activeMatrix[KZC_RENDERER_MAX_MATRICES]; /**< Active matrices for renderer, e.g. current projection or camera matrices. */
    struct KzcRendererMaterialState materialState; /**< Defines material state for renderer. */

    kzFloat activeScalar[KZC_RENDERER_MAX_SCALARS]; /**< Active scalar that renderer contains. */
    kzUint activeShaderHandle; /**< Active shader handle. */

    enum KzcRendererBlendMode activeBlendMode; /**< Active blend mode for renderer. */
    enum KzcRendererCullMode activeCullMode; /**< Active culling mode for renderer. */
    enum KzcRendererStencilFunction activeStencilFunction; /**< Active stencil mode for renderer. */
    enum KzcRendererStencilOperation activeStencilOperationStencilFail; /**< Active stencil fail operation for renderer. */
    enum KzcRendererStencilOperation activeStencilOperationStencilPassDepthFail; /**< Active stencil pass depth fail operation for renderer. */
    enum KzcRendererStencilOperation activeStencilOperationStencilPassDepthPass; /**< Active stencil pass depth pass operation for renderer. */

    struct KzsWindow* activeWindow; /**< Active window for renderer. */
    struct KzcRendererViewport viewport; /**< Viewport area for rendering.. */

    kzInt clearStencilValue; /**< Stencil clear value for renderer. */
    kzUint clearTarget;  /**< Clear target flag for renderer. */

    kzUint currentVertexCount; /**< Current vertex count in vertex buffer. */
    kzUint currentIndexCount; /**< Number of indices currently bound index buffer contains. */
    void* currentIndexData; /**< Current index buffer data. */
    kzUint currentIndexPrimitiveType; /**< Type of the currently bound index buffer */

    kzUint activeTextureUnit; /**< Current texture unit for renderer. */
    kzString currentTextureUniform; /**< Current texture uniform for renderer. */
    struct KzcHashMap* vertexAttributeMap; /**< Attribute map of the currently bound shader. <attributeName, bindIndex> */
    kzUint currentTextureHandle[KZC_RENDERER_MAX_TEXTURE_UNITS]; /**< Current texture handle. */
    kzUint currentBuffer[KZC_RENDERER_MAX_BUFFERS]; /**< Currently bound buffer. */

    kzBool supportedExtensions[KZC_EXTENSION_COUNT]; /** Booleans describing the supported extensions. */

    kzUint currentVertexArrayFlags; /**< Currently bound vertex array flags. */

    kzUint activeFrameBufferHandle; /**< Currently bound framebuffer, 0 if none. */

    struct KzcMemoryManager* uniformCacheMemoryManager; /**< Memory manager for uniform cache. */

    struct KzcHashMap* uniformCaches; /**< Renderer uniform caches for all shaders. 
                                           hashmap<int(shaderHandle), KzcUniformCache(uniformCache)> */
                                               
    struct KzcUniformCache* currentUniformCache; /**< Current uniform cache. */

    struct KzcRendererDebugInfo debugInfo; /**< Renderer debug info. */

    struct KzcShader* shaderTextured; /**< Shader for textured objects. */
    struct KzcShader* shaderSolid; /**< Shader for solid objects. */

    KzcRendererFrameBufferCallback frameBufferCallback; /**< Framebuffer callback when changed. */

    kzFloat lineWidth; /**< Width for lines. */

    /* Float buffers for drawing with glDrawArrays. */
    kzFloat* uvArray;
    kzFloat* vertexArray;

    kzFloat* zeroFloatArray; /**< List of zero floats. */

    kzBool gpuCallsEnabled; /**< GPU calls enabled / disabled. */
};


/** Sets default values for renderer. */
static void kzcRendererSetDefaultValues_internal(struct KzcRenderer* renderer);
/** Initializes extensions for renderer. */
static kzsError kzcRendererExtensionInitialize_internal(struct KzcRenderer* renderer);

/** Checks if tho float array uniforms are equal. */
static kzBool kzcUniformFloatArrayIsEqual_internal(const struct KzcUniformFloatArray* array1,
                                                   const kzFloat* data, kzUint valueCount2);


kzsError kzcRendererCreate(const struct KzcMemoryManager* memoryManager, struct KzcRenderer** out_renderer)
{
    kzsError result;
    struct KzcRenderer* renderer;

    /* Allocate memory for renderer. */
    result = kzcMemoryAllocVariable(memoryManager, renderer, "RendererES2");
    kzsErrorForward(result);

    result = kzcMemoryManagerCreateQuickManager(memoryManager, KZC_RENDERER_UNIFORM_CACHE_SIZE, &renderer->uniformCacheMemoryManager);
    kzsErrorForward(result);

    result = kzcHashMapCreate(renderer->uniformCacheMemoryManager, KZC_HASH_MAP_CONFIGURATION_INT, &renderer->uniformCaches);
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, renderer->zeroFloatArray, 256, "ZeroFloatValues");
    kzsErrorForward(result);
    
    kzsMemset(renderer->zeroFloatArray, 0, sizeof(kzFloat) * kzcArrayLength(renderer->zeroFloatArray));

    result = kzcRendererExtensionInitialize_internal(renderer);
    kzsErrorForward(result);

    /* Set default values for renderer. */
    kzcRendererSetDefaultValues_internal(renderer);

    result = kzcMemoryAllocArray(memoryManager, renderer->vertexArray, 18, "VertexArray");
    kzsErrorForward(result);
    result = kzcMemoryAllocArray(memoryManager, renderer->uvArray, 12, "UvArray");
    kzsErrorForward(result);

    *out_renderer = renderer;
    kzsSuccess();
}

static kzsError kzcRendererExtensionInitialize_internal(struct KzcRenderer* renderer)
{
    kzsError result;
    kzInt i;
    kzString extensionString;
    const kzString KZC_EXTENSIONS_ES2[KZC_EXTENSION_COUNT] =
    {
        "GL_OES_depth24"
    };

    kzsAssert(kzcIsValidPointer(renderer));

    /* Get list of the extensions */
    extensionString = kzsGlGetString(KZS_GL_EXTENSIONS);

    /* Check supported extensions and store info*/
    for(i = 0; i < KZC_EXTENSION_COUNT; ++i)
    {
        kzUint index;
        renderer->supportedExtensions[i] = kzcStringFindSubstring(extensionString, KZC_EXTENSIONS_ES2[i], &index);

        if(renderer->supportedExtensions[i])
        {
            kzMutableString infoString;
            result = kzcStringFormat(kzcMemoryGetManager(renderer), "Extension supported %s", &infoString, extensionString);
            kzsErrorForward(result);
            kzsLog(KZS_LOG_LEVEL_INFO, infoString);
            
            result = kzcStringDelete(infoString);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}

static void kzcRendererResetGPUStates_internal(struct KzcRenderer* renderer)
{
    kzUint i;
    renderer->activeScalar[KZC_RENDERER_SCALAR_CLEAR_DEPTH] = -1.0f;

    for(i = 0; i < KZC_RENDERER_MAX_COLORS; ++i)
    {
        renderer->activeColor[i] = kzcColorRGBA(0.0f, 0.0f, 0.0f, 0.0f);
    }

    for(i = 0; i < KZC_RENDERER_MAX_TEXTURE_UNITS; ++i)
    {
        renderer->currentTextureHandle[i] = 0;
    }

    for(i = 0; i < KZC_RENDERER_MAX_MATRICES; ++i)
    {
        kzcMatrix4x4SetIdentity(&renderer->activeMatrix[i]);
    }

    for(i = 0; i < KZC_RENDERER_MAX_BUFFERS; ++i)
    {
        renderer->currentBuffer[i] = 0;
    }

    renderer->currentIndexCount = 0;
    renderer->currentVertexCount = 0;
    renderer->currentIndexData = KZ_NULL;
    renderer->currentIndexPrimitiveType = KZS_GL_TRIANGLES;
    renderer->vertexAttributeMap  = KZ_NULL;
    renderer->activeFrameBufferHandle = 0;
    renderer->activeShaderHandle = 0;
    renderer->currentVertexArrayFlags = 0;
    renderer->viewport.x = 0;
    renderer->viewport.y = 0;
    renderer->viewport.width = 0;
    renderer->viewport.height = 0;
    renderer->clearTarget = 0;

    renderer->currentTextureUniform = KZ_NULL;
    renderer->lineWidth = 1.0f;

    kzsGlClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    kzsGlEnable(KZS_GL_CULL_FACE);
    kzsGlCullFace(KZS_GL_BACK);
    renderer->activeCullMode = KZC_RENDERER_CULL_MODE_BACK;
    renderer->renderState[KZC_RENDERER_STATE_CULL_FACE] = KZC_RENDERER_STATE_MODE_ENABLED;

    /* Depth settings. */
    renderer->renderState[KZC_RENDERER_STATE_DEPTH_TEST] = KZC_RENDERER_STATE_MODE_ENABLED;
    kzsGlEnable(KZS_GL_DEPTH_TEST);
    kzsGlDepthFunc(KZS_GL_LESS);
    renderer->renderState[KZC_RENDERER_STATE_DEPTH_WRITE] = KZC_RENDERER_STATE_MODE_ENABLED;
    kzsGlDepthMask(KZ_TRUE);

    /* Stencil settings. */
    renderer->activeStencilFunction = KZC_RENDERER_STENCIL_MODE_ALWAYS;
    renderer->activeStencilOperationStencilFail = KZC_RENDERER_STENCIL_OPERATION_KEEP;
    renderer->activeStencilOperationStencilPassDepthFail = KZC_RENDERER_STENCIL_OPERATION_KEEP;
    renderer->activeStencilOperationStencilPassDepthPass = KZC_RENDERER_STENCIL_OPERATION_KEEP;

    renderer->renderState[KZC_RENDERER_STATE_BLEND] = KZC_RENDERER_STATE_MODE_DISABLED;
    renderer->activeBlendMode = KZC_RENDERER_BLEND_MODE_OPAQUE;
    kzsGlDisable(KZS_GL_BLEND);

    renderer->renderState[KZC_RENDERER_STATE_MULTISAMPLE] = KZC_RENDERER_STATE_MODE_DISABLED;
    kzsGlDisable(KZS_GL_SAMPLE_COVERAGE);

    kzsGlStencilFunc(KZS_GL_ALWAYS, 0, 0xFF);
    kzsGlStencilOp(KZS_GL_KEEP, KZS_GL_KEEP, KZS_GL_KEEP);

    renderer->activeTextureUnit = 0;
    kzsGlActiveTexture(KZS_GL_TEXTURE0);

    kzsGlLineWidth(1.0f);

    kzsGlBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, 0);
    kzsGlBindBuffer(KZS_GL_ARRAY_BUFFER, 0);
    renderer->gpuCallsEnabled = KZ_TRUE;
}

static void kzcRendererSetDefaultValues_internal(struct KzcRenderer* renderer)
{
    kzcRendererResetGPUStates_internal(renderer);

    /* Debug info. */
    renderer->debugInfo.loggingEnabled = KZ_FALSE;
    renderer->debugInfo.shaderSwitches = 0;
    renderer->debugInfo.triangleCount = 0;
    renderer->debugInfo.textureSwitches = 0;
    renderer->debugInfo.uniformSendings = 0;

    renderer->activeWindow = KZ_NULL;
    renderer->shaderTextured = KZ_NULL;
    renderer->shaderSolid = KZ_NULL;

    renderer->frameBufferCallback = KZ_NULL;
}

kzsError kzcRendererDelete(struct KzcRenderer* renderer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(renderer));

    result = kzcMemoryFreeArray(renderer->vertexArray);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray(renderer->zeroFloatArray);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray(renderer->uvArray);
    kzsErrorForward(result);

    result = kzcMemoryManagerDelete(renderer->uniformCacheMemoryManager);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(renderer);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcRendererSetDefaultShaders(struct KzcRenderer* renderer, struct KzcShader* solidColorShader, struct KzcShader* texturedShader)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->shaderSolid = solidColorShader;
    renderer->shaderTextured = texturedShader;
}

void kzcRendererResetFrame(struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));

    renderer->debugInfo.shaderSwitches = 0;
    renderer->debugInfo.triangleCount = 0;
    renderer->debugInfo.textureSwitches = 0;
    renderer->debugInfo.uniformSendings = 0;
}

kzsError kzcRendererResetLights(struct KzcRenderer* renderer)
{
    kzsError result;
    struct KzcHashMapIterator it;

    it = kzcHashMapGetIterator(renderer->uniformCaches);
    while(kzcHashMapIterate(it))
    {
        kzUint* shaderHandle = (kzUint*)kzcHashMapIteratorGetKey(it);
        struct KzcUniformCache* uniformCache = (struct KzcUniformCache*)kzcHashMapIteratorGetValue(it);

        kzsAssert(shaderHandle != KZ_NULL);

        result = kzcRendererSetActiveShaderHandle(renderer, *shaderHandle);
        kzsErrorForward(result);

        /* Get float array uniform cache. */
        if(uniformCache != KZ_NULL && uniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY] != KZ_NULL)
        {
            kzUint i;
            struct KzcHashMapIterator floatArrayIterator = kzcHashMapGetIterator(uniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY]);

            while(kzcHashMapIterate(floatArrayIterator))
            {
                kzUint* uniformHandle = (kzUint*)kzcHashMapIteratorGetKey(floatArrayIterator);
                struct KzcUniformFloatArray* foundValue = (struct KzcUniformFloatArray*)kzcHashMapIteratorGetValue(floatArrayIterator);

                kzsAssert(uniformHandle != KZ_NULL);
                kzsAssert(foundValue != KZ_NULL);

                if(foundValue->size == 1)
                {
                    kzsAssert(foundValue->floatValueCount * 1 < kzcArrayLength(renderer->zeroFloatArray));
                    kzsGlUniform1fv((kzInt)(*uniformHandle), (kzInt)foundValue->floatValueCount, renderer->zeroFloatArray);
                }
                else if(foundValue->size == 3)
                {
                    kzsAssert(foundValue->floatValueCount * 3 < kzcArrayLength(renderer->zeroFloatArray));
                    kzsGlUniform3fv((kzInt)(*uniformHandle), (kzInt)foundValue->floatValueCount, renderer->zeroFloatArray);
                }
                else if(foundValue->size == 4)
                {
                    kzsAssert(foundValue->floatValueCount * 4 < kzcArrayLength(renderer->zeroFloatArray));
                    kzsGlUniform4fv((kzInt)(*uniformHandle), (kzInt)foundValue->floatValueCount, renderer->zeroFloatArray);
                }

                for(i = 0; i < foundValue->floatValueCount * foundValue->size; ++i)
                {
                    foundValue->values[i] = 0.0f;
                }
            }
        }
    }

    kzsSuccess();
}

kzsError kzcRendererReset(struct KzcRenderer* renderer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(renderer));

    renderer->currentUniformCache = KZ_NULL;

    result = kzcMemoryManagerResetQuickManager(renderer->uniformCacheMemoryManager);
    kzsErrorForward(result);

    result = kzcHashMapCreate(renderer->uniformCacheMemoryManager, KZC_HASH_MAP_CONFIGURATION_INT, &renderer->uniformCaches);
    kzsErrorForward(result);

    result = kzcRendererResetLights(renderer);
    kzsErrorForward(result);

    {
        kzUint i;
        for(i = 0; i < KZC_RENDERER_MAX_TEXTURE_UNITS; ++i)
        {
            renderer->currentTextureHandle[i] = 0;
        }
    }

    result = kzcRendererSetActiveShaderHandle(renderer, 0);
    kzsErrorForward(result);

    kzcRendererResetGPUStates_internal(renderer);

    kzsSuccess();
}

void kzcRendererSetActiveWindow(struct KzcRenderer* renderer, struct KzsWindow* window)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->activeWindow = window;
}

struct KzsWindow* kzcRendererGetActiveWindow(const struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->activeWindow;
}

void kzcRendererSetState(struct KzcRenderer* renderer, enum KzcRendererState state)
{
    kzsAssert(kzcIsValidPointer(renderer));

    /* If the queried state is disabled, enable (set) it. */
    if(renderer->renderState[state] != KZC_RENDERER_STATE_MODE_ENABLED)
    {
        switch(state)
        {
            case KZC_RENDERER_STATE_DEPTH_TEST:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlEnable(KZS_GL_DEPTH_TEST);
                break;
            }
            case KZC_RENDERER_STATE_CULL_FACE:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlEnable(KZS_GL_CULL_FACE);
                break;
            }
            case KZC_RENDERER_STATE_DEPTH_WRITE:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlDepthMask(KZ_TRUE);
                break;
            }
            case KZC_RENDERER_STATE_STENCIL_TEST:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlEnable(KZS_GL_STENCIL_TEST);
                break;
            }
            case KZC_RENDERER_STATE_STENCIL_WRITE:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlStencilMask(0xFF);
                break;
            }
            case KZC_RENDERER_STATE_MULTISAMPLE:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlEnable(KZS_GL_SAMPLE_COVERAGE);
                kzsGlSampleCoverage(1.0f, KZ_FALSE);
                break;
            }
            case KZC_RENDERER_STATE_BLEND:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlEnable(KZS_GL_BLEND);
                break;
            }
            case KZC_RENDERER_STATE_SCISSOR_TEST:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlEnable(KZS_GL_SCISSOR_TEST);
                break;
            }
            case KZC_RENDERER_MAX_STATES:
            default:
            {
                kzsLog(KZS_LOG_LEVEL_WARNING, "Renderer state not defined at kzcRendererSetState!");
                break;
            }
        }
    }
}

void kzcRendererDisableState(struct KzcRenderer* renderer, enum KzcRendererState state)
{
    kzsAssert(kzcIsValidPointer(renderer));

    /* If the queried state is enabled, disable it. */
    if(renderer->renderState[state] != KZC_RENDERER_STATE_MODE_DISABLED)
    {
        switch(state)
        {
            case KZC_RENDERER_STATE_DEPTH_TEST:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_DISABLED;
                kzsGlDisable(KZS_GL_DEPTH_TEST);
                break;
            }
            case KZC_RENDERER_STATE_CULL_FACE:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_DISABLED;
                kzsGlDisable(KZS_GL_CULL_FACE);
                break;
            }
            case KZC_RENDERER_STATE_DEPTH_WRITE:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_DISABLED;
                kzsGlDepthMask(KZ_FALSE);
                break;
            }
            case KZC_RENDERER_STATE_STENCIL_TEST:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_DISABLED;
                kzsGlDisable(KZS_GL_STENCIL_TEST);
                break;
            }
            case KZC_RENDERER_STATE_STENCIL_WRITE:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_DISABLED;
                kzsGlStencilMask(0);
                break;
            }
            case KZC_RENDERER_STATE_BLEND:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_DISABLED;
                kzsGlDisable(KZS_GL_BLEND);
                break;
            }
            case KZC_RENDERER_STATE_MULTISAMPLE:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_DISABLED;
                kzsGlDisable(KZS_GL_SAMPLE_COVERAGE);
                break;
            }
            case KZC_RENDERER_STATE_SCISSOR_TEST:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_DISABLED;
                kzsGlDisable(KZS_GL_SCISSOR_TEST);
                break;
            }
            case KZC_RENDERER_MAX_STATES:
            default:
            {
                kzsLog(KZS_LOG_LEVEL_WARNING, "Renderer state not defined at kzcRendererDisableState!");
                break;
            }
        }
    }
}

void kzcRendererClear(const struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(renderer->clearTarget != 0)
    {
        if(renderer->gpuCallsEnabled)
        {
            kzsGlClear(renderer->clearTarget);
        }
    }
}

void kzcRendererResetClearTarget(struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->clearTarget = 0;
}

void kzcRendererAddClearTarget(struct KzcRenderer* renderer, enum KzcRendererClearTarget target)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(target == KZC_RENDERER_CLEAR_TARGET_COLOR)
    {
        renderer->clearTarget |= KZS_GL_COLOR_BUFFER_BIT;
    }
    else if(target == KZC_RENDERER_CLEAR_TARGET_DEPTH)
    {
        renderer->clearTarget |= KZS_GL_DEPTH_BUFFER_BIT;
    }
    else if(target ==  KZC_RENDERER_CLEAR_TARGET_STENCIL)
    {
        renderer->clearTarget |= KZS_GL_STENCIL_BUFFER_BIT;
    }
    else
    {
        kzsAssertText(KZ_FALSE, "Invalid clear target specified");
    }
}

void kzcRendererSetScissorTest(struct KzcRenderer* renderer, kzBool enabled, kzUint x, kzUint y, kzUint width, kzUint height)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(enabled)
    {
        kzcRendererSetState(renderer, KZC_RENDERER_STATE_SCISSOR_TEST);
        kzsGlScissor((kzInt)x, (kzInt)y, (kzInt)width, (kzInt)height);
    }
    else
    {
        kzcRendererDisableState(renderer, KZC_RENDERER_STATE_SCISSOR_TEST);
    }
}

kzBool kzcRendererHasClearTarget(const struct KzcRenderer* renderer, enum KzcRendererClearTarget target)
{
    kzBool hasTarget = KZ_FALSE;

    kzsAssert(kzcIsValidPointer(renderer));

    if(target == KZC_RENDERER_CLEAR_TARGET_COLOR && (renderer->clearTarget & KZS_GL_COLOR_BUFFER_BIT) != 0)
    {
        hasTarget = KZ_TRUE;
    }
    else if(target == KZC_RENDERER_CLEAR_TARGET_DEPTH && (renderer->clearTarget & KZS_GL_DEPTH_BUFFER_BIT) != 0)
    {
        hasTarget = KZ_TRUE;
    }
    else if(target == KZC_RENDERER_CLEAR_TARGET_STENCIL && (renderer->clearTarget & KZS_GL_STENCIL_BUFFER_BIT) != 0)
    {
        hasTarget = KZ_TRUE;
    }

    return hasTarget;
}

void kzcRendererRemoveClearTarget(struct KzcRenderer* renderer, enum KzcRendererClearTarget target)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(target == KZC_RENDERER_CLEAR_TARGET_COLOR)
    {
        if((renderer->clearTarget & KZS_GL_COLOR_BUFFER_BIT) != 0)
        {
            renderer->clearTarget ^= KZS_GL_COLOR_BUFFER_BIT;
        }
    }
    else if(target == KZC_RENDERER_CLEAR_TARGET_DEPTH)
    {
        if((renderer->clearTarget & KZS_GL_STENCIL_BUFFER_BIT) != 0)
        {
            renderer->clearTarget ^= KZS_GL_DEPTH_BUFFER_BIT;
        }
    }
    else if(target ==  KZC_RENDERER_CLEAR_TARGET_STENCIL)
    {
        if((renderer->clearTarget & KZS_GL_STENCIL_BUFFER_BIT) != 0)
        {
            renderer->clearTarget ^= KZS_GL_STENCIL_BUFFER_BIT;
        }
    }
    else
    {
        kzsAssertText(KZ_FALSE, "Invalid clear target specified");
    }
}

void kzcRendererSetColorWriteMode(enum KzcRendererColorWriteMode colorWriteMode)
{
    switch(colorWriteMode)
    {
        case KZC_RENDERER_COLOR_WRITE_NONE:
        {
            kzsGlColorMask(KZ_FALSE, KZ_FALSE, KZ_FALSE, KZ_FALSE); break;
        }
        case KZC_RENDERER_COLOR_WRITE_RGB:
        {
            kzsGlColorMask(KZ_TRUE, KZ_TRUE, KZ_TRUE, KZ_FALSE); break;
        }
        case KZC_RENDERER_COLOR_WRITE_RGBA:
        {
            kzsGlColorMask(KZ_TRUE, KZ_TRUE, KZ_TRUE, KZ_TRUE); break;
        }
        case KZC_RENDERER_COLOR_WRITE_ALPHA:
        {
            kzsGlColorMask(KZ_FALSE, KZ_FALSE, KZ_FALSE, KZ_TRUE); break;
        }
        case KZC_RENDERER_COLOR_WRITE_RED:
        {
            kzsGlColorMask(KZ_TRUE, KZ_FALSE, KZ_FALSE, KZ_TRUE); break;
        }
        case KZC_RENDERER_COLOR_WRITE_GREEN:
        {
            kzsGlColorMask(KZ_FALSE, KZ_TRUE, KZ_FALSE, KZ_TRUE); break;
        }
        case KZC_RENDERER_COLOR_WRITE_BLUE:
        {
            kzsGlColorMask(KZ_FALSE, KZ_FALSE, KZ_TRUE, KZ_TRUE); break;
        }
        case KZC_RENDERER_COLOR_WRITE_CYAN:
        {
            kzsGlColorMask(KZ_FALSE, KZ_TRUE, KZ_TRUE, KZ_TRUE); break;
        }
        default:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Color write mode not defined for kzcRendererSetColorWriteMode!");
            break;
        }
    }
}

kzBool kzcRendererIsClearEnabled(const struct KzcRenderer* renderer)
{
    return renderer->clearTarget != 0 ? KZ_TRUE : KZ_FALSE;
}

void kzcRendererSetClearStencil(struct KzcRenderer* renderer, kzInt stencilClearValue)
{
    if(renderer->clearStencilValue != stencilClearValue)
    {
        renderer->clearStencilValue = stencilClearValue;
        kzsGlClearStencil(stencilClearValue);
    }
}

static kzBool kzcUniformFloatArrayIsEqual_internal(const struct KzcUniformFloatArray* array1,
                                                   const kzFloat* data, kzUint valueCount2)
{
    kzUint i;
    kzUint valueCount = array1->floatValueCount;
    kzBool result = KZ_TRUE;

    if(valueCount == valueCount2)
    {
        kzFloat* valueA = array1->values;

        for(i = 0; i < valueCount * array1->size; ++i)
        {
            if(!kzsFloatIsEqual(valueA[i], data[i]))
            {
                result = KZ_FALSE;
                break;
            }
        }
    }
    else
    {
        result = KZ_FALSE;
    }

    return result;
}

kzsError kzcRendererGetUniformLocation(const struct KzcRenderer* renderer, kzString uniformName, kzInt *out_uniformLocation)
{
    kzsError result;
    kzInt* uniformLocation;
    kzBool uniformFound;

    kzsAssert(kzcIsValidPointer(renderer));
    
    uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformLocationCache, (void*)uniformName, (void**)&uniformLocation);
    if(!uniformFound)
    {
        result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, uniformLocation, "Uniform location");
        kzsErrorForward(result);

        *uniformLocation = kzsGlGetUniformLocation(renderer->activeShaderHandle, uniformName);

        result = kzcHashMapPut(renderer->currentUniformCache->uniformLocationCache, (void*)uniformName, uniformLocation);
        kzsErrorForward(result);
    }

    *out_uniformLocation = *uniformLocation;
    kzsSuccess();
}

kzsError kzcRendererSetUniformInteger(struct KzcRenderer* renderer, kzString uniformName, kzInt value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;
    kzBool assignValue = KZ_FALSE;
    
    struct KzcUniformIntegerValue* foundValue;
    result = kzcRendererGetUniformLocation(renderer, uniformName, &uniformLocation);
    kzsErrorForward(result);

    uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_INTEGER], &uniformLocation, (void**)&foundValue);
    if(!uniformFound)
    {
        kzInt* allocatedUniformLocation;

        result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
        kzsErrorForward(result);

        *allocatedUniformLocation = uniformLocation;

        result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheIntegerValue");
        kzsErrorForward(result);
        foundValue->value = value;

        result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_INTEGER], allocatedUniformLocation, foundValue);
        kzsErrorForward(result);
        assignValue = KZ_TRUE;
    }
    else if(foundValue->value != value)
    {
        foundValue->value = value;
        assignValue = KZ_TRUE;
    }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
    if(assignValue)
#endif
    {
        kzsGlUniform1i(uniformLocation, value);
        if(renderer->debugInfo.loggingEnabled)
        {
            ++renderer->debugInfo.uniformSendings;
        }
    }

    kzsSuccess();
}

kzsError kzcRendererSetUniformFloat(struct KzcRenderer* renderer, kzString uniformName, kzFloat value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;
    kzBool assignValue = KZ_FALSE;
    kzFloat* foundValue;
    result = kzcRendererGetUniformLocation(renderer, uniformName, &uniformLocation);
    kzsErrorForward(result);

    uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT], &uniformLocation, (void**)&foundValue);
    if(!uniformFound)
    {
        kzInt* allocatedUniformLocation;
        
        result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
        kzsErrorForward(result);

        *allocatedUniformLocation = uniformLocation;

        result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheFloatValue");
        kzsErrorForward(result);
        *foundValue = value;

        result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT], allocatedUniformLocation, foundValue);
        kzsErrorForward(result);
        assignValue = KZ_TRUE;
    }
    else if(!kzsFloatIsEqual(*foundValue, value))
    {
        *foundValue = value;
        assignValue = KZ_TRUE;
    }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
    if(assignValue)
#endif
    {
        kzsGlUniform1f(uniformLocation, value);
        if(renderer->debugInfo.loggingEnabled)
        {
            ++renderer->debugInfo.uniformSendings;
        }
    }

    kzsSuccess();
}

kzsError kzcRendererSetUniformFloatArray(struct KzcRenderer* renderer, kzString uniformName, kzUint count, const kzFloat* value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;

    result = kzcRendererGetUniformLocation(renderer, uniformName, &uniformLocation);
    kzsErrorForward(result);

    if(uniformLocation != -1)
    {
        kzBool assignValue = KZ_FALSE;
        struct KzcUniformFloatArray* foundValue;
        uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY], 
            &uniformLocation, (void**)&foundValue);

        if(!uniformFound)
        {
            kzInt* allocatedUniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
            kzsErrorForward(result);

            *allocatedUniformLocation = uniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheSamplerValue");
            kzsErrorForward(result);
            result = kzcMemoryAllocArray(renderer->uniformCacheMemoryManager, foundValue->values, 1 * count, "UniformCacheFloatArray");
            kzsErrorForward(result);
            kzsMemcpy(foundValue->values, value, 1 * count);
            foundValue->floatValueCount = count;
            foundValue->size = 1;

            result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY], allocatedUniformLocation, foundValue);
            kzsErrorForward(result);
            assignValue = KZ_TRUE;
        }
        else if(!kzcUniformFloatArrayIsEqual_internal(foundValue, (kzFloat*)value, count))
        {
            kzsMemcpy(foundValue->values, value, 1 * count);
            foundValue->floatValueCount = count;

            assignValue = KZ_TRUE;
        }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
        if(assignValue)
#endif
        {
            kzsGlUniform1fv((kzInt)uniformLocation,(kzInt) count, (kzFloat*)value);
            if(renderer->debugInfo.loggingEnabled)
            {
                ++renderer->debugInfo.uniformSendings;
            }
        }
    }

    kzsSuccess();
}

kzsError kzcRendererSetUniformVec2(struct KzcRenderer* renderer, kzString uniformName, const struct KzcVector2* value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;

    result = kzcRendererGetUniformLocation(renderer, uniformName, &uniformLocation);
    kzsErrorForward(result);

    if(uniformLocation != -1)
    {
        kzBool assignValue = KZ_FALSE;
        struct KzcUniformVec2Value* foundValue;
        uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_VEC2],
            &uniformLocation, (void**)&foundValue);

        if(!uniformFound)
        {
            kzInt* allocatedUniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
            kzsErrorForward(result);

            *allocatedUniformLocation = uniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheVec3Value");
            kzsErrorForward(result);
            foundValue->value = *value;

            result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_VEC2],
                allocatedUniformLocation, foundValue);
            kzsErrorForward(result);
            assignValue = KZ_TRUE;
        }
        else if(!kzcVector2IsEqual(&foundValue->value, value))
        {
            foundValue->value = *value;
            assignValue = KZ_TRUE;
        }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
        if(assignValue)
#endif
        {
            kzsGlUniform2f(uniformLocation, kzcVector2GetX(value), kzcVector2GetY(value));
            if(renderer->debugInfo.loggingEnabled)
            {
                ++renderer->debugInfo.uniformSendings;
            }
        }
    }
    kzsSuccess();
}

kzsError kzcRendererSetUniformVec3(struct KzcRenderer* renderer, kzString uniformName, const struct KzcVector3* value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;
    kzBool assignValue = KZ_FALSE;
    struct KzcUniformVec3Value* foundValue;

    result = kzcRendererGetUniformLocation(renderer, uniformName, &uniformLocation);
    kzsErrorForward(result);
    if(uniformLocation != -1)
    {
        uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_VEC3], &uniformLocation, (void**)&foundValue);
        if(!uniformFound)
        {
            kzInt* allocatedUniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
            kzsErrorForward(result);

            *allocatedUniformLocation = uniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheVec3Value");
            kzsErrorForward(result);
            foundValue->value = *value;

            result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_VEC3], allocatedUniformLocation, foundValue);
            kzsErrorForward(result);
            assignValue = KZ_TRUE;
        }
        else if(!kzcVector3IsEqual(&foundValue->value, value))
        {
            foundValue->value = *value;
            assignValue = KZ_TRUE;
        }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
        if(assignValue)
#endif
        {
            kzsGlUniform3f(uniformLocation, kzcVector3GetX(value), kzcVector3GetY(value), kzcVector3GetZ(value));
            if(renderer->debugInfo.loggingEnabled)
            {
                ++renderer->debugInfo.uniformSendings;
            }
        }
    }

    kzsSuccess();
}

kzsError kzcRendererSetUniformVecArray(struct KzcRenderer* renderer, kzString uniformName, kzUint count, kzUint dimension, const kzFloat* values)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;

    result = kzcRendererGetUniformLocation(renderer, uniformName, &uniformLocation);
    kzsErrorForward(result);

    if(uniformLocation != -1)
    {
        kzBool assignValue = KZ_FALSE;
        struct KzcUniformFloatArray* foundValue;
        uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY], &uniformLocation, (void**)&foundValue);

        if(!uniformFound)
        {
            kzInt* allocatedUniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
            kzsErrorForward(result);

            *allocatedUniformLocation = uniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheSamplerValue");
            kzsErrorForward(result);
            result = kzcMemoryAllocArray(renderer->uniformCacheMemoryManager, foundValue->values, dimension * count * sizeof(kzFloat), "UniformCacheFloatArray");
            kzsErrorForward(result);
            kzsMemcpy(foundValue->values, values, dimension * count);
            foundValue->floatValueCount = count;
            foundValue->size = dimension;

            result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY], allocatedUniformLocation, foundValue);
            kzsErrorForward(result);
            assignValue = KZ_TRUE;
        }
        else if(!kzcUniformFloatArrayIsEqual_internal(foundValue, values, count))
        {
            kzsMemcpy(foundValue->values, values, dimension * count);
            foundValue->floatValueCount = count;

            assignValue = KZ_TRUE;
        }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
        if(assignValue)
#endif
        {
            if(dimension == 1)
            {
                kzsGlUniform1fv((kzInt)uniformLocation, (kzInt)count, values);
            }
            else if(dimension == 2)
            {
                kzsGlUniform2fv((kzInt)uniformLocation, (kzInt)count, values);
            }
            else if(dimension == 3)
            {
                kzsGlUniform3fv((kzInt)uniformLocation, (kzInt)count, values);
            }
            else if(dimension == 4)
            {
                kzsGlUniform4fv((kzInt)uniformLocation, (kzInt)count, values);
            }
            
            if(renderer->debugInfo.loggingEnabled)
            {
                ++renderer->debugInfo.uniformSendings;
            }
        }
    }
    kzsSuccess();
}

kzsError kzcRendererSetUniformVec3Array(struct KzcRenderer* renderer, kzString uniformName, kzUint count, const struct KzcVector3* value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;

    result = kzcRendererGetUniformLocation(renderer, uniformName, &uniformLocation);
    kzsErrorForward(result);

    if(uniformLocation != -1)
    {
        kzBool assignValue = KZ_FALSE;
        struct KzcUniformFloatArray* foundValue;
        uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY], &uniformLocation, (void**)&foundValue);

        if(!uniformFound)
        {
            kzInt* allocatedUniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
            kzsErrorForward(result);

            *allocatedUniformLocation = uniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheSamplerValue");
            kzsErrorForward(result);
            result = kzcMemoryAllocArray(renderer->uniformCacheMemoryManager, foundValue->values, 3 * count * sizeof(kzFloat), "UniformCacheFloatArray");
            kzsErrorForward(result);
            kzsMemcpy(foundValue->values, value, 3 * count);
            foundValue->floatValueCount = count;
            foundValue->size = 3;

            result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY], allocatedUniformLocation, foundValue);
            kzsErrorForward(result);
            assignValue = KZ_TRUE;
        }
        else if(!kzcUniformFloatArrayIsEqual_internal(foundValue, (kzFloat*)&value->data[0], count))
        {
            kzsMemcpy(foundValue->values, value, 3 * count);
            foundValue->floatValueCount = count;

            assignValue = KZ_TRUE;
        }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
        if(assignValue)
#endif
        {
            kzsGlUniform3fv((kzInt)uniformLocation, (kzInt)count, (kzFloat*)&value->data[0]);
            if(renderer->debugInfo.loggingEnabled)
            {
                ++renderer->debugInfo.uniformSendings;
            }
        }
    }
    kzsSuccess();
}

kzsError kzcRendererSetUniformVec4(struct KzcRenderer* renderer, kzString uniformName, const struct KzcVector4* value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;

    result = kzcRendererGetUniformLocation(renderer, uniformName, &uniformLocation);
    kzsErrorForward(result);

    if(uniformLocation != -1)
    {
        kzBool assignValue = KZ_FALSE;
        struct KzcUniformVec4Value* foundValue;
        uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_VEC4],
            &uniformLocation, (void**)&foundValue);

        if(!uniformFound)
        {
            kzInt* allocatedUniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
            kzsErrorForward(result);

            *allocatedUniformLocation = uniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheSamplerValue");
            kzsErrorForward(result);
            foundValue->value = *value;

            result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_VEC4],
                allocatedUniformLocation, foundValue);
            kzsErrorForward(result);
            assignValue = KZ_TRUE;
        }
        else if(!kzcVector4IsEqual(&foundValue->value, value))
        {
            foundValue->value = *value;
            assignValue = KZ_TRUE;
        }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
        if(assignValue)
#endif
        {
            
            kzsGlUniform4f(uniformLocation, kzcVector4GetX(value), kzcVector4GetY(value), kzcVector4GetZ(value), kzcVector4GetW(value));

            if(renderer->debugInfo.loggingEnabled)
            {
                ++renderer->debugInfo.uniformSendings;
            }
        }
    }
    kzsSuccess();
}

kzsError kzcRendererSetUniformColorRGBA(struct KzcRenderer* renderer, kzString uniformName, const struct KzcColorRGBA* value)
{
    kzsError result;
    struct KzcVector4 vector = kzcVector4(value->red, value->green, value->blue, value->alpha);

    result = kzcRendererSetUniformVec4(renderer, uniformName, &vector);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcRendererSetUniformVec4Array(struct KzcRenderer* renderer, kzString uniformName, kzUint count, const struct KzcVector4* value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;

    result = kzcRendererGetUniformLocation(renderer, uniformName, &uniformLocation);
    kzsErrorForward(result);

    if(uniformLocation != -1)
    {
        kzBool assignValue = KZ_FALSE;
        struct KzcUniformFloatArray* foundValue;
        uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY],
            &uniformLocation, (void**)&foundValue);

        if(!uniformFound)
        {
            kzInt* allocatedUniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
            kzsErrorForward(result);

            *allocatedUniformLocation = uniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheSamplerValue");
            kzsErrorForward(result);
            result = kzcMemoryAllocArray(renderer->uniformCacheMemoryManager, foundValue->values, 4 * count, "UniformCacheFloatArray");
            kzsErrorForward(result);
            kzsMemcpy(foundValue->values, value, 4 * count);
            foundValue->floatValueCount = count;
            foundValue->size = 4;

            result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_FLOAT_ARRAY],
                allocatedUniformLocation, foundValue);
            kzsErrorForward(result);
            assignValue = KZ_TRUE;
        }
        else if(!kzcUniformFloatArrayIsEqual_internal(foundValue, (kzFloat*)&value->data[0], count))
        {
            kzsMemcpy(foundValue->values, value, 4 * count);
            foundValue->floatValueCount = count;

            assignValue = KZ_TRUE;
        }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
        if(assignValue)
#endif
        {
            kzsGlUniform4fv((kzInt)uniformLocation, (kzInt)count, (kzFloat*)&value->data[0]);
            if(renderer->debugInfo.loggingEnabled)
            {
                ++renderer->debugInfo.uniformSendings;
            }
        }
    }

    kzsSuccess();
}

kzsError kzcRendererSetUniformMatrix2x2(struct KzcRenderer* renderer, kzString matrixName, const struct KzcMatrix2x2* value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;

    result = kzcRendererGetUniformLocation(renderer, matrixName, &uniformLocation);
    kzsErrorForward(result);

    if(uniformLocation != -1)
    {
        kzBool assignValue = KZ_FALSE;
        struct KzcUniformMatrix2x2Value* foundValue;
        uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_MATRIX2X2],
            &uniformLocation, (void**)&foundValue);

        if(!uniformFound)
        {
            kzInt* allocatedUniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
            kzsErrorForward(result);

            *allocatedUniformLocation = uniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheMatrixValue");
            kzsErrorForward(result);
            foundValue->value = *value;

            result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_MATRIX2X2],
                allocatedUniformLocation, foundValue);
            kzsErrorForward(result);
            assignValue = KZ_TRUE;
        }
        else if(!kzcMatrix2x2IsEqual(&foundValue->value, (struct KzcMatrix2x2*)value))
        {
            foundValue->value = *value;
            assignValue = KZ_TRUE;
        }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
        if(assignValue)
#endif
        {
            kzsGlUniformMatrix2fv(uniformLocation, 1, KZ_FALSE, &value->data[0]);
            if(renderer->debugInfo.loggingEnabled)
            {
                ++renderer->debugInfo.uniformSendings;
            }
        }
    }
    kzsSuccess();
}

kzsError kzcRendererSetUniformMatrix3x3(struct KzcRenderer* renderer, kzString matrixName, const struct KzcMatrix3x3* value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;

    result = kzcRendererGetUniformLocation(renderer, matrixName, &uniformLocation);
    kzsErrorForward(result);

    if(uniformLocation != -1)
    {
        kzBool assignValue = KZ_FALSE;
        struct KzcUniformMatrix3x3Value* foundValue;
        uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_MATRIX3X3],
            &uniformLocation, (void**)&foundValue);

        if(!uniformFound)
        {
            kzInt* allocatedUniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
            kzsErrorForward(result);

            *allocatedUniformLocation = uniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheMatrixValue");
            kzsErrorForward(result);
            foundValue->value = *value;

            result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_MATRIX3X3],
                allocatedUniformLocation, foundValue);
            kzsErrorForward(result);
            assignValue = KZ_TRUE;
        }
        else if(!kzcMatrix3x3IsEqual(&foundValue->value, (struct KzcMatrix3x3*)value))
        {
            foundValue->value = *value;
            assignValue = KZ_TRUE;
        }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
        if(assignValue)
#endif
        {
            kzsGlUniformMatrix3fv(uniformLocation, 1, KZ_FALSE, &value->data[0]);
            if(renderer->debugInfo.loggingEnabled)
            {
                ++renderer->debugInfo.uniformSendings;
            }
        }
    }
    kzsSuccess();
}

kzsError kzcRendererSetUniformMatrix4x4(struct KzcRenderer* renderer, kzString matrixName, const struct KzcMatrix4x4* value)
{
    kzsError result;
    kzInt uniformLocation;
    kzBool uniformFound;

    result = kzcRendererGetUniformLocation(renderer, matrixName, &uniformLocation);
    kzsErrorForward(result);

    if(uniformLocation != -1)
    {
        kzBool assignValue = KZ_FALSE;
        struct KzcUniformMatrix4x4Value* foundValue;
        uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_MATRIX4X4],
            &uniformLocation, (void**)&foundValue);

        if(!uniformFound)
        {
            kzInt* allocatedUniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
            kzsErrorForward(result);

            *allocatedUniformLocation = uniformLocation;

            result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheMatrixValue");
            kzsErrorForward(result);
            foundValue->value = *value;

            result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_MATRIX4X4],
                allocatedUniformLocation, foundValue);
            kzsErrorForward(result);
            assignValue = KZ_TRUE;
        }
        else if(!kzcMatrix4x4IsEqual(&foundValue->value, (struct KzcMatrix4x4*)value))
        {
            foundValue->value = *value;
            assignValue = KZ_TRUE;
        }

#ifdef KZC_RENDERER_ENABLE_UNIFORM_CACHE
       if(assignValue)
#endif
        {
            kzsGlUniformMatrix4fv(uniformLocation, 1, KZ_FALSE, &value->data[0]);
            if(renderer->debugInfo.loggingEnabled)
            {
                ++renderer->debugInfo.uniformSendings;
            }
        }
    }

    kzsSuccess();
}

void kzcRendererSetUniformTexture(struct KzcRenderer* renderer, kzString textureName)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->currentTextureUniform = textureName;
}

void kzcRendererSetActiveColor(struct KzcRenderer* renderer, enum KzcRendererColor color, struct KzcColorRGBA colorRGBA)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->activeColor[color] = colorRGBA;
}

struct KzcColorRGBA kzcRendererGetActiveColor(const struct KzcRenderer* renderer, enum KzcRendererColor color)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->activeColor[color];
}

void kzcRendererSetActiveScalar(struct KzcRenderer* renderer, enum KzcRendererScalar scalar, kzFloat value)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->activeScalar[scalar] = value;
}

kzFloat kzcRendererGetActiveScalar(const struct KzcRenderer* renderer, enum KzcRendererScalar scalar)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->activeScalar[scalar];
}

void kzcRendererSetClearColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA)
{
    struct KzcColorRGBA activeColor = kzcRendererGetActiveColor(renderer, KZC_RENDERER_COLOR_CLEAR);
    if(!kzcColorRGBAIsEqual(&activeColor, &colorRGBA))
    {
        kzcRendererSetActiveColor(renderer, KZC_RENDERER_COLOR_CLEAR, colorRGBA);
        kzsGlClearColor(colorRGBA.red, colorRGBA.green, colorRGBA.blue, colorRGBA.alpha);
    }
}

void kzcRendererSetClearDepthValue(struct KzcRenderer* renderer, kzFloat clearDepthValue)
{
    kzFloat currentClearDepth = kzcRendererGetActiveScalar(renderer, KZC_RENDERER_SCALAR_CLEAR_DEPTH);
    if(!kzsFloatIsEqual(currentClearDepth, clearDepthValue))
    {
        kzcRendererSetActiveScalar(renderer, KZC_RENDERER_SCALAR_CLEAR_DEPTH, clearDepthValue);
        kzsGlClearDepthf(clearDepthValue);
    }
}

void kzcRendererSetViewport(struct KzcRenderer* renderer, kzUint x, kzUint y, kzUint width, kzUint height)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(x != renderer->viewport.x || y != renderer->viewport.y || width != renderer->viewport.width || height != renderer->viewport.height)
    {
        renderer->viewport.x = x;
        renderer->viewport.y = y;
        renderer->viewport.width = width;
        renderer->viewport.height = height;

        kzsGlViewport((kzInt)x, (kzInt)y, (kzInt)width, (kzInt)height);
    }
}

void kzcRendererSetBuffer(struct KzcRenderer* renderer, enum KzcRendererBuffer buffer, kzUint value)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(renderer->currentBuffer[buffer] != value)
    {
        renderer->currentBuffer[buffer] = value;
        switch(buffer)
        {
            case KZC_RENDERER_BUFFER_VERTEX:
            {
                kzsGlBindBuffer(KZS_GL_ARRAY_BUFFER, value);
                break;
            }
            case KZC_RENDERER_BUFFER_INDEX:
            {
                kzsGlBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, value);
                break;
            }
            case KZC_RENDERER_MAX_BUFFERS:
            default:
            {
                kzsAssert(KZ_FALSE);
            }
        }
    }
}

void kzcRendererDrawBuffers(struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(renderer->currentIndexCount != 0 && renderer->currentVertexCount != 0)
    {
        if(renderer->gpuCallsEnabled)
        {
            kzsGlDrawElements(renderer->currentIndexPrimitiveType, (kzInt)renderer->currentIndexCount, KZS_GL_UNSIGNED_SHORT, renderer->currentIndexData);
        }

        /* Increase triangle count for debug info. */
        renderer->debugInfo.triangleCount += renderer->currentIndexCount / 3;
    }
}

void kzcRendererSetIndexType(struct KzcRenderer* renderer, enum KzcPrimitiveType indexType)
{ 
    kzsAssert(kzcIsValidPointer(renderer));

    /* Convert the KZC_PRIMITIVE_TYPE_x to KZS_GL_x for drawing */
    switch (indexType)
    {
        case KZC_PRIMITIVE_TYPE_POINTS: renderer->currentIndexPrimitiveType = KZS_GL_POINTS; break;
        case KZC_PRIMITIVE_TYPE_LINES: renderer->currentIndexPrimitiveType = KZS_GL_LINES; break;
        case KZC_PRIMITIVE_TYPE_LINE_LOOP: renderer->currentIndexPrimitiveType = KZS_GL_LINE_LOOP; break;
        case KZC_PRIMITIVE_TYPE_LINE_STRIP: renderer->currentIndexPrimitiveType = KZS_GL_LINE_STRIP; break;
        case KZC_PRIMITIVE_TYPE_TRIANGLES: renderer->currentIndexPrimitiveType = KZS_GL_TRIANGLES; break;
        case KZC_PRIMITIVE_TYPE_TRIANGLE_STRIP: renderer->currentIndexPrimitiveType = KZS_GL_TRIANGLE_STRIP; break;
        case KZC_PRIMITIVE_TYPE_TRIANGLE_FAN: renderer->currentIndexPrimitiveType = KZS_GL_TRIANGLE_FAN; break;
        default:
        {
            /* Unknown type sent to index buffer */
            kzsLog(KZS_LOG_LEVEL_WARNING, "Unknown primitiveType for indexBuffer passed to kzcRendererSetIndexType_private()");
            kzsAssert(KZ_FALSE);
        }
    }
}

void kzcRendererSetVertexCount(struct KzcRenderer* renderer, kzUint vertexCount)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->currentVertexCount = vertexCount;
}

void kzcRendererSetIndexCount(struct KzcRenderer* renderer, kzUint indexCount)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->currentIndexCount = indexCount;
}

void kzcRendererSetIndexData(struct KzcRenderer* renderer, void* indexData)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->currentIndexData = indexData;
}

void kzcRendererSetLineWidth(struct KzcRenderer* renderer, kzFloat lineWidth)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(!kzsFloatIsEqual(lineWidth, renderer->lineWidth))
    {
        renderer->lineWidth = lineWidth;
        kzsGlLineWidth(lineWidth);
    }
}

void kzcRendererSetMatrix(struct KzcRenderer* renderer, enum KzcRendererMatrix matrixType, const struct KzcMatrix4x4* matrix)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->activeMatrix[matrixType] = *matrix;
}

struct KzcMatrix4x4 kzcRendererGetMatrix(const struct KzcRenderer* renderer, enum KzcRendererMatrix matrixType)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->activeMatrix[matrixType];
}

void kzcRendererCalculateProjectionCameraWorldMatrix(struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));

    kzcMatrix4x4Multiply(&renderer->activeMatrix[KZC_RENDERER_MATRIX_WORLD], &renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA],
        &renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA_WORLD]);
    kzcMatrix4x4Multiply(&renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA_WORLD], &renderer->activeMatrix[KZC_RENDERER_MATRIX_PROJECTION],
        &renderer->activeMatrix[KZC_RENDERER_MATRIX_PROJECTION_CAMERA_WORLD]);
}

kzsError kzcRendererApplyTransformation(struct KzcRenderer* renderer)
{
    kzsError result;
    kzInt uniformLocation;

    kzsAssert(kzcIsValidPointer(renderer));

    /* Apply necessary matrices to GPU. */
    result = kzcRendererSetUniformMatrix4x4(renderer, KZC_RENDERER_UNIFORM_WORLD_MATRIX, &renderer->activeMatrix[KZC_RENDERER_MATRIX_WORLD]);
    kzsErrorForward(result);
    result = kzcRendererSetUniformMatrix4x4(renderer, KZC_RENDERER_UNIFORM_CAMERA_MATRIX, &renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA]);
    kzsErrorForward(result);

    result = kzcRendererGetUniformLocation(renderer, KZC_RENDERER_UNIFORM_CAMERA_POSITION, &uniformLocation);
    kzsErrorForward(result);
    if(uniformLocation != -1)
    {
        struct KzcMatrix4x4 viewInverse;
        struct KzcVector3 cameraPosition;
        kzcMatrix4x4Inverse(&renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA], &viewInverse);
        cameraPosition = kzcVector3(viewInverse.data[12], viewInverse.data[13], viewInverse.data[14]);
        result = kzcRendererSetUniformVec3(renderer, KZC_RENDERER_UNIFORM_CAMERA_POSITION, &cameraPosition);
        kzsErrorForward(result);
    }
    
    result = kzcRendererGetUniformLocation(renderer, KZC_RENDERER_UNIFORM_NORMAL_MATRIX, &uniformLocation);
    kzsErrorForward(result);
    if(uniformLocation != -1)
    {
        struct KzcMatrix4x4 inverseTranspose;
        struct KzcMatrix4x4 worldInverse;
        kzcMatrix4x4Inverse(&renderer->activeMatrix[KZC_RENDERER_MATRIX_WORLD], &worldInverse);
        worldInverse.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] = 0.0f;
        worldInverse.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] = 0.0f;
        worldInverse.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] = 0.0f;
        kzcMatrix4x4Transpose(&worldInverse, &inverseTranspose);
        result = kzcRendererSetUniformMatrix4x4(renderer, KZC_RENDERER_UNIFORM_NORMAL_MATRIX, &inverseTranspose);
        kzsErrorForward(result);
    }
        
    kzcMatrix4x4MultiplyAffine(&renderer->activeMatrix[KZC_RENDERER_MATRIX_WORLD], &renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA], &renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA_WORLD]);
    
    result = kzcRendererGetUniformLocation(renderer, KZC_RENDERER_UNIFORM_PROJECTION_CAMERA_WORLD_MATRIX, &uniformLocation);
    kzsErrorForward(result);
    if(uniformLocation != -1)
    {
        struct KzcMatrix4x4 projectionMultCameraWorld;
        kzcMatrix4x4Multiply(&renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA_WORLD],
            &renderer->activeMatrix[KZC_RENDERER_MATRIX_PROJECTION],
            &projectionMultCameraWorld);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION_CAMERA_WORLD, &projectionMultCameraWorld);

        result = kzcRendererSetUniformMatrix4x4(renderer, KZC_RENDERER_UNIFORM_PROJECTION_CAMERA_WORLD_MATRIX, &renderer->activeMatrix[KZC_RENDERER_MATRIX_PROJECTION_CAMERA_WORLD]);
        kzsErrorForward(result);
    }

    result = kzcRendererSetUniformMatrix4x4(renderer, KZC_RENDERER_UNIFORM_PROJECTION_MATRIX, &renderer->activeMatrix[KZC_RENDERER_MATRIX_PROJECTION]);
    kzsErrorForward(result);
    result = kzcRendererSetUniformMatrix4x4(renderer, KZC_RENDERER_UNIFORM_CAMERA_WORLD_MATRIX, &renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA_WORLD]);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcRendererSetBlendMode(struct KzcRenderer* renderer, enum KzcRendererBlendMode blendMode)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(renderer->activeBlendMode != blendMode)
    {
        renderer->activeBlendMode = blendMode;
        switch(blendMode)
        {
            case KZC_RENDERER_BLEND_MODE_OPAQUE:
            {
                kzcRendererDisableState(renderer, KZC_RENDERER_STATE_BLEND);
                break;
            }
            case KZC_RENDERER_BLEND_MODE_ALPHA:
            {
                kzcRendererSetState(renderer, KZC_RENDERER_STATE_BLEND);
                kzsGlBlendFunc(KZS_GL_SRC_ALPHA, KZS_GL_ONE_MINUS_SRC_ALPHA);
                break;
            }
            case KZC_RENDERER_BLEND_MODE_ADDITIVE:
            {
                kzcRendererSetState(renderer, KZC_RENDERER_STATE_BLEND);
                kzsGlBlendFunc(KZS_GL_ONE, KZS_GL_ONE);
                break;
            }
            default:
            {
                kzsLog(KZS_LOG_LEVEL_WARNING, "Invalid blend mode!");
                break;
            }
        }
    }
    renderer->materialState.blendApplied = KZ_TRUE;
}

void kzcRendererSetCullMode(struct KzcRenderer* renderer, enum KzcRendererCullMode cullMode)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(renderer->activeCullMode != cullMode)
    {
        renderer->activeCullMode = cullMode;
        switch(cullMode)
        {
            case KZC_RENDERER_CULL_MODE_NONE:
            {
                kzcRendererDisableState(renderer, KZC_RENDERER_STATE_CULL_FACE);
                break;
            }
            case KZC_RENDERER_CULL_MODE_BACK:
            {
                kzcRendererSetState(renderer, KZC_RENDERER_STATE_CULL_FACE);
                kzsGlCullFace(KZS_GL_BACK);
                break;
            }
            case KZC_RENDERER_CULL_MODE_FRONT:
            {
                kzcRendererSetState(renderer, KZC_RENDERER_STATE_CULL_FACE);
                kzsGlCullFace(KZS_GL_FRONT);
                break;
            }
            default:
            {
                kzsLog(KZS_LOG_LEVEL_WARNING, "Invalid cull mode!");
                break;
            }
        }
    }
}


void kzcRendererSetStencilFunction(struct KzcRenderer* renderer, enum KzcRendererStencilFunction stencilFunction,
                                    kzUint referenceValue, kzUint maskValue)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(renderer->activeStencilFunction != stencilFunction)
    {
        renderer->activeStencilFunction = stencilFunction;
        switch(stencilFunction)
        {
            case KZC_RENDERER_STENCIL_MODE_NEVER:
            {
                kzsGlStencilFunc(KZS_GL_NEVER, (kzInt)referenceValue, maskValue);
                break;
            }
            case KZC_RENDERER_STENCIL_MODE_LESS:
            {
                kzsGlStencilFunc(KZS_GL_LESS, (kzInt)referenceValue, maskValue);
                break;
            }
            case KZC_RENDERER_STENCIL_MODE_LEQUAL:
            {
                kzsGlStencilFunc(KZS_GL_LEQUAL, (kzInt)referenceValue, maskValue);
                break;
            }
            case KZC_RENDERER_STENCIL_MODE_GREATER:
            {
                kzsGlStencilFunc(KZS_GL_GREATER, (kzInt)referenceValue, maskValue);
                break;
            }
            case KZC_RENDERER_STENCIL_MODE_GEQUAL:
            {
                kzsGlStencilFunc(KZS_GL_GEQUAL, (kzInt)referenceValue, maskValue);
                break;
            }
            case KZC_RENDERER_STENCIL_MODE_EQUAL:
            {
                kzsGlStencilFunc(KZS_GL_EQUAL, (kzInt)referenceValue, maskValue);
                break;
            }
            case KZC_RENDERER_STENCIL_MODE_NOT_EQUAL:
            {
                kzsGlStencilFunc(KZS_GL_NOTEQUAL, (kzInt)referenceValue, maskValue);
                break;
            }
            case KZC_RENDERER_STENCIL_MODE_ALWAYS:
            {
                kzsGlStencilFunc(KZS_GL_ALWAYS, (kzInt)referenceValue, maskValue);
                break;
            }
            default:
            {
                kzsAssertText(KZ_FALSE, "Invalid stencil mode passed for renderer!");
            }
        }
    }
}

static kzUint kzcRendererGetStencilOperationInteger_internal(enum KzcRendererStencilOperation stencilOperation)
{
    kzUint operation = KZS_GL_KEEP;

    switch(stencilOperation)
    {
        case KZC_RENDERER_STENCIL_OPERATION_KEEP: operation = KZS_GL_KEEP; break;
        case KZC_RENDERER_STENCIL_OPERATION_ZERO: operation = KZS_GL_ZERO; break;
        case KZC_RENDERER_STENCIL_OPERATION_INCREASE_WRAP: operation = KZS_GL_INCR_WRAP; break;
        case KZC_RENDERER_STENCIL_OPERATION_DECREASE_WRAP: operation = KZS_GL_DECR_WRAP; break;
        case KZC_RENDERER_STENCIL_OPERATION_INCREASE: operation = KZS_GL_INCR; break;
        case KZC_RENDERER_STENCIL_OPERATION_DECREASE: operation = KZS_GL_DECR; break;
        case KZC_RENDERER_STENCIL_OPERATION_INVERT: operation = KZS_GL_INVERT; break;
        case KZC_RENDERER_STENCIL_OPERATION_REPLACE: operation = KZS_GL_REPLACE; break;
        default: kzsAssertText(KZ_FALSE, "Invalid stencil operation passed for renderer!");
    }

    return operation;
}

void kzcRendererSetStencilOperation(struct KzcRenderer* renderer, enum KzcRendererStencilOperation stencilFailOperation,
                                    enum KzcRendererStencilOperation stencilPassDepthFailOperation,
                                    enum KzcRendererStencilOperation stencilPassDepthPassOperation)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(renderer->activeStencilOperationStencilFail != stencilFailOperation || 
       renderer->activeStencilOperationStencilPassDepthFail != stencilPassDepthFailOperation || 
       renderer->activeStencilOperationStencilPassDepthPass != stencilPassDepthPassOperation)
    {
        kzUint stencilFail = kzcRendererGetStencilOperationInteger_internal(stencilFailOperation);
        kzUint stencilPassDepthFail = kzcRendererGetStencilOperationInteger_internal(stencilPassDepthFailOperation);
        kzUint stencilPassDepthPass = kzcRendererGetStencilOperationInteger_internal(stencilPassDepthPassOperation);

        renderer->activeStencilOperationStencilFail = stencilFailOperation;
        renderer->activeStencilOperationStencilPassDepthFail = stencilPassDepthFailOperation;
        renderer->activeStencilOperationStencilPassDepthPass = stencilPassDepthPassOperation;

        kzsGlStencilOp(stencilFail, stencilPassDepthFail, stencilPassDepthPass);
    }
}

void kzcRendererSetActiveTextureUnit(struct KzcRenderer* renderer, kzUint textureUnit)
{
    if(textureUnit != renderer->activeTextureUnit)
    {
        renderer->activeTextureUnit = textureUnit;
        kzsGlActiveTexture(KZS_GL_TEXTURE0 + textureUnit);
    }
}

kzsError kzcRendererApplyTexture(struct KzcRenderer* renderer, kzUint textureHandle, enum KzcRendererTextureType textureType,
                                 kzUint textureWidth, kzUint textureHeight)
{
    kzsError result;

    if(renderer->activeShaderHandle != 0)
    {
        result = kzcRendererSetUniformFloat(renderer, KZC_RENDERER_TEXTURE_WIDTH[renderer->activeTextureUnit], (kzFloat)(kzInt)textureWidth);
        kzsErrorForward(result);
        result = kzcRendererSetUniformFloat(renderer, KZC_RENDERER_TEXTURE_HEIGHT[renderer->activeTextureUnit], (kzFloat)(kzInt)textureHeight);
        kzsErrorForward(result);

        /* Apply texture */
        if(renderer->currentTextureUniform != KZ_NULL)
        {
            kzInt uniformLocation;
            kzBool uniformFound;
            kzBool assignValue = KZ_FALSE;
            struct KzcUniformSamplerValue* foundValue;

            result = kzcRendererGetUniformLocation(renderer, renderer->currentTextureUniform, &uniformLocation);
            kzsErrorForward(result);
            uniformFound = kzcHashMapGet(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_SAMPLER], &uniformLocation, (void**)&foundValue);

            if(!uniformFound)
            {
                kzInt* allocatedUniformLocation;

                result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, allocatedUniformLocation, "");
                kzsErrorForward(result);

                *allocatedUniformLocation = uniformLocation;

                result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, foundValue, "UniformCacheSamplerValue");
                kzsErrorForward(result);
                foundValue->value = renderer->activeTextureUnit;

                result = kzcHashMapPut(renderer->currentUniformCache->uniformCache[KZC_RENDERER_UNIFORM_CACHE_SAMPLER], allocatedUniformLocation, foundValue);
                kzsErrorForward(result);
                assignValue = KZ_TRUE;
            }
            else if(foundValue->value != renderer->activeTextureUnit)
            {
                foundValue->value = renderer->activeTextureUnit;
                assignValue = KZ_TRUE;
            }

            if(assignValue)
            {
                kzsGlUniform1i(uniformLocation, (kzInt)renderer->activeTextureUnit);
            }
        }
        else
        {
            kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Renderer cannot be applied with texture if no texture uniform has been specified");
        }

        kzcRendererBindTexture(renderer, textureType, textureHandle);
    }
    else
    {
        kzsErrorThrow(KZS_ERROR_ILLEGAL_OPERATION, "Renderer cannot be applied with texture if no shader has been applied");
    }

    kzsSuccess();
}

void kzcRendererDetachTextureHandle(struct KzcRenderer* renderer, kzUint textureHandle)
{
    kzUint i;

    kzsAssert(kzcIsValidPointer(renderer));

    for(i = 0; i < KZC_RENDERER_MAX_TEXTURE_UNITS; ++i)
    {
        if(renderer->currentTextureHandle[i] == textureHandle)
        {
            kzcRendererSetActiveTextureUnit(renderer, i);
            kzcRendererBindTexture(renderer, KZC_RENDERER_TEXTURE_TYPE_2D, 0);
            renderer->currentTextureHandle[i] = 0;
        }
    }
}

void kzcRendererDetachTexture(struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));

    kzcRendererBindTexture(renderer, KZC_RENDERER_TEXTURE_TYPE_2D, 0);
    renderer->currentTextureHandle[renderer->activeTextureUnit] = 0;
}

void kzcRendererBindTexture(struct KzcRenderer* renderer, enum KzcRendererTextureType textureType, kzUint textureHandle)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(renderer->currentTextureHandle[renderer->activeTextureUnit] != textureHandle)
    {
        kzUint textureTarget = KZS_GL_TEXTURE_2D;
        /* Increase texture switches count for debug info. */
        ++renderer->debugInfo.textureSwitches;

        switch(textureType)
        {
            case KZC_RENDERER_TEXTURE_TYPE_2D: textureTarget = KZS_GL_TEXTURE_2D; break;
            case KZC_RENDERER_TEXTURE_TYPE_CUBEMAP: textureTarget = KZS_GL_TEXTURE_CUBE_MAP; break;
            default: kzsAssertText(KZ_FALSE, "Invalid texture type applied for renderer bind texture"); break;
        }
        
        renderer->currentTextureHandle[renderer->activeTextureUnit] = textureHandle;
        kzsGlBindTexture(textureTarget, textureHandle);
    }
}

kzsError kzcRendererSetActiveShaderHandle(struct KzcRenderer* renderer, kzUint activeShaderHandle)
{
    kzsError result;
    if(renderer->activeShaderHandle != activeShaderHandle)
    {
        kzUint i;

        renderer->activeShaderHandle = activeShaderHandle;
        kzsGlUseProgram(activeShaderHandle);
        
        /* Increase shader switches count for debug info. */
        ++renderer->debugInfo.shaderSwitches;
        
        if(activeShaderHandle != 0)
        {
            kzBool uniformCacheFound = kzcHashMapGet(renderer->uniformCaches, &activeShaderHandle,
                (void**)&renderer->currentUniformCache);
            if(!uniformCacheFound)
            {
                kzU32* value;

                result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, renderer->currentUniformCache,
                    "UniformCacheEntry");
                kzsErrorForward(result);

                for(i = 0; i < KZC_RENDERER_UNIFORM_CACHE_COUNT; ++i)
                {
                    result = kzcHashMapCreate(renderer->uniformCacheMemoryManager, KZC_HASH_MAP_CONFIGURATION_INT,
                        &renderer->currentUniformCache->uniformCache[i]);
                    kzsErrorForward(result);
                }

                result = kzcHashMapCreate(renderer->uniformCacheMemoryManager, KZC_HASH_MAP_CONFIGURATION_STRING,
                    &renderer->currentUniformCache->uniformLocationCache);
                kzsErrorForward(result);

                result = kzcMemoryAllocVariable(renderer->uniformCacheMemoryManager, value,
                    "UniformCacheShaderHandleKey");
                kzsErrorForward(result);

                *value = (kzU32)activeShaderHandle;
                /* Active shader handle now contains cache of floats. */
                result = kzcHashMapPut(renderer->uniformCaches, value, renderer->currentUniformCache);
                kzsErrorForward(result);
            }
        }
    }

    kzsSuccess();
}

void kzcRendererSetActiveVertexAttributeMap(struct KzcRenderer* renderer, struct KzcHashMap* vertexAttributeMap)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->vertexAttributeMap = vertexAttributeMap;
}

struct KzcHashMap* kzcRendererGetActiveShaderAttributeMap(const struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->vertexAttributeMap;
}

void kzcRendererSetActiveFrameBuffer(struct KzcRenderer* renderer, kzUint frameBufferHandle)
{
    kzsAssert(kzcIsValidPointer(renderer));
    kzsGlBindFramebuffer(KZS_GL_FRAMEBUFFER, frameBufferHandle);
    renderer->activeFrameBufferHandle = frameBufferHandle;
    if(renderer->frameBufferCallback != KZ_NULL)
    {
        renderer->frameBufferCallback(frameBufferHandle);
    }
}

void kzcRendererSetActiveFrameBufferCallback(struct KzcRenderer* renderer, KzcRendererFrameBufferCallback callbackFunction)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->frameBufferCallback = callbackFunction;
}

kzUint kzcRendererGetActiveFrameBuffer(const struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->activeFrameBufferHandle;
}

void kzcRendererReadPixels(struct KzcRenderer* renderer, kzInt x, kzInt y, kzInt width, kzInt height,
                           enum KzcRendererColorFormat format, enum KzcRendererColorType type, void* pixels)
{
    KZ_UNUSED_PARAMETER(renderer);

    /* the only formats supported at the moment of writing. */
    kzsAssert(format == KZC_RENDERER_COLOR_FORMAT_RGB);
    kzsAssert(type == KZC_RENDERER_COLOR_TYPE_UNSIGNED_SHORT_5_6_5);

    kzsGlReadPixels(x, y, width, height, KZS_GL_RGB, KZS_GL_UNSIGNED_SHORT_5_6_5, pixels);
}

void kzcRendererBeginMaterialSetup(struct KzcRenderer* renderer)
{
    struct KzcRendererMaterialState* materialState = &renderer->materialState;
    materialState->blendApplied = KZ_FALSE;
}

void kzcRendererEndMaterialSetup(struct KzcRenderer* renderer)
{
    struct KzcRendererMaterialState* materialState = &renderer->materialState;

    if(!materialState->blendApplied)
    {
        kzcRendererSetBlendMode(renderer, KZC_RENDERER_BLEND_MODE_OPAQUE);
    }
}

void kzcRendererSetVertexArrayData(const struct KzcRenderer* renderer, kzUint vertexArrayFlag, const void* data)
{
    struct KzcHashMap* vertexAttributeMap;
    kzUint* attributeIndex;
    kzsAssert(kzcIsValidPointer(renderer));

    vertexAttributeMap = renderer->vertexAttributeMap;

    if((vertexArrayFlag & KZC_VERTEX_ARRAY_POSITION) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_POSITION, (void**)&attributeIndex))
        {
            kzsGlVertexAttribPointer(*attributeIndex, 3, KZS_GL_FLOAT, KZ_FALSE, 0, data);
        }
    }
    else if((vertexArrayFlag & KZC_VERTEX_ARRAY_NORMAL) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_NORMAL, (void**)&attributeIndex))
        {
            kzsGlVertexAttribPointer(*attributeIndex, 3, KZS_GL_FLOAT, KZ_FALSE, 0, data);
        }
    }
    else if((vertexArrayFlag & KZC_VERTEX_ARRAY_TEXTURE_COORDINATE) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_TEXTURE_COORDINATE[0], (void**)&attributeIndex))
        {
            kzsGlVertexAttribPointer(*attributeIndex, 2, KZS_GL_FLOAT, KZ_FALSE, 0, data);
        }
    }
    else if((vertexArrayFlag & KZC_VERTEX_ARRAY_COLOR) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_COLOR[0], (void**)&attributeIndex))
        {
            kzsGlVertexAttribPointer(*attributeIndex, 4, KZS_GL_FLOAT, KZ_FALSE, 0, data);
        }
    }
}

void kzcRendererBeginVertexArray(struct KzcRenderer* renderer, kzUint vertexArrayFlags)
{
    struct KzcHashMap* vertexAttributeMap;
    kzUint* attributeIndex;
    kzsAssert(kzcIsValidPointer(renderer));

    vertexAttributeMap = renderer->vertexAttributeMap;
    renderer->currentVertexArrayFlags = vertexArrayFlags;

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_POSITION) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_POSITION, (void**)&attributeIndex))
        {
            kzsGlEnableVertexAttribArray(*attributeIndex);
        }
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_NORMAL) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_NORMAL, (void**)&attributeIndex))
        {
            kzsGlEnableVertexAttribArray(*attributeIndex);
        }
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_TEXTURE_COORDINATE) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_TEXTURE_COORDINATE[0], (void**)&attributeIndex))
        {
            kzsGlEnableVertexAttribArray(*attributeIndex);
        }
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_COLOR) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_COLOR[0], (void**)&attributeIndex))
        {
            kzsGlEnableVertexAttribArray(*attributeIndex);
        }
    }
}

void kzcRendererEndVertexArray(struct KzcRenderer* renderer, enum KzcPrimitiveType primitiveType, kzUint indexCount)
{
    kzUint type;
    kzUint vertexArrayFlags;
    struct KzcHashMap* vertexAttributeMap;
    kzUint* attributeIndex;
    kzsAssert(kzcIsValidPointer(renderer));

    vertexArrayFlags = renderer->currentVertexArrayFlags;

    switch(primitiveType)
    {
        case KZC_PRIMITIVE_TYPE_LINE_LOOP: type = KZS_GL_LINE_LOOP; break;
        case KZC_PRIMITIVE_TYPE_LINES: type = KZS_GL_LINES;  break;
        case KZC_PRIMITIVE_TYPE_LINE_STRIP: type = KZS_GL_LINE_STRIP; break;
        case KZC_PRIMITIVE_TYPE_POINTS: type = KZS_GL_POINTS; break;
        case KZC_PRIMITIVE_TYPE_TRIANGLE_FAN: type = KZS_GL_TRIANGLE_FAN; break;
        case KZC_PRIMITIVE_TYPE_TRIANGLES: type = KZS_GL_TRIANGLES; break;
        case KZC_PRIMITIVE_TYPE_TRIANGLE_STRIP: type = KZS_GL_TRIANGLE_STRIP; break;
        default: type = 0; kzsAssert(KZ_FALSE); break;
    }

    kzsGlDrawArrays(type, 0, (kzInt)indexCount);

    renderer->currentVertexArrayFlags = 0;

    vertexAttributeMap = renderer->vertexAttributeMap;
    renderer->currentVertexArrayFlags = vertexArrayFlags;

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_POSITION) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_POSITION, (void**)&attributeIndex))
        {
            kzsGlDisableVertexAttribArray(*attributeIndex);
        }
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_NORMAL) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_NORMAL, (void**)&attributeIndex))
        {
            kzsGlDisableVertexAttribArray(*attributeIndex);
        }
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_TEXTURE_COORDINATE) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_TEXTURE_COORDINATE[0], (void**)&attributeIndex))
        {
            kzsGlDisableVertexAttribArray(*attributeIndex);
        }
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_COLOR) != 0)
    {
        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_COLOR[0], (void**)&attributeIndex))
        {
            kzsGlDisableVertexAttribArray(*attributeIndex);
        }
    }
}

kzsError kzcRendererPrintInfo(const struct KzcRenderer* renderer)
{
    struct KzcMemoryManager* memoryManager;
    kzString shadingLanguageVersion;
    kzString vendor;
    kzString rendererString;
    kzString version;

    kzsAssert(kzcIsValidPointer(renderer));
    memoryManager = kzcMemoryGetManager(renderer);

    shadingLanguageVersion = kzsGlGetString(KZS_GL_SHADING_LANGUAGE_VERSION);
    vendor = kzsGlGetString(KZS_GL_VENDOR);
    rendererString = kzsGlGetString(KZS_GL_RENDERER);
    version = kzsGlGetString(KZS_GL_VERSION);

    kzsLog(KZS_LOG_LEVEL_INFO, "--- Renderer info ---");
    kzsLog(KZS_LOG_LEVEL_INFO, shadingLanguageVersion);
    kzsLog(KZS_LOG_LEVEL_INFO, vendor);
    kzsLog(KZS_LOG_LEVEL_INFO, version);
    kzsLog(KZS_LOG_LEVEL_INFO, rendererString);

    {
        kzsError result;
        kzUint i;
        kzInt maxFragmentUniformCount = 0;
        kzInt maxVertexUniformCount = 0;
        kzInt maxVaryingCount = 0;
        kzInt maxTextureSize = 0;
        kzInt maxTextureUnits = 0;
        kzInt maxVertexAttributes = 0;
        kzInt maxCubemapSize = 0;
        kzBool etcSupported;
        kzInt stencilBits = 0;
        kzInt depthBits = 0;
        kzInt numBinaryFormats = 0;
        GLboolean shaderCompiler = GL_TRUE;

        kzsLog(KZS_LOG_LEVEL_INFO, "--- Feature specific info ---");

        kzsGlGetIntegerv(KZS_GL_MAX_FRAGMENT_UNIFORM_VECTORS, &maxFragmentUniformCount);
        kzsGlGetIntegerv(KZS_GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniformCount);
        kzsGlGetIntegerv(KZS_GL_MAX_VARYING_VECTORS, &maxVaryingCount);
        kzsGlGetIntegerv(KZS_GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        kzsGlGetIntegerv(KZS_GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
        kzsGlGetIntegerv(KZS_GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes);
        kzsGlGetIntegerv(KZS_GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubemapSize);
        etcSupported = kzcTextureIsCompressionFormatSupported(KZC_TEXTURE_COMPRESSION_ETC);

        kzsGlGetIntegerv(KZS_GL_STENCIL_BITS, &stencilBits);
        kzsGlGetIntegerv(KZS_GL_DEPTH_BITS, &depthBits);

        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Stencil buffer bits: %i", stencilBits);
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Depth buffer bits: %i", depthBits);
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Maximum uniform count in fragment shader: %i", maxFragmentUniformCount);
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Maximum uniform count in vertex shader: %i", maxVertexUniformCount);
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Maximum varying count: %i", maxVaryingCount);
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Maximum vertex attributes count: %i", maxVertexAttributes);
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Maximum texture size: %i", maxTextureSize);
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Maximum texture units: %i", maxTextureUnits);
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Maximum cube map texture size: %i", maxCubemapSize);
        kzsErrorForward(result);

        if(etcSupported)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "ETC Compression supported");
        }
        else
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "ETC Compression not supported");
        }

        kzsGlGetBooleanv(KZS_GL_SHADER_COMPILER, &shaderCompiler);
        if(shaderCompiler == KZS_GL_TRUE)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Shader online compiler supported");
        }
        else
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "Shader online compiler not supported");
        }
        kzsGlGetIntegerv(KZS_GL_NUM_SHADER_BINARY_FORMATS, &numBinaryFormats);
        if(numBinaryFormats != 0)
        {
            kzInt* listFormats;

            result = kzcMemoryAllocArray(memoryManager, listFormats, (kzUint)numBinaryFormats, "BinaryFormats");
            kzsErrorForward(result);

            kzsLog(KZS_LOG_LEVEL_INFO, "Found binary shader formats!");
            
            kzsGlGetIntegerv(KZS_GL_SHADER_BINARY_FORMATS, listFormats);

            for(i = 0; i < (kzUint)numBinaryFormats; ++i)
            {
                result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Binary shader format supported: %i", listFormats[i]);
                kzsErrorForward(result);
            }

            result = kzcMemoryFreeArray(listFormats);
            kzsErrorForward(result);
        }
        else
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "No binary shader formats supported");
        }
    }

    kzsSuccess();
}

kzBool kzcRendererIsExtensionSupported(const struct KzcRenderer* renderer, enum KzcRendererExtension extension)
{
    kzsAssert(kzcIsValidPointer(renderer));
    kzsAssert(extension < KZC_EXTENSION_COUNT);

    /* Required extensions are default features on ES2 */
    return renderer->supportedExtensions[extension];
}

kzsError kzcRendererDrawSolidQuad2DAbsolute(struct KzcRenderer* renderer, kzFloat x, kzFloat y, kzFloat width,
                                            kzFloat height, struct KzcColorRGBA color)
{
    kzsError result;
    kzFloat x1, y1, x2, y2;
    kzInt i;
    const kzFloat z = 0.0f;
    struct KzcMatrix4x4 projection;
    struct KzcMatrix4x4 originalProjection, originalWorld, originalCamera;

    kzsErrorTest(renderer->shaderSolid != KZ_NULL, KZC_ERROR_SHADER_UNDEFINED, "Undefined textured shader for renderer");

    x1 = (kzFloat)x;
    y1 = (kzFloat)y;
    x2 = (kzFloat)(x + width);
    y2 = (kzFloat)(y + height);

    /* Initialize vertices */
    i = 0;
    renderer->vertexArray[i] = x1; i++;
    renderer->vertexArray[i] = y1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = x2; i++;
    renderer->vertexArray[i] = y1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = x2; i++;
    renderer->vertexArray[i] = y2; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = x1; i++;
    renderer->vertexArray[i] = y1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = x2; i++;
    renderer->vertexArray[i] = y2; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = x1; i++;
    renderer->vertexArray[i] = y2; i++;
    renderer->vertexArray[i] = z;

    originalProjection = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION);
    originalWorld = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD);
    originalCamera = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA);

    result = kzcShaderApply(renderer->shaderSolid, renderer);
    kzsErrorForward(result);

    /* Render */
    kzcRendererBeginMaterialSetup(renderer);
    
    result = kzcRendererSetUniformColorRGBA(renderer, "Emissive", &color);
    kzsErrorForward(result);

    kzcRendererSetBlendMode(renderer, KZC_RENDERER_BLEND_MODE_ALPHA);
    kzcRendererSetCullMode(renderer, KZC_RENDERER_CULL_MODE_NONE);
    kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_TEST);

    kzcRendererEndMaterialSetup(renderer);

    kzcMatrix4x4CreateOrthogonalProjection(&projection, (kzFloat)(kzInt)renderer->viewport.x, (kzFloat)(kzInt)(renderer->viewport.x + renderer->viewport.width),
        (kzFloat)(kzInt)renderer->viewport.y, (kzFloat)(kzInt)(renderer->viewport.y + renderer->viewport.height), -1.0f, 1.0f);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &projection);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &KZC_MATRIX4X4_IDENTITY);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &KZC_MATRIX4X4_IDENTITY);

    result = kzcRendererApplyTransformation(renderer);
    kzsErrorForward(result);
    kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);
    kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);

    /* Attach vertex attributes for rendering. */
    {
        struct KzcHashMap* vertexAttributeMap = renderer->vertexAttributeMap;
        kzUint* attributeIndex;

        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_POSITION, (void**)&attributeIndex))
        {
            kzsGlVertexAttribPointer(*attributeIndex, 3, KZS_GL_FLOAT, KZ_FALSE, 0, renderer->vertexArray);
            kzsGlEnableVertexAttribArray(*attributeIndex);
        }
    }

    kzsGlDrawArrays(KZS_GL_TRIANGLES, 0, 6);

    /* Detach vertex attributes. */
    {
        struct KzcHashMap* vertexAttributeMap = renderer->vertexAttributeMap;
        kzUint* attributeIndex;

        if(kzcHashMapGet(vertexAttributeMap, KZC_RENDERER_ATTRIBUTE_POSITION, (void**)&attributeIndex))
        {
            kzsGlDisableVertexAttribArray(*attributeIndex);
        }
    }

    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &originalProjection);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &originalWorld);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &originalCamera);

    kzsSuccess();
}

kzsError kzcRendererDrawTexturedQuad2DAbsolute(struct KzcRenderer* renderer, const struct KzcTexture* texture, kzFloat x, 
                                               kzFloat y, kzFloat width, kzFloat height, kzFloat u1, kzFloat v1,
                                               kzFloat u2, kzFloat v2)
{
    kzsError result;
    kzFloat x1, y1, x2, y2;
    kzInt i=0;
    const kzFloat z = 0.0f;
    struct KzcMatrix4x4 projection;
    struct KzcMatrix4x4 originalProjection, originalWorld, originalCamera;

    kzsErrorTest(renderer->shaderTextured != KZ_NULL, KZC_ERROR_SHADER_UNDEFINED, "Undefined textured shader for renderer");

    x1 = (kzFloat)x;
    y1 = (kzFloat)y;
    x2 = (kzFloat)(x + width);
    y2 = (kzFloat)(y + height);

    /* Initialize texture coordinates */
    renderer->uvArray[i] = u1; i++;
    renderer->uvArray[i] = v2; i++;
    renderer->uvArray[i] = u2; i++;
    renderer->uvArray[i] = v2; i++;
    renderer->uvArray[i] = u2; i++;
    renderer->uvArray[i] = v1; i++;
    renderer->uvArray[i] = u1; i++;
    renderer->uvArray[i] = v2; i++;
    renderer->uvArray[i] = u2; i++;
    renderer->uvArray[i] = v1; i++;
    renderer->uvArray[i] = u1; i++;
    renderer->uvArray[i] = v1; 

    /* Initialize vertices */
    i = 0;
    renderer->vertexArray[i] = x1; i++;
    renderer->vertexArray[i] = y1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = x2; i++;
    renderer->vertexArray[i] = y1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = x2; i++;
    renderer->vertexArray[i] = y2; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = x1; i++;
    renderer->vertexArray[i] = y1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = x2; i++;
    renderer->vertexArray[i] = y2; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = x1; i++;
    renderer->vertexArray[i] = y2; i++;
    renderer->vertexArray[i] = z;

    originalProjection = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION);
    originalWorld = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD);
    originalCamera = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA);

    result = kzcShaderApply(renderer->shaderTextured, renderer);
    kzsErrorForward(result);

    /* Render */
    kzcRendererBeginMaterialSetup(renderer);

    kzcRendererSetActiveTextureUnit(renderer, 0);
    kzcRendererSetUniformTexture(renderer, "kzTexture");
    result = kzcTextureApply(texture, renderer);
    kzsErrorForward(result);

    kzcRendererSetBlendMode(renderer, KZC_RENDERER_BLEND_MODE_OPAQUE);
    kzcRendererSetCullMode(renderer, KZC_RENDERER_CULL_MODE_NONE);
    kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_TEST);

    kzcRendererEndMaterialSetup(renderer);

    kzcMatrix4x4CreateOrthogonalProjection(&projection, (kzFloat)(kzInt)renderer->viewport.x,
        (kzFloat)(kzInt)(renderer->viewport.x + renderer->viewport.width), 
        (kzFloat)(kzInt)renderer->viewport.y,
        (kzFloat)(kzInt)(renderer->viewport.y + renderer->viewport.height),
        -1.0f, 1.0f);

    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &projection);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &KZC_MATRIX4X4_IDENTITY);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &KZC_MATRIX4X4_IDENTITY);
    
    result = kzcRendererApplyTransformation(renderer);
    kzsErrorForward(result);
    kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);
    kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);

    /* Attach vertex attributes for rendering. */
    kzcRendererBeginVertexArray(renderer, KZC_VERTEX_ARRAY_POSITION | KZC_VERTEX_ARRAY_TEXTURE_COORDINATE);
    kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_POSITION, renderer->vertexArray);
    kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_TEXTURE_COORDINATE, renderer->uvArray);
    kzcRendererEndVertexArray(renderer, KZC_PRIMITIVE_TYPE_TRIANGLES, 6);

    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &originalProjection);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &originalWorld);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &originalCamera);

    kzsSuccess();
}

void kzcRendererGetViewport(const struct KzcRenderer* renderer, kzFloat* out_x, kzFloat* out_y, kzFloat* out_width, kzFloat* out_height)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(out_x != KZ_NULL)
    {
        *out_x = (kzFloat)(kzInt)renderer->viewport.x;
    }
    if(out_y != KZ_NULL)
    {
        *out_y = (kzFloat)(kzInt)renderer->viewport.y;
    }
    if(out_width != KZ_NULL)
    {
        *out_width = (kzFloat)(kzInt)renderer->viewport.width;
    }
    if(out_height != KZ_NULL)
    {
        *out_height = (kzFloat)(kzInt)renderer->viewport.height;
    }
}

void kzcRendererSetDebugLoggingState(struct KzcRenderer* renderer, kzBool loggingEnabled)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->debugInfo.loggingEnabled = loggingEnabled;
}

kzUint kzcRendererGetTriangleCount(const struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->debugInfo.triangleCount;
}

kzUint kzcRendererGetShaderSwitchCount(const struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->debugInfo.shaderSwitches;
}

kzUint kzcRendererGetUniformSendCount(const struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->debugInfo.uniformSendings;
}

kzUint kzcRendererGetTextureSwitchCount(const struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->debugInfo.textureSwitches;
}

void kzcRendererSetGPUCallsEnabled(struct KzcRenderer* renderer, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->gpuCallsEnabled = enabled;
}

kzBool kzcRendererGPUCallsIsEnabled(const struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->gpuCallsEnabled;
}

#else

#include <system/kzs_types.h>

KZ_EMPTY_SOURCE_FILE;

#endif

