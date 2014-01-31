/**
* \file
* Kanzi input, Android specific implementation
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_input_native.h"

#include <system/input/kzs_input.h>
#include <system/kzs_error_codes.h>


#define KZS_INPUTNATIVE_KEY_TRANSLATION_TABLE_SIZE  96

static enum KzsInputKey kzsInputKeyTranslationTable[KZS_INPUTNATIVE_KEY_TRANSLATION_TABLE_SIZE] = 
{
    KZS_KEY_UNKNOWN, /* KEYCODE_UNKNOWN         = 0*/
    KZS_KEY_SOFTKEY1, /* KEYCODE_SOFT_LEFT       = 1*/
    KZS_KEY_SOFTKEY2, /* KEYCODE_SOFT_RIGHT      = 2*/
    KZS_KEY_HOME, /* KEYCODE_HOME            = 3*/
    KZS_KEY_ESC, /* KEYCODE_BACK            = 4*/
    KZS_KEY_UNKNOWN, /* KEYCODE_CALL            = 5*/
    KZS_KEY_UNKNOWN, /* KEYCODE_ENDCALL         = 6*/
    KZS_KEY_0, /* KEYCODE_0               = 7*/
    KZS_KEY_1, /* KEYCODE_1               = 8*/
    KZS_KEY_2, /* KEYCODE_2               = 9*/
    KZS_KEY_3, /* KEYCODE_3               = 10*/
    KZS_KEY_4, /* KEYCODE_4               = 11*/
    KZS_KEY_5, /* KEYCODE_5               = 12*/
    KZS_KEY_6, /* KEYCODE_6               = 13*/
    KZS_KEY_7, /* KEYCODE_7               = 14*/
    KZS_KEY_8, /* KEYCODE_8               = 15*/
    KZS_KEY_9, /* KEYCODE_9               = 16*/
    KZS_KEY_UNKNOWN, /* KEYCODE_STAR            = 17*/
    KZS_KEY_UNKNOWN, /* KEYCODE_POUND           = 18*/
    KZS_KEY_ARROW_UP, /* KEYCODE_DPAD_UP         = 19*/
    KZS_KEY_ARROW_DOWN, /* KEYCODE_DPAD_DOWN       = 20*/
    KZS_KEY_ARROW_LEFT, /* KEYCODE_DPAD_LEFT       = 21*/
    KZS_KEY_ARROW_RIGHT, /* KEYCODE_DPAD_RIGHT      = 22*/
    KZS_KEY_UNKNOWN, /* KEYCODE_DPAD_CENTER     = 23*/
    KZS_KEY_UNKNOWN, /* KEYCODE_VOLUME_UP       = 24*/
    KZS_KEY_UNKNOWN, /* KEYCODE_VOLUME_DOWN     = 25*/
    KZS_KEY_UNKNOWN, /* KEYCODE_POWER           = 26*/
    KZS_KEY_UNKNOWN, /* KEYCODE_CAMERA          = 27*/
    KZS_KEY_UNKNOWN, /* KEYCODE_CLEAR           = 28*/
    KZS_KEY_A, /* KEYCODE_A               = 29*/
    KZS_KEY_B, /* KEYCODE_B               = 30*/
    KZS_KEY_C, /* KEYCODE_C               = 31*/
    KZS_KEY_D, /* KEYCODE_D               = 32*/
    KZS_KEY_E, /* KEYCODE_E               = 33*/
    KZS_KEY_F, /* KEYCODE_F               = 34*/
    KZS_KEY_G, /* KEYCODE_G               = 35*/
    KZS_KEY_H, /* KEYCODE_H               = 36*/
    KZS_KEY_I, /* KEYCODE_I               = 37*/
    KZS_KEY_J, /* KEYCODE_J               = 38*/
    KZS_KEY_K, /* KEYCODE_K               = 39*/
    KZS_KEY_L, /* KEYCODE_L               = 40*/
    KZS_KEY_M, /* KEYCODE_M               = 41*/
    KZS_KEY_N, /* KEYCODE_N               = 42*/
    KZS_KEY_O, /* KEYCODE_O               = 43*/
    KZS_KEY_P, /* KEYCODE_P               = 44*/
    KZS_KEY_Q, /* KEYCODE_Q               = 45*/
    KZS_KEY_R, /* KEYCODE_R               = 46*/
    KZS_KEY_S, /* KEYCODE_S               = 47*/
    KZS_KEY_T, /* KEYCODE_T               = 48*/
    KZS_KEY_U, /* KEYCODE_U               = 49*/
    KZS_KEY_V, /* KEYCODE_V               = 50*/
    KZS_KEY_W, /* KEYCODE_W               = 51*/
    KZS_KEY_X, /* KEYCODE_X               = 52*/
    KZS_KEY_Y, /* KEYCODE_Y               = 53*/
    KZS_KEY_Z, /* KEYCODE_Z               = 54*/
    KZS_KEY_COMMA, /* KEYCODE_COMMA           = 55*/
    KZS_KEY_PERIOD, /* KEYCODE_PERIOD          = 56*/
    KZS_KEY_LEFT_ALT, /* KEYCODE_ALT_LEFT        = 57*/
    KZS_KEY_RIGHT_ALT, /* KEYCODE_ALT_RIGHT       = 58*/
    KZS_KEY_LEFT_SHIFT, /* KEYCODE_SHIFT_LEFT      = 59*/
    KZS_KEY_RIGHT_SHIFT, /* KEYCODE_SHIFT_RIGHT     = 60*/
    KZS_KEY_TAB, /* KEYCODE_TAB             = 61*/
    KZS_KEY_SPACE, /* KEYCODE_SPACE           = 62*/
    KZS_KEY_UNKNOWN, /* KEYCODE_SYM             = 63*/
    KZS_KEY_UNKNOWN, /* KEYCODE_EXPLORER        = 64*/
    KZS_KEY_UNKNOWN, /* KEYCODE_ENVELOPE        = 65*/
    KZS_KEY_ENTER, /* KEYCODE_ENTER           = 66*/
    KZS_KEY_DELETE, /* KEYCODE_DEL             = 67*/
    KZS_KEY_UNKNOWN, /* KEYCODE_GRAVE           = 68*/
    KZS_KEY_MINUS, /* KEYCODE_MINUS           = 69*/
    KZS_KEY_UNKNOWN, /* KEYCODE_EQUALS          = 70*/
    KZS_KEY_UNKNOWN, /* KEYCODE_LEFT_BRACKET    = 71*/
    KZS_KEY_UNKNOWN, /* KEYCODE_RIGHT_BRACKET   = 72*/
    KZS_KEY_UNKNOWN, /* KEYCODE_BACKSLASH       = 73*/
    KZS_KEY_UNKNOWN, /* KEYCODE_SEMICOLON       = 74*/
    KZS_KEY_UNKNOWN, /* KEYCODE_APOSTROPHE      = 75*/
    KZS_KEY_UNKNOWN, /* KEYCODE_SLASH           = 76*/
    KZS_KEY_UNKNOWN, /* KEYCODE_AT              = 77*/
    KZS_KEY_UNKNOWN, /* KEYCODE_NUM             = 78*/
    KZS_KEY_UNKNOWN, /* KEYCODE_HEADSETHOOK     = 79*/
    KZS_KEY_UNKNOWN, /* KEYCODE_FOCUS           = 80;*/
    KZS_KEY_PLUS, /* KEYCODE_PLUS            = 81*/
    KZS_KEY_MENU_KEY, /* KEYCODE_MENU            = 82*/
    KZS_KEY_UNKNOWN, /* KEYCODE_NOTIFICATION    = 83*/
    KZS_KEY_UNKNOWN, /* KEYCODE_SEARCH          = 84*/
    KZS_KEY_UNKNOWN, /* KEYCODE_MEDIA_PLAY_PAUSE= 85*/
    KZS_KEY_UNKNOWN, /* KEYCODE_MEDIA_STOP      = 86*/
    KZS_KEY_UNKNOWN, /* KEYCODE_MEDIA_NEXT      = 87*/
    KZS_KEY_UNKNOWN, /* KEYCODE_MEDIA_PREVIOUS  = 88*/
    KZS_KEY_UNKNOWN, /* KEYCODE_MEDIA_REWIND    = 89*/
    KZS_KEY_UNKNOWN, /* KEYCODE_MEDIA_FAST_FORWARD = 90*/
    KZS_KEY_UNKNOWN, /* KEYCODE_MUTE            = 91*/
    KZS_KEY_PAGE_UP, /* KEYCODE_PAGE_UP         = 92*/
    KZS_KEY_PAGE_DOWN, /* KEYCODE_PAGE_DOWN       = 93*/
    KZS_KEY_UNKNOWN, /* KEYCODE_PICTSYMBOLS     = 94;*/
    KZS_KEY_UNKNOWN /* KEYCODE_SWITCH_CHARSET  = 95;*/
};

enum KzsInputKey kzsInputNativeGetTranslatedKey(kzUint button)
{
    enum KzsInputKey key;

    if(button >= KZS_INPUTNATIVE_KEY_TRANSLATION_TABLE_SIZE)
    {
        key = KZS_KEY_UNKNOWN;
    }
    else
    {
        key = kzsInputKeyTranslationTable[button];
    }

    return key;
}
