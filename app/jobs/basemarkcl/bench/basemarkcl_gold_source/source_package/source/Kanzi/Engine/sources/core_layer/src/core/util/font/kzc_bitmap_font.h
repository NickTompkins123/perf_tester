/**
* \file
* Bitmap font.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_BITMAP_FONT_H
#define KZC_BITMAP_FONT_H


#include <core/util/color/kzc_color.h>
#include <core/util/font/kzc_font_base.h>

#include <system/debug/kzs_error.h>
#include <system/wrappers/kzs_opengl_base.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzcFont;
struct KzcTexture;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcRenderer;
struct KzcRectangle;
struct KzcTextLayout;
struct KzcShader;


/**
 * \struct KzcBitmapFontSystem
 * Structure for bitmap font system. This system is needed for bitmap fonts.
 * Usually one bitmap font system per application is enough, but more can be used if needed.
 */
struct KzcBitmapFontSystem;

/**
* \struct KzcBitmapFont
* Font structure for rendering text.
*/
struct KzcBitmapFont;


/** Font type identifier for bitmap font objects. */
extern const KzcFontType KZC_FONT_TYPE_BITMAP;


/** Creates a bitmap font system. */
kzsError kzcBitmapFontSystemCreate(const struct KzcMemoryManager* memoryManager, struct KzcBitmapFontSystem** out_bitmapSystem);

/** Deletes a bitmap font system. */
kzsError kzcBitmapFontSystemDelete(struct KzcBitmapFontSystem* bitmapSystem);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
/** Sets shader for bitmap font system. */
void kzcBitmapFontSystemSetShader(struct KzcBitmapFontSystem* bitmapSystem, struct KzcShader* shader);
#endif

/** Creates new font from file. */
kzsError kzcBitmapFontCreateFromFile(const struct KzcMemoryManager* memoryManager, struct KzcBitmapFontSystem* bitmapSystem,
                                     struct KzcResourceManager* resourceManager, kzString filePath,
                                     struct KzcBitmapFont** out_bitmapFont);
/** Creates new font from resource. */
kzsError kzcBitmapFontCreateFromResource(const struct KzcMemoryManager* memoryManager, struct KzcBitmapFontSystem* bitmapSystem,
                                         struct KzcResourceManager* resourceManager, kzString resourcePath,
                                         struct KzcBitmapFont** out_bitmapFont);
/** Creates new font from preloaded resources. */
kzsError kzcBitmapFontCreateFromMemory(const struct KzcMemoryManager* memoryManager, struct KzcBitmapFontSystem* bitmapSystem,
                                       struct KzcResourceManager* resourceManager, kzString fntFileContent,
                                       struct KzcBitmapFont** out_bitmapFont);

                                       /** Frees memory allocated for given bitmap font. */
kzsError kzcBitmapFontDelete(struct KzcBitmapFont* bitmapFont);


/** Type cast from generic font to bitmap font. */
struct KzcBitmapFont* kzcBitmapFontFromFont(struct KzcFont* font);
/** Type cast from bitmap font to generic font. */
struct KzcFont* kzcBitmapFontToFont(struct KzcBitmapFont* bitmapFont);


/** Gets font texture names from font. */
struct KzcDynamicArrayIterator kzcBitmapFontGetFontTextureNames(const struct KzcBitmapFont* bitmapFont);
/** Sets texture to given slot in font. The font takes ownership of the font after the operation and releases the texture when font is released. */
kzsError kzcBitmapFontSetTexture(const struct KzcBitmapFont* bitmapFont, struct KzcTexture* texture, kzUint slot);
/** Gets texture from given slot in font. */
kzsError kzcBitmapFontGetTexture(const struct KzcBitmapFont* bitmapFont, kzUint slot, struct KzcTexture** out_texture);


/** Sets the size of bitmap font (in pixels). */
void kzcBitmapFontSetSize(const struct KzcBitmapFont* bitmapFont, kzUint size);
/** Sets the scale (relative size) of bitmap font. */
void kzcBitmapFontSetScale(const struct KzcBitmapFont* bitmapFont, kzFloat scale);
/** Returns the size of bitmap font. */
kzFloat kzcBitmapFontGetSize(const struct KzcBitmapFont* bitmapFont);

/** Sets the color of the font. */
void kzcBitmapFontSetColor(const struct KzcBitmapFont* bitmapFont, struct KzcColorRGBA color);
/** Gets the color of the font. */
struct KzcColorRGBA kzcBitmapFontGetColor(const struct KzcBitmapFont* bitmapFont);


/** Returns the line height of bitmap font. */
kzFloat kzcBitmapFontGetLineHeight(const struct KzcBitmapFont* bitmapFont);
/** Returns the ascender of bitmap font. */
kzFloat kzcBitmapFontGetAscender(const struct KzcBitmapFont* bitmapFont);


/** Returns the advance of given character using bitmap font. */
void kzcBitmapFontGetCharacterMetrics(const struct KzcBitmapFont* bitmapFont, kzUnicodeChar character,
                                      kzFloat* out_advance, struct KzcRectangle* out_boundingBox);
/** Returns the kerning of given character pair using bitmap font. */
kzFloat kzcBitmapFontGetKerning(const struct KzcBitmapFont* bitmapFont, kzUnicodeChar previousCharacter, kzUnicodeChar character);


/**
 * Draws a character at given coordinates. The coordinates indicate the character left point on the baseline.
 * This function is meant for debugging purposes only. For real text rendering, use KzcTextLayouter.
 */
kzsError kzcBitmapFontDrawCharacter(const struct KzcBitmapFont* bitmapFont, struct KzcRenderer* renderer,
                                    kzUnicodeChar character, kzFloat x, kzFloat y);


/** Prepares the given pre-layouted text for fast rendering using bitmap font. */
kzsError kzcBitmapFontPrepareTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer);
 
/** Draw the given pre-layouted text using bitmap font. */
kzsError kzcBitmapFontDrawTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y);

/** Draw the given pre-layouted text in 3D space using bitmap font. */
kzsError kzcBitmapFontDrawTextLayout3D(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer);

/** Frees the data and resources created by the prepare function. */
kzsError kzcBitmapFontFreeTextLayoutData(struct KzcTextLayout* textLayout);


#endif
