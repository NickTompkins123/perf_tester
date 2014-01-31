/**
* \file
* Transition shader effect private members.
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TRANSITION_SHADER_PRIVATE_H
#define KZU_TRANSITION_SHADER_PRIVATE_H

#include "kzu_transition_private.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Helper for getting transition internal data. */
#define kzuTransitionShaderGetData_internal(transitionShader_param) ((struct KzuTransitionShaderData*)transitionShader_param->transition.data)


/* Forward declaration structures. */
struct KzuTransition;
struct KzuMaterial;
struct KzuMesh;
struct KzuMeshNode;
struct KzuComposer;
struct KzuRenderPass;


struct KzuTransitionShader
{
    struct KzuTransition transition; /**< Transition. Used for inheritance. */
};

struct KzuTransitionShaderData
{
    struct KzuTransitionData transitionData; /**< Transition data. Used for inheritance. */ /*lint -esym(754, KzuTransitionShaderData::transitionData) Suppressed as this member is required but not referenced. */

    struct KzuEffectScene
    {
        struct KzuScene* scene;             /**< The scene which will be used to show the transition effect. */
        struct KzuCameraNode* cameraNode;   /**< Camera node used to render the effect scene. */
        struct KzuMesh* planeMesh;        /**< Plane mesh for rendering the quad. */
        struct KzuMeshNode* planeNode;    /**< Plane node bound to plane mesh. */
        struct KzuRenderPass* renderPass;   /**< Render pass for the effect scene.*/
    } effectScene;

    struct KzuMaterial* material;       /**< Shader used to render the transition. */
    struct KzcTexture* renderTarget1;   /**< Texture where source scene is rendered. */
    struct KzcTexture* renderTarget2;   /**< Texture where target scene is rendered. */
};


#endif
