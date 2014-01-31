/**
* \file
* Kanzi input, OMAP Zoom specific implementation
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_input_native.h"

#include <system/input/kzs_input.h>
#include <system/kzs_error_codes.h>


#define KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE  116

/* TODO: Add grouping comments */
/* OMAP Zoom2 specific translation table */
static enum KzsInputKey kzsInputKeyTranslationTable[KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE] = 
{
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,  
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,        /* "reload" */

    KZS_KEY_UNKNOWN,
    KZS_KEY_Q,
    KZS_KEY_W,
    KZS_KEY_E,
    KZS_KEY_R,

    KZS_KEY_T,
    KZS_KEY_Y,
    KZS_KEY_U,
    KZS_KEY_I,
    KZS_KEY_O,

    KZS_KEY_P,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_ENTER,
    KZS_KEY_UNKNOWN,

    /* 30 */
    KZS_KEY_A,
    KZS_KEY_S,
    KZS_KEY_D,
    KZS_KEY_F,
    KZS_KEY_G,

    KZS_KEY_H,
    KZS_KEY_J,
    KZS_KEY_K,
    KZS_KEY_L,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_MODIFIER2,  /* halfblock on key, ctrl/fn perhaps? */
    KZS_KEY_UNKNOWN,
    KZS_KEY_Z,

    /* 45 */
    KZS_KEY_X,
    KZS_KEY_C,
    KZS_KEY_V,
    KZS_KEY_B,
    KZS_KEY_N,

    KZS_KEY_M,
    KZS_KEY_UNKNOWN,
    KZS_KEY_PERIOD,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_SPACE,
    KZS_KEY_MODIFIER1,
    KZS_KEY_F1,

    /* 60 */
    KZS_KEY_F2,
    KZS_KEY_F3,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 70 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_PLUS,
    KZS_KEY_UNKNOWN,

    /* 80 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 90 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 100 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,    /* home -key */
    KZS_KEY_ARROW_UP,
    KZS_KEY_UNKNOWN,

    KZS_KEY_ARROW_LEFT,
    KZS_KEY_ARROW_RIGHT,
    KZS_KEY_ESC,        /* the RED key */
    KZS_KEY_ARROW_DOWN,
    KZS_KEY_UNKNOWN,

    /* 110 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_VOLUME_DOWN,

    KZS_KEY_VOLUME_UP,
};

enum KzsInputKey kzsInputNativeGetTranslatedKey(kzUint button)
{
    /* TODO: Only one return point. */
    if(button >= KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE)
    {
        switch(button)
        {
            /* special cases */ 
            case 148: return KZS_KEY_SOFTKEY1;  /* softkeys below the screen */
            case 149: return KZS_KEY_SOFTKEY2;
            case 202: return KZS_KEY_SOFTKEY3;
            case 203: return KZS_KEY_SOFTKEY4;
            case 231: return KZS_KEY_ENTER;     /* green "answer" button */
            case 353: return KZS_KEY_ENTER;     /* the "OK" button in the middle of the arrow keys */
            default: return KZS_KEY_UNKNOWN;
        }
    }
    else
    {
        return kzsInputKeyTranslationTable[button];
    }
}
