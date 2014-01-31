/**
* \file
* Heads up display for displaying debug information.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_hud.h"

#include <user/util/hud/kzu_hud_data.h>
#include <user/project/kzu_project.h>
#include <user/renderer/kzu_renderer.h>
#include <user/renderer/kzu_renderer_util.h>

#include <core/util/image/kzc_image.h>
#include <core/util/font/kzc_font.h>
#include <core/util/font/kzc_text_layouter.h>
#include <core/util/font/kzc_text_layout.h>
#include <core/util/font/kzc_bitmap_font.h>
#include <core/util/color/kzc_color.h>
#include <core/util/string/kzc_string.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/renderer/kzc_renderer.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_math.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_opengl_base.h>


/** Size of old fps buffer for calculating average. */
#define KZU_HUD_BUFFER_SIZE 100
/** Maximum number of log messages. */
#define KZU_HUD_LOG_STORED_STRING_COUNT 50
/** Maximum custom strings to HUD. */
#define KZU_HUD_MAX_CUSTOM_TEXTS 16


struct KzuHud
{
    kzUint fpsBuffer[KZU_HUD_BUFFER_SIZE]; /**< Buffer for old fps data. */
    kzUint batchBuffer[KZU_HUD_BUFFER_SIZE]; /**< Buffer for old batch data. */

    kzUint lastAddedIndex; /**< Index of last added fps. */
    kzUint lastFrameFps; /**< Last frame fps. */
    kzUint lastFrameBatchCount; /**< Last frame batch count. */
    kzUint lastFrameTriangleCount; /**< Last frame triangle count. */
    kzUint lastFrameTextureSwitches; /**< Last frame shader switches. */
    kzUint lastFrameShaderSwitches; /**< Last frame shader switches. */
    kzFloat animationPlayerTime; /**< Current time of the scene animation player. */
    
    kzMutableString infoViewCamera; /**< Name of current view camera. */

    struct KzcFont* font; /**< Font used for rendering the hud. */
    struct KzcBitmapFontSystem* bitmapFontSystem; /**< Bitmap font system. */
    struct KzcTexture* fontTexture; /**< Texture used for font. */
    struct KzcTextLayouter* textLayouter; /**< Text layouter. */

    kzMutableString customTexts[KZU_HUD_MAX_CUSTOM_TEXTS]; /**< Custom texts for HUD. */
    kzUint customTextCount; /**< Number of custom texts in HUD. */

    kzMutableString* logStrings; /**< Strings written to log. */
    kzUint lastLogMessageIndex; /**< Last index of string stored. */
};


/** Draws hud text. */
static kzsError kzuHudDrawText_internal(struct KzcFont* font, const struct KzcTextLayouter* textLayouter, const struct KzcMemoryManager* memoryManager,
                                 struct KzcRenderer* renderer, kzString text, kzFloat x, kzFloat y, kzFloat* out_ySpace);
/** Get the maximum number of items stored in the log message buffer. */
static kzUint kzuHudLogGetMaximumMessageCount_internal(void);
/** Gets item from the log message buffer. 0 is oldest stored. If index is not in valid range KZ_NULL is returned. */
static kzMutableString kzuHudLogGetStoredMessage_internal(const struct KzuHud* hud, kzUint index);
/** A callback for system log handler. Stores given message to message buffer. */
static kzsError kzuHudLogStoreMessage_callback(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line, void* userData);
/** Helper for drawing string with one integer parameter. */
static kzsError kzuHudDrawIntegerString(const struct KzcMemoryManager* quickMemoryManager, struct KzcFont* font, 
                                        const struct KzcTextLayouter* textLayouter, struct KzcRenderer* renderer, 
                                        kzFloat x, kzFloat y, kzString format, kzUint integer);
/** Helper function for calculating average values from stored frames. */
static void kzuHudCalculateHudBufferValues(const kzUint* buffer, kzUint* out_lowest, kzUint* out_highest, kzUint* out_average);
/** Draw meter box. */
static kzsError kzuHudRenderBox(struct KzuRenderer* renderer, kzFloat fpsBoxStartX, kzFloat fpsBoxStartY, const kzFloat boxSizeX, 
                                const kzFloat boxSizeY, const kzFloat margin, kzUint highest, kzUint lastAddedIndex, const kzUint* fpsBuffer);



kzsError kzuHudCreate(const struct KzcMemoryManager* memoryManager, struct KzuHud** out_hud)
{
    kzUint i;
    kzsError result;
    struct KzuHud* hud;

    result = kzcMemoryAllocVariable(memoryManager, hud, "Hud");
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, hud->logStrings, KZU_HUD_LOG_STORED_STRING_COUNT, "Hud log buffer");
    kzsErrorForward(result);

    for(i = 0; i < KZU_HUD_LOG_STORED_STRING_COUNT; ++i)
    {
        hud->logStrings[i] = KZ_NULL;
    }

    hud->lastLogMessageIndex = 0;

    result = kzsLogRegisterHandler(kzuHudLogStoreMessage_callback, hud);
    kzsErrorForward(result);

    for(i = 0; i < KZU_HUD_BUFFER_SIZE; ++i)
    {
        hud->fpsBuffer[i] = 0;
        hud->batchBuffer[i] = 0;
    }

    hud->lastAddedIndex = 0;
    hud->lastFrameFps = 0;
    hud->lastFrameBatchCount = 0;
    hud->lastFrameTriangleCount = 0;
    hud->lastFrameShaderSwitches = 0;
    hud->lastFrameTextureSwitches = 0;
    hud->customTextCount = 0;

    hud->infoViewCamera = KZ_NULL;

    *out_hud = hud;
    kzsSuccess();
}

kzsError kzuHudCreateResources(struct KzuHud* hud, struct KzcResourceManager* resourceManager, struct KzcBitmapFontSystem* fontSystem)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcIsValidPointer(hud));

    memoryManager = kzcMemoryGetManager(hud);

    {
        struct KzcBitmapFont* bitmapFont;
        struct KzcImage* image;

        result = kzcImageLoadMemory(memoryManager, KZU_HUD_DATA_FONT_IMAGE_WIDTH, KZU_HUD_DATA_FONT_IMAGE_HEIGHT, 
            KZC_IMAGE_DATA_FORMAT_ALPHA_8, kzuHudDataFontImage, &image);
        kzsErrorForward(result);

        result = kzcTextureCreateFromImage(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, KZC_TEXTURE_FILTER_POINT_SAMPLE,
            KZC_TEXTURE_WRAP_CLAMP, KZC_TEXTURE_COMPRESSION_NONE, image, &hud->fontTexture);
        kzsErrorForward(result);

        hud->bitmapFontSystem = fontSystem;

        result = kzcBitmapFontCreateFromMemory(memoryManager, hud->bitmapFontSystem, resourceManager, kzuHudDataFontDescriptor, &bitmapFont);
        kzsErrorForward(result);

        result = kzcBitmapFontSetTexture(bitmapFont, hud->fontTexture, 0);
        kzsErrorForward(result);

        result = kzcImageDelete(image);
        kzsErrorForward(result);

        hud->font = kzcBitmapFontToFont(bitmapFont);

        result = kzcTextLayouterCreate(memoryManager, hud->font, &hud->textLayouter);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuHudDelete(struct KzuHud* hud)
{
    kzUint i;
    kzsError result;

    kzsAssert(kzcIsValidPointer(hud));

    kzsLogUnregisterHandler(kzuHudLogStoreMessage_callback);

    for(i = 0; i < KZU_HUD_LOG_STORED_STRING_COUNT; ++i)
    {
        if(hud->logStrings[i] != KZ_NULL)
        {
            result = kzcStringDelete(hud->logStrings[i]);
            kzsErrorForward(result);
        }
    }

    for(i = 0; i < hud->customTextCount; ++i)
    {
        result = kzcStringDelete(hud->customTexts[i]);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeArray(hud->logStrings);
    kzsErrorForward(result);

    result = kzcTextLayouterDelete(hud->textLayouter);
    kzsErrorForward(result);

    result = kzcFontDelete(hud->font);
    kzsErrorForward(result);

    /* TODO: Delete texture. */

    if(hud->infoViewCamera != KZ_NULL)
    {
        result = kzcStringDelete(hud->infoViewCamera);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(hud);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuHudUpdate(struct KzuHud* hud, kzUint fps, kzUint batchCount, kzUint triangleCount, kzUint textureSwitchCount, 
                      kzUint shaderSwitchCount, kzString viewCameraPath, kzFloat animationPlayerTime)
{
    kzsError result;
    kzUint i;
    kzsAssert(kzcIsValidPointer(hud));

    kzsAssert(hud->lastAddedIndex < KZU_HUD_BUFFER_SIZE);


    for(i = 0; i < hud->customTextCount; ++i)
    {
        result = kzcStringDelete(hud->customTexts[i]);
        kzsErrorForward(result);
    }
    hud->customTextCount = 0;

    hud->fpsBuffer[hud->lastAddedIndex] = fps;
    hud->batchBuffer[hud->lastAddedIndex] = batchCount;

    hud->lastAddedIndex++;
    hud->lastAddedIndex = hud->lastAddedIndex % KZU_HUD_BUFFER_SIZE;

    hud->lastFrameFps = fps;
    hud->lastFrameBatchCount = batchCount;
    hud->lastFrameTriangleCount = triangleCount;
    hud->lastFrameShaderSwitches = shaderSwitchCount;
    hud->lastFrameTextureSwitches = textureSwitchCount;
    hud->animationPlayerTime = animationPlayerTime;

    if (viewCameraPath != KZ_NULL)
    {
        if (hud->infoViewCamera == KZ_NULL || !kzcStringIsEqual(viewCameraPath, hud->infoViewCamera))
        {
            if (hud->infoViewCamera != KZ_NULL)
            {
                result = kzcStringDelete(hud->infoViewCamera);
                kzsErrorForward(result);
            }
            result = kzcStringCopy(kzcMemoryGetManager(hud), viewCameraPath, &hud->infoViewCamera);
            kzsErrorForward(result);
        }
    }
    else
    {
        hud->infoViewCamera = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzuHudRenderLog(const struct KzuHud* hud, const struct KzuRenderer* renderer, 
                         const struct KzcMemoryManager* quickMemoryManager, const struct KzsWindow* window)
{
    kzsError result;
    const kzFloat xMargin = 15.0f;
    kzFloat x, y, width, height;
    struct KzcRenderer* coreRenderer;
    kzFloat lineHeight;
    kzFloat yTop;

    kzsAssert(window != KZ_NULL);
    kzsAssert(kzcIsValidPointer(renderer));
    kzsAssert(kzcIsValidPointer(hud));

    coreRenderer = kzuRendererGetCoreRenderer(renderer);
    kzuRendererGetAdjustedViewport(renderer, window, &x, &y, &width, &height);
    yTop = y + height;

    result = kzcFontSetColor(hud->font, KZC_COLOR_WHITE);
    kzsErrorForward(result);

    kzcTextLayouterSetHorizontalAlignment(hud->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_LEFT);
    kzcTextLayouterSetVerticalAlignment(hud->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_TOP);

    kzcRendererSetBlendMode(coreRenderer, KZC_RENDERER_BLEND_MODE_OPAQUE);  

    kzcTextLayouterSetMaximumWidth(hud->textLayouter, width - xMargin * 2.0f);

    result = kzcFontGetLineHeight(hud->font, &lineHeight);
    kzsErrorForward(result);

    result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, "Recent log messages", x + xMargin, yTop - lineHeight, KZ_NULL);
    kzsErrorForward(result);

    {
        kzFloat ySpace; 
        kzInt ty = (kzInt)yTop; 
        kzInt i;
        kzUint count = kzuHudLogGetMaximumMessageCount_internal();
        kzFloat freeSpace = height - (lineHeight + 3.0f) * 3;
        kzInt maxLines = kzsMin((kzInt)(freeSpace / lineHeight), (kzInt)count);
        kzInt drawCount = 0;
        
        for(i = 0; i < maxLines; ++i)
        {
            struct KzcTextLayout* textLayout;
            kzMutableString logString = kzuHudLogGetStoredMessage_internal(hud, (kzUint)i);
            if(logString != KZ_NULL && kzcStringLength(logString) > 0)
            {
                result = kzcTextLayouterCreateLayout(hud->textLayouter, quickMemoryManager, logString, &textLayout);
                kzsErrorForward(result);

                freeSpace -= textLayout->height;

                result = kzcTextLayoutDelete(textLayout);
                kzsErrorForward(result);

                if(freeSpace > 0)
                {
                    ++drawCount;
                }
                else
                {
                    break;
                }
            }
        }
        
        for(i = drawCount - 1; i >= 0; --i)
        {
            kzString logString = kzuHudLogGetStoredMessage_internal(hud, (kzUint)i);
            if(logString != KZ_NULL)
            {
                result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, logString, 
                    x + xMargin * 1.5f, ty - lineHeight * 3.0f, &ySpace);
                kzsErrorForward(result); 
                ty -= (kzInt)ySpace;
            }
        }
    }

    kzsSuccess();
}

kzsError kzuHudRenderDebugInfo(const struct KzuHud* hud, struct KzuRenderer* renderer, const struct KzcMemoryManager* quickMemoryManager, const struct KzsWindow* window)
{
    kzsError result;
    const kzFloat margin = 5.0f;
    const kzFloat boxWidth = 100.0f;
    const kzFloat boxHeight = 100.0f;
    kzFloat x, y, width, height;
    kzFloat fpsBoxStartX;
    kzFloat fpsBoxStartY;
    kzFloat batchBoxStartX;
    kzFloat batchBoxStartY;
    kzFloat lineHeight;
    struct KzcRenderer* coreRenderer;
    kzFloat textHeadersX;
    kzFloat textParagraphsX;
    kzUint highestBatchCount = 0;
    kzUint highestFps = 0;
    kzFloat yTop;
    kzFloat currentY;

    kzsAssert(window != KZ_NULL);
    kzsAssert(kzcIsValidPointer(renderer));
    kzsAssert(kzcIsValidPointer(hud));

    coreRenderer = kzuRendererGetCoreRenderer(renderer);
    kzuRendererGetAdjustedViewport(renderer, window, &x, &y, &width, &height);
    yTop = y + height;

    fpsBoxStartX = x + width - boxWidth - margin;
    fpsBoxStartY = y + margin;
    batchBoxStartX = fpsBoxStartX;
    batchBoxStartY = fpsBoxStartY + boxHeight + margin * 3.0f;
    textHeadersX = x + margin;
    textParagraphsX = x + margin * 2.5f;

    result = kzcFontSetColor(hud->font, KZC_COLOR_WHITE);
    kzsErrorForward(result);

    kzcRendererSetBlendMode(coreRenderer, KZC_RENDERER_BLEND_MODE_OPAQUE);

    result = kzcFontGetLineHeight(hud->font, &lineHeight);
    kzsErrorForward(result);

    kzcTextLayouterSetHorizontalAlignment(hud->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_LEFT);
    kzcTextLayouterSetVerticalAlignment(hud->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_TOP);

    currentY = yTop - lineHeight;

    result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textHeadersX, currentY, "Statistics for last %i frames", KZU_HUD_BUFFER_SIZE);
    kzsErrorForward(result);
    currentY -= 3 * lineHeight;

    result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, "Frames Per Second", textHeadersX, currentY, KZ_NULL);
    kzsErrorForward(result);
    currentY -= lineHeight;

    result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Last frame: %i", hud->lastFrameFps);
    kzsErrorForward(result);
    currentY -= lineHeight;

    {
        kzUint lowestFps;
        kzUint averageFps;
        
        kzuHudCalculateHudBufferValues(hud->fpsBuffer, &lowestFps, &highestFps, &averageFps);

        result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Average: %i", averageFps);
        kzsErrorForward(result);
        currentY -= lineHeight;

        result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Highest: %i", highestFps);
        kzsErrorForward(result);
        currentY -= lineHeight;

        result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Lowest: %i", lowestFps);
        kzsErrorForward(result);
        currentY -= 3.0f * lineHeight;
    }

    result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, "Batch Count", textHeadersX, currentY, KZ_NULL);
    kzsErrorForward(result);
    currentY -= lineHeight;

    result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Last frame: %i", hud->lastFrameBatchCount);
    kzsErrorForward(result);
    currentY -= lineHeight;

    /* Batch count. */
    {
        kzUint lowestBatchCount;
        kzUint averageBatchCount;

        kzuHudCalculateHudBufferValues(hud->batchBuffer, &lowestBatchCount, &highestBatchCount, &averageBatchCount);

        result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Average: %i", averageBatchCount);
        kzsErrorForward(result);
        currentY -= lineHeight;

        result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Highest: %i", highestBatchCount);
        kzsErrorForward(result);
        currentY -= lineHeight;

        result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Lowest: %i", lowestBatchCount);
        kzsErrorForward(result);
        currentY -= lineHeight * 3.0f;
    }

    /* Triangle count. */
    result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, "Triangle Count", textHeadersX, currentY, KZ_NULL);
    kzsErrorForward(result);
    currentY -= lineHeight;

    result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Last frame: %i", hud->lastFrameTriangleCount);
    kzsErrorForward(result);
    currentY -= lineHeight * 3.0f;

    result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, "Texture Switches", textHeadersX, currentY, KZ_NULL);
    kzsErrorForward(result);
    currentY -= lineHeight;

    result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Last frame: %i", hud->lastFrameTextureSwitches);
    kzsErrorForward(result);
    currentY -= lineHeight * 3.0f;

    result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, "Shader Switches", textHeadersX, currentY, KZ_NULL);
    kzsErrorForward(result);
    currentY -= lineHeight;

    result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, textParagraphsX, currentY, "Last frame: %i", hud->lastFrameShaderSwitches);
    kzsErrorForward(result);
    currentY -= lineHeight * 3.0f;

    /* View camera. */
    if(hud->infoViewCamera != KZ_NULL)
    {
        result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, "View camera", textHeadersX, currentY, KZ_NULL);
        kzsErrorForward(result);
        currentY -= lineHeight;
        result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, hud->infoViewCamera, textParagraphsX, currentY, KZ_NULL);
        kzsErrorForward(result);
    }
    currentY -= lineHeight * 3.0f;

    /* Animation player time. */
    {
        result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, "Animation player", textHeadersX, currentY, KZ_NULL);
        kzsErrorForward(result);
        currentY -= lineHeight;
        {
            kzMutableString info;
            result = kzcStringFormat(quickMemoryManager, "Time: %f", &info, hud->animationPlayerTime);
            kzsErrorForward(result);
            result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, info, textParagraphsX, currentY, KZ_NULL);
            kzsErrorForward(result);
            result = kzcStringDelete(info);
            kzsErrorForward(result);
        }
    }

    /* Batch meter text. */
    kzcTextLayouterSetHorizontalAlignment(hud->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_LEFT);
    kzcTextLayouterSetVerticalAlignment(hud->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_BOTTOM);
    result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, "Batch'meter", batchBoxStartX, batchBoxStartY + boxHeight + 2, KZ_NULL);
    kzsErrorForward(result);

    kzcTextLayouterSetHorizontalAlignment(hud->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_RIGHT);
    kzcTextLayouterSetVerticalAlignment(hud->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_TOP);
    result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, batchBoxStartX - 1, batchBoxStartY + boxHeight, "%i", hud->lastFrameBatchCount); 
    kzsErrorForward(result);

    /* Fps meter text. */
    kzcTextLayouterSetHorizontalAlignment(hud->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_LEFT);
    kzcTextLayouterSetVerticalAlignment(hud->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_BOTTOM);
    result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, "FPS'meter", fpsBoxStartX, fpsBoxStartY + boxHeight + 2, KZ_NULL);
    kzsErrorForward(result);

    kzcTextLayouterSetHorizontalAlignment(hud->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_RIGHT);
    kzcTextLayouterSetVerticalAlignment(hud->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_TOP);
    result = kzuHudDrawIntegerString(quickMemoryManager, hud->font, hud->textLayouter, coreRenderer, batchBoxStartX - 1, fpsBoxStartY + boxHeight, "%i", hud->lastFrameFps);
    kzsErrorForward(result);

    /* Custom texts. */
    {
        kzUint i;

        for(i = 0; i < hud->customTextCount; ++i)
        {
            kzcTextLayouterSetHorizontalAlignment(hud->textLayouter, KZC_TEXT_HORIZONTAL_ALIGNMENT_RIGHT);
            kzcTextLayouterSetVerticalAlignment(hud->textLayouter, KZC_TEXT_VERTICAL_ALIGNMENT_TOP);
            result = kzuHudDrawText_internal(hud->font, hud->textLayouter, quickMemoryManager, coreRenderer, hud->customTexts[i], fpsBoxStartX + boxWidth, yTop - (kzFloat)i * 16.0f - 16.0f, KZ_NULL);
            kzsErrorForward(result);
        }
    }

    /* Render meter boxes */
    {
        kzFloat viewport[4];

        struct KzcMatrix4x4 originalProjection = kzcRendererGetMatrix(coreRenderer, KZC_RENDERER_MATRIX_PROJECTION);
        struct KzcMatrix4x4 originalWorld = kzcRendererGetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD);
        struct KzcMatrix4x4 originalCamera = kzcRendererGetMatrix(coreRenderer, KZC_RENDERER_MATRIX_CAMERA);
        struct KzcMatrix4x4 projection;

        kzcRendererSetViewport(coreRenderer, 0, 0, kzsWindowGetWidth(window), kzsWindowGetHeight(window));

        /* Setup material */
        kzcRendererBeginMaterialSetup(coreRenderer);
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
        kzcRendererSetEmissiveColor(coreRenderer, KZC_COLOR_WHITE);
#endif
        kzcRendererSetBlendMode(coreRenderer, KZC_RENDERER_BLEND_MODE_ALPHA);
        kzcRendererSetCullMode(coreRenderer, KZC_RENDERER_CULL_MODE_NONE);
        kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_DEPTH_TEST);
        kzcRendererDisableState(coreRenderer, KZC_RENDERER_STATE_DEPTH_WRITE);
        kzcRendererEndMaterialSetup(coreRenderer);

        /* Setup viewport */
        kzcRendererGetViewport(coreRenderer, &viewport[0], &viewport[1], &viewport[2], &viewport[3]);
        kzcMatrix4x4CreateOrthogonalProjection(&projection, viewport[0], viewport[0] + viewport[2], 
            viewport[1], viewport[1] + viewport[3], -1.0f, 1.0f);

        /* Setup matrices */
        kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_PROJECTION, &projection);
        kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_CAMERA, &KZC_MATRIX4X4_IDENTITY);
        kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD, &KZC_MATRIX4X4_IDENTITY);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_PROJECTION_CAMERA_WORLD, &projection);
#endif
        result = kzcRendererApplyTransformation(coreRenderer);
        kzsErrorForward(result);

        result = kzuHudRenderBox(renderer, fpsBoxStartX, fpsBoxStartY, boxWidth, boxHeight, margin, highestFps, hud->lastAddedIndex, hud->fpsBuffer);
        kzsErrorForward(result);
        result = kzuHudRenderBox(renderer, batchBoxStartX, batchBoxStartY, boxWidth, boxHeight, margin, highestBatchCount, hud->lastAddedIndex, hud->batchBuffer);
        kzsErrorForward(result);

        /* Restore matrices */
        kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_PROJECTION, &originalProjection);
        kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD, &originalWorld);
        kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_CAMERA, &originalCamera); 
    }

    kzsSuccess();
}

static kzsError kzuHudRenderBox(struct KzuRenderer* renderer, kzFloat boxStartX, kzFloat boxStartY, const kzFloat boxSizeX, const kzFloat boxSizeY,
                                const kzFloat margin, kzUint highest, kzUint lastAddedIndex, const kzUint* buffer)
{
    kzsError result;
    struct KzcRenderer* coreRenderer = kzuRendererGetCoreRenderer(renderer);

    /* Disable index and vertex buffers */
    kzcRendererSetBuffer(coreRenderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);
    kzcRendererSetBuffer(coreRenderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);

    /* Fps meter */
    {
        kzFloat positions[KZU_HUD_BUFFER_SIZE * 3];
        /* Fps meter box */
        {
            kzUint offset = 0;
            positions[offset++] = boxStartX + boxSizeX; 
            positions[offset++] = boxStartY;
            positions[offset++] = 0.0f;
            positions[offset++] = boxStartX + 1.0f;
            positions[offset++] = boxStartY;
            positions[offset++] = 0.0f;
            positions[offset++] = boxStartX + 1.0f;
            positions[offset++] = boxStartY + boxSizeY + 1.0f; 
            positions[offset++] = 0.0f;
            positions[offset++] = boxStartX + boxSizeX; 
            positions[offset++] = boxStartY + boxSizeY + 1.0f; 
            positions[offset++] = 0.0f;
            positions[offset++] = boxStartX + boxSizeX; 
            positions[offset++] = boxStartY;
            positions[offset++] = 0.0f;

            result = kzuRendererSetSolidColor(renderer, &KZC_COLOR_WHITE);
            kzsErrorForward(result);
            result = kzuRendererApplyMaterial(renderer, kzuRendererGetSolidColorMaterial(renderer));
            kzsErrorForward(result);

            kzcRendererBeginVertexArray(coreRenderer, KZC_VERTEX_ARRAY_POSITION);
            kzcRendererSetVertexArrayData(coreRenderer, KZC_VERTEX_ARRAY_POSITION, positions);
            kzcRendererEndVertexArray(coreRenderer, KZC_PRIMITIVE_TYPE_LINE_STRIP, 5);
        }

        /* Meter */
        {
            kzUint i;
            kzFloat marginBox = 2.0f;

            for(i = 0; i < KZU_HUD_BUFFER_SIZE; ++i)
            {
                kzUint id = i * 3;
                kzUint fetchId = (lastAddedIndex + i + 1) % KZU_HUD_BUFFER_SIZE;
                kzUint fps = buffer[fetchId];
                positions[id] = boxStartX + marginBox + i * (boxSizeX - marginBox * 2) / (kzFloat)(KZU_HUD_BUFFER_SIZE - 1);
                positions[id + 1] = boxStartY + margin - marginBox * 2 + fps * (boxSizeY - marginBox * 2) / (kzFloat)highest;
                positions[id + 2] = 0.0f;
            }

            kzcRendererBeginVertexArray(coreRenderer, KZC_VERTEX_ARRAY_POSITION);
            kzcRendererSetVertexArrayData(coreRenderer, KZC_VERTEX_ARRAY_POSITION, positions);
            kzcRendererEndVertexArray(coreRenderer, KZC_PRIMITIVE_TYPE_LINE_STRIP, KZU_HUD_BUFFER_SIZE);

            { 
                result = kzuRendererSetSolidColor(renderer, &KZC_COLOR_BLACK);
                kzsErrorForward(result);
                result = kzuRendererApplyMaterial(renderer, kzuRendererGetSolidColorMaterial(renderer));
                kzsErrorForward(result);
            }

            /* Make another stroke of the line to emphasize it. */
            for(i = 0; i < KZU_HUD_BUFFER_SIZE; ++i)
            {
                kzUint index = i * 3;
                positions[index] += 1.0f;
                positions[index + 1] -= 1.0f;
            }

            kzcRendererBeginVertexArray(coreRenderer, KZC_VERTEX_ARRAY_POSITION);
            kzcRendererSetVertexArrayData(coreRenderer, KZC_VERTEX_ARRAY_POSITION, positions);
            kzcRendererEndVertexArray(coreRenderer, KZC_PRIMITIVE_TYPE_LINE_STRIP, KZU_HUD_BUFFER_SIZE);
        }
    }
    kzsSuccess();
}

static kzsError kzuHudDrawIntegerString(const struct KzcMemoryManager* quickMemoryManager, struct KzcFont* font, 
                                        const struct KzcTextLayouter* textLayouter, struct KzcRenderer* renderer, 
                                        kzFloat x, kzFloat y, kzString format, kzUint integer)
{
    kzsError result;
    kzMutableString info;

    result = kzcStringFormat(quickMemoryManager, format, &info, integer);
    kzsErrorForward(result);
    result = kzuHudDrawText_internal(font, textLayouter, quickMemoryManager, renderer, info, x, y, KZ_NULL);
    kzsErrorForward(result);
    result = kzcStringDelete(info);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzuHudDrawText_internal(struct KzcFont* font, const struct KzcTextLayouter* textLayouter, const struct KzcMemoryManager* memoryManager,
                                 struct KzcRenderer* renderer, kzString text, kzFloat x, kzFloat y, kzFloat* out_ySpace)
{
    kzsError result;
    kzFloat spaceY;
    struct KzcTextLayout* textLayout;

    result = kzcTextLayouterCreateLayout(textLayouter, memoryManager, text, &textLayout);
    kzsErrorForward(result);
    spaceY = textLayout->height;

    result = kzcFontSetColor(font, KZC_COLOR_BLACK);
    kzsErrorForward(result);
    result = kzcFontDrawTextLayout(textLayout, renderer, x, y);
    kzsErrorForward(result);
    
    result = kzcFontSetColor(font, KZC_COLOR_WHITE);
    kzsErrorForward(result);
    result = kzcFontDrawTextLayout(textLayout, renderer, x + 1, y + 1);
    kzsErrorForward(result);

    result = kzcTextLayoutDelete(textLayout);
    kzsErrorForward(result);

    if(out_ySpace != KZ_NULL)
    {
        *out_ySpace = spaceY;
    }

    kzsSuccess();
}

kzsError kzuHudAddCustomTextLine(struct KzuHud* hud, kzString customString)
{
    kzsError result;

    if(hud->customTextCount < KZU_HUD_MAX_CUSTOM_TEXTS)
    {
        result = kzcStringCopy(kzcMemoryGetManager(hud), customString, &hud->customTexts[hud->customTextCount]);
        kzsErrorForward(result);
        ++hud->customTextCount;
    }

    kzsSuccess();
}

static kzUint kzuHudLogGetMaximumMessageCount_internal()
{
    return KZU_HUD_LOG_STORED_STRING_COUNT;
}

static kzMutableString kzuHudLogGetStoredMessage_internal(const struct KzuHud* hud, kzUint index)
{
    kzUint messageCount = kzuHudLogGetMaximumMessageCount_internal();
    kzMutableString storedMessage;

    if(index >= messageCount)
    {
        storedMessage = KZ_NULL;
    }
    else
    {
        kzInt id = (kzInt)hud->lastLogMessageIndex + (kzInt)(messageCount - index);
        id %= (kzInt)messageCount;
        kzsAssert(id >= 0 && id < KZU_HUD_LOG_STORED_STRING_COUNT);
        storedMessage = hud->logStrings[id];
    }
    return storedMessage;
}

static kzsError kzuHudLogStoreMessage_callback(KzsLogLevel level, kzsError errorCode, kzString message, kzString file, kzUint line, void* userData)
{
    kzsError result;
    struct KzuHud* hud;
    kzString prefix;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcIsValidPointer(userData));

    hud = (struct KzuHud*)userData;
    memoryManager = kzcMemoryGetManager(hud);

    hud->lastLogMessageIndex++;
    if(hud->lastLogMessageIndex >= KZU_HUD_LOG_STORED_STRING_COUNT)
    {
        hud->lastLogMessageIndex = 0;
    }

    /* Remove old string if exists. */
    if(hud->logStrings[hud->lastLogMessageIndex] != KZ_NULL)
    {
        result = kzcStringDelete(hud->logStrings[hud->lastLogMessageIndex]);
        kzsErrorForward(result);
    }

    switch (level)
    {
        case KZS_LOG_LEVEL_ERROR:
        {
            prefix = "ERROR";
            break;
        }
        case KZS_LOG_LEVEL_WARNING:
        {
            prefix = "WARNING";
            break;
        }
        default:
        {
            prefix = "";
            break;
        }
    }

    {
        kzMutableString formattedMessage;
        if (errorCode == KZS_SUCCESS)
        {
            if(kzcStringLength(prefix) > 0)
            {
                result = kzcStringFormat(memoryManager, "%s: %s\n", &formattedMessage, prefix, message);
                kzsErrorForward(result);
            }
            else
            {
                result = kzcStringFormat(memoryManager, "%s\n", &formattedMessage, message);
                kzsErrorForward(result);
            }
        }
        else
        {
            result = kzcStringFormat(memoryManager, "%s %i: %s [%s:%u]\n", &formattedMessage, prefix, errorCode, message, file, line);
            kzsErrorForward(result);
        }
        hud->logStrings[hud->lastLogMessageIndex] = formattedMessage;
    }


    kzsSuccess();
}

static void kzuHudCalculateHudBufferValues(const kzUint* buffer, kzUint* out_lowest, kzUint* out_highest, kzUint* out_average)
{
    kzUint i;
    kzUint lowest = KZ_UINT_MAXIMUM;
    kzUint highest = 0;
    kzUint average = 0;
    kzUint calculated = 0;

    for(i = 0; i < KZU_HUD_BUFFER_SIZE; ++i)
    {
        if(buffer[i] > 0)
        {
            ++calculated;
            average += buffer[i];
            lowest = kzsMinU(buffer[i], lowest);
            highest = kzsMaxU(buffer[i], highest);
        }
    }

    if(calculated > 0)
    {
        average = (kzUint)(average / (kzFloat)calculated);
    }
    else
    {
        lowest = 0;
    }
    *out_lowest = lowest;
    *out_highest = highest;
    *out_average = average;
}

struct KzcFont* kzuHudGetFont(const struct KzuHud* hud)
{
    kzsAssert(kzcIsValidPointer(hud));
    return hud->font;
}
