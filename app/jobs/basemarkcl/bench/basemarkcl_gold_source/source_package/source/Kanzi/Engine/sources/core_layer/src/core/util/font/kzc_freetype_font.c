/**
* \file
* FreeType font.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_freetype_font.h"
#include "kzc_font.h"

#include "kzc_font_private.h"
#include "kzc_text_layout.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/string/kzc_string.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/math/kzc_rectangle.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/kzc_error_codes.h>
#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/shader/kzc_resource_shader.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/kzc_resource_manager.h>

#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_opengl_base.h>

#include <ft2build.h>
/*lint -save*/ /* PC-lint mistakenly reports an error from FreeType headers. Suppress it. */
/*lint --e{10}*/
#include FT_FREETYPE_H
/*lint -restore*/
#include <freetype/ftmodapi.h>


struct KzcFreetypeSystem
{
    struct KzcTruetypeSystem truetypeSystem; /**< Base object. */
    struct KzcResourceManager* resourceManager;
    FT_Memory memory; /** FreeType memory manager. */
    FT_Library library; /**< FreeType state object. */
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    struct KzcShader* fontShader; /**< Shader used for rendering characters with FreeType font. */
#endif
};

struct KzcFreetypeFont
{
    struct KzcFont font; /**< Font. Used for inheritance. */
};

/** FreeType font data structure. */
struct KzcFreetypeFontData
{
    struct KzcFontData fontData; /**< Font data. Used for inheritance. */ /*lint -esym(754, KzcFreetypeFontData::fontData) Suppressed as this member is required but not referenced. */ 
    struct KzcFreetypeSystem* system; /**< Reference to FreeType font system. */
    kzByte* memoryData; /**< Data of the font if loaded from memory. KZ_NULL if loaded from a file.*/
    FT_Face face; /**< Font face object. */
    kzBool useIntegerCoordinates; /**< Flag to enable rounding to integer coordinates. This improves rendering quality for axis-aligned 2D rendering. Default: enabled. */
    kzFloat size; /**< Size of the font. */
    kzFloat lineHeight; /**< Line height of the font. */
    kzFloat ascender; /**< Ascender of the font. */
    struct KzcColorRGBA color; /**< Current drawing color of the font. */
};

/** Data needed for rendering text layout quickly. Calculated in prepare. */
struct KzcFreetypeFontTextRenderData
{
    struct KzcRenderer* renderer; /**< Renderer used for rendering the text. */
    struct KzcTexture* texture; /**< Texture used in freetype font. */

    kzFloat* vertexCoordinates; /**< Vertex coordinates of each character. */
    kzFloat* vertexTextureCoordinates; /**< Vertex texture coordinates of each character. */
};


/** Font class for FreeType font. */
extern const struct KzcFontClass KZC_FREETYPE_FONT_CLASS;

const KzcFontType KZC_FONT_TYPE_FREETYPE = &KZC_FREETYPE_FONT_CLASS;


/** Helper macro for getting the data structure of an FreeType font. */
#define kzcFreetypeFontGetData_internal(freetypeFont_param) ((struct KzcFreetypeFontData*)freetypeFont_param->font.data)


KZ_CALLBACK static void* kzcFreetypeMalloc_internal(FT_Memory memory, /*lint -e{970}*/ long size)
{
    kzsError result;
    const struct KzcMemoryManager* memoryManager = (struct KzcMemoryManager*)memory->user;
    void* block;

    result = kzcMemoryAllocPointer(memoryManager, &block, (kzUint)size, "FreeType memory");
    kzsErrorIf(result)
    {
        block = KZ_NULL;
    }

    return block;
}

KZ_CALLBACK static void kzcFreetypeFree_internal(FT_Memory memory, void* block)
{
    kzsError result;

    result = kzcMemoryFreePointer(block);
    KZ_UNUSED_RETURN_VALUE(result); /* Ignore, as the call-back can't handle errors. */
}

KZ_CALLBACK static void* kzcFreetypeRealloc_internal(FT_Memory memory, /*lint -e{970}*/ long curSize, /*lint -e{970}*/ long newSize, void* block)
{
    kzsError result;
    void* newBlock;

    result = kzcMemoryReallocPointer(block, (kzUint)curSize, (kzUint)newSize, &newBlock);
    kzsErrorIf(result)
    {
        newBlock = KZ_NULL;
    }

    return newBlock;
}

static kzsError kzcFreetypeSystemDeletor_internal(struct KzcTruetypeSystem* truetypeSystem)
{
    kzsError result;
    struct KzcFreetypeSystem* freetypeSystem = kzcFreetypeSystemFromTruetypeSystem(truetypeSystem);

    result = kzcFreetypeSystemDelete(freetypeSystem);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError kzcFreetypeFontFileCreator_internal(const struct KzcMemoryManager* memoryManager, struct KzcTruetypeSystem* truetypeSystem,
                                                    kzString filePath, struct KzcFont** out_font)
{
    kzsError result;
    struct KzcFreetypeFont* freetypeFont;
    struct KzcFreetypeSystem* freetypeSystem = kzcFreetypeSystemFromTruetypeSystem(truetypeSystem);

    result = kzcFreetypeFontCreateFromFile(memoryManager, freetypeSystem, filePath, &freetypeFont);
    kzsErrorForward(result);

    *out_font = kzcFreetypeFontToFont(freetypeFont);
    kzsSuccess();
}

static kzsError kzcFreetypeFontMemoryCreator_internal(const struct KzcMemoryManager* memoryManager, struct KzcTruetypeSystem* truetypeSystem,
                                                      const kzByte* data, kzUint size, struct KzcFont** out_font)
{
    kzsError result;
    struct KzcFreetypeFont* freetypeFont;
    struct KzcFreetypeSystem* freetypeSystem = kzcFreetypeSystemFromTruetypeSystem(truetypeSystem);

    result = kzcFreetypeFontCreateFromMemory(memoryManager, freetypeSystem, data, size, &freetypeFont);
    kzsErrorForward(result);

    *out_font = kzcFreetypeFontToFont(freetypeFont);
    kzsSuccess();
}

kzsError kzcFreetypeSystemCreate(const struct KzcMemoryManager* memoryManager, struct KzcResourceManager* resourceManager,
                                 struct KzcFreetypeSystem** out_freetypeSystem)
{
    kzsError result;
    struct KzcFreetypeSystem* freetypeSystem;

    result = kzcMemoryAllocVariable(memoryManager, freetypeSystem, "FreeType font system");
    kzsErrorForward(result);

    result = kzcTruetypeSystemInitialize_private(kzcFreetypeSystemToTruetypeSystem(freetypeSystem), kzcFreetypeSystemDeletor_internal,
                                                 kzcFreetypeFontFileCreator_internal, kzcFreetypeFontMemoryCreator_internal);
    kzsErrorForward(result);

    /* Initialize FreeType */
    {
        FT_Error ftResult;

        result = kzcMemoryAllocVariable(memoryManager, freetypeSystem->memory, "FreeType memory manager");
        kzsErrorForward(result);

        freetypeSystem->memory->user = (struct KzcMemoryManager*)memoryManager;
        freetypeSystem->memory->alloc = kzcFreetypeMalloc_internal;
        freetypeSystem->memory->free = kzcFreetypeFree_internal;
        freetypeSystem->memory->realloc = kzcFreetypeRealloc_internal;

        ftResult = FT_New_Library(freetypeSystem->memory, &freetypeSystem->library);
        kzsErrorTest(ftResult == 0, KZC_ERROR_FREETYPE_FAILED, "Failed to initialize FreeType font system");

        FT_Add_Default_Modules(freetypeSystem->library);
    }

    freetypeSystem->resourceManager = resourceManager;

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    freetypeSystem->fontShader = KZ_NULL;
#endif

    *out_freetypeSystem = freetypeSystem;
    kzsSuccess();
}

kzsError kzcFreetypeSystemDelete(struct KzcFreetypeSystem* freetypeSystem)
{
    kzsError result;
    FT_Error ftResult;

    kzsAssert(kzcIsValidPointer(freetypeSystem));

    ftResult = FT_Done_Library(freetypeSystem->library);
    kzsErrorTest(ftResult == 0, KZC_ERROR_FREETYPE_FAILED, "Failed to delete FreeType system");

    result = kzcMemoryFreeVariable(freetypeSystem->memory);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(freetypeSystem);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Sets shader for freetype system. */
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
void kzcFreetypeSystemSetShader(struct KzcFreetypeSystem* freetypeSystem, struct KzcShader* shader)
{
    kzsAssert(kzcIsValidPointer(freetypeSystem));
    freetypeSystem->fontShader = shader;
}
#endif

struct KzcFreetypeSystem* kzcFreetypeSystemFromTruetypeSystem(struct KzcTruetypeSystem* truetypeSystem)
{
    return (struct KzcFreetypeSystem*)truetypeSystem;
}

struct KzcTruetypeSystem* kzcFreetypeSystemToTruetypeSystem(struct KzcFreetypeSystem* freetypeSystem)
{
    return &freetypeSystem->truetypeSystem;
}

/** Creates the basic structure for an FreeType font, but does not initialize it. */
static kzsError kzcFreetypeFontCreate_internal(const struct KzcMemoryManager* memoryManager, struct KzcFreetypeSystem* freetypeSystem,
                                               struct KzcFreetypeFont** out_freetypeFont)
{
    kzsError result;
    struct KzcFont* font;
    struct KzcFreetypeFontData* freetypeFontData;

    result = kzcMemoryAllocVariable(memoryManager, freetypeFontData, "FreeType font");
    kzsErrorForward(result);

    result = kzcFontCreate_private(memoryManager, &KZC_FREETYPE_FONT_CLASS, &freetypeFontData->fontData, &font);
    kzsErrorForward(result);

    freetypeFontData->system = freetypeSystem;
    freetypeFontData->color = KZC_COLOR_WHITE;
    freetypeFontData->useIntegerCoordinates = KZ_TRUE;

    *out_freetypeFont = kzcFreetypeFontFromFont(font);
    kzsSuccess();
}

/* TODO: Implement */
/*kzsError kzcFreetypeFontCreateFromResource(const struct KzcMemoryManager* memoryManager, kzString resourcePath,
                                        struct KzcFreetypeFont** out_freetypeFont)
{
    kzsError result;
    struct KzcFreetypeFont* freetypeFont;
    kzMutableString *lines;
    
    result = kzcFileReadTextResource(memoryManager, resourcePath, &lines);
    kzsErrorForward(result);

    result = kzcFreetypeFontCreate_internal(memoryManager, lines, &freetypeFont);
    kzsErrorForward(result);

    *out_freetypeFont = freetypeFont;
    kzsSuccess();
}*/

kzsError kzcFreetypeFontCreateFromFile(const struct KzcMemoryManager* memoryManager, struct KzcFreetypeSystem* freetypeSystem,
                                       kzString filePath, struct KzcFreetypeFont** out_freetypeFont)
{
    kzsError result;
    struct KzcFreetypeFont* freetypeFont;
    struct KzcFreetypeFontData* freetypeFontData;

    kzsAssert(kzcIsValidPointer(freetypeSystem));

    result = kzcFreetypeFontCreate_internal(memoryManager, freetypeSystem, &freetypeFont);
    kzsErrorForward(result);

    freetypeFontData = kzcFreetypeFontGetData_internal(freetypeFont);

    {
        FT_Error ftResult;

        freetypeFontData->memoryData = KZ_NULL;

        ftResult = FT_New_Face(freetypeSystem->library, filePath, 0, &freetypeFontData->face);
        kzsErrorTest(ftResult == 0, KZC_ERROR_FREETYPE_FAILED, "Failed to load FreeType font from file");
    }

    *out_freetypeFont = freetypeFont;
    kzsSuccess();
}

kzsError kzcFreetypeFontCreateFromMemory(const struct KzcMemoryManager* memoryManager, struct KzcFreetypeSystem* freetypeSystem,
                                         const kzByte* data, kzUint size, struct KzcFreetypeFont** out_freetypeFont)
{
    kzsError result;
    struct KzcFreetypeFont* freetypeFont;
    struct KzcFreetypeFontData* freetypeFontData;

    kzsAssert(kzcIsValidPointer(freetypeSystem));

    result = kzcFreetypeFontCreate_internal(memoryManager, freetypeSystem, &freetypeFont);
    kzsErrorForward(result);

    freetypeFontData = kzcFreetypeFontGetData_internal(freetypeFont);

    {
        FT_Error ftResult;

        result = kzcMemoryAllocArray(memoryManager, freetypeFontData->memoryData, size, "FreeType font memory data");
        kzsErrorForward(result);

        kzsMemcpy(freetypeFontData->memoryData, data, size);

        ftResult = FT_New_Memory_Face(freetypeSystem->library, (FT_Byte*)freetypeFontData->memoryData, (FT_Long)size, 0, &freetypeFontData->face);
        kzsErrorTest(ftResult == 0, KZC_ERROR_FREETYPE_FAILED, "Failed to load FreeType font from memory");
    }

    result = kzcFontSetSize(kzcFreetypeFontToFont(freetypeFont), 10.0f);
    kzsErrorForward(result);

    *out_freetypeFont = freetypeFont;
    kzsSuccess();
}

kzsError kzcFreetypeFontDelete(struct KzcFreetypeFont* freetypeFont)
{
    kzsError result;
    struct KzcFreetypeFontData* freetypeFontData;

    kzsAssert(kzcIsValidPointer(freetypeFont));

    freetypeFontData = kzcFreetypeFontGetData_internal(freetypeFont);

    if (freetypeFontData->memoryData != KZ_NULL)
    {
        result = kzcMemoryFreeArray(freetypeFontData->memoryData);
        kzsErrorForward(result);
    }

    result = kzcFontDeleteBase_private(&freetypeFont->font);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcFreetypeFont* kzcFreetypeFontFromFont(struct KzcFont* font)
{
    return (struct KzcFreetypeFont*)font;
}

struct KzcFont* kzcFreetypeFontToFont(struct KzcFreetypeFont* freetypeFont)
{
    return (struct KzcFont*)freetypeFont;
}

kzsError kzcFreetypeFontSetSize(const struct KzcFreetypeFont* freetypeFont, kzFloat size)
{
    FT_Error ftResult;
    struct KzcFreetypeFontData* freetypeFontData;

    kzsAssert(kzcIsValidPointer(freetypeFont));

    freetypeFontData = kzcFreetypeFontGetData_internal(freetypeFont);

    ftResult = FT_Set_Char_Size(freetypeFontData->face, 0, (FT_F26Dot6)(size * 64), 0, 0);
    kzsErrorTest(ftResult == 0, KZC_ERROR_FREETYPE_FAILED, "Failed to set size for FreeType font");

    freetypeFontData->size = size;

    if (freetypeFontData->useIntegerCoordinates)
    {
        freetypeFontData->ascender = (kzFloat)(kzInt)(freetypeFontData->face->size->metrics.ascender / 64);
        freetypeFontData->lineHeight = (kzFloat)(kzInt)(freetypeFontData->face->size->metrics.height / 64);
    }
    else
    {
        freetypeFontData->ascender = (kzFloat)freetypeFontData->face->size->metrics.ascender / 64;
        freetypeFontData->lineHeight = (kzFloat)freetypeFontData->face->size->metrics.height / 64;
    }

    kzsSuccess();
}

kzFloat kzcFreetypeFontGetSize(const struct KzcFreetypeFont* freetypeFont)
{
    kzsAssert(kzcIsValidPointer(freetypeFont));

    return kzcFreetypeFontGetData_internal(freetypeFont)->size;
}

void kzcFreetypeFontSetColor(const struct KzcFreetypeFont* freetypeFont, struct KzcColorRGBA color)
{
    kzsAssert(kzcIsValidPointer(freetypeFont));

    kzcFreetypeFontGetData_internal(freetypeFont)->color = color;
}

struct KzcColorRGBA kzcFreetypeFontGetColor(const struct KzcFreetypeFont* freetypeFont)
{
    kzsAssert(kzcIsValidPointer(freetypeFont));

    return kzcFreetypeFontGetData_internal(freetypeFont)->color;
}

kzFloat kzcFreetypeFontGetLineHeight(const struct KzcFreetypeFont* freetypeFont)
{
    kzsAssert(kzcIsValidPointer(freetypeFont));

    return kzcFreetypeFontGetData_internal(freetypeFont)->lineHeight;
}
                              
kzFloat kzcFreetypeFontGetAscender(const struct KzcFreetypeFont* freetypeFont)
{
    kzsAssert(kzcIsValidPointer(freetypeFont));

    return kzcFreetypeFontGetData_internal(freetypeFont)->ascender;
}

kzsError kzcFreetypeFontGetCharacterMetrics(const struct KzcFreetypeFont* freetypeFont, kzUnicodeChar character,
                                            kzFloat* out_advanceX, kzFloat* out_advanceY, struct KzcRectangle* out_boundingBox)
{
    FT_Error ftResult;
    struct KzcFreetypeFontData* freetypeFontData;
    kzFloat advanceX;
    kzFloat advanceY;
    struct KzcRectangle boundingBox;

    kzsAssert(kzcIsValidPointer(freetypeFont));

    freetypeFontData = kzcFreetypeFontGetData_internal(freetypeFont);

    ftResult = FT_Load_Char(freetypeFontData->face, (FT_ULong)character, FT_LOAD_DEFAULT);
    kzsErrorTest(ftResult != 36, KZC_ERROR_FREETYPE_FAILED, "Failed to get character metrics from FreeType. Font size was not set."); 
    kzsErrorTest(ftResult == 0, KZC_ERROR_FREETYPE_FAILED, "Failed to get character metrics from FreeType");

    if (freetypeFontData->useIntegerCoordinates)
    {
        advanceX = (kzFloat)(kzInt)(freetypeFontData->face->glyph->advance.x / 64);
        advanceY = (kzFloat)(kzInt)(freetypeFontData->face->glyph->advance.y / 64);
        boundingBox.x = (kzFloat)(kzInt)freetypeFontData->face->glyph->metrics.horiBearingX / 64;
        boundingBox.width = (kzFloat)(kzInt)freetypeFontData->face->glyph->metrics.width / 64;
        boundingBox.height = (kzFloat)(kzInt)freetypeFontData->face->glyph->metrics.height/ 64;
        boundingBox.y = (kzFloat)(kzInt)freetypeFontData->face->glyph->metrics.horiBearingY / 64 - boundingBox.height;
    }
    else
    {
        advanceX = (kzFloat)freetypeFontData->face->glyph->advance.x / 64;
        advanceY = (kzFloat)freetypeFontData->face->glyph->advance.y / 64;
        boundingBox.x = (kzFloat)freetypeFontData->face->glyph->metrics.horiBearingX / 64;
        boundingBox.width = (kzFloat)freetypeFontData->face->glyph->metrics.width / 64;
        boundingBox.height = (kzFloat)freetypeFontData->face->glyph->metrics.height/ 64;
        boundingBox.y = (kzFloat)freetypeFontData->face->glyph->metrics.horiBearingY / 64 - boundingBox.height;
    }

    *out_advanceX = advanceX;
    *out_advanceY = advanceY;
    *out_boundingBox = boundingBox;
    kzsSuccess();
}

kzsError kzcFreetypeFontGetKerning(const struct KzcFreetypeFont* freetypeFont, kzUnicodeChar previousCharacter,
                                   kzUnicodeChar character, kzFloat* out_kerningX, kzFloat* out_kerningY)
{
    FT_Error ftResult;
    struct KzcFreetypeFontData* freetypeFontData;
    FT_Vector kerning;
    kzFloat kerningX;
    kzFloat kerningY;
    FT_Kerning_Mode kerningMode;

    kzsAssert(kzcIsValidPointer(freetypeFont));

    freetypeFontData = kzcFreetypeFontGetData_internal(freetypeFont);

    kerningMode = freetypeFontData->useIntegerCoordinates ? FT_KERNING_DEFAULT : FT_KERNING_UNFITTED;

    ftResult = FT_Get_Kerning(freetypeFontData->face, (FT_UInt)previousCharacter, (FT_UInt)character,
                              /*lint -e{930}*/(FT_UInt)kerningMode, &kerning);
    kzsErrorTest(ftResult == 0, KZC_ERROR_FREETYPE_FAILED, "Failed to get kerning for characters from FreeType");

    if (freetypeFontData->useIntegerCoordinates)
    {
        kerningX = (kzFloat)(kzInt)(kerning.x / 64);
        kerningY = (kzFloat)(kzInt)(kerning.y / 64);
    }
    else
    {
        kerningX = (kzFloat)kerning.x / 64;
        kerningY = (kzFloat)kerning.y / 64;
    }

    *out_kerningX = kerningX;
    *out_kerningY = kerningY;
    kzsSuccess();
}

/** Rounds the given value to the next power of two. */
static kzUint kzcFreetypeFontRoundToPowerOfTwo_internal(kzUint value)
{
    kzUint newValue = 1;

    while (newValue < value)
    {
        newValue <<= 1;
    }

    return newValue;
}

kzsError kzcFreetypeFontPrepareTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result;
    FT_Error ftResult;
    struct KzcFreetypeFont* freetypeFont;
    struct KzcFreetypeFontData* freetypeFontData;
    struct KzcFreetypeFontTextRenderData* renderData;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcIsValidPointer(textLayout));

    freetypeFont = kzcFreetypeFontFromFont(textLayout->font);

    kzsAssert(kzcIsValidPointer(freetypeFont));
    kzsAssert(textLayout->font->data->fontClass == &KZC_FREETYPE_FONT_CLASS);

    freetypeFontData = kzcFreetypeFontGetData_internal(freetypeFont);

    memoryManager = kzcMemoryGetManager(textLayout);

    result = kzcMemoryAllocVariable(memoryManager, renderData, "FreeType font text render data");
    kzsErrorForward(result);

    renderData->renderer = renderer;

    {
        kzUint i;
        kzFloat* positions;
        kzFloat* uvs;
        kzUint textureWidth = kzcFreetypeFontRoundToPowerOfTwo_internal((kzUint)kzsCeilf(textLayout->width));
        kzUint textureHeight = kzcFreetypeFontRoundToPowerOfTwo_internal((kzUint)kzsCeilf(textLayout->height));
        kzByte* textureData;
        struct KzcTextureDescriptor descriptor;

        result = kzcMemoryAllocArray(memoryManager, textureData, textureWidth * textureHeight, "Initial black texture");
        kzsErrorForward(result);

        kzsMemset(textureData, 0, textureWidth * textureHeight);

        kzcTextureDescriptorSet(textureWidth, textureHeight, KZC_TEXTURE_FORMAT_ALPHA, KZC_TEXTURE_FILTER_BILINEAR, KZC_TEXTURE_WRAP_CLAMP,
            KZC_TEXTURE_COMPRESSION_NONE, &descriptor);
        result = kzcTextureCreate(freetypeFontData->system->resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, &descriptor, textureData, &renderData->texture);
        kzsErrorForward(result);

        /* 6 vertices per character quad */
        result = kzcMemoryAllocArray(memoryManager, positions, 6 * 3,
                                     "FreeType font rendering vertex coordinates");
        kzsErrorForward(result);
        result = kzcMemoryAllocArray(memoryManager, uvs, 6 * 2,
                                     "FreeType font rendering vertex texture coordinates");
        kzsErrorForward(result);

        renderData->vertexCoordinates = positions;
        renderData->vertexTextureCoordinates = uvs;

        for (i = 0; i < textLayout->characterCount; ++i)
        {
            struct KzcCharacterLayout* character = &textLayout->characters[i];
            FT_GlyphSlot glyph;
            kzUint width;
            kzUint height;
            kzFloat left;
            kzFloat bottom;
 
            ftResult = FT_Load_Char(freetypeFontData->face, (FT_ULong)character->character, FT_LOAD_RENDER);
            kzsErrorTest(ftResult != 36, KZC_ERROR_FREETYPE_FAILED, "Failed to prepare text layout with FreeType. Font size was not set.");
            kzsErrorTest(ftResult == 0, KZC_ERROR_FREETYPE_FAILED, "Failed to prepare text layout with FreeType");

            glyph = freetypeFontData->face->glyph;

            width = (kzUint)(kzInt)glyph->bitmap.width;
            height = (kzUint)(kzInt)glyph->bitmap.rows;

            left = character->x - textLayout->left + (kzFloat)(kzInt)glyph->bitmap_left;
            bottom = -character->y + textLayout->bottom + textLayout->height - (kzFloat)(kzInt)glyph->bitmap_top;

            if (textLayout->overlaps)
            {
                kzUint x, y;
                const kzByte* source = (const kzByte*)glyph->bitmap.buffer;
                kzByte* target = textureData + (kzUint)bottom * textureWidth + (kzInt)left;
                for (y = 0; y < height; ++y, target += textureWidth - width)
                {
                    for (x = 0; x < width; ++x, ++target, ++source)
                    {
                        /* Bitwise OR is not graphically perfect way of combining overlapping pixels, but produces good enough results fast enough. */
                        *target |= *source;
                    }
                }
            }
            else
            {
                result = kzcTextureUpdateSubData(renderData->texture, glyph->bitmap.buffer, (kzUint)left, (kzUint)bottom, (kzUint)width, (kzUint)height);
                kzsErrorForward(result);
            }
        }

        {
            kzFloat x1 = textLayout->left;
            kzFloat y1 = textLayout->bottom;
            kzFloat x2 = x1 + textLayout->width;
            kzFloat y2 = y1 + textLayout->height;
            kzFloat u1 = 0.0f;
            kzFloat v1 = 0.0f;
            kzFloat u2 = (kzFloat)textLayout->width / textureWidth;
            kzFloat v2 = (kzFloat)textLayout->height / textureHeight;
            kzUint positionOffset = 0;
            kzUint uvOffset = 0;

            positions[positionOffset++] = x1; positions[positionOffset++] = y2; positions[positionOffset++] = 0.0f;
            positions[positionOffset++] = x1; positions[positionOffset++] = y1; positions[positionOffset++] = 0.0f;
            positions[positionOffset++] = x2; positions[positionOffset++] = y2; positions[positionOffset++] = 0.0f;
            positions[positionOffset++] = x1; positions[positionOffset++] = y1; positions[positionOffset++] = 0.0f;
            positions[positionOffset++] = x2; positions[positionOffset++] = y1; positions[positionOffset++] = 0.0f;
            positions[positionOffset++] = x2; positions[positionOffset++] = y2; positions[positionOffset++] = 0.0f;
            uvs[uvOffset++] = u1; uvs[uvOffset++] = v1;
            uvs[uvOffset++] = u1; uvs[uvOffset++] = v2;
            uvs[uvOffset++] = u2; uvs[uvOffset++] = v1;
            uvs[uvOffset++] = u1; uvs[uvOffset++] = v2;
            uvs[uvOffset++] = u2; uvs[uvOffset++] = v2;
            uvs[uvOffset++] = u2; uvs[uvOffset++] = v1;
        }

        if (textLayout->overlaps)
        {
            result = kzcTextureUpdateSubData(renderData->texture, textureData, 0, 0, textureWidth, textureHeight);
            kzsErrorForward(result);

            if(textLayout->font->data->targetTexture != KZ_NULL)
            {
                result = kzcMemoryAllocArray(memoryManager, textLayout->font->data->targetTextureData, textureWidth * textureHeight, "TextureData");
                kzsErrorForward(result);
                kzsMemcpy(textLayout->font->data->targetTextureData, textureData, textureWidth * textureHeight);
            }
        }

        result = kzcMemoryFreeArray(textureData);
        kzsErrorForward(result);
    }

    textLayout->renderData = renderData;

    kzsSuccess();
}

/** Draws the prepared text layout render data. */
static kzsError kzcFreetypeFontDraw_internal(const struct KzcFreetypeFontData* freetypeFontData, struct KzcRenderer* renderer,
                                             const struct KzcFreetypeFontTextRenderData* renderData)
{
    kzsError result;

    /* Disable index and vertex buffers. */
    kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);
    kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);

    /* Setup material. */
    kzcRendererSetActiveTextureUnit(renderer, 0);
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsErrorTest(freetypeFontData->system->fontShader != KZ_NULL, KZC_ERROR_FONT_SHADER_NOT_INITIALIZED, "Font shader not initialized");

    result = kzcShaderApply(freetypeFontData->system->fontShader, renderer);
    kzsErrorForward(result);

    kzcRendererSetUniformTexture(renderer, "Texture");
    result = kzcRendererSetUniformColorRGBA(renderer, "FontColorUniform", &freetypeFontData->color);
    kzsErrorForward(result);
#else
    KZ_UNUSED_PARAMETER(freetypeFontData);
#endif

    result = kzcRendererApplyTransformation(renderer);
    kzsErrorForward(result);

    kzcRendererBeginMaterialSetup(renderer);
    result = kzcTextureApply(renderData->texture, renderData->renderer);
    kzsErrorForward(result);

#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    kzcRendererSetEmissiveColor(renderer, freetypeFontData->color);
#endif
    kzcRendererSetBlendMode(renderer, KZC_RENDERER_BLEND_MODE_ALPHA);
    kzcRendererEndMaterialSetup(renderer);

    /* Draw quad. */
    kzcRendererBeginVertexArray(renderer, KZC_VERTEX_ARRAY_POSITION | KZC_VERTEX_ARRAY_TEXTURE_COORDINATE);
    kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_POSITION, renderData->vertexCoordinates);
    kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_TEXTURE_COORDINATE, renderData->vertexTextureCoordinates);
    kzcRendererEndVertexArray(renderer, KZC_PRIMITIVE_TYPE_TRIANGLES, 6);

    kzsSuccess();
}

kzsError kzcFreetypeFontDrawTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y)
{
    kzsError result;
    struct KzcFreetypeFont* freetypeFont;
    struct KzcFreetypeFontData* freetypeFontData;
    struct KzcFreetypeFontTextRenderData* renderData;

    kzsAssert(kzcIsValidPointer(textLayout));
    kzsAssert(textLayout->font->data->fontClass == &KZC_FREETYPE_FONT_CLASS);

    freetypeFont = kzcFreetypeFontFromFont(textLayout->font);
    kzsAssert(kzcIsValidPointer(freetypeFont));
    freetypeFontData = kzcFreetypeFontGetData_internal(freetypeFont);

    if (textLayout->renderData == KZ_NULL)
    {
        result = kzcFreetypeFontPrepareTextLayout(textLayout, renderer);
        kzsErrorForward(result);
    }
    renderData = (struct KzcFreetypeFontTextRenderData*)textLayout->renderData;

    /* Render the text. */
    if(textLayout->font->data->targetTexture == KZ_NULL)
    {
        kzFloat viewport[4];
        struct KzcMatrix4x4 originalProjection = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION);
        struct KzcMatrix4x4 originalWorld = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD);
        struct KzcMatrix4x4 originalCamera = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA);
        struct KzcMatrix4x4 projection;
        struct KzcMatrix4x4 world;

        /* Setup viewport. */
        kzcRendererGetViewport(renderer, &viewport[0], &viewport[1], &viewport[2], &viewport[3]);
        kzcMatrix4x4CreateOrthogonalProjection(&projection, viewport[0], viewport[0] + viewport[2], viewport[1],
                                               viewport[1] + viewport[3], -1.0f, 1.0f);

        /* Setup matrices. */
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &projection);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &KZC_MATRIX4X4_IDENTITY);
        kzcMatrix4x4SetIdentity(&world);
        kzcMatrix4x4Translate(&world, x, y, 0.0f);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &world);
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION_CAMERA_WORLD, &projection);
#endif

        /* Disable depth buffer. */
        kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_TEST);
        kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_WRITE);

        /* Draw the text. */
        result = kzcFreetypeFontDraw_internal(freetypeFontData, renderer, renderData);
        kzsErrorForward(result);

        /* Restore matrices. */
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &originalProjection);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &originalWorld);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &originalCamera);
    }
    else
    {
        result = kzcTextureBlendData(textLayout->font->data->targetTexture, textLayout->font->data->targetTextureData,
            (kzUint)(x + textLayout->left), (kzUint)(y + textLayout->top), kzcTextureGetWidth(renderData->texture), kzcTextureGetHeight(renderData->texture), &freetypeFontData->color,
            KZC_TEXTURE_FORMAT_ALPHA);
        kzsErrorForward(result);

        result = kzcMemoryFreeArray(textLayout->font->data->targetTextureData);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzcFreetypeFontDrawTextLayout3D(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result;
    struct KzcFreetypeFont* freetypeFont;
    struct KzcFreetypeFontData* freetypeFontData;
    struct KzcFreetypeFontTextRenderData* renderData;

    kzsAssert(kzcIsValidPointer(textLayout));
    kzsAssert(textLayout->font->data->fontClass == &KZC_FREETYPE_FONT_CLASS);

    freetypeFont = kzcFreetypeFontFromFont(textLayout->font);
    kzsAssert(kzcIsValidPointer(freetypeFont));
    freetypeFontData = kzcFreetypeFontGetData_internal(freetypeFont);

    if (textLayout->renderData == KZ_NULL)
    {
        result = kzcFreetypeFontPrepareTextLayout(textLayout, renderer);
        kzsErrorForward(result);
    }
    renderData = (struct KzcFreetypeFontTextRenderData*)textLayout->renderData;

    /* Draw the text. */
    result = kzcFreetypeFontDraw_internal(freetypeFontData, renderer, renderData);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcFreetypeFontFreeTextLayoutData(struct KzcTextLayout* textLayout)
{
    kzsError result;
    struct KzcFreetypeFontTextRenderData* renderData;

    kzsAssert(kzcIsValidPointer(textLayout));
    kzsAssert(textLayout->font->data->fontClass == &KZC_FREETYPE_FONT_CLASS);

    renderData = (struct KzcFreetypeFontTextRenderData*)textLayout->renderData;

    result = kzcTextureDelete(renderData->texture);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray(renderData->vertexCoordinates);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray(renderData->vertexTextureCoordinates);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(renderData);
    kzsErrorForward(result);

    textLayout->renderData = KZ_NULL;

    kzsSuccess();
}


/** FontDelete implementation for FreeType fonts. */
static kzsError kzcFreetypeFontDeleteImplementation_internal(struct KzcFont* font)
{
    kzsError result = kzcFreetypeFontDelete(kzcFreetypeFontFromFont(font));
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontSetSize implementation for FreeType fonts. */
static kzsError kzcFreetypeFontSetSizeImplementation_internal(struct KzcFont* font, kzFloat size)
{
    kzsError result = kzcFreetypeFontSetSize(kzcFreetypeFontFromFont(font), size);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontGetSize implementation for FreeType fonts. */
static kzsError kzcFreetypeFontGetSizeImplementation_internal(struct KzcFont* font, kzFloat* out_size)
{
    *out_size = kzcFreetypeFontGetSize(kzcFreetypeFontFromFont(font));
    kzsSuccess();
}

/** FontSetColor implementation for FreeType fonts. */
static kzsError kzcFreetypeFontSetColorImplementation_internal(struct KzcFont* font, struct KzcColorRGBA color)
{
    kzcFreetypeFontSetColor(kzcFreetypeFontFromFont(font), color);
    kzsSuccess();
}

/** FontGetColor implementation for FreeType fonts. */
static kzsError kzcFreetypeFontGetColorImplementation_internal(struct KzcFont* font, struct KzcColorRGBA* out_color)
{
    *out_color = kzcFreetypeFontGetColor(kzcFreetypeFontFromFont(font));
    kzsSuccess();
}

/** FontGetLineHeight implementation for FreeType fonts. */
static kzsError kzcFreetypeFontGetLineHeightImplementation_internal(struct KzcFont* font, kzFloat* out_lineHeight)
{
    *out_lineHeight = kzcFreetypeFontGetLineHeight(kzcFreetypeFontFromFont(font));
    kzsSuccess();
}

/** FontGetAscender implementation for FreeType fonts. */
static kzsError kzcFreetypeFontGetAscenderImplementation_internal(struct KzcFont* font, kzFloat* out_ascender)
{
    *out_ascender = kzcFreetypeFontGetAscender(kzcFreetypeFontFromFont(font));
    kzsSuccess();
}

/** FontGetCharacterAdvance implementation for FreeType fonts. */
static kzsError kzcFreetypeFontGetCharacterAdvanceImplementation_internal(struct KzcFont* font, kzUnicodeChar character,
                                                                          kzFloat* out_advance, struct KzcRectangle* out_boundingBox)
{
    kzsError result;
    kzFloat advanceX;
    kzFloat advanceY;
    
    result = kzcFreetypeFontGetCharacterMetrics(kzcFreetypeFontFromFont(font), character, &advanceX, &advanceY, out_boundingBox);
    kzsErrorForward(result);

    *out_advance = advanceX;
    kzsSuccess();
}

/** FontGetKerning implementation for FreeType fonts. */
static kzsError kzcFreetypeFontGetKerningImplementation_internal(struct KzcFont* font, kzUnicodeChar previousCharacter, kzUnicodeChar character, kzFloat* out_kerning)
{
    kzsError result;
    kzFloat kerningX;
    kzFloat kerningY;

    result = kzcFreetypeFontGetKerning(kzcFreetypeFontFromFont(font), previousCharacter, character, &kerningX, &kerningY);
    kzsErrorForward(result);

    *out_kerning = kerningX;
    kzsSuccess();
}

/** FontPrepareTextLayout implementation for FreeType fonts. */
static kzsError kzcFreetypeFontPrepareTextLayoutImplementation_internal(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result = kzcFreetypeFontPrepareTextLayout(textLayout, renderer);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontDrawTextLayout implementation for FreeType fonts. */
static kzsError kzcFreetypeFontDrawTextLayoutImplementation_internal(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y)
{
    kzsError result = kzcFreetypeFontDrawTextLayout(textLayout, renderer, x, y);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontDrawTextLayout3D implementation for FreeType fonts. */
static kzsError kzcFreetypeFontDrawTextLayout3DImplementation_internal(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result = kzcFreetypeFontDrawTextLayout3D(textLayout, renderer);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontFreeTextLayoutData implementation for FreeType fonts. */
static kzsError kzcFreetypeFontFreeTextLayoutDataImplementation_internal(struct KzcTextLayout* textLayout)
{
    kzsError result = kzcFreetypeFontFreeTextLayoutData(textLayout);
    kzsErrorForward(result);
    kzsSuccess();
}

const struct KzcFontClass KZC_FREETYPE_FONT_CLASS =
{
    kzcFreetypeFontDeleteImplementation_internal,
    kzcFreetypeFontSetSizeImplementation_internal,
    kzcFreetypeFontGetSizeImplementation_internal,
    kzcFreetypeFontSetColorImplementation_internal,
    kzcFreetypeFontGetColorImplementation_internal,
    kzcFreetypeFontGetLineHeightImplementation_internal,
    kzcFreetypeFontGetAscenderImplementation_internal,
    kzcFreetypeFontGetCharacterAdvanceImplementation_internal,
    kzcFreetypeFontGetKerningImplementation_internal,
    kzcFreetypeFontPrepareTextLayoutImplementation_internal,
    kzcFreetypeFontDrawTextLayoutImplementation_internal,
    kzcFreetypeFontDrawTextLayout3DImplementation_internal,
    kzcFreetypeFontFreeTextLayoutDataImplementation_internal
};
