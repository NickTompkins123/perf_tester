/**
* \file
* Font handling interface.
* This interface provides functionality for measuring font related information and for drawing pre-layeted text.
* To draw arbitrary strings with this interface, the text must be first layed out with kzc_text_layouter.h.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_FONT_H
#define KZC_FONT_H


#include <core/util/color/kzc_color.h>
#include <core/util/font/kzc_font_base.h>

#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzcTexture;
struct KzcShader;
struct KzcMemoryManager;
struct KzcResourceManager;
struct KzcRenderer;
struct KzcRectangle;
struct KzcTextLayout;


/**
* \struct KzcFont
* Font structure for rendering text.
*/
struct KzcFont;


/** Frees memory allocated for given font. */
kzsError kzcFontDelete(struct KzcFont* font);


/** Sets the size of the font. */
kzsError kzcFontSetSize(struct KzcFont* font, kzFloat size);
/** Returns the height of the font. */
kzsError kzcFontGetSize(struct KzcFont* font, kzFloat* out_size);

/** Sets the color of the font. */
kzsError kzcFontSetColor(struct KzcFont* font, struct KzcColorRGBA color);
/** Gets the color of the font. */
kzsError kzcFontGetColor(struct KzcFont* font, struct KzcColorRGBA* out_color);

/** Returns the fonts preferred line height. */
kzsError kzcFontGetLineHeight(struct KzcFont* font, kzFloat* out_lineHeight);

/** Returns the fonts ascender. This is how much the font ascends below the baseline. */
kzsError kzcFontGetAscender(struct KzcFont* font, kzFloat* out_baseline);

/** Gets the number of units that the cursor should move after given character. */
kzsError kzcFontGetCharacterMetrics(struct KzcFont* font, kzUnicodeChar character, kzFloat* out_advance, struct KzcRectangle* out_boundingBox);

/** Gets the kerning between two given characters. */
kzsError kzcFontGetKerning(struct KzcFont* font, kzUnicodeChar previousCharacter, kzUnicodeChar character, kzFloat* out_kerning);

/** Prepares the given text layout for rendering. This step is automatically done when drawing for the first time if not already prepared before. */
kzsError kzcFontPrepareTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer);

/** Draws the given text layout to given 2D position on the screen. */
kzsError kzcFontDrawTextLayout(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer, kzFloat x, kzFloat y);

/** Draws the given text layout to 3D world with the current transformation. */
kzsError kzcFontDrawTextLayout3D(struct KzcTextLayout* textLayout, struct KzcRenderer* renderer);

/** Frees the data allocated in the prepare. This is automatically done when deleting the text layout if needed. */
kzsError kzcFontFreeTextLayoutData(struct KzcTextLayout* textLayout);

/** Returns font's type */ 
KzcFontType kzcFontGetType(const struct KzcFont* font);

/** Sets target texture for font. KZ_NULL for rendering to screen. */
void kzcFontSetTargetTexture(const struct KzcFont* font, struct KzcTexture* texture);




/**
* Takes ownership of the component container deletion. After this, when component is deleted,
* only the data is actually deleted and the container remains alive.
*/
void kzcFontTakeOwnership(const struct KzcFont* font);
/** Checks if the font is valid (pointer and the data are non-null). */
kzBool kzcFontIsValid(const struct KzcFont* font);
/** Moves the data from source component to the target component. */
void kzcFontTransferData(struct KzcFont* targetFont, struct KzcFont* font);


#endif
