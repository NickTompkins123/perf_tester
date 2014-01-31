/**
 * \file
 * Color structure and operations.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_COLOR_H
#define KZC_COLOR_H


#include <system/kzs_types.h>


/** Structure of 4-component RGBA color */
struct KzcColorRGBA
{
    kzFloat red;    /**< Red component      */
    kzFloat green;  /**< Green component    */
    kzFloat blue;   /**< Blue component     */
    kzFloat alpha;  /**< Alpha component    */
};



/** Constant invisible zero color. */
extern const struct KzcColorRGBA KZC_COLOR_BLACK_TRANSPARENT;
/** Constant zero color with full alpha. */
extern const struct KzcColorRGBA KZC_COLOR_BLACK;
/** Dark gray color. */
extern const struct KzcColorRGBA KZC_COLOR_DARK_GRAY;
/** Gray color. */
extern const struct KzcColorRGBA KZC_COLOR_GRAY;
/** White color. */
extern const struct KzcColorRGBA KZC_COLOR_WHITE;
/** Red color. */
extern const struct KzcColorRGBA KZC_COLOR_RED;
/** Green color. */
extern const struct KzcColorRGBA KZC_COLOR_GREEN;
/** Blue color. */
extern const struct KzcColorRGBA KZC_COLOR_BLUE;
/** Yellow color. */
extern const struct KzcColorRGBA KZC_COLOR_YELLOW;
/** Orange color. */
extern const struct KzcColorRGBA KZC_COLOR_ORANGE;
/** Lime color. */
extern const struct KzcColorRGBA KZC_COLOR_LIME;
/** Teal color. */
extern const struct KzcColorRGBA KZC_COLOR_TEAL;
/** Purple color. */
extern const struct KzcColorRGBA KZC_COLOR_PURPLE;
/** Pink color. */
extern const struct KzcColorRGBA KZC_COLOR_PINK;
/** Cyan color. */
extern const struct KzcColorRGBA KZC_COLOR_CYAN;


/** Sets a value for RGBA color. */
KZ_INLINE void kzcColorRGBASet(struct KzcColorRGBA* color, kzFloat red, kzFloat green, kzFloat blue, kzFloat alpha)
{
    color->red = red;
    color->green = green;
    color->blue = blue;
    color->alpha = alpha;
}

/** Copies color to other. */
KZ_INLINE void kzcColorRGBACopy(const struct KzcColorRGBA* sourceColor, struct KzcColorRGBA* destinationColor)
{
    destinationColor->red = sourceColor->red;
    destinationColor->green = sourceColor->green;
    destinationColor->blue = sourceColor->blue;
    destinationColor->alpha = sourceColor->alpha;
}

/** Adds color A to color B component-wise. */
KZ_INLINE void kzcColorRGBAAdd(const struct KzcColorRGBA* colorA, const struct KzcColorRGBA* colorB,
                               struct KzcColorRGBA* destinationColor)
{
    destinationColor->red = colorA->red + colorB->red;
    destinationColor->green = colorA->green + colorB->green;
    destinationColor->blue = colorA->blue + colorB->blue;
    destinationColor->alpha = colorA->alpha + colorB->alpha;
}

/** Multiplies color A by color B component-wise. */
KZ_INLINE void kzcColorRGBAMultiply(const struct KzcColorRGBA* colorA, const struct KzcColorRGBA* colorB,
                                    struct KzcColorRGBA* destinationColor)
{
    destinationColor->red = colorA->red * colorB->red;
    destinationColor->green = colorA->green * colorB->green;
    destinationColor->blue = colorA->blue * colorB->blue;
    destinationColor->alpha = colorA->alpha * colorB->alpha;
}

/** Returns new KzcColorRGBA according to r, g, b, a values. */
KZ_INLINE struct KzcColorRGBA kzcColorRGBA(kzFloat r, kzFloat g, kzFloat b, kzFloat a)
{
    struct KzcColorRGBA color;
    kzcColorRGBASet(&color, r, g, b, a);
    return color;
}


/** Compares if two colors are equal. */
kzBool kzcColorRGBAIsEqual(const struct KzcColorRGBA* color1, const struct KzcColorRGBA* color2);

/** Converts RGB color to HSL. */
void kzcColorRGBAToHSL(const struct KzcColorRGBA* color, kzFloat* out_hue, kzFloat* out_saturation, kzFloat* out_lightness);

/** Converts HSL to RGBA color. Alpha will be 1.0f. */
struct KzcColorRGBA kzcColorRGBAFromHSL(kzFloat hue, kzFloat saturation, kzFloat lightness);

/** Blends 2 colors, blend percentage clamped to [0, 1]. */
struct KzcColorRGBA kzcColorRGBABlend(const struct KzcColorRGBA* colorA, const struct KzcColorRGBA* colorB, kzFloat blendValue);

/** Interpolates 2 colors. */
struct KzcColorRGBA kzcColorRGBAInterpolate(const struct KzcColorRGBA* colorA, const struct KzcColorRGBA* colorB, kzFloat interpolationValue);

/** Sets an individual component by index. Clamps the value to 0-1 range. */
void kzcColorRGBASetComponent(struct KzcColorRGBA* color, kzUint component, kzFloat value);


#endif
