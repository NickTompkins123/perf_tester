/**
 * \file
 * Wrapper for Ericsson texture compression. Original implementation licenced
 * free of charge by Ericsson.
 *
 * For technical details, see OES_compressed_ETC1_RGB8_texture extension in the OpenGL ES extensions database.
 *
 * For explanation of the compression routine see
 * Ström, Akeninen - Möller:'iPACKMAN: High - Quality, Low - Complexity Texture Compression for Mobile Phones'.
 *
 * (iPackman == ETC)
 *
 * Copyright 2008 - 2010 by Rightware. All rights reserved.
 */
/* etcpack v0.9 */
/**/
/* NO WARRANTY */
/**/
/* BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, ERICSSON MAKES NO */
/* REPRESENTATIONS OF ANY KIND, EXTENDS NO WARRANTIES OF ANY KIND; EITHER */
/* EXPRESS OR IMPLIED; INCLUDING, BUT NOT LIMITED TO, THE IMPLIED */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR */
/* PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE */
/* PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME */
/* THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. ERICSSON */
/* MAKES NO WARRANTY THAT THE MANUFACTURE, SALE, LEASE, USE OR */
/* IMPORTATION WILL BE FREE FROM INFRINGEMENT OF PATENTS, COPYRIGHTS OR */
/* OTHER INTELLECTUAL PROPERTY RIGHTS OF OTHERS, AND IT SHALL BE THE SOLE */
/* RESPONSIBILITY OF THE LICENSEE TO MAKE SUCH DETERMINATION AS IS */
/* NECESSARY WITH RESPECT TO THE ACQUISITION OF LICENSES UNDER PATENTS */
/* AND OTHER INTELLECTUAL PROPERTY OF THIRD PARTIES; */
/**/
/* IN NO EVENT WILL ERICSSON, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY */
/* GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF */
/* THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO */
/* LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY */
/* YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY */
/* OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED */
/* OF THE POSSIBILITY OF SUCH DAMAGES. */
/**/
/* (C) Ericsson AB 2005. All Rights Reserved. */
/**/

/*lint --e{701, 702, 778, 835, 845} These Lint warnings are suppressed as fixing them wouldn't make the code any better. */
/*lint --e{534, 632, 713, 732, 734, 737, 919} These Lint warnings are suppressed as it is unknown how they should be fixed. */

/*
 * Ported to Rightware Kanzi platform.
 *
 */

#include "kzc_etc.h"

#include <system/debug/kzs_log.h>
#include <system/wrappers/kzs_math.h>
#include <system/wrappers/kzs_memory.h>


#define CLAMP(ll, x, ul) (((x) < (ll)) ? (ll) : (((x) > (ul)) ? (ul) : (x)))
#define SQUARE(x) ((x) * (x))
#define JAS_ROUND(x) (((x) < 0.0) ? ((kzInt)((x) - 0.5)) : ((kzInt)((x) + 0.5)))

/* The error metric Wr Wg Wb should be definied so that Wr^2 + Wg^2 + Wb^2 = 1. */
/* Hence it is easier to first define the squared values and derive the weights */
/* as their square - roots. */

#define PERCEPTUAL_WEIGHT_R_SQUARED 0.299
#define PERCEPTUAL_WEIGHT_G_SQUARED 0.587
#define PERCEPTUAL_WEIGHT_B_SQUARED 0.114

#define RED(img, width, x, y)   img[3 * (y * width + x)]
#define GREEN(img, width, x, y) img[3 * (y * width + x) + 1]
#define BLUE(img, width, x, y)  img[3 * (y * width + x) + 2]

/* SLOW SCAN RANGE IS -5 to 5 in all three colors */
#define SLOW_SCAN_MIN (-5)
#define SLOW_SCAN_MAX (5)
#define SLOW_SCAN_RANGE ((SLOW_SCAN_MAX - (SLOW_SCAN_MIN) + 1))
#define SLOW_SCAN_OFFSET (-(SLOW_SCAN_MIN))
/* We need this to guarrantee that at least one try is valid */
#define SLOW_TRY_MIN (-4 - SLOW_SCAN_MAX)
#define SLOW_TRY_MAX (3 - (SLOW_SCAN_MIN))


/* MEDIUM SCAN RANGE IS -3 to 3in all three colors */
#define MEDIUM_SCAN_MIN (-3)
#define MEDIUM_SCAN_MAX (3)
#define MEDIUM_SCAN_RANGE ((MEDIUM_SCAN_MAX - (MEDIUM_SCAN_MIN) + 1))
#define MEDIUM_SCAN_OFFSET (-(MEDIUM_SCAN_MIN))

/* We need this to guarrantee that at least one try is valid */
#define MEDIUM_TRY_MIN (-4 - MEDIUM_SCAN_MAX)
#define MEDIUM_TRY_MAX (3 - (MEDIUM_SCAN_MIN))


#define PUTBITS(dest, data, size, startpos) (dest |= ((data) & ((1 << (size)) - 1)) << ((startpos) - (size) + 1))
#define PUTBITSHIGH(dest, data, size, startpos) (dest |= ((data) & ((1 << (size)) - 1)) << (((startpos) - 32) - (size) + 1))
#define GETBITS(source, size, startpos) (((source) >> ((startpos) - (size) + 1)) & ((1 << (size)) -1))
#define GETBITSHIGH(source, size, startpos) (((source) >> (((startpos) - 32) - (size) + 1)) & ((1 << (size)) -1))

#define ETC_SCRAMBLE {3, 2, 0, 1}
#define ETC_UNSCRAMBLE {2, 3, 1, 0}


static const kzInt KZC_ETC_COMPRESS_PARAMS[16][4] = {
    {-8, -2, 2, 8},
    {-8, -2, 2, 8},
    {-17, -5, 5, 17},
    {-17, -5, 5, 17},
    {-29, -9, 9, 29},
    {-29, -9, 9, 29},
    {-42, -13, 13, 42},
    {-42, -13, 13, 42},
    {-60, -18, 18, 60},
    {-60, -18, 18, 60},
    {-80, -24, 24, 80},
    {-80, -24, 24, 80},
    {-106, -33, 33, 106},
    {-106, -33, 33, 106},
    {-183, -47, 47, 183},
    {-183, -47, 47, 183},
};

static void kzcEtcComputeAverageColor2x4noQuantFloat_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty, kzFloat* avg_color)
{
    kzInt r = 0, g = 0, b = 0;
    kzInt y;
    KZ_UNUSED_PARAMETER(height);
    for (y = starty; y < starty + 4; y++)
    {
        kzInt x;
        for (x = startx; x < startx + 2; x++)
        {
            r += (kzInt)RED(img, width, x, y);
            g += (kzInt)GREEN(img, width, x, y);
            b += (kzInt)BLUE(img, width, x, y);
        }
    }

    avg_color[0] = (kzFloat)(r / 8.0);
    avg_color[1] = (kzFloat)(g / 8.0);
    avg_color[2] = (kzFloat)(b / 8.0);

}

static void kzcEtcComputeAverageColor4x2noQuantFloat_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty, kzFloat* avg_color)
{
    kzInt y;
    kzInt r = 0, g = 0, b = 0;
    KZ_UNUSED_PARAMETER(height);
    for (y = starty; y < starty + 2; y++)
    {
        kzInt x;
        for (x = startx; x < startx + 4; x++)
        {
            r += (kzInt)RED(img, width, x, y);
            g += (kzInt)GREEN(img, width, x, y);
            b += (kzInt)BLUE(img, width, x, y);
        }
    }

    avg_color[0] = (kzFloat)(r / 8.0);
    avg_color[1] = (kzFloat)(g / 8.0);
    avg_color[2] = (kzFloat)(b / 8.0);

}

static kzInt kzcEtcCompressBlockWithTable2x4_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty, const kzU8* avg_color,
                                                      kzInt table, kzUint* pixel_indices_MSBp, kzUint* pixel_indices_LSBp)
{

    kzU8 orig[3], approx[3];
    kzUint pixel_indices_MSB = 0, pixel_indices_LSB = 0, pixel_indices = 0;
    kzInt sum_error = 0;
    kzInt q, i, x;
    kzInt scramble[4] = ETC_SCRAMBLE;

    KZ_UNUSED_PARAMETER(height);
    i = 0;
    for (x = startx; x < startx + 2; x++)
    {
        kzInt y;
        for (y = starty; y < starty + 4; y++)
        {
            kzInt err;
            kzInt best = 0;
            kzInt min_error = 255 * 255 * 3 * 16;
            orig[0] = RED(img, width, x, y);
            orig[1] = GREEN(img, width, x, y);
            orig[2] = BLUE(img, width, x, y);

            for (q = 0; q < 4; q++)
            {
                approx[0] = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);
                approx[1] = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);
                approx[2] = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);

                /* Here we just use equal weights to R, G and B. Although this will */
                /* give visually worse results, it will give a better PSNR score. */
                err = (kzInt)(SQUARE(approx[0] - orig[0]) + SQUARE(approx[1] - orig[1]) + SQUARE(approx[2] - orig[2]));
                if (err < min_error)
                {
                    min_error = err;
                    best = q;
                }

            }

            pixel_indices = scramble[best];

            PUTBITS(pixel_indices_MSB, (pixel_indices >> 1), 1, i);
            PUTBITS(pixel_indices_LSB, (pixel_indices & 1), 1, i);

            i++;

            /* In order to simplify hardware, the table { - 12, -4, 4, 12} is indexed {11, 10, 00, 01} */
            /* so that first bit is sign bit and the other bit is size bit (4 or 12). */
            /* This means that we have to scramble the bits before storing them. */

           
            sum_error += min_error;
        }

    }

    *pixel_indices_MSBp = pixel_indices_MSB;
    *pixel_indices_LSBp = pixel_indices_LSB;

    return sum_error;
}

static kzFloat ktcEtcCompressBlockWithTable2x4percep_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                              const kzU8* avg_color, kzInt table, kzUint* pixel_indices_MSBp, kzUint* pixel_indices_LSBp)
{

    kzU8 orig[3], approx[3];
    kzUint pixel_indices_MSB = 0, pixel_indices_LSB = 0, pixel_indices = 0;
    kzFloat sum_error = 0;
    kzInt q, i;
    kzInt x;
    kzDouble wR2 = PERCEPTUAL_WEIGHT_R_SQUARED;
    kzDouble wG2 = PERCEPTUAL_WEIGHT_G_SQUARED;
    kzDouble wB2 = PERCEPTUAL_WEIGHT_B_SQUARED;
    kzInt scramble[4] = ETC_SCRAMBLE;
    KZ_UNUSED_PARAMETER(height);

    i = 0;
    for (x = startx; x < startx + 2; x++)
    {
        kzInt y;
        for (y = starty; y < starty + 4; y++)
        {
            kzFloat err;
            kzInt best = 0;
            kzFloat min_error = 255 * 255 * 3 * 16;
            orig[0] = RED(img, width, x, y);
            orig[1] = GREEN(img, width, x, y);
            orig[2] = BLUE(img, width, x, y);

            for (q = 0; q < 4; q++)
            {
                approx[0] = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);
                approx[1] = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);
                approx[2] = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);

                /* Here we just use equal weights to R, G and B. Although this will */
                /* give visually worse results, it will give a better PSNR score. */
                err = (kzFloat)(wR2 * SQUARE((approx[0] - orig[0])) + (kzFloat)wG2 * SQUARE((approx[1] - orig[1])) + (kzFloat)wB2 * SQUARE((approx[2] - orig[2])));
                if (err < min_error)
                {
                    min_error = err;
                    best = q;
                }

            }

            pixel_indices = scramble[best];

            PUTBITS(pixel_indices_MSB, (pixel_indices >> 1), 1, i);
            PUTBITS(pixel_indices_LSB, (pixel_indices & 1), 1, i);

            i++;

            /* In order to simplify hardware, the table { - 12, -4, 4, 12} is indexed {11, 10, 00, 01} */
            /* so that first bit is sign bit and the other bit is size bit (4 or 12). */
            /* This means that we have to scramble the bits before storing them. */

           
            sum_error += min_error;
        }

    }

    *pixel_indices_MSBp = pixel_indices_MSB;
    *pixel_indices_LSBp = pixel_indices_LSB;

    return sum_error;
}

static kzInt ktcEtcCompressBlockWithTable4x2_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                      const kzU8* avg_color, kzInt table, kzUint* pixel_indices_MSBp, kzUint* pixel_indices_LSBp)
{
    kzU8 orig[3], approx[3];
    kzUint pixel_indices_MSB = 0, pixel_indices_LSB = 0, pixel_indices = 0;
    kzInt sum_error = 0;
    kzInt q;
    kzInt i;
    kzInt x;
    kzInt scramble[4] = ETC_SCRAMBLE;

    KZ_UNUSED_PARAMETER(height);

    i = 0;
    for (x = startx; x < startx + 4; x++)
    {
        kzInt y;
        for (y = starty; y < starty + 2; y++)
        {
            kzInt err;
            kzInt best = 0;
            kzInt min_error = 255 * 255 * 3 * 16;
            orig[0] = RED(img, width, x, y);
            orig[1] = GREEN(img, width, x, y);
            orig[2] = BLUE(img, width, x, y);

            for (q = 0; q < 4; q++)
            {
                approx[0] = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);
                approx[1] = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);
                approx[2] = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);

                /* Here we just use equal weights to R, G and B. Although this will */
                /* give visually worse results, it will give a better PSNR score. */
                err = (kzInt)(SQUARE(approx[0] - orig[0]) + SQUARE(approx[1] - orig[1]) + SQUARE(approx[2] - orig[2]));
                if (err < min_error)
                {
                    min_error = err;
                    best = q;
                }

            }
            pixel_indices = scramble[best];

            PUTBITS(pixel_indices_MSB, (pixel_indices >> 1), 1, i);
            PUTBITS(pixel_indices_LSB, (pixel_indices & 1), 1, i);
            i++;

            /* In order to simplify hardware, the table { - 12, -4, 4, 12} is indexed {11, 10, 00, 01} */
            /* so that first bit is sign bit and the other bit is size bit (4 or 12). */
            /* This means that we have to scramble the bits before storing them. */

            sum_error += min_error;
        }
        i += 2;

    }

    *pixel_indices_MSBp = pixel_indices_MSB;
    *pixel_indices_LSBp = pixel_indices_LSB;


    return sum_error;
}

static kzFloat ktcEtcCompressBlockWithTable4x2percep_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx,
                                                              kzInt starty, const kzU8* avg_color, kzInt table,
                                                              kzUint* pixel_indices_MSBp, kzUint* pixel_indices_LSBp)
{
    kzU8 orig[3], approx[3];
    kzUint pixel_indices_MSB = 0, pixel_indices_LSB = 0, pixel_indices = 0;
    kzFloat sum_error = 0;
    kzInt q;
    kzInt i;
    kzFloat wR2 = (kzFloat)PERCEPTUAL_WEIGHT_R_SQUARED;
    kzFloat wG2 = (kzFloat)PERCEPTUAL_WEIGHT_G_SQUARED;
    kzFloat wB2 = (kzFloat)PERCEPTUAL_WEIGHT_B_SQUARED;
    kzInt x;
    kzInt scramble[4] = ETC_SCRAMBLE;

    KZ_UNUSED_PARAMETER(height);

    i = 0;
    for (x = startx; x < startx + 4; x++)
    {
        kzInt y;
        for (y = starty; y < starty + 2; y++)
        {
            kzFloat err;
            kzInt best = 0;
            kzFloat min_error = 255 * 255 * 3 * 16;
            orig[0] = RED(img, width, x, y);
            orig[1] = GREEN(img, width, x, y);
            orig[2] = BLUE(img, width, x, y);

            for (q = 0; q < 4; q++)
            {
                approx[0] = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);
                approx[1] = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);
                approx[2] = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][q], 255);

                /* Here we just use equal weights to R, G and B. Although this will */
                /* give visually worse results, it will give a better PSNR score. */
                err = (kzFloat)wR2 * (kzFloat)SQUARE(approx[0] - orig[0]) + (kzFloat)wG2 * (kzFloat)SQUARE(approx[1] - orig[1]) + (kzFloat)wB2 * (kzFloat)SQUARE(approx[2] - orig[2]);
                if (err < min_error)
                {
                    min_error = err;
                    best = q;
                }

            }
            pixel_indices = scramble[best];

            PUTBITS(pixel_indices_MSB, (pixel_indices >> 1), 1, i);
            PUTBITS(pixel_indices_LSB, (pixel_indices & 1), 1, i);
            i++;

            /* In order to simplify hardware, the table { - 12, -4, 4, 12} is indexed {11, 10, 00, 01} */
            /* so that first bit is sign bit and the other bit is size bit (4 or 12). */
            /* This means that we have to scramble the bits before storing them. */

            sum_error += min_error;
        }
        i += 2;

    }

    *pixel_indices_MSBp = pixel_indices_MSB;
    *pixel_indices_LSBp = pixel_indices_LSB;


    return sum_error;
}

static kzInt ktcEtcTryalltables3bittable2x4_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty, const kzU8* avg_color,
                                                     kzUint* best_table, kzUint* best_pixel_indices_MSB, kzUint* best_pixel_indices_LSB)
{
    kzInt min_error = 3 * 255 * 255 * 16;
    kzInt q;
    kzInt err;
    kzUint pixel_indices_MSB, pixel_indices_LSB;

    for (q = 0; q < 16; q += 2)        /* try all the 8 tables. */
    {

        err = kzcEtcCompressBlockWithTable2x4_internal(img, width, height, startx, starty, avg_color, q, &pixel_indices_MSB, &pixel_indices_LSB);

        if (err < min_error)
        {

            min_error = err;
            *best_pixel_indices_MSB = pixel_indices_MSB;
            *best_pixel_indices_LSB = pixel_indices_LSB;
            (*best_table) = q >> 1;

        }
    }
    return min_error;
}

static kzInt ktcEtcTryalltables3bittable2x4percep_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                           const kzU8* avg_color, kzUint* best_table, kzUint* best_pixel_indices_MSB,
                                                           kzUint* best_pixel_indices_LSB)
{
    kzFloat min_error = 3 * 255 * 255 * 16;
    kzInt q;
    kzFloat err;
    kzUint pixel_indices_MSB, pixel_indices_LSB;

    for (q = 0; q < 16; q += 2)        /* try all the 8 tables. */
    {

        err = ktcEtcCompressBlockWithTable2x4percep_internal(img, width, height, startx, starty, avg_color, q, &pixel_indices_MSB, &pixel_indices_LSB);

        if (err < min_error)
        {

            min_error = err;
            *best_pixel_indices_MSB = pixel_indices_MSB;
            *best_pixel_indices_LSB = pixel_indices_LSB;
            *best_table = q >> 1;

        }
    }
    return (kzInt)min_error;
}

static kzInt ktcEtcTryalltables3bittable4x2_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                     const kzU8* avg_color, kzUint* best_table, kzUint* best_pixel_indices_MSB,
                                                     kzUint* best_pixel_indices_LSB)
{
    kzInt min_error = 3 * 255 * 255 * 16;
    kzInt q;
    kzInt err;
    kzUint pixel_indices_MSB, pixel_indices_LSB;

    for (q = 0; q < 16; q += 2)        /* try all the 8 tables. */
    {

        err = ktcEtcCompressBlockWithTable4x2_internal(img, width, height, startx, starty, avg_color, q, &pixel_indices_MSB, &pixel_indices_LSB);

        if (err < min_error)
        {

            min_error = err;
            *best_pixel_indices_MSB = pixel_indices_MSB;
            *best_pixel_indices_LSB = pixel_indices_LSB;
            *best_table = q >> 1;
        }
    }

    return min_error;
}

static kzInt ktcEtcTryalltables3bittable4x2percep_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                           const kzU8* avg_color, kzUint* best_table, kzUint* best_pixel_indices_MSB,
                                                           kzUint* best_pixel_indices_LSB)
{
    kzFloat min_error = 3 * 255 * 255 * 16;
    kzInt q;
    kzFloat err;
    kzUint pixel_indices_MSB, pixel_indices_LSB;

    for (q = 0; q < 16; q += 2)        /* try all the 8 tables. */
    {

        err = ktcEtcCompressBlockWithTable4x2percep_internal(img, width, height, startx, starty, avg_color, q, &pixel_indices_MSB, &pixel_indices_LSB);

        if (err < min_error)
        {

            min_error = err;
            *best_pixel_indices_MSB = pixel_indices_MSB;
            *best_pixel_indices_LSB = pixel_indices_LSB;
            *best_table = q >> 1;
        }
    }

    return (kzInt)min_error;
}

/* The below code quantizes a float RGB value to RGB555. */
/**/
/* It uses a trick: Since this color will later be modified with equal amounts in R, G and B, */
/* (effectively adding k * (1, 1, 1) to the color) we quantize not to the nearest RGB555 point, */
/* but to the nearest line starting in an RGB555 point with the direction vector (1, 1, 1). */
/**/
/* (See the presentation http://www.jacobstrom.com/publications/PACKMAN.ppt for more info.) */
/**/

static void ktcEtcQuantize444ColorCombined_internal(const kzFloat* avg_col_in, kzInt* enc_color, kzU8* avg_color)
{
    kzFloat dr, dg, db;
    kzFloat kr, kg, kb;
    kzFloat wR2, wG2, wB2;
    kzU8 low_color[3];
    kzU8 high_color[3];
    kzFloat lowhightable[8];
    kzInt q;
    kzFloat kval = (kzFloat)(255.0/15.0);


    /* These are the values that we want to have: */
    kzFloat red_average, green_average, blue_average;

    kzInt red_4bit_low, green_4bit_low, blue_4bit_low;
    kzInt red_4bit_high, green_4bit_high, blue_4bit_high;
   
    /* These are the values that we approximate with: */
    kzInt red_low, green_low, blue_low;
    kzInt red_high, green_high, blue_high;
    kzFloat min_value;
    kzInt min_index;

    red_average = avg_col_in[0];
    green_average = avg_col_in[1];
    blue_average = avg_col_in[2];

    /* Find the 5 - bit reconstruction levels red_low, red_high */
    /* so that red_average is in interval [red_low, red_high]. */
    /* (The same with green and blue.) */

    red_4bit_low = (kzInt)(red_average/kval);
    green_4bit_low = (kzInt)(green_average/kval);
    blue_4bit_low = (kzInt)(blue_average/kval);

    red_4bit_high = CLAMP(0, red_4bit_low + 1, 15);
    green_4bit_high = CLAMP(0, green_4bit_low + 1, 15);
    blue_4bit_high = CLAMP(0, blue_4bit_low + 1, 15);

    red_low = (red_4bit_low << 4) | (red_4bit_low >> 0);
    green_low = (green_4bit_low << 4) | (green_4bit_low >> 0);
    blue_low = (blue_4bit_low << 4) | (blue_4bit_low >> 0);

    red_high = (red_4bit_high << 4) | (red_4bit_high >> 0);
    green_high = (green_4bit_high << 4) | (green_4bit_high >> 0);
    blue_high = (blue_4bit_high << 4) | (blue_4bit_high >> 0);

    kr = (kzFloat)red_high - (kzFloat)red_low;
    kg = (kzFloat)green_high - (kzFloat)green_low;
    kb = (kzFloat)blue_high - (kzFloat)blue_low;

    /* Note that dr, dg, and db are all negative. */
    dr = red_low - red_average;
    dg = green_low - green_average;
    db = blue_low - blue_average;

    /* Use straight (nonperceptive) weights. */
    wR2 = 1.0f;
    wG2 = 1.0f;
    wB2 = 1.0f;

    /* The zeroth table entry shows the error between the original (averaged) color */
    /* and the closest match along a line that starts in */
    /* (R, G, B) = (low_color[0], low_color[1], low_color[2]) and has the direction vector (1, 1, 1). */
    /* The next table entry is (low_color[0] + kr, low_color[1], low_color[2]) etc. */
    /**/
    lowhightable[0] = wR2 * wG2 * SQUARE((dr +  0) - (dg +  0)) + wR2 * wB2 * SQUARE((dr +  0) - (db +  0)) + wG2 * wB2 * SQUARE((dg +  0) - (db +  0));
    lowhightable[1] = wR2 * wG2 * SQUARE((dr + kr) - (dg +  0)) + wR2 * wB2 * SQUARE((dr + kr) - (db +  0)) + wG2 * wB2 * SQUARE((dg +  0) - (db +  0));
    lowhightable[2] = wR2 * wG2 * SQUARE((dr +  0) - (dg + kg)) + wR2 * wB2 * SQUARE((dr +  0) - (db +  0)) + wG2 * wB2 * SQUARE((dg + kg) - (db +  0));
    lowhightable[3] = wR2 * wG2 * SQUARE((dr +  0) - (dg +  0)) + wR2 * wB2 * SQUARE((dr +  0) - (db + kb)) + wG2 * wB2 * SQUARE((dg +  0) - (db + kb));
    lowhightable[4] = wR2 * wG2 * SQUARE((dr + kr) - (dg + kg)) + wR2 * wB2 * SQUARE((dr + kr) - (db +  0)) + wG2 * wB2 * SQUARE((dg + kg) - (db +  0));
    lowhightable[5] = wR2 * wG2 * SQUARE((dr + kr) - (dg +  0)) + wR2 * wB2 * SQUARE((dr + kr) - (db + kb)) + wG2 * wB2 * SQUARE((dg +  0) - (db + kb));
    lowhightable[6] = wR2 * wG2 * SQUARE((dr +  0) - (dg + kg)) + wR2 * wB2 * SQUARE((dr +  0) - (db + kb)) + wG2 * wB2 * SQUARE((dg + kg) - (db + kb));
    lowhightable[7] = wR2 * wG2 * SQUARE((dr + kr) - (dg + kg)) + wR2 * wB2 * SQUARE((dr + kr) - (db + kb)) + wG2 * wB2 * SQUARE((dg + kg) - (db + kb));

    min_value = lowhightable[0];
    min_index = 0;

    for (q = 1; q < 8; q++)
    {
        if (lowhightable[q] < min_value)
        {
            min_value = lowhightable[q];
            min_index = q;
        }
    }

    low_color[0] = (kzU8)red_4bit_low;
    low_color[1] = (kzU8)green_4bit_low;
    low_color[2] = (kzU8)blue_4bit_low;

    high_color[0] = (kzU8)red_4bit_high;
    high_color[1] = (kzU8)green_4bit_high;
    high_color[2] = (kzU8)blue_4bit_high;

    switch (min_index)
    {
        case 0:
        {
            /* Since the step size is always 17 in RGB444 format (15 * 17 = 255), */
            /* kr = kg = kb = 17, which means that case 0 and case 7 will */
            /* always have equal projected error. Choose the one that is */
            /* closer to the desired color. */
            if (dr * dr + dg * dg + db * db > 3 * 8 * 8)
            {
                enc_color[0] = high_color[0];
                enc_color[1] = high_color[1];
                enc_color[2] = high_color[2];
            }
            else
            {
                enc_color[0] = low_color[0];
                enc_color[1] = low_color[1];
                enc_color[2] = low_color[2];
            }
            break;
        }

        case 1:
        {
            enc_color[0] = high_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 2:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 3:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 4:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 5:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 6:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 7:   
        {
            if (dr * dr + dg * dg + db * db > 3 * 8 * 8)
            {
                enc_color[0] = high_color[0];
                enc_color[1] = high_color[1];
                enc_color[2] = high_color[2];
            }
            else
            {
                enc_color[0] = low_color[0];
                enc_color[1] = low_color[1];
                enc_color[2] = low_color[2];
            }
            break;
        }

        default:
        {
            /* Not sure if this is error case or not. */
        }
    }

    /* Expand 5 - bit encoded color to 8 - bit color */
    avg_color[0] = (kzU8)((enc_color[0] << 3) | (enc_color[0] >> 2));
    avg_color[1] = (kzU8)((enc_color[1] << 3) | (enc_color[1] >> 2));
    avg_color[2] = (kzU8)((enc_color[2] << 3) | (enc_color[2] >> 2));
}


/* The below code quantizes a kzFloat RGB value to RGB555. */
/**/
/* It uses a trick: Since this color will later be modified with equal amounts in R, G and B, */
/* (effectively adding k * (1, 1, 1) to the color) we quantize not to the nearest RGB555 point, */
/* but to the nearest line starting in an RGB555 point with the direction vector (1, 1, 1). */
/**/
/* (See the presentation http://www.jacobstrom.com/publications/PACKMAN.ppt for more info.) */
/**/
static void ktcEtcQuantize555ColorCombined_internal(const kzFloat* avg_col_in, kzInt* enc_color, kzU8* avg_color)
{
    kzFloat dr, dg, db;
    kzFloat kr, kg, kb;
    kzFloat wR2, wG2, wB2;
    kzU8 low_color[3];
    kzU8 high_color[3];
    kzFloat lowhightable[8];
    kzInt q;
    kzFloat kval = (kzFloat)(255.0/31.0);


    /* These are the values that we want to have: */
    kzFloat red_average, green_average, blue_average;

    kzInt red_5bit_low, green_5bit_low, blue_5bit_low;
    kzInt red_5bit_high, green_5bit_high, blue_5bit_high;
   
    /* These are the values that we approximate with: */
    kzInt red_low, green_low, blue_low;
    kzInt red_high, green_high, blue_high;

    kzFloat min_value;
    kzInt min_index;

    red_average = avg_col_in[0];
    green_average = avg_col_in[1];
    blue_average = avg_col_in[2];

    /* Find the 5 - bit reconstruction levels red_low, red_high */
    /* so that red_average is in interval [red_low, red_high]. */
    /* (The same with green and blue.) */

    red_5bit_low = (kzInt)(red_average/kval);
    green_5bit_low = (kzInt)(green_average/kval);
    blue_5bit_low = (kzInt)(blue_average/kval);

    red_5bit_high = CLAMP(0, red_5bit_low + 1, 31);
    green_5bit_high = CLAMP(0, green_5bit_low + 1, 31);
    blue_5bit_high = CLAMP(0, blue_5bit_low + 1, 31);

    red_low = (red_5bit_low << 3) | (red_5bit_low >> 2);
    green_low = (green_5bit_low << 3) | (green_5bit_low >> 2);
    blue_low = (blue_5bit_low << 3) | (blue_5bit_low >> 2);

    red_high = (red_5bit_high << 3) | (red_5bit_high >> 2);
    green_high = (green_5bit_high << 3) | (green_5bit_high >> 2);
    blue_high = (blue_5bit_high << 3) | (blue_5bit_high >> 2);

    kr = (kzFloat)red_high - (kzFloat)red_low;
    kg = (kzFloat)green_high - (kzFloat)green_low;
    kb = (kzFloat)blue_high - (kzFloat)blue_low;

    /* Note that dr, dg, and db are all negative. */

    dr = red_low - red_average;
    dg = green_low - green_average;
    db = blue_low - blue_average;

    /* Use straight (nonperceptive) weights. */
    wR2 = 1.0f;
    wG2 = 1.0f;
    wB2 = 1.0f;

    /* The zeroth table entry shows the error between the original (averaged) color */
    /* and the closest match along a line that starts in */
    /* (R, G, B) = (low_color[0], low_color[1], low_color[2]) and has the direction vector (1, 1, 1). */
    /* The next table entry is (low_color[0] + kr, low_color[1], low_color[2]) etc. */
    /**/
    lowhightable[0] = wR2 * wG2 * SQUARE((dr +  0) - (dg +  0)) + wR2 * wB2 * SQUARE((dr +  0) - (db +  0)) + wG2 * wB2 * SQUARE((dg +  0) - (db +  0));
    lowhightable[1] = wR2 * wG2 * SQUARE((dr + kr) - (dg +  0)) + wR2 * wB2 * SQUARE((dr + kr) - (db +  0)) + wG2 * wB2 * SQUARE((dg +  0) - (db +  0));
    lowhightable[2] = wR2 * wG2 * SQUARE((dr +  0) - (dg + kg)) + wR2 * wB2 * SQUARE((dr +  0) - (db +  0)) + wG2 * wB2 * SQUARE((dg + kg) - (db +  0));
    lowhightable[3] = wR2 * wG2 * SQUARE((dr +  0) - (dg +  0)) + wR2 * wB2 * SQUARE((dr +  0) - (db + kb)) + wG2 * wB2 * SQUARE((dg +  0) - (db + kb));
    lowhightable[4] = wR2 * wG2 * SQUARE((dr + kr) - (dg + kg)) + wR2 * wB2 * SQUARE((dr + kr) - (db +  0)) + wG2 * wB2 * SQUARE((dg + kg) - (db +  0));
    lowhightable[5] = wR2 * wG2 * SQUARE((dr + kr) - (dg +  0)) + wR2 * wB2 * SQUARE((dr + kr) - (db + kb)) + wG2 * wB2 * SQUARE((dg +  0) - (db + kb));
    lowhightable[6] = wR2 * wG2 * SQUARE((dr +  0) - (dg + kg)) + wR2 * wB2 * SQUARE((dr +  0) - (db + kb)) + wG2 * wB2 * SQUARE((dg + kg) - (db + kb));
    lowhightable[7] = wR2 * wG2 * SQUARE((dr + kr) - (dg + kg)) + wR2 * wB2 * SQUARE((dr + kr) - (db + kb)) + wG2 * wB2 * SQUARE((dg + kg) - (db + kb));


    min_value = lowhightable[0];
    min_index = 0;

    for (q = 1; q < 8; q++)
    {
        if (lowhightable[q] < min_value)
        {
            min_value = lowhightable[q];
            min_index = q;
        }
    }

    low_color[0] = (kzU8)red_5bit_low;
    low_color[1] = (kzU8)green_5bit_low;
    low_color[2] = (kzU8)blue_5bit_low;

    high_color[0] = (kzU8)red_5bit_high;
    high_color[1] = (kzU8)green_5bit_high;
    high_color[2] = (kzU8)blue_5bit_high;

    switch (min_index)
    {
        case 0:
        {
            enc_color[0] = low_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 1:
        {
            enc_color[0] = high_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 2:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 3:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 4:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 5:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 6:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 7:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        default:
        {
            /* Not sure if this is error case or not. */
        }
    }

    /* Expand 5 - bit encoded color to 8 - bit color */
    avg_color[0] = (kzU8)((enc_color[0] << 3) | (enc_color[0] >> 2));
    avg_color[1] = (kzU8)((enc_color[1] << 3) | (enc_color[1] >> 2));
    avg_color[2] = (kzU8)((enc_color[2] << 3) | (enc_color[2] >> 2));
   
}


/* The below code quantizes a kzFloat RGB value to RGB444. */
/* It is thus the same as the above function quantize444ColorCombined(), but it uses a */
/* weighted error metric instead. */
/**/
/* It uses the same trick: Since this color will later be modified with equal amounts in R, G and B, */
/* (effectively adding k * (1, 1, 1) to the color) we quantize not to the nearest RGB555 point, */
/* but to the nearest line starting in an RGB555 point with the direction vector (1, 1, 1). */
/**/
/* (See the presentation http://www.jacobstrom.com/publications/PACKMAN.ppt for more info.) */
/**/
static void ktcEtcQuantize444ColorCombinedPerceptual_internal(const kzFloat* avg_col_in, kzInt* enc_color, kzU8* avg_color)
{
    kzFloat dr, dg, db;
    kzFloat kr, kg, kb;
    kzFloat wR2, wG2, wB2;
    kzU8 low_color[3];
    kzU8 high_color[3];
    kzFloat lowhightable[8];
    kzInt q;
    kzFloat kval = (kzFloat)(255.0/15.0);


    /* These are the values that we want to have: */
    kzFloat red_average, green_average, blue_average;

    kzInt red_4bit_low, green_4bit_low, blue_4bit_low;
    kzInt red_4bit_high, green_4bit_high, blue_4bit_high;
   
    /* These are the values that we approximate with: */
    kzInt red_low, green_low, blue_low;
    kzInt red_high, green_high, blue_high;
   
    kzFloat min_value;
    kzInt min_index;

    red_average = avg_col_in[0];
    green_average = avg_col_in[1];
    blue_average = avg_col_in[2];

    /* Find the 5 - bit reconstruction levels red_low, red_high */
    /* so that red_average is in interval [red_low, red_high]. */
    /* (The same with green and blue.) */

    red_4bit_low = (kzInt)(red_average/kval);
    green_4bit_low = (kzInt)(green_average/kval);
    blue_4bit_low = (kzInt)(blue_average/kval);

    red_4bit_high = CLAMP(0, red_4bit_low + 1, 15);
    green_4bit_high = CLAMP(0, green_4bit_low + 1, 15);
    blue_4bit_high = CLAMP(0, blue_4bit_low + 1, 15);

    red_low = (red_4bit_low << 4) | (red_4bit_low >> 0);
    green_low = (green_4bit_low << 4) | (green_4bit_low >> 0);
    blue_low = (blue_4bit_low << 4) | (blue_4bit_low >> 0);

    red_high = (red_4bit_high << 4) | (red_4bit_high >> 0);
    green_high = (green_4bit_high << 4) | (green_4bit_high >> 0);
    blue_high = (blue_4bit_high << 4) | (blue_4bit_high >> 0);

    low_color[0] = (kzU8)red_4bit_low;
    low_color[1] = (kzU8)green_4bit_low;
    low_color[2] = (kzU8)blue_4bit_low;

    high_color[0] = (kzU8)red_4bit_high;
    high_color[1] = (kzU8)green_4bit_high;
    high_color[2] = (kzU8)blue_4bit_high;

    kr = (kzFloat)red_high - (kzFloat)red_low;
    kg = (kzFloat)green_high - (kzFloat)green_low;
    kb = (kzFloat)blue_high - (kzFloat)blue_low;

    /* Note that dr, dg, and db are all negative. */

    dr = red_low - red_average;
    dg = green_low - green_average;
    db = blue_low - blue_average;

    /* Perceptual weights to use */
    wR2 = (kzFloat)PERCEPTUAL_WEIGHT_R_SQUARED;
    wG2 = (kzFloat)PERCEPTUAL_WEIGHT_G_SQUARED;
    wB2 = (kzFloat)PERCEPTUAL_WEIGHT_B_SQUARED;

    /* The zeroth table entry shows the error between the original (averaged) color */
    /* and the closest match along a line that starts in */
    /* (R, G, B) = (low_color[0], low_color[1], low_color[2]) and has the direction vector (1, 1, 1). */
    /* The next table entry is (low_color[0] + kr, low_color[1], low_color[2]) etc. */
    /**/
    lowhightable[0] = wR2 * wG2 * SQUARE((dr +  0) - (dg +  0)) + wR2 * wB2 * SQUARE((dr +  0) - (db +  0)) + wG2 * wB2 * SQUARE((dg +  0) - (db +  0));
    lowhightable[1] = wR2 * wG2 * SQUARE((dr + kr) - (dg +  0)) + wR2 * wB2 * SQUARE((dr + kr) - (db +  0)) + wG2 * wB2 * SQUARE((dg +  0) - (db +  0));
    lowhightable[2] = wR2 * wG2 * SQUARE((dr +  0) - (dg + kg)) + wR2 * wB2 * SQUARE((dr +  0) - (db +  0)) + wG2 * wB2 * SQUARE((dg + kg) - (db +  0));
    lowhightable[3] = wR2 * wG2 * SQUARE((dr +  0) - (dg +  0)) + wR2 * wB2 * SQUARE((dr +  0) - (db + kb)) + wG2 * wB2 * SQUARE((dg +  0) - (db + kb));
    lowhightable[4] = wR2 * wG2 * SQUARE((dr + kr) - (dg + kg)) + wR2 * wB2 * SQUARE((dr + kr) - (db +  0)) + wG2 * wB2 * SQUARE((dg + kg) - (db +  0));
    lowhightable[5] = wR2 * wG2 * SQUARE((dr + kr) - (dg +  0)) + wR2 * wB2 * SQUARE((dr + kr) - (db + kb)) + wG2 * wB2 * SQUARE((dg +  0) - (db + kb));
    lowhightable[6] = wR2 * wG2 * SQUARE((dr +  0) - (dg + kg)) + wR2 * wB2 * SQUARE((dr +  0) - (db + kb)) + wG2 * wB2 * SQUARE((dg + kg) - (db + kb));
    lowhightable[7] = wR2 * wG2 * SQUARE((dr + kr) - (dg + kg)) + wR2 * wB2 * SQUARE((dr + kr) - (db + kb)) + wG2 * wB2 * SQUARE((dg + kg) - (db + kb));


    min_value = lowhightable[0];
    min_index = 0;

    for (q = 1; q < 8; q++)
    {
        if (lowhightable[q] < min_value)
        {
            min_value = lowhightable[q];
            min_index = q;
        }
    }

    switch (min_index)
    {
        case 0:
        {
            enc_color[0] = low_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 1:
        {
            enc_color[0] = high_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 2:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 3:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 4:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 5:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 6:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 7:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        default:
        {
            /* Not sure if this is error case or not. */
        }
    }

    /* Expand encoded color to eight bits */
    avg_color[0] = (kzU8)((enc_color[0] << 4) | enc_color[0]);
    avg_color[1] = (kzU8)((enc_color[1] << 4) | enc_color[1]);
    avg_color[2] = (kzU8)((enc_color[2] << 4) | enc_color[2]);
}


/* The below code quantizes a kzFloat RGB value to RGB555. */
/* It is thus the same as the above function quantize555ColorCombined(), but it uses a */
/* weighted error metric instead. */
/**/
/* It uses the same trick: Since this color will later be modified with equal amounts in R, G and B, */
/* (effectively adding k * (1, 1, 1) to the color) we quantize not to the nearest RGB555 point, */
/* but to the nearest line starting in an RGB555 point with the direction vector (1, 1, 1). */
/**/
/* (See the presentation http://www.jacobstrom.com/publications/PACKMAN.ppt for more info.) */
/**/
static void ktcEtcQuantize555ColorCombinedPerceptual_internal(const kzFloat* avg_col_in, kzInt* enc_color, kzU8* avg_color)
{
    kzFloat dr, dg, db;
    kzFloat kr, kg, kb;
    kzFloat wR2, wG2, wB2;
    kzU8 low_color[3];
    kzU8 high_color[3];
    kzFloat lowhightable[8];
    kzInt q;
    kzFloat kval = (kzFloat)(255.0/31.0);


    /* These are the values that we want to have: */
    kzFloat red_average, green_average, blue_average;

    kzInt red_5bit_low, green_5bit_low, blue_5bit_low;
    kzInt red_5bit_high, green_5bit_high, blue_5bit_high;
   
    /* These are the values that we approximate with: */
    kzInt red_low, green_low, blue_low;
    kzInt red_high, green_high, blue_high;

    kzFloat min_value;
    kzInt min_index;

    red_average = avg_col_in[0];
    green_average = avg_col_in[1];
    blue_average = avg_col_in[2];

    /* Find the 5 - bit reconstruction levels red_low, red_high */
    /* so that red_average is in interval [red_low, red_high]. */
    /* (The same with green and blue.) */

    red_5bit_low = (kzInt)(red_average/kval);
    green_5bit_low = (kzInt)(green_average/kval);
    blue_5bit_low = (kzInt)(blue_average/kval);

    red_5bit_high = CLAMP(0, red_5bit_low + 1, 31);
    green_5bit_high = CLAMP(0, green_5bit_low + 1, 31);
    blue_5bit_high = CLAMP(0, blue_5bit_low + 1, 31);

    red_low = (red_5bit_low << 3) | (red_5bit_low >> 2);
    green_low = (green_5bit_low << 3) | (green_5bit_low >> 2);
    blue_low = (blue_5bit_low << 3) | (blue_5bit_low >> 2);

    red_high = (red_5bit_high << 3) | (red_5bit_high >> 2);
    green_high = (green_5bit_high << 3) | (green_5bit_high >> 2);
    blue_high = (blue_5bit_high << 3) | (blue_5bit_high >> 2);

    low_color[0] = (kzU8)red_5bit_low;
    low_color[1] = (kzU8)green_5bit_low;
    low_color[2] = (kzU8)blue_5bit_low;

    high_color[0] = (kzU8)red_5bit_high;
    high_color[1] = (kzU8)green_5bit_high;
    high_color[2] = (kzU8)blue_5bit_high;

    kr = (kzFloat)red_high - (kzFloat)red_low;
    kg = (kzFloat)green_high - (kzFloat)green_low;
    kb = (kzFloat)blue_high - (kzFloat)blue_low;

    /* Note that dr, dg, and db are all negative. */

    dr = red_low - red_average;
    dg = green_low - green_average;
    db = blue_low - blue_average;

    /* Perceptual weights to use */
    wR2 = (kzFloat)PERCEPTUAL_WEIGHT_R_SQUARED;
    wG2 = (kzFloat)PERCEPTUAL_WEIGHT_G_SQUARED;
    wB2 = (kzFloat)PERCEPTUAL_WEIGHT_B_SQUARED;

    /* The zeroth table entry shows the error between the original (averaged) color */
    /* and the closest match along a line that starts in */
    /* (R, G, B) = (low_color[0], low_color[1], low_color[2]) and has the direction vector (1, 1, 1). */
    /* The next table entry is (low_color[0] + kr, low_color[1], low_color[2]) etc. */
    /**/
    lowhightable[0] = wR2 * wG2 * SQUARE((dr +  0) - (dg +  0)) + wR2 * wB2 * SQUARE((dr +  0) - (db +  0)) + wG2 * wB2 * SQUARE((dg +  0) - (db +  0));
    lowhightable[1] = wR2 * wG2 * SQUARE((dr + kr) - (dg +  0)) + wR2 * wB2 * SQUARE((dr + kr) - (db +  0)) + wG2 * wB2 * SQUARE((dg +  0) - (db +  0));
    lowhightable[2] = wR2 * wG2 * SQUARE((dr +  0) - (dg + kg)) + wR2 * wB2 * SQUARE((dr +  0) - (db +  0)) + wG2 * wB2 * SQUARE((dg + kg) - (db +  0));
    lowhightable[3] = wR2 * wG2 * SQUARE((dr +  0) - (dg +  0)) + wR2 * wB2 * SQUARE((dr +  0) - (db + kb)) + wG2 * wB2 * SQUARE((dg +  0) - (db + kb));
    lowhightable[4] = wR2 * wG2 * SQUARE((dr + kr) - (dg + kg)) + wR2 * wB2 * SQUARE((dr + kr) - (db +  0)) + wG2 * wB2 * SQUARE((dg + kg) - (db +  0));
    lowhightable[5] = wR2 * wG2 * SQUARE((dr + kr) - (dg +  0)) + wR2 * wB2 * SQUARE((dr + kr) - (db + kb)) + wG2 * wB2 * SQUARE((dg +  0) - (db + kb));
    lowhightable[6] = wR2 * wG2 * SQUARE((dr +  0) - (dg + kg)) + wR2 * wB2 * SQUARE((dr +  0) - (db + kb)) + wG2 * wB2 * SQUARE((dg + kg) - (db + kb));
    lowhightable[7] = wR2 * wG2 * SQUARE((dr + kr) - (dg + kg)) + wR2 * wB2 * SQUARE((dr + kr) - (db + kb)) + wG2 * wB2 * SQUARE((dg + kg) - (db + kb));


    min_value = lowhightable[0];
    min_index = 0;

    for (q = 1; q < 8; q++)
    {
        if (lowhightable[q] < min_value)
        {
            min_value = lowhightable[q];
            min_index = q;
        }
    }

    switch (min_index)
    {
        case 0:
        {
            enc_color[0] = low_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 1:
        {
            enc_color[0] = high_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 2:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 3:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 4:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = low_color[2];
            break;
        }

        case 5:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = low_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 6:   
        {
            enc_color[0] = low_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        case 7:   
        {
            enc_color[0] = high_color[0];
            enc_color[1] = high_color[1];
            enc_color[2] = high_color[2];
            break;
        }

        default:
        {
            /* Not sure if this is error case or not. */
        }
    }

    /* Expand 5 - bit encoded color to 8 - bit color */
    avg_color[0] = (kzU8)((enc_color[0] << 3) | (enc_color[0] >> 2));
    avg_color[1] = (kzU8)((enc_color[1] << 3) | (enc_color[1] >> 2));
    avg_color[2] = (kzU8)((enc_color[2] << 3) | (enc_color[2] >> 2));
}


static void ktcEtcCompressBlockDiffFlipSlow_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                     kzUint* compressed1, kzUint* compressed2)
{
    kzUint compressed1_norm_diff = 0, compressed2_norm_diff = 0;
    kzUint compressed1_norm_444, compressed2_norm_444;
    kzUint compressed1_flip_diff = 0, compressed2_flip_diff = 0;
    kzUint compressed1_flip_444, compressed2_flip_444;
    kzUint best_err_norm_diff = 255 * 255 * 8 * 3;
    kzUint best_err_norm_444;
    kzUint best_err_flip_diff = 255 * 255 * 8 * 3;
    kzUint best_err_flip_444;
    kzU8 avg_color_quant1[3], avg_color_quant2[3];

    kzFloat avg_color_kzFloat1[3], avg_color_kzFloat2[3];
    kzInt enc_color1[3], enc_color2[3], diff[3];
    kzInt enc_base1[3], enc_base2[3];
    kzInt enc_try1[3], enc_try2[3];
    kzUint best_pixel_indices1_MSB = 0;
    kzUint best_pixel_indices1_LSB = 0;
    kzUint best_pixel_indices2_MSB = 0;
    kzUint best_pixel_indices2_LSB = 0;

    kzUint best_table1 = 0, best_table2 = 0;
    kzInt diffbit;

    kzInt norm_err;
    kzInt flip_err = 0;
    kzInt minerr;
    kzInt dr1, dg1, db1, dr2, dg2, db2;

    /* First try normal blocks 2x4: */

    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx + 2, starty, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));
   
    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= SLOW_TRY_MIN) && (diff[0] <= SLOW_TRY_MAX) && (diff[1] >= SLOW_TRY_MIN) && (diff[1] <= SLOW_TRY_MAX) && (diff[2] >= SLOW_TRY_MIN) && (diff[2] <= SLOW_TRY_MAX))
    {
        kzInt err1[SLOW_SCAN_RANGE][SLOW_SCAN_RANGE][SLOW_SCAN_RANGE];
        kzInt err2[SLOW_SCAN_RANGE][SLOW_SCAN_RANGE][SLOW_SCAN_RANGE];

        diffbit = 1;

        enc_base1[0] = enc_color1[0];
        enc_base1[1] = enc_color1[1];
        enc_base1[2] = enc_color1[2];
        enc_base2[0] = enc_color2[0];
        enc_base2[1] = enc_color2[1];
        enc_base2[2] = enc_color2[2];


        /* left part of block */
        for (dr1 = SLOW_SCAN_MIN; dr1 < SLOW_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = SLOW_SCAN_MIN; dg1 < SLOW_SCAN_MAX + 1; dg1++)
            {
                for (db1 = SLOW_SCAN_MIN; db1 < SLOW_SCAN_MAX + 1; db1++)
                {
                    enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                    enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                    enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);

                    avg_color_quant1[0] = (kzU8)(enc_try1[0] << 3 | (enc_try1[0] >> 2));
                    avg_color_quant1[1] = (kzU8)(enc_try1[1] << 3 | (enc_try1[1] >> 2));
                    avg_color_quant1[2] = (kzU8)(enc_try1[2] << 3 | (enc_try1[2] >> 2));

                    /* left part of block */
                    err1[dr1 + SLOW_SCAN_OFFSET][dg1 + SLOW_SCAN_OFFSET][db1 + SLOW_SCAN_OFFSET] = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
                }
            }
        }

        /* right part of block */
        for (dr2 = SLOW_SCAN_MIN; dr2 < SLOW_SCAN_MAX + 1; dr2++)
        {
            for (dg2 = SLOW_SCAN_MIN; dg2 < SLOW_SCAN_MAX + 1; dg2++)
            {
                for (db2 = SLOW_SCAN_MIN; db2 < SLOW_SCAN_MAX + 1; db2++)
                {
                    enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                    enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                    enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                    avg_color_quant2[0] = (kzU8)(enc_try2[0] << 3 | (enc_try2[0] >> 2));
                    avg_color_quant2[1] = (kzU8)(enc_try2[1] << 3 | (enc_try2[1] >> 2));
                    avg_color_quant2[2] = (kzU8)(enc_try2[2] << 3 | (enc_try2[2] >> 2));

                    /* left part of block */
                    err2[dr2 + SLOW_SCAN_OFFSET][dg2 + SLOW_SCAN_OFFSET][db2 + SLOW_SCAN_OFFSET] = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);
                }
            }
        }

        /* Now see what combinations are both low in error and possible to */
        /* encode differentially. */

        minerr = 255 * 255 * 3 * 8 * 2;

        for (dr1 = SLOW_SCAN_MIN; dr1 < SLOW_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = SLOW_SCAN_MIN; dg1 < SLOW_SCAN_MAX + 1; dg1++)
            {
                for (db1 = SLOW_SCAN_MIN; db1 < SLOW_SCAN_MAX + 1; db1++)
                {
                    for (dr2 = SLOW_SCAN_MIN; dr2 < SLOW_SCAN_MAX + 1; dr2++)
                    {
                        for (dg2 = SLOW_SCAN_MIN; dg2 < SLOW_SCAN_MAX + 1; dg2++)
                        {
                            for (db2 = SLOW_SCAN_MIN; db2 < SLOW_SCAN_MAX + 1; db2++)
                            {                               
                                enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                                enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                                enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);
                                enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                                enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                                enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                                /* We must make sure that the difference between the tries still is less than allowed */

                                diff[0] = enc_try2[0] - enc_try1[0];   
                                diff[1] = enc_try2[1] - enc_try1[1];   
                                diff[2] = enc_try2[2] - enc_try1[2];

                                if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
                                {
                                    /* The diff is OK, calculate total error: */
                                   
                                    kzInt err = err1[dr1 + SLOW_SCAN_OFFSET][dg1 + SLOW_SCAN_OFFSET][db1 + SLOW_SCAN_OFFSET] + err2[dr2 + SLOW_SCAN_OFFSET][dg2 + SLOW_SCAN_OFFSET][db2 + SLOW_SCAN_OFFSET];

                                    if (err < minerr)
                                    {
                                        minerr = err;

                                        enc_color1[0] = enc_try1[0];
                                        enc_color1[1] = enc_try1[1];
                                        enc_color1[2] = enc_try1[2];
                                        enc_color2[0] = enc_try2[0];
                                        enc_color2[1] = enc_try2[1];
                                        enc_color2[2] = enc_try2[2];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        best_err_norm_diff = minerr;
        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

       
        /* Pack bits into the first word. */


   
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm_diff = 0;
        PUTBITSHIGH(compressed1_norm_diff, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_norm_diff, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_norm_diff, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_norm_diff, diff[2], 3, 42);

       
        /* left part of block */
        ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty,
                                                                       avg_color_quant1, &best_table1,
                                                                       &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty,
                                                                       avg_color_quant2, &best_table2,
                                                                       &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm_diff, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm_diff, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm_diff, 0, 1, 32);

        compressed2_norm_diff = 0;
        PUTBITS(compressed2_norm_diff, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices2_LSB), 8, 15);

    }
    /* We should do this in any case... */
    {
        kzInt besterr = 255 * 255 * 3 * 8;
        kzInt bestri = 0, bestgi = 0, bestbi = 0;
        kzInt ri, gi, bi;

        diffbit = 0;
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */


        /* Color for left block */


        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;

                    enc_color1[0] = ri;
                    enc_color1[1] = gi;
                    enc_color1[2] = bi;

                    avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
                    avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
                    avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        norm_err = besterr;
       
        enc_color1[0] = bestri;
        enc_color1[1] = bestgi;
        enc_color1[2] = bestbi;
        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);


        /* Color for right block */

        besterr = 255 * 255 * 3 * 8;
        bestri = 0; bestgi = 0; bestbi = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color2[0] = ri;
                    enc_color2[1] = gi;
                    enc_color2[2] = bi;

                    avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
                    avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
                    avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }


        norm_err += besterr;
        best_err_norm_444 = norm_err;

        enc_color2[0] = bestri;
        enc_color2[1] = bestgi;
        enc_color2[2] = bestbi;
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);


        /* Pack bits into the first word. */
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm_444 = 0;
        PUTBITSHIGH(compressed1_norm_444, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[2], 4, 43);


        /* left part of block */
        ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm_444, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm_444, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm_444, 0, 1, 32);

        compressed2_norm_444 = 0;
        PUTBITS(compressed2_norm_444, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm_444, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm_444, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm_444, (best_pixel_indices2_LSB), 8, 15);

    }

    /* Now try flipped blocks 4x2: */

    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty + 2, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= SLOW_TRY_MIN) && (diff[0] <= SLOW_TRY_MAX) && (diff[1] >= SLOW_TRY_MIN) && (diff[1] <= SLOW_TRY_MAX) && (diff[2] >= SLOW_TRY_MIN) && (diff[2] <= SLOW_TRY_MAX))
    {
        kzInt err1[SLOW_SCAN_RANGE][SLOW_SCAN_RANGE][SLOW_SCAN_RANGE];
        kzInt err2[SLOW_SCAN_RANGE][SLOW_SCAN_RANGE][SLOW_SCAN_RANGE];

        diffbit = 1;

        enc_base1[0] = enc_color1[0];
        enc_base1[1] = enc_color1[1];
        enc_base1[2] = enc_color1[2];
        enc_base2[0] = enc_color2[0];
        enc_base2[1] = enc_color2[1];
        enc_base2[2] = enc_color2[2];

        /* upper part of block */
        for (dr1 = SLOW_SCAN_MIN; dr1 < SLOW_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = SLOW_SCAN_MIN; dg1 < SLOW_SCAN_MAX + 1; dg1++)
            {
                for (db1 = SLOW_SCAN_MIN; db1 < SLOW_SCAN_MAX + 1; db1++)
                {
                    enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                    enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                    enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);

                    avg_color_quant1[0] = (kzU8)(enc_try1[0] << 3 | (enc_try1[0] >> 2));
                    avg_color_quant1[1] = (kzU8)(enc_try1[1] << 3 | (enc_try1[1] >> 2));
                    avg_color_quant1[2] = (kzU8)(enc_try1[2] << 3 | (enc_try1[2] >> 2));

                    /* upper part of block */
                    err1[dr1 + SLOW_SCAN_OFFSET][dg1 + SLOW_SCAN_OFFSET][db1 + SLOW_SCAN_OFFSET] = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
                }
            }
        }

        /* lower part of block */
        for (dr2 = SLOW_SCAN_MIN; dr2 < SLOW_SCAN_MAX + 1; dr2++)
        {
            for (dg2 = SLOW_SCAN_MIN; dg2 < SLOW_SCAN_MAX + 1; dg2++)
            {
                for (db2 = SLOW_SCAN_MIN; db2 < SLOW_SCAN_MAX + 1; db2++)
                {
                    enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                    enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                    enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                    avg_color_quant2[0] = (kzU8)(enc_try2[0] << 3 | (enc_try2[0] >> 2));
                    avg_color_quant2[1] = (kzU8)(enc_try2[1] << 3 | (enc_try2[1] >> 2));
                    avg_color_quant2[2] = (kzU8)(enc_try2[2] << 3 | (enc_try2[2] >> 2));

                    /* lower part of block */
                    err2[dr2 + SLOW_SCAN_OFFSET][dg2 + SLOW_SCAN_OFFSET][db2 + SLOW_SCAN_OFFSET] = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);
                }
            }
        }

        /* Now see what combinations are both low in error and possible to */
        /* encode differentially. */

        minerr = 255 * 255 * 3 * 8 * 2;

        for (dr1 = SLOW_SCAN_MIN; dr1 < SLOW_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = SLOW_SCAN_MIN; dg1 < SLOW_SCAN_MAX + 1; dg1++)
            {
                for (db1 = SLOW_SCAN_MIN; db1 < SLOW_SCAN_MAX + 1; db1++)
                {
                    for (dr2 = SLOW_SCAN_MIN; dr2 < SLOW_SCAN_MAX + 1; dr2++)
                    {
                        for (dg2 = SLOW_SCAN_MIN; dg2 < SLOW_SCAN_MAX + 1; dg2++)
                        {
                            for (db2 = SLOW_SCAN_MIN; db2 < SLOW_SCAN_MAX + 1; db2++)
                            {                               
                                enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                                enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                                enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);
                                enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                                enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                                enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                                /* We must make sure that the difference between the tries still is less than allowed */

                                diff[0] = enc_try2[0] - enc_try1[0];   
                                diff[1] = enc_try2[1] - enc_try1[1];   
                                diff[2] = enc_try2[2] - enc_try1[2];

                                if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
                                {
                                    /* The diff is OK, calculate total error: */
                                   
                                    kzInt err = err1[dr1 + SLOW_SCAN_OFFSET][dg1 + SLOW_SCAN_OFFSET][db1 + SLOW_SCAN_OFFSET] + err2[dr2 + SLOW_SCAN_OFFSET][dg2 + SLOW_SCAN_OFFSET][db2 + SLOW_SCAN_OFFSET];

                                    if (err < minerr)
                                    {
                                        minerr = err;

                                        enc_color1[0] = enc_try1[0];
                                        enc_color1[1] = enc_try1[1];
                                        enc_color1[2] = enc_try1[2];
                                        enc_color2[0] = enc_try2[0];
                                        enc_color2[1] = enc_try2[1];
                                        enc_color2[2] = enc_try2[2];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }


        flip_err = minerr;

        best_err_flip_diff = flip_err;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

       
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        /* Pack bits into the first word. */

        compressed1_flip_diff = 0;
        PUTBITSHIGH(compressed1_flip_diff, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_flip_diff, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_flip_diff, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_flip_diff, diff[2], 3, 42);




        /* upper part of block */
        ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
        /* lower part of block */
        ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);


        PUTBITSHIGH(compressed1_flip_diff, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip_diff, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip_diff, 1, 1, 32);


        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip_diff = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);
   
    }
    {
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */


        /* Color for upper block */

        kzInt besterr = 255 * 255 * 3 * 8;
        kzInt bestri = 0, bestgi = 0, bestbi = 0;
        kzInt ri, gi, bi;

        diffbit = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color1[0] = ri;
                    enc_color1[1] = gi;
                    enc_color1[2] = bi;

                    avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
                    avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
                    avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);

                    /* upper part of block */
                    err = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        flip_err = besterr;

        enc_color1[0] = bestri;
        enc_color1[1] = bestgi;
        enc_color1[2] = bestbi;
        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);


        /* Color for lower block */

        besterr = 255 * 255 * 3 * 8;
        bestri = 0; bestgi = 0; bestbi = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color2[0] = ri;
                    enc_color2[1] = gi;
                    enc_color2[2] = bi;

                    avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
                    avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
                    avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        flip_err += besterr;
        best_err_flip_444 = flip_err;

        enc_color2[0] = bestri;
        enc_color2[1] = bestgi;
        enc_color2[2] = bestbi;
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);


        /* Pack bits into the first word. */
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_flip_444 = 0;
        PUTBITSHIGH(compressed1_flip_444, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[2], 4, 43);

        /* upper part of block */
        ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* lower part of block */
        ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip_444, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip_444, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip_444, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip_444 = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);


    }

    /* Now lets see which is the best table to use. Only 8 tables are possible. */
    {
        kzInt compressed1_norm;
        kzInt compressed2_norm;
        kzInt compressed1_flip;
        kzInt compressed2_flip;

        /* See which of the norm blocks is better */

        if (best_err_norm_diff <= best_err_norm_444)
        {
            compressed1_norm = compressed1_norm_diff;
            compressed2_norm = compressed2_norm_diff;
            norm_err = best_err_norm_diff;
        }
        else
        {
            compressed1_norm = compressed1_norm_444;
            compressed2_norm = compressed2_norm_444;
            norm_err = best_err_norm_444;
        }

        /* See which of the flip blocks is better */

        if (best_err_flip_diff <= best_err_flip_444)
        {
            compressed1_flip = compressed1_flip_diff;
            compressed2_flip = compressed2_flip_diff;
            flip_err = best_err_flip_diff;
        }
        else
        {
            compressed1_flip = compressed1_flip_444;
            compressed2_flip = compressed2_flip_444;
            flip_err = best_err_flip_444;
        }

        /* See if flip or norm is better */


        if (norm_err <= flip_err)
        {

            *compressed1 = compressed1_norm | 0;
            *compressed2 = compressed2_norm;
        }
        else
        {

            *compressed1 = compressed1_flip | 1;
            *compressed2 = compressed2_flip;
        }
    }
}

static void ktcEtcCompressBlockDiffFlipMedium_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty, kzUint* compressed1, kzUint* compressed2)
{
    kzUint compressed1_norm_diff = 0, compressed2_norm_diff = 0;
    kzUint compressed1_norm_444 = 0, compressed2_norm_444 = 0;
    kzUint compressed1_flip_diff = 0, compressed2_flip_diff = 0;
    kzUint compressed1_flip_444 = 0, compressed2_flip_444 = 0;
    kzUint best_err_norm_diff = 255 * 255 * 16 * 3;
    kzUint best_err_norm_444 = 255 * 255 * 16 * 3;
    kzUint best_err_flip_diff = 255 * 255 * 16 * 3;
    kzUint best_err_flip_444 = 255 * 255 * 16 * 3;
    kzU8 avg_color_quant1[3], avg_color_quant2[3];

    kzFloat avg_color_kzFloat1[3], avg_color_kzFloat2[3];
    kzInt enc_color1[3], enc_color2[3], diff[3];
    kzInt enc_base1[3], enc_base2[3];
    kzInt enc_try1[3], enc_try2[3];
/*   kzInt min_error = 255 * 255 * 16 * 3, err; */
    kzUint best_pixel_indices1_MSB = 0;
    kzUint best_pixel_indices1_LSB = 0;
    kzUint best_pixel_indices2_MSB = 0;
    kzUint best_pixel_indices2_LSB = 0;

    kzUint best_table1 = 0, best_table2 = 0;
    kzInt diffbit;

    kzInt norm_err = 0;
    kzInt flip_err = 0;
    kzInt minerr;
    kzInt dr1, dg1, db1, dr2, dg2, db2;

    /* First try normal blocks 2x4: */

    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx + 2, starty, avg_color_kzFloat2);



    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));
   
    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= MEDIUM_TRY_MIN) && (diff[0] <= MEDIUM_TRY_MAX) && (diff[1] >= MEDIUM_TRY_MIN) && (diff[1] <= MEDIUM_TRY_MAX) && (diff[2] >= MEDIUM_TRY_MIN) && (diff[2] <= MEDIUM_TRY_MAX))
    {
        kzInt err1[MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE];
        kzInt err2[MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE];
        diffbit = 1;

        enc_base1[0] = enc_color1[0];
        enc_base1[1] = enc_color1[1];
        enc_base1[2] = enc_color1[2];
        enc_base2[0] = enc_color2[0];
        enc_base2[1] = enc_color2[1];
        enc_base2[2] = enc_color2[2];

        /* left part of block */
        for (dr1 = MEDIUM_SCAN_MIN; dr1 < MEDIUM_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = MEDIUM_SCAN_MIN; dg1 < MEDIUM_SCAN_MAX + 1; dg1++)
            {
                for (db1 = MEDIUM_SCAN_MIN; db1 < MEDIUM_SCAN_MAX + 1; db1++)
                {
                    enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                    enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                    enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);

                    avg_color_quant1[0] = (kzU8)(enc_try1[0] << 3 | (enc_try1[0] >> 2));
                    avg_color_quant1[1] = (kzU8)(enc_try1[1] << 3 | (enc_try1[1] >> 2));
                    avg_color_quant1[2] = (kzU8)(enc_try1[2] << 3 | (enc_try1[2] >> 2));

                    /* left part of block */
                    err1[dr1 + MEDIUM_SCAN_OFFSET][dg1 + MEDIUM_SCAN_OFFSET][db1 + MEDIUM_SCAN_OFFSET] = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
                }
            }
        }

        /* right part of block */
        for (dr2 = MEDIUM_SCAN_MIN; dr2 < MEDIUM_SCAN_MAX + 1; dr2++)
        {
            for (dg2 = MEDIUM_SCAN_MIN; dg2 < MEDIUM_SCAN_MAX + 1; dg2++)
            {
                for (db2 = MEDIUM_SCAN_MIN; db2 < MEDIUM_SCAN_MAX + 1; db2++)
                {
                    enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                    enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                    enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                    avg_color_quant2[0] = (kzU8)(enc_try2[0] << 3 | (enc_try2[0] >> 2));
                    avg_color_quant2[1] = (kzU8)(enc_try2[1] << 3 | (enc_try2[1] >> 2));
                    avg_color_quant2[2] = (kzU8)(enc_try2[2] << 3 | (enc_try2[2] >> 2));

                    /* left part of block */
                    err2[dr2 + MEDIUM_SCAN_OFFSET][dg2 + MEDIUM_SCAN_OFFSET][db2 + MEDIUM_SCAN_OFFSET] = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);
                }
            }
        }

        /* Now see what combinations are both low in error and possible to */
        /* encode differentially. */

        minerr = 255 * 255 * 3 * 8 * 2;

        for (dr1 = MEDIUM_SCAN_MIN; dr1 < MEDIUM_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = MEDIUM_SCAN_MIN; dg1 < MEDIUM_SCAN_MAX + 1; dg1++)
            {
                for (db1 = MEDIUM_SCAN_MIN; db1 < MEDIUM_SCAN_MAX + 1; db1++)
                {
                    for (dr2 = MEDIUM_SCAN_MIN; dr2 < MEDIUM_SCAN_MAX + 1; dr2++)
                    {
                        for (dg2 = MEDIUM_SCAN_MIN; dg2 < MEDIUM_SCAN_MAX + 1; dg2++)
                        {
                            for (db2 = MEDIUM_SCAN_MIN; db2 < MEDIUM_SCAN_MAX + 1; db2++)
                            {                               
                                enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                                enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                                enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);
                                enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                                enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                                enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                                /* We must make sure that the difference between the tries still is less than allowed */

                                diff[0] = enc_try2[0] - enc_try1[0];   
                                diff[1] = enc_try2[1] - enc_try1[1];   
                                diff[2] = enc_try2[2] - enc_try1[2];

                                if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
                                {
                                    /* The diff is OK, calculate total error: */
                                   
                                    kzInt err = err1[dr1 + MEDIUM_SCAN_OFFSET][dg1 + MEDIUM_SCAN_OFFSET][db1 + MEDIUM_SCAN_OFFSET] + err2[dr2 + MEDIUM_SCAN_OFFSET][dg2 + MEDIUM_SCAN_OFFSET][db2 + MEDIUM_SCAN_OFFSET];

                                    if (err < minerr)
                                    {
                                        minerr = err;

                                        enc_color1[0] = enc_try1[0];
                                        enc_color1[1] = enc_try1[1];
                                        enc_color1[2] = enc_try1[2];
                                        enc_color2[0] = enc_try2[0];
                                        enc_color2[1] = enc_try2[1];
                                        enc_color2[2] = enc_try2[2];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        best_err_norm_diff = minerr;
        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

       
        /* Pack bits into the first word. */


   
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm_diff = 0;
        PUTBITSHIGH(compressed1_norm_diff, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_norm_diff, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_norm_diff, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_norm_diff, diff[2], 3, 42);

       
        /* left part of block */
        ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm_diff, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm_diff, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm_diff, 0, 1, 32);

        compressed2_norm_diff = 0;
        PUTBITS(compressed2_norm_diff, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices2_LSB), 8, 15);


    }
    else
    {
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */
        /* Color for left block */

        kzInt besterr = 255 * 255 * 3 * 8;
        kzInt bestri = 0, bestgi = 0, bestbi = 0;
        kzInt ri, gi, bi;

        diffbit = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color1[0] = ri;
                    enc_color1[1] = gi;
                    enc_color1[2] = bi;

                    avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
                    avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
                    avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        norm_err = besterr;
       
        enc_color1[0] = bestri;
        enc_color1[1] = bestgi;
        enc_color1[2] = bestbi;
        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);


        /* Color for right block */

        besterr = 255 * 255 * 3 * 8;
        bestri = 0; bestgi = 0; bestbi = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color2[0] = ri;
                    enc_color2[1] = gi;
                    enc_color2[2] = bi;

                    avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
                    avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
                    avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }


        norm_err += besterr;
        best_err_norm_444 = norm_err;

        enc_color2[0] = bestri;
        enc_color2[1] = bestgi;
        enc_color2[2] = bestbi;
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);


        /* Pack bits into the first word. */
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm_444 = 0;
        PUTBITSHIGH(compressed1_norm_444, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[2], 4, 43);


        /* left part of block */
        ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm_444, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm_444, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm_444, 0, 1, 32);

        compressed2_norm_444 = 0;
        PUTBITS(compressed2_norm_444, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm_444, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm_444, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm_444, (best_pixel_indices2_LSB), 8, 15);

    }

    /* Now try flipped blocks 4x2: */

    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty + 2, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));
   
    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= MEDIUM_TRY_MIN) && (diff[0] <= MEDIUM_TRY_MAX) && (diff[1] >= MEDIUM_TRY_MIN) && (diff[1] <= MEDIUM_TRY_MAX) && (diff[2] >= MEDIUM_TRY_MIN) && (diff[2] <= MEDIUM_TRY_MAX))
    {
        kzInt err1[MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE];
        kzInt err2[MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE];

        diffbit = 1;

        enc_base1[0] = enc_color1[0];
        enc_base1[1] = enc_color1[1];
        enc_base1[2] = enc_color1[2];
        enc_base2[0] = enc_color2[0];
        enc_base2[1] = enc_color2[1];
        enc_base2[2] = enc_color2[2];

        /* upper part of block */
        for (dr1 = MEDIUM_SCAN_MIN; dr1 < MEDIUM_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = MEDIUM_SCAN_MIN; dg1 < MEDIUM_SCAN_MAX + 1; dg1++)
            {
                for (db1 = MEDIUM_SCAN_MIN; db1 < MEDIUM_SCAN_MAX + 1; db1++)
                {
                    enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                    enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                    enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);

                    avg_color_quant1[0] = (kzU8)(enc_try1[0] << 3 | (enc_try1[0] >> 2));
                    avg_color_quant1[1] = (kzU8)(enc_try1[1] << 3 | (enc_try1[1] >> 2));
                    avg_color_quant1[2] = (kzU8)(enc_try1[2] << 3 | (enc_try1[2] >> 2));

                    /* upper part of block */
                    err1[dr1 + MEDIUM_SCAN_OFFSET][dg1 + MEDIUM_SCAN_OFFSET][db1 + MEDIUM_SCAN_OFFSET] = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
                }
            }
        }

        /* lower part of block */
        for (dr2 = MEDIUM_SCAN_MIN; dr2 < MEDIUM_SCAN_MAX + 1; dr2++)
        {
            for (dg2 = MEDIUM_SCAN_MIN; dg2 < MEDIUM_SCAN_MAX + 1; dg2++)
            {
                for (db2 = MEDIUM_SCAN_MIN; db2 < MEDIUM_SCAN_MAX + 1; db2++)
                {
                    enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                    enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                    enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                    avg_color_quant2[0] = (kzU8)(enc_try2[0] << 3 | (enc_try2[0] >> 2));
                    avg_color_quant2[1] = (kzU8)(enc_try2[1] << 3 | (enc_try2[1] >> 2));
                    avg_color_quant2[2] = (kzU8)(enc_try2[2] << 3 | (enc_try2[2] >> 2));

                    /* lower part of block */
                    err2[dr2 + MEDIUM_SCAN_OFFSET][dg2 + MEDIUM_SCAN_OFFSET][db2 + MEDIUM_SCAN_OFFSET] = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);
                }
            }
        }

        /* Now see what combinations are both low in error and possible to */
        /* encode differentially. */

        minerr = 255 * 255 * 3 * 8 * 2;

        for (dr1 = MEDIUM_SCAN_MIN; dr1 < MEDIUM_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = MEDIUM_SCAN_MIN; dg1 < MEDIUM_SCAN_MAX + 1; dg1++)
            {
                for (db1 = MEDIUM_SCAN_MIN; db1 < MEDIUM_SCAN_MAX + 1; db1++)
                {
                    for (dr2 = MEDIUM_SCAN_MIN; dr2 < MEDIUM_SCAN_MAX + 1; dr2++)
                    {
                        for (dg2 = MEDIUM_SCAN_MIN; dg2 < MEDIUM_SCAN_MAX + 1; dg2++)
                        {
                            for (db2 = MEDIUM_SCAN_MIN; db2 < MEDIUM_SCAN_MAX + 1; db2++)
                            {                               
                                enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                                enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                                enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);
                                enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                                enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                                enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                                /* We must make sure that the difference between the tries still is less than allowed */

                                diff[0] = enc_try2[0] - enc_try1[0];   
                                diff[1] = enc_try2[1] - enc_try1[1];   
                                diff[2] = enc_try2[2] - enc_try1[2];

                                if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
                                {
                                    /* The diff is OK, calculate total error: */
                                   
                                    kzInt err = err1[dr1 + MEDIUM_SCAN_OFFSET][dg1 + MEDIUM_SCAN_OFFSET][db1 + MEDIUM_SCAN_OFFSET] + err2[dr2 + MEDIUM_SCAN_OFFSET][dg2 + MEDIUM_SCAN_OFFSET][db2 + MEDIUM_SCAN_OFFSET];

                                    if (err < minerr)
                                    {
                                        minerr = err;

                                        enc_color1[0] = enc_try1[0];
                                        enc_color1[1] = enc_try1[1];
                                        enc_color1[2] = enc_try1[2];
                                        enc_color2[0] = enc_try2[0];
                                        enc_color2[1] = enc_try2[1];
                                        enc_color2[2] = enc_try2[2];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }


        flip_err = minerr;

        best_err_flip_diff = flip_err;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

       
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        /* Pack bits into the first word. */

        compressed1_flip_diff = 0;
        PUTBITSHIGH(compressed1_flip_diff, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_flip_diff, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_flip_diff, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_flip_diff, diff[2], 3, 42);




        /* upper part of block */
        ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
        /* lower part of block */
        ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);


        PUTBITSHIGH(compressed1_flip_diff, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip_diff, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip_diff, 1, 1, 32);


        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip_diff = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);
   
    }
    else
    {
        kzInt besterr = 255 * 255 * 3 * 8;
        kzInt bestri = 0, bestgi = 0, bestbi = 0;
        kzInt ri, gi, bi;

        diffbit = 0;
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */


        /* Color for upper block */

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color1[0] = ri;
                    enc_color1[1] = gi;
                    enc_color1[2] = bi;

                    avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
                    avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
                    avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);

                    /* upper part of block */
                    err = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        flip_err = besterr;

        enc_color1[0] = bestri;
        enc_color1[1] = bestgi;
        enc_color1[2] = bestbi;
        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);


        /* Color for lower block */

        besterr = 255 * 255 * 3 * 8;
        bestri = 0; bestgi = 0; bestbi = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color2[0] = ri;
                    enc_color2[1] = gi;
                    enc_color2[2] = bi;

                    avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
                    avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
                    avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        flip_err += besterr;
        best_err_flip_444 = flip_err;

        enc_color2[0] = bestri;
        enc_color2[1] = bestgi;
        enc_color2[2] = bestbi;
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);


        /* Pack bits into the first word. */
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_flip_444 = 0;
        PUTBITSHIGH(compressed1_flip_444, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[2], 4, 43);

        /* upper part of block */
        ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* lower part of block */
        ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip_444, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip_444, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip_444, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip_444 = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);


    }

    /* Now lets see which is the best table to use. Only 8 tables are possible. */
    {
        kzInt compressed1_norm;
        kzInt compressed2_norm;
        kzInt compressed1_flip;
        kzInt compressed2_flip;

        /* See which of the norm blocks is better */

        if (best_err_norm_diff <= best_err_norm_444)
        {
            compressed1_norm = compressed1_norm_diff;
            compressed2_norm = compressed2_norm_diff;
            norm_err = best_err_norm_diff;
        }
        else
        {
            compressed1_norm = compressed1_norm_444;
            compressed2_norm = compressed2_norm_444;
            norm_err = best_err_norm_444;
        }

        /* See which of the flip blocks is better */

        if (best_err_flip_diff <= best_err_flip_444)
        {
            compressed1_flip = compressed1_flip_diff;
            compressed2_flip = compressed2_flip_diff;
            flip_err = best_err_flip_diff;
        }
        else
        {
            compressed1_flip = compressed1_flip_444;
            compressed2_flip = compressed2_flip_444;
            flip_err = best_err_flip_444;
        }

        /* See if flip or norm is better */


        if (norm_err <= flip_err)
        {

            *compressed1 = compressed1_norm | 0;
            *compressed2 = compressed2_norm;
        }
        else
        {

            *compressed1 = compressed1_flip | 1;
            *compressed2 = compressed2_flip;
        }
    }
}

static void ktcEtcCompressBlockDiffFlipSlowPerceptual_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                               kzUint* compressed1, kzUint* compressed2)
{
    kzUint compressed1_norm_diff = 0, compressed2_norm_diff = 0;
    kzUint compressed1_norm_444, compressed2_norm_444;
    kzUint compressed1_flip_diff = 0, compressed2_flip_diff = 0;
    kzUint compressed1_flip_444, compressed2_flip_444;
    kzUint best_err_norm_diff = 255 * 255 * 8 * 3;
    kzUint best_err_norm_444;
    kzUint best_err_flip_diff = 255 * 255 * 8 * 3;
    kzUint best_err_flip_444;
    kzU8 avg_color_quant1[3], avg_color_quant2[3];

    kzFloat avg_color_kzFloat1[3], avg_color_kzFloat2[3];
    kzInt enc_color1[3], enc_color2[3], diff[3];
    kzInt enc_base1[3], enc_base2[3];
    kzInt enc_try1[3], enc_try2[3];
/*   kzInt min_error = 255 * 255 * 8 * 3, err; */
    kzUint best_pixel_indices1_MSB = 0;
    kzUint best_pixel_indices1_LSB = 0;
    kzUint best_pixel_indices2_MSB = 0;
    kzUint best_pixel_indices2_LSB = 0;

    kzUint best_table1 = 0, best_table2 = 0;
    kzInt diffbit;

    kzInt norm_err;
    kzInt flip_err = 0;
    kzInt minerr;
    kzInt dr1, dg1, db1, dr2, dg2, db2;

    /* First try normal blocks 2x4: */

    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx + 2, starty, avg_color_kzFloat2);



    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= SLOW_TRY_MIN) && (diff[0] <= SLOW_TRY_MAX) && (diff[1] >= SLOW_TRY_MIN) && (diff[1] <= SLOW_TRY_MAX) && (diff[2] >= SLOW_TRY_MIN) && (diff[2] <= SLOW_TRY_MAX))
    {
        kzInt err1[SLOW_SCAN_RANGE][SLOW_SCAN_RANGE][SLOW_SCAN_RANGE];
        kzInt err2[SLOW_SCAN_RANGE][SLOW_SCAN_RANGE][SLOW_SCAN_RANGE];

        diffbit = 1;

        enc_base1[0] = enc_color1[0];
        enc_base1[1] = enc_color1[1];
        enc_base1[2] = enc_color1[2];
        enc_base2[0] = enc_color2[0];
        enc_base2[1] = enc_color2[1];
        enc_base2[2] = enc_color2[2];

        /* left part of block */
        for (dr1 = SLOW_SCAN_MIN; dr1 < SLOW_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = SLOW_SCAN_MIN; dg1 < SLOW_SCAN_MAX + 1; dg1++)
            {
                for (db1 = SLOW_SCAN_MIN; db1 < SLOW_SCAN_MAX + 1; db1++)
                {
                    enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                    enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                    enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);

                    avg_color_quant1[0] = (kzU8)(enc_try1[0] << 3 | (enc_try1[0] >> 2));
                    avg_color_quant1[1] = (kzU8)(enc_try1[1] << 3 | (enc_try1[1] >> 2));
                    avg_color_quant1[2] = (kzU8)(enc_try1[2] << 3 | (enc_try1[2] >> 2));

                    /* left part of block */
                    err1[dr1 + SLOW_SCAN_OFFSET][dg1 + SLOW_SCAN_OFFSET][db1 + SLOW_SCAN_OFFSET] = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
                }
            }
        }

        /* right part of block */
        for (dr2 = SLOW_SCAN_MIN; dr2 < SLOW_SCAN_MAX + 1; dr2++)
        {
            for (dg2 = SLOW_SCAN_MIN; dg2 < SLOW_SCAN_MAX + 1; dg2++)
            {
                for (db2 = SLOW_SCAN_MIN; db2 < SLOW_SCAN_MAX + 1; db2++)
                {
                    enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                    enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                    enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                    avg_color_quant2[0] = (kzU8)(enc_try2[0] << 3 | (enc_try2[0] >> 2));
                    avg_color_quant2[1] = (kzU8)(enc_try2[1] << 3 | (enc_try2[1] >> 2));
                    avg_color_quant2[2] = (kzU8)(enc_try2[2] << 3 | (enc_try2[2] >> 2));

                    /* left part of block */
                    err2[dr2 + SLOW_SCAN_OFFSET][dg2 + SLOW_SCAN_OFFSET][db2 + SLOW_SCAN_OFFSET] = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);
                }
            }
        }

        /* Now see what combinations are both low in error and possible to */
        /* encode differentially. */

        minerr = 255 * 255 * 3 * 8 * 2;

        for (dr1 = SLOW_SCAN_MIN; dr1 < SLOW_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = SLOW_SCAN_MIN; dg1 < SLOW_SCAN_MAX + 1; dg1++)
            {
                for (db1 = SLOW_SCAN_MIN; db1 < SLOW_SCAN_MAX + 1; db1++)
                {
                    for (dr2 = SLOW_SCAN_MIN; dr2 < SLOW_SCAN_MAX + 1; dr2++)
                    {
                        for (dg2 = SLOW_SCAN_MIN; dg2 < SLOW_SCAN_MAX + 1; dg2++)
                        {
                            for (db2 = SLOW_SCAN_MIN; db2 < SLOW_SCAN_MAX + 1; db2++)
                            {                               
                                enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                                enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                                enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);
                                enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                                enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                                enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                                /* We must make sure that the difference between the tries still is less than allowed */

                                diff[0] = enc_try2[0] - enc_try1[0];   
                                diff[1] = enc_try2[1] - enc_try1[1];   
                                diff[2] = enc_try2[2] - enc_try1[2];

                                if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
                                {
                                    /* The diff is OK, calculate total error: */
                                   
                                    kzInt err = err1[dr1 + SLOW_SCAN_OFFSET][dg1 + SLOW_SCAN_OFFSET][db1 + SLOW_SCAN_OFFSET] + err2[dr2 + SLOW_SCAN_OFFSET][dg2 + SLOW_SCAN_OFFSET][db2 + SLOW_SCAN_OFFSET];

                                    if (err < minerr)
                                    {
                                        minerr = err;

                                        enc_color1[0] = enc_try1[0];
                                        enc_color1[1] = enc_try1[1];
                                        enc_color1[2] = enc_try1[2];
                                        enc_color2[0] = enc_try2[0];
                                        enc_color2[1] = enc_try2[1];
                                        enc_color2[2] = enc_try2[2];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        best_err_norm_diff = minerr;
        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

       
        /* Pack bits into the first word. */


   
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm_diff = 0;
        PUTBITSHIGH(compressed1_norm_diff, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_norm_diff, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_norm_diff, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_norm_diff, diff[2], 3, 42);

       
        /* left part of block */
        ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm_diff, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm_diff, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm_diff, 0, 1, 32);

        compressed2_norm_diff = 0;
        PUTBITS(compressed2_norm_diff, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices2_LSB), 8, 15);


    }
    /* We should do this in any case... */
    {
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */


        /* Color for left block */

        kzInt besterr = 255 * 255 * 3 * 8;
        kzInt bestri = 0, bestgi = 0, bestbi = 0;
        kzInt ri, gi, bi;

        diffbit = 0;
       
        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color1[0] = ri;
                    enc_color1[1] = gi;
                    enc_color1[2] = bi;

                    avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
                    avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
                    avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        norm_err = besterr;
       
        enc_color1[0] = bestri;
        enc_color1[1] = bestgi;
        enc_color1[2] = bestbi;
        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);


        /* Color for right block */

        besterr = 255 * 255 * 3 * 8;
        bestri = 0; bestgi = 0; bestbi = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color2[0] = ri;
                    enc_color2[1] = gi;
                    enc_color2[2] = bi;

                    avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
                    avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
                    avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }


        norm_err += besterr;
        best_err_norm_444 = norm_err;

        enc_color2[0] = bestri;
        enc_color2[1] = bestgi;
        enc_color2[2] = bestbi;
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);


        /* Pack bits into the first word. */
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm_444 = 0;
        PUTBITSHIGH(compressed1_norm_444, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[2], 4, 43);


        /* left part of block */
        ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm_444, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm_444, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm_444, 0, 1, 32);

        compressed2_norm_444 = 0;
        PUTBITS(compressed2_norm_444, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm_444, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm_444, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm_444, (best_pixel_indices2_LSB), 8, 15);

    }

    /* Now try flipped blocks 4x2: */

    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty + 2, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= SLOW_TRY_MIN) && (diff[0] <= SLOW_TRY_MAX) && (diff[1] >= SLOW_TRY_MIN) && (diff[1] <= SLOW_TRY_MAX) && (diff[2] >= SLOW_TRY_MIN) && (diff[2] <= SLOW_TRY_MAX))
    {
        kzInt err1[SLOW_SCAN_RANGE][SLOW_SCAN_RANGE][SLOW_SCAN_RANGE];
        kzInt err2[SLOW_SCAN_RANGE][SLOW_SCAN_RANGE][SLOW_SCAN_RANGE];

        diffbit = 1;

        enc_base1[0] = enc_color1[0];
        enc_base1[1] = enc_color1[1];
        enc_base1[2] = enc_color1[2];
        enc_base2[0] = enc_color2[0];
        enc_base2[1] = enc_color2[1];
        enc_base2[2] = enc_color2[2];

        /* upper part of block */
        for (dr1 = SLOW_SCAN_MIN; dr1 < SLOW_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = SLOW_SCAN_MIN; dg1 < SLOW_SCAN_MAX + 1; dg1++)
            {
                for (db1 = SLOW_SCAN_MIN; db1 < SLOW_SCAN_MAX + 1; db1++)
                {
                    enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                    enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                    enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);

                    avg_color_quant1[0] = (kzU8)(enc_try1[0] << 3 | (enc_try1[0] >> 2));
                    avg_color_quant1[1] = (kzU8)(enc_try1[1] << 3 | (enc_try1[1] >> 2));
                    avg_color_quant1[2] = (kzU8)(enc_try1[2] << 3 | (enc_try1[2] >> 2));

                    /* upper part of block */
                    err1[dr1 + SLOW_SCAN_OFFSET][dg1 + SLOW_SCAN_OFFSET][db1 + SLOW_SCAN_OFFSET] = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, & best_pixel_indices1_LSB);
                }
            }
        }

        /* lower part of block */
        for (dr2 = SLOW_SCAN_MIN; dr2 < SLOW_SCAN_MAX + 1; dr2++)
        {
            for (dg2 = SLOW_SCAN_MIN; dg2 < SLOW_SCAN_MAX + 1; dg2++)
            {
                for (db2 = SLOW_SCAN_MIN; db2 < SLOW_SCAN_MAX + 1; db2++)
                {
                    enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                    enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                    enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                    avg_color_quant2[0] = (kzU8)(enc_try2[0] << 3 | (enc_try2[0] >> 2));
                    avg_color_quant2[1] = (kzU8)(enc_try2[1] << 3 | (enc_try2[1] >> 2));
                    avg_color_quant2[2] = (kzU8)(enc_try2[2] << 3 | (enc_try2[2] >> 2));

                    /* lower part of block */
                    err2[dr2 + SLOW_SCAN_OFFSET][dg2 + SLOW_SCAN_OFFSET][db2 + SLOW_SCAN_OFFSET] = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);
                }
            }
        }

        /* Now see what combinations are both low in error and possible to */
        /* encode differentially. */

        minerr = 255 * 255 * 3 * 8 * 2;

        for (dr1 = SLOW_SCAN_MIN; dr1 < SLOW_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = SLOW_SCAN_MIN; dg1 < SLOW_SCAN_MAX + 1; dg1++)
            {
                for (db1 = SLOW_SCAN_MIN; db1 < SLOW_SCAN_MAX + 1; db1++)
                {
                    for (dr2 = SLOW_SCAN_MIN; dr2 < SLOW_SCAN_MAX + 1; dr2++)
                    {
                        for (dg2 = SLOW_SCAN_MIN; dg2 < SLOW_SCAN_MAX + 1; dg2++)
                        {
                            for (db2 = SLOW_SCAN_MIN; db2 < SLOW_SCAN_MAX + 1; db2++)
                            {                               
                                enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                                enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                                enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);
                                enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                                enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                                enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                                /* We must make sure that the difference between the tries still is less than allowed */

                                diff[0] = enc_try2[0] - enc_try1[0];   
                                diff[1] = enc_try2[1] - enc_try1[1];   
                                diff[2] = enc_try2[2] - enc_try1[2];

                                if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
                                {
                                    /* The diff is OK, calculate total error: */
                                   
                                    kzInt err = err1[dr1 + SLOW_SCAN_OFFSET][dg1 + SLOW_SCAN_OFFSET][db1 + SLOW_SCAN_OFFSET] + err2[dr2 + SLOW_SCAN_OFFSET][dg2 + SLOW_SCAN_OFFSET][db2 + SLOW_SCAN_OFFSET];

                                    if (err < minerr)
                                    {
                                        minerr = err;

                                        enc_color1[0] = enc_try1[0];
                                        enc_color1[1] = enc_try1[1];
                                        enc_color1[2] = enc_try1[2];
                                        enc_color2[0] = enc_try2[0];
                                        enc_color2[1] = enc_try2[1];
                                        enc_color2[2] = enc_try2[2];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }


        flip_err = minerr;

        best_err_flip_diff = flip_err;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

       
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        /* Pack bits into the first word. */

        compressed1_flip_diff = 0;
        PUTBITSHIGH(compressed1_flip_diff, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_flip_diff, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_flip_diff, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_flip_diff, diff[2], 3, 42);




        /* upper part of block */
        ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, & best_pixel_indices1_LSB);
        /* lower part of block */
        ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);


        PUTBITSHIGH(compressed1_flip_diff, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip_diff, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip_diff, 1, 1, 32);


        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip_diff = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);
   
    }
    {
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */


        /* Color for upper block */

        kzInt besterr = 255 * 255 * 3 * 8;
        kzInt bestri = 0, bestgi = 0, bestbi = 0;
        kzInt ri, gi, bi;

        diffbit = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color1[0] = ri;
                    enc_color1[1] = gi;
                    enc_color1[2] = bi;

                    avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
                    avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
                    avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);

                    /* upper part of block */
                    err = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, & best_pixel_indices1_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        flip_err = besterr;

        enc_color1[0] = bestri;
        enc_color1[1] = bestgi;
        enc_color1[2] = bestbi;
        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);


        /* Color for lower block */

        besterr = 255 * 255 * 3 * 8;
        bestri = 0; bestgi = 0; bestbi = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color2[0] = ri;
                    enc_color2[1] = gi;
                    enc_color2[2] = bi;

                    avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
                    avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
                    avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        flip_err += besterr;
        best_err_flip_444 = flip_err;

        enc_color2[0] = bestri;
        enc_color2[1] = bestgi;
        enc_color2[2] = bestbi;
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);


        /* Pack bits into the first word. */
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_flip_444 = 0;
        PUTBITSHIGH(compressed1_flip_444, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[2], 4, 43);

        /* upper part of block */
        ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* lower part of block */
        ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip_444, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip_444, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip_444, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip_444 = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);


    }

    /* Now lets see which is the best table to use. Only 8 tables are possible. */
    {
        kzInt compressed1_norm;
        kzInt compressed2_norm;
        kzInt compressed1_flip;
        kzInt compressed2_flip;

        /* See which of the norm blocks is better */

        if (best_err_norm_diff <= best_err_norm_444)
        {
            compressed1_norm = compressed1_norm_diff;
            compressed2_norm = compressed2_norm_diff;
            norm_err = best_err_norm_diff;
        }
        else
        {
            compressed1_norm = compressed1_norm_444;
            compressed2_norm = compressed2_norm_444;
            norm_err = best_err_norm_444;
        }

        /* See which of the flip blocks is better */

        if (best_err_flip_diff <= best_err_flip_444)
        {
            compressed1_flip = compressed1_flip_diff;
            compressed2_flip = compressed2_flip_diff;
            flip_err = best_err_flip_diff;
        }
        else
        {
            compressed1_flip = compressed1_flip_444;
            compressed2_flip = compressed2_flip_444;
            flip_err = best_err_flip_444;
        }

        /* See if flip or norm is better */

        if (norm_err <= flip_err)
        {

            *compressed1 = compressed1_norm | 0;
            *compressed2 = compressed2_norm;
        }
        else
        {

            *compressed1 = compressed1_flip | 1;
            *compressed2 = compressed2_flip;
        }
    }
}

static void ktcEtcCompressBlockDiffFlipMediumPerceptual_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                                 kzUint* compressed1, kzUint* compressed2)
{
    kzUint compressed1_norm_diff = 0, compressed2_norm_diff = 0;
    kzUint compressed1_norm_444 = 0, compressed2_norm_444 = 0;
    kzUint compressed1_flip_diff = 0, compressed2_flip_diff = 0;
    kzUint compressed1_flip_444 = 0, compressed2_flip_444 = 0;
    kzUint best_err_norm_diff = 255 * 255 * 16 * 3;
    kzUint best_err_norm_444 = 255 * 255 * 16 * 3;
    kzUint best_err_flip_diff = 255 * 255 * 16 * 3;
    kzUint best_err_flip_444 = 255 * 255 * 16 * 3;
    kzU8 avg_color_quant1[3], avg_color_quant2[3];

    kzFloat avg_color_kzFloat1[3], avg_color_kzFloat2[3];
    kzInt enc_color1[3], enc_color2[3], diff[3];
    kzInt enc_base1[3], enc_base2[3];
    kzInt enc_try1[3], enc_try2[3];
    kzUint best_pixel_indices1_MSB = 0;
    kzUint best_pixel_indices1_LSB = 0;
    kzUint best_pixel_indices2_MSB = 0;
    kzUint best_pixel_indices2_LSB = 0;

    kzUint best_table1 = 0, best_table2 = 0;
    kzInt diffbit;

    kzInt norm_err = 0;
    kzInt flip_err = 0;
    kzInt minerr;
    kzInt dr1, dg1, db1, dr2, dg2, db2;

    /* First try normal blocks 2x4: */

    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx + 2, starty, avg_color_kzFloat2);



    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= MEDIUM_TRY_MIN) && (diff[0] <= MEDIUM_TRY_MAX) && (diff[1] >= MEDIUM_TRY_MIN) && (diff[1] <= MEDIUM_TRY_MAX) && (diff[2] >= MEDIUM_TRY_MIN) && (diff[2] <= MEDIUM_TRY_MAX))
    {
        kzInt err1[MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE];
        kzInt err2[MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE];

        diffbit = 1;

        enc_base1[0] = enc_color1[0];
        enc_base1[1] = enc_color1[1];
        enc_base1[2] = enc_color1[2];
        enc_base2[0] = enc_color2[0];
        enc_base2[1] = enc_color2[1];
        enc_base2[2] = enc_color2[2];

        /* left part of block */
        for (dr1 = MEDIUM_SCAN_MIN; dr1 < MEDIUM_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = MEDIUM_SCAN_MIN; dg1 < MEDIUM_SCAN_MAX + 1; dg1++)
            {
                for (db1 = MEDIUM_SCAN_MIN; db1 < MEDIUM_SCAN_MAX + 1; db1++)
                {
                    enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                    enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                    enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);

                    avg_color_quant1[0] = (kzU8)(enc_try1[0] << 3 | (enc_try1[0] >> 2));
                    avg_color_quant1[1] = (kzU8)(enc_try1[1] << 3 | (enc_try1[1] >> 2));
                    avg_color_quant1[2] = (kzU8)(enc_try1[2] << 3 | (enc_try1[2] >> 2));

                    /* left part of block */
                    err1[dr1 + MEDIUM_SCAN_OFFSET][dg1 + MEDIUM_SCAN_OFFSET][db1 + MEDIUM_SCAN_OFFSET] = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
                }
            }
        }

        /* right part of block */
        for (dr2 = MEDIUM_SCAN_MIN; dr2 < MEDIUM_SCAN_MAX + 1; dr2++)
        {
            for (dg2 = MEDIUM_SCAN_MIN; dg2 < MEDIUM_SCAN_MAX + 1; dg2++)
            {
                for (db2 = MEDIUM_SCAN_MIN; db2 < MEDIUM_SCAN_MAX + 1; db2++)
                {
                    enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                    enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                    enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                    avg_color_quant2[0] = (kzU8)(enc_try2[0] << 3 | (enc_try2[0] >> 2));
                    avg_color_quant2[1] = (kzU8)(enc_try2[1] << 3 | (enc_try2[1] >> 2));
                    avg_color_quant2[2] = (kzU8)(enc_try2[2] << 3 | (enc_try2[2] >> 2));

                    /* left part of block */
                    err2[dr2 + MEDIUM_SCAN_OFFSET][dg2 + MEDIUM_SCAN_OFFSET][db2 + MEDIUM_SCAN_OFFSET] = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);
                }
            }
        }

        /* Now see what combinations are both low in error and possible to */
        /* encode differentially. */

        minerr = 255 * 255 * 3 * 8 * 2;

        for (dr1 = MEDIUM_SCAN_MIN; dr1 < MEDIUM_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = MEDIUM_SCAN_MIN; dg1 < MEDIUM_SCAN_MAX + 1; dg1++)
            {
                for (db1 = MEDIUM_SCAN_MIN; db1 < MEDIUM_SCAN_MAX + 1; db1++)
                {
                    for (dr2 = MEDIUM_SCAN_MIN; dr2 < MEDIUM_SCAN_MAX + 1; dr2++)
                    {
                        for (dg2 = MEDIUM_SCAN_MIN; dg2 < MEDIUM_SCAN_MAX + 1; dg2++)
                        {
                            for (db2 = MEDIUM_SCAN_MIN; db2 < MEDIUM_SCAN_MAX + 1; db2++)
                            {                               
                                enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                                enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                                enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);
                                enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                                enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                                enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                                /* We must make sure that the difference between the tries still is less than allowed */

                                diff[0] = enc_try2[0] - enc_try1[0];   
                                diff[1] = enc_try2[1] - enc_try1[1];   
                                diff[2] = enc_try2[2] - enc_try1[2];

                                if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
                                {
                                    /* The diff is OK, calculate total error: */
                                   
                                    kzInt err = err1[dr1 + MEDIUM_SCAN_OFFSET][dg1 + MEDIUM_SCAN_OFFSET][db1 + MEDIUM_SCAN_OFFSET] + err2[dr2 + MEDIUM_SCAN_OFFSET][dg2 + MEDIUM_SCAN_OFFSET][db2 + MEDIUM_SCAN_OFFSET];

                                    if (err < minerr)
                                    {
                                        minerr = err;

                                        enc_color1[0] = enc_try1[0];
                                        enc_color1[1] = enc_try1[1];
                                        enc_color1[2] = enc_try1[2];
                                        enc_color2[0] = enc_try2[0];
                                        enc_color2[1] = enc_try2[1];
                                        enc_color2[2] = enc_try2[2];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        best_err_norm_diff = minerr;
        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

       
        /* Pack bits into the first word. */


   
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm_diff = 0;
        PUTBITSHIGH(compressed1_norm_diff, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_norm_diff, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_norm_diff, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_norm_diff, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_norm_diff, diff[2], 3, 42);

       
        /* left part of block */
        ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm_diff, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm_diff, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm_diff, 0, 1, 32);

        compressed2_norm_diff = 0;
        PUTBITS(compressed2_norm_diff, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm_diff, (best_pixel_indices2_LSB), 8, 15);


    }
    else
    {
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */


        /* Color for left block */

        kzInt besterr = 255 * 255 * 3 * 8;
        kzInt bestri = 0, bestgi = 0, bestbi = 0;
        kzInt ri, gi, bi;

        diffbit = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color1[0] = ri;
                    enc_color1[1] = gi;
                    enc_color1[2] = bi;

                    avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
                    avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
                    avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        norm_err = besterr;
       
        enc_color1[0] = bestri;
        enc_color1[1] = bestgi;
        enc_color1[2] = bestbi;
        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);


        /* Color for right block */

        besterr = 255 * 255 * 3 * 8;
        bestri = 0; bestgi = 0; bestbi = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color2[0] = ri;
                    enc_color2[1] = gi;
                    enc_color2[2] = bi;

                    avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
                    avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
                    avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }


        norm_err += besterr;
        best_err_norm_444 = norm_err;

        enc_color2[0] = bestri;
        enc_color2[1] = bestgi;
        enc_color2[2] = bestbi;
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);


        /* Pack bits into the first word. */
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm_444 = 0;
        PUTBITSHIGH(compressed1_norm_444, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_norm_444, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_norm_444, enc_color2[2], 4, 43);


        /* left part of block */
        ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm_444, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm_444, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm_444, 0, 1, 32);

        compressed2_norm_444 = 0;
        PUTBITS(compressed2_norm_444, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm_444, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm_444, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm_444, (best_pixel_indices2_LSB), 8, 15);

    }

    /* Now try flipped blocks 4x2: */

    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty + 2, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= MEDIUM_TRY_MIN) && (diff[0] <= MEDIUM_TRY_MAX) && (diff[1] >= MEDIUM_TRY_MIN) && (diff[1] <= MEDIUM_TRY_MAX) && (diff[2] >= MEDIUM_TRY_MIN) && (diff[2] <= MEDIUM_TRY_MAX))
    {
        kzInt err1[MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE];
        kzInt err2[MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE][MEDIUM_SCAN_RANGE];

        diffbit = 1;

        enc_base1[0] = enc_color1[0];
        enc_base1[1] = enc_color1[1];
        enc_base1[2] = enc_color1[2];
        enc_base2[0] = enc_color2[0];
        enc_base2[1] = enc_color2[1];
        enc_base2[2] = enc_color2[2];

        /* upper part of block */
        for (dr1 = MEDIUM_SCAN_MIN; dr1 < MEDIUM_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = MEDIUM_SCAN_MIN; dg1 < MEDIUM_SCAN_MAX + 1; dg1++)
            {
                for (db1 = MEDIUM_SCAN_MIN; db1 < MEDIUM_SCAN_MAX + 1; db1++)
                {
                    enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                    enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                    enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);

                    avg_color_quant1[0] = (kzU8)(enc_try1[0] << 3 | (enc_try1[0] >> 2));
                    avg_color_quant1[1] = (kzU8)(enc_try1[1] << 3 | (enc_try1[1] >> 2));
                    avg_color_quant1[2] = (kzU8)(enc_try1[2] << 3 | (enc_try1[2] >> 2));

                    /* upper part of block */
                    err1[dr1 + MEDIUM_SCAN_OFFSET][dg1 + MEDIUM_SCAN_OFFSET][db1 + MEDIUM_SCAN_OFFSET] = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, & best_pixel_indices1_LSB);
                }
            }
        }

        /* lower part of block */
        for (dr2 = MEDIUM_SCAN_MIN; dr2 < MEDIUM_SCAN_MAX + 1; dr2++)
        {
            for (dg2 = MEDIUM_SCAN_MIN; dg2 < MEDIUM_SCAN_MAX + 1; dg2++)
            {
                for (db2 = MEDIUM_SCAN_MIN; db2 < MEDIUM_SCAN_MAX + 1; db2++)
                {
                    enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                    enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                    enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                    avg_color_quant2[0] = (kzU8)(enc_try2[0] << 3 | (enc_try2[0] >> 2));
                    avg_color_quant2[1] = (kzU8)(enc_try2[1] << 3 | (enc_try2[1] >> 2));
                    avg_color_quant2[2] = (kzU8)(enc_try2[2] << 3 | (enc_try2[2] >> 2));

                    /* lower part of block */
                    err2[dr2 + MEDIUM_SCAN_OFFSET][dg2 + MEDIUM_SCAN_OFFSET][db2 + MEDIUM_SCAN_OFFSET] = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);
                }
            }
        }

        /* Now see what combinations are both low in error and possible to */
        /* encode differentially. */

        minerr = 255 * 255 * 3 * 8 * 2;

        for (dr1 = MEDIUM_SCAN_MIN; dr1 < MEDIUM_SCAN_MAX + 1; dr1++)
        {
            for (dg1 = MEDIUM_SCAN_MIN; dg1 < MEDIUM_SCAN_MAX + 1; dg1++)
            {
                for (db1 = MEDIUM_SCAN_MIN; db1 < MEDIUM_SCAN_MAX + 1; db1++)
                {
                    for (dr2 = MEDIUM_SCAN_MIN; dr2 < MEDIUM_SCAN_MAX + 1; dr2++)
                    {
                        for (dg2 = MEDIUM_SCAN_MIN; dg2 < MEDIUM_SCAN_MAX + 1; dg2++)
                        {
                            for (db2 = MEDIUM_SCAN_MIN; db2 < MEDIUM_SCAN_MAX + 1; db2++)
                            {                               
                                enc_try1[0] = CLAMP(0, enc_base1[0] + dr1, 31);
                                enc_try1[1] = CLAMP(0, enc_base1[1] + dg1, 31);
                                enc_try1[2] = CLAMP(0, enc_base1[2] + db1, 31);
                                enc_try2[0] = CLAMP(0, enc_base2[0] + dr2, 31);
                                enc_try2[1] = CLAMP(0, enc_base2[1] + dg2, 31);
                                enc_try2[2] = CLAMP(0, enc_base2[2] + db2, 31);

                                /* We must make sure that the difference between the tries still is less than allowed */

                                diff[0] = enc_try2[0] - enc_try1[0];   
                                diff[1] = enc_try2[1] - enc_try1[1];   
                                diff[2] = enc_try2[2] - enc_try1[2];

                                if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
                                {
                                    /* The diff is OK, calculate total error: */
                                   
                                    kzInt err = err1[dr1 + MEDIUM_SCAN_OFFSET][dg1 + MEDIUM_SCAN_OFFSET][db1 + MEDIUM_SCAN_OFFSET] + err2[dr2 + MEDIUM_SCAN_OFFSET][dg2 + MEDIUM_SCAN_OFFSET][db2 + MEDIUM_SCAN_OFFSET];

                                    if (err < minerr)
                                    {
                                        minerr = err;

                                        enc_color1[0] = enc_try1[0];
                                        enc_color1[1] = enc_try1[1];
                                        enc_color1[2] = enc_try1[2];
                                        enc_color2[0] = enc_try2[0];
                                        enc_color2[1] = enc_try2[1];
                                        enc_color2[2] = enc_try2[2];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }


        flip_err = minerr;

        best_err_flip_diff = flip_err;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

       
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        /* Pack bits into the first word. */

        compressed1_flip_diff = 0;
        PUTBITSHIGH(compressed1_flip_diff, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_flip_diff, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_flip_diff, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_flip_diff, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_flip_diff, diff[2], 3, 42);




        /* upper part of block */
        ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, & best_pixel_indices1_LSB);
        /* lower part of block */
        ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);


        PUTBITSHIGH(compressed1_flip_diff, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip_diff, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip_diff, 1, 1, 32);


        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip_diff = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);
   
    }
    else
    {
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */


        /* Color for upper block */

        kzInt besterr = 255 * 255 * 3 * 8;
        kzInt bestri = 0, bestgi = 0, bestbi = 0;
        kzInt ri, gi, bi;

        diffbit = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color1[0] = ri;
                    enc_color1[1] = gi;
                    enc_color1[2] = bi;

                    avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
                    avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
                    avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);

                    /* upper part of block */
                    err = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, & best_pixel_indices1_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        flip_err = besterr;

        enc_color1[0] = bestri;
        enc_color1[1] = bestgi;
        enc_color1[2] = bestbi;
        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);


        /* Color for lower block */

        besterr = 255 * 255 * 3 * 8;
        bestri = 0; bestgi = 0; bestbi = 0;

        for (ri = 0; ri < 15; ri++)
        {
            for (gi = 0; gi < 15; gi++)
            {
                for (bi = 0; bi < 15; bi++)
                {
                    kzInt err;
                    enc_color2[0] = ri;
                    enc_color2[1] = gi;
                    enc_color2[2] = bi;

                    avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
                    avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
                    avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

                    /* left part of block */
                    err = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);

                    if (err < besterr)
                    {
                        bestri = ri; bestgi = gi; bestbi = bi;
                        besterr = err;
                    }


                }
            }
        }

        flip_err += besterr;
        best_err_flip_444 = flip_err;

        enc_color2[0] = bestri;
        enc_color2[1] = bestgi;
        enc_color2[2] = bestbi;
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);


        /* Pack bits into the first word. */
        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_flip_444 = 0;
        PUTBITSHIGH(compressed1_flip_444, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_flip_444, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_flip_444, enc_color2[2], 4, 43);

        /* upper part of block */
        ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* lower part of block */
        ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip_444, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip_444, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip_444, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip_444 = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);


    }

    /* Now lets see which is the best table to use. Only 8 tables are possible. */
    {
        kzInt compressed1_norm;
        kzInt compressed2_norm;
        kzInt compressed1_flip;
        kzInt compressed2_flip;

        /* See which of the norm blocks is better */

        if (best_err_norm_diff <= best_err_norm_444)
        {
            compressed1_norm = compressed1_norm_diff;
            compressed2_norm = compressed2_norm_diff;
            norm_err = best_err_norm_diff;
        }
        else
        {
            compressed1_norm = compressed1_norm_444;
            compressed2_norm = compressed2_norm_444;
            norm_err = best_err_norm_444;
        }

        /* See which of the flip blocks is better */

        if (best_err_flip_diff <= best_err_flip_444)
        {
            compressed1_flip = compressed1_flip_diff;
            compressed2_flip = compressed2_flip_diff;
            flip_err = best_err_flip_diff;
        }
        else
        {
            compressed1_flip = compressed1_flip_444;
            compressed2_flip = compressed2_flip_444;
            flip_err = best_err_flip_444;
        }

        /* See if flip or norm is better */


        if (norm_err <= flip_err)
        {

            *compressed1 = compressed1_norm | 0;
            *compressed2 = compressed2_norm;
        }
        else
        {

            *compressed1 = compressed1_flip | 1;
            *compressed2 = compressed2_flip;
        }
    }
}

static void ktcEtcCompressBlockDiffFlipAverage_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                        kzUint* compressed1, kzUint* compressed2)
{
    kzUint compressed1_norm, compressed2_norm;
    kzUint compressed1_flip, compressed2_flip;
    kzU8 avg_color_quant1[3], avg_color_quant2[3];

    kzFloat avg_color_kzFloat1[3], avg_color_kzFloat2[3];
    kzInt enc_color1[3], enc_color2[3], diff[3];
    kzUint best_table1 = 0, best_table2 = 0;
    kzInt diffbit;

    kzInt norm_err = 0;
    kzInt flip_err = 0;
    kzFloat eps;

    /* First try normal blocks 2x4: */

    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx + 2, starty, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */



    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 1;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

        /* Pack bits into the first word. */

        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm = 0;
        PUTBITSHIGH(compressed1_norm, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_norm, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_norm, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_norm, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_norm, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_norm, diff[2], 3, 42);


        /* left part of block */
        norm_err = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        norm_err += ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm, 0, 1, 32);

        compressed2_norm = 0;
        PUTBITS(compressed2_norm, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm, (best_pixel_indices2_LSB), 8, 15);

    }
    else
    {

        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 0;
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */

        eps = (kzFloat)0.0001;

        enc_color1[0] = (kzInt)(((kzFloat)avg_color_kzFloat1[0] / (17.0))  + 0.5 + eps);
        enc_color1[1] = (kzInt)(((kzFloat)avg_color_kzFloat1[1] / (17.0))  + 0.5 + eps);
        enc_color1[2] = (kzInt)(((kzFloat)avg_color_kzFloat1[2] / (17.0))  + 0.5 + eps);
        enc_color2[0] = (kzInt)(((kzFloat)avg_color_kzFloat2[0] / (17.0))  + 0.5 + eps);
        enc_color2[1] = (kzInt)(((kzFloat)avg_color_kzFloat2[1] / (17.0))  + 0.5 + eps);
        enc_color2[2] = (kzInt)(((kzFloat)avg_color_kzFloat2[2] / (17.0))  + 0.5 + eps);
        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);
   

        /* Pack bits into the first word. */

        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */

        compressed1_norm = 0;
        PUTBITSHIGH(compressed1_norm, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_norm, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_norm, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_norm, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_norm, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_norm, enc_color2[2], 4, 43);


       
        /* left part of block */
        norm_err = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        norm_err += ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm, 0, 1, 32);

        compressed2_norm = 0;
        PUTBITS(compressed2_norm, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm, (best_pixel_indices2_LSB), 8, 15);

   
    }

    /* Now try flipped blocks 4x2: */

    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty + 2, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 1;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

        /* Pack bits into the first word. */

        compressed1_flip = 0;
        PUTBITSHIGH(compressed1_flip, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_flip, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_flip, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_flip, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_flip, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_flip, diff[2], 3, 42);




        /* upper part of block */
        flip_err = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
        /* lower part of block */
        flip_err += ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);

   
    }
    else
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 0;
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */
        eps = (kzFloat)0.0001;

        enc_color1[0] = (kzInt)(((kzFloat)avg_color_kzFloat1[0] / (17.0))  + 0.5 + eps);
        enc_color1[1] = (kzInt)(((kzFloat)avg_color_kzFloat1[1] / (17.0))  + 0.5 + eps);
        enc_color1[2] = (kzInt)(((kzFloat)avg_color_kzFloat1[2] / (17.0))  + 0.5 + eps);
        enc_color2[0] = (kzInt)(((kzFloat)avg_color_kzFloat2[0] / (17.0))  + 0.5 + eps);
        enc_color2[1] = (kzInt)(((kzFloat)avg_color_kzFloat2[1] / (17.0))  + 0.5 + eps);
        enc_color2[2] = (kzInt)(((kzFloat)avg_color_kzFloat2[2] / (17.0))  + 0.5 + eps);

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */


        /* Pack bits into the first word. */

        compressed1_flip = 0;
        PUTBITSHIGH(compressed1_flip, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_flip, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_flip, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_flip, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_flip, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_flip, enc_color2[2], 4, 43);


        /* upper part of block */
        flip_err = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
        /* lower part of block */
        flip_err += ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);


    }

    /* Now lets see which is the best table to use. Only 8 tables are possible. */

    if (norm_err <= flip_err)
    {

        *compressed1 = compressed1_norm | 0;
        *compressed2 = compressed2_norm;

    }
    else
    {

        *compressed1 = compressed1_flip | 1;
        *compressed2 = compressed2_flip;
    }
}

static void ktcEtcCompressBlockDiffFlipCombined_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                         kzUint* compressed1, kzUint* compressed2)
{
    kzUint compressed1_norm, compressed2_norm;
    kzUint compressed1_flip, compressed2_flip;
    kzU8 avg_color_quant1[3], avg_color_quant2[3];

    kzFloat avg_color_kzFloat1[3], avg_color_kzFloat2[3];
    kzInt enc_color1[3], enc_color2[3], diff[3];
    kzUint best_table1 = 0, best_table2 = 0;
    kzInt diffbit;

    kzInt norm_err = 0;
    kzInt flip_err = 0;
    kzU8 dummy[3];
    /* First try normal blocks 2x4: */

    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx + 2, starty, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */




    ktcEtcQuantize555ColorCombined_internal(avg_color_kzFloat1, enc_color1, dummy);
    ktcEtcQuantize555ColorCombined_internal(avg_color_kzFloat2, enc_color2, dummy);

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 1;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

        /* Pack bits into the first word. */

        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm = 0;
        PUTBITSHIGH(compressed1_norm, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_norm, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_norm, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_norm, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_norm, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_norm, diff[2], 3, 42);


        /* left part of block */
        norm_err = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        norm_err += ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm, 0, 1, 32);

        compressed2_norm = 0;
        PUTBITS(compressed2_norm, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm, (best_pixel_indices2_LSB), 8, 15);

    }
    else
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;
        kzU8 dummy2[3];
        diffbit = 0;
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */


        ktcEtcQuantize444ColorCombined_internal(avg_color_kzFloat1, enc_color1, dummy2);
        ktcEtcQuantize444ColorCombined_internal(avg_color_kzFloat2, enc_color2, dummy2);

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);
   

        /* Pack bits into the first word. */

        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */

        compressed1_norm = 0;
        PUTBITSHIGH(compressed1_norm, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_norm, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_norm, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_norm, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_norm, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_norm, enc_color2[2], 4, 43);


       
        /* left part of block */
        norm_err = ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        norm_err += ktcEtcTryalltables3bittable2x4_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm, 0, 1, 32);

        compressed2_norm = 0;
        PUTBITS(compressed2_norm, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm, (best_pixel_indices2_LSB), 8, 15);

   
    }

    /* Now try flipped blocks 4x2: */

    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty + 2, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    ktcEtcQuantize555ColorCombined_internal(avg_color_kzFloat1, enc_color1, dummy);
    ktcEtcQuantize555ColorCombined_internal(avg_color_kzFloat2, enc_color2, dummy);

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 1;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

        /* Pack bits into the first word. */

        compressed1_flip = 0;
        PUTBITSHIGH(compressed1_flip, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_flip, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_flip, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_flip, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_flip, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_flip, diff[2], 3, 42);




        /* upper part of block */
        flip_err = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
        /* lower part of block */
        flip_err += ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);

   
    }
    else
    {
        kzU8 dummy2[3];
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 0;
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */

        ktcEtcQuantize444ColorCombined_internal(avg_color_kzFloat1, enc_color1, dummy2);
        ktcEtcQuantize444ColorCombined_internal(avg_color_kzFloat2, enc_color2, dummy2);

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */


        /* Pack bits into the first word. */

        compressed1_flip = 0;
        PUTBITSHIGH(compressed1_flip, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_flip, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_flip, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_flip, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_flip, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_flip, enc_color2[2], 4, 43);


        /* upper part of block */
        flip_err = ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);
        /* lower part of block */
        flip_err += ktcEtcTryalltables3bittable4x2_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);


    }

    /* Now lets see which is the best table to use. Only 8 tables are possible. */

    if (norm_err <= flip_err)
    {

        *compressed1 = compressed1_norm | 0;
        *compressed2 = compressed2_norm;

    }
    else
    {

        *compressed1 = compressed1_flip | 1;
        *compressed2 = compressed2_flip;
    }
}

static void ktcEtcCompressBlockDiffFlipAveragePerceptual_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                                  kzUint* compressed1, kzUint* compressed2)
{

    kzUint compressed1_norm, compressed2_norm;
    kzUint compressed1_flip, compressed2_flip;
    kzU8 avg_color_quant1[3], avg_color_quant2[3];

    kzFloat avg_color_kzFloat1[3], avg_color_kzFloat2[3];
    kzInt enc_color1[3], enc_color2[3], diff[3];
    kzUint best_table1 = 0, best_table2 = 0;
    kzInt diffbit;

    kzInt norm_err = 0;
    kzInt flip_err = 0;
    kzFloat eps;

    /* First try normal blocks 2x4: */

    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx + 2, starty, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */



    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 1;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

        /* Pack bits into the first word. */

        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm = 0;
        PUTBITSHIGH(compressed1_norm, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_norm, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_norm, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_norm, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_norm, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_norm, diff[2], 3, 42);


        /* left part of block */
        norm_err = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        norm_err += ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm, 0, 1, 32);

        compressed2_norm = 0;
        PUTBITS(compressed2_norm, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm, (best_pixel_indices2_LSB), 8, 15);

    }
    else
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 0;
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */

        eps = (kzFloat)0.0001;

        enc_color1[0] = (kzInt)(((kzFloat)avg_color_kzFloat1[0] / (17.0))  + 0.5 + eps);
        enc_color1[1] = (kzInt)(((kzFloat)avg_color_kzFloat1[1] / (17.0))  + 0.5 + eps);
        enc_color1[2] = (kzInt)(((kzFloat)avg_color_kzFloat1[2] / (17.0))  + 0.5 + eps);
        enc_color2[0] = (kzInt)(((kzFloat)avg_color_kzFloat2[0] / (17.0))  + 0.5 + eps);
        enc_color2[1] = (kzInt)(((kzFloat)avg_color_kzFloat2[1] / (17.0))  + 0.5 + eps);
        enc_color2[2] = (kzInt)(((kzFloat)avg_color_kzFloat2[2] / (17.0))  + 0.5 + eps);
        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);
   

        /* Pack bits into the first word. */

        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */

        compressed1_norm = 0;
        PUTBITSHIGH(compressed1_norm, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_norm, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_norm, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_norm, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_norm, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_norm, enc_color2[2], 4, 43);


       
        /* left part of block */
        norm_err = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        norm_err += ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm, 0, 1, 32);

        compressed2_norm = 0;
        PUTBITS(compressed2_norm, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm, (best_pixel_indices2_LSB), 8, 15);

   
    }

    /* Now try flipped blocks 4x2: */

    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty + 2, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    enc_color1[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[0] / 255.0));
    enc_color1[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[1] / 255.0));
    enc_color1[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat1[2] / 255.0));
    enc_color2[0] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[0] / 255.0));
    enc_color2[1] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[1] / 255.0));
    enc_color2[2] = (kzInt)(JAS_ROUND(31.0 * avg_color_kzFloat2[2] / 255.0));

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 1;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

        /* Pack bits into the first word. */

        compressed1_flip = 0;
        PUTBITSHIGH(compressed1_flip, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_flip, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_flip, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_flip, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_flip, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_flip, diff[2], 3, 42);




        /* upper part of block */
        flip_err = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, & best_pixel_indices1_LSB);
        /* lower part of block */
        flip_err += ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);

   
    }
    else
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 0;
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */
        eps = (kzFloat)0.0001;

        enc_color1[0] = (kzInt)(((kzFloat)avg_color_kzFloat1[0] / (17.0))  + 0.5 + eps);
        enc_color1[1] = (kzInt)(((kzFloat)avg_color_kzFloat1[1] / (17.0))  + 0.5 + eps);
        enc_color1[2] = (kzInt)(((kzFloat)avg_color_kzFloat1[2] / (17.0))  + 0.5 + eps);
        enc_color2[0] = (kzInt)(((kzFloat)avg_color_kzFloat2[0] / (17.0))  + 0.5 + eps);
        enc_color2[1] = (kzInt)(((kzFloat)avg_color_kzFloat2[1] / (17.0))  + 0.5 + eps);
        enc_color2[2] = (kzInt)(((kzFloat)avg_color_kzFloat2[2] / (17.0))  + 0.5 + eps);

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */


        /* Pack bits into the first word. */

        compressed1_flip = 0;
        PUTBITSHIGH(compressed1_flip, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_flip, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_flip, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_flip, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_flip, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_flip, enc_color2[2], 4, 43);


        /* upper part of block */
        flip_err = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, & best_pixel_indices1_LSB);
        /* lower part of block */
        flip_err += ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);


    }

    /* Now lets see which is the best table to use. Only 8 tables are possible. */

    if (norm_err <= flip_err)
    {

        *compressed1 = compressed1_norm | 0;
        *compressed2 = compressed2_norm;

    }
    else
    {

        *compressed1 = compressed1_flip | 1;
        *compressed2 = compressed2_flip;
    }
}

static void ktcEtcCompressBlockDiffFlipCombinedPerceptual_internal(const kzU8* img, kzInt width, kzInt height, kzInt startx, kzInt starty,
                                                                   kzUint* compressed1, kzUint* compressed2)
{

    kzUint compressed1_norm, compressed2_norm;
    kzUint compressed1_flip, compressed2_flip;
    kzU8 avg_color_quant1[3], avg_color_quant2[3];

    kzFloat avg_color_kzFloat1[3], avg_color_kzFloat2[3];
    kzInt enc_color1[3], enc_color2[3], diff[3];
/*   kzInt min_error = 255 * 255 * 8 * 3; */
/*   kzUint best_table_indices1 = 0, best_table_indices2 = 0; */
    kzUint best_table1 = 0, best_table2 = 0;
    kzInt diffbit;

    kzInt norm_err = 0;
    kzInt flip_err = 0;
    kzU8 dummy[3];
    /* First try normal blocks 2x4: */

    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor2x4noQuantFloat_internal(img, width, height, startx + 2, starty, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */

    ktcEtcQuantize555ColorCombinedPerceptual_internal(avg_color_kzFloat1, enc_color1, dummy);
    ktcEtcQuantize555ColorCombinedPerceptual_internal(avg_color_kzFloat2, enc_color2, dummy);

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 1;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

        /* Pack bits into the first word. */

        /*    ETC1_RGB8_OES: */
        /**/
        /*    a) bit layout in bits 63 through 32 if diffbit = 0 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    */
        /*    b) bit layout in bits 63 through 32 if diffbit = 1 */
        /**/
        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
        /*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */
        /**/
        /*    c) bit layout in bits 31 through 0 (in both cases) */
        /**/
        /*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
        /*     -------------------------------------------------------------------------------------------------- */
        /*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
        /*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
        /*     --------------------------------------------------------------------------------------------------     */


        compressed1_norm = 0;
        PUTBITSHIGH(compressed1_norm, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_norm, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_norm, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_norm, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_norm, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_norm, diff[2], 3, 42);


        /* left part of block */
        norm_err = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        norm_err += ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm, 0, 1, 32);

        compressed2_norm = 0;
        PUTBITS(compressed2_norm, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm, (best_pixel_indices2_LSB), 8, 15);

    }
    else
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 0;
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */

        ktcEtcQuantize444ColorCombinedPerceptual_internal(avg_color_kzFloat1, enc_color1, dummy);
        ktcEtcQuantize444ColorCombinedPerceptual_internal(avg_color_kzFloat2, enc_color2, dummy);

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);
   

        /* Pack bits into the first word. */

        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */

        compressed1_norm = 0;
        PUTBITSHIGH(compressed1_norm, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_norm, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_norm, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_norm, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_norm, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_norm, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_norm, enc_color2[2], 4, 43);


       
        /* left part of block */
        norm_err = ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, &best_pixel_indices1_LSB);

        /* right part of block */
        norm_err += ktcEtcTryalltables3bittable2x4percep_internal(img, width, height, startx + 2, starty, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, &best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_norm, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_norm, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_norm, 0, 1, 32);

        compressed2_norm = 0;
        PUTBITS(compressed2_norm, (best_pixel_indices1_MSB), 8, 23);
        PUTBITS(compressed2_norm, (best_pixel_indices2_MSB), 8, 31);
        PUTBITS(compressed2_norm, (best_pixel_indices1_LSB), 8, 7);
        PUTBITS(compressed2_norm, (best_pixel_indices2_LSB), 8, 15);

   
    }

    /* Now try flipped blocks 4x2: */

    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty, avg_color_kzFloat1);
    kzcEtcComputeAverageColor4x2noQuantFloat_internal(img, width, height, startx, starty + 2, avg_color_kzFloat2);

    /* First test if avg_color1 is similar enough to avg_color2 so that */
    /* we can use differential coding of colors. */


    ktcEtcQuantize555ColorCombinedPerceptual_internal(avg_color_kzFloat1, enc_color1, dummy);
    ktcEtcQuantize555ColorCombinedPerceptual_internal(avg_color_kzFloat2, enc_color2, dummy);

    diff[0] = enc_color2[0] - enc_color1[0];   
    diff[1] = enc_color2[1] - enc_color1[1];   
    diff[2] = enc_color2[2] - enc_color1[2];

    if ((diff[0] >= -4) && (diff[0] <= 3) && (diff[1] >= -4) && (diff[1] <= 3) && (diff[2] >= -4) && (diff[2] <= 3))
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 1;

        /* The difference to be coded: */

        diff[0] = enc_color2[0] - enc_color1[0];   
        diff[1] = enc_color2[1] - enc_color1[1];   
        diff[2] = enc_color2[2] - enc_color1[2];

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 3 | (enc_color1[0] >> 2));
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 3 | (enc_color1[1] >> 2));
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 3 | (enc_color1[2] >> 2));
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 3 | (enc_color2[0] >> 2));
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 3 | (enc_color2[1] >> 2));
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 3 | (enc_color2[2] >> 2));

        /* Pack bits into the first word. */

        compressed1_flip = 0;
        PUTBITSHIGH(compressed1_flip, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip, enc_color1[0], 5, 63);
        PUTBITSHIGH(compressed1_flip, enc_color1[1], 5, 55);
        PUTBITSHIGH(compressed1_flip, enc_color1[2], 5, 47);
        PUTBITSHIGH(compressed1_flip, diff[0], 3, 58);
        PUTBITSHIGH(compressed1_flip, diff[1], 3, 50);
        PUTBITSHIGH(compressed1_flip, diff[2], 3, 42);




        /* upper part of block */
        flip_err = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, & best_pixel_indices1_LSB);
        /* lower part of block */
        flip_err += ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);

   
    }
    else
    {
        kzUint best_pixel_indices1_MSB;
        kzUint best_pixel_indices1_LSB;
        kzUint best_pixel_indices2_MSB;
        kzUint best_pixel_indices2_LSB;

        diffbit = 0;
        /* The difference is bigger than what fits in 555 plus delta - 333, so we will have */
        /* to deal with 444 444. */

        ktcEtcQuantize444ColorCombinedPerceptual_internal(avg_color_kzFloat1, enc_color1, dummy);
        ktcEtcQuantize444ColorCombinedPerceptual_internal(avg_color_kzFloat2, enc_color2, dummy);

        avg_color_quant1[0] = (kzU8)(enc_color1[0] << 4 | enc_color1[0]);
        avg_color_quant1[1] = (kzU8)(enc_color1[1] << 4 | enc_color1[1]);
        avg_color_quant1[2] = (kzU8)(enc_color1[2] << 4 | enc_color1[2]);
        avg_color_quant2[0] = (kzU8)(enc_color2[0] << 4 | enc_color2[0]);
        avg_color_quant2[1] = (kzU8)(enc_color2[1] << 4 | enc_color2[1]);
        avg_color_quant2[2] = (kzU8)(enc_color2[2] << 4 | enc_color2[2]);

        /*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
        /*     -------------------------------------------------------------------------------------------------- - */
        /*    | base col1 | base col2 | base col1 | base col2 | base col1 | base col2 | table  | table  |diff|flip| */
        /*    | R1 (4bits)| R2 (4bits)| G1 (4bits)| G2 (4bits)| B1 (4bits)| B2 (4bits)| cw 1   | cw 2   |bit |bit | */
        /*     -------------------------------------------------------------------------------------------------- - */


        /* Pack bits into the first word. */

        compressed1_flip = 0;
        PUTBITSHIGH(compressed1_flip, diffbit, 1, 33);
        PUTBITSHIGH(compressed1_flip, enc_color1[0], 4, 63);
        PUTBITSHIGH(compressed1_flip, enc_color1[1], 4, 55);
        PUTBITSHIGH(compressed1_flip, enc_color1[2], 4, 47);
        PUTBITSHIGH(compressed1_flip, enc_color2[0], 4, 59);
        PUTBITSHIGH(compressed1_flip, enc_color2[1], 4, 51);
        PUTBITSHIGH(compressed1_flip, enc_color2[2], 4, 43);


        /* upper part of block */
        flip_err = ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty, avg_color_quant1, &best_table1, &best_pixel_indices1_MSB, & best_pixel_indices1_LSB);
        /* lower part of block */
        flip_err += ktcEtcTryalltables3bittable4x2percep_internal(img, width, height, startx, starty + 2, avg_color_quant2, &best_table2, &best_pixel_indices2_MSB, & best_pixel_indices2_LSB);

        PUTBITSHIGH(compressed1_flip, best_table1, 3, 39);
        PUTBITSHIGH(compressed1_flip, best_table2, 3, 36);
        PUTBITSHIGH(compressed1_flip, 1, 1, 32);

        best_pixel_indices1_MSB |= (best_pixel_indices2_MSB << 2);
        best_pixel_indices1_LSB |= (best_pixel_indices2_LSB << 2);
       
        compressed2_flip = ((best_pixel_indices1_MSB & 0xFFff) << 16) | (best_pixel_indices1_LSB & 0xFFff);


    }

    /* Now lets see which is the best table to use. Only 8 tables are possible. */

    if (norm_err <= flip_err)
    {

        *compressed1 = compressed1_norm | 0;
        *compressed2 = compressed2_norm;

    }
    else
    {

        *compressed1 = compressed1_flip | 1;
        *compressed2 = compressed2_flip;
    }
}

static void ktcEtcDecompressBlockDiffFlip_internal(kzUint block_part1, kzUint block_part2, kzU8* img, kzInt width, kzInt height,
                                                   kzInt startx, kzInt starty)
{
    kzU8 avg_color[3], enc_color1[3], enc_color2[3];
    kzS8 diff[3];
    kzInt table;
    kzInt index, shift;
    kzInt diffbit;
    kzInt flipbit;
    kzInt unscramble[4] = ETC_UNSCRAMBLE;
    KZ_UNUSED_PARAMETER(height);

    diffbit = (GETBITSHIGH(block_part1, 1, 33));
    flipbit = (GETBITSHIGH(block_part1, 1, 32));

    if (diffbit == 0)
    {
        kzUint pixel_indices_MSB, pixel_indices_LSB;

        /* We have diffbit = 0. */

        /* First decode left part of block. */
        avg_color[0] = GETBITSHIGH(block_part1, 4, 63);
        avg_color[1] = GETBITSHIGH(block_part1, 4, 55);
        avg_color[2] = GETBITSHIGH(block_part1, 4, 47);

        /* Here, we should really multiply by 17 instead of 16. This can */
        /* be done by just copying the four lower bits to the upper ones */
        /* while keeping the lower bits. */
        avg_color[0] |= (avg_color[0] << 4);
        avg_color[1] |= (avg_color[1] << 4);
        avg_color[2] |= (avg_color[2] << 4);

        table = GETBITSHIGH(block_part1, 3, 39) << 1;

       
        pixel_indices_MSB = GETBITS(block_part2, 16, 31);
        pixel_indices_LSB = GETBITS(block_part2, 16, 15);

        if ((flipbit) == 0)
        {
            kzInt x;
            /* We should not flip */
            shift = 0;
            for (x = startx; x < startx + 2; x++)
            {
                kzInt y;
                for (y = starty; y < starty + 4; y++)
                {
                    index = ((pixel_indices_MSB >> shift) & 1) << 1;
                    index |= ((pixel_indices_LSB >> shift) & 1);
                    shift++;
                    index = unscramble[index];

                    RED(img, width, x, y) = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    GREEN(img, width, x, y) = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    BLUE(img, width, x, y) = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                }
            }
        }
        else
        {
            kzInt x;
            /* We should flip */
            shift = 0;
            for (x = startx; x < startx + 4; x++)
            {
                kzInt y;
                for (y = starty; y < starty + 2; y++)
                {
                    index = ((pixel_indices_MSB >> shift) & 1) << 1;
                    index |= ((pixel_indices_LSB >> shift) & 1);
                    shift++;
                    index = unscramble[index];

                    RED(img, width, x, y) = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    GREEN(img, width, x, y) = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    BLUE(img, width, x, y) = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                }
                shift += 2;
            }
        }

        /* Now decode other part of block. */
        avg_color[0] = GETBITSHIGH(block_part1, 4, 59);
        avg_color[1] = GETBITSHIGH(block_part1, 4, 51);
        avg_color[2] = GETBITSHIGH(block_part1, 4, 43);

        /* Here, we should really multiply by 17 instead of 16. This can */
        /* be done by just copying the four lower bits to the upper ones */
        /* while keeping the lower bits. */
        avg_color[0] |= (avg_color[0] << 4);
        avg_color[1] |= (avg_color[1] << 4);
        avg_color[2] |= (avg_color[2] << 4);

        table = GETBITSHIGH(block_part1, 3, 36) << 1;
        pixel_indices_MSB = GETBITS(block_part2, 16, 31);
        pixel_indices_LSB = GETBITS(block_part2, 16, 15);

        if ((flipbit) == 0)
        {
            kzInt x;
            /* We should not flip */
            shift = 8;
            for (x = startx + 2; x < startx + 4; x++)
            {
                kzInt y;
                for (y = starty; y < starty + 4; y++)
                {
                    index = ((pixel_indices_MSB >> shift) & 1) << 1;
                    index |= ((pixel_indices_LSB >> shift) & 1);
                    shift++;
                    index = unscramble[index];

                    RED(img, width, x, y) = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    GREEN(img, width, x, y) = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    BLUE(img, width, x, y) = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                }
            }
        }
        else
        {
            kzInt x;
            /* We should flip */
            shift = 2;
            for (x = startx; x < startx + 4; x++)
            {
                kzInt y;
                for (y = starty + 2; y < starty + 4; y++)
                {
                    index = ((pixel_indices_MSB >> shift) & 1) << 1;
                    index |= ((pixel_indices_LSB >> shift) & 1);
                    shift++;
                    index = unscramble[index];

                    RED(img, width, x, y) = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    GREEN(img, width, x, y) = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    BLUE(img, width, x, y) = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                }
                shift += 2;
            }
        }
    }
    else
    {
        /* We have diffbit = 1. */

/*     63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34  33  32 */
/*     -------------------------------------------------------------------------------------------------- - */
/*    | base col1    | dcol 2 | base col1    | dcol 2 | base col 1   | dcol 2 | table  | table  |diff|flip| */
/*    | R1' (5 bits) | dR2    | G1' (5 bits) | dG2    | B1' (5 bits) | dB2    | cw 1   | cw 2   |bit |bit | */
/*     -------------------------------------------------------------------------------------------------- - */
/**/
/**/
/*    c) bit layout in bits 31 through 0 (in both cases) */
/**/
/*     31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3   2   1  0 */
/*     -------------------------------------------------------------------------------------------------- */
/*    |       most significant pixel index bits       |         least significant pixel index bits       |  */
/*    | p| o| n| m| l| k| j| i| h| g| f| e| d| c| b| a| p| o| n| m| l| k| j| i| h| g| f| e| d| c | b | a | */
/*     --------------------------------------------------------------------------------------------------     */

        kzUint pixel_indices_MSB, pixel_indices_LSB;

        /* First decode left part of block. */
        enc_color1[0] = GETBITSHIGH(block_part1, 5, 63);
        enc_color1[1] = GETBITSHIGH(block_part1, 5, 55);
        enc_color1[2] = GETBITSHIGH(block_part1, 5, 47);


        /* Expand from 5 to 8 bits */
        avg_color[0] = (enc_color1[0] << 3) | (enc_color1[0] >> 2);
        avg_color[1] = (enc_color1[1] << 3) | (enc_color1[1] >> 2);
        avg_color[2] = (enc_color1[2] << 3) | (enc_color1[2] >> 2);


        table = GETBITSHIGH(block_part1, 3, 39) << 1;

           
        pixel_indices_MSB = GETBITS(block_part2, 16, 31);
        pixel_indices_LSB = GETBITS(block_part2, 16, 15);

        if ((flipbit) == 0)
        {
            kzInt x;
            /* We should not flip */
            shift = 0;
            for (x = startx; x < startx + 2; x++)
            {
                kzInt y;
                for (y = starty; y < starty + 4; y++)
                {
                    index = ((pixel_indices_MSB >> shift) & 1) << 1;
                    index |= ((pixel_indices_LSB >> shift) & 1);
                    shift++;
                    index = unscramble[index];

                    RED(img, width, x, y) = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    GREEN(img, width, x, y) = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    BLUE(img, width, x, y) = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                }
            }
        }
        else
        {
            kzInt x;
            /* We should flip */
            shift = 0;
            for (x = startx; x < startx + 4; x++)
            {
                kzInt y;
                for (y = starty; y < starty + 2; y++)
                {
                    index = ((pixel_indices_MSB >> shift) & 1) << 1;
                    index |= ((pixel_indices_LSB >> shift) & 1);
                    shift++;
                    index = unscramble[index];

                    RED(img, width, x, y) = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    GREEN(img, width, x, y) = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    BLUE(img, width, x, y) = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                }
                shift += 2;
            }
        }


        /* Now decode right part of block. */


        diff[0] = GETBITSHIGH(block_part1, 3, 58);
        diff[1] = GETBITSHIGH(block_part1, 3, 50);
        diff[2] = GETBITSHIGH(block_part1, 3, 42);

        enc_color2[0] = enc_color1[0] + diff[0];
        enc_color2[1] = enc_color1[1] + diff[1];
        enc_color2[2] = enc_color1[2] + diff[2];

        /* Extend sign bit to entire byte. */
        diff[0] = (diff[0] << 5);
        diff[1] = (diff[1] << 5);
        diff[2] = (diff[2] << 5);
        diff[0] = diff[0] >> 5;
        diff[1] = diff[1] >> 5;
        diff[2] = diff[2] >> 5;

        /* Calculale second color */
        enc_color2[0] = enc_color1[0] + diff[0];
        enc_color2[1] = enc_color1[1] + diff[1];
        enc_color2[2] = enc_color1[2] + diff[2];

        /* Expand from 5 to 8 bits */
        avg_color[0] = (enc_color2[0] << 3) | (enc_color2[0] >> 2);
        avg_color[1] = (enc_color2[1] << 3) | (enc_color2[1] >> 2);
        avg_color[2] = (enc_color2[2] << 3) | (enc_color2[2] >> 2);


        table = GETBITSHIGH(block_part1, 3, 36) << 1;
        pixel_indices_MSB = GETBITS(block_part2, 16, 31);
        pixel_indices_LSB = GETBITS(block_part2, 16, 15);

        if ((flipbit) == 0)
        {
            kzInt x;
            /* We should not flip */
            shift = 8;
            for (x = startx + 2; x < startx + 4; x++)
            {
                kzInt y;
                for (y = starty; y < starty + 4; y++)
                {
                    index = ((pixel_indices_MSB >> shift) & 1) << 1;
                    index |= ((pixel_indices_LSB >> shift) & 1);
                    shift++;
                    index = unscramble[index];

                    RED(img, width, x, y) = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    GREEN(img, width, x, y) = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    BLUE(img, width, x, y) = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                }
            }
        }
        else
        {
            kzInt x;
            /* We should flip */
            shift = 2;
            for (x = startx; x < startx + 4; x++)
            {
                kzInt y;
                for (y = starty + 2; y < starty + 4; y++)
                {
                    index = ((pixel_indices_MSB >> shift) & 1) << 1;
                    index |= ((pixel_indices_LSB >> shift) & 1);
                    shift++;
                    index = unscramble[index];

                    RED(img, width, x, y) = (kzU8)CLAMP(0, avg_color[0] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    GREEN(img, width, x, y) = (kzU8)CLAMP(0, avg_color[1] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                    BLUE(img, width, x, y) = (kzU8)CLAMP(0, avg_color[2] + KZC_ETC_COMPRESS_PARAMS[table][index], 255);
                }
                shift += 2;
            }
        }
    }
}

static kzDouble ktcEtcCalcBlockErrorRGB_internal(const kzU8* img, const kzU8* imgdec, kzInt width, kzInt height, kzInt startx, kzInt starty)
{
    kzInt xx, yy;
    kzDouble err;
    KZ_UNUSED_PARAMETER(height);
    err = 0;

    for (xx = startx; xx <  startx + 4; xx++)
    {
        for (yy = starty; yy < starty + 4; yy++)
        {
            err += SQUARE(1.0 * RED(img, width, xx, yy)  - 1.0 * RED(imgdec, width, xx, yy));
            err += SQUARE(1.0 * GREEN(img, width, xx, yy) - 1.0 * GREEN(imgdec, width, xx, yy));
            err += SQUARE(1.0 * BLUE(img, width, xx, yy) - 1.0 * BLUE(imgdec, width, xx, yy));
        }
    }

    return err;
}

static kzDouble ktcEtcCalcBlockPerceptualErrorRGB_internal(const kzU8* img, const kzU8* imgdec, kzInt width, kzInt height, kzInt startx, kzInt starty)
{
    kzInt xx, yy;
    kzDouble err;
    KZ_UNUSED_PARAMETER(height);
    err = 0;

    for (xx = startx; xx <  startx + 4; xx++)
    {
        for (yy = starty; yy < starty + 4; yy++)
        {
            err += PERCEPTUAL_WEIGHT_R_SQUARED * SQUARE(1.0 * RED(img, width, xx, yy)  - 1.0 * RED(imgdec, width, xx, yy));
            err += PERCEPTUAL_WEIGHT_G_SQUARED * SQUARE(1.0 * GREEN(img, width, xx, yy) - 1.0 * GREEN(imgdec, width, xx, yy));
            err += PERCEPTUAL_WEIGHT_B_SQUARED * SQUARE(1.0 * BLUE(img, width, xx, yy) - 1.0 * BLUE(imgdec, width, xx, yy));
        }
    }

    return err;
}

static void ktcEtcCompressBlockDiffFlipFast_internal(const kzU8* img, kzU8* imgdec, kzInt width, kzInt height, kzInt startx, kzInt starty, kzUint* compressed1, kzUint* compressed2)
{
    kzUint average_block1;
    kzUint average_block2;
    kzDouble error_average;

    kzUint combined_block1;
    kzUint combined_block2;
    kzDouble error_combined;

    /* First quantize the average color to the nearest neighbor. */
    ktcEtcCompressBlockDiffFlipAverage_internal(img, width, height, startx, starty, &average_block1, &average_block2);
    ktcEtcDecompressBlockDiffFlip_internal(average_block1, average_block2, imgdec, width, height, startx, starty);
    error_average = ktcEtcCalcBlockErrorRGB_internal(img, imgdec, width, height, startx, starty);

    /* Then try to quantize the average color to the nearest (1, 1, 1) - line. */
    ktcEtcCompressBlockDiffFlipCombined_internal(img, width, height, startx, starty, &combined_block1, &combined_block2);
    ktcEtcDecompressBlockDiffFlip_internal(combined_block1, combined_block2, imgdec, width, height, startx, starty);
    error_combined = ktcEtcCalcBlockErrorRGB_internal(img, imgdec, width, height, startx, starty);

    if (error_combined < error_average)
    {
        *compressed1 = combined_block1;
        *compressed2 = combined_block2;
    }
    else
    {
        *compressed1 = average_block1;
        *compressed2 = average_block2;
    }
}

static void ktcEtcCompressBlockDiffFlipFastPerceptual_internal(const kzU8* img, kzU8* imgdec, kzInt width, kzInt height, kzInt startx, kzInt starty, kzUint* compressed1, kzUint* compressed2)
{
    kzUint average_block1;
    kzUint average_block2;
    kzDouble error_average;

    kzUint combined_block1;
    kzUint combined_block2;
    kzDouble error_combined;

    /* First quantize the average color to the nearest neighbor. */
    ktcEtcCompressBlockDiffFlipAveragePerceptual_internal(img, width, height, startx, starty, &average_block1, &average_block2);
    ktcEtcDecompressBlockDiffFlip_internal(average_block1, average_block2, imgdec, width, height, startx, starty);
    error_average = ktcEtcCalcBlockPerceptualErrorRGB_internal(img, imgdec, width, height, startx, starty);

    /* Then try to quantize the average color to the nearest (1, 1, 1) - line. */
    ktcEtcCompressBlockDiffFlipCombinedPerceptual_internal(img, width, height, startx, starty, &combined_block1, &combined_block2);
    ktcEtcDecompressBlockDiffFlip_internal(combined_block1, combined_block2, imgdec, width, height, startx, starty);
    error_combined = ktcEtcCalcBlockPerceptualErrorRGB_internal(img, imgdec, width, height, startx, starty);

    if (error_combined < error_average)
    {
        *compressed1 = combined_block1;
        *compressed2 = combined_block2;
    }
    else
    {
        *compressed1 = average_block1;
        *compressed2 = average_block2;
    }
}



static void ktcEtcWriteBigEndian4byteWord_internal(const kzUint* blockadr, kzU8* buffer, kzUint* pos)
{
    kzU8 bytes[4];
    kzUint block;

    block = blockadr[0];

    bytes[0] = (block >> 24) & 0xFF;
    bytes[1] = (block >> 16) & 0xFF;
    bytes[2] = (block >> 8) & 0xFF;
    bytes[3] = (block >> 0) & 0xFF;

   
    buffer[(*pos)++] = bytes[0];
    buffer[(*pos)++] = bytes[1];
    buffer[(*pos)++] = bytes[2];
    buffer[(*pos)++] = bytes[3];
}

static void ktcEtcReadBigEndian4byteWord_internal(kzUint* blockadr, const kzU8* buffer, kzUint* pos)
{
    kzU8 bytes[4];
    kzUint block;

    bytes[0] = *(buffer + *pos);
    bytes[1] = *(buffer + *pos + 1);
    bytes[2] = *(buffer + *pos + 2);
    bytes[3] = *(buffer + *pos + 3);

    *pos += 4;

    block = 0;

    block |= bytes[0];
    block = block << 8;
    block |= bytes[1];
    block = block << 8;
    block |= bytes[2];
    block = block << 8;
    block |= bytes[3];

    blockadr[0] = block;
}


kzsError kzcEtcCompressImageToBuffer(const struct KzcMemoryManager* memoryManager, const kzU8* img, kzInt width, kzInt height,
                                     kzU8* dst, enum KzcEtcCompressionMode compressionMode, kzUint* out_bytesWritten)
{
    kzsError result;
    kzInt x, y;
    kzUint block1, block2;
   
    kzUint buffer_position = 0;
   
    kzU8* imgdec;

    kzUint temporaryArraySize = width * height * 3;

    result = kzcMemoryAllocPointer(memoryManager, &imgdec, temporaryArraySize, "ETC compresssion temporary array");
    kzsErrorForward(result);

    /* Also write the active pixels. For instance, if we want to compress */
    /* a 128 x 129 image, we have to extend it to 128 x 132 pixels. */
    /* Then the wi and hi written above will be 128 and 132, but the */
    /* additional information that we write below will be 128 and 129, */
    /* to indicate that it is only the top 129 lines of data in the */
    /* decompressed image that will be valid data, and the rest will */
    /* be just garbage. */


    /*/ xxx */
    for (y = 0; y < height / 4; y++)
    {
        for (x = 0; x < width / 4; x++)
        {
            switch (compressionMode)
            {
                case KZC_ETC_COMPRESSION_MODE_FAST:
                {
                    /* FAST only tests the two most likely base colors. */
                    ktcEtcCompressBlockDiffFlipFast_internal(img, imgdec, width, height, 4 * x, 4 * y, &block1, &block2);
                    break;
                }

                case KZC_ETC_COMPRESSION_MODE_MEDIUM:
                {
                    /* The MEDIUM version tests all colors in a 3x3x3 cube around the average colors */
                    /* This increases the likelihood that the differential mode is selected. */
                    ktcEtcCompressBlockDiffFlipMedium_internal(img, width, height, 4 * x, 4 * y, &block1, &block2);
                    break;
                }

                case KZC_ETC_COMPRESSION_MODE_SLOW:
                {
                    /* The SLOW version tests all colors in a a 5x5x5 cube around the average colors */
                    /* It also tries the nondifferential mode for each block even if the differential succeeds. */
                    ktcEtcCompressBlockDiffFlipSlow_internal(img, width, height, 4 * x, 4 * y, &block1, &block2);
                    break;
                }

                case KZC_ETC_COMPRESSION_MODE_FAST_PERCEPTUAL:
                {
                    /* FAST with PERCEPTUAL error metric */
                    ktcEtcCompressBlockDiffFlipFastPerceptual_internal(img, imgdec, width, height, 4 * x, 4 * y, &block1, &block2);
                    break;
                }

                case KZC_ETC_COMPRESSION_MODE_MEDIUM_PERCEPTUAL:
                {
                    /* MEDIUM with PERCEPTUAL error metric */
                    ktcEtcCompressBlockDiffFlipMediumPerceptual_internal(img, width, height, 4 * x, 4 * y, &block1, &block2);
                    break;
                }

                case KZC_ETC_COMPRESSION_MODE_SLOW_PERCEPTUAL:
                {
                    /* SLOW with PERCEPTUAL error metric */
                    ktcEtcCompressBlockDiffFlipSlowPerceptual_internal(img, width, height, 4 * x, 4 * y, &block1, &block2);
                    break;
                }

                default:
                {
                    kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Bad compression mode");
                }
            }
            ktcEtcWriteBigEndian4byteWord_internal(&block1, dst, &buffer_position);
            ktcEtcWriteBigEndian4byteWord_internal(&block2, dst, &buffer_position);

        }
    }
    kzsLog(KZS_LOG_LEVEL_INFO, "Compressed image buffer to ETC format");

    result = kzcMemoryFreePointer(imgdec);
    kzsErrorForward(result);

    *out_bytesWritten = buffer_position;
    kzsSuccess();
}


kzsError kzcEtcUncompressImageToBuffer(const struct KzcMemoryManager* memoryManager, const kzU8* img, kzInt width, kzInt height,
                                       kzInt active_width, kzInt active_height, kzU8* dst, kzUint* out_bytesWritten)
{
    kzsError result;
    kzUint bytes_written;
    kzUint block_part1, block_part2;
    kzU8* buffer;
    kzInt xx, yy;
    kzUint buffer_position = 0;
    kzInt y;

    kzUint buffer_size = width * height *3;

    result = kzcMemoryAllocPointer(memoryManager, &buffer, buffer_size, "ETC decompression temporary buffer");
    kzsErrorForward(result);

    /* active_width and active_height tell us how many pixels contain active data (the rest are just
       for making sure we have a 2 * a x 4 * b size).
       the target buffer *dst should contain space for at least active_width * active_height * 3 bits.
       and the decompression buffer *buffer should contain enough space for width * height * 3 bits */

    /* Actual decompression starts here */


    for (y = 0; y < height / 4; y++)
    {
        kzInt x;
        for (x = 0; x < width / 4; x++)
        {
            ktcEtcReadBigEndian4byteWord_internal(&block_part1, img, &buffer_position);
            ktcEtcReadBigEndian4byteWord_internal(&block_part2, img, &buffer_position);
            ktcEtcDecompressBlockDiffFlip_internal(block_part1, block_part2, buffer, width, height, 4 * x, 4 * y);
        }
    }

    /* Ok, and now only write out the active pixels (But only if the active pixels differ from the total pixels) */

    {
        /* Convert from total area to active area: */
        for (yy = 0; yy < active_height; yy++)
        {
            for (xx = 0; xx <  active_width; xx++)
            {
                dst[(yy * active_width) * 3 + xx * 3 + 0] = buffer[(yy * width) * 3 + xx * 3 + 0];
                dst[(yy * active_width) * 3 + xx * 3 + 1] = buffer[(yy * width) * 3 + xx * 3 + 1];
                dst[(yy * active_width) * 3 + xx * 3 + 2] = buffer[(yy * width) * 3 + xx * 3 + 2];
            }
        }
        bytes_written = active_width * active_height * 3;
    }
    result = kzcMemoryFreePointer(buffer);
    kzsErrorForward(result);

    /*kzsLog(KZS_LOG_LEVEL_INFO, "Uncompressed image buffer from ETC format");*/

    *out_bytesWritten = bytes_written;
    kzsSuccess();
}

kzUint kzcEtcGetCompressedBufferMemorySize(kzUint width, kzUint height)
{
    kzUint size = width * height / 2;
    size = kzsMaxU(8, size); /* Minimum size is one block */
    return size;
}
