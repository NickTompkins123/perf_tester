/**
* \file
* Bitmap font.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_bitmap_font.h"

#include "kzc_font_private.h"
#include "kzc_text_layout.h"

#include <core/debug/kzc_log.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/util/string/kzc_string.h>
#include <core/util/image/kzc_image.h>
#include <core/util/io/kzc_file.h>
#include <core/util/collection/kzc_hash_map.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/color/kzc_color.h>
#include <core/util/math/kzc_rectangle.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/kzc_resource_manager.h>
#include <core/resource_manager/kzc_resource_memory_type.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/shader/kzc_resource_shader.h>
#include <core/kzc_error_codes.h>

#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_string.h>
#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_opengl_base.h>


struct KzcBitmapFontSystem
{
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    struct KzcShader* fontShader; /**< Shader used for rendering characters with bitmap font. */
#else
    kzInt unused; /*lint -esym(754, KzcBitmapFontSystem::unused) */
#endif
};

/** Internal structure for single character. */
struct KzcBitmapFontCharacter
{
    kzUint x;           /**< Horizontal position in texture. */
    kzUint y;           /**< Vertical position in texture. */
    kzUint width;       /**< Width in texture. */
    kzUint height;      /**< Height in texture. */
    kzInt offsetX;      /**< Horizontal offset from baseline. */
    kzInt offsetY;      /**< Vertical offset from baseline. */
    kzInt advanceX;     /**< How much space must be reserved for this letter before the next one. */
    kzUint page;        /**< Identifier to array of textures. */
};

/** Key used for kerning pair hashmap. */
struct KzcBitmapFontKerningPairKey
{
    kzUnicodeChar characterA; /**< First character of kerning pair. */
    kzUnicodeChar characterB; /**< Second character of kerning pair. */
};

struct KzcBitmapFont
{
    struct KzcFont font; /**< Font. Used for inheritance. */
};

/** Bitmap font data structure. */
struct KzcBitmapFontData
{
    struct KzcFontData fontData; /**< Font data. Used for inheritance. */ /*lint -esym(754, KzcBitmapFontData::fontData) Suppressed as this member is required but not referenced. */ 
    struct KzcBitmapFontSystem* system; /**< Reference to bitmap font system. */

    struct KzcHashMap* characters; /**< Available characters. <kzUnicodeChar, KzuFontCharacter> */
    struct KzcHashMap* kerningPairs; /**< Available kerning pairs. <struct KzuFontKerningPairKey, kzInt>. */

    kzUint size; /**< Height of font. */
    kzUint lineHeight; /**< Height of a line. */
    kzUint ascender; /**< Base line position. */
    kzUint textureCount; /**< How many textures this font requires */

    kzFloat scale; /**< Font scale factor relative to the size which the bitmap font was generated. */

    struct KzcColorRGBA color; /**< Current drawing color of the font. */
    struct KzcTexture** fontTextures; /**< Texture used with this font. */
    struct KzcDynamicArray* textureNames; /**< Names of bitmap files constructing this font. */
};

/** Texture specific data needed for rendering text layout quickly. */
struct KzcBitmapFontTextTextureRenderData
{
    kzFloat* vertexCoordinates; /**< Vertex coordinates of each character. */
    kzFloat* vertexTextureCoordinates; /**< Vertex texture coordinates of each character. */
    kzUint vertexCount; /**< Number of vertices. */
};

/** Data needed for rendering text layout quickly. Calculated in prepare. */
struct KzcBitmapFontTextRenderData
{
    struct KzcRenderer* renderer; /**< Renderer used for rendering the text. */
    struct KzcBitmapFontTextTextureRenderData* textureRenderDatas; /**< Per-texture data. */
};


/** Font class for bitmap font. */
extern const struct KzcFontClass KZC_BITMAP_FONT_CLASS;

const KzcFontType KZC_FONT_TYPE_BITMAP = &KZC_BITMAP_FONT_CLASS;


/** Helper macro for getting the data structure of a bitmap font. */
#define kzcBitmapFontGetData_internal(bitmapFont_param) ((struct KzcBitmapFontData*)bitmapFont_param->font.data)


kzsError kzcBitmapFontSystemCreate(const struct KzcMemoryManager* memoryManager, struct KzcBitmapFontSystem** out_bitmapSystem)
{
    kzsError result;
    struct KzcBitmapFontSystem* bitmapSystem;

    result = kzcMemoryAllocVariable(memoryManager, bitmapSystem, "Bitmap font system");
    kzsErrorForward(result);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    bitmapSystem->fontShader = KZ_NULL;
#endif

    *out_bitmapSystem = bitmapSystem;
    kzsSuccess();
}

kzsError kzcBitmapFontSystemDelete(struct KzcBitmapFontSystem* bitmapSystem)
{
    kzsError result;

    result = kzcMemoryFreeVariable(bitmapSystem);
    kzsErrorForward(result);

    kzsSuccess();
}

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
void kzcBitmapFontSystemSetShader(struct KzcBitmapFontSystem* bitmapSystem, struct KzcShader* shader)
{
    bitmapSystem->fontShader = shader;
}
#endif

/* Reads integer property from line of string. Works only for ASCII text. */
static kzsError kzcBitmapFontReadIntProperty_internal(const struct KzcMemoryManager* memoryManager, kzString string,
                                                      kzString propertyString, kzInt *out_value)
{
    kzsError result;
    kzUint splitStringCount;
    kzMutableString* splitStrings;
    kzUint splitStringResultCount;
    kzMutableString* splitStringsResult;
    kzMutableString resultString1;
    kzMutableString resultString2;
    kzInt value;
    kzUint elementIndex;
    kzString pattern = " ";

    /* FIXME: Doesn't handle invalid string properly (currently buffer overflow or something) */
    /* Split string from property string position */
    result = kzcStringSplit(memoryManager, string, propertyString, &splitStringCount, &splitStrings);
    kzsErrorForward(result);

    /* Get string for parsing */
    elementIndex = kzsMinU(splitStringCount - 1, 1);
    resultString1 = splitStrings[elementIndex];

    /* Split the value string */
    result = kzcStringSplit(memoryManager, resultString1, pattern, &splitStringResultCount, &splitStringsResult);
    kzsErrorForward(result);

    /* Remove '=' mark and convert to integer */
    result = kzcStringReplace(memoryManager, splitStringsResult[0], "=", "", &resultString2);
    kzsErrorForward(result);
    value = kzcStringToInt(resultString2);

    /* Release all memory from quick memory manager */
    result = kzcMemoryManagerResetQuickManager(memoryManager);
    kzsErrorForward(result);

    /* Store found value */
    *out_value = value;
    kzsSuccess();
}

/* Reads string property from line of string. Works only for ASCII text. */
static kzsError kzcBitmapFontReadStringProperty_internal(const struct KzcMemoryManager* memoryManager, kzString string, 
                                                         kzString propertyString, kzString* out_string)
{
    kzsError result;
    kzUint splitStringCount;
    kzMutableString* splitStrings;
    kzUint splitStringResultCount;
    kzMutableString* splitStringsResult;
    kzMutableString resultString1;
    kzMutableString resultString2;
    kzMutableString resultString3;
    kzUint elementIndex;
    kzString pattern = " ";

    /* Split string from property string position */
    result = kzcStringSplit(memoryManager, string, propertyString, &splitStringCount, &splitStrings);
    kzsErrorForward(result);

    /* Get string for parsing */
    elementIndex = kzsMinU(splitStringCount - 1, 1);
    resultString1 = splitStrings[elementIndex];

    /* Split the value string */
    result = kzcStringSplit(memoryManager, resultString1, pattern, &splitStringResultCount, &splitStringsResult);
    kzsErrorForward(result);

    /* Remove '=' mark and convert to integer */
    result = kzcStringReplace(memoryManager, splitStringsResult[0], "=", "", &resultString2);
    kzsErrorForward(result);
    /* Remove '"' marks */
    result = kzcStringReplace(memoryManager, resultString2, "\"", "", &resultString2);
    kzsErrorForward(result);
    /* Remove '\r' marks */
    result = kzcStringReplace(memoryManager, resultString2, "\r", "", &resultString3); /* TODO: Replace parsing with better one */
    kzsErrorForward(result);

    /* Store found value */
    *out_string = resultString3;
    kzsSuccess();
}

/** Loads the data from array of lines. */
static kzsError kzcBitmapFontDataLoad_internal(struct KzcBitmapFontData* bitmapFontData,
                                               struct KzcResourceManager* resourceManager, const kzMutableString* lines)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(bitmapFontData);
    kzInt size;
    kzInt ascender;
    kzInt lineHeight;
    kzInt textureCount;
    kzInt index;
    kzUint lineCount;
    kzUint currentLine = 0;
    kzInt characterCount = 0;
    kzInt kerningPairCount;
    kzUint lead;
    #define KZU_FONT_CHARACTER_PROPERTY_COUNT 9
    const kzString fontPropertyStrings[KZU_FONT_CHARACTER_PROPERTY_COUNT] =
    {
        "id",
        "x",
        "y",
        "width",
        "height",
        "xoffset",
        "yoffset",
        "xadvance",
        "page"
    };
    const kzString fontKerningStrings[] =
    {
        "first",
        "second",
        "amount"
    };
    struct KzcMemoryManager* quickMemoryManager;

    /* FIXME: Add proper error handling for missing lines. Currently buffer overflows. */
    result = kzcMemoryManagerCreateQuickManager(memoryManager, 1000000, &quickMemoryManager);
    kzsErrorForward(result);

    kzsAssert(kzcIsValidPointer(bitmapFontData));

    lineCount = kzcArrayLength(lines);

    kzsErrorTest(currentLine < lineCount, KZC_ERROR_FONT_INVALID_DATA, "Font: Failed to read first line.");

    result = kzcBitmapFontReadIntProperty_internal(quickMemoryManager, lines[currentLine], "size", &size);
    kzsErrorSubstitute(result, result, "Font: Failed to read font size.");

    bitmapFontData->size = kzsAbs(size);

    {
        kzString spacing;
        kzUint spacingCount;
        kzMutableString* spacings;

        result = kzcBitmapFontReadStringProperty_internal(quickMemoryManager, lines[currentLine], "spacing", &spacing);
        kzsErrorSubstitute(result, result, "Font: Failed to read base.");

        result = kzcStringSplit(quickMemoryManager, spacing, ",", &spacingCount, &spacings);
        kzsErrorForward(result);

        lead = (kzUint)kzcStringToInt(spacings[1]);
    }

    ++currentLine;
    kzsErrorTest(currentLine < lineCount, KZC_ERROR_FONT_INVALID_DATA, "Font: Failed to read line.");
 
    result = kzcBitmapFontReadIntProperty_internal(quickMemoryManager, lines[currentLine], "base", &ascender);
    kzsErrorSubstitute(result, result, "Font: Failed to read base.");
    bitmapFontData->ascender = (kzUint)ascender;
 
    result = kzcBitmapFontReadIntProperty_internal(quickMemoryManager, lines[currentLine], "lineHeight", &lineHeight);
    kzsErrorSubstitute(result, result, "Font: Failed to read line height.");
    bitmapFontData->lineHeight = (kzUint)lineHeight + lead;

    result = kzcBitmapFontReadIntProperty_internal(quickMemoryManager, lines[currentLine], "pages", &textureCount);
    kzsErrorSubstitute(result, result, "Font: Failed to read pages.");
    bitmapFontData->textureCount = (kzUint)textureCount;

    result = kzcMemoryAllocArray(memoryManager, bitmapFontData->fontTextures, (kzUint)bitmapFontData->textureCount, "Textures array");
    kzsErrorForward(result);

    result = kzcDynamicArrayCreate(memoryManager, &bitmapFontData->textureNames);
    kzsErrorForward(result);

    ++currentLine;
    /* Skip texture name lines until we are at the start of the characters. */
    {
        kzUint i;

        for (i = 0; i < bitmapFontData->textureCount; ++i)
        {
            if(currentLine >= lineCount)
            {
                kzsErrorThrow(KZS_ERROR_MALFORMED_DATA, "Font: File ended before page-fields were read.");
            }

            /* Load texture */
            {
                kzString textureName;
                result = kzcBitmapFontReadStringProperty_internal(quickMemoryManager, lines[currentLine], "file", &textureName);
                kzsErrorSubstitute(result, result, "Font: Failed to line height.");

                /* Load font texture */
                if(kzcFileExists(textureName))
                {
                    struct KzcImage* image;
                    result = kzcImageLoadFile(memoryManager, textureName, &image);
                    kzsErrorForward(result);
                    
                    result = kzcTextureCreateFromImage(resourceManager, KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM, KZC_TEXTURE_FILTER_BILINEAR, 
                        KZC_TEXTURE_WRAP_CLAMP, KZC_TEXTURE_COMPRESSION_NONE, image, &bitmapFontData->fontTextures[i]);
                    kzsErrorForward(result);

                    result = kzcImageDelete(image);
                    kzsErrorForward(result);
                }
                else
                {
                    /* Store name for future loading. */
                    kzMutableString name;
                    result = kzcStringCopy(memoryManager, textureName, &name);
                    kzsErrorForward(result);
                    result = kzcDynamicArrayAdd(bitmapFontData->textureNames, name);
                    kzsErrorForward(result);
                }
            }

            ++currentLine;
        }
    }

    /* Read the character count */
    result = kzcBitmapFontReadIntProperty_internal(quickMemoryManager, lines[currentLine], "chars count", &characterCount);
    kzsErrorForward(result);
    
    /* Move to correct line for reading characters */
    ++currentLine;

    /* Initialize hash map for characters. */
    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_U32, &bitmapFontData->characters);
    kzsErrorForward(result);

    /* Start reading character data */
    for (index = 0; index < characterCount; ++index)
    {
        struct KzcBitmapFontCharacter *character;
        kzUint propertyIndex;
        kzInt characterIndex = -1;

        /* Allocate memory for new character */
        result = kzcMemoryAllocVariable(memoryManager, character, "Font character definition");
        kzsErrorForward(result);

        for (propertyIndex = 0; propertyIndex < KZU_FONT_CHARACTER_PROPERTY_COUNT; ++propertyIndex)
        {
            kzInt value;

            result = kzcBitmapFontReadIntProperty_internal(quickMemoryManager, lines[currentLine], fontPropertyStrings[propertyIndex], &value);
            kzsErrorIf(result)
            {
                result = kzcLog(memoryManager, KZS_LOG_LEVEL_WARNING, "BitmapFont: Failed to read character property: %s for character %d", fontPropertyStrings[propertyIndex], index);
                kzsErrorForward(result);
            }
 
            switch (propertyIndex)
            {
                case 0:
                {
                    characterIndex = value;
                    break;
                }
                case 1:
                {
                    character->x = (kzUint)value;
                    break;
                }
                case 2:
                {
                    character->y = (kzUint)value;
                    break;
                }
                case 3:
                {
                    character->width = (kzUint)value;
                    break;
                }
                case 4:
                {
                    character->height = (kzUint)value;
                    break;
                }
                case 5:
                {            
                    character->offsetX = value;
                    break;
                }
                case 6:
                {
                    character->offsetY = value;
                    break;
                }
                case 7:
                {
                    character->advanceX = value;
                    break;
                }
                case 8:
                {
                    character->page = (kzUint)value;
                    break;
                }                 
                default:
                {
                    kzsLog(KZS_LOG_LEVEL_WARNING, "Unknown property index encountered while loading font.");
                    break;
                }
            }
        }

        if(characterIndex == -1)
        {
            kzsErrorThrow(KZS_ERROR_MALFORMED_DATA, "Font: Font properties line did not contain character id.");
        }

        /* Add to hashmap */
        {
            kzUnicodeChar* codePoint;
            result = kzcMemoryAllocVariable(memoryManager, codePoint, "Font character hash key");
            kzsErrorForward(result);
            *codePoint = (kzUnicodeChar)characterIndex;
            result = kzcHashMapPut(bitmapFontData->characters, codePoint, character);
            kzsErrorForward(result);
        }

        /* Move to next line for reading */
        ++currentLine;
        if(currentLine >= lineCount)
        {
            result = kzcLog(memoryManager, KZS_LOG_LEVEL_WARNING, "Font: Failed to read line at char: %d", index);
            kzsErrorForward(result);
        }
    }

    result = kzcBitmapFontReadIntProperty_internal(quickMemoryManager, lines[currentLine], "kernings count", &kerningPairCount);
    kzsErrorSubstitute(result, result, "Font: Failed to read kernings count.");

    ++currentLine;

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_POINTER, &bitmapFontData->kerningPairs);
    kzsErrorForward(result);

    /* Read kerning pairs */
    for(index=0; index < kerningPairCount; ++index)
    {
        kzInt kerningPairA;
        kzInt kerningPairB;
        kzInt kerningAmount;

        /* Read values */
        result = kzcBitmapFontReadIntProperty_internal(quickMemoryManager, lines[currentLine], fontKerningStrings[0], &kerningPairA);
        kzsErrorForward(result);
        result = kzcBitmapFontReadIntProperty_internal(quickMemoryManager, lines[currentLine], fontKerningStrings[1], &kerningPairB);
        kzsErrorForward(result);
        result = kzcBitmapFontReadIntProperty_internal(quickMemoryManager, lines[currentLine], fontKerningStrings[2], &kerningAmount);
        kzsErrorForward(result);

        /* Add kerning pair to hash map */
        {
            struct KzcBitmapFontKerningPairKey* hashKey;
            kzInt* hashValue;
            result = kzcMemoryAllocVariable(memoryManager, hashKey, "Font kerning pair hash key");
            kzsErrorForward(result);
            hashKey->characterA = (kzUnicodeChar)kerningPairA;
            hashKey->characterB = (kzUnicodeChar)kerningPairB;

            result = kzcMemoryAllocVariable(memoryManager, hashValue, "Font kerning amount");
            kzsErrorForward(result);
            *hashValue = kerningAmount;

            result = kzcHashMapPut(bitmapFontData->kerningPairs, hashKey, hashValue);
            kzsErrorForward(result);
        }

        /* Move to next line */
        ++currentLine;
        if(currentLine >= lineCount)
        {
            break;
        }
    }

    /* Release quick memory manager. */
    result = kzcMemoryManagerDelete(quickMemoryManager);
    kzsErrorForward(result);

    kzsSuccess();
}

/** Creates a bitmap font. */
static kzsError kzcBitmapFontCreate_internal(const struct KzcMemoryManager* memoryManager, struct KzcBitmapFontSystem* bitmapSystem,
                                             struct KzcResourceManager* resourceManager,
                                             const kzMutableString* lines, struct KzcBitmapFont** out_bitmapFont)
{
    kzsError result;
    struct KzcFont* font;
    struct KzcBitmapFontData* bitmapFontData;

    /* Allocate memory */
    result = kzcMemoryAllocVariable(memoryManager, bitmapFontData, "Bitmap font");
    kzsErrorForward(result);

    result = kzcFontCreate_private(memoryManager, &KZC_BITMAP_FONT_CLASS, &bitmapFontData->fontData, &font);
    kzsErrorForward(result);

    /* Load font data from lines */
    result = kzcBitmapFontDataLoad_internal(bitmapFontData, resourceManager, lines);
    kzsErrorForward(result);

    bitmapFontData->system = bitmapSystem;
    bitmapFontData->color = KZC_COLOR_WHITE;
    bitmapFontData->scale = 1.0f;

    *out_bitmapFont = kzcBitmapFontFromFont(font);
    kzsSuccess();
}

kzsError kzcBitmapFontCreateFromResource(const struct KzcMemoryManager* memoryManager, struct KzcBitmapFontSystem* bitmapSystem,
                                         struct KzcResourceManager* resourceManager, kzString resourcePath,
                                         struct KzcBitmapFont** out_bitmapFont)
{
    kzsError result;
    struct KzcBitmapFont* bitmapFont;
    kzMutableString *lines;
    
    kzsAssert(kzcIsValidPointer(resourceManager));

    result = kzcFileReadTextResourceLines(memoryManager, resourcePath, &lines);
    kzsErrorForward(result);

    result = kzcBitmapFontCreate_internal(memoryManager, bitmapSystem, resourceManager, lines, &bitmapFont);
    kzsErrorForward(result);

    {
        kzUint index;
        kzUint arrayLength = kzcArrayLength(lines);
        for(index=0; index<arrayLength; ++index)
        {
            result = kzcStringDelete(lines[index]);
            kzsErrorForward(result);
        }
        result = kzcMemoryFreeArray(lines);
        kzsErrorForward(result);
    }

    *out_bitmapFont = bitmapFont;
    kzsSuccess();
}

kzsError kzcBitmapFontCreateFromFile(const struct KzcMemoryManager* memoryManager, struct KzcBitmapFontSystem* bitmapSystem,
                                     struct KzcResourceManager* resourceManager, kzString filePath,
                                     struct KzcBitmapFont** out_bitmapFont)
{
    kzsError result;
    kzMutableString *lines;
    struct KzcBitmapFont* bitmapFont;

    kzsAssert(kzcIsValidPointer(resourceManager));

    /* Read file to line array */
    result = kzcFileReadTextResourceLines(memoryManager, filePath, &lines);
    kzsErrorForward(result);

    /* Create font */
    result = kzcBitmapFontCreate_internal(memoryManager, bitmapSystem, resourceManager, lines, &bitmapFont);
    kzsErrorForward(result);

    {
        kzUint index;
        kzUint arrayLength = kzcArrayLength(lines);
        for(index=0; index<arrayLength; ++index)
        {
            result = kzcStringDelete(lines[index]);
            kzsErrorForward(result);
        }
        result = kzcMemoryFreeArray(lines);
        kzsErrorForward(result);
    }

    *out_bitmapFont = bitmapFont;
    kzsSuccess();
}

kzsError kzcBitmapFontCreateFromMemory(const struct KzcMemoryManager* memoryManager, struct KzcBitmapFontSystem* bitmapSystem,
                                       struct KzcResourceManager* resourceManager, kzString fntFileContent,
                                       struct KzcBitmapFont** out_bitmapFont)
{
    kzsError result;
    kzMutableString *lines;
    kzUint lineCount;
    struct KzcBitmapFont* bitmapFont;

    kzsAssert(kzcIsValidPointer(resourceManager));

    /* Read file to line array */
    result = kzcStringSplit(memoryManager, fntFileContent, "\n", &lineCount, &lines);
    kzsErrorForward(result);

    /* Create font */
    result = kzcBitmapFontCreate_internal(memoryManager, bitmapSystem, resourceManager, lines, &bitmapFont);
    kzsErrorForward(result);

    {
        kzUint index;
        kzUint arrayLength = kzcArrayLength(lines);
        for(index=0; index<arrayLength; ++index)
        {
            result = kzcStringDelete(lines[index]);
            kzsErrorForward(result);
        }
        result = kzcMemoryFreeArray(lines);
        kzsErrorForward(result);
    }

    *out_bitmapFont = bitmapFont;
    kzsSuccess();
}

kzsError kzcBitmapFontDelete(struct KzcBitmapFont* bitmapFont)
{
    kzsError result;
    struct KzcBitmapFontData* data;

    kzsAssert(kzcIsValidPointer(bitmapFont));

    data = kzcBitmapFontGetData_internal(bitmapFont);
  
    {
        struct KzcHashMapIterator it;
        it = kzcHashMapGetIterator(data->kerningPairs);
        while(kzcHashMapIterate(it))
        {
            kzInt* kerning = (kzInt*)kzcHashMapIteratorGetValue(it);
            struct KzcBitmapFontKerningPairKey* key = (struct KzcBitmapFontKerningPairKey*)kzcHashMapIteratorGetKey(it);
            result = kzcMemoryFreeVariable(key);
            kzsErrorForward(result);
            result = kzcMemoryFreeVariable(kerning);
            kzsErrorForward(result);
        }
    }
    result = kzcHashMapDelete(kzcBitmapFontGetData_internal(bitmapFont)->kerningPairs);
    kzsErrorForward(result);

    {
        struct KzcHashMapIterator it;
        it = kzcHashMapGetIterator(kzcBitmapFontGetData_internal(bitmapFont)->characters);
        while(kzcHashMapIterate(it))
        {
            struct KzcBitmapFontCharacter* character = (struct KzcBitmapFontCharacter*)kzcHashMapIteratorGetValue(it);
            kzUnicodeChar* key = (kzUnicodeChar*)kzcHashMapIteratorGetKey(it);
            result = kzcMemoryFreeVariable(key);
            kzsErrorForward(result);
            result = kzcMemoryFreeVariable(character);
            kzsErrorForward(result);
        }
    }
    result = kzcHashMapDelete(kzcBitmapFontGetData_internal(bitmapFont)->characters);
    kzsErrorForward(result);
    
    {
        struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(kzcBitmapFontGetData_internal(bitmapFont)->textureNames);
        while(kzcDynamicArrayIterate(it))
        {
            kzMutableString string = (kzMutableString)kzcDynamicArrayIteratorGetValue(it);
            result = kzcStringDelete(string);
            kzsErrorForward(result);
        }
        result = kzcDynamicArrayDelete(kzcBitmapFontGetData_internal(bitmapFont)->textureNames);
        kzsErrorForward(result);
    }

    {
        kzUint i;
        kzUint count = kzcArrayLength(kzcBitmapFontGetData_internal(bitmapFont)->fontTextures);
        for(i=0; i<count; ++i)
        {
            result = kzcTextureDelete(kzcBitmapFontGetData_internal(bitmapFont)->fontTextures[i]);
            kzsErrorForward(result);
        }
        result = kzcMemoryFreeArray(kzcBitmapFontGetData_internal(bitmapFont)->fontTextures);
        kzsErrorForward(result);
    }

    result = kzcFontDeleteBase_private(&bitmapFont->font);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcBitmapFont* kzcBitmapFontFromFont(struct KzcFont* font)
{
    return (struct KzcBitmapFont*)font;
}

struct KzcFont* kzcBitmapFontToFont(struct KzcBitmapFont* bitmapFont)
{
    return (struct KzcFont*)bitmapFont;
}

struct KzcDynamicArrayIterator kzcBitmapFontGetFontTextureNames(const struct KzcBitmapFont* bitmapFont)
{
    kzsAssert(kzcIsValidPointer(bitmapFont));

    return kzcDynamicArrayGetIterator(kzcBitmapFontGetData_internal(bitmapFont)->textureNames);
}

kzsError kzcBitmapFontSetTexture(const struct KzcBitmapFont* bitmapFont, struct KzcTexture* texture, kzUint slot)
{
    kzsAssert(kzcIsValidPointer(bitmapFont));

    kzsErrorTest(slot < kzcArrayLength(kzcBitmapFontGetData_internal(bitmapFont)->fontTextures), KZS_ERROR_ILLEGAL_ARGUMENT, "Assigning to invalid slot.");
    
    kzcBitmapFontGetData_internal(bitmapFont)->fontTextures[slot] = texture;

    kzsSuccess();
}

kzsError kzcBitmapFontGetTexture(const struct KzcBitmapFont* bitmapFont, kzUint slot, struct KzcTexture** out_texture)
{
    kzsAssert(kzcIsValidPointer(bitmapFont));

    kzsErrorTest(slot < kzcArrayLength(kzcBitmapFontGetData_internal(bitmapFont)->fontTextures), KZS_ERROR_ILLEGAL_ARGUMENT, "Retrieving from invalid slot.");

    *out_texture = kzcBitmapFontGetData_internal(bitmapFont)->fontTextures[slot];
    kzsSuccess();
}

void kzcBitmapFontSetSize(const struct KzcBitmapFont* bitmapFont, kzUint size)
{
    struct KzcBitmapFontData* bitmapFontData;

    kzsAssert(kzcIsValidPointer(bitmapFont));

    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);

    bitmapFontData->scale = (kzFloat)size / bitmapFontData->size;
}

void kzcBitmapFontSetScale(const struct KzcBitmapFont* bitmapFont, kzFloat scale)
{
    kzsAssert(kzcIsValidPointer(bitmapFont));

    kzcBitmapFontGetData_internal(bitmapFont)->scale = scale;
}

kzFloat kzcBitmapFontGetSize(const struct KzcBitmapFont* bitmapFont)
{
    struct KzcBitmapFontData* bitmapFontData;

    kzsAssert(kzcIsValidPointer(bitmapFont));

    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);

    return (kzFloat)bitmapFontData->size *  bitmapFontData->scale;
}

void kzcBitmapFontSetColor(const struct KzcBitmapFont* bitmapFont, struct KzcColorRGBA color)
{
    kzsAssert(kzcIsValidPointer(bitmapFont));

    kzcBitmapFontGetData_internal(bitmapFont)->color = color;
}

struct KzcColorRGBA kzcBitmapFontGetColor(const struct KzcBitmapFont* bitmapFont)
{
    kzsAssert(kzcIsValidPointer(bitmapFont));

    return kzcBitmapFontGetData_internal(bitmapFont)->color;
}

kzFloat kzcBitmapFontGetLineHeight(const struct KzcBitmapFont* bitmapFont)
{
    struct KzcBitmapFontData* bitmapFontData;

    kzsAssert(kzcIsValidPointer(bitmapFont));

    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);

    return (kzFloat)bitmapFontData->lineHeight * bitmapFontData->scale;
}

kzFloat kzcBitmapFontGetAscender(const struct KzcBitmapFont* bitmapFont)
{
    struct KzcBitmapFontData* bitmapFontData;

    kzsAssert(kzcIsValidPointer(bitmapFont));

    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);

    return (kzFloat)bitmapFontData->ascender * bitmapFontData->scale;
}

void kzcBitmapFontGetCharacterMetrics(const struct KzcBitmapFont* bitmapFont, kzUnicodeChar character,
                                      kzFloat* out_advance, struct KzcRectangle* out_boundingBox)
{
    struct KzcBitmapFontData* bitmapFontData;
    struct KzcBitmapFontCharacter* fontCharacter;
    kzFloat advance;
    struct KzcRectangle boundingBox;

    kzsAssert(kzcIsValidPointer(bitmapFont));

    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);
    
    if (kzcHashMapGet(bitmapFontData->characters, &character, (void**)&fontCharacter))
    {
        advance = (kzFloat)fontCharacter->advanceX * bitmapFontData->scale;
        boundingBox.x = (kzFloat)fontCharacter->offsetX * bitmapFontData->scale;
        boundingBox.width = (kzFloat)fontCharacter->width * bitmapFontData->scale;
        boundingBox.height = (kzFloat)fontCharacter->height * bitmapFontData->scale;
        boundingBox.y = (bitmapFontData->ascender - (kzFloat)fontCharacter->offsetY) * bitmapFontData->scale - boundingBox.height;
    }
    else
    {
        advance = 0.0f;;
        boundingBox.x = 0.0f;
        boundingBox.y = 0.0f;
        boundingBox.width = 0.0f;
        boundingBox.height = 0.0f;
    };

    *out_advance = advance;
    *out_boundingBox = boundingBox;
}

kzFloat kzcBitmapFontGetKerning(const struct KzcBitmapFont* bitmapFont, kzUnicodeChar previousCharacter, kzUnicodeChar character)
{
    struct KzcBitmapFontData* bitmapFontData;
    struct KzcBitmapFontKerningPairKey kerningKey;
    kzInt* kerningAmount;
    kzFloat kerning;

    kzsAssert(kzcIsValidPointer(bitmapFont));

    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);

    kerningKey.characterA = previousCharacter;
    kerningKey.characterA = character;
    if (kzcHashMapGet(bitmapFontData->kerningPairs, &kerningKey, (void**)&kerningAmount))
    {
        kerning = (kzFloat)*kerningAmount;
    }
    else
    {
        kerning = 0.0f;
    }
    
    return kerning * bitmapFontData->scale;
}

kzsError kzcBitmapFontDrawCharacter(const struct KzcBitmapFont* bitmapFont, struct KzcRenderer* renderer,
                                    kzUnicodeChar character, kzFloat x, kzFloat y)
{
    kzsError result;
    struct KzcBitmapFontData* bitmapFontData;
    struct KzcBitmapFontCharacter* fontCharacter;

    kzsAssert(kzcIsValidPointer(bitmapFont));

    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);

    if (kzcHashMapGet(bitmapFontData->characters, &character, (void**)&fontCharacter))
    {
        kzUint width = fontCharacter->width;
        kzUint height = fontCharacter->height;
        kzFloat left = x + fontCharacter->offsetX * bitmapFontData->scale;
        kzFloat right = left + width * bitmapFontData->scale;
        kzFloat top = y + ((kzInt)bitmapFontData->ascender - fontCharacter->offsetY) * bitmapFontData->scale;
        kzFloat bottom = top - height * bitmapFontData->scale;

        /* Render */
        {
            struct KzcTexture* texture = bitmapFontData->fontTextures[fontCharacter->page];
            kzFloat positions[18];
            kzFloat uvs[12];
            kzFloat viewport[4];
            kzUint offset;
            kzUint textureWidth = kzcTextureGetWidth(texture);
            kzUint textureHeight = kzcTextureGetHeight(texture);
            kzFloat x1 = left;
            kzFloat y1 = bottom;
            kzFloat x2 = right;
            kzFloat y2 = top;
            kzFloat u1 = (kzFloat)fontCharacter->x / textureWidth;
            kzFloat v1 = (kzFloat)fontCharacter->y / textureHeight;
            kzFloat u2 = (kzFloat)(fontCharacter->x + width) / textureWidth;
            kzFloat v2 = (kzFloat)(fontCharacter->y + height) / textureHeight;

            struct KzcMatrix4x4 originalProjection = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION);
            struct KzcMatrix4x4 originalWorld = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD);
            struct KzcMatrix4x4 originalCamera = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA);
            struct KzcMatrix4x4 projection;

            /* Create texture */
            kzcRendererSetActiveTextureUnit(renderer, 0);
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
            kzsErrorTest(bitmapFontData->system->fontShader != KZ_NULL, KZC_ERROR_FONT_SHADER_NOT_INITIALIZED, "Font shader not initialized");

            result = kzcShaderApply(bitmapFontData->system->fontShader, renderer);
            kzsErrorForward(result);

            kzcRendererSetUniformTexture(renderer, "Texture");
            result = kzcRendererSetUniformColorRGBA(renderer, "FontColorUniform", &bitmapFontData->color);
            kzsErrorForward(result);
#endif

            /* Setup vertices */
            offset = 0;
            positions[offset++] = x1; positions[offset++] = y1; positions[offset++] = 0.0f;
            positions[offset++] = x1; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y1; positions[offset++] = 0.0f;
            positions[offset++] = x1; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y2; positions[offset++] = 0.0f;
            positions[offset++] = x2; positions[offset++] = y1; positions[offset++] = 0.0f;

            offset = 0;
            uvs[offset++] = u1; uvs[offset++] = v2;
            uvs[offset++] = u1; uvs[offset++] = v1;
            uvs[offset++] = u2; uvs[offset++] = v2;
            uvs[offset++] = u1; uvs[offset++] = v1;
            uvs[offset++] = u2; uvs[offset++] = v1;
            uvs[offset++] = u2; uvs[offset++] = v2;

            /* Setup material */
            kzcRendererBeginMaterialSetup(renderer);
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
            kzcRendererSetEmissiveColor(renderer, bitmapFontData->color);
            kzcRendererEnableTexturing(renderer, KZC_RENDERER_TEXTURE_TYPE_2D);
#endif
            kzcRendererSetBlendMode(renderer, KZC_RENDERER_BLEND_MODE_ALPHA);
            kzcRendererSetCullMode(renderer, KZC_RENDERER_CULL_MODE_NONE);
            kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_TEST);
            kzcRendererDisableState(renderer, KZC_RENDERER_STATE_DEPTH_WRITE);
            result = kzcTextureApply(bitmapFontData->fontTextures[fontCharacter->page], renderer);
            kzsErrorForward(result);
            kzcRendererEndMaterialSetup(renderer);

            /* Setup viewport */
            kzcRendererGetViewport(renderer, &viewport[0], &viewport[1], &viewport[2], &viewport[3]);
            kzcMatrix4x4CreateOrthogonalProjection(&projection, viewport[0], viewport[0] + viewport[2], viewport[1],
                                                   viewport[1] + viewport[3], -1.0f, 1.0f);

            /* Setup matrices */
            kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &projection);
            kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &KZC_MATRIX4X4_IDENTITY);
            kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &KZC_MATRIX4X4_IDENTITY);
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
            kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION_CAMERA_WORLD, &projection);
#endif
            result = kzcRendererApplyTransformation(renderer);
            kzsErrorForward(result);

            /* Disable index and vertex buffers */
            kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);
            kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);

            /* Draw quad */
            kzcRendererBeginVertexArray(renderer, KZC_VERTEX_ARRAY_POSITION | KZC_VERTEX_ARRAY_TEXTURE_COORDINATE);
            kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_POSITION, positions);
            kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_TEXTURE_COORDINATE, uvs);
            kzcRendererEndVertexArray(renderer, KZC_PRIMITIVE_TYPE_TRIANGLES, 6);

            /* Restore matrices */
            kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &originalProjection);
            kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &originalWorld);
            kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &originalCamera);
        }
    }

    kzsSuccess();
}

kzsError kzcBitmapFontPrepareTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result;
    struct KzcBitmapFont* bitmapFont;
    struct KzcBitmapFontData* bitmapFontData;
    struct KzcBitmapFontTextRenderData* renderData;
    struct KzcMemoryManager* memoryManager;

    kzsAssert(kzcIsValidPointer(textLayout));

    bitmapFont = kzcBitmapFontFromFont(textLayout->font);

    kzsAssert(kzcIsValidPointer(bitmapFont));
    kzsAssert(textLayout->font->data->fontClass == &KZC_BITMAP_FONT_CLASS);

    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);

    memoryManager = kzcMemoryGetManager(textLayout);

    result = kzcMemoryAllocVariable(memoryManager, renderData, "Bitmap font text render data");
    kzsErrorForward(result);

    renderData->renderer = renderer;

    {
        kzUint i;

        result = kzcMemoryAllocArray(memoryManager, renderData->textureRenderDatas, bitmapFontData->textureCount, "Bitmap font rendering texture render data");
        kzsErrorForward(result);

        for (i = 0; i < bitmapFontData->textureCount; ++i)
        {
            kzFloat* positions;
            kzFloat* uvs;
            kzUint positionOffset = 0;
            kzUint uvOffset = 0;
            kzUint vertexCount = 0;
            kzUint j;

            /* 6 vertices per character quad */
            result = kzcMemoryAllocArray(memoryManager, positions, 6 * 3 * textLayout->characterCount,
                                         "Bitmap font rendering vertex coordinates");
            kzsErrorForward(result);
            result = kzcMemoryAllocArray(memoryManager, uvs, 6 * 2 * textLayout->characterCount,
                                         "Bitmap font rendering vertex texture coordinates");
            kzsErrorForward(result);

            renderData->textureRenderDatas[i].vertexCoordinates = positions;
            renderData->textureRenderDatas[i].vertexTextureCoordinates = uvs;

            for (j = 0; j < textLayout->characterCount; ++j)
            {
                struct KzcCharacterLayout* character = &textLayout->characters[j];
                struct KzcBitmapFontCharacter* fontCharacter;

                if (kzcHashMapGet(bitmapFontData->characters, &character->character, (void**)&fontCharacter))
                {
                    if (fontCharacter->page == i)
                    {
                        struct KzcTexture* texture = bitmapFontData->fontTextures[fontCharacter->page];
                        kzUint textureWidth = kzcTextureGetWidth(texture);
                        kzUint textureHeight = kzcTextureGetHeight(texture);
                        kzUint width = fontCharacter->width;
                        kzUint height = fontCharacter->height;
                        kzFloat left = character->x + fontCharacter->offsetX * bitmapFontData->scale;
                        kzFloat right = left + width * bitmapFontData->scale;
                        kzFloat top = character->y + ((kzInt)bitmapFontData->ascender - fontCharacter->offsetY) * bitmapFontData->scale;
                        kzFloat bottom = top - height * bitmapFontData->scale;
                        kzFloat x1 = left;
                        kzFloat y1 = bottom;
                        kzFloat x2 = right;
                        kzFloat y2 = top;
                        kzFloat u1 = (kzFloat)fontCharacter->x / textureWidth;
                        kzFloat v1 = (kzFloat)fontCharacter->y / textureHeight;
                        kzFloat u2 = (kzFloat)(fontCharacter->x + width) / textureWidth;
                        kzFloat v2 = (kzFloat)(fontCharacter->y + height) / textureHeight;

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

                        ++vertexCount;
                    }
                }
            }

            renderData->textureRenderDatas[i].vertexCount = vertexCount;
        }
    }

    textLayout->renderData = renderData;

    kzsSuccess();
}

/** Draws the prepared text layout render data. */
static kzsError kzcBitmapFontDraw_internal(const struct KzcBitmapFontData* bitmapFontData, struct KzcRenderer* renderer,
                                           const struct KzcBitmapFontTextRenderData* renderData,
                                           const struct KzcTextLayout* textLayout)
{
    kzsError result;
    kzUint i;

    /* Disable index and vertex buffers. */
    kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_INDEX, KZC_RENDERER_BUFFER_VALUE_NONE);
    kzcRendererSetBuffer(renderer, KZC_RENDERER_BUFFER_VERTEX, KZC_RENDERER_BUFFER_VALUE_NONE);

    /* Setup material. */
    kzcRendererSetActiveTextureUnit(renderer, 0);
    
#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
    kzsErrorTest(bitmapFontData->system->fontShader != KZ_NULL, KZC_ERROR_FONT_SHADER_NOT_INITIALIZED, "Font shader not initialized");
    result = kzcShaderApply(bitmapFontData->system->fontShader, renderer);
    kzsErrorForward(result);
    kzcRendererSetUniformTexture(renderer, "Texture");
    result = kzcRendererSetUniformColorRGBA(renderer, "FontColorUniform", &bitmapFontData->color);
    kzsErrorForward(result);
#elif KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
    kzcRendererSetTextureMatrix(renderer, &KZC_MATRIX4X4_IDENTITY);
#endif

    result = kzcRendererApplyTransformation(renderer);
    kzsErrorForward(result);

    for (i = 0; i < bitmapFontData->textureCount; ++i)
    {
        /* Render to texture. */
        if(textLayout->overlaps && textLayout->font->data->targetTexture != KZ_NULL)
        {
            kzUint j;
            struct KzcMatrix4x4 worldMatrix = kzcRendererGetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD);
            kzUint textureWidth = kzcTextureGetWidth(bitmapFontData->fontTextures[i]);
            kzUint textureHeight = kzcTextureGetHeight(bitmapFontData->fontTextures[i]);

            for(j = 0; j < renderData->textureRenderDatas[i].vertexCount; ++j)
            {
                kzByte* data;
                
                kzInt x, y, width, height;
                kzInt u1, u2, v1, v2;
                kzUint imageX, imageY;
                kzUint newWidth, newHeight;
                struct KzcImage* glyphImage;
                
                x = (kzInt)renderData->textureRenderDatas[i].vertexCoordinates[j * 18];
                y = (kzInt)renderData->textureRenderDatas[i].vertexCoordinates[1 + j * 18];
                width = (kzInt)(renderData->textureRenderDatas[i].vertexCoordinates[6 + j * 18] -
                    (kzInt)renderData->textureRenderDatas[i].vertexCoordinates[j * 18]);
                height = (kzInt)-(renderData->textureRenderDatas[i].vertexCoordinates[4 + j * 18] -
                    renderData->textureRenderDatas[i].vertexCoordinates[1 + j * 18]);

                y = -y;

                x += (kzInt)worldMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X];
                y += (kzInt)worldMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y];

                u1 = (kzInt)((kzFloat)textureWidth * renderData->textureRenderDatas[i].vertexTextureCoordinates[j * 12]);
                v1 = (kzInt)((kzFloat)textureHeight * renderData->textureRenderDatas[i].vertexTextureCoordinates[1 + j * 12]);
                u2 = (kzInt)((kzFloat)textureWidth * renderData->textureRenderDatas[i].vertexTextureCoordinates[4 + j * 12]);
                v2 = (kzInt)((kzFloat)textureHeight * renderData->textureRenderDatas[i].vertexTextureCoordinates[3 + j * 12]);
                newWidth = (kzUint)(u2 - u1);
                newHeight = (kzUint)(v2 - v1);

                result = kzcMemoryAllocArray(kzcMemoryGetManager(textLayout->font->data->targetTexture), data, newWidth * newHeight * 4, "BitmapData");
                kzsErrorForward(result);

                if(kzcTextureGetTextureFormat(bitmapFontData->fontTextures[i]) == KZC_TEXTURE_FORMAT_RGBA)
                {
                    kzByte* textureData = kzcTextureGetData(bitmapFontData->fontTextures[i]);
                    
                    kzsAssert(kzcIsValidPointer(textureData));

                    for(imageY = (kzUint)v1; imageY < (kzUint)v2; ++imageY)
                    {
                        for(imageX = (kzUint)u1; imageX < (kzUint)u2; ++imageX)
                        {
                            kzUint newOffset = (imageY - (kzUint)v1) * newWidth + (imageX - (kzUint)u1);
                            kzUint oldOffset = imageY * textureWidth + imageX;
                            data[newOffset * 4] = textureData[oldOffset * 4];
                            data[newOffset * 4 + 1] = textureData[oldOffset * 4 + 1];
                            data[newOffset * 4 + 2] = textureData[oldOffset * 4 + 2];
                            data[newOffset * 4 + 3] = textureData[oldOffset * 4 + 3] / 2;
                        }
                    }
                }
                else
                {
                    kzsErrorThrow(KZS_ERROR_NOT_IMPLEMENTED_YET, "Only RGBA bitmap font is currently supported");
                }

                result = kzcImageLoadMemory(kzcMemoryGetManager(bitmapFontData), newWidth, newHeight, KZC_IMAGE_DATA_FORMAT_RGBA_8888, data, &glyphImage);
                kzsErrorForward(result);
                result = kzcMemoryFreeArray(data);
                kzsErrorForward(result);
                if(width > 0 && height > 0)
                {
                    result = kzcImageResize(glyphImage, (kzUint)width, (kzUint)height, KZC_IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR);
                    kzsErrorForward(result);

                    result = kzcTextureBlendData(textLayout->font->data->targetTexture, kzcImageGetData(glyphImage), x, y,  kzcImageGetWidth(glyphImage), kzcImageGetHeight(glyphImage),
                        &bitmapFontData->color, KZC_TEXTURE_FORMAT_RGBA);
                    kzsErrorForward(result);
                }
                result = kzcImageDelete(glyphImage);
                kzsErrorForward(result);
            }
        }
        else
        {
            struct KzcTexture* texture = bitmapFontData->fontTextures[i];

            /* Setup material */
            kzcRendererBeginMaterialSetup(renderer);
#if KZ_OPENGL_VERSION == KZ_OPENGL_ES_1_1
            kzcRendererSetEmissiveColor(renderer, bitmapFontData->color);
#endif
            kzcRendererSetBlendMode(renderer, KZC_RENDERER_BLEND_MODE_ALPHA);
            result = kzcTextureApply(texture, renderer);
            kzsErrorForward(result);
            kzcRendererEndMaterialSetup(renderer);

            /* Draw quad. */
            kzcRendererBeginVertexArray(renderer, KZC_VERTEX_ARRAY_POSITION | KZC_VERTEX_ARRAY_TEXTURE_COORDINATE);
            kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_POSITION, renderData->textureRenderDatas[i].vertexCoordinates);
            kzcRendererSetVertexArrayData(renderer, KZC_VERTEX_ARRAY_TEXTURE_COORDINATE, renderData->textureRenderDatas[i].vertexTextureCoordinates);
            kzcRendererEndVertexArray(renderer, KZC_PRIMITIVE_TYPE_TRIANGLES, 6 * renderData->textureRenderDatas[i].vertexCount);
        }
    }

    kzsSuccess();
}
 
kzsError kzcBitmapFontDrawTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y)
{
    kzsError result;
    struct KzcBitmapFont* bitmapFont;
    struct KzcBitmapFontData* bitmapFontData;
    struct KzcBitmapFontTextRenderData* renderData;

    kzsAssert(kzcIsValidPointer(textLayout));
    kzsAssert(textLayout->font->data->fontClass == &KZC_BITMAP_FONT_CLASS);

    bitmapFont = kzcBitmapFontFromFont(textLayout->font);
    kzsAssert(kzcIsValidPointer(bitmapFont));
    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);

    if (textLayout->renderData == KZ_NULL)
    {
        result = kzcBitmapFontPrepareTextLayout(textLayout, renderer);
        kzsErrorForward(result);
    }
    renderData = (struct KzcBitmapFontTextRenderData*)textLayout->renderData;

    /* Render the text*/
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
        result = kzcBitmapFontDraw_internal(bitmapFontData, renderer, renderData, textLayout);
        kzsErrorForward(result);

        /* Restore matrices. */
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_PROJECTION, &originalProjection);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_WORLD, &originalWorld);
        kzcRendererSetMatrix(renderer, KZC_RENDERER_MATRIX_CAMERA, &originalCamera);
    }

    kzsSuccess();
}

kzsError kzcBitmapFontDrawTextLayout3D(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result;
    struct KzcBitmapFont* bitmapFont;
    struct KzcBitmapFontData* bitmapFontData;
    struct KzcBitmapFontTextRenderData* renderData;

    kzsAssert(kzcIsValidPointer(textLayout));
    kzsAssert(textLayout->font->data->fontClass == &KZC_BITMAP_FONT_CLASS);

    bitmapFont = kzcBitmapFontFromFont(textLayout->font);
    kzsAssert(kzcIsValidPointer(bitmapFont));
    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);

    if (textLayout->renderData == KZ_NULL)
    {
        result = kzcBitmapFontPrepareTextLayout(textLayout, renderer);
        kzsErrorForward(result);
    }
    renderData = (struct KzcBitmapFontTextRenderData*)textLayout->renderData;

    /* Draw the text. */
    result = kzcBitmapFontDraw_internal(bitmapFontData, renderer, renderData, textLayout);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcBitmapFontFreeTextLayoutData(struct KzcTextLayout* textLayout)
{
    kzsError result;
    struct KzcBitmapFont* bitmapFont;
    struct KzcBitmapFontData* bitmapFontData;
    struct KzcBitmapFontTextRenderData* renderData;
    kzUint i;

    kzsAssert(kzcIsValidPointer(textLayout));
    kzsAssert(textLayout->font->data->fontClass == &KZC_BITMAP_FONT_CLASS);

    bitmapFont = kzcBitmapFontFromFont(textLayout->font);
    kzsAssert(kzcIsValidPointer(bitmapFont));
    bitmapFontData = kzcBitmapFontGetData_internal(bitmapFont);

    renderData = (struct KzcBitmapFontTextRenderData*)textLayout->renderData;

    for (i = 0; i < bitmapFontData->textureCount; ++i)
    {
        result = kzcMemoryFreeArray(renderData->textureRenderDatas[i].vertexCoordinates);
        kzsErrorForward(result);

        result = kzcMemoryFreeArray(renderData->textureRenderDatas[i].vertexTextureCoordinates);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeArray(renderData->textureRenderDatas);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(renderData);
    kzsErrorForward(result);

    textLayout->renderData = KZ_NULL;

    kzsSuccess();
}


/** FontDelete implementation for bitmap fonts. */
static kzsError kzcBitmapFontDeleteImplementation_internal(struct KzcFont* font)
{
    kzsError result = kzcBitmapFontDelete(kzcBitmapFontFromFont(font));
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontSetSize implementation for bitmap fonts. */
static kzsError kzcBitmapFontSetSizeImplementation_internal(struct KzcFont* font, kzFloat size)
{
    kzcBitmapFontSetSize(kzcBitmapFontFromFont(font), kzsFloatToUint(size));
    kzsSuccess();
}

/** FontGetSize implementation for bitmap fonts. */
static kzsError kzcBitmapFontGetSizeImplementation_internal(struct KzcFont* font, kzFloat* out_size)
{
    *out_size = kzcBitmapFontGetSize(kzcBitmapFontFromFont(font));
    kzsSuccess();
}

/** FontSetColor implementation for bitmap fonts. */
static kzsError kzcBitmapFontSetColorImplementation_internal(struct KzcFont* font, struct KzcColorRGBA color)
{
    kzcBitmapFontSetColor(kzcBitmapFontFromFont(font), color);
    kzsSuccess();
}

/** FontGetColor implementation for bitmap fonts. */
static kzsError kzcBitmapFontGetColorImplementation_internal(struct KzcFont* font, struct KzcColorRGBA* out_color)
{
    *out_color = kzcBitmapFontGetColor(kzcBitmapFontFromFont(font));
    kzsSuccess();
}

/** FontGetLineHeight implementation for bitmap fonts. */
static kzsError kzcBitmapFontGetLineHeightImplementation_internal(struct KzcFont* font, kzFloat* out_lineHeight)
{
    *out_lineHeight = kzcBitmapFontGetLineHeight(kzcBitmapFontFromFont(font));
    kzsSuccess();
}

/** FontGetAscender implementation for bitmap fonts. */
static kzsError kzcBitmapFontGetAscenderImplementation_internal(struct KzcFont* font, kzFloat* out_ascender)
{
    *out_ascender = kzcBitmapFontGetAscender(kzcBitmapFontFromFont(font));
    kzsSuccess();
}

/** FontGetCharacterAdvance implementation for bitmap fonts. */
static kzsError kzcBitmapFontGetCharacterMetricsImplementation_internal(struct KzcFont* font, kzUnicodeChar character,
                                                                        kzFloat* out_advance, struct KzcRectangle* out_boundingBox)
{
    kzcBitmapFontGetCharacterMetrics(kzcBitmapFontFromFont(font), character, out_advance, out_boundingBox);
    kzsSuccess();
}

/** FontGetKerning implementation for bitmap fonts. */
static kzsError kzcBitmapFontGetKerningImplementation_internal(struct KzcFont* font, kzUnicodeChar previousCharacter, kzUnicodeChar character, kzFloat* out_kerning)
{
    *out_kerning = kzcBitmapFontGetKerning(kzcBitmapFontFromFont(font), previousCharacter, character);
    kzsSuccess();
}

/** FontPrepareTextLayout implementation for bitmap fonts. */
static kzsError kzcBitmapFontPrepareTextLayoutImplementation_internal(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result = kzcBitmapFontPrepareTextLayout(textLayout, renderer);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontDrawTextLayout implementation for bitmap fonts. */
static kzsError kzcBitmapFontDrawTextLayoutImplementation_internal(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y)
{
    kzsError result = kzcBitmapFontDrawTextLayout(textLayout, renderer, x, y);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontDrawTextLayout3D implementation for bitmap fonts. */
static kzsError kzcBitmapFontDrawTextLayout3DImplementation_internal(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer)
{
    kzsError result = kzcBitmapFontDrawTextLayout3D(textLayout, renderer);
    kzsErrorForward(result);
    kzsSuccess();
}

/** FontFreeTextLayoutData implementation for bitmap fonts. */
static kzsError kzcBitmapFontFreeTextLayoutDataImplementation_internal(struct KzcTextLayout* textLayout)
{
    kzsError result = kzcBitmapFontFreeTextLayoutData(textLayout);
    kzsErrorForward(result);
    kzsSuccess();
}

const struct KzcFontClass KZC_BITMAP_FONT_CLASS =
{
    kzcBitmapFontDeleteImplementation_internal,
    kzcBitmapFontSetSizeImplementation_internal,
    kzcBitmapFontGetSizeImplementation_internal,
    kzcBitmapFontSetColorImplementation_internal,
    kzcBitmapFontGetColorImplementation_internal,
    kzcBitmapFontGetLineHeightImplementation_internal,
    kzcBitmapFontGetAscenderImplementation_internal,
    kzcBitmapFontGetCharacterMetricsImplementation_internal,
    kzcBitmapFontGetKerningImplementation_internal,
    kzcBitmapFontPrepareTextLayoutImplementation_internal,
    kzcBitmapFontDrawTextLayoutImplementation_internal,
    kzcBitmapFontDrawTextLayout3DImplementation_internal,
    kzcBitmapFontFreeTextLayoutDataImplementation_internal
};
