/**
* \file
* Kanzi input, Nokia N900 specific implementation
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_input_native.h"

#include <system/input/kzs_input.h>
#include <system/kzs_error_codes.h>


#define KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE  1

static enum KzsInputKey kzsInputKeyTranslationTable[KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE] = 
{
    /* 0 */
    KZS_KEY_UNKNOWN
};

enum KzsInputKey kzsInputNativeGetTranslatedKey(kzUint button)
{
    /* TODO: Only one return point. */
    if(button >= KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE)
    {
        switch(button)
        {
            /* special cases */ 
            case 113: return KZS_KEY_ARROW_LEFT;
            case 114: return KZS_KEY_ARROW_RIGHT;
            case 111: return KZS_KEY_ARROW_UP;
            case 116: return KZS_KEY_ARROW_DOWN;
            case 22: return KZS_KEY_BACKSPACE;
            case 43: return KZS_KEY_H;
            case 36: return KZS_KEY_ENTER;
            case 10: return KZS_KEY_1;
            case 11: return KZS_KEY_2;
            case 12: return KZS_KEY_3;
            case 13: return KZS_KEY_4;
            
            default: return KZS_KEY_UNKNOWN;
        }
    }
    else
    {
        return kzsInputKeyTranslationTable[button];
    }
}
