/**
* \file
* Font private members.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_FONT_PRIVATE_H
#define KZC_FONT_PRIVATE_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/wrappers/kzs_opengl_base.h>


/* Forward declarations. */
struct KzcTexture;
struct KzcShader;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcRenderer;
struct KzcColorRGBA;
struct KzcRectangle;
struct KzcTextLayout;
struct KzcTruetypeSystem;


/** Function definition for deleting TrueType font system. */
typedef kzsError (*KzcTruetypeSystemDeletor)(struct KzcTruetypeSystem* truetypeSystem);
/** Function definition for creating TrueType fonts from file. */
typedef kzsError (*KzcTruetypeFontFileCreator)(const struct KzcMemoryManager* memoryManager, struct KzcTruetypeSystem* truetypeSystem,
                                               kzString filePath, struct KzcFont** out_font);
/** Function definition for creating TrueType fonts from memory. */
typedef kzsError (*KzcTruetypeFontMemoryCreator)(const struct KzcMemoryManager* memoryManager, struct KzcTruetypeSystem* truetypeSystem,
                                                 const kzByte* data, kzUint size, struct KzcFont** out_font);


struct KzcTruetypeSystem
{
    KzcTruetypeSystemDeletor systemDeletor; /**< Function for deleting the font system. */
    KzcTruetypeFontFileCreator fontFileCreator; /**< Function for creating a font from file. */
    KzcTruetypeFontMemoryCreator fontMemoryCreator; /**< Function for creating a font from memory. */
};


/** Defines functions for different font implementations. */
struct KzcFontClass
{
    /** Frees memory allocated for given font. */
    kzsError (*kzcFontDelete)(struct KzcFont* font);

    /** Sets the size of the font (in pixels). */
    kzsError (*kzcFontSetSize)(struct KzcFont* font, kzFloat size);
    /** Returns the size of the font. */
    kzsError (*kzcFontGetSize)(struct KzcFont* font, kzFloat* out_size);

    /** Sets the color of the font. */
    kzsError (*kzcFontSetColor)(struct KzcFont* font, struct KzcColorRGBA color);
    /** Gets the color of the font. */
    kzsError (*kzcFontGetColor)(struct KzcFont* font, struct KzcColorRGBA* out_color);

    /** Returns the line height of the font. */
    kzsError (*kzcFontGetLineHeight)(struct KzcFont* font, kzFloat* out_lineHeight);

    /** Returns the ascender of the font. */
    kzsError (*kzcFontGetAscender)(struct KzcFont* font, kzFloat* out_ascender);

    /** Returns the advance of given character using. */
    kzsError (*kzcFontGetCharacterMetrics)(struct KzcFont* font, kzUnicodeChar character, kzFloat* out_advance, struct KzcRectangle* out_boundingBox);

    /** Returns the kerning of given character pair using. */
    kzsError (*kzcFontGetKerning)(struct KzcFont* font, kzUnicodeChar previousCharacter, kzUnicodeChar character, kzFloat* out_kerning);

    /** Prepares the given pre-layouted text for fast rendering. */
    kzsError (*kzcFontPrepareTextLayout)(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer);

    /** Draw the given pre-layouted text. */
    kzsError (*kzcFontDrawTextLayout)(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y);

    /** Draw the given pre-layouted text in 3D space. */
    kzsError (*kzcFontDrawTextLayout3D)(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer);

    /** Frees the data and resources created by the prepare function. */
    kzsError (*kzcFontFreeTextLayoutData)(struct KzcTextLayout* textLayout);
};

struct KzcFont
{
    struct KzcFontData* data; /**< Data of the font object. */
};

/** Font object data. */
struct KzcFontData
{
    kzBool selfOwned;
    const struct KzcFontClass* fontClass; /**< Class of the font. */
    
    struct KzcTexture* targetTexture; /**< Target texture for font. KZ_NULL to render to screen. */
    kzU8* targetTextureData;
};


/** Initializes the base of a TrueType font system. */
kzsError kzcTruetypeSystemInitialize_private(struct KzcTruetypeSystem* truetypeSystem, KzcTruetypeSystemDeletor systemDeletor,
                                             KzcTruetypeFontFileCreator fontFileCreator,
                                             KzcTruetypeFontMemoryCreator fontMemoryCreator);


/** Creates a font. */
kzsError kzcFontCreate_private(const struct KzcMemoryManager* memoryManager, const struct KzcFontClass* fontClass,
                               struct KzcFontData* fontData, struct KzcFont** out_font);

/** Deletes the base structure of a font. */
kzsError kzcFontDeleteBase_private(struct KzcFont* font);


#endif
