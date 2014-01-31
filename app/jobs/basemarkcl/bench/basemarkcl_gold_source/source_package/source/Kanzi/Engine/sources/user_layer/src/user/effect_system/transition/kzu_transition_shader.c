/**
* \file
* Transition shader effect.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_transition_shader.h"

#include "kzu_transition.h"
#include "kzu_transition_shader_private.h"

#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_object.h>
#include <user/renderer/kzu_composer.h>
#include <user/renderer/kzu_render_pass.h>
#include <user/material/kzu_material.h>
#include <user/renderer/kzu_composer.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/util/color/kzc_color.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/debug/kzs_log.h>


kzsError kzuTransitionShaderCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                   struct KzcResourceManager* resourceManager, struct KzuObjectSource* rootObjectSource,
                                   kzUint renderTargetWidth, kzUint renderTargetHeight, struct KzuMaterial* material, 
                                   struct KzuTransitionShader** out_transition)
{
    kzsError result;
    struct KzuTransition* transition;
    struct KzuTransitionShaderData* data;

    kzsAssert(material == KZ_NULL || kzcIsValidPointer(material));

    if(material != KZ_NULL)
    {
        if (!kzuPropertyManagerHasProperty(propertyManager, material, KZU_PROPERTY_TYPE_BLEND_INTENSITY))
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Shader bound to TransitionEffect did not have BlendIntensity property");
        }
    }

    result = kzcMemoryAllocVariable(memoryManager, data, "Transition Shader Data");
    kzsErrorForward(result);

    result = kzuTransitionCreate_private(memoryManager, KZU_EFFECT_TRANSITION_TYPE_SHADER, &data->transitionData, &transition);
    kzsErrorForward(result);

    data->material = material;

    /* Create render target textures. */
    {
        result = kzcTextureCreateFrameBufferTexture(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGBA, 
            renderTargetWidth, renderTargetHeight, KZC_TEXTURE_FILTER_BILINEAR, &data->renderTarget1);
        kzsErrorForward(result);
        result = kzcTextureCreateFrameBufferTexture(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGBA,
            renderTargetWidth, renderTargetHeight, KZC_TEXTURE_FILTER_BILINEAR, &data->renderTarget2);
        kzsErrorForward(result);
    }

    {
        struct KzuEffectScene* scene = &data->effectScene;
        result = kzuSceneCreate(memoryManager, propertyManager, messageDispatcher, "Transition scene", &scene->scene);
        kzsErrorForward(result);

        /* Create camera */
        result = kzuCameraNodeCreate(memoryManager, propertyManager, messageDispatcher, "Transition Effect Camera", &scene->cameraNode);
        kzsErrorForward(result);
        result = kzuSceneAddObject(scene->scene, kzuCameraNodeToObjectNode(scene->cameraNode));
        kzsErrorForward(result);
        result = kzuCameraNodeSetOrthogonalProjection(scene->cameraNode, KZU_ORTHOGONAL_COORDINATE_SYSTEM_RELATIVE);
        kzsErrorForward(result);
        kzuSceneSetViewCamera(scene->scene, scene->cameraNode);
        result = kzuCameraNodeSetAspectRatio(scene->cameraNode, 1.0f);
        kzsErrorForward(result);
        result = kzuCameraNodeLookAt(scene->cameraNode, kzcVector3(0.0f, 0.0f, 1.0f), kzcVector3(0.0f, 0.0f, 0.0f), KZC_VECTOR3_Y_AXIS);
        kzsErrorForward(result);
        result = kzuCameraNodeSetDisableAspectRatio(scene->cameraNode, KZ_FALSE);
        kzsErrorForward(result);
        result = kzuCameraNodeSetZNear(scene->cameraNode, 0.1f);
        kzsErrorForward(result);
        result = kzuCameraNodeSetOrthogonalPlaneHeight(scene->cameraNode, 1.0f);
        kzsErrorForward(result);

        /* Create plane mesh. */
        result = kzuMeshCreate(memoryManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, &scene->planeMesh);
        kzsErrorForward(result);
        result = kzuMeshGeneratePlane(scene->planeMesh, resourceManager, 1.0f, 1.0f, KZU_PLANE_XY, KZ_TRUE, KZ_TRUE);
        kzsErrorForward(result);
        if(material != KZ_NULL)
        {
            kzuMeshSetMaterial(scene->planeMesh, material);
            kzsErrorForward(result);
        }
        
        result = kzuMeshNodeCreate(memoryManager, propertyManager, messageDispatcher, "Transition Effect Plane Node", scene->planeMesh, &scene->planeNode);
        kzsErrorForward(result);
        result = kzuSceneAddObject(scene->scene, kzuMeshNodeToObjectNode(scene->planeNode));
        kzsErrorForward(result);

        /* Create render pass. */
        result = kzuRenderPassCreate(memoryManager, propertyManager, &scene->renderPass);
        kzsErrorForward(result);
        result = kzuRenderPassSetObjectSource(scene->renderPass, rootObjectSource);
        kzsErrorForward(result);
        kzuRenderPassSetCamera(scene->renderPass, scene->cameraNode);
        result = kzuRenderPassSetCullMode(scene->renderPass, KZU_CULL_NONE);
        kzsErrorForward(result);
        result = kzuRenderPassSetColorBuffer(scene->renderPass, KZ_FALSE, kzcColorRGBA(0.0f, 0.0f, 0.0f, 0.0f), KZU_COLOR_WRITE_RGBA);
        kzsErrorForward(result);
        result = kzuRenderPassSetDepthBuffer(scene->renderPass, KZ_TRUE, 1.0f, KZ_FALSE, KZ_FALSE);
        kzsErrorForward(result);
        kzuRenderPassSetViewportRelative(scene->renderPass, 0.0f, 0.0f, 1.0f, 1.0f);

        /* Add to scene. */
        kzuSceneSetComposer(scene->scene, kzuRenderPassToComposer(scene->renderPass));
    }

     *out_transition = kzuTransitionShaderFromTransition(transition);
    kzsSuccess();
}

kzsError kzuTransitionShaderDelete(struct KzuTransitionShader* transition)
{
    kzsError result;
    struct KzuEffectScene* scene;
    struct KzcTexture* renderTarget1;
    struct KzcTexture* renderTarget2;

    kzsAssert(kzcIsValidPointer(transition));

    scene = &(kzuTransitionShaderGetData_internal(transition)->effectScene);
    renderTarget1 = kzuTransitionShaderGetData_internal(transition)->renderTarget1;
    renderTarget2 = kzuTransitionShaderGetData_internal(transition)->renderTarget2;
    
    result = kzcTextureDelete(renderTarget1);
    kzsErrorForward(result);
    result = kzcTextureDelete(renderTarget2);
    kzsErrorForward(result);

    result = kzuRenderPassDelete(kzuRenderPassToComposer(scene->renderPass));
    kzsErrorForward(result);

    result = kzuCameraNodeDelete(scene->cameraNode);
    kzsErrorForward(result);

    result = kzuMeshDelete(scene->planeMesh);
    kzsErrorForward(result);
    result = kzuMeshNodeDelete(scene->planeNode);
    kzsErrorForward(result);

    result = kzuSceneDelete(scene->scene);
    kzsErrorForward(result);

    result = kzuTransitionDeleteBase_private(&transition->transition);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzuTransitionShader* kzuTransitionShaderFromTransition(struct KzuTransition* transition)
{
    kzsAssert(kzcIsValidPointer(transition));
    kzsAssert(transition->data->type == KZU_EFFECT_TRANSITION_TYPE_SHADER);
    return (struct KzuTransitionShader*)transition;
}

struct KzuTransition* kzuTransitionFromTransitionShader(struct KzuTransitionShader* transitionShader)
{
    kzsAssert(kzcIsValidPointer(transitionShader));
    return (struct KzuTransition*)transitionShader;
}

struct KzuTransition* kzuTransitionShaderToTransition(struct KzuTransitionShader* transition)
{
    kzsAssert(kzcIsValidPointer(transition));
    return (struct KzuTransition*)transition;
}

kzsError kzuTransitionShaderStart(const struct KzuTransitionShader* transition)
{
    kzsError result;
    struct KzuTransitionShaderData* data;
    struct KzuMaterial* material;

    kzsAssert(kzcIsValidPointer(transition));

    data = kzuTransitionShaderGetData_internal(transition);
    material = data->material;

    if(material != KZ_NULL)
    {
        struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
        const struct KzuPropertyType* texture1Property = kzuPropertyManagerFindPropertyType(propertyManager, "Texture");
        const struct KzuPropertyType* texture2Property = kzuPropertyManagerFindPropertyType(propertyManager, "Texture2");

        /* Use textures if found. */
        if (texture1Property!= KZ_NULL && kzuPropertyManagerHasProperty(propertyManager, material, texture1Property))
        {
            result = kzuPropertyManagerSetVoid(propertyManager, material, texture1Property, data->renderTarget1);
            kzsErrorForward(result);
        }

        if (texture2Property!= KZ_NULL && kzuPropertyManagerHasProperty(propertyManager, material, texture2Property))
        {
            result = kzuPropertyManagerSetVoid(propertyManager, material, texture2Property, data->renderTarget2);
            kzsErrorForward(result);
        }
    }

    kzsSuccess();
}
