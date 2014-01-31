/**
* \file
* Configured font. This font implementation wraps an existing font with given font configuration (size, color).
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_configured_font.h"

#include "kzc_font_private.h"
#include "kzc_font.h"
#include "kzc_text_layout.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/color/kzc_color.h>
#include <core/util/math/kzc_rectangle.h>


#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_string.h>
#include <system/wrappers/kzs_math.h>


struct KzcConfiguredFont
{
    struct KzcFont font; /**< Font. Used for inheritance. */
};

/** Configured font data structure. */
struct KzcConfiguredFontData
{
    struct KzcFontData fontData; /**< Font data. Used for inheritance. */ /*lint -esym(754, KzcConfiguredFontData::fontData) Suppressed as this member is required but not referenced. */ 

    struct KzcFont* targetFont; /**< Target font wrapped by this configured font. */
    kzFloat size; /**< Height of font. */
    struct KzcColorRGBA color; /**< Current drawing color of the font. */
};


/** Font class for configured font. */
extern const struct KzcFontClass KZC_CONFIGURED_FONT_CLASS;

const KzcFontType KZC_FONT_TYPE_CONFIGURED = &KZC_CONFIGURED_FONT_CLASS;


/** Helper macro for getting the data structure of a configured font. */
#define kzcConfiguredFontGetData_internal(configuredFont_param) ((struct KzcConfiguredFontData*)configuredFont_param->font.data)




kzsError kzcConfiguredFontCreate(const struct KzcMemoryManager* memoryManager, struct KzcFont* targetFont,
                                 kzFloat size, struct KzcColorRGBA color, struct KzcConfiguredFont** out_configuredFont)
{
    kzsError result;
    struct KzcFont* font;
    struct KzcConfiguredFontData* configuredFontData;

    result = kzcMemoryAllocVariable(memoryManager, configuredFontData, "Configured font");
    kzsErrorForward(result);

    result = kzcFontCreate_private(memoryManager, &KZC_CONFIGURED_FONT_CLASS, &configuredFontData->fontData, &font);
    kzsErrorForward(result);

    configuredFontData->targetFont = targetFont;
    configuredFontData->size = size;
    configuredFontData->color = color;

    *out_configuredFont = kzcConfiguredFontFromFont(font);
    kzsSuccess();
}

kzsError kzcConfiguredFontDelete(struct KzcConfiguredFont* configuredFont)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(configuredFont));

    result = kzcFontDeleteBase_private(&configuredFont->font);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcConfiguredFont* kzcConfiguredFontFromFont(struct KzcFont* font)
{
    return (struct KzcConfiguredFont*)font;
}

struct KzcFont* kzcConfiguredFontToFont(struct KzcConfiguredFont* configuredFont)
{
    return (struct KzcFont*)configuredFont;
}

static kzsError kzcConfiguredFontApplySize_internal(const struct KzcConfiguredFont* configuredFont)
{
    kzsError result;
    struct KzcConfiguredFontData* configuredFontData;
    
    kzsAssert(kzcIsValidPointer(configuredFont));
    
    configuredFontData = kzcConfiguredFontGetData_internal(configuredFont);

    result = kzcFontSetSize(configuredFontData->targetFont, configuredFontData->size);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzcConfiguredFontApplyColor_internal(const struct KzcConfiguredFont* configuredFont)
{
    kzsError result;
    struct KzcConfiguredFontData* configuredFontData;
    
    kzsAssert(kzcIsValidPointer(configuredFont));
    
    configuredFontData = kzcConfiguredFontGetData_internal(configuredFont);

    result = kzcFontSetColor(configuredFontData->targetFont, configuredFontData->color);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcConfiguredFontSetSize(const struct KzcConfiguredFont* configuredFont, kzFloat size)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(configuredFont));

    kzcConfiguredFontGetData_internal(configuredFont)->size = size;

    result = kzcConfiguredFontApplySize_internal(configuredFont);
    kzsErrorForward(result);

    kzsSuccess();
}

kzFloat kzcConfiguredFontGetSize(const struct KzcConfiguredFont* configuredFont)
{
    kzsAssert(kzcIsValidPointer(configuredFont));

    return kzcConfiguredFontGetData_internal(configuredFont)->size;
}

kzsError kzcConfiguredFontSetColor(const struct KzcConfiguredFont* configuredFont, struct KzcColorRGBA color)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(configuredFont));

    kzcConfiguredFontGetData_internal(configuredFont)->color = color;

    result = kzcFontSetColor(kzcConfiguredFontGetData_internal(configuredFont)->targetFont, color);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcColorRGBA kzcConfiguredFontGetColor(const struct KzcConfiguredFont* configuredFont)
{
    kzsAssert(kzcIsValidPointer(configuredFont));

    return kzcConfiguredFontGetData_internal(configuredFont)->color;
}

kzsError kzcConfiguredFontGetLineHeight(const struct KzcConfiguredFont* configuredFont, kzFloat* out_lineHeight)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(configuredFont));

    result = kzcConfiguredFontApplySize_internal(configuredFont);
    kzsErrorForward(result);

    result = kzcFontGetLineHeight(kzcConfiguredFontGetData_internal(configuredFont)->targetFont, out_lineHeight);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcConfiguredFontGetAscender(const struct KzcConfiguredFont* configuredFont, kzFloat* out_ascender)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(configuredFont));

    result = kzcConfiguredFontApplySize_internal(configuredFont);
    kzsErrorForward(result);

    result = kzcFontGetAscender(kzcConfiguredFontGetData_internal(configuredFont)->targetFont, out_ascender);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcConfiguredFontGetCharacterMetrics(const struct KzcConfiguredFont* configuredFont, kzUnicodeChar character,
                                              kzFloat* out_advance, struct KzcRectangle* out_boundingBox)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(configuredFont));

    result = kzcConfiguredFontApplySize_internal(configuredFont);
    kzsErrorForward(result);

    result = kzcFontGetCharacterMetrics(kzcConfiguredFontGetData_internal(configuredFont)->targetFont, character, out_advance, out_boundingBox);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcConfiguredFontGetKerning(const struct KzcConfiguredFont* configuredFont, kzUnicodeChar previousCharacter,
                                     kzUnicodeChar character, kzFloat* out_kerning)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(configuredFont));

    result = kzcConfiguredFontApplySize_internal(configuredFont);
    kzsErrorForward(result);

    result = kzcFontGetKerning(kzcConfiguredFontGetData_internal(configuredFont)->targetFont, previousCharacter, character, out_kerning);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcConfiguredFontPrepareTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result;
    struct KzcFont* font;
    struct KzcConfiguredFont* configuredFont;
    struct KzcConfiguredFontData* configuredFontData;

    kzsAssert(kzcIsValidPointer(textLayout));

    font = textLayout->font;
    configuredFont = kzcConfiguredFontFromFont(font);
    kzsAssert(kzcIsValidPointer(configuredFont));

    result = kzcConfiguredFontApplySize_internal(configuredFont);
    kzsErrorForward(result);

    configuredFontData = kzcConfiguredFontGetData_internal(configuredFont);

    /* Replace the text layout font temporarily. */
    textLayout->font = configuredFontData->targetFont;

    result = kzcFontPrepareTextLayout(textLayout, renderer);
    kzsErrorForward(result);

    textLayout->font = font;

    kzsSuccess();
}
 
kzsError kzcConfiguredFontDrawTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y)
{
    kzsError result;
    struct KzcFont* font;
    struct KzcConfiguredFont* configuredFont;
    struct KzcConfiguredFontData* configuredFontData;

    kzsAssert(kzcIsValidPointer(textLayout));

    font = textLayout->font;
    configuredFont = kzcConfiguredFontFromFont(font);
    kzsAssert(kzcIsValidPointer(configuredFont));

    result = kzcConfiguredFontApplySize_internal(configuredFont);
    kzsErrorForward(result);

    result = kzcConfiguredFontApplyColor_internal(configuredFont);
    kzsErrorForward(result);

    configuredFontData = kzcConfiguredFontGetData_internal(configuredFont);

    /* Replace the text layout font temporarily. */
    textLayout->font = configuredFontData->targetFont;

    result = kzcFontDrawTextLayout(textLayout, renderer, x, y);
    kzsErrorForward(result);

    textLayout->font = font;

    kzsSuccess();
}

kzsError kzcConfiguredFontDrawTextLayout3D(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result;
    struct KzcFont* font;
    struct KzcConfiguredFont* configuredFont;
    struct KzcConfiguredFontData* configuredFontData;

    kzsAssert(kzcIsValidPointer(textLayout));

    font = textLayout->font;
    configuredFont = kzcConfiguredFontFromFont(font);
    kzsAssert(kzcIsValidPointer(configuredFont));

    result = kzcConfiguredFontApplySize_internal(configuredFont);
    kzsErrorForward(result);

    result = kzcConfiguredFontApplyColor_internal(configuredFont);
    kzsErrorForward(result);

    configuredFontData = kzcConfiguredFontGetData_internal(configuredFont);

    /* Replace the text layout font temporarily. */
    textLayout->font = configuredFontData->targetFont;

    result = kzcFontDrawTextLayout3D(textLayout, renderer);
    kzsErrorForward(result);

    textLayout->font = font;

    kzsSuccess();
}

kzsError kzcConfiguredFontFreeTextLayoutData(struct KzcTextLayout* textLayout)
{
    kzsError result;
    struct KzcFont* font;
    struct KzcConfiguredFont* configuredFont;
    struct KzcConfiguredFontData* configuredFontData;

    kzsAssert(kzcIsValidPointer(textLayout));

    font = textLayout->font;
    configuredFont = kzcConfiguredFontFromFont(font);
    kzsAssert(kzcIsValidPointer(configuredFont));
    configuredFontData = kzcConfiguredFontGetData_internal(configuredFont);

    /* Replace the text layout font temporarily. */
    textLayout->font = configuredFontData->targetFont;

    result = kzcFontFreeTextLayoutData(textLayout);
    kzsErrorForward(result);

    textLayout->font = font;

    kzsSuccess();
}


/** FontDelete implementation for configured fonts. */
static kzsError kzcConfiguredFontDeleteImplementation_internal(struct KzcFont* font)
{
    kzsError result = kzcConfiguredFontDelete(kzcConfiguredFontFromFont(font));
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontSetSize implementation for configured fonts. */
static kzsError kzcConfiguredFontSetSizeImplementation_internal(struct KzcFont* font, kzFloat size)
{
    kzsError result = kzcConfiguredFontSetSize(kzcConfiguredFontFromFont(font), size);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontGetSize implementation for configured fonts. */
static kzsError kzcConfiguredFontGetSizeImplementation_internal(struct KzcFont* font, kzFloat* out_size)
{
    *out_size = kzcConfiguredFontGetSize(kzcConfiguredFontFromFont(font));
    kzsSuccess();
}

/** FontSetColor implementation for configured fonts. */
static kzsError kzcConfiguredFontSetColorImplementation_internal(struct KzcFont* font, struct KzcColorRGBA color)
{
    kzsError result = kzcConfiguredFontSetColor(kzcConfiguredFontFromFont(font), color);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontGetColor implementation for configured fonts. */
static kzsError kzcConfiguredFontGetColorImplementation_internal(struct KzcFont* font, struct KzcColorRGBA* out_color)
{
    *out_color = kzcConfiguredFontGetColor(kzcConfiguredFontFromFont(font));
    kzsSuccess();
}

/** FontGetLineHeight implementation for configured fonts. */
static kzsError kzcConfiguredFontGetLineHeightImplementation_internal(struct KzcFont* font, kzFloat* out_lineHeight)
{
    kzsError result = kzcConfiguredFontGetLineHeight(kzcConfiguredFontFromFont(font), out_lineHeight);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontGetAscender implementation for configured fonts. */
static kzsError kzcConfiguredFontGetAscenderImplementation_internal(struct KzcFont* font, kzFloat* out_ascender)
{
    kzsError result = kzcConfiguredFontGetAscender(kzcConfiguredFontFromFont(font), out_ascender);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontGetCharacterAdvance implementation for configured fonts. */
static kzsError kzcConfiguredFontGetCharacterMetricsImplementation_internal(struct KzcFont* font, kzUnicodeChar character,
                                                                            kzFloat* out_advance, struct KzcRectangle* out_boundingBox)
{
    kzsError result = kzcConfiguredFontGetCharacterMetrics(kzcConfiguredFontFromFont(font), character, out_advance, out_boundingBox);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontGetKerning implementation for configured fonts. */
static kzsError kzcConfiguredFontGetKerningImplementation_internal(struct KzcFont* font, kzUnicodeChar previousCharacter,
                                                                   kzUnicodeChar character, kzFloat* out_kerning)
{
    kzsError result = kzcConfiguredFontGetKerning(kzcConfiguredFontFromFont(font), previousCharacter, character, out_kerning);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontPrepareTextLayout implementation for configured fonts. */
static kzsError kzcConfiguredFontPrepareTextLayoutImplementation_internal(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result = kzcConfiguredFontPrepareTextLayout(textLayout, renderer);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontDrawTextLayout implementation for configured fonts. */
static kzsError kzcConfiguredFontDrawTextLayoutImplementation_internal(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer,
                                                                       kzFloat x, kzFloat y)
{
    kzsError result = kzcConfiguredFontDrawTextLayout(textLayout, renderer, x, y);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontDrawTextLayout3D implementation for configured fonts. */
static kzsError kzcConfiguredFontDrawTextLayout3DImplementation_internal(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result = kzcConfiguredFontDrawTextLayout3D(textLayout, renderer);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontFreeTextLayoutData implementation for configured fonts. */
static kzsError kzcConfiguredFontFreeTextLayoutDataImplementation_internal(struct KzcTextLayout* textLayout)
{
    kzsError result = kzcConfiguredFontFreeTextLayoutData(textLayout);
    kzsErrorForward(result);
    kzsSuccess();
}

const struct KzcFontClass KZC_CONFIGURED_FONT_CLASS =
{
    kzcConfiguredFontDeleteImplementation_internal,
    kzcConfiguredFontSetSizeImplementation_internal,
    kzcConfiguredFontGetSizeImplementation_internal,
    kzcConfiguredFontSetColorImplementation_internal,
    kzcConfiguredFontGetColorImplementation_internal,
    kzcConfiguredFontGetLineHeightImplementation_internal,
    kzcConfiguredFontGetAscenderImplementation_internal,
    kzcConfiguredFontGetCharacterMetricsImplementation_internal,
    kzcConfiguredFontGetKerningImplementation_internal,
    kzcConfiguredFontPrepareTextLayoutImplementation_internal,
    kzcConfiguredFontDrawTextLayoutImplementation_internal,
    kzcConfiguredFontDrawTextLayout3DImplementation_internal,
    kzcConfiguredFontFreeTextLayoutDataImplementation_internal
};
