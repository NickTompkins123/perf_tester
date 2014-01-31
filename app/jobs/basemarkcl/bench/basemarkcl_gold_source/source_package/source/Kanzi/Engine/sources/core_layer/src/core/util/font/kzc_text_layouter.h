/**
* \file
* Text layouter is used for laying out text to a constrained area.
* The text is splitted to several lines when necessary.
*
* Below is a common use scenario for the text layouting.
* initialization:
*     result = kzcTextLayouterCreate(memoryManager, font, &textLayouter);
*     kzsErrorForward(result);
*
*     Optionally set text layout parameters such as alignment and maximum bounds
*
* when the text has changed or in initialization for static text:
*     result = kzcTextLayouterCreateLayout(textLayouter, memoryManager, text, &textLayout);
*     kzsErrorForward(result);
*
*     result = kzcFontPrepareTextLayout(textLayout, renderer);
*     kzsErrorForward(result);
*
* rendering:
*     result = kzcFontDrawTextLayout(textLayout, renderer, x, y);
*     kzsErrorForward(result);
*
* uninitialization:
*     result = kzcTextLayoutDelete(textLayout);
*     kzsErrorForward(result);
*
*     result = kzcTextLayouterDelete(textLayouter);
*     kzsErrorForward(result);
*     Text layouter can also be deleted already right after the text layout is created.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_TEXT_LAYOUTER_H
#define KZC_TEXT_LAYOUTER_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>


/* Forward declaration structures. */
struct KzcMemoryManager;
struct KzcRenderer;
struct KzcFont;
struct KzcTextLayout;


/**
* \struct KzcTextLayout
* Structure for text layouting and rendering.
*/
struct KzcTextLayouter;


/** Horizontal alignment options for text layout. */
enum KzcTextHorizontalAlignment
{
    KZC_TEXT_HORIZONTAL_ALIGNMENT_LEFT, /**< Text is aligned right. */
    KZC_TEXT_HORIZONTAL_ALIGNMENT_RIGHT, /**< Text is aligned left. */
    KZC_TEXT_HORIZONTAL_ALIGNMENT_CENTER /**< Text is centered horizontally. */
};

/** Vertical alignment options for text layout. */
enum KzcTextVerticalAlignment
{
    KZC_TEXT_VERTICAL_ALIGNMENT_TOP, /**< Text is aligned top. */
    KZC_TEXT_VERTICAL_ALIGNMENT_BASELINE, /**< Text is aligned on the baseline of the first line of text. */
    KZC_TEXT_VERTICAL_ALIGNMENT_CENTER, /**< Text is centered vertically. */
    KZC_TEXT_VERTICAL_ALIGNMENT_BOTTOM /**< Text is aligned bottom. */
};

/** A constant to indicate that the text layout area is not restricted horizontally. */
#define KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION 0
/** A constant to indicate that the text layout area is not restricted vertically. */
#define KZC_TEXT_LAYOUT_NO_HEIGHT_RESTRICTION 0


/** Creates a text layouter with the given font. */
kzsError kzcTextLayouterCreate(const struct KzcMemoryManager* memoryManager, struct KzcFont* font,
                               struct KzcTextLayouter** out_textLayouter);

/** Deletes the text layouter. */
kzsError kzcTextLayouterDelete(struct KzcTextLayouter* textLayouter);

/**
 * Sets the maximum width in pixels for the text layout area. Note that at least one character per line will always be layouted.
 * Use KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION for unlimited width.
 */
void kzcTextLayouterSetMaximumWidth(struct KzcTextLayouter* textLayouter, kzFloat width);

/**
 * Sets the maximum number of characters per line for the text layout area.
 * Use KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION for unlimited characters per line.
 */
void kzcTextLayouterSetMaximumCharactersPerLine(struct KzcTextLayouter* textLayouter, kzUint charactersPerLine);

/**
 * Sets the maximum height in pixels for the layout. Note that at least one line will always be layouted.
 * Use KZC_TEXT_LAYOUT_NO_HEIGHT_RESTRICTION for unlimited height.
 */
void kzcTextLayouterSetMaximumHeight(struct KzcTextLayouter* textLayouter, kzFloat height);

/**
 * Sets the maximum number of lines for the layout.
 * Use KZC_TEXT_LAYOUT_NO_HEIGHT_RESTRICTION for unlimited number of lines.
 */
void kzcTextLayouterSetMaximumLines(struct KzcTextLayouter* textLayouter, kzUint maximumLines);

/** Sets the terminator string used when both horizontal and vertical space is running out for the text layout. */
kzsError kzcTextLayouterSetTerminator(struct KzcTextLayouter* textLayouter, kzString terminator);

/** Sets the horizontal alignment of the text. */
void kzcTextLayouterSetHorizontalAlignment(struct KzcTextLayouter* textLayouter, enum KzcTextHorizontalAlignment alignment);

/** Sets the vertical alignment of the text. */
void kzcTextLayouterSetVerticalAlignment(struct KzcTextLayouter* textLayouter, enum KzcTextVerticalAlignment alignment);

/** Sets the line spacing. The value is relative to the font height, for example 1 is normal spacing and 2 is double spacing. */
void kzcTextLayouterSetLineSpacing(struct KzcTextLayouter* textLayouter, kzFloat lineSpacing);

/**
 * Lays out the given text using the given text layouter.
 * The returned text layout can be directly rendered with the associated font
 * or the layout information can be read from the structure.
 */
kzsError kzcTextLayouterCreateLayout(const struct KzcTextLayouter* textLayouter, const struct KzcMemoryManager* memoryManager,
                                     kzString text, struct KzcTextLayout** out_textLayout);

/**
 * Helper function for performing the layout and drawing the result on the screen. If the same text is rendered more
 * than once, the layout and drawing should be done separately.
 * Parameters x and y specifies where the text layout is drawn and depends on the specified alignments of the text layouter.
 */
kzsError kzcTextLayouterDrawText(const struct KzcTextLayouter* textLayouter, const struct KzcMemoryManager* memoryManager,
                                 struct KzcRenderer* renderer, kzString text, kzFloat x, kzFloat y);

/**
 * Helper function for performing the layout and drawing the result to 3D space. If the same text is rendered more
 * than once, the layout and drawing should be done separately.
 */
kzsError kzcTextLayouterDrawText3D(const struct KzcTextLayouter* textLayouter, const struct KzcMemoryManager* memoryManager,
                                   struct KzcRenderer* renderer, kzString text);

/** Gets the font used for the text layout. */
struct KzcFont* kzcTextLayoutGetFont(const struct KzcTextLayout* textLayout);

/** Deletes the text layout. */
kzsError kzcTextLayoutDelete(struct KzcTextLayout* textLayout);


#endif
