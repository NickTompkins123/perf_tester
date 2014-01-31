/**
* \file
* Transition effect.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kza_transition_shader.h"
#include "kza_transition.h"

#include <application/kza_application.h>

#include <user/effect_system/transition/kzu_transition_shader_private.h>
#include <user/effect_system/transition/kzu_transition_shader.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_object.h>
#include <user/renderer/kzu_composer.h>
#include <user/renderer/kzu_render_pass.h>
#include <user/renderer/kzu_renderer.h>
#include <user/material/kzu_material.h>
#include <user/renderer/kzu_composer.h>
#include <user/engine/kzu_engine.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>

#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/util/color/kzc_color.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/debug/kzs_log.h>


kzsError kzaTransitionShaderUpdateEffect(const struct KzaTransition* transition, const kzFloat time)
{
    kzsError result;
    struct KzuTransition* transitionEffect = kzaTransitionGetTransitionEffect(transition);
    struct KzuTransitionShader* shaderTransitionEffect = kzuTransitionShaderFromTransition(transitionEffect);
    struct KzuMaterial* material;
    
    kzsAssert(kzcIsValidPointer(shaderTransitionEffect));

    material = kzuTransitionShaderGetData_internal(shaderTransitionEffect)->material;

    if (material != KZ_NULL)
    {
        struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);

        result = kzuPropertyManagerSetFloat(propertyManager, material, KZU_PROPERTY_TYPE_BLEND_INTENSITY, time);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

/* TODO: Simplify rendering of multiple scenes when it is properly supported by the engine. Don't use application for it. */
/* TODO: Above TODO message is incorrect. Always use application, because there can be animations or scripts affecting the scene. */
kzsError kzaTransitionShaderBegin(const struct KzaTransition* transition, struct KzaApplication* application)
{
    kzsError result;
    struct KzuTransitionShaderData* data;
    struct KzuTransitionShader* shaderTransitionEffect;
    struct KzuTransition* transitionEffect;
    struct KzuRenderer* renderer;

    kzsAssert(kzcIsValidPointer(transition));
    kzsAssert(kzcIsValidPointer(application));

    renderer = kzuEngineGetRenderer(kzaApplicationGetEngine(application));

    transitionEffect = kzaTransitionGetTransitionEffect(transition);
    shaderTransitionEffect = kzuTransitionShaderFromTransition(transitionEffect);
    kzsAssert(kzcIsValidPointer(shaderTransitionEffect));
    data = kzuTransitionShaderGetData_internal(shaderTransitionEffect);

    /* Render content to render target 1. */
    result = kzaApplicationSetScene(application, kzaTransitionGetSourceScene(transition));
    kzsErrorForward(result);

    result = kzuRendererPushOverrideScreenTargetTexture(renderer, data->renderTarget1);
    kzsErrorForward(result);
    result = kzaApplicationUpdate(application, 0);
    kzsErrorForward(result);
    result = kzaApplicationUpdateWindows(application);
    kzsErrorForward(result);
    result = kzuRendererPopOverrideScreenTargetTexture(renderer, KZ_NULL);
    kzsErrorForward(result);

    /* Render content to render target 2. */
    result = kzaApplicationSetScene(application, kzaTransitionGetTargetScene(transition));
    kzsErrorForward(result);
    {
        struct KzuCameraNode* cameraNode = kzaTransitionGetTargetCameraNode(transition);
        if(cameraNode != KZ_NULL)
        {
            kzuSceneSetViewCamera(kzaTransitionGetTargetScene(transition), cameraNode);
        }
    }
    result = kzuRendererPushOverrideScreenTargetTexture(renderer, data->renderTarget2);
    kzsErrorForward(result);
    result = kzaApplicationUpdate(application, 0);
    kzsErrorForward(result);
    result = kzaApplicationUpdateWindows(application);
    kzsErrorForward(result);
    result = kzuRendererPopOverrideScreenTargetTexture(renderer, KZ_NULL);
    kzsErrorForward(result);

    result = kzaApplicationSetScene(application, data->effectScene.scene);
    kzsErrorForward(result);

    /* Update once to remove artifacts from rendering. */
    result = kzaTransitionShaderUpdateEffect(transition, 0.0f);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzaTransitionShaderEnd(const struct KzaTransition* transition, struct KzaApplication* application)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(transition));

    result = kzaApplicationSetScene(application, kzaTransitionGetTargetScene(transition));
    kzsErrorForward(result);

    kzsSuccess();
}
