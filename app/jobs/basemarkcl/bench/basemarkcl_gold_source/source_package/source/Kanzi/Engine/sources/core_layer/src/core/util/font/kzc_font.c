/**
* \file
* Font rendering functionality.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_font.h"
#include "kzc_font_private.h"

#include "kzc_text_layout.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/debug/kzc_log.h>

#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_opengl_base.h>


kzsError kzcFontCreate_private(const struct KzcMemoryManager* memoryManager, const struct KzcFontClass* fontClass,
                               struct KzcFontData* fontData, struct KzcFont** out_font)
{
    kzsError result;
    struct KzcFont* font;

    result = kzcMemoryAllocVariable(memoryManager, font, "Font");
    kzsErrorForward(result);

    font->data = fontData;
    fontData->selfOwned = KZ_TRUE;
    fontData->fontClass = fontClass;
    fontData->targetTexture = KZ_NULL;

    *out_font = font;
    kzsSuccess();
}

kzsError kzcFontDeleteBase_private(struct KzcFont* font)
{
    kzsError result;
    struct KzcFontData* fontData;
    kzBool selfOwned;

    kzsAssert(kzcFontIsValid(font));

    fontData = font->data;
    selfOwned = fontData->selfOwned;

    result = kzcMemoryFreeVariable(fontData);
    kzsErrorForward(result);

    if (selfOwned)
    {
        result = kzcMemoryFreeVariable(font);
        kzsErrorForward(result);
    }
    else
    {
        font->data = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzcFontDelete(struct KzcFont* font)
{
    kzsError result;

    kzsAssert(kzcFontIsValid(font));

    result = font->data->fontClass->kzcFontDelete(font);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcFontSetSize(struct KzcFont* font, kzFloat size)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(font));

    result = font->data->fontClass->kzcFontSetSize(font, size);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcFontGetSize(struct KzcFont* font, kzFloat* out_size)
{
    kzsError result;
    kzFloat size;

    kzsAssert(kzcIsValidPointer(font));

    result = font->data->fontClass->kzcFontGetSize(font, &size);
    kzsErrorForward(result);

    *out_size = size;
    kzsSuccess();
}

kzsError kzcFontSetColor(struct KzcFont* font, struct KzcColorRGBA color)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(font));

    result = font->data->fontClass->kzcFontSetColor(font, color);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcFontGetColor(struct KzcFont* font, struct KzcColorRGBA* out_color)
{
    kzsError result;
    struct KzcColorRGBA color;

    kzsAssert(kzcIsValidPointer(font));

    result = font->data->fontClass->kzcFontGetColor(font, &color);
    kzsErrorForward(result);

    *out_color = color;
    kzsSuccess();
}

kzsError kzcFontGetLineHeight(struct KzcFont* font, kzFloat* out_lineHeight)
{
    kzsError result;
    kzFloat lineHeight;

    kzsAssert(kzcIsValidPointer(font));

    result = font->data->fontClass->kzcFontGetLineHeight(font, &lineHeight);
    kzsErrorForward(result);

    *out_lineHeight = lineHeight;
    kzsSuccess();
}

kzsError kzcFontGetAscender(struct KzcFont* font, kzFloat* out_ascender)
{
    kzsError result;
    kzFloat ascender;

    kzsAssert(kzcIsValidPointer(font));

    result = font->data->fontClass->kzcFontGetAscender(font, &ascender);
    kzsErrorForward(result);

    *out_ascender = ascender;
    kzsSuccess();
}

kzsError kzcFontGetCharacterMetrics(struct KzcFont* font, kzUnicodeChar character, kzFloat* out_advance, struct KzcRectangle* out_boundingBox)
{
    kzsError result;
    kzFloat advance;

    kzsAssert(kzcFontIsValid(font));

    result = font->data->fontClass->kzcFontGetCharacterMetrics(font, character, &advance, out_boundingBox);
    kzsErrorForward(result);

    *out_advance = advance;
    kzsSuccess();
}

kzsError kzcFontGetKerning(struct KzcFont* font, kzUnicodeChar previousCharacter, kzUnicodeChar character, kzFloat* out_kerning)
{
    kzsError result;
    kzFloat kerning;

    kzsAssert(kzcFontIsValid(font));

    result = font->data->fontClass->kzcFontGetKerning(font, previousCharacter, character, &kerning);
    kzsErrorForward(result);

    *out_kerning = kerning;
    kzsSuccess();
}

kzsError kzcFontPrepareTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(textLayout));
    kzsAssert(kzcIsValidPointer(textLayout->font));

    result = textLayout->font->data->fontClass->kzcFontPrepareTextLayout(textLayout, renderer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcFontDrawTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(textLayout));
    kzsAssert(kzcIsValidPointer(textLayout->font));

    result = textLayout->font->data->fontClass->kzcFontDrawTextLayout(textLayout, renderer, x, y);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcFontDrawTextLayout3D(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(textLayout));
    kzsAssert(kzcIsValidPointer(textLayout->font));

    result = textLayout->font->data->fontClass->kzcFontDrawTextLayout3D(textLayout, renderer);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcFontFreeTextLayoutData(struct KzcTextLayout* textLayout)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(textLayout));
    kzsAssert(kzcIsValidPointer(textLayout->font));

    result = textLayout->font->data->fontClass->kzcFontFreeTextLayoutData(textLayout);
    kzsErrorForward(result);

    kzsSuccess();
}

KzcFontType kzcFontGetType(const struct KzcFont* font)
{
    kzsAssert(kzcFontIsValid(font));

    return (KzcFontType)font->data->fontClass;
}

void kzcFontSetTargetTexture(const struct KzcFont* font, struct KzcTexture* texture)
{
    kzsAssert(kzcIsValidPointer(font));
    font->data->targetTexture = texture;
}

void kzcFontTakeOwnership(const struct KzcFont* font)
{
    kzsAssert(kzcFontIsValid(font));
    font->data->selfOwned = KZ_FALSE;
}

kzBool kzcFontIsValid(const struct KzcFont* font)
{
    return font != KZ_NULL && font->data != KZ_NULL;
}

void kzcFontTransferData(struct KzcFont* targetFont, struct KzcFont* sourceFont)
{
    kzsAssert(kzcIsValidPointer(targetFont));
    kzsAssert(targetFont->data == KZ_NULL);
    kzsAssert(kzcFontIsValid(sourceFont));

    targetFont->data = sourceFont->data;
    sourceFont->data = KZ_NULL;
}
