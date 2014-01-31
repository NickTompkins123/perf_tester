/**
 * \file
 * Input device handling. Keyboards, mice, touch screens, joysticks and similar devices.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZS_INPUT_H
#define KZS_INPUT_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzsPointingDeviceInputData;
struct KzsKeyDeviceInputData;
struct KzsInputEvent;
struct KzsInputEventQueue;
struct KzsInputEventQueueIterator;


/** Enumeration for different types of input events. */
enum KzsInputEventType
{
    KZS_INPUT_EVENT_KEY,                    /**< Key press or release. */
    KZS_INPUT_EVENT_POINTER                 /**< Pointer movement or press action. */
};

/** Enumeration for different states for a pointing device. */
enum KzsInputPointingDeviceState
{
    KZS_POINTING_DEVICE_STATE_DOWN,         /**< Pressed. */
    KZS_POINTING_DEVICE_STATE_UP,           /**< Released. */
    KZS_POINTING_DEVICE_STATE_MOVE,         /**< Pointer moved. Not all platforms support this, for example many touch screens. */
    KZS_POINTING_DEVICE_STATE_DRAG          /**< Pointer dragged. */
};

/** Orientation angles for pointing devices in degrees. */
enum KzsInputPointingDeviceOrientationAngle
{
    KZS_INPUT_POINTING_DEVICE_ORIENTATION_ANGLE_0, /**< 0 degrees. */
    KZS_INPUT_POINTING_DEVICE_ORIENTATION_ANGLE_180 /**< 180 degrees. */
};

/** Enumeration for different states for a key device. */
enum KzsInputKeyDeviceState
{
    KZS_KEY_DEVICE_STATE_DOWN,         /**< Pressed. */
    KZS_KEY_DEVICE_STATE_UP,           /**< Released. */
    KZS_KEY_DEVICE_STATE_REPEAT        /**< Key is being held. */
};


/** Iterator for input event queues. */
struct KzsInputEventQueueIterator
{
    kzInt index;                            /**< Index to current event. */
    const struct KzsInputEventQueue* queue; /**< Queue to iterate in. */
};


#define KZS_POINTING_DEVICE_BUTTON_PRIMARY      (1)         /**< Primary pointer button, e.g. usually mouse left on Windows. */
#define KZS_POINTING_DEVICE_BUTTON_SECONDARY    (1 << 1)    /**< Secondary pointer button, e.g. usually mouse right on Windows. */
#define KZS_POINTING_DEVICE_BUTTON_THIRD        (1 << 2)    /**< Third pointer button, e.g. usually mouse middle on Windows. */
#define KZS_POINTING_DEVICE_BUTTON_ROLL_UP      (1 << 3)    /**< Roll up pointer button. */
#define KZS_POINTING_DEVICE_BUTTON_ROLL_DOWN    (1 << 4)    /**< Roll down pointer button. */

/** Platform independent key enumeration */
enum KzsInputKey
{
        /* Unknown keys are used for unmapped keys. */
    KZS_KEY_UNKNOWN = 0,

        /* Keys used for numeric input */   
    KZS_KEY_0,
    KZS_KEY_1,
    KZS_KEY_2,
    KZS_KEY_3,
    KZS_KEY_4,
    KZS_KEY_5,
    KZS_KEY_6,
    KZS_KEY_7,
    KZS_KEY_8,
    KZS_KEY_9,
    KZS_KEY_COMMA,
    KZS_KEY_PERIOD,
    KZS_KEY_DECIMAL_SEPARATOR,      /**< allows for locale specific separator key mapping */
    KZS_KEY_THOUSANDS_SEPARATOR,    /**< allows for locale specific separator key mapping */

        /* Keys used for text input */
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
    KZS_KEY_Z,

    KZS_KEY_SPACE,
    KZS_KEY_BACKSPACE,
    KZS_KEY_ENTER,
    KZS_KEY_TAB,
    KZS_KEY_ESC,
    
    KZS_KEY_CAPS_LOCK,
    KZS_KEY_NUM_LOCK,
    KZS_KEY_PRINT_SCREEN,
    KZS_KEY_SCROLL_LOCK,
    KZS_KEY_BREAK,

    KZS_KEY_INSERT,
    KZS_KEY_DELETE,
    KZS_KEY_HOME,
    KZS_KEY_END,
    KZS_KEY_PAGE_UP,
    KZS_KEY_PAGE_DOWN,

    KZS_KEY_PLUS,
    KZS_KEY_MINUS,
    KZS_KEY_MULTIPLY,
    KZS_KEY_DIVIDE,

        /* Arrow keys for navigation (8-directional on some devices?) */
    KZS_KEY_ARROW_UP,
    KZS_KEY_ARROW_DOWN,
    KZS_KEY_ARROW_LEFT,
    KZS_KEY_ARROW_RIGHT,
    
        /* Numpad keys on a common IBM keyboard */
    KZS_KEY_NUM_0,
    KZS_KEY_NUM_1,
    KZS_KEY_NUM_2,
    KZS_KEY_NUM_3,
    KZS_KEY_NUM_4,
    KZS_KEY_NUM_5,
    KZS_KEY_NUM_6,
    KZS_KEY_NUM_7,
    KZS_KEY_NUM_8,
    KZS_KEY_NUM_9,
    KZS_KEY_NUM_DIVIDE,
    KZS_KEY_NUM_MULTIPLY,
    KZS_KEY_NUM_PLUS,
    KZS_KEY_NUM_MINUS,
    KZS_KEY_NUM_COMMA,
    KZS_KEY_NUM_ENTER,
    
        /* Function keys on a common IBM keyboard */
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
    KZS_KEY_F11,
    KZS_KEY_F12,

        /* Sound volume control keys */
    KZS_KEY_VOLUME_UP,
    KZS_KEY_VOLUME_DOWN,

        /* Unnamed buttons, usually on phones etc */
    KZS_KEY_SOFTKEY1,  
    KZS_KEY_SOFTKEY2,
    KZS_KEY_SOFTKEY3,
    KZS_KEY_SOFTKEY4,
    
        /* Generic names for modifier keys such as shift, ctrl, alt, Fn, command, super, etc. */
    KZS_KEY_MODIFIER1, 
    KZS_KEY_MODIFIER2,
    KZS_KEY_MODIFIER3,
    
        /* Specific modifier key names on a common IBM keyboard */
    KZS_KEY_RIGHT_ALT,
    KZS_KEY_RIGHT_CONTROL,
    KZS_KEY_RIGHT_SHIFT,
    KZS_KEY_RIGHT_WINDOWS_KEY,
    KZS_KEY_LEFT_ALT,
    KZS_KEY_LEFT_CONTROL,
    KZS_KEY_LEFT_SHIFT,
    KZS_KEY_LEFT_WINDOWS_KEY,
    KZS_KEY_MENU_KEY,
    
        /* Key names for Finnish 105-key IBM keyboard */
    KZS_KEY_SECTION,        /**< left of 1 */
    KZS_KEY_QUOTE,          /**< left of enter, below */
    KZS_KEY_LESS_THAN,      /**< left of Z */
    KZS_KEY_UMLAUT,         /**< left of enter, above */
    KZS_KEY_AUML,           /**< right of OUML */
    KZS_KEY_OUML,           /**< right of L */   
    KZS_KEY_ARING,          /**< right of P */
    KZS_KEY_ACCENT,          /**< left of backspace */

    KZS_KEY_COUNT
};


/** Callback function type for pointing device input handling. */ 
typedef kzsError (*kzsInputEventHandleFunction)(const struct KzsInputEventQueue* queue, void* userData);


/** Creates a new input event queue. */
kzsError kzsInputEventQueueCreate(struct KzsInputEventQueue** queue_out);
/** Frees memory allocated for a input event queue. */
void kzsInputEventQueueDelete(struct KzsInputEventQueue* queue);

/** Set the event handler function. */
void kzsInputEventQueueSetHandleCallback(struct KzsInputEventQueue* queue, kzsInputEventHandleFunction func);
/** Get the event handler function. */
kzsInputEventHandleFunction kzsInputEventQueueGetHandleCallback(const struct KzsInputEventQueue* queue);

/** Get an iterator over an input event queue. */
struct KzsInputEventQueueIterator kzsInputEventQueueGetIterator(const struct KzsInputEventQueue* queue);
/** Internal iteration. */
kzBool kzsInputEventQueueIteratorIterate_private(struct KzsInputEventQueueIterator* it);
/** Iterate an input event queue iterator.
*
* Return true if this value is still valid.
*/
#define kzsInputEventQueueIteratorIterate(it) kzsInputEventQueueIteratorIterate_private(&it)
/** Internal iteration get value. */
struct KzsInputEvent* kzsInputEventQueueIteratorGetValue_private(const struct KzsInputEventQueueIterator* it);
/** Get the current value of an input event queue iterator. */
#define kzsInputEventQueueIteratorGetValue(it) kzsInputEventQueueIteratorGetValue_private(&it)

/** Get nth event from an input event queue. */
struct KzsInputEvent* kzsInputEventQueueGetEvent(struct KzsInputEventQueue* queue, kzUint index);
/** Get the number of events in an input event queue. */
kzUint kzsInputEventQueueGetEventCount(const struct KzsInputEventQueue* queue);

/** Clear all events from an input event queue. */
void kzsInputEventQueueClear(struct KzsInputEventQueue* queue);
/** Reserve a new event from the input event queue.
*
* This may fail as there is a maximum number of events. In this case, it just returns NULL.
*/
struct KzsInputEvent* kzsInputEventQueueReserveEvent(struct KzsInputEventQueue* queue);

/** Adds a new pointing device event into the queue with the input values.
*
* @return True if there was space to add the event, false otherwise.
*/
kzBool kzsInputEventQueueAddPointingDeviceEvent(struct KzsInputEventQueue* queue, kzInt cursorId, kzInt x, kzInt y,
                                                kzFloat pressure, kzU32 buttons, enum KzsInputPointingDeviceState state);
/** Adds a new key device event into the queue with input values.
*
* @return True if there was space to add the event, false otherwise.
*/
kzBool kzsInputEventQueueAddKeyDeviceEvent(struct KzsInputEventQueue* queue, kzUint button,
                                           enum KzsInputKeyDeviceState state);

/** Get the pointer event from an input event. */
struct KzsPointingDeviceInputData* kzsInputEventGetPointingDeviceData(struct KzsInputEvent* event);
/** Set the input event to a pointing device event. */
void kzsInputEventSetAsPointingDevice(struct KzsInputEvent* event);
/** Get the pointer event from an input event. */
struct KzsKeyDeviceInputData* kzsInputEventGetKeyDeviceData(struct KzsInputEvent* event);
/** Set the input event to a pointing device event. */
void kzsInputEventSetAsKeyDevice(struct KzsInputEvent* event);
/** Get the event type. */
enum KzsInputEventType kzsInputEventGetType(const struct KzsInputEvent* event);

/** Fills pointing device input data with input values. */
void kzsInputSetPointingDeviceInputData(struct KzsPointingDeviceInputData* pointingDeviceInputData, kzInt cursorId, 
                                        kzInt x, kzInt y, kzFloat pressure, kzU32 buttons, enum KzsInputPointingDeviceState state);
/** Fills key device input data with input values. */
void kzsInputSetKeyDeviceInputButtonData(struct KzsKeyDeviceInputData* keyDeviceInputData, kzUint button, enum KzsInputKeyDeviceState state);

/** Sets the active pointing device. Cursor id is platform specific. */
void kzsInputSetActivePointingDevice(struct KzsPointingDeviceInputData* pointingDeviceInputData, kzInt cursorId);

/** Gets the x coordinate of the active pointing device. */
kzInt kzsInputGetPointingDeviceX(const struct KzsPointingDeviceInputData* inputData);
/** Gets the y coordinate of the active pointing device. */
kzInt kzsInputGetPointingDeviceY(const struct KzsPointingDeviceInputData* inputData);
/** Gets the pressure of the active pointing device. */
kzFloat kzsInputGetPointingDevicePressure(const struct KzsPointingDeviceInputData* inputData);
/** Gets the radius of the active pointing device. */
kzFloat kzsInputGetPointingDeviceRadius(const struct KzsPointingDeviceInputData* inputData);
/** Gets the bitfield of the buttons of the active pointing device.
 * \param inputData Input data pointer.
 * \return Bitmask of pointing device buttons, defined as KZS_POINTING_DEVICE_BUTTON_ *.
 */
kzU32 kzsInputGetPointingDeviceButtons(const struct KzsPointingDeviceInputData* inputData);
/** Gets the state of the active pointing device. */
enum KzsInputPointingDeviceState kzsInputGetPointingDeviceState(const struct KzsPointingDeviceInputData* inputData);

/** Sets input orientation settings. */
void kzsInputSetOrientationSettings(struct KzsPointingDeviceInputData* inputData, enum KzsInputPointingDeviceOrientationAngle angle,
                                    kzUint areaWidth, kzUint areaHeight);

/** Gets the raw button code of the key device. */
kzUint kzsInputGetKeyDeviceButtonRaw(const struct KzsKeyDeviceInputData* inputData);
/** Gets the translated button of the key device. */
enum KzsInputKey kzsInputGetKeyDeviceButton(const struct KzsKeyDeviceInputData* inputData);

/** Gets the state of the key device. */
enum KzsInputKeyDeviceState kzsInputGetKeyDeviceState(const struct KzsKeyDeviceInputData* inputData);

/** Returns the translated key. */
enum KzsInputKey kzsInputNativeGetTranslatedKey(kzUint button);


#endif
