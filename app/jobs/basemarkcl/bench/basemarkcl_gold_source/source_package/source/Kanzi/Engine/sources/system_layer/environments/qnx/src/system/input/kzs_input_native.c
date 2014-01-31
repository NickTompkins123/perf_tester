/**
* \file
* Kanzi input, QNX specific implementation
*
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_input_native.h"

#include <system/input/kzs_input.h>
#include <system/kzs_error_codes.h>


#define KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE  9

static enum KzsInputKey kzsInputKeyTranslationTable[KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE] =
{
    KZS_KEY_ARROW_LEFT,
    KZS_KEY_ARROW_RIGHT,
    KZS_KEY_ENTER,
    KZS_KEY_SOFTKEY1,
    KZS_KEY_SOFTKEY2,
    KZS_KEY_SOFTKEY3,
    KZS_KEY_SOFTKEY4,
    KZS_KEY_H,
    KZS_KEY_ESC
};

enum KzsInputKey kzsInputNativeGetTranslatedKey(kzUint button)
{
    enum KzsInputKey key;

    if(button >= KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE)
    {
        key = KZS_KEY_UNKNOWN;
    }
    else
    {
        key = kzsInputKeyTranslationTable[button];
    }

    return key;
}
