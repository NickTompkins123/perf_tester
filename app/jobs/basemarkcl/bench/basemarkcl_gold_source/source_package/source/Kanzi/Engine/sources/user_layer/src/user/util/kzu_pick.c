/**
* \file
* Color buffer based picking utility.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_pick.h"

#include <user/filter/kzu_object_source.h>
#include <user/material/kzu_material_type.h>
#include <user/material/kzu_material.h>
#include <user/properties/kzu_property.h>
#include <user/properties/kzu_color_property.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/engine/kzu_engine.h>
#include <user/project/kzu_project.h>
#include <user/renderer/kzu_render_pass.h>
#include <user/renderer/kzu_renderer.h>

#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/frame_buffer/kzc_resource_frame_buffer.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/display/kzs_window.h>


struct KzuPick
{
    struct KzcTexture* renderTarget; /** Render target used during picking. */
    struct KzcMemoryManager* quickMemoryManager; /** Quick memory manager used during picking. */
    struct KzuMaterial* material; /** Material used during picking. */
    struct KzuRenderPass* renderPass; /** Render pass used during picking. */
    kzUint idCounter; /** Object counter used during picking. */
    struct KzcDynamicArray* idObjectMap; /** Object to id map used during picking. */
    kzUint width; /** Render target width. */
    kzUint height; /** Render target height. */
    kzBool isDoubleSided;
};

/** Structure to store picking data associated with objects. */
struct KzuPickEntry
{
    const struct KzuTransformedObjectNode* transformedObjectNode;
    kzUint meshCluster;
};


/** (Re)creates the render buffer for the picking utility. */
static kzsError kzuPickCreateFrameBuffer_internal(struct KzuPick* pick, struct KzcResourceManager* resourceManager, kzUint width, kzUint height)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(pick));
    kzsAssert(kzcIsValidPointer(resourceManager));

    if (pick->renderTarget != KZ_NULL)
    {
        result = kzcTextureDelete(pick->renderTarget);
        kzsErrorForward(result);
    }

    result = kzcTextureCreateFrameBufferTexture(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FORMAT_RGB, width, height,
                                                KZC_TEXTURE_FILTER_POINT_SAMPLE, &pick->renderTarget);
    kzsErrorForward(result);

    result = kzuRenderPassAttachFrameBuffer(pick->renderPass, pick->renderTarget);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Packs given value to a 565 RGB-color. */
static struct KzcColorRGBA kzuPickPackValueToColor(kzUint value)
{
    /* Decode the counter as rgb color and convert to float representation. Works for 16 bit 5-6-5 colors right now. */
    const kzUint redShift = 11;
    const kzUint greenShift = 5;
    const kzUint blueShift = 0;
    const kzUint redMask = 0x1f;
    const kzUint greenMask = 0x3f;
    const kzUint blueMask = 0x1f;
    const kzFloat redDivisor = 31.0f;
    const kzFloat greenDivisor = 63.0f;
    const kzFloat blueDivisor = 31.0f;

    const kzUint red = (value >> redShift) & redMask;
    const kzUint green = (value >> greenShift) & greenMask;
    const kzUint blue = (value >> blueShift) & blueMask; /*lint !e845 Right argument for >> is always 0 */

    const kzFloat normalizedRed = (kzFloat)red / redDivisor;
    const kzFloat normalizedGreen = (kzFloat)green / greenDivisor;
    const kzFloat normalizedBlue = (kzFloat)blue / blueDivisor;

    return kzcColorRGBA(normalizedRed, normalizedGreen, normalizedBlue, 1.0f);
}

/** Material override callback for object picking. */
KZ_CALLBACK static kzsError kzuPickMaterialOverride_internal(const struct KzuTransformedObjectNode* transformedObjectNode, struct KzuMaterial* material,
                                                             kzUint cluster, void* userData, struct KzuMaterial** out_material)
{
    kzsError result;
    struct KzuPick* pick = (struct KzuPick*)userData;
    struct KzuPickEntry* pickEntry;

    kzsAssert(kzcIsValidPointer(pick));

    {
        struct KzcColorRGBA color = kzuPickPackValueToColor(pick->idCounter);
        result = kzuPropertyManagerSetColor(kzuMaterialGetPropertyManager(pick->material), pick->material, KZU_PROPERTY_TYPE_EMISSIVE, color);
        kzsErrorForward(result);
    }

    /* Allocate and initialize the entry. */
    result = kzcMemoryAllocVariable(pick->quickMemoryManager, pickEntry, "PickEntry");
    kzsErrorForward(result);

    pickEntry->transformedObjectNode = transformedObjectNode;
    pickEntry->meshCluster = cluster;

    /* Add entry to the dynamic array. */
    result = kzcDynamicArrayAdd(pick->idObjectMap, pickEntry);
    kzsErrorForward(result);

    /* Advance the counter. */
    ++pick->idCounter;

    /* Return the override material. */
    *out_material = pick->material;
    kzsSuccess();
}


/** Material override callback for material picking. */
KZ_CALLBACK static kzsError kzuPickMaterialOverrideForMaterial_internal(const struct KzuTransformedObjectNode* transformedObjectNode, struct KzuMaterial* material,
                                                                        kzUint cluster, void* userData, struct KzuMaterial** out_material)
{
     kzsError result;
     struct KzuPick* pick = (struct KzuPick*)userData;

     kzsAssert(kzcIsValidPointer(pick));

     {
         struct KzcColorRGBA color = kzuPickPackValueToColor(pick->idCounter);
         result = kzuPropertyManagerSetColor(kzuMaterialGetPropertyManager(pick->material), pick->material, KZU_PROPERTY_TYPE_EMISSIVE, color);
         kzsErrorForward(result);
     }

     /* Add entry to the dynamic array. */
     result = kzcDynamicArrayAdd(pick->idObjectMap, (void*)material);
     kzsErrorForward(result);

     /* Advance the counter. */
     ++pick->idCounter;

     /* Return the override material. */
     *out_material = pick->material;
     kzsSuccess();
}

/* TODO: Check if FBO is supported */
kzsError kzuPickCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, const struct KzuEngine* engine, struct KzuPick** out_pick)
{
    kzsError result;
    struct KzuPick* pick;

    result = kzcMemoryAllocVariable(memoryManager, pick, "Pick");
    kzsErrorForward(result);

    result = kzcMemoryManagerCreateQuickManager(memoryManager, 8 * 1024, &pick->quickMemoryManager);
    kzsErrorForward(result);

    {
        pick->material = kzuRendererGetSolidColorMaterial(kzuEngineGetRenderer(engine));
        pick->isDoubleSided = KZ_FALSE;
    }

    /* Create the render pass. */
    {
        result = kzuRenderPassCreate(memoryManager, propertyManager, &pick->renderPass);
        kzsErrorForward(result);
        result = kzuRenderPassSetColorBuffer(pick->renderPass, KZ_TRUE, KZC_COLOR_BLACK_TRANSPARENT, KZU_COLOR_WRITE_RGBA);
        kzsErrorForward(result);
        /* Exact match between color & frame buffer is needed, hence calculate viewport from window dimensions. */
        kzuRenderPassSetFrameBufferViewportFromWindow(pick->renderPass, KZ_TRUE);
    }

    {
        struct KzsWindow* window = kzuEngineGetActiveWindow(engine);
        kzUint width = kzsWindowGetWidth(window);
        kzUint height = kzsWindowGetHeight(window);
        pick->renderTarget = KZ_NULL;
        result = kzuPickCreateFrameBuffer_internal(pick, kzuEngineGetResourceManager(engine), width, height);
        kzsErrorForward(result);

        /* Save dimensions. */
        pick->width = width;
        pick->height = height;
    }

    *out_pick = pick;
    kzsSuccess();
}

kzsError kzuPickDelete(struct KzuPick* pick)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(pick));

    result = kzuRenderPassDelete(kzuRenderPassToComposer(pick->renderPass));
    kzsErrorForward(result);
    result = kzcMemoryManagerDelete(pick->quickMemoryManager);
    kzsErrorForward(result);
    result = kzcTextureDelete(pick->renderTarget);
    kzsErrorForward(result);
    result = kzcMemoryFreeVariable(pick);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzuPickEnableDoubleSided(struct KzuPick* pick, kzBool enable)
{
    kzsAssert(kzcIsValidPointer(pick));
    
    pick->isDoubleSided = enable;
}

kzBool kzuPickIsDoubleSided(const struct KzuPick* pick)
{
    kzsAssert(kzcIsValidPointer(pick));

    return pick->isDoubleSided;
}

kzsError kzuPick(struct KzuPick* pick, const struct KzuEngine* engine, struct KzuObjectSource* pickObjectSource,
                 const struct KzuTransformedScene* transformedScene, struct KzuCameraNode* camera,
                 kzUint positionX, kzUint positionY, struct KzuObjectNode** out_node, kzUint* out_meshCluster)
{
    kzsError result;
    kzU16 pixel = 0;
    struct KzuObjectNode* outNode = KZ_NULL;
    kzUint outCluster = 0;
    kzBool isBoundingBoxVisualized; /* TODO: Remove. We shouldn't render bounding boxes. */

    kzsAssert(kzcIsValidPointer(pick));
    kzsAssert(kzcIsValidPointer(engine));
    kzsAssert(kzcIsValidPointer(pickObjectSource));
    kzsAssert(kzcIsValidPointer(camera));

    isBoundingBoxVisualized = kzuRendererIsBoundingBoxVisualizationEnabled(kzuEngineGetRenderer(engine));

    {
        struct KzsWindow* window = kzuEngineGetActiveWindow(engine);
        kzUint width = kzsWindowGetWidth(window);
        kzUint height = kzsWindowGetHeight(window);

        if (pick->width != width || pick->height != height)
        {
            result = kzuPickCreateFrameBuffer_internal(pick, kzuEngineGetResourceManager(engine), width, height);
            kzsErrorForward(result);
        }

        /* Save dimensions. */
        pick->width = width;
        pick->height = height;
    }

    result = kzcDynamicArrayCreate(pick->quickMemoryManager, &pick->idObjectMap);
    kzsErrorForward(result);

    result = kzuRendererSetOverrideMaterialCallback(kzuEngineGetRenderer(engine), kzuPickMaterialOverride_internal, pick);
    kzsErrorForward(result);

    /* Reset the id counter. */
    pick->idCounter = 1;
    
    /* Render. */
    {
        result = kzuRenderPassSetObjectSource(pick->renderPass, pickObjectSource);
        kzsErrorForward(result);
        kzuRenderPassSetCamera(pick->renderPass, camera);
        result = kzuRenderPassSetCullMode(pick->renderPass, (pick->isDoubleSided) ? KZU_CULL_NONE : KZU_CULL_BACK);        
        kzsErrorForward(result);
        kzuRendererSetBoundingBoxVisualizationEnabled(kzuEngineGetRenderer(engine), KZ_FALSE);
        result = kzuRenderPassApply(kzuRenderPassToComposer(pick->renderPass), kzuEngineGetRenderer(engine), transformedScene,
                                    kzuEngineGetActiveWindow(engine));
        kzsErrorForward(result);
        kzuRendererSetBoundingBoxVisualizationEnabled(kzuEngineGetRenderer(engine), isBoundingBoxVisualized);
    }

    /* Read the pixels back. */
    {
        struct KzcRenderer* renderer = kzuRendererGetCoreRenderer(kzuEngineGetRenderer(engine));
        struct KzcFrameBuffer* frameBuffer = kzcTextureGetFrameBufferReference(pick->renderTarget);
        kzcFrameBufferBind(frameBuffer, renderer);
        kzcRendererReadPixels(kzuRendererGetCoreRenderer(kzuEngineGetRenderer(engine)), (kzInt)positionX, (kzInt)pick->height - (kzInt)positionY, 1, 1,
                              KZC_RENDERER_COLOR_FORMAT_RGB, KZC_RENDERER_COLOR_TYPE_UNSIGNED_SHORT_5_6_5, &pixel);
        
        kzcFrameBufferUnbind(frameBuffer, renderer);
    }
    
    /* Find the node. */
    if (pixel != 0)
    {
        kzUint index = (kzUint)pixel;
        const struct KzuPickEntry* entry;
        kzsAssert(index <= kzcDynamicArrayGetSize(pick->idObjectMap));
        entry = (const struct KzuPickEntry*)kzcDynamicArrayGet(pick->idObjectMap, index - 1);
        kzsAssert(entry != KZ_NULL);
        outNode = kzuTransformedObjectNodeGetObjectNode(entry->transformedObjectNode);
        outCluster = entry->meshCluster;
    }

    result = kzuRendererSetOverrideMaterialCallback(kzuEngineGetRenderer(engine), KZ_NULL, KZ_NULL);
    kzsErrorForward(result);

    result = kzuRendererReset(kzuEngineGetRenderer(engine));
    kzsErrorForward(result);

    result = kzcMemoryManagerResetQuickManager(pick->quickMemoryManager);
    kzsErrorForward(result);

    *out_node = outNode;

    if (out_meshCluster != KZ_NULL)
    {
        *out_meshCluster = outCluster;
    }

    kzsSuccess();
}

kzsError kzuPickMaterial(struct KzuPick* pick, const struct KzuEngine* engine, struct KzuObjectSource* pickObjectSource,
                         const struct KzuTransformedScene* transformedScene, struct KzuCameraNode* camera,
                         kzUint positionX, kzUint positionY, struct KzuMaterial** out_material)
{
    kzsError result;
    kzU16 pixel = 0;
    struct KzuMaterial* outMaterial = KZ_NULL;
    kzBool isBoundingBoxVisualized;

    kzsAssert(kzcIsValidPointer(pick));
    kzsAssert(kzcIsValidPointer(engine));
    kzsAssert(kzcIsValidPointer(pickObjectSource));
    kzsAssert(kzcIsValidPointer(camera));

    isBoundingBoxVisualized = kzuRendererIsBoundingBoxVisualizationEnabled(kzuEngineGetRenderer(engine));

    {
        struct KzsWindow* window = kzuEngineGetActiveWindow(engine);
        kzUint width = kzsWindowGetWidth(window);
        kzUint height = kzsWindowGetHeight(window);

        if (pick->width != width || pick->height != height)
        {
            result = kzuPickCreateFrameBuffer_internal(pick, kzuEngineGetResourceManager(engine), width, height);
            kzsErrorForward(result);
        }

        /* Save dimensions. */
        pick->width = width;
        pick->height = height;
    }

    result = kzcDynamicArrayCreate(pick->quickMemoryManager, &pick->idObjectMap);
    kzsErrorForward(result);

    result = kzuRendererSetOverrideMaterialCallback(kzuEngineGetRenderer(engine), kzuPickMaterialOverrideForMaterial_internal, pick);
    kzsErrorForward(result);

    /* Reset the id counter. */
    pick->idCounter = 1;

    /* Render. */
    {
        result = kzuRenderPassSetObjectSource(pick->renderPass, pickObjectSource);
        kzsErrorForward(result);
        kzuRenderPassSetCamera(pick->renderPass, camera);
        result = kzuRenderPassSetCullMode(pick->renderPass, (pick->isDoubleSided) ? KZU_CULL_NONE : KZU_CULL_BACK);        
        kzsErrorForward(result);
        kzuRendererSetBoundingBoxVisualizationEnabled(kzuEngineGetRenderer(engine), KZ_FALSE);
        result = kzuRenderPassApply(kzuRenderPassToComposer(pick->renderPass), kzuEngineGetRenderer(engine), transformedScene,
            kzuEngineGetActiveWindow(engine));
        kzsErrorForward(result);
        kzuRendererSetBoundingBoxVisualizationEnabled(kzuEngineGetRenderer(engine), isBoundingBoxVisualized);
    }

    /* Read the pixels back. */
    {
        struct KzcRenderer* renderer = kzuRendererGetCoreRenderer(kzuEngineGetRenderer(engine));
        struct KzcFrameBuffer* frameBuffer = kzcTextureGetFrameBufferReference(pick->renderTarget);
        kzcFrameBufferBind(frameBuffer, renderer);
        kzcRendererReadPixels(kzuRendererGetCoreRenderer(kzuEngineGetRenderer(engine)), (kzInt)positionX, (kzInt)pick->height - (kzInt)positionY, 1, 1,
            KZC_RENDERER_COLOR_FORMAT_RGB, KZC_RENDERER_COLOR_TYPE_UNSIGNED_SHORT_5_6_5, &pixel);
        kzcFrameBufferUnbind(frameBuffer, renderer);
    }

    /* Find the object. */
    if (pixel != 0)
    {
        kzUint index = (kzUint)pixel;
        kzsAssert(index <= kzcDynamicArrayGetSize(pick->idObjectMap));
        outMaterial = (struct KzuMaterial*)kzcDynamicArrayGet(pick->idObjectMap, index - 1);
    }

    result = kzuRendererSetOverrideMaterialCallback(kzuEngineGetRenderer(engine), KZ_NULL, KZ_NULL);
    kzsErrorForward(result);

    result = kzuRendererReset(kzuEngineGetRenderer(engine));
    kzsErrorForward(result);

    result = kzcMemoryManagerResetQuickManager(pick->quickMemoryManager);
    kzsErrorForward(result);

    *out_material = outMaterial;
    kzsSuccess();
}
