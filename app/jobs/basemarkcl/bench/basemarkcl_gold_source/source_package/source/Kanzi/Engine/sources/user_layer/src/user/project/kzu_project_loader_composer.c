/**
* \file
* Project loader for composer files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_loader_composer.h"

#include "kzu_project_loader.h"
#include "kzu_project_loader_general.h"
#include "kzu_project_loader_property.h"
#include "kzu_project_loader_camera.h"
#include "kzu_project_loader_object_source.h"
#include "kzu_project_loader_texture.h"
#include "kzu_project_loader_material.h"
#include "kzu_project_loader_object_node.h"
#include "kzu_project.h"

#include <user/binary/kzu_binary_directory.h>
#include <user/renderer/kzu_composer.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_light.h>
#include <user/effect_system/stereoscopic_rendering/kzu_stereoscopic_composer.h>
#include <user/effect_system/kzu_bloom_composer.h>
#include <user/effect_system/kzu_dof_composer.h>
#include <user/effect_system/kzu_color_adjustment_composer.h>
#include <user/renderer/kzu_render_pass.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_bool_property.h>
#include <user/properties/kzu_float_property.h>
#include <user/properties/kzu_string_property.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/kzu_error_codes.h>

#include <core/renderer/kzc_renderer.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/string/kzc_string.h>

#include <system/display/kzs_window.h>
#include <system/debug/kzs_error.h>


/* Binary composer types. */
enum KzuBinaryComposerType
{
    KZU_BINARY_COMPOSER_TYPE_EMPTY = 0, /**< Binary empty composer. */
    KZU_BINARY_COMPOSER_TYPE_RENDER_PASS = 1, /**< Binary render pass composer. */
    KZU_BINARY_COMPOSER_TYPE_STEREOSCOPIC = 2, /**< Binary stereoscopic composer. */
    KZU_BINARY_COMPOSER_TYPE_BLOOM = 3, /**< Binary bloom composer. */
    KZU_BINARY_COMPOSER_TYPE_DOF = 4, /**< Binary DOF composer. */
    KZU_BINARY_COMPOSER_TYPE_BLUR = 5, /**< Binary blur composer. */
    KZU_BINARY_COMPOSER_TYPE_MOTION_BLUR = 6, /**< Binary motion blur composer. */
    KZU_BINARY_COMPOSER_TYPE_RADIAL_BLUR = 7, /**< Binary radial blur composer. */
    KZU_BINARY_COMPOSER_TYPE_VIGNETTE = 8, /**< Binary vignette composer. */
    KZU_BINARY_COMPOSER_TYPE_COLOR_ADJUST = 9, /**< Binary color adjustment composer. */
    KZU_BINARY_COMPOSER_TYPE_SELECTIVE = 10 /**< Binary selective composer. */
};

/* TODO: Enable when composer loading has been implemented. */
kzsException kzuProjectLoaderLoadComposer(struct KzuProject* project, kzString path, struct KzuComposer** out_composer)
{
    kzsError result;
    struct KzuBinaryFileInfo* file;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(project);
    struct KzuPropertyManager* propertyManager = kzuProjectGetPropertyManager(project);
    struct KzuComposer* composer;

    kzsErrorTest(path != KZ_NULL, KZU_ERROR_INVALID_FILE_PATH, "Trying to load composer with null path");

    /* Check if the composer is already loaded. */
    composer = (struct KzuComposer*)kzuProjectGetObject(project, KZU_PROJECT_OBJECT_TYPE_COMPOSER, path);

    if (!kzuComposerIsValid(composer))
    {
        struct KzcInputStream* inputStream;

        /* Get file info from directory corresponding the given path. */
        result = kzuBinaryDirectoryGetFile(kzuProjectGetBinaryDirectory(project), path, &file);
        kzsErrorForward(result);

        kzsErrorTest(kzuBinaryFileInfoGetType(file) == KZU_BINARY_FILE_TYPE_COMPOSER, KZU_ERROR_WRONG_BINARY_FILE_TYPE,
            "Wrong file type encountered while trying to load composer file.");

        /* Load the composer of the file info. */
        {
            /* References */
            kzUint numChildComposers;
            kzString* composerPaths;
            enum KzuBinaryComposerType composerType;
            const void* composerProperties = (const void*)&composerProperties;
            kzUint i;

            result = kzuBinaryDirectoryOpenFile(memoryManager, file, &inputStream);
            kzsErrorForward(result);

            result = kzcInputStreamReadU32Int(inputStream, &numChildComposers);
            kzsErrorForward(result);

            result = kzcMemoryAllocArray(memoryManager, composerPaths, numChildComposers, "Render pass references");
            kzsErrorForward(result);

            for (i = 0; i < numChildComposers; ++i)
            {
                result = kzuBinaryDirectoryReadReference(inputStream, file, &composerPaths[i]);
                kzsErrorForward(result);
            }

            /* Properties */
            result = kzuProjectLoaderReadProperties(project, inputStream, file, composerProperties);
            kzsErrorForward(result);

            result = kzcInputStreamReadS32Int(inputStream, (kzInt*)&composerType);
            kzsErrorForward(result);

            switch(composerType)
            {
                case KZU_BINARY_COMPOSER_TYPE_EMPTY:
                {
                    result = kzuComposerCreate(memoryManager, kzuProjectGetPropertyManager(project), &composer);
                    kzsErrorForward(result);
                    break;
                }
                case KZU_BINARY_COMPOSER_TYPE_RENDER_PASS:
                {
                    struct KzuRenderPass* renderPass;
                    kzString cameraPath;
                    kzString objectSourcePath;
                    kzString renderTargetPath;
                    kzString materialPath;

                    result = kzuRenderPassCreate(memoryManager, kzuProjectGetPropertyManager(project), &renderPass);
                    kzsErrorForward(result);
                    composer = kzuRenderPassToComposer(renderPass);

                    result = kzuBinaryDirectoryReadReference(inputStream, file, &cameraPath);
                    kzsErrorForward(result);
                    result = kzuBinaryDirectoryReadReference(inputStream, file, &objectSourcePath);
                    kzsErrorForward(result);
                    result = kzuBinaryDirectoryReadReference(inputStream, file, &renderTargetPath);
                    kzsErrorForward(result);
                    result = kzuBinaryDirectoryReadReference(inputStream, file, &materialPath);
                    kzsErrorForward(result);

                    /* Camera */
                    if (cameraPath != KZ_NULL)
                    {
                        struct KzuObjectNode* cameraObjectNode;

                        result = kzuProjectLoaderLoadObjectNode(project, cameraPath, &cameraObjectNode);
                        kzsErrorForward(result);
                        kzsErrorTest(kzuObjectNodeGetType(cameraObjectNode) == KZU_OBJECT_TYPE_CAMERA, KZU_ERROR_INVALID_BINARY_DATA,
                            "Camera reference in render pass must be either NULL or a reference to a camera node");

                        kzuRenderPassSetCamera(renderPass, kzuCameraNodeFromObjectNode(cameraObjectNode));
                    }

                    /* Object source */
                    {
                        struct KzuObjectSource* objectSource;

                        result = kzuProjectLoaderLoadObjectSource(project, objectSourcePath, &objectSource);
                        kzsErrorForward(result);

                        result = kzuRenderPassSetObjectSource(renderPass, objectSource);
                        kzsErrorForward(result);
                    }

                    /* Render target */
                    if (renderTargetPath != KZ_NULL)
                    {
                        struct KzcTexture* renderTarget;

                        result = kzuProjectLoaderLoadTexture(project, renderTargetPath, &renderTarget);
                        kzsErrorForward(result);

                        result = kzuRenderPassAttachFrameBuffer(renderPass, renderTarget);
                        kzsErrorForward(result);
                    }

                    /* Material. */
                    {
                        struct KzuMaterial* material = KZ_NULL;
                        if (materialPath != KZ_NULL)
                        {
                            result = kzuProjectLoaderLoadMaterial(project, materialPath, &material);
                            kzsErrorForward(result);
                        }
                        kzuRenderPassSetMaterialOverride(renderPass, material);
                    }


                    break;
                }
                case KZU_BINARY_COMPOSER_TYPE_STEREOSCOPIC:
                {
                    struct KzuMaterial* anaglyphicMaterial;
                    struct KzuStereoscopicComposer* stereoscopicComposer;
                    result = kzuStereoscopicComposerCreate(memoryManager, kzuProjectGetPropertyManager(project), &stereoscopicComposer);
                    kzsErrorForward(result);
                    composer = kzuStereoscopicComposerToComposer(stereoscopicComposer);

                    result = kzuProjectLoaderLoadMaterial(project, "Materials/PostprocessingEffectAnaglyphicRedCyan/PostprocessingEffectAnaglyphicRedCyanMaterial", &anaglyphicMaterial);
                    kzsExceptionCatch(result, KZU_EXCEPTION_FILE_NOT_FOUND)
                    {
                        kzsErrorThrow(KZU_ERROR_PROJECT_CORE_ASSET_NOT_FOUND, "Post processing stereoscopic material not found from CoreAssets");
                    }
                    result = kzuProjectAddPermanentFilePath(project, "Materials/PostprocessingEffectAnaglyphicRedCyan/PostprocessingEffectAnaglyphicRedCyanMaterial");
                    kzsErrorForward(result);

                    kzuStereoscopicComposerInitialize(stereoscopicComposer, kzuProjectGetResourceManager(project), anaglyphicMaterial);

                    break;
                }
                case KZU_BINARY_COMPOSER_TYPE_BLOOM:
                {
#ifndef KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS
                    /* Bloom composer not supported, create just empty composer. */
                    result = kzuComposerCreate(memoryManager, propertyManager, &composer);
                    kzsErrorForward(result);
#else
                    struct KzuBloomComposer* bloomComposer;
                    struct KzuMaterial* bloomMaterial;
                    struct KzuMaterial* blitMaterial;

                    result = kzuBloomComposerCreate(memoryManager, kzuProjectGetPropertyManager(project), &bloomComposer);
                    kzsErrorForward(result);
                    composer = kzuBloomComposerToComposer(bloomComposer);

                    result = kzuProjectLoaderLoadMaterial(project, "Materials/PostprocessingEffectBoxBlur/PostprocessingEffectBloomBlurMaterial", &bloomMaterial);
                    kzsExceptionCatch(result, KZU_EXCEPTION_FILE_NOT_FOUND)
                    {
                        kzsErrorThrow(KZU_ERROR_PROJECT_CORE_ASSET_NOT_FOUND, "Post processing blur material not found from CoreAssets");
                    }
                    result = kzuProjectLoaderLoadMaterial(project, "Materials/PostprocessingBlit/PostprocessingBlitMaterial", &blitMaterial);
                    kzsExceptionCatch(result, KZU_EXCEPTION_FILE_NOT_FOUND)
                    {
                        kzsErrorThrow(KZU_ERROR_PROJECT_CORE_ASSET_NOT_FOUND, "Post processing blit material not found from CoreAssets");
                    }
                    result = kzuProjectAddPermanentFilePath(project, "Materials/PostprocessingBlit/PostprocessingBlitMaterial");
                    kzsErrorForward(result);
                    result = kzuProjectAddPermanentFilePath(project, "Materials/PostprocessingEffectBoxBlur/PostprocessingEffectBloomBlurMaterial");
                    kzsErrorForward(result);

                    result = kzuBloomComposerInitialize(bloomComposer, kzuProjectGetResourceManager(project), 256, 256, bloomMaterial, blitMaterial);
                    kzsErrorForward(result);
#endif
                    break;
                }
                case KZU_BINARY_COMPOSER_TYPE_COLOR_ADJUST:
                {
#ifndef KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS
                    /* Color adjust composer not supported, create just empty composer. */
                    result = kzuComposerCreate(memoryManager, propertyManager, &composer);
                    kzsErrorForward(result);
#else
                    struct KzuColorAdjustmentComposer* colorAdjustComposer;
                    struct KzuMaterial* colorAdjustmentMaterial;

                    result = kzuColorAdjustmentComposerCreate(memoryManager, kzuProjectGetPropertyManager(project), &colorAdjustComposer);
                    kzsErrorForward(result);
                    composer = kzuColorAdjustmentComposerToComposer(colorAdjustComposer);

                    result = kzuProjectLoaderLoadMaterial(project, "Materials/PostprocessingEffectColorAdjust/PostprocessingEffectColorAdjustMaterial", &colorAdjustmentMaterial);
                    kzsExceptionCatch(result, KZU_EXCEPTION_FILE_NOT_FOUND)
                    {
                        kzsErrorThrow(KZU_ERROR_PROJECT_CORE_ASSET_NOT_FOUND, "Post processing color adjust material not found from CoreAssets");
                    }
                    result = kzuProjectAddPermanentFilePath(project, "Materials/PostprocessingEffectColorAdjust/PostprocessingEffectColorAdjustMaterial");
                    kzsErrorForward(result);

                    result = kzuColorAdjustmentComposerInitialize(colorAdjustComposer, kzuProjectGetResourceManager(project), 0, 0, colorAdjustmentMaterial);
                    kzsErrorForward(result);
#endif
                    break;
                }
                case KZU_BINARY_COMPOSER_TYPE_DOF:
                {
#ifndef KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS
                    /* DOF composer not supported, create just empty composer. */
                    result = kzuComposerCreate(memoryManager, propertyManager, &composer);
                    kzsErrorForward(result);
#else
                    struct KzuDOFComposer* dofComposer;
                    struct KzuMaterial* depthMaterial;
                    struct KzuMaterial* blurMaterial;
                    struct KzuMaterial* blitMaterial;

                    result = kzuDOFComposerCreate(memoryManager, propertyManager, &dofComposer);
                    kzsErrorForward(result);
                    composer = kzuDOFComposerToComposer(dofComposer);

                    result = kzuProjectLoaderLoadMaterial(project, "Materials/PostprocessingEffectBoxBlur/PostprocessingEffectBloomBlurMaterial", &blurMaterial);
                    kzsExceptionCatch(result, KZU_EXCEPTION_FILE_NOT_FOUND)
                    {
                        kzsErrorThrow(KZU_ERROR_PROJECT_CORE_ASSET_NOT_FOUND, "Post processing blur material not found from CoreAssets");
                    }
                    result = kzuProjectLoaderLoadMaterial(project, "Materials/PostprocessingDOFBlit/PostprocessingDOFBlit", &blitMaterial);
                    kzsExceptionCatch(result, KZU_EXCEPTION_FILE_NOT_FOUND)
                    {
                        kzsErrorThrow(KZU_ERROR_PROJECT_CORE_ASSET_NOT_FOUND, "Post processing DOF blit material not found from CoreAssets");
                    }
                    result = kzuProjectLoaderLoadMaterial(project, "Materials/PostprocessingEffectDepth/PostprocessingEffectDepthMaterial", &depthMaterial);
                    kzsExceptionCatch(result, KZU_EXCEPTION_FILE_NOT_FOUND)
                    {
                        kzsErrorThrow(KZU_ERROR_PROJECT_CORE_ASSET_NOT_FOUND, "Post processing depth material not found from CoreAssets");
                    }
                    result = kzuProjectAddPermanentFilePath(project, "Materials/PostprocessingEffectDepth/PostprocessingEffectDepthMaterial");
                    kzsErrorForward(result);
                    result = kzuProjectAddPermanentFilePath(project, "Materials/PostprocessingDOFBlit/PostprocessingDOFBlit");
                    kzsErrorForward(result);
                    result = kzuProjectAddPermanentFilePath(project, "Materials/PostprocessingEffectBoxBlur/PostprocessingEffectBloomBlurMaterial");
                    kzsErrorForward(result);

                    result = kzuDOFComposerInitialize(dofComposer, kzuProjectGetResourceManager(project), 256, 256, blurMaterial, depthMaterial, blitMaterial);
                    kzsErrorForward(result);
#endif
                    break;
                }
                case KZU_BINARY_COMPOSER_TYPE_BLUR:
                case KZU_BINARY_COMPOSER_TYPE_MOTION_BLUR:
                case KZU_BINARY_COMPOSER_TYPE_RADIAL_BLUR:
                case KZU_BINARY_COMPOSER_TYPE_SELECTIVE:
                case KZU_BINARY_COMPOSER_TYPE_VIGNETTE:
                {
                    KZS_NOT_IMPLEMENTED_YET_ERROR;
                }
            }

            /* Move properties from temporary composer to the real one. */
            result = kzuPropertyManagerCopyProperties(propertyManager, composerProperties, composer);
            kzsErrorForward(result);
            result = kzuPropertyManagerRemoveProperties(propertyManager, composerProperties);
            kzsErrorForward(result);

            result = kzcInputStreamDelete(inputStream);
            kzsErrorForward(result);

            for (i = 0; i < numChildComposers; ++i)
            {
                struct KzuComposer* subComposer;

                result = kzuProjectLoaderLoadComposer(project, composerPaths[i], &subComposer);
                kzsErrorForward(result);

                result = kzuComposerAddChild(composer, subComposer);
                kzsErrorForward(result);
            }

            result = kzcMemoryFreeArray((kzMutableString*)composerPaths);
            kzsErrorForward(result);

            if(composerType == KZU_BINARY_COMPOSER_TYPE_RENDER_PASS)
            {
                /* Assign shadow pass. */
                struct KzuRenderPass* renderPass = kzuRenderPassFromComposer(composer);
                struct KzuObjectNode* lightNode = (struct KzuObjectNode*)kzuPropertyManagerGetVoidDefault(propertyManager, renderPass, KZU_PROPERTY_TYPE_SHADOW_PASS_SHADOW_CASTER);

                if (kzuPropertyManagerGetBoolDefault(propertyManager, renderPass, KZU_PROPERTY_TYPE_IS_SHADOW_PASS) &&
                    lightNode != KZ_NULL)
                {
                    kzuRenderPassSetShadowPassEnabled(renderPass, KZ_TRUE);
                    kzuRenderPassSetShadowPassInformation(renderPass, kzuLightNodeFromObjectNode(lightNode),
                                                          kzuPropertyManagerGetFloatDefault(propertyManager, renderPass, KZU_PROPERTY_TYPE_SHADOW_PASS_NEAR_PLANE),
                                                          kzuPropertyManagerGetFloatDefault(propertyManager, renderPass, KZU_PROPERTY_TYPE_SHADOW_PASS_FAR_PLANE),
                                                          kzuPropertyManagerGetFloatDefault(propertyManager, renderPass, KZU_PROPERTY_TYPE_SHADOW_PASS_VIEW_ANGLE));
                }
                else
                {
                    kzuRenderPassSetShadowPassEnabled(renderPass, KZ_FALSE);
                }
            }

            result = kzuProjectObjectLoaded(project, KZU_PROJECT_OBJECT_TYPE_COMPOSER, composer, kzuBinaryFileInfoGetPath(file),
                (kzuBinaryFileInfoGetFlags(file) & KZU_BINARY_FILE_INFO_FLAG_CACHED_RESOURCE) != 0, (void**)&composer);
            kzsErrorForward(result);
        }
    }

    *out_composer = composer;
    kzsSuccess();
}

kzsException kzuProjectLoaderUnloadComposer(struct KzuProject* project, const struct KzuComposer* composer)
{
    kzsError result;
    kzString objectPath = kzuProjectGetObjectPath(project, composer);

    if(objectPath != KZ_NULL)
    {
        result = kzuProjectLoaderUnloadObject(project, objectPath);
        kzsErrorForward(result);
    }

    kzsSuccess();
}
