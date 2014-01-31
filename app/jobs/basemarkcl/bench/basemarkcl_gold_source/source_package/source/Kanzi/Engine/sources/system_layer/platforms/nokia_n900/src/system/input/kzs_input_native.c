/**
* \file
* Kanzi input, Nokia N900 specific implementation
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_input_native.h"

#include <system/input/kzs_input.h>
#include <system/kzs_error_codes.h>


#define KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE  120

/* Nokia N900 specific translation table, Finnish layout */
/* EXCEPT arrow keys from English layout (overriding KZS_KEY_AUML and KZS_KEY_PERIOD in Finnish layout) */
static enum KzsInputKey kzsInputKeyTranslationTable[KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE] = 
{
    /* 0 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 5 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,  
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 10 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 15 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 20 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_BACKSPACE,
    KZS_KEY_UNKNOWN,
    KZS_KEY_Q,
    
    /* 25 */
    KZS_KEY_W,
    KZS_KEY_E,
    KZS_KEY_R,
    KZS_KEY_T,
    KZS_KEY_Y,
    
    /* 30 */
    KZS_KEY_U,
    KZS_KEY_I,
    KZS_KEY_O,
    KZS_KEY_P,
    KZS_KEY_UNKNOWN,
    
    /* 35 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_ENTER,
    KZS_KEY_MODIFIER2,
    KZS_KEY_A,
    KZS_KEY_S,
    
    /* 40 */
    KZS_KEY_D,
    KZS_KEY_F,
    KZS_KEY_G,
    KZS_KEY_H,
    KZS_KEY_J,
    
    /* 45 */
    KZS_KEY_K,
    KZS_KEY_L,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    
    /* 50 */
    KZS_KEY_MODIFIER1,
    KZS_KEY_UNKNOWN,
    KZS_KEY_Z,
    KZS_KEY_X,
    KZS_KEY_C,
    
    /* 55 */
    KZS_KEY_V,
    KZS_KEY_B,
    KZS_KEY_N,
    KZS_KEY_M,
    KZS_KEY_ARING,
    
    /* 60 */
    KZS_KEY_OUML,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 65 */
    KZS_KEY_SPACE,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    
    /* 70 */
    KZS_KEY_SPACE,
    KZS_KEY_MODIFIER1,
    KZS_KEY_F1,
    KZS_KEY_F2,
    KZS_KEY_F3,

    /* 75 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 80 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 85 */
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

    /* 95 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 100 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 105 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    /* 110 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_ARROW_UP,          /* English layout, KZS_KEY_AUML on Finnish */
    KZS_KEY_UNKNOWN,
    KZS_KEY_ARROW_LEFT,        /* English layout, KZS_KEY_PERIOD on Finnish */
    KZS_KEY_ARROW_RIGHT,
    
    /* 115 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_ARROW_DOWN,        /* English layout, KZS_KEY_ARROW_LEFT on Finnish */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
};

enum KzsInputKey kzsInputNativeGetTranslatedKey(kzUint button)
{
    return (button < KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE) ? kzsInputKeyTranslationTable[button] : KZS_KEY_UNKNOWN;
}
