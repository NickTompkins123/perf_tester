/**
 * \file
 * Color operations.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_color.h"

#include <system/wrappers/kzs_math.h>

const struct KzcColorRGBA KZC_COLOR_BLACK_TRANSPARENT = {0.0f, 0.0f, 0.0f, 0.0f};
const struct KzcColorRGBA KZC_COLOR_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_DARK_GRAY = {0.2f, 0.2f, 0.2f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_GRAY = {0.4f, 0.4f, 0.4f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_RED = {1.0f, 0.0f, 0.0f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_ORANGE = {1.0f, 0.5f, 0.0f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_LIME = {0.5f, 1.0f, 0.0f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_TEAL = {0.0f, 1.0f, 1.0f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_PURPLE = {1.0f, 0.0f, 1.0f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_PINK = {1.0f, 0.0f, 0.5f, 1.0f};
const struct KzcColorRGBA KZC_COLOR_CYAN = {0.0f, 1.0f, 1.0f, 1.0f};


/** Internal function for converting hue value to RGB. */
static kzFloat kzcColorHueToRGBComponent_internal(kzFloat f1, kzFloat f2, kzFloat hue);


void kzcColorRGBAToHSL(const struct KzcColorRGBA* color, kzFloat* out_hue, kzFloat* out_saturation, kzFloat* out_lightness)
{
    kzFloat fmin = kzsMinf(kzsMinf(color->red, color->green), color->blue);
    kzFloat fmax = kzsMaxf(kzsMaxf(color->red, color->green), color->blue);
    kzFloat delta = fmax - fmin;
    kzFloat hue = 0.0f;
    kzFloat saturation = 0.0f;
    kzFloat lightness = (fmax + fmin) / 2.0f;;

    if(kzsFloatIsEqual(delta, 0.0f))
    {
        hue = 0.0f;
        saturation = 0.0f;
    }
    else /* Chromatic data */
    {
        kzFloat deltaR, deltaG, deltaB;

        if(lightness < 0.5f)
        {
            saturation = delta / (fmax + fmin);
        }
        else
        {
            saturation = delta / (2.0f - fmax - fmin);
        }

        deltaR = (((fmax - color->red) / 6.0f) + (delta / 2.0f)) / delta;
        deltaG = (((fmax - color->green) / 6.0f) + (delta / 2.0f)) / delta;
        deltaB = (((fmax - color->blue) / 6.0f) + (delta / 2.0f)) / delta;

        if(kzsFloatIsEqual(color->red, fmax))
        {
            hue = deltaB - deltaG;
        }
        else if(kzsFloatIsEqual(color->green, fmax))
        {
            hue = (1.0f / 3.0f) + deltaR - deltaB;
        }
        else if(kzsFloatIsEqual(color->blue, fmax))
        {
            hue = (2.0f / 3.0f) + deltaG - deltaR;
        }

        if (hue < 0.0f)
        {
            hue += 1.0f;
        }
        else if (hue > 1.0f)
        {
            hue -= 1.0f;
        }
    }

    *out_hue = hue;
    *out_saturation = saturation;
    *out_lightness = lightness;
}

static kzFloat kzcColorHueToRGBComponent_internal(kzFloat f1, kzFloat f2, kzFloat hue)
{
    kzFloat res;
    if(hue < 0.0f)
    {
        hue += 1.0f;
    }
    else if(hue > 1.0f)
    {
        hue -= 1.0f;
    }
    
    if((6.0 * hue) < 1.0f)
    {
        res = f1 + (f2 - f1) * 6.0f * hue;
    }
    else if((2.0f * hue) < 1.0f)
    {
        res = f2;
    }
    else if((3.0f * hue) < 2.0f)
    {
        res = f1 + (f2 - f1) * ((2.0f / 3.0f) - hue) * 6.0f;
    }
    else
    {
        res = f1;
    }

    return res;
}

struct KzcColorRGBA kzcColorRGBAFromHSL(kzFloat hue, kzFloat saturation, kzFloat lightness)
{
    struct KzcColorRGBA rgbaColor = kzcColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);

    if(kzsFloatIsEqual(saturation, 0.0f))
    {
        rgbaColor = kzcColorRGBA(lightness, lightness, lightness, 1.0f);
    }
    else
    {
        kzFloat f2;
        kzFloat f1;

        if(lightness < 0.5f)
        {
            f2 = lightness * (1.0f + saturation);
        }
        else
        {
            f2 = (lightness + saturation) - (saturation * lightness);
        }

        f1 = 2.0f * lightness - f2;

        rgbaColor.red = kzcColorHueToRGBComponent_internal(f1, f2, hue + (1.0f / 3.0f));
        rgbaColor.green = kzcColorHueToRGBComponent_internal(f1, f2, hue);
        rgbaColor.blue = kzcColorHueToRGBComponent_internal(f1, f2, hue - (1.0f / 3.0f));
    }

    return rgbaColor;
}


kzBool kzcColorRGBAIsEqual(const struct KzcColorRGBA* color1, const struct KzcColorRGBA* color2)
{
    kzBool result;
    if (kzsFloatIsEqual(color1->red, color2->red) && kzsFloatIsEqual(color1->green, color2->green) &&
        kzsFloatIsEqual(color1->blue, color2->blue) && kzsFloatIsEqual(color1->alpha, color2->alpha))
    {
        result = KZ_TRUE;
    }
    else
    {
        result = KZ_FALSE;
    }
    return result;
}

struct KzcColorRGBA kzcColorRGBABlend(const struct KzcColorRGBA* colorA, const struct KzcColorRGBA* colorB, kzFloat interpolation)
{
    kzFloat t = kzsClampf(interpolation, 0.0f, 1.0f);
    struct KzcColorRGBA newColor;
    newColor.red = colorA->red * (1.0f - t) + colorB->red * t;
    newColor.green = colorA->green * (1.0f - t) + colorB->green * t;
    newColor.blue = colorA->blue * (1.0f - t) + colorB->blue * t;
    newColor.alpha = colorA->alpha * (1.0f - t) + colorB->alpha * t;

    return newColor;
}

struct KzcColorRGBA kzcColorRGBAInterpolate(const struct KzcColorRGBA* colorA, const struct KzcColorRGBA* colorB, kzFloat interpolationValue)
{
    struct KzcColorRGBA newColor;
    newColor = *colorA;

    if(colorA->red > colorB->red)
    {
        newColor.red = colorA->red - interpolationValue;
        if(newColor.red <= colorB->red)
        {
            newColor.red = colorB->red;
        }
    }
    else if(colorA->red < colorB->red)
    {
        newColor.red = colorA->red + interpolationValue;
        if(newColor.red >= colorB->red)
        {
            newColor.red = colorB->red;
        }
    }

    if(colorA->green > colorB->green)
    {
        newColor.green = colorA->green - interpolationValue;
        if(newColor.green <= colorB->green)
        {
            newColor.green = colorB->green;
        }
    }
    else if(colorA->green < colorB->green)
    {
        newColor.green = colorA->green + interpolationValue;
        if(newColor.green >= colorB->green)
        {
            newColor.green = colorB->green;
        }
    }

    if(colorA->blue > colorB->blue)
    {
        newColor.blue = colorA->blue - interpolationValue;
        if(newColor.blue <= colorB->blue)
        {
            newColor.blue = colorB->blue;
        }
    }
    else if(colorA->blue < colorB->blue)
    {
        newColor.blue = colorA->blue + interpolationValue;
        if(newColor.blue >= colorB->blue)
        {
            newColor.blue = colorB->blue;
        }
    }

    if(colorA->alpha > colorB->alpha)
    {
        newColor.alpha = colorA->alpha - interpolationValue;
        if(newColor.alpha <= colorB->alpha)
        {
            newColor.alpha = colorB->alpha;
        }
    }
    else if(colorA->alpha < colorB->alpha)
    {
        newColor.alpha = colorA->alpha + interpolationValue;
        if(newColor.alpha >= colorB->alpha)
        {
            newColor.alpha = colorB->alpha;
        }
    }
    
    return newColor;
}

void kzcColorRGBASetComponent(struct KzcColorRGBA* color, kzUint component, kzFloat value)
{
    kzFloat newValue = kzsClampf(value, 0.0f, 1.0f);

    switch (component)
    {
    case 0:
        {
            color->red = newValue;
            break;
        }
    case 1:
        {
            color->green = newValue;
            break;
        }
    case 2:
        {
            color->blue = newValue;
            break;
        }
    case 3:
        {
            color->alpha = newValue;
            break;
        }
    default:
        {
            kzsAssertText(KZ_FALSE, "Component out of valid range");
            break;
        }
    }
}
