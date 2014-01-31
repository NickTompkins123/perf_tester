/**
* \file
* Core renderer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_renderer_es1.h"


#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1 || KZ_OPENGL_VERSION == KZ_OPENGL_NONE

#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/index_buffer/kzc_resource_index_buffer.h>
#include <core/resource_manager/index_buffer/kzc_index_buffer_type.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex_buffer.h>
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/color/kzc_color.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/string/kzc_string.h>

#include <system/display/kzs_window.h>
#include <system/kzs_opengl_extensions.h>
#include <system/debug/kzs_log.h>
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
#include <system/wrappers/kzs_opengles11.h>
#else
#include <system/wrappers/kzs_opengl_none.h>
#endif
#include <system/wrappers/kzs_string.h>


/** Structure for debugging informations. */
struct KzcRendererDebugInfo
{
    kzBool loggingEnabled;            /**< Logging enabled / disabled. */
    kzUint triangleCount;             /**< Triangle count. */
    kzUint textureSwitches;           /**< Texture switches. */
};

/** Defines light for renderer. */
struct KzcRendererLight
{
    kzBool enabled;                 /**< Light enabled. */
    enum KzcRendererLightType type; /**< Light type. */
    struct KzcColorRGBA color;      /**< Light color. */
    struct KzcVector3 position;     /**< Light position. */
    struct KzcVector3 direction;    /**< Light direciton. */
    struct KzcVector3 spotDirection;/**< Spot direction. */
    struct KzcVector3 attenuation;  /**< Light attenuation. */
    kzFloat cutoffAngle;            /**< Cutoff angle for spot light. */
    kzFloat exponent;               /**< Light exponent. */
    struct KzcMatrix4x4 modelViewMatrix; /**< Modelview matrix for light. */
};

/** Defines viewport for renderer. */
struct KzcRendererViewport
{
    kzUint x;   /**< Absolute X coordinate of view port top-left. */
    kzUint y;   /**< Absolute Y coordinate of view port top-left. */
    kzUint width;   /** Absolute width of view port. */
    kzUint height;  /** Absolute height of view port. */
};

/** Fixed function material states. */
struct KzcRendererMaterialState
{
    kzBool lightingEnabled;         /**< Is lighting enabled for material. */
    kzBool diffuseApplied;          /**< Is diffuse applied for material. */
    kzBool ambientApplied;          /**< Is ambient applied for material. */
    kzBool specularApplied;         /**< Is specular applied for material. */
    kzBool emissiveApplied;         /**< Is emissive applied for material. */
    
    kzBool lightApplied[KZC_RENDERER_MAX_LIGHTS];       /**< List of applied light indices. */
    kzBool textureApplied[KZC_RENDERER_MAX_TEXTURE_UNITS];  /**< List of applied texture indices. */

    kzBool fogApplied;          /**< Is fog applied for this material. */

    kzBool blendApplied;        /**< Is any blending mode applied for material (additive/alpha). */
};

/** Structure for renderer. Contains rendering state machine, extension handling, choosing code path etc. */
struct KzcRenderer
{
    enum KzcRendererStateMode renderState[KZC_RENDERER_MAX_STATES]; /**< Render states .*/

    struct KzcColorRGBA activeColor[KZC_RENDERER_MAX_COLORS]; /**< Active colors for renderer, e.g. current diffuse material or clear color value. */
    struct KzcMatrix4x4 activeMatrix[KZC_RENDERER_MAX_MATRICES]; /**< Active matrices for renderer, e.g. current projection or camera matrices. */
    struct KzcRendererLight activeLight[KZC_RENDERER_MAX_LIGHTS]; /**< Defines renderer light. */
    struct KzcRendererMaterialState materialState; /**< Defines material state for renderer. */
    
    kzFloat activeScalar[KZC_RENDERER_MAX_SCALARS]; /**< Active scalar that renderer contains. */

    enum KzcRendererBlendMode activeBlendMode; /**< Active blend mode for renderer. */
    enum KzcRendererFogMode activeFogMode; /**< Active fog mode for renderer. */
    enum KzcRendererCullMode activeCullMode;    /**< Active culling mode for renderer. */
    enum KzcRendererStencilFunction activeStencilFunction;  /**< Active stencil mode for renderer. */
    enum KzcRendererStencilOperation activeStencilOperationStencilFail; /**< Active stencil fail operation for renderer. */
    enum KzcRendererStencilOperation activeStencilOperationStencilPassDepthFail; /**< Active stencil pass depth fail operation for renderer. */
    enum KzcRendererStencilOperation activeStencilOperationStencilPassDepthPass; /**< Active stencil pass depth pass operation for renderer. */

    struct KzsWindow* activeWindow; /**< Active window for renderer. */
    struct KzcRendererViewport viewport; /**< Current rendering viewport. */

    kzBool supportedExtensions[KZC_EXTENSION_COUNT]; /** Booleans describing the supported extensions. Ordered as in KzcRendererExtensionES1 */

    kzInt clearStencilValue; /**< Stencil clear value for renderer. */
    kzUint clearTarget; /**< Clear target flag for renderer. */
    
    kzUint currentVertexCount; /**< Number of vertices currently bound vertex buffer contains. */
    kzUint currentIndexCount; /**< Number of indices currently bound index buffer contains. */
    void* currentIndexData; /**< Current index buffer data. */
    kzUint currentIndexPrimitiveType; /**< Type of the currently bound index buffer */

    kzFloat currentBlendIntensity;  /**< Sets current blend intensity for renderer. */

    kzUint activeTextureUnit; /**< Active texture unit for renderer. */
    kzUint currentTextureHandle[KZC_RENDERER_MAX_TEXTURE_UNITS]; /**< Current texture handle. */
    kzBool texturingEnabled[KZC_RENDERER_MAX_TEXTURE_UNITS]; /**< Is texturing enabled. */
    kzUint activeMatrixMode; /**< Active matrix mode. */
    enum KzcTextureOperation currentTextureOperation[KZC_RENDERER_MAX_TEXTURE_UNITS]; /**< Current texture operation. */
    kzUint currentBuffer[KZC_RENDERER_MAX_BUFFERS]; /**< Currently bound buffer. */

    struct KzcDynamicArray* vertexAttributeMap; /**< Vertex attribute map for renderer. */

    kzUint currentVertexArrayFlags; /**< Currently bound vertex array flags. */

    kzUint activeFrameBufferHandle; /**< Currently bound frame or 0 if none. */

    struct KzcRendererDebugInfo debugInfo; /**< Renderer debug info. */

    kzFloat lineWidth; /**< Width for lines. */

    kzFloat* vertexArray; /**< Float buffer for applying vertex position with glDrawArrays. */
    kzFloat* uvArray; /**< Float buffer for applying texture coordinates with glDrawArrays. */

    kzBool gpuCallsEnabled; /**< GPU calls enabled / disabled. */
};


/** Sets default values for renderer. */
static void kzcRendererSetDefaultValues_internal(struct KzcRenderer* renderer);
/** Initializes the list of supported extensions */
static void kzcRendererExtensionInitialize_internal(struct KzcRenderer* renderer);


kzsError kzcRendererCreate(const struct KzcMemoryManager* memoryManager, struct KzcRenderer** out_renderer)
{
    kzsError result;
    struct KzcRenderer* renderer;

    /* Allocate memory for renderer. */
    result = kzcMemoryAllocVariable(memoryManager, renderer, "RendererES1");
    kzsErrorForward(result);

    renderer->vertexAttributeMap = KZ_NULL;

    /* Set default values for renderer. */
    kzcRendererSetDefaultValues_internal(renderer);

    result = kzcMemoryAllocArray(memoryManager, renderer->vertexArray, 18, "VertexArray");
    kzsErrorForward(result);
    result = kzcMemoryAllocArray(memoryManager, renderer->uvArray, 12, "UvArray");
    kzsErrorForward(result);

    *out_renderer = renderer;
    kzsSuccess();
}

static void kzcRendererSetDefaultValues_internal(struct KzcRenderer* renderer)
{
    kzUint i;
    
    renderer->debugInfo.loggingEnabled = KZ_FALSE;
    renderer->activeWindow = KZ_NULL;
    renderer->debugInfo.triangleCount = 0;
    renderer->debugInfo.textureSwitches = 0;

    for(i = 0; i < KZC_RENDERER_MAX_COLORS; ++i)
    {
        renderer->activeColor[i] = kzcColorRGBA(0.0f, 0.0f, 0.0f, 0.0f);
    }
    for(i = 0; i < KZC_RENDERER_MAX_TEXTURE_UNITS; ++i)
    {
        kzsGlActiveTexture(KZS_GL_TEXTURE0 + i);
        kzsGlDisable(KZS_GL_TEXTURE_2D);
        kzsGlTexEnvi(KZS_GL_TEXTURE_2D,  KZS_GL_TEXTURE_ENV_MODE, KZS_GL_MODULATE);
        kzsGlMatrixMode(KZS_GL_TEXTURE);
        kzsGlLoadIdentity();
        renderer->currentTextureHandle[i] = 0;
        renderer->texturingEnabled[i] = KZ_FALSE;
        renderer->currentTextureOperation[i] = KZC_TEXTURE_OPERATION_MODULATE;
    }

    kzsGlActiveTexture(KZS_GL_TEXTURE0);
    kzsGlMatrixMode(KZS_GL_PROJECTION);
    kzsGlLoadIdentity();
    kzsGlMatrixMode(KZS_GL_MODELVIEW);
    kzsGlLoadIdentity();

    for(i = 0; i < KZC_RENDERER_MAX_MATRICES; ++i)
    {
        kzcMatrix4x4SetIdentity(&renderer->activeMatrix[i]);
    }

    for(i = 0; i < KZC_RENDERER_MAX_LIGHTS; ++i)
    {
        renderer->activeLight[i].color = kzcColorRGBA(0.0f, 0.0f, 0.0f, 0.0f);
        renderer->activeLight[i].enabled = KZ_FALSE;
        renderer->activeLight[i].type = KZC_RENDERER_LIGHT_TYPE_DIRECTIONAL;
        kzsGlDisable(KZS_GL_LIGHT0 + i);
    }

    for(i = 0; i < KZC_RENDERER_MAX_BUFFERS; ++i)
    {
        renderer->currentBuffer[i] = 0;
    }

    renderer->activeScalar[KZC_RENDERER_SCALAR_CLEAR_DEPTH] = -1.0f;
    renderer->activeScalar[KZC_RENDERER_SCALAR_SPECULAR_EXPONENT] = 0.0f;

    renderer->activeBlendMode = KZC_RENDERER_BLEND_MODE_OPAQUE;
    renderer->activeFogMode = KZC_RENDERER_FOG_MODE_NONE;
    renderer->activeCullMode = KZC_RENDERER_CULL_MODE_BACK;

    renderer->clearStencilValue = 1;

    renderer->currentIndexCount = 0;
    renderer->currentIndexData = KZ_NULL;
    renderer->currentBlendIntensity = 1.0f;

    renderer->activeTextureUnit = 0;
    renderer->activeMatrixMode = KZS_GL_MODELVIEW;

    renderer->currentVertexArrayFlags = 0;
    renderer->clearTarget = 0;
    renderer->activeFrameBufferHandle = 0;
    renderer->lineWidth = 1.0f;

    /* Cull face default. */
    kzsGlClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    kzsGlEnable(KZS_GL_CULL_FACE);
    kzsGlCullFace(KZS_GL_BACK);
    renderer->activeCullMode = KZC_RENDERER_CULL_MODE_BACK;
    renderer->renderState[KZC_RENDERER_STATE_CULL_FACE] = KZC_RENDERER_STATE_MODE_ENABLED;

    /* Depth test default. */
    renderer->renderState[KZC_RENDERER_STATE_DEPTH_TEST] = KZC_RENDERER_STATE_MODE_ENABLED;
    kzsGlEnable(KZS_GL_DEPTH_TEST);
    kzsGlDepthFunc(KZS_GL_LESS);
    renderer->renderState[KZC_RENDERER_STATE_DEPTH_WRITE] = KZC_RENDERER_STATE_MODE_ENABLED;
    kzsGlDepthMask(KZ_TRUE);
    
    renderer->renderState[KZC_RENDERER_STATE_FOG] = KZC_RENDERER_STATE_MODE_DISABLED;
    kzsGlDisable(KZS_GL_FOG);

    renderer->renderState[KZC_RENDERER_STATE_BLEND] = KZC_RENDERER_STATE_MODE_DISABLED;
    kzsGlDisable(KZS_GL_BLEND);

    renderer->renderState[KZC_RENDERER_STATE_MULTISAMPLE] = KZC_RENDERER_STATE_MODE_DISABLED;
    kzsGlDisable(KZS_GL_MULTISAMPLE);
    kzsGlDisable(KZS_GL_SAMPLE_COVERAGE);
    
    /* Stencil defaults. */
    renderer->activeStencilFunction = KZC_RENDERER_STENCIL_MODE_ALWAYS;
    renderer->activeStencilOperationStencilFail = KZC_RENDERER_STENCIL_OPERATION_KEEP;
    renderer->activeStencilOperationStencilPassDepthFail = KZC_RENDERER_STENCIL_OPERATION_KEEP;
    renderer->activeStencilOperationStencilPassDepthPass = KZC_RENDERER_STENCIL_OPERATION_KEEP;

    kzsGlActiveTexture(KZS_GL_TEXTURE0);
    kzsGlClientActiveTexture(KZS_GL_TEXTURE0);
    kzsGlDisable(KZS_GL_TEXTURE_2D);
    kzsGlDisable(KZS_GL_LIGHTING);
    kzsGlShadeModel(KZS_GL_SMOOTH);
    kzsGlEnable(KZS_GL_NORMALIZE);
    kzsGlMatrixMode(KZS_GL_MODELVIEW);
    kzsGlStencilFunc(KZS_GL_ALWAYS, 0, 0xFF);
    kzsGlStencilOp(KZS_GL_KEEP, KZS_GL_KEEP, KZS_GL_KEEP);
    
    kzsGlDepthFunc(KZS_GL_LESS);
    kzsGlDisableClientState(KZS_GL_VERTEX_ARRAY);
    kzsGlDisableClientState(KZS_GL_NORMAL_ARRAY);
    kzsGlDisableClientState(KZS_GL_TEXTURE_COORD_ARRAY);
    kzsGlDisableClientState(KZS_GL_COLOR_ARRAY);

    kzsGlLineWidth(1.0f);

    kzsGlColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    kzsGlBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, 0);
    kzsGlBindBuffer(KZS_GL_ARRAY_BUFFER, 0);

    renderer->gpuCallsEnabled = KZ_TRUE;

    kzcRendererExtensionInitialize_internal(renderer);
}

kzsError kzcRendererDelete(struct KzcRenderer* renderer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(renderer));

    result = kzcMemoryFreeArray(renderer->vertexArray);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray(renderer->uvArray);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(renderer);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcRendererResetFrame(struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));

    renderer->debugInfo.triangleCount = 0;
    renderer->debugInfo.textureSwitches = 0;
}

kzsError kzcRendererReset(struct KzcRenderer* renderer)
{
    kzUint i;

    kzsAssert(kzcIsValidPointer(renderer));

    for(i = 0; i < KZC_RENDERER_MAX_LIGHTS; ++i)
    {
        renderer->activeLight[i].enabled = KZ_FALSE;
        kzsGlDisable(KZS_GL_LIGHT0 + i);
    }
    for(i = 0; i < KZC_RENDERER_MAX_TEXTURE_UNITS; ++i)
    {
        renderer->texturingEnabled[i] = KZ_FALSE;
        kzsGlActiveTexture(KZS_GL_TEXTURE0 + i);
        kzsGlDisable(KZS_GL_TEXTURE_2D);
        kzsGlBindTexture(KZS_GL_TEXTURE_2D, 0);
        renderer->currentTextureHandle[i] = 0;
    }
    kzsGlActiveTexture(KZS_GL_TEXTURE0);
    renderer->activeTextureUnit = 0;

    kzsGlColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    kzsSuccess();
}

void kzcRendererSetActiveWindow(struct KzcRenderer* renderer, struct KzsWindow* window)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->activeWindow  = window;
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
            case KZC_RENDERER_STATE_LIGHTING:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlEnable(KZS_GL_LIGHTING);
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
            case KZC_RENDERER_STATE_FOG:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlEnable(KZS_GL_FOG);
                break;
            }
            case KZC_RENDERER_STATE_BLEND:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlEnable(KZS_GL_BLEND);
                break;
            }
            case KZC_RENDERER_STATE_MULTISAMPLE:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_ENABLED;
                kzsGlEnable(KZS_GL_SAMPLE_COVERAGE);
                kzsGlEnable(KZS_GL_MULTISAMPLE);
                kzsGlSampleCoverage(1.0f, KZ_FALSE);
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
            case KZC_RENDERER_STATE_LIGHTING:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_DISABLED;
                kzsGlDisable(KZS_GL_LIGHTING);
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
            case KZC_RENDERER_STATE_FOG:
            {
                renderer->renderState[state] = KZC_RENDERER_STATE_MODE_DISABLED;
                kzsGlDisable(KZS_GL_FOG);
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
                kzsGlDisable(KZS_GL_MULTISAMPLE);
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

void kzcRendererEnableTexturing(struct KzcRenderer* renderer, enum KzcRendererTextureType textureType)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(!renderer->texturingEnabled[renderer->activeTextureUnit])
    {
        kzUint textureTarget = KZS_GL_TEXTURE_2D;
        switch(textureType)
        {
            case KZC_RENDERER_TEXTURE_TYPE_2D: textureTarget = KZS_GL_TEXTURE_2D; break;
            case KZC_RENDERER_TEXTURE_TYPE_CUBEMAP: textureTarget = KZS_GL_TEXTURE_CUBE_MAP; break;
            default: kzsAssertText(KZ_FALSE, "Invalid texture type applied for renderer bind texture"); break;
        }
        renderer->texturingEnabled[renderer->activeTextureUnit] = KZ_TRUE;
        kzsGlEnable(textureTarget);
    }
}

void kzcRendererDisableTexturing(struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(renderer->texturingEnabled[renderer->activeTextureUnit])
    {
        renderer->texturingEnabled[renderer->activeTextureUnit] = KZ_FALSE;
        renderer->currentTextureHandle[renderer->activeTextureUnit] = 0;
        kzsGlDisable(KZS_GL_TEXTURE_2D);
    }
}

void kzcRendererSetTextureOperation(struct KzcRenderer* renderer, enum KzcTextureOperation operation)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if(renderer->currentTextureOperation[renderer->activeTextureUnit] != operation)
    {
        renderer->currentTextureOperation[renderer->activeTextureUnit] = operation;
        switch(operation)
        {
            case KZC_TEXTURE_OPERATION_MODULATE:
            {
                kzsGlTexEnvi(KZS_GL_TEXTURE_ENV, KZS_GL_TEXTURE_ENV_MODE, KZS_GL_MODULATE);
                break;
            }
            case KZC_TEXTURE_OPERATION_ADD:
            {
                kzsGlTexEnvi(KZS_GL_TEXTURE_ENV, KZS_GL_TEXTURE_ENV_MODE, KZS_GL_ADD);
                break;
            }
            case KZC_TEXTURE_OPERATION_REPLACE:
            {
                kzsGlTexEnvi(KZS_GL_TEXTURE_ENV, KZS_GL_TEXTURE_ENV_MODE, KZS_GL_REPLACE);
                break;
            }
            case KZC_TEXTURE_OPERATION_DECAL:
            {
                kzsGlTexEnvi(KZS_GL_TEXTURE_ENV, KZS_GL_TEXTURE_ENV_MODE, KZS_GL_DECAL);
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
            kzsGlColorMask(KZ_TRUE, KZ_FALSE, KZ_FALSE, KZ_FALSE); break;
        }
        case KZC_RENDERER_COLOR_WRITE_GREEN:
        {
            kzsGlColorMask(KZ_FALSE, KZ_TRUE, KZ_FALSE, KZ_FALSE); break;
        }
        case KZC_RENDERER_COLOR_WRITE_BLUE:
        {
            kzsGlColorMask(KZ_FALSE, KZ_FALSE, KZ_TRUE, KZ_FALSE); break;
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

void kzcRendererSetLightType(struct KzcRenderer* renderer, kzUint lightIndex, enum KzcRendererLightType lightType)
{
    renderer->activeLight[lightIndex].type = lightType;
}

void kzcRendererSetLightColor_private(struct KzcRenderer* renderer, kzUint lightIndex, struct KzcColorRGBA colorRGBA)
{
    renderer->activeLight[lightIndex].color = colorRGBA;
}

static void kzcRendererSetLightState_internal(struct KzcRenderer* renderer, kzUint lightIndex, kzBool enable)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(renderer->activeLight[lightIndex].enabled && !enable)
    {
        kzsGlDisable(KZS_GL_LIGHT0 + lightIndex);
    }
    else if(!renderer->activeLight[lightIndex].enabled && enable)
    {
        kzsGlEnable(KZS_GL_LIGHT0 + lightIndex);
    }
    renderer->activeLight[lightIndex].enabled = enable;
    renderer->materialState.lightApplied[lightIndex] = KZ_TRUE;
}

struct KzcColorRGBA kzcRendererGetLightColor(const struct KzcRenderer* renderer, kzUint lightIndex)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->activeLight[lightIndex].color;
}

void kzcRendererSetActiveColor(struct KzcRenderer* renderer, enum KzcRendererColor color, struct KzcColorRGBA colorRGBA)
{
    renderer->activeColor[color] = colorRGBA;
}

struct KzcColorRGBA kzcRendererGetActiveColor(const struct KzcRenderer* renderer, enum KzcRendererColor color)
{
    return renderer->activeColor[color];
}

void kzcRendererSetActiveScalar(struct KzcRenderer* renderer, enum KzcRendererScalar scalar, kzFloat value)
{
    renderer->activeScalar[scalar] = value;
}

kzFloat kzcRendererGetActiveScalar(const struct KzcRenderer* renderer, enum KzcRendererScalar scalar)
{
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
        kzUint target = 0;
        renderer->currentBuffer[buffer] = value;
        switch(buffer)
        {
            case KZC_RENDERER_BUFFER_INDEX:
            {
                target = KZS_GL_ELEMENT_ARRAY_BUFFER;
                break;
            }
            case KZC_RENDERER_BUFFER_VERTEX:
            {
                target = KZS_GL_ARRAY_BUFFER;
                break;
            }
            case KZC_RENDERER_MAX_BUFFERS:
            default:
            {
                kzsAssert(KZ_FALSE);
            }
        }

        kzsGlBindBuffer(target, value);
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

void kzcRendererSetTextureMatrix(struct KzcRenderer* renderer, const struct KzcMatrix4x4* matrix)
{
    enum KzcRendererMatrix textureMatrix;
    struct KzcMatrix4x4 activeTextureMatrix;

    if(renderer->activeTextureUnit == 0)
    {
        textureMatrix = KZC_RENDERER_MATRIX_TEXTURE0;
    }
    else if(renderer->activeTextureUnit == 1)
    {
        textureMatrix = KZC_RENDERER_MATRIX_TEXTURE1;
    }
    else if(renderer->activeTextureUnit == 2)
    {
        textureMatrix = KZC_RENDERER_MATRIX_TEXTURE2;
    }
    else if(renderer->activeTextureUnit == 3)
    {
        textureMatrix = KZC_RENDERER_MATRIX_TEXTURE3;
    }
    else
    {
        textureMatrix = KZC_RENDERER_MATRIX_TEXTURE0;
        kzsAssertText(KZ_FALSE, "Invalid texture unit for texture matrix!");
    }
    
    activeTextureMatrix = kzcRendererGetMatrix(renderer, textureMatrix);
    if(!kzcMatrix4x4IsEqual(matrix, &activeTextureMatrix))
    {
        kzcRendererSetMatrix(renderer, textureMatrix, matrix);
        kzsGlMatrixMode(KZS_GL_TEXTURE);
        kzsGlLoadMatrixf(&matrix->data[0]);
        kzsGlMatrixMode(KZS_GL_MODELVIEW);
    }
}

void kzcRendererApplyModelviewTransformation(const struct KzcRenderer* renderer, const struct KzcMatrix4x4* matrix)
{
    KZ_UNUSED_PARAMETER(renderer); /* TODO: There is no caching or anything like that in this function. */

    kzsGlMatrixMode(KZS_GL_MODELVIEW);
    kzsGlLoadMatrixf(&matrix->data[0]);
}

kzsError kzcRendererApplyTransformation(const struct KzcRenderer* renderer)
{
    struct KzcMatrix4x4 projectionMatrix, cameraMatrix, worldMatrix;
    projectionMatrix = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION);
    worldMatrix = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD);
    cameraMatrix = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA);

    kzsGlMatrixMode(KZS_GL_PROJECTION);
    kzsGlLoadMatrixf(&projectionMatrix.data[0]);

    kzsGlMatrixMode(KZS_GL_MODELVIEW);
    kzsGlLoadMatrixf(&cameraMatrix.data[0]);
    kzsGlMultMatrixf(&worldMatrix.data[0]);

    kzsSuccess();
}

void kzcRendererSetBlendMode(struct KzcRenderer* renderer, enum KzcRendererBlendMode blendMode)
{
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
                kzsGlBlendFunc(KZS_GL_SRC_ALPHA, KZS_GL_ONE);
                
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

void kzcRendererSetBlendIntensity(struct KzcRenderer* renderer, kzFloat intensity)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->currentBlendIntensity = intensity;
}

void kzcRendererSetFogMode(struct KzcRenderer* renderer, enum KzcRendererFogMode fogMode)
{
    if(fogMode != renderer->activeFogMode)
    {
        renderer->activeFogMode = fogMode;
        switch(fogMode)
        {
            case KZC_RENDERER_FOG_MODE_NONE:
            {
                kzcRendererDisableState(renderer, KZC_RENDERER_STATE_FOG);
                break;
            }
            case KZC_RENDERER_FOG_MODE_LINEAR:
            {
                kzcRendererSetState(renderer, KZC_RENDERER_STATE_FOG);
                kzsGlFogf(KZS_GL_FOG_MODE, (kzFloat)KZS_GL_LINEAR);
                break;
            }
            case KZC_RENDERER_FOG_MODE_DENSITY:
            {
                kzcRendererSetState(renderer, KZC_RENDERER_STATE_FOG);
                kzsGlFogf(KZS_GL_FOG_MODE, (kzFloat)KZS_GL_EXP);
                break;
            }
            default:
            {
                kzsLog(KZS_LOG_LEVEL_WARNING, "Unknown fog mode!");
                break;
            }
        }
    }

    renderer->materialState.fogApplied = KZ_TRUE;
}

void kzcRendererSetCullMode(struct KzcRenderer* renderer, enum KzcRendererCullMode cullMode)
{
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
    case KZC_RENDERER_STENCIL_OPERATION_INCREASE_WRAP: operation = KZS_GL_INCR; break;
    case KZC_RENDERER_STENCIL_OPERATION_DECREASE_WRAP: operation = KZS_GL_DECR; break;
    case KZC_RENDERER_STENCIL_OPERATION_INCREASE: operation = KZS_GL_INCR; break;
    case KZC_RENDERER_STENCIL_OPERATION_DECREASE: operation = KZS_GL_DECR; break;
    case KZC_RENDERER_STENCIL_OPERATION_INVERT: operation = KZS_GL_KEEP; break;
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

void kzcRendererSetGlobalAmbientColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA)
{
    struct KzcColorRGBA activeColor;
    kzsAssert(kzcIsValidPointer(renderer));
    activeColor = kzcRendererGetActiveColor(renderer, KZC_RENDERER_COLOR_GLOBAL_AMBIENT);

    if(!kzcColorRGBAIsEqual(&activeColor, &colorRGBA))
    {
        kzcRendererSetActiveColor(renderer, KZC_RENDERER_COLOR_GLOBAL_AMBIENT, colorRGBA);
        kzsGlLightModelfv(KZS_GL_LIGHT_MODEL_AMBIENT, &colorRGBA.red);
    }
}

void kzcRendererSetDiffuseColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA)
{
    struct KzcColorRGBA activeColor;
    kzsAssert(kzcIsValidPointer(renderer));
    activeColor = kzcRendererGetActiveColor(renderer, KZC_RENDERER_COLOR_DIFFUSE);

    if(!kzcColorRGBAIsEqual(&activeColor, &colorRGBA))
    {
        kzcRendererSetActiveColor(renderer, KZC_RENDERER_COLOR_DIFFUSE, colorRGBA);
        kzsGlMaterialfv(KZS_GL_FRONT_AND_BACK, KZS_GL_DIFFUSE, &colorRGBA.red);
    }
    renderer->materialState.diffuseApplied = KZ_TRUE;
    renderer->materialState.lightingEnabled = KZ_TRUE;
}

void kzcRendererSetAmbientColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA)
{
    struct KzcColorRGBA activeColor = kzcRendererGetActiveColor(renderer, KZC_RENDERER_COLOR_AMBIENT);
    if(!kzcColorRGBAIsEqual(&activeColor, &colorRGBA))
    {
        kzcRendererSetActiveColor(renderer, KZC_RENDERER_COLOR_AMBIENT, colorRGBA);
        kzsGlMaterialfv(KZS_GL_FRONT_AND_BACK, KZS_GL_AMBIENT, &colorRGBA.red);
    }
    renderer->materialState.ambientApplied = KZ_TRUE;
    renderer->materialState.lightingEnabled = KZ_TRUE;
}

void kzcRendererSetSpecular(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA, const kzFloat exponent)
{
    kzcRendererSetSpecularColor(renderer, colorRGBA);
    kzcRendererSetSpecularExponent(renderer, exponent);
}

void kzcRendererSetSpecularColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA)
{
    struct KzcColorRGBA activeColor = kzcRendererGetActiveColor(renderer, KZC_RENDERER_COLOR_SPECULAR);
    if(!kzcColorRGBAIsEqual(&activeColor, &colorRGBA))
    {
        kzcRendererSetActiveColor(renderer, KZC_RENDERER_COLOR_SPECULAR, colorRGBA);
        kzsGlMaterialfv(KZS_GL_FRONT_AND_BACK, KZS_GL_SPECULAR, &colorRGBA.red);
    }
    renderer->materialState.specularApplied = KZ_TRUE;
    renderer->materialState.lightingEnabled = KZ_TRUE;
}

void kzcRendererSetSpecularExponent(struct KzcRenderer* renderer, kzFloat exponent)
{
    kzFloat currentValue = kzcRendererGetActiveScalar(renderer, KZC_RENDERER_SCALAR_SPECULAR_EXPONENT);
    if(!kzsFloatIsEqual(currentValue, exponent))
    {
        kzcRendererSetActiveScalar(renderer, KZC_RENDERER_SCALAR_SPECULAR_EXPONENT, exponent);
        kzsGlMaterialf(KZS_GL_FRONT_AND_BACK, KZS_GL_SHININESS, kzsClampf(exponent, 0.0f, 128.0f));
    }
    renderer->materialState.specularApplied = KZ_TRUE;
    renderer->materialState.lightingEnabled = KZ_TRUE;
}

void kzcRendererSetEmissiveColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA)
{
    struct KzcColorRGBA activeColor = kzcRendererGetActiveColor(renderer, KZC_RENDERER_COLOR_EMISSIVE);
    if(!kzcColorRGBAIsEqual(&activeColor, &colorRGBA))
    {
        kzcRendererSetActiveColor(renderer, KZC_RENDERER_COLOR_EMISSIVE, colorRGBA);
        kzsGlMaterialfv(KZS_GL_FRONT_AND_BACK, KZS_GL_EMISSION, &colorRGBA.red);
    }
    kzsGlColor4f(colorRGBA.red, colorRGBA.green, colorRGBA.blue, colorRGBA.alpha);
    renderer->materialState.emissiveApplied = KZ_TRUE;
}

void kzcRendererSetLightColor(struct KzcRenderer* renderer, kzUint lightIndex, struct KzcColorRGBA colorRGBA)
{
    struct KzcColorRGBA activeColor = kzcRendererGetLightColor(renderer, lightIndex);
    if(!kzcColorRGBAIsEqual(&activeColor, &colorRGBA))
    {
        kzcRendererSetLightColor_private(renderer, lightIndex, colorRGBA);
        kzsGlLightfv(KZS_GL_LIGHT0 + lightIndex, KZS_GL_DIFFUSE, &colorRGBA.red);
        kzsGlLightfv(KZS_GL_LIGHT0 + lightIndex, KZS_GL_AMBIENT, &colorRGBA.red);
        kzsGlLightfv(KZS_GL_LIGHT0 + lightIndex, KZS_GL_SPECULAR, &colorRGBA.red);
    }
}

void kzcRendererSetFogColor(struct KzcRenderer* renderer, struct KzcColorRGBA colorRGBA)
{
    struct KzcColorRGBA activeColor = kzcRendererGetActiveColor(renderer, KZC_RENDERER_COLOR_FOG);
    if(!kzcColorRGBAIsEqual(&activeColor, &colorRGBA))
    {
        kzcRendererSetActiveColor(renderer, KZC_RENDERER_COLOR_FOG, colorRGBA);
        kzsGlFogfv(KZS_GL_FOG_COLOR, (kzFloat*)&colorRGBA.red);
    }
}

void kzcRendererSetFogStart(struct KzcRenderer* renderer, kzFloat value)
{
    kzFloat scalarValue = kzcRendererGetActiveScalar(renderer, KZC_RENDERER_SCALAR_FOG_START);
    if(!kzsFloatIsEqual(scalarValue, value))
    {
        kzcRendererSetActiveScalar(renderer, KZC_RENDERER_SCALAR_FOG_START, value);
        kzsGlFogf(KZS_GL_FOG_START, value);
    }
}

void kzcRendererSetFogEnd(struct KzcRenderer* renderer, kzFloat value)
{
    kzFloat scalarValue = kzcRendererGetActiveScalar(renderer, KZC_RENDERER_SCALAR_FOG_END);
    if(!kzsFloatIsEqual(scalarValue, value))
    {
        kzcRendererSetActiveScalar(renderer, KZC_RENDERER_SCALAR_FOG_END, value);
        kzsGlFogf(KZS_GL_FOG_END, value);
    }
}

void kzcRendererSetDirectionalLight(struct KzcRenderer* renderer, kzUint lightIndex, const struct KzcVector3* lightDirection)
{
    kzsAssert(kzcIsValidPointer(renderer));

    kzcRendererSetLightState_internal(renderer, lightIndex, KZ_TRUE);
    /* Apply directional light if either modelview matrix or direction has changed. */
    if(!kzcMatrix4x4IsEqual(&renderer->activeLight[lightIndex].modelViewMatrix, &renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA]) ||
        !kzcVector3IsEqual(&renderer->activeLight[lightIndex].direction, lightDirection))
    {
        kzFloat position[4];
        struct KzcVector3 dir = *lightDirection;

        kzcVector3Normalize(&dir);

        position[0] = -kzcVector3GetX(&dir);
        position[1] = -kzcVector3GetY(&dir);
        position[2] = -kzcVector3GetZ(&dir);
        position[3] = 0.0f;

        renderer->activeLight[lightIndex].direction = dir;
        renderer->activeLight[lightIndex].modelViewMatrix = renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA];

        kzsGlLoadMatrixf(&renderer->activeLight[lightIndex].modelViewMatrix.data[0]);

        kzsGlLightfv(KZS_GL_LIGHT0 + lightIndex, KZS_GL_POSITION, position);
    }

    kzcRendererSetLightType(renderer, lightIndex, KZC_RENDERER_LIGHT_TYPE_DIRECTIONAL);
}

void kzcRendererSetPointLight(struct KzcRenderer* renderer, kzUint lightIndex, const struct KzcVector3* lightPosition)
{
    kzsAssert(kzcIsValidPointer(renderer));

    kzcRendererSetLightState_internal(renderer, lightIndex, KZ_TRUE);
    kzsGlEnable(KZS_GL_LIGHT0 + lightIndex);
    /* Apply positional light. In practice state caching is not needed as position changes always when view changes. */
    if(!kzcMatrix4x4IsEqual(&renderer->activeLight[lightIndex].modelViewMatrix, &renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA]) ||
        !kzcVector3IsEqual(&renderer->activeLight[lightIndex].position, lightPosition))
    {
        kzFloat position[4];
        kzFloat noCutoff[1] = {180.0f};
        struct KzcMatrix4x4 positionMatrix;
        position[0] = 0.0f;
        position[1] = 0.0f;
        position[2] = 0.0f;
        position[3] = 1.0f;

        renderer->activeLight[lightIndex].position = *lightPosition;
        renderer->activeLight[lightIndex].modelViewMatrix = renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA];

        kzcMatrix4x4SetIdentity(&positionMatrix);
        kzcMatrix4x4TranslateVector3(&positionMatrix, lightPosition);

        kzsGlLoadMatrixf(&renderer->activeLight[lightIndex].modelViewMatrix.data[0]);
        kzsGlMultMatrixf(&positionMatrix.data[0]);

        kzsGlLightfv(KZS_GL_LIGHT0 + lightIndex, KZS_GL_POSITION, position);
        kzsGlLightfv(KZS_GL_LIGHT0 + lightIndex, KZS_GL_SPOT_CUTOFF, &noCutoff[0]);
    }
    
    kzcRendererSetLightType(renderer, lightIndex, KZC_RENDERER_LIGHT_TYPE_POINT);
}

void kzcRendererSetSpotLight(struct KzcRenderer* renderer, kzUint lightIndex, const struct KzcVector3* lightPosition,
                             const struct KzcVector3* lightDirection, kzFloat exponent, kzFloat cutoffAngleInDegrees)
{
    kzUint lightGLIndex = KZS_GL_LIGHT0 + lightIndex;
    if(!kzcMatrix4x4IsEqual(&renderer->activeLight[lightIndex].modelViewMatrix, &renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA])
        || !kzcVector3IsEqual(&renderer->activeLight[lightIndex].position, lightPosition)
        || !kzcVector3IsEqual(&renderer->activeLight[lightIndex].spotDirection, lightDirection))
    {
        kzsGlMatrixMode(KZS_GL_MODELVIEW);

        renderer->activeLight[lightIndex].modelViewMatrix = renderer->activeMatrix[KZC_RENDERER_MATRIX_CAMERA];

        /* Set spot light's position. */
        {
            struct KzcMatrix4x4 positionMatrix;
            kzFloat position[4];
            position[0] = 0.0f;
            position[1] = 0.0f;
            position[2] = 0.0f;
            position[3] = 1.0f;

            renderer->activeLight[lightIndex].position = *lightPosition;

            kzcMatrix4x4SetIdentity(&positionMatrix);
            kzcMatrix4x4TranslateVector3(&positionMatrix, lightPosition);

            kzsGlLoadMatrixf(&renderer->activeLight[lightIndex].modelViewMatrix.data[0]);
            kzsGlMultMatrixf(&positionMatrix.data[0]);
            
            kzsGlLightfv(lightGLIndex, KZS_GL_POSITION, position);
        }

        /* Set spot light's direction. */
        {
            kzFloat direction[4];
            direction[0] = kzcVector3GetX(lightDirection);
            direction[1] = kzcVector3GetY(lightDirection);
            direction[2] = kzcVector3GetZ(lightDirection);
            direction[3] = 1.0f;

            renderer->activeLight[lightIndex].spotDirection = *lightDirection;
            kzsGlLightfv(lightGLIndex, KZS_GL_SPOT_DIRECTION, direction);
        }
    }


    kzcRendererSetLightState_internal(renderer, lightIndex, KZ_TRUE);
    
    if(!kzsFloatIsEqual(renderer->activeLight[lightIndex].exponent, exponent))
    {
        renderer->activeLight[lightIndex].exponent = exponent;
        kzsGlLightf(lightGLIndex, KZS_GL_SPOT_EXPONENT, exponent);
    }
    if(!kzsFloatIsEqual(renderer->activeLight[lightIndex].cutoffAngle, cutoffAngleInDegrees))
    {
        renderer->activeLight[lightIndex].cutoffAngle = cutoffAngleInDegrees;
        kzsGlLightf(lightGLIndex, KZS_GL_SPOT_CUTOFF, cutoffAngleInDegrees);
    }
    
    kzcRendererSetLightType(renderer, lightIndex, KZC_RENDERER_LIGHT_TYPE_SPOT);
}

void kzcRendererSetLightAttenuation(struct KzcRenderer* renderer, kzUint lightIndex, struct KzcVector3 attenuationCoefficients)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(!kzcVector3IsEqual(&renderer->activeLight[lightIndex].attenuation, &attenuationCoefficients))
    {
        renderer->activeLight[lightIndex].attenuation = attenuationCoefficients;
        kzsGlLightf(KZS_GL_LIGHT0 + lightIndex, KZS_GL_CONSTANT_ATTENUATION, kzcVector3GetX(&attenuationCoefficients));
        kzsGlLightf(KZS_GL_LIGHT0 + lightIndex, KZS_GL_LINEAR_ATTENUATION, kzcVector3GetY(&attenuationCoefficients));
        kzsGlLightf(KZS_GL_LIGHT0 + lightIndex, KZS_GL_QUADRATIC_ATTENUATION, kzcVector3GetZ(&attenuationCoefficients));
    }
}

void kzcRendererSetActiveTextureUnit(struct KzcRenderer* renderer, kzUint textureUnit)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(textureUnit != renderer->activeTextureUnit)
    {
        renderer->activeTextureUnit = textureUnit;
        kzsGlActiveTexture(KZS_GL_TEXTURE0 + textureUnit);
    }
}

kzsError kzcRendererApplyTexture(struct KzcRenderer* renderer, kzUint textureHandle, enum KzcRendererTextureType textureType,
                                 kzUint textureWidth, kzUint textureHeight)
{
    kzsAssert(kzcIsValidPointer(renderer));    

    /* Texture width and height are not used in ES 1.1. */
    KZ_UNUSED_PARAMETER(textureWidth);
    KZ_UNUSED_PARAMETER(textureHeight);

    kzcRendererEnableTexturing(renderer, textureType);
    kzcRendererBindTexture(renderer, textureType, textureHandle);
    renderer->materialState.textureApplied[renderer->activeTextureUnit] = KZ_TRUE;

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

kzBool kzcRendererIsTexturingEnabled(const struct KzcRenderer* renderer, kzUint textureUnit)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->texturingEnabled[textureUnit];
}

void kzcRendererSetActiveFrameBuffer(struct KzcRenderer* renderer, kzUint frameBufferHandle)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(kzcRendererIsExtensionSupported(renderer, KZC_EXTENSION_FRAMEBUFFER_OBJECT))
    {
        kzsGlBindFramebuffer(KZS_GL_FRAMEBUFFER, frameBufferHandle);
    }
    
    renderer->activeFrameBufferHandle = frameBufferHandle;
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

void kzcRendererSetActiveShaderAttributeMap(struct KzcRenderer* renderer, struct KzcDynamicArray* vertexAttributeMap)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->vertexAttributeMap = vertexAttributeMap;
}

struct KzcDynamicArray* kzcRendererGetActiveShaderAttributeMap(const struct KzcRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->vertexAttributeMap;
}

void kzcRendererBeginMaterialSetup(struct KzcRenderer* renderer)
{
    kzUint i;
    struct KzcRendererMaterialState* materialState;
    kzsAssert(kzcIsValidPointer(renderer));

    materialState = &renderer->materialState;
    materialState->diffuseApplied = KZ_FALSE;
    materialState->ambientApplied = KZ_FALSE;
    materialState->specularApplied = KZ_FALSE;
    materialState->emissiveApplied = KZ_FALSE;
    materialState->lightingEnabled = KZ_FALSE;
    
    for(i = 0; i < KZC_RENDERER_MAX_TEXTURE_UNITS; ++i)
    {
        materialState->textureApplied[i] = KZ_FALSE;
    }

    for(i = 0; i < KZC_RENDERER_MAX_LIGHTS; ++i)
    {
        materialState->lightApplied[i] = KZ_FALSE;
    }

    materialState->blendApplied = KZ_FALSE;
    materialState->fogApplied = KZ_FALSE;
    kzsGlMatrixMode(KZS_GL_MODELVIEW);
}

void kzcRendererEndMaterialSetup(struct KzcRenderer* renderer)
{
    struct KzcRendererMaterialState* materialState = &renderer->materialState;
    kzUint i;
    if(!materialState->lightingEnabled)
    {
        kzcRendererDisableState(renderer, KZC_RENDERER_STATE_LIGHTING);
        if(!materialState->emissiveApplied)
        {
            kzcRendererSetEmissiveColor(renderer, KZC_COLOR_WHITE);
        }
    }
    else
    {
        kzcRendererSetState(renderer, KZC_RENDERER_STATE_LIGHTING);
        if(!materialState->ambientApplied)
        {
            kzcRendererSetAmbientColor(renderer, KZC_COLOR_BLACK_TRANSPARENT);
        }
        if(!materialState->diffuseApplied)
        {
            kzcRendererSetDiffuseColor(renderer, KZC_COLOR_BLACK);
        }
        if(!materialState->specularApplied)
        {
            kzcRendererSetSpecular(renderer, KZC_COLOR_BLACK_TRANSPARENT, 1.0f);
        }
        if(!materialState->emissiveApplied)
        {
            kzcRendererSetEmissiveColor(renderer, KZC_COLOR_BLACK_TRANSPARENT);
        }
    }

    /* Disable unused texture units. */
    {
        kzUint textureUnit = renderer->activeTextureUnit;
        for(i = 0; i < KZC_RENDERER_MAX_TEXTURE_UNITS; ++i)
        {
            if(!materialState->textureApplied[i])
            {
                kzcRendererSetActiveTextureUnit(renderer, i);
                kzcRendererDisableTexturing(renderer);
            }
        }
        kzcRendererSetActiveTextureUnit(renderer, textureUnit);
    }

    for(i = 0; i < KZC_RENDERER_MAX_LIGHTS; ++i)
    {
        if(!materialState->lightApplied[i])
        {
            kzcRendererSetLightState_internal(renderer, i, KZ_FALSE);
        }
    }

    if(!materialState->blendApplied)
    {
        kzcRendererSetBlendMode(renderer, KZC_RENDERER_BLEND_MODE_OPAQUE);
    }
    else
    {
        if(!materialState->lightingEnabled)
        {
            struct KzcColorRGBA emissiveColor = kzcRendererGetActiveColor(renderer, KZC_RENDERER_COLOR_EMISSIVE);
            emissiveColor.alpha = renderer->currentBlendIntensity;
            kzcRendererSetEmissiveColor(renderer, emissiveColor);
        }
        else
        {
            struct KzcColorRGBA diffuseColor = kzcRendererGetActiveColor(renderer, KZC_RENDERER_COLOR_DIFFUSE);
            diffuseColor.alpha = renderer->currentBlendIntensity;
            kzcRendererSetDiffuseColor(renderer, diffuseColor);
        }
    }
    if(!materialState->fogApplied)
    {
        kzcRendererSetFogMode(renderer, KZC_RENDERER_FOG_MODE_NONE);
    }
}

void kzcRendererBeginVertexArray(struct KzcRenderer* renderer, kzUint vertexArrayFlags)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->currentVertexArrayFlags = vertexArrayFlags;

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_POSITION) != 0)
    {
        kzsGlEnableClientState(KZS_GL_VERTEX_ARRAY);
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_NORMAL) != 0)
    {
        kzsGlEnableClientState(KZS_GL_NORMAL_ARRAY);
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_TEXTURE_COORDINATE) != 0)
    {
        kzsGlEnableClientState(KZS_GL_TEXTURE_COORD_ARRAY);
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_COLOR) != 0)
    {
        kzsGlEnableClientState(KZS_GL_COLOR_ARRAY);
    }
}

void kzcRendererSetVertexArrayData(/*lint -e{818}*/struct KzcRenderer* renderer, kzUint vertexArrayFlag, const void* data)
{
    kzsAssert(kzcIsValidPointer(renderer));

    if((renderer->currentVertexArrayFlags & vertexArrayFlag) != 0)
    {
        if((vertexArrayFlag & KZC_VERTEX_ARRAY_POSITION) != 0)
        {
            kzsGlVertexPointer(3, KZS_GL_FLOAT, 0, (kzFloat*)data);
        }
        else if((vertexArrayFlag & KZC_VERTEX_ARRAY_NORMAL) != 0)
        {
            kzsGlNormalPointer(KZS_GL_FLOAT, 0, (kzFloat*)data);
        }
        else if((vertexArrayFlag & KZC_VERTEX_ARRAY_COLOR) != 0)
        {
            kzsGlColorPointer(4, KZS_GL_FLOAT, 0, (kzFloat*)data);
        }
        else if((vertexArrayFlag & KZC_VERTEX_ARRAY_TEXTURE_COORDINATE) != 0)
        {
            kzsGlTexCoordPointer(2, KZS_GL_FLOAT, 0, (kzFloat*)data);
        }
    }
    else
    {
        kzsAssertText(KZ_FALSE, "Vertex array data tried to be assigned, while it is not assigned in kzcRendererBeginVertexArray!");
    }
}

void kzcRendererEndVertexArray(struct KzcRenderer* renderer, enum KzcPrimitiveType primitiveType, kzUint indexCount)
{
    kzUint type;
    kzUint vertexArrayFlags;
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
    if((vertexArrayFlags & KZC_VERTEX_ARRAY_POSITION) != 0)
    {
        kzsGlDisableClientState(KZS_GL_VERTEX_ARRAY);
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_NORMAL) != 0)
    {
        kzsGlDisableClientState(KZS_GL_NORMAL_ARRAY);
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_TEXTURE_COORDINATE) != 0)
    {
        kzsGlDisableClientState(KZS_GL_TEXTURE_COORD_ARRAY);
    }

    if((vertexArrayFlags & KZC_VERTEX_ARRAY_COLOR) != 0)
    {
        kzsGlDisableClientState(KZS_GL_COLOR_ARRAY);
    }
}

#define LOG_RENDERER_INFO(memoryManager, stringInfo, value) \
    if(value > 0)   \
{   \
    result = kzcStringFormat(memoryManager, stringInfo, &infoString, value); \
    kzsErrorForward(result);    \
    kzsLog(KZS_LOG_LEVEL_INFO, infoString); \
    result = kzcStringDelete(infoString);   \
    kzsErrorForward(result);    \
}

kzsError kzcRendererPrintInfo(const struct KzcRenderer* renderer)
{
    struct KzcMemoryManager* memoryManager;
    kzString vendor;
    kzString rendererString;
    kzString version;

    kzsAssert(kzcIsValidPointer(renderer));
    memoryManager = kzcMemoryGetManager(renderer);

    vendor = kzsGlGetString(KZS_GL_VENDOR);
    rendererString = kzsGlGetString(KZS_GL_RENDERER);
    version = kzsGlGetString(KZS_GL_VERSION);

    kzsLog(KZS_LOG_LEVEL_INFO, "--- Renderer info ---");
    kzsLog(KZS_LOG_LEVEL_INFO, vendor);
    kzsLog(KZS_LOG_LEVEL_INFO, version);
    kzsLog(KZS_LOG_LEVEL_INFO, rendererString);

    {
        kzsError result;
        kzMutableString infoString;
        kzInt maxTextureSize = 0;
        kzInt maxTextureUnits = 0;
        kzBool etcSupported;
        kzInt stencilBits = 0;
        kzInt depthBits = 0;

        kzsLog(KZS_LOG_LEVEL_INFO, "--- Feature specific info ---");

        kzsGlGetIntegerv(KZS_GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        kzsGlGetIntegerv(KZS_GL_MAX_TEXTURE_UNITS, &maxTextureUnits);
        etcSupported = kzcTextureIsCompressionFormatSupported(KZC_TEXTURE_COMPRESSION_ETC);

        kzsGlGetIntegerv(KZS_GL_STENCIL_BITS, &stencilBits);
        kzsGlGetIntegerv(KZS_GL_DEPTH_BITS, &depthBits);

        LOG_RENDERER_INFO(memoryManager, "Stencil buffer bits: %i", stencilBits);
        LOG_RENDERER_INFO(memoryManager, "Depth buffer bits: %i", depthBits);
        LOG_RENDERER_INFO(memoryManager, "Maximum texture size: %i", maxTextureSize);
        LOG_RENDERER_INFO(memoryManager, "Maximum texture units: %i", maxTextureUnits);

        if(etcSupported)
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "ETC Compression supported.");
        }
        else
        {
            kzsLog(KZS_LOG_LEVEL_INFO, "ETC Compression not supported.");
        }
    }

    kzsSuccess();
}

/* TODO: move to system */
static void kzcRendererExtensionInitialize_internal(struct KzcRenderer* renderer)
{
    kzInt i;
    kzString extensionString;
    const kzString KZC_EXTENSIONS_ES1[KZC_EXTENSION_COUNT] =
    {
        "GL_OES_framebuffer_object",
        "GL_OES_texture_cube_map"
    };

    kzsAssert(kzcIsValidPointer(renderer));

    /* Get list of the extensions */
    extensionString = kzsGlGetString(KZS_GL_EXTENSIONS);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    /* Check supported extensions and store info*/
    if(extensionString != KZ_NULL)
    { 
        for(i = 0; i < KZC_EXTENSION_COUNT; ++i)
        {
            /* TODO: Use kzcStringFindSubstring instead */
            renderer->supportedExtensions[i] = (kzsStrstr(extensionString, KZC_EXTENSIONS_ES1[i]) != KZ_NULL);

            if(!renderer->supportedExtensions[i])
            {
                kzsLog(KZS_LOG_LEVEL_WARNING, "Following extension was not supported:");
                kzsLog(KZS_LOG_LEVEL_WARNING, KZC_EXTENSIONS_ES1[i]);
            }
        }
    }
#endif

#ifndef GL_GLEXT_PROTOTYPES
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    /* Initialize extension functions for cube maps */
    if(renderer->supportedExtensions[KZC_EXTENSION_TEXTURE_CUBE_MAP])
    {
        glTexGenfOES = (PFNGLTEXGENFOESPROC)kzsGLGetProcAddress("glTexGenfOES");
        glTexGenfvOES = (PFNGLTEXGENFVOESPROC)kzsGLGetProcAddress("glTexGenfvOES");
        glTexGeniOES = (PFNGLTEXGENIOESPROC)kzsGLGetProcAddress("glTexGenfvOES");
        glTexGenivOES = (PFNGLTEXGENIVOESPROC)kzsGLGetProcAddress("glTexGenivOES");
        glTexGenxOES = (PFNGLTEXGENXOESPROC)kzsGLGetProcAddress("glTexGenxOES");
        glTexGenxvOES = (PFNGLTEXGENXVOESPROC)kzsGLGetProcAddress("glTexGenxvOES");
        glGetTexGenfvOES = (PFNGLGETTEXGENFVOESPROC)kzsGLGetProcAddress("glGetTexGenfvOES");
        glGetTexGenivOES = (PFNGLGETTEXGENIVOESPROC)kzsGLGetProcAddress("glGetTexGenivOES");
        glGetTexGenxvOES = (PFNGLGETTEXGENXVOESPROC)kzsGLGetProcAddress("glGetTexGenxvOES");

        /* Check that we actually found the values. Sometimes driver promises support but fails to deliver it. */

        if(glTexGenfOES == KZ_NULL || 
        glTexGeniOES == KZ_NULL || 
        glTexGenfOES == KZ_NULL || 
        glTexGenfvOES == KZ_NULL || 
        glTexGeniOES == KZ_NULL || 
        glTexGenivOES == KZ_NULL || 
        glTexGenxOES == KZ_NULL || 
        glTexGenxvOES == KZ_NULL || 
        glGetTexGenfvOES == KZ_NULL || 
        glGetTexGenivOES == KZ_NULL || 
        glGetTexGenxvOES == KZ_NULL)
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Extension support was promised by driver, but not all extension functions could be get.");
            kzsLog(KZS_LOG_LEVEL_WARNING, KZC_EXTENSIONS_ES1[KZC_EXTENSION_TEXTURE_CUBE_MAP]);
            renderer->supportedExtensions[KZC_EXTENSION_TEXTURE_CUBE_MAP] = KZ_FALSE;
        }
    }

    /* Initialize extension functions for frame buffer objects */
    if(renderer->supportedExtensions[KZC_EXTENSION_FRAMEBUFFER_OBJECT])
    {
        glIsRenderbufferOES = (PFNGLISRENDERBUFFEROESPROC)kzsGLGetProcAddress("glIsRenderbufferOES");
        glBindRenderbufferOES = (PFNGLBINDRENDERBUFFEROESPROC)kzsGLGetProcAddress("glBindRenderbufferOES");
        glDeleteRenderbuffersOES = (PFNGLDELETERENDERBUFFERSOESPROC)kzsGLGetProcAddress("glDeleteRenderbuffersOES");
        glGenRenderbuffersOES = (PFNGLGENRENDERBUFFERSOESPROC)kzsGLGetProcAddress("glGenRenderbuffersOES");
        glRenderbufferStorageOES = (PFNGLRENDERBUFFERSTORAGEOESPROC)kzsGLGetProcAddress("glRenderbufferStorageOES");
        glGetRenderbufferParameterivOES = (PFNGLGETRENDERBUFFERPARAMETERIVOESPROC)kzsGLGetProcAddress("glGetRenderbufferParameterivOES");
        glIsFramebufferOES = (PFNGLISFRAMEBUFFEROESPROC)kzsGLGetProcAddress("glIsFramebufferOES");
        glBindFramebufferOES = (PFNGLBINDFRAMEBUFFEROESPROC)kzsGLGetProcAddress("glBindFramebufferOES");
        glDeleteFramebuffersOES = (PFNGLDELETEFRAMEBUFFERSOESPROC)kzsGLGetProcAddress("glDeleteFramebuffersOES");
        glGenFramebuffersOES = (PFNGLGENFRAMEBUFFERSOESPROC)kzsGLGetProcAddress("glGenFramebuffersOES");
        glCheckFramebufferStatusOES = (PFNGLCHECKFRAMEBUFFERSTATUSOESPROC)kzsGLGetProcAddress("glCheckFramebufferStatusOES");
        glFramebufferRenderbufferOES = (PFNGLFRAMEBUFFERRENDERBUFFEROESPROC)kzsGLGetProcAddress("glFramebufferRenderbufferOES");
        glFramebufferTexture2DOES = (PFNGLFRAMEBUFFERTEXTURE2DOESPROC)kzsGLGetProcAddress("glFramebufferTexture2DOES");
        glGetFramebufferAttachmentParameterivOES = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC)kzsGLGetProcAddress("glGetFramebufferAttachmentParameterivOES");
        glGenerateMipmapOES = (PFNGLGENERATEMIPMAPOESPROC)kzsGLGetProcAddress("glGenerateMipmapOES");

        if(glIsRenderbufferOES == KZ_NULL || 
        glBindRenderbufferOES == KZ_NULL || 
        glDeleteRenderbuffersOES == KZ_NULL || 
        glGenRenderbuffersOES == KZ_NULL || 
        glRenderbufferStorageOES == KZ_NULL || 
        glGetRenderbufferParameterivOES == KZ_NULL || 
        glIsFramebufferOES == KZ_NULL || 
        glBindFramebufferOES == KZ_NULL || 
        glDeleteFramebuffersOES == KZ_NULL || 
        glGenFramebuffersOES == KZ_NULL || 
        glCheckFramebufferStatusOES == KZ_NULL || 
        glFramebufferRenderbufferOES == KZ_NULL || 
        glFramebufferTexture2DOES == KZ_NULL || 
        glGetFramebufferAttachmentParameterivOES == KZ_NULL || 
        glGenerateMipmapOES == KZ_NULL)
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Extension support was promised, but not all extension functions could be get.");
            kzsLog(KZS_LOG_LEVEL_WARNING, KZC_EXTENSIONS_ES1[KZC_EXTENSION_FRAMEBUFFER_OBJECT]);
            renderer->supportedExtensions[KZC_EXTENSION_FRAMEBUFFER_OBJECT] = KZ_FALSE;
        }
    }
#endif
#endif
}

kzBool kzcRendererIsExtensionSupported(const struct KzcRenderer* renderer, enum KzcRendererExtension extension)
{
    kzsAssert(kzcIsValidPointer(renderer));
    kzsAssert(extension < KZC_EXTENSION_COUNT);

    return renderer->supportedExtensions[extension];
}

kzsError kzcRendererResetLights(struct KzcRenderer* renderer)
{
    KZ_UNUSED_PARAMETER(renderer);
    kzsSuccess();
}

kzsError kzcRendererDrawTexturedQuad2DAbsolute(struct KzcRenderer* renderer, const struct KzcTexture* texture, kzFloat x, kzFloat y, kzFloat width, kzFloat height, kzFloat u1, kzFloat v1, kzFloat u2, kzFloat v2)
{
    kzsError result;
    kzFloat quadx1 = (kzFloat)x;
    kzFloat quady1 = (kzFloat)y;
    kzFloat quadx2 = (kzFloat)(x + width);
    kzFloat quady2 = (kzFloat)(y + height);
    kzInt i=0;
    const kzFloat z = 0.0f;
    struct KzcMatrix4x4 projection;
    struct KzcMatrix4x4 originalProjection, originalWorld, originalCamera;

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
    i=0;
    renderer->vertexArray[i] = quadx1; i++;
    renderer->vertexArray[i] = quady1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = quadx2; i++;
    renderer->vertexArray[i] = quady1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = quadx2; i++;
    renderer->vertexArray[i] = quady2; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = quadx1; i++;
    renderer->vertexArray[i] = quady1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = quadx2; i++;
    renderer->vertexArray[i] = quady2; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = quadx1; i++;
    renderer->vertexArray[i] = quady2; i++;
    renderer->vertexArray[i] = z; 

    originalProjection = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION);
    originalWorld = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD);
    originalCamera = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA);

    kzcRendererBeginMaterialSetup(renderer);

    kzcRendererSetActiveTextureUnit(renderer, 0);
    kzcRendererSetTextureMatrix(renderer, &KZC_MATRIX4X4_IDENTITY);
    result = kzcTextureApply(texture, renderer);
    kzsErrorForward(result);

    kzcRendererSetBlendMode(renderer, KZC_RENDERER_BLEND_MODE_ALPHA);
    kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_TEST);

    kzcRendererEndMaterialSetup(renderer);

    kzcMatrix4x4CreateOrthogonalProjection(&projection, (kzFloat)(kzInt)renderer->viewport.x,
        (kzFloat)(kzInt)(renderer->viewport.x+renderer->viewport.width), (kzFloat)(kzInt)renderer->viewport.y,
        (kzFloat)(kzInt)(renderer->viewport.y+renderer->viewport.height), -1.0f, 1.0f);

    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &projection);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &KZC_MATRIX4X4_IDENTITY);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &KZC_MATRIX4X4_IDENTITY);

    result = kzcRendererApplyTransformation(renderer);
    kzsErrorForward(result);

    kzsGlEnableClientState(KZS_GL_TEXTURE_COORD_ARRAY);
    kzsGlEnableClientState(KZS_GL_VERTEX_ARRAY);

    kzsGlTexCoordPointer(2, KZS_GL_FLOAT, 0, renderer->uvArray);
    kzsGlVertexPointer(3, KZS_GL_FLOAT, 0, renderer->vertexArray);
    kzsGlDrawArrays(KZS_GL_TRIANGLES, 0, 6);

    kzsGlDisableClientState(KZS_GL_VERTEX_ARRAY);
    kzsGlDisableClientState(KZS_GL_TEXTURE_COORD_ARRAY);

    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &originalProjection);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &originalCamera);
    kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &originalWorld);

    kzsSuccess();
}

kzsError kzcRendererDrawSolidQuad2DAbsolute(struct KzcRenderer* renderer, kzFloat x, kzFloat y, kzFloat width,
                                            kzFloat height, struct KzcColorRGBA color)
{
    kzsError result;
    kzInt i;
    kzFloat xp1, yp1, xp2, yp2;
    const kzFloat z = 0.0f;
    struct KzcMatrix4x4 projection;
    struct KzcMatrix4x4 originalProjection, originalWorld, originalCamera;

    xp1 = (kzFloat)x;
    yp1 = (kzFloat)y;
    xp2 = (kzFloat)(x + width);
    yp2 = (kzFloat)(y + height);

    /* Initialize vertices */
    i = 0;
    renderer->vertexArray[i] = xp1; i++;
    renderer->vertexArray[i] = yp1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = xp2; i++;
    renderer->vertexArray[i] = yp1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = xp2; i++;
    renderer->vertexArray[i] = yp2; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = xp1; i++;
    renderer->vertexArray[i] = yp1; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = xp2; i++;
    renderer->vertexArray[i] = yp2; i++;
    renderer->vertexArray[i] = z; i++;
    renderer->vertexArray[i] = xp1; i++;
    renderer->vertexArray[i] = yp2; i++;
    renderer->vertexArray[i] = z;

    originalProjection = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION);
    originalWorld = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD);
    originalCamera = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA);

    /* Render */
    kzcRendererBeginMaterialSetup(renderer);
    kzcRendererSetEmissiveColor(renderer, color);

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

    kzsGlEnableClientState(KZS_GL_VERTEX_ARRAY);

    kzsGlVertexPointer(3, KZS_GL_FLOAT, 0, renderer->vertexArray);
    kzsGlDrawArrays(KZS_GL_TRIANGLES, 0, 6);

    kzsGlDisableClientState(KZS_GL_VERTEX_ARRAY);

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
