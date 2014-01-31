/**
 * \file
 * Input device handling. Keyboards, mice, touch screens, joysticks and similar devices.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzs_input.h"

#include <system/wrappers/kzs_memory.h>
#include <system/debug/kzs_error.h>
#include <system/kzs_error_codes.h>

#define KZS_INPUT_MAX_CURSORS    5
#define KZS_MAX_EVENTS           64


/** Structure for cursor location and state data. */
struct KzsCursor
{
    kzInt x;                      /**< X coordinate for pointer. */
    kzInt y;                      /**< Y coordinate for pointer. */
    kzFloat pressure;             /**< Pressure information for pointer, if supported by platform. Otherwise value unspecified. */
    kzFloat radius;               /**< Size of the area touched, if supported by platform. Otherwise value unspecified. */

    kzU32 buttons;                          /**< Bitmask of the pressed buttons. */
    enum KzsInputPointingDeviceState state; /**< Pointer state. */
};

/** Struct for cursor data (mouse, touch screen, analog joysticks,...) from system for an input handler to process. */
struct KzsPointingDeviceInputData
{
    kzInt       activePointingDeviceID; /**< Currently active pointing device. */
    struct KzsCursor   cursorData[KZS_INPUT_MAX_CURSORS];   /**< Cursors. Some platforms support more than one. */ /* TODO: Rename to cursors */

    struct  
    {
        enum KzsInputPointingDeviceOrientationAngle angle;
        kzUint areaWidth;
        kzUint areaHeight;
    } orientationInfo;
};

/** Struct to hold button data (keyboard, keypad, buttons, digital joysticks,...) from system for an input handler to process. */
struct KzsKeyDeviceInputData
{
    enum KzsInputKey    translatedButtonCode;   /**< Platform independent button key value. */
    kzUint              rawButtonCode;          /**< Raw value from the OS. */

    enum KzsInputKeyDeviceState  state;  /**< Button state. */
};

/** Structure to contain all input events. */
struct KzsInputEvent
{
    enum KzsInputEventType type;                         /**< Type of the event (which struct to access) */

    union 
    {
        struct KzsPointingDeviceInputData pointingEvent; /**< Pointing event data. */
        struct KzsKeyDeviceInputData keyEvent;           /**< Key event data. */
    } inputData;
};

/* TODO: storing the struct is designed to keep track of all previous events for multi touch to work. Redesign it? */
/** Pointing device structure. Holds input data and the function pointer to the handler. */
struct KzsInputEventQueue
{
    struct KzsInputEvent events[KZS_MAX_EVENTS]; /**< Table of events, no need for allocations. */
    kzUint numEvents;                            /**< Number of events currently allocated from the queue. */
    kzsInputEventHandleFunction callback;        /**< Handler callback for events. */
};


kzsError kzsInputEventQueueCreate(struct KzsInputEventQueue** queue_out)
{
    struct KzsInputEventQueue* queue = (struct KzsInputEventQueue*)kzsMalloc(sizeof(struct KzsInputEventQueue));

    kzsErrorTest(queue != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while trying to allocate memory");

    queue->numEvents = 0;
    queue->callback = NULL;

    *queue_out = queue;
    kzsSuccess();
}
void kzsInputEventQueueDelete(struct KzsInputEventQueue* queue)
{
    kzsFree(queue);
}

void kzsInputEventQueueSetHandleCallback(struct KzsInputEventQueue* queue, kzsInputEventHandleFunction func)
{
    kzsAssert(queue != NULL);

    queue->callback = func;
}

kzsInputEventHandleFunction kzsInputEventQueueGetHandleCallback(const struct KzsInputEventQueue* queue)
{
    kzsAssert(queue != NULL);

    return queue->callback;
}

struct KzsInputEventQueueIterator kzsInputEventQueueGetIterator(const struct KzsInputEventQueue* queue)
{
    struct KzsInputEventQueueIterator it;

    it.index = -1;
    it.queue = queue;

    return it;
}

kzBool kzsInputEventQueueIteratorIterate_private(struct KzsInputEventQueueIterator* it)
{
    kzBool returnValue = KZ_FALSE;
    kzInt index;
    const struct KzsInputEventQueue* queue;

    kzsAssert(it != KZ_NULL);

    index = it->index;
    queue = it->queue;

    kzsAssert(queue != KZ_NULL);

    if(++index < (kzInt)queue->numEvents)
    {
        returnValue = KZ_TRUE;
        it->index = index; 
    }
    return returnValue;
}

struct KzsInputEvent* kzsInputEventQueueIteratorGetValue_private(const struct KzsInputEventQueueIterator* it)
{
    const struct KzsInputEventQueue* queue;

    kzsAssert(it != KZ_NULL);

    queue = it->queue;

    kzsAssert(queue != KZ_NULL);

    return (struct KzsInputEvent*)(queue->events + it->index);
}

struct KzsInputEvent* kzsInputEventQueueGetEvent(struct KzsInputEventQueue* queue, kzUint index)
{
    kzsAssert(queue != NULL);

    return queue->events + index;
}

kzUint kzsInputEventQueueGetEventCount(const struct KzsInputEventQueue* queue)
{
    kzsAssert(queue != NULL);

    return queue->numEvents;
}

void kzsInputEventQueueClear(struct KzsInputEventQueue* queue)
{
    kzsAssert(queue != NULL);

    queue->numEvents = 0;
}

struct KzsInputEvent* kzsInputEventQueueReserveEvent(struct KzsInputEventQueue* queue)
{
    kzUint numEvents = queue->numEvents;
    struct KzsInputEvent* returnEvent = NULL;

    if(numEvents < KZS_MAX_EVENTS)
    {
        returnEvent = queue->events + numEvents;
        queue->numEvents = numEvents + 1;
    }

    return returnEvent;
}

kzBool kzsInputEventQueueAddPointingDeviceEvent(struct KzsInputEventQueue* queue, kzInt cursorId, kzInt x, kzInt y,
                                                kzFloat pressure, kzU32 buttons, enum KzsInputPointingDeviceState state)
{
    kzBool returnValue = KZ_FALSE;
    struct KzsInputEvent* event = kzsInputEventQueueReserveEvent(queue);

    if(event != KZ_NULL)
    {
        event->type = KZS_INPUT_EVENT_POINTER;
        kzsInputSetPointingDeviceInputData(&event->inputData.pointingEvent, cursorId, x, y, pressure, buttons, state);
        returnValue = KZ_TRUE;
    }

    return returnValue;
}

kzBool kzsInputEventQueueAddKeyDeviceEvent(struct KzsInputEventQueue* queue, kzUint button,
                                           enum KzsInputKeyDeviceState state)
{
    kzBool returnValue = KZ_FALSE;
    struct KzsInputEvent* event = kzsInputEventQueueReserveEvent(queue);

    if(event != KZ_NULL)
    {
        event->type = KZS_INPUT_EVENT_KEY;
        kzsInputSetKeyDeviceInputButtonData(&event->inputData.keyEvent, button, state);
        returnValue = KZ_TRUE;
    }

    return returnValue;
}

struct KzsPointingDeviceInputData* kzsInputEventGetPointingDeviceData(struct KzsInputEvent* event)
{
    kzsAssert(event != NULL);

    return &event->inputData.pointingEvent;
}


void kzsInputEventSetAsPointingDevice(struct KzsInputEvent* event)
{
    kzsAssert(event != NULL);

    event->type = KZS_INPUT_EVENT_POINTER;
}

struct KzsKeyDeviceInputData* kzsInputEventGetKeyDeviceData(struct KzsInputEvent* event)
{
    kzsAssert(event != NULL);

    return &event->inputData.keyEvent;
}

void kzsInputEventSetAsKeyDevice(struct KzsInputEvent* event)
{
    kzsAssert(event != NULL);

    event->type = KZS_INPUT_EVENT_KEY;
}

enum KzsInputEventType kzsInputEventGetType(const struct KzsInputEvent* event)
{
    kzsAssert(event != KZ_NULL);

    return event->type;
}

void kzsInputSetPointingDeviceInputData(struct KzsPointingDeviceInputData* pointingDeviceInputData, kzInt cursorId, 
                                        kzInt x, kzInt y, kzFloat pressure, kzU32 buttons, enum KzsInputPointingDeviceState state)
{
    struct KzsCursor *cursor = &pointingDeviceInputData->cursorData[cursorId];

    kzsAssert(pointingDeviceInputData != KZ_NULL);

    /* TODO: Go through the button data and rebuild the bitfield to make it Kanzi specific. */
    cursor->buttons = buttons;
    cursor->x = x;
    cursor->y = y;
    cursor->pressure = pressure;
    cursor->state = state;
    switch(pointingDeviceInputData->orientationInfo.angle)
    {
    case KZS_INPUT_POINTING_DEVICE_ORIENTATION_ANGLE_0: break;
    case KZS_INPUT_POINTING_DEVICE_ORIENTATION_ANGLE_180:
        {
            cursor->x = (kzInt)pointingDeviceInputData->orientationInfo.areaWidth - cursor->x;
            cursor->y = (kzInt)pointingDeviceInputData->orientationInfo.areaHeight - cursor->y;
            break;
        }
    default:
        {
            kzsAssertText(KZ_FALSE, "Invalid orientation angle given for input");
        }
    }
}

void kzsInputSetKeyDeviceInputButtonData(struct KzsKeyDeviceInputData* keyDeviceInputData, kzUint button, enum KzsInputKeyDeviceState state)
{
    keyDeviceInputData->rawButtonCode = button;
    keyDeviceInputData->translatedButtonCode = kzsInputNativeGetTranslatedKey(button);
    keyDeviceInputData->state = state;
}

void kzsInputSetActivePointingDevice(struct KzsPointingDeviceInputData* pointingDeviceInputData, kzInt cursorId)
{
    pointingDeviceInputData->activePointingDeviceID = cursorId;
}

kzInt kzsInputGetPointingDeviceX(const struct KzsPointingDeviceInputData* inputData)
{
    kzsAssert(inputData != KZ_NULL);
    return inputData->cursorData[inputData->activePointingDeviceID].x;
}

kzInt kzsInputGetPointingDeviceY(const struct KzsPointingDeviceInputData* inputData)
{
    kzsAssert(inputData != KZ_NULL);
    return inputData->cursorData[inputData->activePointingDeviceID].y;
}

void kzsInputSetOrientationSettings(struct KzsPointingDeviceInputData* inputData, enum KzsInputPointingDeviceOrientationAngle angle,
                                    kzUint areaWidth, kzUint areaHeight)
{
    inputData->orientationInfo.angle = angle;
    inputData->orientationInfo.areaWidth = areaWidth;
    inputData->orientationInfo.areaHeight = areaHeight;
}

kzFloat kzsInputGetPointingDevicePressure(const struct KzsPointingDeviceInputData* inputData)
{
    kzsAssert(inputData != KZ_NULL);
    return inputData->cursorData[inputData->activePointingDeviceID].pressure;
}

kzFloat kzsInputGetPointingDeviceRadius(const struct KzsPointingDeviceInputData* inputData)
{
    kzsAssert(inputData != KZ_NULL);
    return inputData->cursorData[inputData->activePointingDeviceID].radius;
}

kzU32 kzsInputGetPointingDeviceButtons(const struct KzsPointingDeviceInputData* inputData)
{
    kzsAssert(inputData != KZ_NULL);
    return inputData->cursorData[inputData->activePointingDeviceID].buttons;
}

enum KzsInputPointingDeviceState kzsInputGetPointingDeviceState(const struct KzsPointingDeviceInputData* inputData)
{
    kzsAssert(inputData != KZ_NULL);
    return inputData->cursorData[inputData->activePointingDeviceID].state;
}

kzUint kzsInputGetKeyDeviceButtonRaw(const struct KzsKeyDeviceInputData* inputData)
{
    kzsAssert(inputData != KZ_NULL);
    return inputData->rawButtonCode;
}

enum KzsInputKey kzsInputGetKeyDeviceButton(const struct KzsKeyDeviceInputData* inputData)
{
    kzsAssert(inputData != KZ_NULL);
    return inputData->translatedButtonCode;
}

enum KzsInputKeyDeviceState kzsInputGetKeyDeviceState(const struct KzsKeyDeviceInputData* inputData)
{
    kzsAssert(inputData != KZ_NULL);
    return inputData->state;
}
