/**
* \file
* Specifies a shadow map rendering utility, that is used for rendering shadows based on depth pass rendered from light.
*/
#include "kzu_shadow_map.h"

#include <system/wrappers/kzs_opengl_base.h>
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)

#include <user/renderer/kzu_renderer.h>
#include <user/properties/kzu_matrix4x4_property.h>
#include <user/properties/kzu_property.h>

#include <core/renderer/kzc_renderer.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/memory/kzc_memory_manager.h>


/* Name of the shadow map property. */
const kzString KZU_PROPERTY_SHADOW_PROJECTION_MATRIX            = "kzLightShadowProjectionMatrix";


struct KzuShadowMap
{
    struct KzuMatrix4x4Property* shadowMatrixProperty; /**< Property for shadow matrix. */
    kzBool depthPass; /**< Depth pass enabled / disabled. */
};


kzsError kzuShadowMapCreate(const struct KzcMemoryManager* memoryManager, struct KzuShadowMap** out_shadowMap)
{
    kzsError result;
    struct KzuMatrix4x4PropertyType* shadowMatrixPropertyType;
    struct KzuShadowMap* shadowMap;

    result = kzcMemoryAllocVariable(memoryManager, shadowMap, "ShadowMap");
    kzsErrorForward(result);

    /* Create matrix property for shadow map. */
    result = kzuMatrix4x4PropertyTypeCreate(memoryManager, KZU_PROPERTY_SHADOW_PROJECTION_MATRIX, KZC_MATRIX4X4_IDENTITY, &shadowMatrixPropertyType);
    kzsErrorForward(result);

    result = kzuMatrix4x4PropertyCreate(memoryManager, shadowMatrixPropertyType, &KZC_MATRIX4X4_IDENTITY, &shadowMap->shadowMatrixProperty);
    kzsErrorForward(result);

    shadowMap->depthPass = KZ_FALSE;

    *out_shadowMap = shadowMap;
    kzsSuccess();
}

kzsError kzuShadowMapDelete(struct KzuShadowMap* shadowMap)
{
    kzsError result;
    struct KzuPropertyType* shadowCasterPropertyType = kzuPropertyGetPropertyType(kzuMatrix4x4PropertyToProperty(shadowMap->shadowMatrixProperty));

    result = kzuPropertyDelete(kzuMatrix4x4PropertyToProperty(shadowMap->shadowMatrixProperty));
    kzsErrorForward(result);

    if(shadowCasterPropertyType != KZ_NULL)
    {
        result = kzuPropertyTypeDelete(shadowCasterPropertyType);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(shadowMap);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuShadowMapPrepare(const struct KzuShadowMap* shadowMap, const struct KzuRenderer* renderer)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    struct KzcRenderer* coreRenderer;
    struct KzcMatrix4x4 biasMatrix;
    struct KzcMatrix4x4 lightMatrix;

    kzsAssert(kzcIsValidPointer(renderer));
    kzsAssert(kzcIsValidPointer(shadowMap));
    
    /* Calculate shadow matrix from shadow pass. */
    if(shadowMap->depthPass)
    {
        coreRenderer = kzuRendererGetCoreRenderer(renderer);

        kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD, &KZC_MATRIX4X4_IDENTITY);
        kzcRendererCalculateProjectionCameraWorldMatrix(coreRenderer);
        biasMatrix = kzcRendererGetMatrix(coreRenderer, KZC_RENDERER_MATRIX_PROJECTION_CAMERA_WORLD);

        /* Calculate bias matrix, that transfers from [-1...-1] to [0...1] coordinates (texture area). */
        kzcMatrix4x4Multiply(&biasMatrix, &KZC_MATRIX4X4_SHADOW_MAP_BIAS, &lightMatrix);
        kzuMatrix4x4PropertySetValue(shadowMap->shadowMatrixProperty, &lightMatrix);
    }

#else
    KZ_UNUSED_PARAMETER(shadowMap);
    KZ_UNUSED_PARAMETER(renderer);
#endif
    kzsSuccess();
}

kzsError kzuShadowMapApply(const struct KzuShadowMap* shadowMap, const struct KzuRenderer* renderer)
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsError result;
    struct KzcMatrix4x4 resultMatrix;

    kzsAssert(kzcIsValidPointer(shadowMap));
    kzsAssert(kzcIsValidPointer(renderer));
    
    if(!shadowMap->depthPass)
    {
        resultMatrix = kzuMatrix4x4PropertyGetValue(shadowMap->shadowMatrixProperty);

        /* Pass uniform for core renderer. */
        result = kzcRendererSetUniformMatrix4x4(kzuRendererGetCoreRenderer(renderer), KZU_PROPERTY_SHADOW_PROJECTION_MATRIX, &resultMatrix);
        kzsErrorForward(result);
    }

#else
    KZ_UNUSED_PARAMETER(shadowMap);
    KZ_UNUSED_PARAMETER(renderer);
#endif

    kzsSuccess();
}

void kzuShadowMapSetDepthPass(struct KzuShadowMap* shadowMap, kzBool enabled)
{
    kzsAssert(kzcIsValidPointer(shadowMap));
    shadowMap->depthPass = enabled;
}

kzBool kzuShadowMapIsDepthPass(const struct KzuShadowMap* shadowMap)
{
    kzsAssert(kzcIsValidPointer(shadowMap));
    return shadowMap->depthPass;
}

#endif
