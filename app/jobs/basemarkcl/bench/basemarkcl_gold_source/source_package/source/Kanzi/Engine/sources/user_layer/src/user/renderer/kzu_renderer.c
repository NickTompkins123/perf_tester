/**
* \file
* Specifies an user renderer. Handles material states.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_renderer.h"

#include <system/wrappers/kzs_opengl_base.h>
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
#include "kzu_default_shader.h"
#endif

#include "kzu_renderer_util.h"

#include <user/properties/kzu_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_matrix2x2_property.h>
#include <user/properties/kzu_matrix3x3_property.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_vector2_property.h>
#include <user/properties/kzu_vector3_property.h>
#include <user/properties/kzu_vector4_property.h>
#include <user/properties/kzu_texture_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_int_property.h>
#include <user/properties/kzu_light_property.h>
#include <user/properties/kzu_property_collection.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_property_query.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/material/kzu_material.h>
#include <user/material/kzu_material_type.h>
#include <user/renderer/kzu_shadow_map.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/bounding_volume/kzu_bounding_volume.h>
#include <user/scene_graph/kzu_light.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_mesh.h>
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
#include <user/kzu_error_codes.h>
#endif

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/renderer/kzc_renderer.h>
#include <core/util/math/kzc_matrix2x2.h>
#include <core/util/math/kzc_matrix3x3.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
#include <core/resource_manager/shader/kzc_resource_shader.h>
#endif
#include <core/resource_manager/vertex_buffer/kzc_resource_vertex_buffer.h>
#include <core/resource_manager/index_buffer/kzc_resource_index_buffer.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_stack.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/string/kzc_string.h>
#include <core/debug/kzc_log.h>

#include <system/time/kzs_tick.h>
#include <system/display/kzs_window.h>
#include <system/debug/kzs_counter.h>
#include <system/wrappers/kzs_memory.h>


/** Structure for renderer debug info. Contains statistics of rendering. */
struct KzuRendererDebugInfo
{
    kzBool loggingEnabled;                  /**< Logging enabled/disabled. */
    kzBool boundingBoxRenderingEnabled;     /**< Bounding box visualization enabled / disabled. */
    kzUint batchCount;                      /**< Current batch count. */
    kzUint lightCount;                      /**< Current light count. */
};

/** Structure for holding stereoscopic rendering state. */
struct KzuRendererStereoscopicState
{
    kzBool stereoscopicEnabled; /**< Stereoscopic rendering enabled. */
    enum KzuStereoscopicRenderingMode stereoscopicMode; /**< Current stereoscopic mode. */
    kzUint stereoscopicPass; /**< Current stereoscopic pass. */
    kzBool steroscopicFlipped; /**< Stereoscopic eyes flipped. */
    kzBool clearOnceOccurred; /**< Is clearing the right eye occurred. */
};

/** Structure for user renderer. Handles material states. */
struct KzuRenderer
{
    struct KzcMemoryManager* quickMemoryManager;
    struct KzcResourceManager* resourceManager;

    /* Dynamic variables. */
    struct KzcHashMap* lights;      /**< Lights that renderer contains. <KzuPropertyType(pointer of property type), KzcDynamicArray(list of KzuLightNode)>. */
    struct KzcDynamicArray* transformedObjectList;  /**< Transformed object list currently hold in renderer. */

    struct KzuMaterial* defaultMaterial;        /**< Default material of renderer. */
    struct KzuMaterial* errorMaterial;     /**< Solid color material of renderer. */
    struct KzuMaterial* solidColorMaterial;     /**< Solid color material of renderer. */
    struct KzuMaterial* texturedMaterial;     /**< Textured material of renderer. */

    struct KzuPropertyQuery* propertyQuery;     /**< Hierarchical property query used during rendering. */

    struct KzcRenderer* coreRenderer;           /**< Core renderer component. */

    struct KzuRendererDebugInfo debugInfo;      /**< Debug information. */

    kzFloat* vertexArrayMemory;                   /**< Float value array for rendering e.g. bounding boxes and wireframe grid. */

    struct KzuMaterial* overrideMaterial; /**< Material used to override the default materials or KZ_NULL if none. */
    KzuRendererOverrideMaterialCallback overrideMaterialCallback; /* Callback for overriding materials or KZ_NULL if none. */
    void* overrideMaterialCallbackUserData;

    struct KzcStack* overrideScreenTargetTextureStack; /**< Stack of override screen target textures. Will replace rendering to screen with rendering to texture. */
    struct KzuMaterial* currentMaterial;    /**< Current material in renderer. */

    struct KzcColorRGBA overrideClearColor; /**< Override clear color value. */
    kzBool overrideClearColorEnabled; /**< Override clear color enabled. */

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    struct KzuShadowMap* shadowMap; /**< Shadow map properties. */
#endif
    kzFloat viewportX; /**< Viewport absolute X coordinate. */
    kzFloat viewportY; /**< Viewport absolute Y coordinate. */
    kzFloat viewportWidth; /**< Viewport width in pixels. */
    kzFloat viewportHeight; /**< Viewport height in pixels. */
    kzFloat viewportAspectRatio; /**< Viewport aspect ratio. */
    kzBool viewportRelative; /**< Viewport relative / absolute. */

    struct KzuRendererStereoscopicState stereoscopicState; /**< Stereoscopic state. */
};


/** Applies property type. */
static kzsError kzuRendererApplyPropertyType_internal(const struct KzuRenderer* renderer, struct KzuPropertyType* propertyType);
/** Creates default material for renderer. */
static kzsError kzuRendererCreateDefaultMaterial_internal(struct KzuRenderer* renderer, struct KzuPropertyManager* propertyManager);
/** Creates error material for renderer. */
static kzsError kzuRendererCreateErrorMaterial_internal(struct KzuRenderer* renderer, struct KzuPropertyManager* propertyManager);
/** Creates solid color material for renderer. */
static kzsError kzuRendererCreateSolidColorMaterial_internal(struct KzuRenderer* renderer, struct KzuPropertyManager* propertyManager);
/** Creates textured material for renderer. */
static kzsError kzuRendererCreateTexturedMaterial_internal(struct KzuRenderer* renderer, struct KzuPropertyManager* propertyManager);


kzsError kzuRendererCreate(const struct KzcMemoryManager* memoryManager, kzUint quickMemorySize,
                           struct KzuPropertyManager* propertyManager, struct KzcResourceManager* resourceManager,
                           kzBool createDefaultMaterials, struct KzuRenderer** out_renderer)
{
    kzsError result;
    struct KzuRenderer* renderer;

    /* Allocate memory for renderer. */
    result = kzcMemoryAllocVariable(memoryManager, renderer, "UserRenderer");
    kzsErrorForward(result);

    /* Create quick memory manager. */
    result = kzcMemoryManagerCreateQuickManager(memoryManager, quickMemorySize, &renderer->quickMemoryManager);
    kzsErrorForward(result);

    renderer->resourceManager = resourceManager;

    renderer->debugInfo.loggingEnabled = KZ_FALSE;
    renderer->debugInfo.boundingBoxRenderingEnabled = KZ_FALSE;
    renderer->overrideMaterial = KZ_NULL;
    renderer->overrideMaterialCallback = KZ_NULL;
    renderer->overrideMaterialCallbackUserData = KZ_NULL;
    renderer->overrideClearColor = KZC_COLOR_BLACK;
    renderer->overrideClearColorEnabled = KZ_FALSE;
    renderer->currentMaterial = KZ_NULL;

    /* Creates a new core renderer. */
    result = kzcRendererCreate(memoryManager, &renderer->coreRenderer);
    kzsErrorForward(result);

    if(createDefaultMaterials)
    {
        /* Create default material. */
        result = kzuRendererCreateDefaultMaterial_internal(renderer, propertyManager);
        kzsErrorForward(result);
        /* Create error material. */
        result = kzuRendererCreateErrorMaterial_internal(renderer, propertyManager);
        kzsErrorForward(result);
        /* Create solid color material. */
        result = kzuRendererCreateSolidColorMaterial_internal(renderer, propertyManager);
        kzsErrorForward(result);
        /* Created textured material. */
        result = kzuRendererCreateTexturedMaterial_internal(renderer, propertyManager);
        kzsErrorForward(result);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        kzcRendererSetDefaultShaders(renderer->coreRenderer, kzuMaterialTypeGetShaderProgram(kzuMaterialGetMaterialType(renderer->solidColorMaterial)),
            kzuMaterialTypeGetShaderProgram(kzuMaterialGetMaterialType(renderer->texturedMaterial)));
#endif

    }
    else
    {
        renderer->solidColorMaterial = KZ_NULL;
        renderer->defaultMaterial = KZ_NULL;
        renderer->errorMaterial = KZ_NULL;
        renderer->texturedMaterial = KZ_NULL;
    }

    result = kzuPropertyQueryCreate(memoryManager, propertyManager, &renderer->propertyQuery);
    kzsErrorForward(result);

    /* Allocate memory for float values that are used in vertex arrays (wireframe grid, bounding boxes). */
    result = kzcMemoryAllocArray(memoryManager, renderer->vertexArrayMemory, KZU_RENDERER_FLOAT_BUFFER_LENGTH, "RendererFloatValues");
    kzsErrorForward(result);

    result = kzuRendererReset(renderer);
    kzsErrorForward(result);

    kzuRendererSetViewport(renderer, 0.0f, 0.0f, 1.0f, 1.0f, 1.6666f, KZ_TRUE);
    
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    result = kzuShadowMapCreate(memoryManager, &renderer->shadowMap);
    kzsErrorForward(result);
#endif

    result = kzcStackCreate(memoryManager, &renderer->overrideScreenTargetTextureStack);
    kzsErrorForward(result);

    renderer->stereoscopicState.stereoscopicEnabled = KZ_FALSE;
    renderer->stereoscopicState.stereoscopicMode = KZU_STEREOSCOPIC_RENDERING_ANAGLYPHIC_RED_CYAN;
    renderer->stereoscopicState.stereoscopicPass = 0;
    renderer->stereoscopicState.steroscopicFlipped = KZ_FALSE;
    renderer->stereoscopicState.clearOnceOccurred = KZ_FALSE;

    *out_renderer = renderer;
    kzsSuccess();
}

kzsError kzuRendererDelete(struct KzuRenderer* renderer)
{
    kzsError result;
    struct KzuMaterialType* materialType;

    kzsAssert(kzcIsValidPointer(renderer));

    result = kzuPropertyQueryDelete(renderer->propertyQuery);
    kzsErrorForward(result);


    result = kzcStackDelete(renderer->overrideScreenTargetTextureStack);
    kzsErrorForward(result);

    /* Delete default material. */
    if(renderer->defaultMaterial != KZ_NULL)
    {
        materialType = kzuMaterialGetMaterialType(renderer->defaultMaterial);
        result = kzuMaterialDelete(renderer->defaultMaterial);
        kzsErrorForward(result);
        result = kzuMaterialTypeDelete(materialType);
        kzsErrorForward(result);
    }

    if(renderer->solidColorMaterial != KZ_NULL)
    {
        materialType = kzuMaterialGetMaterialType(renderer->solidColorMaterial);
        result = kzuMaterialDelete(renderer->solidColorMaterial);
        kzsErrorForward(result);
        result = kzuMaterialTypeDelete(materialType);
        kzsErrorForward(result);
    }

    if(renderer->errorMaterial != KZ_NULL)
    {
        materialType = kzuMaterialGetMaterialType(renderer->errorMaterial);
        result = kzuMaterialDelete(renderer->errorMaterial);
        kzsErrorForward(result);
        result = kzuMaterialTypeDelete(materialType);
        kzsErrorForward(result);
    }

    if(renderer->texturedMaterial != KZ_NULL)
    {
        materialType = kzuMaterialGetMaterialType(renderer->texturedMaterial);
        result = kzuMaterialDelete(renderer->texturedMaterial);
        kzsErrorForward(result);
        result = kzuMaterialTypeDelete(materialType);
        kzsErrorForward(result);
    }

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    result = kzuShadowMapDelete(renderer->shadowMap);
    kzsErrorForward(result);
#endif    

    /* Remove allocated float value array values. */
    result = kzcMemoryFreeArray(renderer->vertexArrayMemory);
    kzsErrorForward(result);

    result = kzcRendererDelete(renderer->coreRenderer);
    kzsErrorForward(result);

    result = kzcMemoryManagerDelete(renderer->quickMemoryManager);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(renderer);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuRendererSetLogging(struct KzuRenderer* renderer, kzBool logging)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->debugInfo.loggingEnabled = logging;
}

kzsError kzuRendererReset(struct KzuRenderer* renderer)
{
    kzsError result;
    kzUint i;
    kzsAssert(kzcIsValidPointer(renderer));

    for(i = 0; i < KZC_RENDERER_MAX_TEXTURE_UNITS; ++i)
    {
        kzcRendererSetActiveTextureUnit(renderer->coreRenderer, i);
        kzcRendererDetachTexture(renderer->coreRenderer);
    }

    renderer->currentMaterial = KZ_NULL;

    result = kzcRendererReset(renderer->coreRenderer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuRendererResetPass(struct KzuRenderer* renderer)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(renderer));

    /* Create per-pass hash map for lights that are applied through pipeline. */
    result = kzcHashMapCreate(renderer->quickMemoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &renderer->lights);
    kzsErrorForward(result);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzuShadowMapSetDepthPass(renderer->shadowMap, KZ_FALSE);
#endif

    kzsSuccess();
}

kzsError kzuRendererResetFrame(struct KzuRenderer* renderer)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(renderer));

    result = kzcMemoryManagerResetQuickManager(renderer->quickMemoryManager);
    kzsErrorForward(result);
    
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    result = kzcRendererSetActiveShaderHandle(renderer->coreRenderer, 0);
    kzsErrorForward(result);
#endif

    kzcRendererResetFrame(renderer->coreRenderer);
    kzcRendererSetActiveFrameBuffer(renderer->coreRenderer, 0);

    kzcRendererSetDebugLoggingState(renderer->coreRenderer, renderer->debugInfo.loggingEnabled);
    
    renderer->debugInfo.batchCount = 0;
    renderer->debugInfo.lightCount = 0;

    renderer->currentMaterial = KZ_NULL;

    kzsSuccess();
}

kzsError kzuRendererPrintInfo(const struct KzuRenderer* renderer)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    kzsAssert(kzcIsValidPointer(renderer));

    if(renderer->debugInfo.loggingEnabled)
    {
        memoryManager = kzcMemoryGetManager(renderer);
        
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "---- Renderer Info ----");
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Batch count for frame: %u", renderer->debugInfo.batchCount);
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Light switch for frame: %u", renderer->debugInfo.lightCount);
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Triangles per frame: %u", kzcRendererGetTriangleCount(renderer->coreRenderer));
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Texture switches per frame: %u", kzcRendererGetTextureSwitchCount(renderer->coreRenderer));
        kzsErrorForward(result);
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Uniforms passed per frame: %u", kzcRendererGetUniformSendCount(renderer->coreRenderer));
        kzsErrorForward(result);
        result = kzcLog(memoryManager, KZS_LOG_LEVEL_INFO, "Shader switches per frame: %u", kzcRendererGetShaderSwitchCount(renderer->coreRenderer));
        kzsErrorForward(result);
#endif
    }

    kzsSuccess();
}

kzsError kzuRendererApplyLightProperty(const struct KzuRenderer* renderer, struct KzuLightNode* lightNode,
                                       const struct KzcMatrix4x4* worldTransformation)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(renderer));

    if(kzuLightNodeGetEnabled(lightNode))
    {
        struct KzcDynamicArray* lightNodeList;
        struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(kzuLightNodeToObjectNode(lightNode));
        struct KzuLightPropertyType* lightPropertyType = kzuLightNodeGetLightPropertyType(lightNode);
        
        if(!kzcHashMapGet(renderer->lights, lightPropertyType, (void**)&lightNodeList))
        {
            result = kzcDynamicArrayCreate(renderer->quickMemoryManager, &lightNodeList);
            kzsErrorForward(result);

            result = kzcHashMapPut(renderer->lights, lightPropertyType, lightNodeList);
            kzsErrorForward(result);
        }

        /* Assign matrix values to light properties (if needed). */
        {
            struct KzcDynamicArrayIterator it = kzuLightPropertyTypeGetPropertyTypes(lightPropertyType);
            while(kzcDynamicArrayIterate(it))
            {
                struct KzuPropertyType* childPropertyType = kzcDynamicArrayIteratorGetValue(it);

                if(kzuPropertyTypeGetDataType(childPropertyType) == KZU_PROPERTY_DATA_TYPE_VECTOR3)
                {
                    struct KzuVector3PropertyType* vectorPropertyType = kzuVector3PropertyTypeFromPropertyType(childPropertyType);
                    enum KzuVector3Type vectorType = kzuVector3PropertyTypeGetVectorType(vectorPropertyType);

                    /* Get position from matrix. */
                    if(vectorType == KZU_VECTOR3_TYPE_POSITION_FROM_MATRIX4X4)
                    {
                        result = kzuPropertyManagerSetVector3(propertyManager, lightNode, childPropertyType, kzcVector3(
                                                              worldTransformation->data[KZC_MATRIX4X4_INDEX_TRANSLATION_X],
                                                              worldTransformation->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y],
                                                              worldTransformation->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z]));
                        kzsErrorForward(result);
                    }
                    else if(vectorType == KZU_VECTOR3_TYPE_DIRECTION_FROM_MATRIX4X4)
                    {
                        struct KzcVector3 originalDirection = kzcVector3(0.0f, 0.0f, -1.0f);
                        struct KzcVector3 resultVector;
                        kzcMatrix4x4MultiplyVector3By3x3(worldTransformation, &originalDirection, &resultVector);

                        result = kzuPropertyManagerSetVector3(propertyManager, lightNode, childPropertyType, resultVector);
                        kzsErrorForward(result);
                    }
                }
            }
        }

        /* Add light to light list. */
        result = kzcDynamicArrayAdd(lightNodeList, lightNode);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

struct KzuPropertyQuery* kzuRendererGetPropertyQuery(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));

    return renderer->propertyQuery;
}

static kzsError kzuRendererApplyFloatProperty_internal(const struct KzuRenderer* renderer, struct KzcRenderer* coreRenderer,
                                                       const struct KzuPropertyType* propertyType)
{
    kzFloat value = kzuPropertyQueryGetFloat(renderer->propertyQuery, propertyType);

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    {
        if (propertyType == KZU_PROPERTY_TYPE_SPECULAR_EXPONENT)
        {
            kzcRendererSetSpecularExponent(coreRenderer, value);
        }
        else if (propertyType == KZU_PROPERTY_TYPE_FOG_START)
        {
            kzcRendererSetFogStart(coreRenderer, value);
        }
        else if (propertyType == KZU_PROPERTY_TYPE_FOG_END)
        {
            kzcRendererSetFogEnd(coreRenderer, value);
        }
        else if (propertyType == KZU_PROPERTY_TYPE_BLEND_INTENSITY)
        {
            kzcRendererSetBlendIntensity(coreRenderer, value);
        }
    }
#elif defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(propertyType);
    {
        kzsError result;

        result = kzcRendererSetUniformFloat(coreRenderer, kzuPropertyTypeGetName(propertyType), value);
        kzsErrorForward(result);
    }
#else
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(value);
    KZ_UNUSED_PARAMETER(propertyType);
    KZ_UNUSED_PARAMETER(coreRenderer);
#endif

    kzsSuccess();
}

static kzsError kzuRendererApplyColorProperty_internal(const struct KzuRenderer* renderer, struct KzcRenderer* coreRenderer, 
                                                       const struct KzuPropertyType* propertyType)
{
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    struct KzcColorRGBA value;

    value = kzuPropertyQueryGetColor(renderer->propertyQuery, propertyType);

    if (propertyType == KZU_PROPERTY_TYPE_AMBIENT)
    {
        struct KzcColorRGBA globalAmbientColor = kzuPropertyQueryGetColor(renderer->propertyQuery, KZU_PROPERTY_TYPE_GLOBAL_AMBIENT);
        kzcColorRGBAMultiply(&value, &globalAmbientColor, &value);
    }

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    if (propertyType == KZU_PROPERTY_TYPE_DIFFUSE)
    {
        kzcRendererSetDiffuseColor(coreRenderer, value);
    }
    else if (propertyType == KZU_PROPERTY_TYPE_AMBIENT)
    {
        kzcRendererSetAmbientColor(coreRenderer, value);
    }
    else if (propertyType == KZU_PROPERTY_TYPE_EMISSIVE)
    {
        kzcRendererSetEmissiveColor(coreRenderer, value);
    }
    else if (propertyType == KZU_PROPERTY_TYPE_SPECULAR_COLOR)
    {
        kzcRendererSetSpecularColor(coreRenderer, value);
    }
    else if (propertyType == KZU_PROPERTY_TYPE_FOG_COLOR)
    {
        kzcRendererSetFogColor(coreRenderer, value);
    }
#elif defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    {
        kzsError result;
        kzString propertyName = kzuPropertyTypeGetName(propertyType);
        result = kzcRendererSetUniformColorRGBA(coreRenderer, propertyName, &value);
        kzsErrorForward(result);
    }
#endif
#else
    KZ_UNUSED_PARAMETER(propertyType);
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(coreRenderer);
#endif

    kzsSuccess();
}

static kzsError kzuRendererApplyVector2Property_internal(const struct KzuRenderer* renderer, struct KzcRenderer* coreRenderer, 
                                                         const struct KzuPropertyType* propertyType)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
    struct KzcVector2 value;

    value = kzuPropertyQueryGetVector2(renderer->propertyQuery, propertyType);

    result = kzcRendererSetUniformVec2(coreRenderer, kzuPropertyTypeGetName(propertyType), &value);
    kzsErrorForward(result);
#else
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(coreRenderer);
    KZ_UNUSED_PARAMETER(propertyType);
#endif
    kzsSuccess();
}

static kzsError kzuRendererApplyVector3Property_internal(const struct KzuRenderer* renderer, struct KzcRenderer* coreRenderer, 
                                                         const struct KzuPropertyType* propertyType)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
    struct KzcVector3 value;

    value = kzuPropertyQueryGetVector3(renderer->propertyQuery, propertyType);

    result = kzcRendererSetUniformVec3(coreRenderer, kzuPropertyTypeGetName(propertyType), &value);
    kzsErrorForward(result);
#else
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(coreRenderer);
    KZ_UNUSED_PARAMETER(propertyType);
#endif
    kzsSuccess();
}

static kzsError kzuRendererApplyVector4Property_internal(const struct KzuRenderer* renderer, struct KzcRenderer* coreRenderer, 
                                                         const struct KzuPropertyType* propertyType)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
    struct KzcVector4 value;

    value = kzuPropertyQueryGetVector4(renderer->propertyQuery, propertyType);

    result = kzcRendererSetUniformVec4(coreRenderer, kzuPropertyTypeGetName(propertyType), &value);
    kzsErrorForward(result);
#else
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(coreRenderer);
    KZ_UNUSED_PARAMETER(propertyType);
#endif
    kzsSuccess();
}

static kzsError kzuRendererApplyTextureProperty_internal(struct KzcRenderer* coreRenderer, const struct KzuRenderer* renderer,
                                                         struct KzuPropertyType* propertyType)
{
    kzsError result;
    struct KzuTexturePropertyType* texturePropertyType = (struct KzuTexturePropertyType*)(void*)propertyType;
    struct KzcTexture* value;

    value = kzuPropertyQueryGetVoid(renderer->propertyQuery, propertyType);

    kzcRendererSetActiveTextureUnit(coreRenderer, kzuTexturePropertyTypeGetTextureUnit(texturePropertyType));

#if !defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    {
        struct KzuMatrix4x4PropertyType* matrixPropertyType = kzuTexturePropertyTypeGetTextureMatrix(texturePropertyType);
        if (matrixPropertyType != KZ_NULL)
        {
            struct KzuPropertyType* basePropertyType = kzuMatrix4x4PropertyTypeToPropertyType(matrixPropertyType);
            struct KzcMatrix4x4 textureMatrix = kzuPropertyQueryGetMatrix4x4(renderer->propertyQuery, basePropertyType);
            kzcRendererSetTextureMatrix(coreRenderer, &textureMatrix);
        }
        else
        {
            kzcRendererSetTextureMatrix(coreRenderer, &KZC_MATRIX4X4_IDENTITY);
        }
    }
#endif

    if (kzcTextureIsValid(value))
    {

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
        enum KzuTextureOperation operation = kzuTexturePropertyTypeGetTextureOperation(texturePropertyType);
        switch(operation)
        {
            case KZU_TEXTURE_OPERATION_MODULATE:
            {
                kzcRendererSetTextureOperation(coreRenderer, KZC_TEXTURE_OPERATION_MODULATE);
                break;
            }
            case KZU_TEXTURE_OPERATION_REPLACE:
            {
                kzcRendererSetTextureOperation(coreRenderer, KZC_TEXTURE_OPERATION_REPLACE);
                break;
            }
            case KZU_TEXTURE_OPERATION_DECAL:
            {
                kzcRendererSetTextureOperation(coreRenderer, KZC_TEXTURE_OPERATION_DECAL);
                break;
            }
            case KZU_TEXTURE_OPERATION_ADD:
            {
                kzcRendererSetTextureOperation(coreRenderer, KZC_TEXTURE_OPERATION_ADD);
                break;
            }
            case KZU_TEXTURE_OPERATION_CUSTOM:
            default:
            {
                kzsErrorThrow(KZU_ERROR_INVALID_PROPERTY, "Invalid texture operation for ES1!");
            }
        }
#elif defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        KZ_UNUSED_PARAMETER(renderer);

        kzcRendererSetUniformTexture(coreRenderer, kzuPropertyTypeGetName(propertyType));
#endif

        result = kzcTextureApply(value, coreRenderer);
        kzsErrorForward(result);
    }
    else
    {
        /** Detach texture from given unit. */
        result = kzcTextureDetach(coreRenderer);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

static kzsError kzuRendererApplyMatrix2x2Property_internal(const struct KzuRenderer* renderer, struct KzcRenderer* coreRenderer,
                                                           const struct KzuPropertyType* propertyType)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
    struct KzcMatrix2x2 value;

    value = kzuPropertyQueryGetMatrix2x2(renderer->propertyQuery, propertyType);

    result = kzcRendererSetUniformMatrix2x2(coreRenderer, kzuPropertyTypeGetName(propertyType), &value);
    kzsErrorForward(result);
#else
    KZ_UNUSED_PARAMETER(propertyType);
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(coreRenderer);
#endif

    kzsSuccess();
}

static kzsError kzuRendererApplyMatrix3x3Property_internal(const struct KzuRenderer* renderer, struct KzcRenderer* coreRenderer,
                                                           const struct KzuPropertyType* propertyType)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
    struct KzcMatrix3x3 value;

    value = kzuPropertyQueryGetMatrix3x3(renderer->propertyQuery, propertyType);

    result = kzcRendererSetUniformMatrix3x3(coreRenderer, kzuPropertyTypeGetName(propertyType), &value);
    kzsErrorForward(result);
#else
    KZ_UNUSED_PARAMETER(propertyType);
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(coreRenderer);
#endif

    kzsSuccess();
}

static kzsError kzuRendererApplyMatrix4x4Property_internal(const struct KzuRenderer* renderer, struct KzcRenderer* coreRenderer,
                                                           const struct KzuPropertyType* propertyType)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
    struct KzcMatrix4x4 value;

    value = kzuPropertyQueryGetMatrix4x4(renderer->propertyQuery, propertyType);

    result = kzcRendererSetUniformMatrix4x4(coreRenderer, kzuPropertyTypeGetName(propertyType), &value);
    kzsErrorForward(result);
#else
    KZ_UNUSED_PARAMETER(propertyType);
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(coreRenderer);
#endif

    kzsSuccess();
}

static kzsError kzuRendererApplyIntProperty_internal(const struct KzuRenderer* renderer, struct KzcRenderer* coreRenderer,
                                                     const struct KzuPropertyType* propertyType)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
#endif
    
    kzInt value = kzuPropertyQueryGetInt(renderer->propertyQuery, propertyType);

    if (propertyType == KZU_PROPERTY_TYPE_BLEND_MODE)
    {
        /*lint -e(930)*/
        kzcRendererSetBlendMode(coreRenderer, (enum KzcRendererBlendMode)value);
    }
    else
    {
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
        if (propertyType == KZU_PROPERTY_TYPE_FOG_MODE)
        {
            /*lint -e(930)*/
            kzcRendererSetFogMode(coreRenderer, (enum KzcRendererFogMode)value);
        }
#elif defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        result = kzcRendererSetUniformInteger(coreRenderer, kzuPropertyTypeGetName(propertyType), value);
        kzsErrorForward(result);
#endif
    }
    kzsSuccess();
}

static kzsError kzuRendererApplyMaterialLights_internal(struct KzuRenderer* renderer, /*lint -e{818}*/struct KzuMaterialType* materialType, 
                                                        struct KzcRenderer* coreRenderer)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
#endif
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    struct KzcHashMap* materialLights = kzuMaterialTypeGetLightPropertyTypes(materialType);
    struct KzcHashMapIterator materialLightIterator = kzcHashMapGetIterator(materialLights);

    /* Iterate through material lights. */
    while(kzcHashMapIterate(materialLightIterator))
    {
        struct KzcDynamicArray* lightNodeList;
        struct KzuLightPropertyType* lightType = (struct KzuLightPropertyType*)kzcHashMapIteratorGetKey(materialLightIterator);

        /* Get the light list of the type being processed. */
        if(kzcHashMapGet(renderer->lights, lightType, (void**)&lightNodeList))
        {
            kzUint maxLights;

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
            kzUint i;
#elif defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
            struct KzcDynamicArrayIterator lightPropertyIterator = kzuLightPropertyTypeGetPropertyTypes(lightType);
#endif

            kzUint* numLightsInMaterial = (kzUint*)kzcHashMapIteratorGetValue(materialLightIterator);
            kzsAssert(kzcIsValidPointer(numLightsInMaterial));

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
            /* Loop through light's properties that this material's light property contains. */
            while(kzcDynamicArrayIterate(lightPropertyIterator))
            {
                kzUint j;
                kzUint index = 0;
                kzUint lightArraySize;
                kzFloat* valueArray;

                struct KzuPropertyType* currentPropertyType = (struct KzuPropertyType*)kzcDynamicArrayIteratorGetValue(lightPropertyIterator);
                kzString currentPropertyTypeName = kzuPropertyTypeGetName(currentPropertyType);
                enum KzuPropertyDataType dataType = kzuPropertyTypeGetDataType(currentPropertyType);
                struct KzcDynamicArrayIterator lightIterator = kzcDynamicArrayGetIterator(lightNodeList);
                maxLights = kzsMinU(*numLightsInMaterial, kzcDynamicArrayGetSize(lightNodeList));

                lightArraySize = *numLightsInMaterial;

                /* Allocate 4 (max. vector size) times maxLights for lights. */
                result = kzcMemoryAllocArray(renderer->quickMemoryManager, valueArray, 4 * lightArraySize, "ValueArray");
                kzsErrorForward(result);

                kzsMemset(valueArray, 0, 4 * lightArraySize * sizeof(kzFloat));

                /* Loop through the scene's lights. */
                /* TODO: Proper use of iterator */
                for(j = 0; kzcDynamicArrayIterate(lightIterator) && j < maxLights; ++j)
                {
                    struct KzuLightNode* lightNode = (struct KzuLightNode*)kzcDynamicArrayIteratorGetValue(lightIterator);
                    struct KzuPropertyManager* propertyManager = kzuObjectNodeGetPropertyManager(kzuLightNodeToObjectNode(lightNode));

                    switch(dataType)
                    {
                        case KZU_PROPERTY_DATA_TYPE_VECTOR3:
                        {
                            struct KzcVector3 value = kzuPropertyManagerGetVector3Default(propertyManager, lightNode, currentPropertyType);
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = kzcVector3GetX(&value);
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = kzcVector3GetY(&value);
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = kzcVector3GetZ(&value);
                            break;
                        }
                        case KZU_PROPERTY_DATA_TYPE_VECTOR4:
                        {
                            struct KzcVector4 value = kzuPropertyManagerGetVector4Default(propertyManager, lightNode, currentPropertyType);
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = kzcVector4GetX(&value);
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = kzcVector4GetY(&value);
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = kzcVector4GetZ(&value);
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = kzcVector4GetW(&value);
                            break;
                        }
                        case KZU_PROPERTY_DATA_TYPE_COLOR:
                        {
                            struct KzcColorRGBA value = kzuPropertyManagerGetColorDefault(propertyManager, lightNode, currentPropertyType);
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = value.red;
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = value.green;
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = value.blue;
                            kzsAssert(kzcIsValidArrayIndex(valueArray, index));
                            valueArray[index++] = value.alpha;
                            break;
                        }
                        case KZU_PROPERTY_DATA_TYPE_FLOAT:
                        {
                            valueArray[index++] = kzuPropertyManagerGetFloatDefault(propertyManager, lightNode, currentPropertyType);
                            break;
                        }
                        case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
                        {
                            struct KzcMatrix2x2 resultMatrix = kzuPropertyManagerGetMatrix2x2Default(propertyManager, lightNode, currentPropertyType);
                            result = kzcRendererSetUniformMatrix2x2(coreRenderer, currentPropertyTypeName, &resultMatrix);
                            kzsErrorForward(result);

                            break;
                        }
                        case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
                        {
                            struct KzcMatrix3x3 resultMatrix = kzuPropertyManagerGetMatrix3x3Default(propertyManager, lightNode, currentPropertyType);
                            result = kzcRendererSetUniformMatrix3x3(coreRenderer, currentPropertyTypeName, &resultMatrix);
                            kzsErrorForward(result);

                            break;
                        }
                        case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
                        {
                            struct KzcMatrix4x4 resultMatrix = kzuPropertyManagerGetMatrix4x4Default(propertyManager, lightNode, currentPropertyType);
                            result = kzcRendererSetUniformMatrix4x4(coreRenderer, currentPropertyTypeName, &resultMatrix);
                            kzsErrorForward(result);

                            break;
                        }
                        case KZU_PROPERTY_DATA_TYPE_TEXTURE:
                        case KZU_PROPERTY_DATA_TYPE_BOOL:
                        case KZU_PROPERTY_DATA_TYPE_INT:
                        case KZU_PROPERTY_DATA_TYPE_LIGHT:
                        case KZU_PROPERTY_DATA_TYPE_STRING:
                        case KZU_PROPERTY_DATA_TYPE_VECTOR2:
                        case KZU_PROPERTY_DATA_TYPE_VOID:
                        {
                            /* Do nothing. */
                            break;
                        }
                        default:
                        {
                            kzsErrorThrow(KZU_ERROR_INVALID_PROPERTY, "Invalid property assigned for light!");
                        }
                    }
                }

                if(dataType == KZU_PROPERTY_DATA_TYPE_VECTOR3)
                {
                    result = kzcRendererSetUniformVecArray(coreRenderer, currentPropertyTypeName,
                        lightArraySize, 3, valueArray);
                    kzsErrorForward(result);
                }
                else if(dataType == KZU_PROPERTY_DATA_TYPE_COLOR)
                {
                    result = kzcRendererSetUniformVecArray(coreRenderer, currentPropertyTypeName,
                        lightArraySize, 4, valueArray);
                    kzsErrorForward(result);
                }
                else if(dataType == KZU_PROPERTY_DATA_TYPE_FLOAT)
                {
                    result = kzcRendererSetUniformVecArray(coreRenderer, currentPropertyTypeName,
                        lightArraySize, 1, valueArray);
                    kzsErrorForward(result);
                }

                if(renderer->debugInfo.loggingEnabled)
                {
                    ++renderer->debugInfo.lightCount;
                }
            }
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
            {
                kzUint index = 0;
                struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(lightNodeList);
                
                maxLights = kzsMinU(*numLightsInMaterial, kzcDynamicArrayGetSize(lightNodeList));                

                for(i = 0; kzcDynamicArrayIterate(it) && i < maxLights; ++i)
                {
                    struct KzuLightNode* currentLight = (struct KzuLightNode*)kzcDynamicArrayIteratorGetValue(it);
                    struct KzuLightPropertyType* lightPropertyType = kzuLightNodeGetLightPropertyType(currentLight);
                    struct KzuObjectNode* lightObjectNode = kzuLightNodeToObjectNode(currentLight);

                    if (lightPropertyType == kzuLightPropertyTypeFromPropertyType(KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT))
                    {
                        struct KzcVector3 direction = kzuObjectNodeGetVector3PropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_DIRECTION);
                        struct KzcColorRGBA color = kzuObjectNodeGetColorPropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_COLOR);
                        
                        kzcRendererSetDirectionalLight(coreRenderer, index, &direction);
                        kzcRendererSetLightColor(coreRenderer, index, color);
                    }
                    else if (lightPropertyType == kzuLightPropertyTypeFromPropertyType(KZU_PROPERTY_TYPE_POINT_LIGHT))
                    {
                        struct KzcVector3 position = kzuObjectNodeGetVector3PropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_POINT_LIGHT_POSITION);
                        struct KzcColorRGBA color = kzuObjectNodeGetColorPropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_POINT_LIGHT_COLOR);
                        struct KzcVector3 attenuation = kzuObjectNodeGetVector3PropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_POINT_LIGHT_ATTENUATION);

                        kzcRendererSetPointLight(coreRenderer, index, &position);
                        kzcRendererSetLightColor(coreRenderer, index, color);
                        kzcRendererSetLightAttenuation(coreRenderer, index, attenuation);
                    }
                    else if(lightPropertyType == kzuLightPropertyTypeFromPropertyType(KZU_PROPERTY_TYPE_SPOT_LIGHT))
                    {
                        struct KzcVector3 position = kzuObjectNodeGetVector3PropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_SPOT_LIGHT_POSITION);
                        struct KzcVector3 direction = kzuObjectNodeGetVector3PropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_SPOT_LIGHT_DIRECTION);
                        struct KzcColorRGBA color = kzuObjectNodeGetColorPropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_SPOT_LIGHT_COLOR);
                        kzFloat cutoff = kzuObjectNodeGetFloatPropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_SPOT_LIGHT_CUTOFF_ANGLE);
                        kzFloat exponent = kzuObjectNodeGetFloatPropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_SPOT_LIGHT_EXPONENT);
                        struct KzcVector3 attenuation = kzuObjectNodeGetVector3PropertyDefault(lightObjectNode, KZU_PROPERTY_TYPE_SPOT_LIGHT_ATTENUATION);

                        kzcRendererSetSpotLight(coreRenderer, index, &position, &direction, exponent, cutoff);
                        kzcRendererSetLightColor(coreRenderer, index, color);
                        kzcRendererSetLightAttenuation(coreRenderer, index, attenuation);
                    }
                    else
                    {
                        kzsErrorThrow(KZU_ERROR_INVALID_PROPERTY, "Invalid light for es1 renderer!");
                    }
                    ++index;
                }
            }
#endif
        }
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        else
        {
            struct KzcDynamicArrayIterator propertyTypeIterator = kzuLightPropertyTypeGetPropertyTypes(lightType);
            kzFloat* valueArray;
            kzUint* lightPropertyCountPointer = (kzUint*)kzcHashMapIteratorGetValue(materialLightIterator);
            kzUint lightPropertyCount;

            kzsAssert(kzcIsValidPointer(lightPropertyCountPointer));

            lightPropertyCount = *lightPropertyCountPointer;

            result = kzcMemoryAllocArray(renderer->quickMemoryManager, valueArray, 4 * lightPropertyCount, "ValueArray");
            kzsErrorForward(result);

            kzsMemset(valueArray, 0, 4 * lightPropertyCount * sizeof(kzFloat));

            while(kzcDynamicArrayIterate(propertyTypeIterator))
            {
                struct KzuPropertyType* propertyType = (struct KzuPropertyType*)kzcDynamicArrayIteratorGetValue(propertyTypeIterator);
                kzString currentPropertyTypeName = kzuPropertyTypeGetName(propertyType);
                enum KzuPropertyDataType dataType = kzuPropertyTypeGetDataType(propertyType);

                if(dataType == KZU_PROPERTY_DATA_TYPE_VECTOR3)
                {
                    result = kzcRendererSetUniformVecArray(coreRenderer, currentPropertyTypeName, lightPropertyCount, 3, valueArray);
                    kzsErrorForward(result);
                }
                if(dataType == KZU_PROPERTY_DATA_TYPE_COLOR)
                {
                    result = kzcRendererSetUniformVecArray(coreRenderer, currentPropertyTypeName, lightPropertyCount, 4, valueArray);
                    kzsErrorForward(result);
                }
                if(dataType == KZU_PROPERTY_DATA_TYPE_FLOAT)
                {
                    result = kzcRendererSetUniformVecArray(coreRenderer, currentPropertyTypeName, lightPropertyCount, 1, valueArray);
                    kzsErrorForward(result);
                }
            }

            result = kzcMemoryFreeArray(valueArray);
            kzsErrorForward(result);
        }
#endif
    }
#else
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(materialType);
    KZ_UNUSED_PARAMETER(coreRenderer);
#endif
    kzsSuccess();
}

static kzsError kzuRendererApplyPropertyType_internal(const struct KzuRenderer* renderer, struct KzuPropertyType* propertyType)
{
    kzsError result;

    struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
    enum KzuPropertyDataType dataType = kzuPropertyTypeGetDataType(propertyType);

    /* Apply property according to its datatype. */
    switch(dataType)
    {
        case KZU_PROPERTY_DATA_TYPE_FLOAT:
        {
            result = kzuRendererApplyFloatProperty_internal(renderer, coreRenderer, propertyType);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_INT:
        {
            result = kzuRendererApplyIntProperty_internal(renderer, coreRenderer, propertyType);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_COLOR:
        {
            result = kzuRendererApplyColorProperty_internal(renderer, coreRenderer, propertyType);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_VECTOR2:
        {
            result = kzuRendererApplyVector2Property_internal(renderer, coreRenderer, propertyType);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_VECTOR3:
        {
            result = kzuRendererApplyVector3Property_internal(renderer, coreRenderer, propertyType);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_VECTOR4:
        {
            result = kzuRendererApplyVector4Property_internal(renderer, coreRenderer, propertyType);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_MATRIX2X2:
        {
            result = kzuRendererApplyMatrix2x2Property_internal(renderer, coreRenderer, propertyType);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_MATRIX3X3:
        {
            result = kzuRendererApplyMatrix3x3Property_internal(renderer, coreRenderer, propertyType);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_MATRIX4X4:
        {
            result = kzuRendererApplyMatrix4x4Property_internal(renderer, coreRenderer, propertyType);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_TEXTURE:
        {
            result = kzuRendererApplyTextureProperty_internal(coreRenderer, renderer, propertyType);
            kzsErrorForward(result);
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_BOOL:
        case KZU_PROPERTY_DATA_TYPE_STRING:
        case KZU_PROPERTY_DATA_TYPE_VOID:
        {
            /* Do not pass bool/int/string/void to shader. */
            break;
        }
        case KZU_PROPERTY_DATA_TYPE_LIGHT:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Light property type introduced in wrong property list");
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Unsupported property data type!");
        }
    }

    kzsSuccess();
}

kzsError kzuRendererApplyMaterial(struct KzuRenderer* renderer, struct KzuMaterial* material)
{
    kzsError result;
    struct KzcDynamicArrayIterator it;
    struct KzcRenderer* coreRenderer;
    struct KzuMaterialType* materialType;

    kzsAssert(kzcIsValidPointer(renderer));
    kzsAssert(kzcIsValidPointer(material));

    materialType = kzuMaterialGetMaterialType(material);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    {
        struct KzcShader* shader;
        shader = kzuMaterialTypeGetShaderProgram(materialType);
        result = kzcShaderApply(shader, renderer->coreRenderer);
        kzsErrorForward(result);
    }
#else
    {
        struct KzcDynamicArray* attributes;
        attributes = kzuMaterialTypeGetVertexAttributes(materialType);

        kzcRendererSetActiveShaderAttributeMap(renderer->coreRenderer, attributes);
    }
#endif

    coreRenderer = renderer->coreRenderer;

    /* TODO: Rethink. It does not correctly handle overrides on material properties. */
    /*if(renderer->currentMaterial != material)*/
    {
        renderer->currentMaterial = material;

        result = kzuPropertyQueryPushObject(renderer->propertyQuery, material);
        kzsErrorForward(result);

        kzcRendererBeginMaterialSetup(coreRenderer);

        it = kzcDynamicArrayGetIterator(kzuMaterialTypeGetPropertyTypes(materialType));
        /* Apply all material property types. */
        while (kzcDynamicArrayIterate(it))
        {
            struct KzuPropertyType* propertyType = (struct KzuPropertyType*)kzcDynamicArrayIteratorGetValue(it);
            result = kzuRendererApplyPropertyType_internal(renderer, propertyType);
            kzsErrorForward(result);
        }

        result = kzuRendererApplyMaterialLights_internal(renderer, materialType, coreRenderer);
        kzsErrorForward(result);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        result = kzuShadowMapApply(renderer->shadowMap, renderer);
        kzsErrorForward(result);

        {
            kzFloat time = ((kzFloat)(kzInt)(kzsTimeGetCurrentTimestamp() & 0xFFFFFF)) / 1000.0f;
            
            result = kzcRendererSetUniformFloat(coreRenderer, "kzTime", time);
            kzsErrorForward(result);
        }
#endif

        kzcRendererEndMaterialSetup(coreRenderer);

        result = kzuPropertyQueryPopObject(renderer->propertyQuery);
        kzsErrorForward(result);
    }

    result = kzcRendererApplyTransformation(coreRenderer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuRendererApplyCameraNode(const struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedCameraNode,
                                    kzFloat viewportX, kzFloat viewportY, kzFloat viewportWidth, kzFloat viewportHeight)
{
    struct KzcMatrix4x4 cameraProjectionMatrix;
    struct KzuCameraNode* cameraNode = kzuCameraNodeFromObjectNode(kzuTransformedObjectNodeGetObjectNode(transformedCameraNode));

    KZ_UNUSED_PARAMETER(viewportX);
    KZ_UNUSED_PARAMETER(viewportY);

    /* Get data for camera projection matrix. */
    switch (kzuCameraNodeGetProjectionType(cameraNode))
    {
        case KZU_PROJECTION_PERSPECTIVE:
        {
            kzuCameraNodeCalculateProjectionMatrix(cameraNode, &cameraProjectionMatrix);
            break;
        }

        case KZU_PROJECTION_ORTHOGRAPHIC:
        {
            switch (kzuCameraNodeGetOrthogonalCoordinateSystemType(cameraNode))
            {
                case KZU_ORTHOGONAL_COORDINATE_SYSTEM_ABSOLUTE:
                {
                    kzuCameraNodeCalculateOrthogonalProjection(cameraNode, 0.0f, viewportWidth, viewportHeight,
                        0.0f, &cameraProjectionMatrix);
                    break;
                }

                case KZU_ORTHOGONAL_COORDINATE_SYSTEM_RELATIVE:
                {
                    kzuCameraNodeCalculateOrthogonalProjection(cameraNode, -1.0f, 1.0f, 1.0f, -1.0f,
                        &cameraProjectionMatrix);
                    break;
                }

                default:
                {
                    kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid orthogonal coordinate system");
                }
            }
            break;
        }

        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid projection type");
        }
    }

    if(kzcRendererGetActiveFrameBuffer(kzuRendererGetCoreRenderer(renderer)) == 0)
    {
        switch(kzsWindowGetOrientationAngle(kzcRendererGetActiveWindow(kzuRendererGetCoreRenderer(renderer))))
        {
            case KZS_WINDOW_ORIENTATION_ANGLE_0:
            {
                break;
            }
            case KZS_WINDOW_ORIENTATION_ANGLE_180:
            {
                cameraProjectionMatrix.data[0] *= -1; cameraProjectionMatrix.data[1] *= -1;
                cameraProjectionMatrix.data[4] *= -1; cameraProjectionMatrix.data[5] *= -1;
                break;
            }
            default:
            {
                kzsAssertText(KZ_FALSE, "Invalid orientation angle given");
            }
        }
    }

    /* Get camera's projection matrix. */
    {
        struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);
        struct KzcMatrix4x4 originalMatrix = kzuTransformedObjectNodeGetMatrix(transformedCameraNode);
        struct KzcMatrix4x4 cameraViewMatrix;

        kzcMatrix4x4SetToViewCoordinates(&originalMatrix, &cameraViewMatrix);

        /* Apply projection & view matrix to renderer .*/
        kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_PROJECTION, &cameraProjectionMatrix);
        kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_CAMERA, &cameraViewMatrix);
    }

    kzsSuccess();
}

static kzsError kzuRendererCreateDefaultMaterial_internal(struct KzuRenderer* renderer, struct KzuPropertyManager* propertyManager)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzuMaterialType* defaultMaterialType;
    struct KzuMaterialTypeShaderInfo info;
    struct KzcDynamicArray* attributeList;
    kzBool dummy;
    kzString attributes[] = {
        "kzPosition"
    };

    memoryManager = kzcMemoryGetManager(renderer);

    result = kzuMaterialTypeCreate(memoryManager, &defaultMaterialType);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &attributeList);
    kzsErrorForward(result);
    result = kzcDynamicArrayAdd(attributeList, (void*)attributes[0]);
    kzsErrorForward(result);

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    kzuMaterialTypeShaderInfoSetNoShader(&info, attributeList);
#elif defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzuMaterialTypeShaderInfoSetSourceShader(&info, KZU_DEFAULT_VERTEX_SHADER_SOURCE, KZU_DEFAULT_FRAGMENT_SHADER_SOURCE, KZ_NULL, attributeList);
#else
    kzuMaterialTypeShaderInfoSetNoShader(&info, attributeList);
#endif

    result = kzuMaterialTypeSetShader(defaultMaterialType, renderer->resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, &info, &dummy);
    kzsErrorForward(result);

    result = kzcDynamicArrayDelete(attributeList);
    kzsErrorForward(result);

    result = kzuMaterialTypeAddPropertyType(defaultMaterialType, (struct KzuPropertyType*)KZU_PROPERTY_TYPE_EMISSIVE);
    kzsErrorForward(result);

    result = kzuMaterialCreate(memoryManager, propertyManager, defaultMaterialType, "DefaultMaterial", &renderer->defaultMaterial);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuRendererCreateErrorMaterial_internal(struct KzuRenderer* renderer, struct KzuPropertyManager* propertyManager)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzuMaterialType* defaultMaterialType;
    struct KzuMaterialTypeShaderInfo info;
    struct KzcDynamicArray* attributeList;
    kzBool dummy;
    kzString attributes[] = {
        "kzPosition"
    };

    memoryManager = kzcMemoryGetManager(renderer);

    result = kzuMaterialTypeCreate(memoryManager, &defaultMaterialType);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &attributeList);
    kzsErrorForward(result);
    result = kzcDynamicArrayAdd(attributeList, (void*)attributes[0]);
    kzsErrorForward(result);

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    kzuMaterialTypeShaderInfoSetNoShader(&info, attributeList);
#elif defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzuMaterialTypeShaderInfoSetSourceShader(&info, KZU_ERROR_VERTEX_SHADER_SOURCE, KZU_ERROR_FRAGMENT_SHADER_SOURCE, KZ_NULL, attributeList);
#else
    kzuMaterialTypeShaderInfoSetNoShader(&info, attributeList);
#endif

    result = kzuMaterialTypeSetShader(defaultMaterialType, renderer->resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, &info, &dummy);
    kzsErrorForward(result);

    result = kzcDynamicArrayDelete(attributeList);
    kzsErrorForward(result);

    result = kzuMaterialCreate(memoryManager, propertyManager, defaultMaterialType, "ErrorMaterial", &renderer->errorMaterial);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuRendererApplyMeshObject(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode)
{
    kzsError result;
    struct KzuObjectNode* objectNode;
    struct KzcRenderer* coreRenderer;
    struct KzcVertexBuffer* vertexBuffer;
    struct KzuMesh* mesh;

    coreRenderer = renderer->coreRenderer;

    /* Get the original object node from transformed object node. */
    objectNode = kzuTransformedObjectNodeGetObjectNode(transformedObjectNode);

    /* Get the mesh data from object node. */
    mesh = kzuMeshNodeGetMesh(kzuMeshNodeFromObjectNode(objectNode));
    vertexBuffer = kzuMeshGetVertexBuffer(mesh);

    /* Render mesh if it's visible and located in GPU. */
    if (kzcVertexBufferGetMemoryType(vertexBuffer) != KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY &&
        kzcVertexBufferGetMemoryType(vertexBuffer) != KZC_RESOURCE_MEMORY_TYPE_NONE)
    {
        {
            kzUint clusterCount;
            kzUint i;

            clusterCount = kzuMeshGetClusterCount(mesh);

            for(i = 0; i < clusterCount; ++i)
            {
                struct KzuMeshCluster* meshCluster;
                struct KzcIndexBuffer* indexBuffer;
                struct KzuMaterial* material;
                struct KzuMaterial* clusterMaterial;

                meshCluster = kzuMeshGetClusterAtIndex(mesh, i);
                indexBuffer = kzuMeshClusterGetIndexBuffer(meshCluster);
                material = kzuMeshClusterGetMaterial(meshCluster);
                clusterMaterial = material;

                /* Get override material if any. */         
                if(renderer->overrideMaterial != KZ_NULL)
                {
                    material = renderer->overrideMaterial;
                }
                /* Call override material callback if any. */
                if (renderer->overrideMaterialCallback != KZ_NULL)
                {
                    struct KzuMaterial* overrideMaterial;
                    
                    result = renderer->overrideMaterialCallback(transformedObjectNode, clusterMaterial, i,
                                                                renderer->overrideMaterialCallbackUserData, &overrideMaterial);
                    kzsErrorForward(result);

                    material = overrideMaterial;
                }

                if(!kzuMaterialIsValid(material))
                {
                    material = kzuRendererGetDefaultMaterial(renderer);

                }
                else if(!kzuMaterialTypeIsValidShader(kzuMaterialGetMaterialType(material)))
                {
                    material = kzuRendererGetErrorMaterial(renderer);
                }

                result = kzcVertexBufferSetActiveShader(vertexBuffer, kzuMaterialGetMaterialType(material), kzuMaterialTypeGetVertexAttributes(kzuMaterialGetMaterialType(material)));
                kzsErrorForward(result);

                result = kzuRendererApplyMaterial(renderer, material);
                kzsErrorForward(result);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
                /* Apply bones for skinning. */
                {
                    struct KzuMeshBone* bones = kzuMeshGetSkeletonBones(mesh);
                    if(bones != KZ_NULL)
                    {
                        kzUint j;
                        kzFloat boneData[512]; /* TODO: Remove magic number 512 */
                        kzUint boneOffset = 0;
                        kzUint boneCount = kzcArrayLength(bones);

                        kzsMemset(boneData, 0, 512 * sizeof(*boneData));

                        for(j = 0; j < boneCount; ++j)
                        {
                            struct KzuTransformedObjectNode* transformedBone = kzuTransformedObjectListFindObject(renderer->transformedObjectList, bones[j].bone);
                            struct KzcMatrix4x4 transformation = kzuTransformedObjectNodeGetMatrix(transformedBone);
                            struct KzcMatrix4x4 invBindMatrix = bones[j].invBindMatrix;
                            struct KzcMatrix4x4 bindShapeMatrix = kzuMeshGetBindShapeMatrix(mesh);
                            struct KzcMatrix4x4 boneMatrix;
                            struct KzcMatrix4x4 transMulBind;

                            kzcMatrix4x4MultiplyAffine(&invBindMatrix, &transformation, &transMulBind);
                            kzcMatrix4x4MultiplyAffine(&bindShapeMatrix, &transMulBind, &boneMatrix);

                            boneData[boneOffset++] = boneMatrix.data[0];
                            boneData[boneOffset++] = boneMatrix.data[1];
                            boneData[boneOffset++] = boneMatrix.data[2];
                            boneData[boneOffset++] = boneMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X];
                            boneData[boneOffset++] = boneMatrix.data[4];
                            boneData[boneOffset++] = boneMatrix.data[5];
                            boneData[boneOffset++] = boneMatrix.data[6];
                            boneData[boneOffset++] = boneMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y];
                            boneData[boneOffset++] = boneMatrix.data[8];
                            boneData[boneOffset++] = boneMatrix.data[9];
                            boneData[boneOffset++] = boneMatrix.data[10];
                            boneData[boneOffset++] = boneMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z];
                        }

                        result = kzcRendererSetUniformVecArray(renderer->coreRenderer, "kzMatrixPalette", boneCount * 3, 4, boneData);
                        kzsErrorForward(result);
                    }
                }
#endif
                /* Vertex morphing. */
                {
                    void* positionOverrideData;
                    void* normalOverrideData;
                    void* tangentOverrideData;

                    result = kzuMeshInterpolateMorphTargets(kzuRendererGetQuickMemoryManager(renderer), mesh, &positionOverrideData,
                        &normalOverrideData, &tangentOverrideData);
                    kzsErrorForward(result);

                    if(positionOverrideData != KZ_NULL)
                    {
                        kzcVertexBufferAddOverrideSemantic(vertexBuffer, KZC_VERTEX_ATTRIBUTE_POSITION, 3, positionOverrideData);
                    }
                    if(normalOverrideData != KZ_NULL)
                    {
                        kzcVertexBufferAddOverrideSemantic(vertexBuffer, KZC_VERTEX_ATTRIBUTE_NORMAL, 3, normalOverrideData);
                    }
                    if(tangentOverrideData != KZ_NULL)
                    {
                        kzcVertexBufferAddOverrideSemantic(vertexBuffer, KZC_VERTEX_ATTRIBUTE_TANGENT, 3, tangentOverrideData);
                    }

                    /* Attach vertex buffer. */
                    result = kzcVertexBufferAttach(vertexBuffer, coreRenderer);
                    kzsErrorForward(result);
                    /* Attach index buffer. */
                    result = kzcIndexBufferAttach(indexBuffer, coreRenderer);
                    kzsErrorForward(result);
                    /* Draw the attached buffers. */
                    kzcRendererDrawBuffers(coreRenderer);
                    /* Detach index buffer. */
                    kzcIndexBufferDetach(coreRenderer);
                    /* Detach the vertex buffer. */
                    result = kzcVertexBufferDetach(vertexBuffer, coreRenderer);
                    kzsErrorForward(result);

                    if(positionOverrideData != KZ_NULL)
                    {
                        kzcVertexBufferRemoveOverrideSemantic(vertexBuffer, KZC_VERTEX_ATTRIBUTE_POSITION);
                        result = kzcMemoryFreePointer(positionOverrideData);
                        kzsErrorForward(result);
                    }
                    if(normalOverrideData != KZ_NULL)
                    {
                        kzcVertexBufferRemoveOverrideSemantic(vertexBuffer, KZC_VERTEX_ATTRIBUTE_NORMAL);
                        result = kzcMemoryFreePointer(normalOverrideData);
                        kzsErrorForward(result);
                    }
                    if(tangentOverrideData != KZ_NULL)
                    {
                        kzcVertexBufferRemoveOverrideSemantic(vertexBuffer, KZC_VERTEX_ATTRIBUTE_TANGENT);
                        result = kzcMemoryFreePointer(tangentOverrideData);
                        kzsErrorForward(result);
                    }
                }


            }
        }

        if(kzuMeshGetSkeletonBones(mesh) != KZ_NULL)
        {
            result = kzuRendererDrawSkeleton(renderer, renderer->transformedObjectList, kzuMeshGetSkeleton(mesh));
            kzsErrorForward(result);
        }

        ++renderer->debugInfo.batchCount;
    }

    kzsSuccess();
}

void kzuRendererSetTransformedObjectList(struct KzuRenderer* renderer, struct KzcDynamicArray* transformedObjectList)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->transformedObjectList = transformedObjectList;
}

static kzsError kzuRendererCreateSolidColorMaterial_internal(struct KzuRenderer* renderer, struct KzuPropertyManager* propertyManager)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzuMaterialType* solidMaterialType;
    struct KzcDynamicArray* attributeList;
    struct KzuMaterialTypeShaderInfo info;
    kzBool dummy; /* TODO: Rename */

    memoryManager = kzcMemoryGetManager(renderer);

    result = kzuMaterialTypeCreate(memoryManager, &solidMaterialType);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &attributeList);
    kzsErrorForward(result);
    result = kzcDynamicArrayAdd(attributeList, (void*)"kzPosition");
    kzsErrorForward(result);

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    kzuMaterialTypeShaderInfoSetNoShader(&info, attributeList);
#elif defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzuMaterialTypeShaderInfoSetSourceShader(&info, KZU_SOLID_COLOR_VERTEX_SHADER_SOURCE, KZU_SOLID_COLOR_FRAGMENT_SHADER_SOURCE, KZ_NULL, attributeList);
#else
    kzuMaterialTypeShaderInfoSetNoShader(&info, attributeList);
#endif
    
    result = kzuMaterialTypeSetShader(solidMaterialType, renderer->resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, &info, &dummy); /* TODO: Comment why out_shaderValid is ignored */
    kzsErrorForward(result);

    result = kzcDynamicArrayDelete(attributeList);
    kzsErrorForward(result);
    
    result = kzuMaterialTypeAddPropertyType(solidMaterialType, (struct KzuPropertyType*)KZU_PROPERTY_TYPE_EMISSIVE);
    kzsErrorForward(result);

    result = kzuMaterialCreate(memoryManager, propertyManager, solidMaterialType, "SolidColorMaterial", &renderer->solidColorMaterial);
    kzsErrorForward(result);

    result = kzuPropertyManagerSetColor(propertyManager, renderer->solidColorMaterial, KZU_PROPERTY_TYPE_EMISSIVE, kzcColorRGBA(0.0f, 1.0f, 0.0f, 1.0f));
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuRendererCreateTexturedMaterial_internal(struct KzuRenderer* renderer, struct KzuPropertyManager* propertyManager)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;
    struct KzuMaterialType* texturedMaterialType;
    struct KzcDynamicArray* attributeList;
    struct KzuMaterialTypeShaderInfo info;
    kzBool dummy;

    memoryManager = kzcMemoryGetManager(renderer);

    result = kzuMaterialTypeCreate(memoryManager, &texturedMaterialType);
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &attributeList);
    kzsErrorForward(result);
    result = kzcDynamicArrayAdd(attributeList, (void*)"kzPosition");
    kzsErrorForward(result);
    result = kzcDynamicArrayAdd(attributeList, (void*)"kzTextureCoordinate0");
    kzsErrorForward(result);

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    kzuMaterialTypeShaderInfoSetNoShader(&info, attributeList);
#elif defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzuMaterialTypeShaderInfoSetSourceShader(&info, KZU_TEXTURED_VERTEX_SHADER_SOURCE, KZU_TEXTURED_FRAGMENT_SHADER_SOURCE, KZ_NULL, attributeList);
#else
    kzuMaterialTypeShaderInfoSetNoShader(&info, attributeList);
#endif

    result = kzuMaterialTypeSetShader(texturedMaterialType, renderer->resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, &info, &dummy);
    kzsErrorForward(result);

    result = kzcDynamicArrayDelete(attributeList);
    kzsErrorForward(result);

    {
        result = kzuMaterialTypeAddPropertyType(texturedMaterialType, (struct KzuPropertyType*)KZU_PROPERTY_TYPE_TEXTURE);
        kzsErrorForward(result);
        result = kzuMaterialTypeAddPropertyType(texturedMaterialType, (struct KzuPropertyType*)KZU_PROPERTY_TYPE_BLEND_MODE);
        kzsErrorForward(result);
        result = kzuMaterialTypeAddPropertyType(texturedMaterialType, (struct KzuPropertyType*)KZU_PROPERTY_TYPE_BLEND_INTENSITY);
        kzsErrorForward(result);

        result = kzuMaterialTypeAddPropertyType(texturedMaterialType, (struct KzuPropertyType*)KZU_PROPERTY_TYPE_COLOR_INTENSITY);
        kzsErrorForward(result);
    }

    result = kzuMaterialCreate(memoryManager, propertyManager, texturedMaterialType, "TexturedMaterial", &renderer->texturedMaterial);
    kzsErrorForward(result);

    result = kzuPropertyManagerSetVoid(propertyManager, renderer->texturedMaterial, KZU_PROPERTY_TYPE_TEXTURE, KZ_NULL);
    kzsErrorForward(result);

    result = kzuPropertyManagerSetInt(propertyManager, renderer->texturedMaterial, KZU_PROPERTY_TYPE_BLEND_MODE, (kzInt)KZC_RENDERER_BLEND_MODE_OPAQUE);/*lint !e930*/
    kzsErrorForward(result);

    result = kzuPropertyManagerSetFloat(propertyManager, renderer->texturedMaterial, KZU_PROPERTY_TYPE_BLEND_INTENSITY, 1.0f);
    kzsErrorForward(result);

    result = kzuPropertyManagerSetVector3(propertyManager, renderer->texturedMaterial, (struct KzuPropertyType*)KZU_PROPERTY_TYPE_COLOR_INTENSITY, kzcVector3(1.0f, 1.0f, 1.0f));
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuMaterial* kzuRendererGetDefaultMaterial(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->defaultMaterial;
}

struct KzuMaterial* kzuRendererGetErrorMaterial(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->errorMaterial;
}

struct KzuMaterial* kzuRendererGetSolidColorMaterial(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->solidColorMaterial;
}

struct KzuMaterial* kzuRendererGetTexturedMaterial(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->texturedMaterial;
}

struct KzcRenderer*  kzuRendererGetCoreRenderer(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->coreRenderer;
}

void kzuRendererSetBoundingBoxVisualizationEnabled(struct KzuRenderer* renderer, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->debugInfo.boundingBoxRenderingEnabled = enabled;
}

kzBool kzuRendererIsBoundingBoxVisualizationEnabled(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->debugInfo.boundingBoxRenderingEnabled;
}

kzFloat* kzuRendererGetFloatBuffer(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->vertexArrayMemory;
}

void kzuRendererSetMaterialOverride(struct KzuRenderer* renderer, struct KzuMaterial* material)
{
    kzsAssert(kzcIsValidPointer(renderer));
    kzsAssert(kzcIsValidPointer(material) || material == KZ_NULL); /* KZ_NULL is allowed. */
    renderer->overrideMaterial = material;
}

void kzuRendererSetClearColorOverride(struct KzuRenderer* renderer, const struct KzcColorRGBA* colorRGBA)
{
    if(colorRGBA != KZ_NULL)
    {
        renderer->overrideClearColorEnabled = KZ_TRUE;
        renderer->overrideClearColor = *colorRGBA;
    }
    else
    {
        renderer->overrideClearColorEnabled = KZ_FALSE;
    }
}

kzBool kzuRendererGetClearColorOverride(const struct KzuRenderer* renderer, struct KzcColorRGBA* out_colorRGBA)
{
    kzsAssert(kzcIsValidPointer(renderer));

    *out_colorRGBA = renderer->overrideClearColor;
    return renderer->overrideClearColorEnabled;
}

void kzuRendererSetViewport(struct KzuRenderer* renderer, kzFloat x, kzFloat y, kzFloat width, kzFloat height, kzFloat aspectRatio,
                            kzBool relative)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->viewportX = x;
    renderer->viewportY = y;
    renderer->viewportWidth = width;
    renderer->viewportHeight = height;
    renderer->viewportAspectRatio = aspectRatio;
    renderer->viewportRelative = relative;
}

void kzuRendererGetViewport(const struct KzuRenderer* renderer, kzFloat* out_x, kzFloat* out_y, kzFloat* out_width, kzFloat* out_height,
                            kzFloat* out_aspectRatio, kzBool* out_relative)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(out_x != KZ_NULL)
    {
        *out_x = renderer->viewportX;
    }
    if(out_y != KZ_NULL)
    {
        *out_y = renderer->viewportY;
    }    
    if(out_width != KZ_NULL)
    {
        *out_width = renderer->viewportWidth;
    }
    if(out_height != KZ_NULL)
    {
        *out_height = renderer->viewportHeight;
    }
    if(out_aspectRatio != KZ_NULL)
    {
        *out_aspectRatio = renderer->viewportAspectRatio;
    }
    if(out_relative != KZ_NULL)
    {
        *out_relative = renderer->viewportRelative;
    }
}

void kzuRendererGetAdjustedViewport(const struct KzuRenderer* renderer, const struct KzsWindow* window,
                                    kzFloat* out_x, kzFloat* out_y, kzFloat* out_width, kzFloat* out_height)
{
    kzFloat x, y, width, height;
    
    kzsAssert(kzcIsValidPointer(renderer));    

    x = renderer->viewportX;
    y = renderer->viewportY;
    width = renderer->viewportWidth;
    height = renderer->viewportHeight;
    if(renderer->viewportRelative)
    {
        kzuWindowGetAdjustedViewportRectangle(
            (kzFloat)kzsWindowGetWidth(window) * x, 
            (kzFloat)kzsWindowGetHeight(window) * y,
            (kzFloat)kzsWindowGetWidth(window) * width,
            (kzFloat)kzsWindowGetHeight(window) * height, renderer->viewportAspectRatio,
            &x, &y, &width, &height);
    }
    *out_x = x;
    *out_y = y;
    *out_width = width;
    *out_height = height;
}

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
struct KzuShadowMap* kzuRendererGetShadowMap(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->shadowMap;
}
#endif

kzsError kzuRendererSetOverrideMaterialCallback(struct KzuRenderer* renderer, KzuRendererOverrideMaterialCallback callbackFunction, void* userData)
{
    kzsAssert(kzcIsValidPointer(renderer));

    renderer->overrideMaterialCallback = callbackFunction;
    renderer->overrideMaterialCallbackUserData = userData;

    kzsSuccess();
}

KzuRendererOverrideMaterialCallback kzuRendererGetOverrideMaterialCallback(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->overrideMaterialCallback;
}

void* kzuRendererGetOverrideMaterialCallbackUserData(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->overrideMaterialCallbackUserData;
}

struct KzcMemoryManager* kzuRendererGetQuickMemoryManager(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->quickMemoryManager;
}

kzUint kzuRendererGetBatchCount(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->debugInfo.batchCount;
}

kzUint kzuRendererGetTriangleCount(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return kzcRendererGetTriangleCount(renderer->coreRenderer);
}

kzUint kzuRendererGetShaderSwitchCount(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    return kzcRendererGetShaderSwitchCount(renderer->coreRenderer);
#else
    return 0;
#endif
}

kzUint kzuRendererGetTextureSwitchCount(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return kzcRendererGetTextureSwitchCount(renderer->coreRenderer);
}

void kzuRendererSetFrameBufferCallback(/*lint -e{818}*/struct KzuRenderer* renderer, KzuRendererFrameBufferCallback callbackFunction)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzcRendererSetActiveFrameBufferCallback(renderer->coreRenderer, (KzcRendererFrameBufferCallback)callbackFunction);
#else
    KZ_UNUSED_PARAMETER(renderer);
    KZ_UNUSED_PARAMETER(callbackFunction);
#endif
}

void kzuRendererEnableStereoscopic(struct KzuRenderer* renderer, enum KzuStereoscopicRenderingMode mode, kzUint pass)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->stereoscopicState.stereoscopicEnabled = KZ_TRUE;
    renderer->stereoscopicState.stereoscopicMode = mode;
    renderer->stereoscopicState.stereoscopicPass = pass;
    renderer->stereoscopicState.clearOnceOccurred = KZ_FALSE;
}

void kzuRendererSetStereoscopicClearOnceOccurred(struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->stereoscopicState.clearOnceOccurred = KZ_TRUE;
}

kzBool kzuRendererIsStereoscopicClearOnceOccurred(const struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    return renderer->stereoscopicState.clearOnceOccurred;
}

void kzuRendererDisableStereoscopic(struct KzuRenderer* renderer)
{
    kzsAssert(kzcIsValidPointer(renderer));
    renderer->stereoscopicState.stereoscopicEnabled = KZ_FALSE;
}

void kzuRendererGetStereoscopic(const struct KzuRenderer* renderer, kzBool* out_enabled, enum KzuStereoscopicRenderingMode* out_mode, kzUint* out_pass,
                                kzBool* out_flipped)
{
    kzsAssert(kzcIsValidPointer(renderer));
    if(out_enabled != KZ_NULL)
    {
        *out_enabled = renderer->stereoscopicState.stereoscopicEnabled;
    }
    if(out_mode != KZ_NULL)
    {
        *out_mode = renderer->stereoscopicState.stereoscopicMode;
    }
    if(out_pass != KZ_NULL)
    {
        *out_pass = renderer->stereoscopicState.stereoscopicPass;
    }
    if(out_flipped != KZ_NULL)
    {
        *out_flipped = renderer->stereoscopicState.steroscopicFlipped;
    }
}

kzsError kzuRendererPushOverrideScreenTargetTexture(const struct KzuRenderer* renderer, struct KzcTexture* overrideScreenTargetTexture)
{
    kzsError result;
    kzsAssert(kzcIsValidPointer(renderer));

    result = kzcStackPush(renderer->overrideScreenTargetTextureStack, overrideScreenTargetTexture);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuRendererPopOverrideScreenTargetTexture(const struct KzuRenderer* renderer, struct KzcTexture** out_overrideScreenTargetTexture)
{
    kzsError result;
    struct KzcTexture* texture;
    kzsAssert(kzcIsValidPointer(renderer));

    result = kzcStackPop(renderer->overrideScreenTargetTextureStack, (void**)&texture);
    kzsErrorForward(result);

    if(out_overrideScreenTargetTexture != KZ_NULL)
    {
        *out_overrideScreenTargetTexture = texture;
    }
    kzsSuccess();
}

kzsError kzuRendererPeekOverrideScreenTargetTexture(const struct KzuRenderer* renderer, struct KzcTexture** out_overrideScreenTargetTexture)
{
    kzsError result;
    struct KzcTexture* texture;
    kzsAssert(kzcIsValidPointer(renderer));

    if(kzcStackIsEmpty(renderer->overrideScreenTargetTextureStack))
    {
        texture = KZ_NULL;
    }
    else
    {
        result = kzcStackPeek(renderer->overrideScreenTargetTextureStack, (void**)&texture);
        kzsErrorForward(result);
    }

    *out_overrideScreenTargetTexture = texture;
    kzsSuccess();
}
