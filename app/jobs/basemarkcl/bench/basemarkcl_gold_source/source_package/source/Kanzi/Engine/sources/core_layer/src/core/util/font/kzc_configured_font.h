/**
* \file
* Configured font. This font implementation wraps an existing font with given font configuration (size, color).
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_CONFIGURED_FONT_H
#define KZC_CONFIGURED_FONT_H


#include <core/util/color/kzc_color.h>
#include <core/util/font/kzc_font_base.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzcFont;
struct KzcTexture;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcRenderer;
struct KzcRectangle;
struct KzcTextLayout;


/**
* \struct KzcConfiguredFont
* Font structure for rendering text.
*/
struct KzcConfiguredFont;


/** Font type identifier for configured font objects. */
extern const KzcFontType KZC_FONT_TYPE_CONFIGURED;


/** Creates new configured font wrapping the given target font. */
kzsError kzcConfiguredFontCreate(const struct KzcMemoryManager* memoryManager, struct KzcFont* targetFont,
                                 kzFloat size, struct KzcColorRGBA color, struct KzcConfiguredFont** out_configuredFont);

/** Frees memory allocated for given configured font. */
kzsError kzcConfiguredFontDelete(struct KzcConfiguredFont* configuredFont);


/** Type cast from generic font to configured font. */
struct KzcConfiguredFont* kzcConfiguredFontFromFont(struct KzcFont* font);
/** Type cast from configured font to generic font. */
struct KzcFont* kzcConfiguredFontToFont(struct KzcConfiguredFont* configuredFont);


/** Sets the size of configured font. */
kzsError kzcConfiguredFontSetSize(const struct KzcConfiguredFont* configuredFont, kzFloat size);
/** Returns the size of configured font. */
kzFloat kzcConfiguredFontGetSize(const struct KzcConfiguredFont* configuredFont);

/** Sets the color of the font. */
kzsError kzcConfiguredFontSetColor(const struct KzcConfiguredFont* configuredFont, struct KzcColorRGBA color);
/** Gets the color of the font. */
struct KzcColorRGBA kzcConfiguredFontGetColor(const struct KzcConfiguredFont* configuredFont);


/** Returns the line height of configured font. */
kzsError kzcConfiguredFontGetLineHeight(const struct KzcConfiguredFont* configuredFont, kzFloat* out_lineHeight);
/** Returns the ascender of configured font. */
kzsError kzcConfiguredFontGetAscender(const struct KzcConfiguredFont* configuredFont, kzFloat* out_ascender);


/** Returns the advance of given character using configured font. */
kzsError kzcConfiguredFontGetCharacterMetrics(const struct KzcConfiguredFont* configuredFont, kzUnicodeChar character,
                                              kzFloat* out_advance, struct KzcRectangle* out_boundingBox);
/** Returns the kerning of given character pair using configured font. */
kzsError kzcConfiguredFontGetKerning(const struct KzcConfiguredFont* configuredFont, kzUnicodeChar previousCharacter,
                                     kzUnicodeChar character, kzFloat* out_kerning);


/**
 * Draws a character at given coordinates. The coordinates indicate the character left point on the baseline.
 * This function is meant for debugging purposes only. For real text rendering, use KzcTextLayouter.
 */
kzsError kzcConfiguredFontDrawCharacter(const struct KzcConfiguredFont* configuredFont, struct KzcRenderer* renderer,
                                        kzUnicodeChar character, kzFloat x, kzFloat y);


/** Prepares the given pre-layouted text for fast rendering using configured font. */
kzsError kzcConfiguredFontPrepareTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer);
 
/** Draw the given pre-layouted text using configured font. */
kzsError kzcConfiguredFontDrawTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y);

/** Draw the given pre-layouted text in 3D space using configured font. */
kzsError kzcConfiguredFontDrawTextLayout3D(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer);

/** Frees the data and resources created by the prepare function. */
kzsError kzcConfiguredFontFreeTextLayoutData(struct KzcTextLayout* textLayout);


#endif
