/**
* \file
* FreeType font.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_FREETYPE_FONT_H
#define KZC_FREETYPE_FONT_H


#include <core/util/color/kzc_color.h>
#include <core/util/font/kzc_font_base.h>

#include <system/debug/kzs_error.h>
#include <system/wrappers/kzs_opengl_base.h>
#include <system/kzs_types.h>


/* Forward declaration structures. */
struct KzcTruetypeSystem;
struct KzcFont;
struct KzcMemoryManager;
struct KzcRenderer;
struct KzcResourceManager;
struct KzcRectangle;
struct KzcTextLayout;
struct KzcShader;


/**
 * \struct KzcFreetypeSystem
 * Structure for FreeType font system. This system is needed for FreeType fonts.
 * Usually one FreeType font system per application is enough, but more can be used if needed.
 */
struct KzcFreetypeSystem;

/**
* \struct KzcFreetypeFont
* Font structure for rendering text with FreeType.
*/
struct KzcFreetypeFont;


/** Font type identifier for font FreeType fonts. */
extern const KzcFontType KZC_FONT_TYPE_FREETYPE;


/** Creates an FreeType font system. */
kzsError kzcFreetypeSystemCreate(const struct KzcMemoryManager* memoryManager, struct KzcResourceManager* resourceManager,
                                 struct KzcFreetypeSystem** out_freetypeSystem);

/** Deletes an FreeType font system. */
kzsError kzcFreetypeSystemDelete(struct KzcFreetypeSystem* freetypeSystem);

#if defined(KZ_OPENGL_VERSION_SUPPORTS_SHADER_PROGRAMS)
/** Sets shader for freetype system. */
void kzcFreetypeSystemSetShader(struct KzcFreetypeSystem* freetypeSystem, struct KzcShader* shader);
#endif

/** Type casts a generic TrueType font system to more specific FreeType font system. */
struct KzcFreetypeSystem* kzcFreetypeSystemFromTruetypeSystem(struct KzcTruetypeSystem* truetypeSystem);

/** Type casts a FreeType font system to more generic TrueType font system. */
struct KzcTruetypeSystem* kzcFreetypeSystemToTruetypeSystem(struct KzcFreetypeSystem* freetypeSystem);


/** Creates new FreeType font from resource. */
/*kzsError kzcFreetypeFontCreateFromResource(const struct KzcMemoryManager* memoryManager, kzString resourcePath,
                                        struct KzcFreetypeFont** out_freetypeFont);*/

/** Creates new FreeType font from file. */
kzsError kzcFreetypeFontCreateFromFile(const struct KzcMemoryManager* memoryManager, struct KzcFreetypeSystem* freetypeSystem,
                                       kzString filePath, struct KzcFreetypeFont** out_freetypeFont);

/** Creates new FreeType font from memory. */
kzsError kzcFreetypeFontCreateFromMemory(const struct KzcMemoryManager* memoryManager, struct KzcFreetypeSystem* freetypeSystem,
                                         const kzByte* data, kzUint size, struct KzcFreetypeFont** out_freetypeFont);

/** Deletes the give FreeType font. */
kzsError kzcFreetypeFontDelete(struct KzcFreetypeFont* freetypeFont);


/** Type casts generic font structure to FreeType font structure. */
struct KzcFreetypeFont* kzcFreetypeFontFromFont(struct KzcFont* font);
/** Type casts FreeType font structure to generic font structure. */
struct KzcFont* kzcFreetypeFontToFont(struct KzcFreetypeFont* freetypeFont);


/** Sets the size of FreeType font (in pixels). */
kzsError kzcFreetypeFontSetSize(const struct KzcFreetypeFont* freetypeFont, kzFloat size);
/** Returns the size of FreeType font. */
kzFloat kzcFreetypeFontGetSize(const struct KzcFreetypeFont* freetypeFont);

/** Sets the color of the font. */
void kzcFreetypeFontSetColor(const struct KzcFreetypeFont* freetypeFont, struct KzcColorRGBA color);
/** Gets the color of the font. */
struct KzcColorRGBA kzcFreetypeFontGetColor(const struct KzcFreetypeFont* freetypeFont);


/** Returns the line height of FreeType font. */
kzFloat kzcFreetypeFontGetLineHeight(const struct KzcFreetypeFont* freetypeFont);
/** Returns the ascender of FreeType font. */
kzFloat kzcFreetypeFontGetAscender(const struct KzcFreetypeFont* freetypeFont);


/** Returns the advance of given character using FreeType font. */
kzsError kzcFreetypeFontGetCharacterMetrics(const struct KzcFreetypeFont* freetypeFont, kzUnicodeChar character,
                                            kzFloat* out_advanceX, kzFloat* out_advanceY, struct KzcRectangle* out_boundingBox);
/** Returns the kerning of given character pair using FreeType font. */
kzsError kzcFreetypeFontGetKerning(const struct KzcFreetypeFont* freetypeFont, kzUnicodeChar previousCharacter,
                                   kzUnicodeChar character, kzFloat* out_kerningX, kzFloat* out_kerningY);


/** Prepares the given pre-layouted text for fast rendering using FreeType. */
kzsError kzcFreetypeFontPrepareTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer);

/** Draw the given pre-layouted text using FreeType. */
kzsError kzcFreetypeFontDrawTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y);

/** Draw the given pre-layouted text in 3D space using FreeType. */
kzsError kzcFreetypeFontDrawTextLayout3D(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer);

/** Frees the data and resources created by the prepare function. */
kzsError kzcFreetypeFontFreeTextLayoutData(struct KzcTextLayout* textLayout);


#endif
