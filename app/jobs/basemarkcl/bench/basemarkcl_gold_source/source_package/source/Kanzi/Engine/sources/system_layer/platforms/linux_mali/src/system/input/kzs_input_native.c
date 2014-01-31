/**
* \file
* Kanzi input, ARM Mali specific implementation
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_input_native.h"

#include <system/input/kzs_input.h>
#include <system/kzs_error_codes.h>

/* 
    Select input key translation by the value of ARM_MALI_INPUT:
       ARM_MALI_INPUT = 1: use TCC8900 on-board buttons (/dev/event0)
       ARM_MALI_INPUT = 2: use keyboard connected via USB (/dev/event3)
       else:               use both
*/

#if defined(ARM_MALI_INPUT) && ARM_MALI_INPUT == 1

#define KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE  12

/* TCC8900 on-board buttons (/dev/event0), translated into semi-similar keyboard commands */
static enum KzsInputKey kzsInputKeyTranslationTable[KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE] = 
{
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_ESC,          
    KZS_KEY_ENTER,            
    KZS_KEY_ARROW_UP,         
    KZS_KEY_UNKNOWN,
    KZS_KEY_BACKSPACE,         
    KZS_KEY_UNKNOWN,  
    KZS_KEY_ARROW_LEFT,       
    KZS_KEY_ARROW_RIGHT,      
    KZS_KEY_ARROW_DOWN,         
    KZS_KEY_SPACE,            
};

#elif defined(ARM_MALI_INPUT) && ARM_MALI_INPUT == 2

#define KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE  128

/* Finnish 105-key IBM keyboard connected via USB (/dev/event3) */
static enum KzsInputKey kzsInputKeyTranslationTable[KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE] = 
{
    KZS_KEY_UNKNOWN,
    KZS_KEY_ESC,
    KZS_KEY_1,
    KZS_KEY_2,
    KZS_KEY_3,
    
    KZS_KEY_4,
    KZS_KEY_5,
    KZS_KEY_6,
    KZS_KEY_7,
    KZS_KEY_8,
    
    KZS_KEY_9,
    KZS_KEY_0,
    KZS_KEY_PLUS,
    KZS_KEY_ACCENT,
    KZS_KEY_BACKSPACE,
    
    KZS_KEY_TAB,
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
    KZS_KEY_ARING,
    KZS_KEY_UMLAUT,
    KZS_KEY_ENTER,
    KZS_KEY_LEFT_CONTROL,
    
    KZS_KEY_A,
    KZS_KEY_S,
    KZS_KEY_D,
    KZS_KEY_F,
    KZS_KEY_G,
    
    KZS_KEY_H,
    KZS_KEY_J,
    KZS_KEY_K,
    KZS_KEY_L,
    KZS_KEY_OUML,
    
    KZS_KEY_AUML,
    KZS_KEY_SECTION,
    KZS_KEY_LEFT_SHIFT,
    KZS_KEY_QUOTE,
    KZS_KEY_Z,
    
    KZS_KEY_X,
    KZS_KEY_C,
    KZS_KEY_V,
    KZS_KEY_B,
    KZS_KEY_N,
    
    KZS_KEY_M,
    KZS_KEY_COMMA,
    KZS_KEY_PERIOD,
    KZS_KEY_MINUS,
    KZS_KEY_RIGHT_SHIFT,
    
    KZS_KEY_NUM_MULTIPLY,
    KZS_KEY_LEFT_ALT,
    KZS_KEY_SPACE,
    KZS_KEY_CAPS_LOCK,
    KZS_KEY_F1,
    
    KZS_KEY_F2,
    KZS_KEY_F3,
    KZS_KEY_F4,
    KZS_KEY_F5,
    KZS_KEY_F6,
    
    KZS_KEY_F7,
    KZS_KEY_F8,
    KZS_KEY_F9,
    KZS_KEY_F10,
    KZS_KEY_NUM_LOCK,
    
    KZS_KEY_SCROLL_LOCK,
    KZS_KEY_NUM_7,
    KZS_KEY_NUM_8,
    KZS_KEY_NUM_9,
    KZS_KEY_NUM_MINUS,
    
    KZS_KEY_NUM_4,
    KZS_KEY_NUM_5,
    KZS_KEY_NUM_6,
    KZS_KEY_NUM_PLUS,
    KZS_KEY_NUM_1,
    
    KZS_KEY_NUM_2,
    KZS_KEY_NUM_3,
    KZS_KEY_NUM_0,
    KZS_KEY_NUM_COMMA,
    KZS_KEY_UNKNOWN,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_LESS_THAN,
    KZS_KEY_F11,
    KZS_KEY_F12,
    KZS_KEY_UNKNOWN,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_NUM_ENTER,
    KZS_KEY_RIGHT_CONTROL,
    KZS_KEY_NUM_DIVIDE,
    KZS_KEY_PRINT_SCREEN,
    
    KZS_KEY_RIGHT_ALT,
    KZS_KEY_UNKNOWN,
    KZS_KEY_HOME,
    KZS_KEY_ARROW_UP,
    KZS_KEY_PAGE_UP,
    
    KZS_KEY_ARROW_LEFT,
    KZS_KEY_ARROW_RIGHT,
    KZS_KEY_END,
    KZS_KEY_ARROW_DOWN,
    KZS_KEY_PAGE_DOWN,
    
    KZS_KEY_INSERT,
    KZS_KEY_DELETE,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_BREAK,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    
    KZS_KEY_LEFT_WINDOWS_KEY,
    KZS_KEY_RIGHT_WINDOWS_KEY,
    KZS_KEY_MENU_KEY
};

#else   /* if not defined(ARM_MALI_INPUT) */

#define KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE  128

/* Combined TCC8900 on-board buttons (/dev/event0) and Finnish 105-key
        IBM keyboard connected to ARM Mali via USB (/dev/event3) */
/* Keycodes 2-11 are interpreted as on-board buttons, others are
        keyboard keypresses. */
static enum KzsInputKey kzsInputKeyTranslationTable[KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE] = 
{
    KZS_KEY_UNKNOWN,
    KZS_KEY_ESC,
    KZS_KEY_ESC,          
    KZS_KEY_ENTER,            
    KZS_KEY_ARROW_UP,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_BACKSPACE,         
    KZS_KEY_UNKNOWN,  
    KZS_KEY_ARROW_LEFT,       
    KZS_KEY_ARROW_RIGHT,
    
    KZS_KEY_ARROW_DOWN,         
    KZS_KEY_SPACE,     
    KZS_KEY_PLUS,
    KZS_KEY_ACCENT,
    KZS_KEY_BACKSPACE,
    
    KZS_KEY_TAB,
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
    KZS_KEY_ARING,
    KZS_KEY_UMLAUT,
    KZS_KEY_ENTER,
    KZS_KEY_LEFT_CONTROL,
    
    KZS_KEY_A,
    KZS_KEY_S,
    KZS_KEY_D,
    KZS_KEY_F,
    KZS_KEY_G,
    
    KZS_KEY_H,
    KZS_KEY_J,
    KZS_KEY_K,
    KZS_KEY_L,
    KZS_KEY_OUML,
    
    KZS_KEY_AUML,
    KZS_KEY_SECTION,
    KZS_KEY_LEFT_SHIFT,
    KZS_KEY_QUOTE,
    KZS_KEY_Z,
    
    KZS_KEY_X,
    KZS_KEY_C,
    KZS_KEY_V,
    KZS_KEY_B,
    KZS_KEY_N,
    
    KZS_KEY_M,
    KZS_KEY_COMMA,
    KZS_KEY_PERIOD,
    KZS_KEY_MINUS,
    KZS_KEY_RIGHT_SHIFT,
    
    KZS_KEY_NUM_MULTIPLY,
    KZS_KEY_LEFT_ALT,
    KZS_KEY_SPACE,
    KZS_KEY_CAPS_LOCK,
    KZS_KEY_F1,
    
    KZS_KEY_F2,
    KZS_KEY_F3,
    KZS_KEY_F4,
    KZS_KEY_F5,
    KZS_KEY_F6,
    
    KZS_KEY_F7,
    KZS_KEY_F8,
    KZS_KEY_F9,
    KZS_KEY_F10,
    KZS_KEY_NUM_LOCK,
    
    KZS_KEY_SCROLL_LOCK,
    KZS_KEY_NUM_7,
    KZS_KEY_NUM_8,
    KZS_KEY_NUM_9,
    KZS_KEY_NUM_MINUS,
    
    KZS_KEY_NUM_4,
    KZS_KEY_NUM_5,
    KZS_KEY_NUM_6,
    KZS_KEY_NUM_PLUS,
    KZS_KEY_NUM_1,
    
    KZS_KEY_NUM_2,
    KZS_KEY_NUM_3,
    KZS_KEY_NUM_0,
    KZS_KEY_NUM_COMMA,
    KZS_KEY_UNKNOWN,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_LESS_THAN,
    KZS_KEY_F11,
    KZS_KEY_F12,
    KZS_KEY_UNKNOWN,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_NUM_ENTER,
    KZS_KEY_RIGHT_CONTROL,
    KZS_KEY_NUM_DIVIDE,
    KZS_KEY_PRINT_SCREEN,
    
    KZS_KEY_RIGHT_ALT,
    KZS_KEY_UNKNOWN,
    KZS_KEY_HOME,
    KZS_KEY_ARROW_UP,
    KZS_KEY_PAGE_UP,
    
    KZS_KEY_ARROW_LEFT,
    KZS_KEY_ARROW_RIGHT,
    KZS_KEY_END,
    KZS_KEY_ARROW_DOWN,
    KZS_KEY_PAGE_DOWN,
    
    KZS_KEY_INSERT,
    KZS_KEY_DELETE,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_BREAK,
    
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    
    KZS_KEY_LEFT_WINDOWS_KEY,
    KZS_KEY_RIGHT_WINDOWS_KEY,
    KZS_KEY_MENU_KEY
};

#endif


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
