/**
* \file
* Font rendering helper for rendering text in a given area.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_text_layouter.h"

#include "kzc_text_layout.h"
#include "kzc_font.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>
#include <core/util/collection/kzc_dynamic_array.h>
#include <core/util/string/kzc_string.h>
#include <core/util/math/kzc_rectangle.h>

#include <system/wrappers/kzs_math.h>


struct KzcTextLayouter
{
    struct KzcFont* font; /**< Font used for layouting the text. */
    enum KzcTextHorizontalAlignment horizontalAlignment; /**< Horizontal alignment. */
    enum KzcTextVerticalAlignment verticalAlignment; /**< Vertical alignment. */
    kzFloat maximumWidth; /**< Maximum width in pixels or KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION if no maximum specified. */
    kzUint maximumCharactersPerLine; /**< Maximum width in characters or KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION if no maximum specified. */
    kzFloat maximumHeight; /**< Maximum height in pixels or KZC_TEXT_LAYOUT_NO_HEIGHT_RESTRICTION if no maximum specified. */
    kzUint maximumLines; /**< Maximum height in lines or KZC_TEXT_LAYOUT_NO_HEIGHT_RESTRICTION if no maximum specified. */
    kzFloat lineSpacing; /**< Line spacing in pixels. */
    kzMutableString terminator; /**< Terminator string to use if the given text does not fit to the given maximum size. */
};


kzsError kzcTextLayouterCreate(const struct KzcMemoryManager* memoryManager, struct KzcFont* font,
                               struct KzcTextLayouter** out_textLayouter)
{
    kzsError result;
    struct KzcTextLayouter* textLayouter;

    /* TODO: Assert font as it should not be possible to make textlayouter without one. */

    result = kzcMemoryAllocVariable(memoryManager, textLayouter, "Text layouter");
    kzsErrorForward(result);

    textLayouter->font = font;
    textLayouter->horizontalAlignment = KZC_TEXT_HORIZONTAL_ALIGNMENT_LEFT;
    textLayouter->verticalAlignment = KZC_TEXT_VERTICAL_ALIGNMENT_TOP;
    textLayouter->maximumWidth = (kzFloat)KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION; /* No maximum defined */
    textLayouter->maximumCharactersPerLine = KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION;
    textLayouter->maximumHeight = (kzFloat)KZC_TEXT_LAYOUT_NO_HEIGHT_RESTRICTION;
    textLayouter->maximumLines = KZC_TEXT_LAYOUT_NO_HEIGHT_RESTRICTION;
    textLayouter->lineSpacing = 1.0f;
    textLayouter->terminator = KZ_NULL;

    *out_textLayouter = textLayouter;
    kzsSuccess();
}

kzsError kzcTextLayouterDelete(struct KzcTextLayouter* textLayouter)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(textLayouter));

    if (textLayouter->terminator != KZ_NULL)
    {
        result = kzcStringDelete(textLayouter->terminator);
        kzsErrorForward(result);
        textLayouter->terminator = KZ_NULL;
    }

    result = kzcMemoryFreeVariable(textLayouter);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcTextLayouterSetMaximumWidth(struct KzcTextLayouter* textLayouter, kzFloat width)
{
    kzsAssert(kzcIsValidPointer(textLayouter));

    textLayouter->maximumWidth = width;
}

void kzcTextLayouterSetMaximumCharactersPerLine(struct KzcTextLayouter* textLayouter, kzUint charactersPerLine)
{
    kzsAssert(kzcIsValidPointer(textLayouter));

    textLayouter->maximumCharactersPerLine = charactersPerLine;
}

void kzcTextLayouterSetMaximumHeight(struct KzcTextLayouter* textLayouter, kzFloat height)
{
    kzsAssert(kzcIsValidPointer(textLayouter));

    textLayouter->maximumHeight = height;
}

void kzcTextLayouterSetMaximumLines(struct KzcTextLayouter* textLayouter, kzUint maximumLines)
{
    kzsAssert(kzcIsValidPointer(textLayouter));

    textLayouter->maximumLines = maximumLines;
}

kzsError kzcTextLayouterSetTerminator(struct KzcTextLayouter* textLayouter, kzString terminator)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(textLayouter));

    if (textLayouter->terminator != KZ_NULL)
    {
        result = kzcStringDelete(textLayouter->terminator);
        kzsErrorForward(result);
    }

    result = kzcStringCopy(kzcMemoryGetManager(textLayouter), terminator, &textLayouter->terminator);
    kzsErrorForward(result);

    kzsSuccess();
}

void kzcTextLayouterSetHorizontalAlignment(struct KzcTextLayouter* textLayouter, enum KzcTextHorizontalAlignment alignment)
{
    kzsAssert(kzcIsValidPointer(textLayouter));

    textLayouter->horizontalAlignment = alignment;
}

void kzcTextLayouterSetVerticalAlignment(struct KzcTextLayouter* textLayouter, enum KzcTextVerticalAlignment alignment)
{
    kzsAssert(kzcIsValidPointer(textLayouter));

    textLayouter->verticalAlignment = alignment;
}

void kzcTextLayouterSetLineSpacing(struct KzcTextLayouter* textLayouter, kzFloat lineSpacing)
{
    kzsAssert(kzcIsValidPointer(textLayouter));

    textLayouter->lineSpacing = lineSpacing;
}


/** State variables for laying out text. */
struct KzcTextLayouterState
{
    /* Cursor indicates the baseline position of character. */
    kzFloat x; /**< X-coordinate of the cursor. */
    kzFloat y; /**< Y-coordinate of the cursor. */
    kzUint index; /**< Current index in the text character array. */
    kzUint charactersOnLine; /**< Number of characters on the current line. */
    kzUint characterCount; /**< Number of characters layed out. */
    kzUnicodeChar previousCharacter; /**< Previous character in the line or '\0' if none available. */
    struct KzcRectangle boundingBox; /** Bounding box of the layouted characters. */
};


/* TODO: Take into account where glyph bounding rectangles overlap. With normal fonts this should not be a problem unless line spacing < 1 is used. */
kzsError kzcTextLayouterCreateLayout(const struct KzcTextLayouter* textLayouter, const struct KzcMemoryManager* memoryManager,
                                     kzString text, struct KzcTextLayout** out_textLayout)
{
    kzsError result;
    kzFloat lineHeight;
    kzFloat ascender;
    struct KzcFont* font;
    struct KzcTextLayout* textLayout;

    kzsAssert(kzcIsValidPointer(textLayouter));

    result = kzcMemoryAllocVariable(memoryManager, textLayout, "Text layout");
    kzsErrorForward(result);

    font = textLayouter->font;
    textLayout->font = font;

    result = kzcFontGetLineHeight(font, &lineHeight);
    kzsErrorForward(result);

    result = kzcFontGetAscender(font, &ascender);
    kzsErrorForward(result);

    /* Perform text layout, including line wrapping. */
    {
        kzUint textLength;
        kzUnicodeChar* unicodeText;
        struct KzcTextLayouterState state;
        struct KzcTextLayouterState lineStartState; /* This copy allows reverting to beginning of the current line. */
        struct KzcTextLayouterState lastPossibleLineBreakState; /* This copy allows reverting to previous position where a line break is possible. */
        struct KzcTextLayouterState lastVisibleWithTerminatorState; /* This copy allows reverting to previous character. */
        kzUint lineCount = 0;
        kzFloat width = 0.0f;
        kzFloat height = 0.0f;
        kzUint lineStartCharacterIndex = 0;
        kzBool forceBreakWord = KZ_FALSE; /* This flag is set to true when a single word doesn't fit to one line. */
        struct KzcTextLayout* terminatorLayout = KZ_NULL;
        kzFloat maximumWidth = textLayouter->maximumWidth;
        kzUint maximumCharactersPerLine = textLayouter->maximumCharactersPerLine;
        kzUint allocatedTextArrayLength;

        state.x = 0.0f;
        state.y = -ascender;
        state.index = 0;
        state.charactersOnLine = 0;
        state.characterCount = 0;
        state.previousCharacter = 0;
        state.boundingBox = KZC_RECTANGLE_EMPTY;

        lineStartState = state;
        lastPossibleLineBreakState = state;
        lastVisibleWithTerminatorState = state;

        result = kzcStringToUnicodeArray(memoryManager, text, &textLength, &unicodeText);
        kzsErrorForward(result);

        allocatedTextArrayLength = textLength + (textLayouter->terminator != KZ_NULL ? kzcStringLength(textLayouter->terminator) : 0);

        result = kzcMemoryAllocArray(memoryManager, textLayout->characters, allocatedTextArrayLength, "Text character layouts");
        kzsErrorForward(result);

        textLayout->characterCount = 0;
        textLayout->truncated = KZ_FALSE;
        textLayout->overlaps = KZ_TRUE; /* TODO: Only set to TRUE if at least two characters overlap */

        /* Process all characters, layout to the given size restrictions and handle line changes. */
        while (state.index < textLength)
        {
            kzBool firstCharacterOnLine = state.charactersOnLine == 0;
            kzUnicodeChar character = unicodeText[state.index];
            kzBool lineChange = KZ_FALSE;
            kzBool endOfLine = KZ_FALSE;

            /* Space indicates a good line breaking position. */
            if (character == ' ' || forceBreakWord)
            {
                lastPossibleLineBreakState = state;
            }

            /* '\n' forces a line break. */
            if (character == '\n')
            {
                if (terminatorLayout != KZ_NULL)
                {
                    endOfLine = KZ_TRUE;
                    lastVisibleWithTerminatorState = state;
                }
                else
                {
                    lineChange = KZ_TRUE;
                }
            }
            /* '\r' is stripped out without any handling. */
            else if (character == '\r')
            {
            }
            /* All other characters are layouted. */
            else
            {
                kzFloat kerning = 0.0f;
                kzFloat advance;
                struct KzcRectangle boundingBox;
                struct KzcCharacterLayout* characterLayout;

                if (terminatorLayout != KZ_NULL)
                {
                    lastVisibleWithTerminatorState = state;
                }

                /* Get kerning for gluing current character with the previous one. */
                if (state.previousCharacter != 0)
                {
                    result = kzcFontGetKerning(font, state.previousCharacter, character, &kerning);
                    kzsErrorForward(result);
                }

                characterLayout = &textLayout->characters[state.characterCount++];
                characterLayout->character = character;
                characterLayout->x = state.x + kerning;
                characterLayout->y = state.y;

                result = kzcFontGetCharacterMetrics(font, character, &advance, &boundingBox);
                kzsErrorForward(result);
                boundingBox.x += state.x;
                boundingBox.y += state.y;

                state.x += advance;
                ++state.charactersOnLine;
                state.boundingBox = kzcRectangleUnion(&state.boundingBox, &boundingBox);
            }

            /* If the horizontal space is running out. */
            if (!firstCharacterOnLine &&
                (endOfLine ||
                 (maximumWidth > KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION && state.x >= maximumWidth) ||
                 (maximumCharactersPerLine > KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION && state.charactersOnLine > maximumCharactersPerLine)))
            {
                /* If the line should be terminated with the specified terminator. */
                if (terminatorLayout != KZ_NULL)
                {
                    kzUint i;

                    state = lastVisibleWithTerminatorState;

                    /* Copy characters from the terminator. */
                    for (i = 0; i < terminatorLayout->characterCount; ++i)
                    {
                        struct KzcCharacterLayout* sourceCharacterLayout = &terminatorLayout->characters[i];
                        struct KzcCharacterLayout* targetCharacterLayout = &textLayout->characters[state.characterCount++];
                        targetCharacterLayout->character = sourceCharacterLayout->character;
                        targetCharacterLayout->x = state.x + sourceCharacterLayout->x;
                        targetCharacterLayout->y = state.y;
                    }
                    state.x += terminatorLayout->width;
                    state.index = textLength;
                }
                else
                {
                    state = lastPossibleLineBreakState;

                    /* If no possible line break positions are available, try again with forcing a break inside the word. */
                    if (lastPossibleLineBreakState.x == 0.0f)
                    {
                        forceBreakWord = KZ_TRUE;
                    }
                    /* Otherwise change line. */
                    else
                    {
                        forceBreakWord = KZ_FALSE;
                        lineChange = KZ_TRUE;
                    }
                }
            }
            else
            {
                state.previousCharacter = character;
                ++state.index;
            }
            
            if (lineChange && terminatorLayout == KZ_NULL)
            {
                /* Check if maximum height would be exceeded. */
                if ((textLayouter->maximumHeight > KZC_TEXT_LAYOUT_NO_HEIGHT_RESTRICTION && (height + lineHeight * (1.0f + textLayouter->lineSpacing)) > textLayouter->maximumHeight) ||
                    (textLayouter->maximumLines > KZC_TEXT_LAYOUT_NO_HEIGHT_RESTRICTION && (lineCount + 1) >= textLayouter->maximumLines))
                {
                    /* Check if terminator should be added. */
                    if (textLayouter->terminator != KZ_NULL)
                    {
                        struct KzcTextLayouter* terminatorLayouter;

                        result = kzcTextLayouterCreate(memoryManager, textLayouter->font, &terminatorLayouter);
                        kzsErrorForward(result);

                        result = kzcTextLayouterCreateLayout(terminatorLayouter, memoryManager, textLayouter->terminator, &terminatorLayout);
                        kzsErrorForward(result);

                        result = kzcTextLayouterDelete(terminatorLayouter);
                        kzsErrorForward(result);

                        if (maximumWidth > KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION)
                        {
                            maximumWidth = kzsMaxf(maximumWidth - terminatorLayout->width, 1.0f);
                        }
                        if (maximumCharactersPerLine > KZC_TEXT_LAYOUT_NO_WIDTH_RESTRICTION)
                        {
                            maximumCharactersPerLine = (kzUint)kzsMax((kzInt)maximumCharactersPerLine - (kzInt)kzcStringLength(textLayouter->terminator), 1);
                        }

                        state = lineStartState;
                        forceBreakWord = KZ_TRUE;
                        lineChange = KZ_FALSE;

                        textLayout->truncated = KZ_TRUE;
                    }
                    else
                    {
                        state.index = textLength;
                    }
                }
            }

            /* Last character also ends the line. */
            if (state.index == textLength)
            {
                lineChange = KZ_TRUE;
            }

            if (lineChange)
            {
                kzFloat lineWidth = state.x;
                width = kzsMaxf(width, lineWidth);

                ++lineCount;
                height += lineHeight * textLayouter->lineSpacing;

                /* Horizontal alignment of the line. */
                {
                    kzFloat offset;

                    switch (textLayouter->horizontalAlignment)
                    {
                        case KZC_TEXT_HORIZONTAL_ALIGNMENT_LEFT: offset = 0.0f; break;
                        case KZC_TEXT_HORIZONTAL_ALIGNMENT_CENTER: offset = -lineWidth / 2.0f; break;
                        case KZC_TEXT_HORIZONTAL_ALIGNMENT_RIGHT: offset = -lineWidth; break;
                        default:
                        {
                            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid text alignment");
                        }
                    }
                    
                    if (offset != 0)
                    {
                        kzUint i;

                        for (i = lineStartCharacterIndex; i < state.characterCount; ++i)
                        {
                            textLayout->characters[i].x += offset;
                        }
                    }
                }

                state.x = 0.0f;
                state.y -= lineHeight * textLayouter->lineSpacing;
                state.charactersOnLine = 0;
                state.previousCharacter = 0;

                while (unicodeText[state.index] == ' ')
                {
                    ++state.index;
                }
                lineStartCharacterIndex = state.characterCount;

                lineStartState = state;
                lastPossibleLineBreakState = state;
            }
        }

        height -= (lineHeight * (textLayouter->lineSpacing - 1.0f));
        if (terminatorLayout != KZ_NULL)
        {
            result = kzcTextLayoutDelete(terminatorLayout);
            kzsErrorForward(result);
        }

        result = kzcMemoryFreeArray(unicodeText);
        kzsErrorForward(result);

        state.boundingBox.y += height;
        textLayout->lineCount = lineCount;
        {
            struct KzcRectangle rectangle = kzcRectangle(0.0f, 0.0f, width, height);
            state.boundingBox = kzcRectangleUnion(&state.boundingBox, &rectangle);
        }
        textLayout->left = state.boundingBox.x;
        textLayout->bottom = state.boundingBox.y;
        textLayout->width = state.boundingBox.width;
        textLayout->height = state.boundingBox.height;
        textLayout->characterCount = state.characterCount;

        /* Horizontal alignment of text boundary. */
        {
            kzFloat offset;

            switch (textLayouter->horizontalAlignment)
            {
                case KZC_TEXT_HORIZONTAL_ALIGNMENT_LEFT: offset = 0.0f; break;
                case KZC_TEXT_HORIZONTAL_ALIGNMENT_CENTER: offset = -width / 2.0f; break;
                case KZC_TEXT_HORIZONTAL_ALIGNMENT_RIGHT: offset = -width; break;
                default:
                {
                    kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid text alignment");
                }
            }

            textLayout->left += offset;
        }

        /* TODO: Add option to justify text both horizontally and vertically. */
        /* Vertical alignment. */
        {
            kzFloat offset;

            switch (textLayouter->verticalAlignment)
            {
                case KZC_TEXT_VERTICAL_ALIGNMENT_BOTTOM: offset = 0.0f; break;
                case KZC_TEXT_VERTICAL_ALIGNMENT_CENTER: offset = -height / 2.0f; break;
                case KZC_TEXT_VERTICAL_ALIGNMENT_BASELINE: offset = -height + ascender; break;
                case KZC_TEXT_VERTICAL_ALIGNMENT_TOP: offset = -height; break;
                default:
                {
                    kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid text alignment");
                }
            }
                    
            {
                kzUint i;

                for (i = 0; i < state.characterCount; ++i)
                {
                    textLayout->characters[i].y += offset + height;
                }

                textLayout->bottom += offset;
            }
        }
        textLayout->right = textLayout->left + textLayout->width;
        textLayout->top = textLayout->bottom + textLayout->height;

        textLayout->renderData = KZ_NULL;
    }

    *out_textLayout = textLayout;
    kzsSuccess();
}

kzsError kzcTextLayouterDrawText(const struct KzcTextLayouter* textLayouter, const struct KzcMemoryManager* memoryManager,
                                 struct KzcRenderer* renderer, kzString text, kzFloat x, kzFloat y)
{
    kzsError result;
    struct KzcTextLayout* textLayout;

    result = kzcTextLayouterCreateLayout(textLayouter, memoryManager, text, &textLayout);
    kzsErrorForward(result);

    result = kzcFontDrawTextLayout(textLayout, renderer, x, y);
    kzsErrorForward(result);

    result = kzcTextLayoutDelete(textLayout);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzcTextLayouterDrawText3D(const struct KzcTextLayouter* textLayouter, const struct KzcMemoryManager* memoryManager,
                                   struct KzcRenderer* renderer, kzString text)
{
    kzsError result;
    struct KzcTextLayout* textLayout;

    result = kzcTextLayouterCreateLayout(textLayouter, memoryManager, text, &textLayout);
    kzsErrorForward(result);

    result = kzcFontDrawTextLayout3D(textLayout, renderer);
    kzsErrorForward(result);

    result = kzcTextLayoutDelete(textLayout);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzcFont* kzcTextLayoutGetFont(const struct KzcTextLayout* textLayout)
{
    kzsAssert(kzcIsValidPointer(textLayout));

    return textLayout->font;
}

kzsError kzcTextLayoutDelete(struct KzcTextLayout* textLayout)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(textLayout));

    if (textLayout->renderData != KZ_NULL)
    {
        result = kzcFontFreeTextLayoutData(textLayout);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeArray(textLayout->characters);
    kzsErrorForward(result);

    result = kzcMemoryFreeVariable(textLayout);
    kzsErrorForward(result);

    kzsSuccess();
}
