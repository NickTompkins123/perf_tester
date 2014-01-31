/**
* \file
* Kanzi input, win32 specific implementation
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_input_native.h"

#include <system/input/kzs_input.h>
#include <system/kzs_error_codes.h>
#include <system/display/kzs_window.h>
#include <system/display/kzs_window_native.h>

#include <windows.h>
#include <WindowsX.h>


#define KZS_INPUT_NATIVE_KEY_TRANSLATION_TABLE_SIZE  191

/* win32 specific translation table */
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
    KZS_KEY_BACKSPACE,      /* 8 */
    KZS_KEY_TAB,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_ENTER,          /* 13 */
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_MODIFIER1,      /* shift */
    KZS_KEY_MODIFIER2,      /* ctrl, alt gr */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_ESC,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_SPACE,          /* 32 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_ARROW_LEFT,
    KZS_KEY_ARROW_UP,
    KZS_KEY_ARROW_RIGHT,

    KZS_KEY_ARROW_DOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_0,  /* 48 */
    KZS_KEY_1,

    KZS_KEY_2,
    KZS_KEY_3,
    KZS_KEY_4,
    KZS_KEY_5,
    KZS_KEY_6,

    KZS_KEY_7,
    KZS_KEY_8,
    KZS_KEY_9,  /* 57 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_A,
    KZS_KEY_B,
    KZS_KEY_C,
    KZS_KEY_D,
    KZS_KEY_E,

    KZS_KEY_F,
    KZS_KEY_G,
    KZS_KEY_H,
    KZS_KEY_I,
    KZS_KEY_J,

    KZS_KEY_K,
    KZS_KEY_L,
    KZS_KEY_M,
    KZS_KEY_N,
    KZS_KEY_O,

    KZS_KEY_P,
    KZS_KEY_Q,
    KZS_KEY_R,
    KZS_KEY_S,
    KZS_KEY_T,

    KZS_KEY_U,
    KZS_KEY_V,
    KZS_KEY_W,
    KZS_KEY_X,
    KZS_KEY_Y,

    KZS_KEY_Z,   /* 90 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, /* 100 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,

    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN,
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, /* 110 */
    KZS_KEY_UNKNOWN,
    KZS_KEY_F1,
    KZS_KEY_F2,
    KZS_KEY_F3,

    KZS_KEY_F4,
    KZS_KEY_F5,
    KZS_KEY_F6,
    KZS_KEY_F7,
    KZS_KEY_F8,

    KZS_KEY_F9, /* 120 */
    KZS_KEY_F10,
    KZS_KEY_F11,
    KZS_KEY_F12,
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, /* 130 */
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, /* 140 */
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, /* 150 */
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, /* 160 */
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, /* 170 */
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, /* 180 */
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 

    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_UNKNOWN, 
    KZS_KEY_COMMA, 
    KZS_KEY_MINUS,

    KZS_KEY_PERIOD /* 190 */
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

static kzU32 kzsPointingDeviceHandlerGetButtonsFromWParam_internal(WPARAM wparam)
{
    kzU32 bitmask = 0;

    if((wparam & 1) != 0)
    {
        bitmask |= KZS_POINTING_DEVICE_BUTTON_PRIMARY;
    }
    else if((wparam & 2) != 0)
    {
        bitmask |= KZS_POINTING_DEVICE_BUTTON_SECONDARY;
    }
    else if((wparam & 16) != 0)
    {
        bitmask |= KZS_POINTING_DEVICE_BUTTON_THIRD;
    }

    return bitmask;
}

kzsError kzsInputNativeHandleMessage(struct KzsWindow* window, HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam, kzBool* out_handled)
{
    kzBool handled = KZ_TRUE;

    switch(message)
    {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        {
            /* TODO: format mouse pointer and other data into the struct */
            kzU32 mouseButtons = kzsPointingDeviceHandlerGetButtonsFromWParam_internal(wparam);
            struct KzsPointingDeviceInputData* inputData;
            
            inputData = kzsWindowSetActivePointer(window, 0);
            if(inputData != KZ_NULL)
            {
                kzsInputSetPointingDeviceInputData(inputData, 0, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0.0f,
                                                   mouseButtons, KZS_POINTING_DEVICE_STATE_DOWN);
            }
            
            SetCapture(hWnd);   /* Set capture on, to get drag movement outside the window. */

            break;
        }

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            /* TODO: format mouse pointer and other data into the struct */
            kzU32 mouseButtons = kzsPointingDeviceHandlerGetButtonsFromWParam_internal(wparam);
            struct KzsPointingDeviceInputData* inputData;
            
            inputData = kzsWindowSetActivePointer(window, 0);
            if(inputData != KZ_NULL)
            {
                kzsInputSetPointingDeviceInputData(inputData, 0, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0.0f,
                                                   mouseButtons, KZS_POINTING_DEVICE_STATE_UP);
            }
            
            ReleaseCapture();   /* Release mouse capture after click/drag has ended. */

            break;
        }

        case WM_MOUSEWHEEL:
        {
            int wheelDelta = GET_WHEEL_DELTA_WPARAM(wparam);
            struct KzsPointingDeviceInputData* inputData;
            kzU32 mouseButtons = 0;
            
            if(wheelDelta > 0)
            {
                mouseButtons |= KZS_POINTING_DEVICE_BUTTON_ROLL_UP;
            }
            else if(wheelDelta < 0)
            {
                mouseButtons |= KZS_POINTING_DEVICE_BUTTON_ROLL_DOWN;
            }

            inputData = kzsWindowSetActivePointer(window, 0);
            if(inputData != KZ_NULL)
            {
                kzsInputSetPointingDeviceInputData(inputData, 0, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0.0f,
                                                   mouseButtons, KZS_POINTING_DEVICE_STATE_DOWN);
            }

            break;
        }

        case WM_MOUSEMOVE:
        {
            /* TODO: format mouse pointer and other data into the struct */
            enum KzsInputPointingDeviceState state = KZS_POINTING_DEVICE_STATE_MOVE;
            struct KzsPointingDeviceInputData* inputData;

            if(wparam > 0)
            {
                state = KZS_POINTING_DEVICE_STATE_DRAG;
            }
            
            inputData = kzsWindowSetActivePointer(window, 0);
            if(inputData != KZ_NULL)
            {
                kzsInputSetPointingDeviceInputData(inputData, 0, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0.0f,
                                                   wparam, state);
            }

            /* for tracking the mouse leaving a window */
            {
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hWnd;
                tme.dwHoverTime = 0;
                TrackMouseEvent(&tme);
            }
            break;
        }

        case WM_MOUSELEAVE:
        {
            /* The mouse has left the window.. don't care because the mouse is still tracked by capturing it earlier. */
            handled = KZ_FALSE;
            break;
        }
        /* Keyboard input. */
        case WM_KEYDOWN:
        {
            /* TODO: format keyboard data into the struct (handle virtual keys and real keys) */
            struct KzsWindowNative* windowNative = kzsWindowGetNative(window);
            struct KzsInputEventQueue* queue;
            kzUint keyCode = (kzUint)wparam;

            kzsWindowNativeSetKeyState(windowNative, keyCode, KZS_KEY_DEVICE_STATE_DOWN);

            queue = kzsWindowGetInputEventQueue(window);

            kzsAssert(queue != KZ_NULL);

            kzsInputEventQueueAddKeyDeviceEvent(queue, keyCode, KZS_KEY_DEVICE_STATE_DOWN);

            break;
        }

        case WM_KEYUP:
        {
            /* TODO: format keyboard data into the struct (handle virtual keys and real keys) */
            struct KzsWindowNative* windowNative = kzsWindowGetNative(window);
            struct KzsInputEventQueue* queue;
            kzUint keyCode = (kzUint)wparam;

            kzsWindowNativeSetKeyState(windowNative, keyCode, KZS_KEY_DEVICE_STATE_UP);

            queue = kzsWindowGetInputEventQueue(window);

            kzsAssert(queue != KZ_NULL);

            kzsInputEventQueueAddKeyDeviceEvent(queue, keyCode, KZS_KEY_DEVICE_STATE_UP);

            break;
        }
        default:
        {
            handled = KZ_FALSE;
            break;
        }
    }
    *out_handled = handled;
    kzsSuccess();
}
