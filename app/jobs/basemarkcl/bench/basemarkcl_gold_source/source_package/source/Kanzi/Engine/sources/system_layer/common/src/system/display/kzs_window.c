/**
* \file
* Window handling.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_window.h"

#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/display/kzs_surface.h>


/* TODO: create a more generic KzsEventType enumeration instead of KzsWindowEventType? */

struct KzsWindow
{
    /* Common */
    enum KzsWindowState        state;      /**< Current state of the window. */
    struct KzsWindowProperties properties; /**< Properties of the window. */

    /* Input devices. */
    struct KzsInputEventQueue* queue;   /**< Pointing devices such as mice and touch screens. */

    kzsWindowEventHandlerFunction   eventHandler;

    void* userData; /**< User data for accessing from callbacks. */

    enum KzsWindowOrientationAngle angle; /**< Orientation angle. */

    /* System specific window implementation. */
    struct KzsWindowNative*     windowNative;   /**< Platform specific data. */

    struct KzsSurface*          surface;    /**< The surface to draw on. Can also be KZ_NULL if no surface is bound. */
};

kzsError kzsWindowCreateFromNative(struct KzsWindowNative* windowNative, const struct KzsWindowProperties* windowProperties,
                                   struct KzsWindow** out_window)
{
    kzsError result;
    struct KzsWindow* window;

    kzsAssert(windowNative != KZ_NULL);
    kzsAssert(windowProperties != KZ_NULL);

    window = (struct KzsWindow*)kzsMalloc(sizeof(*window));
    kzsErrorTest(window != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while trying to allocate memory");

    result = kzsInputEventQueueCreate(&window->queue);
    kzsErrorForward(result);

    /* Copy properties */
    window->properties = *windowProperties;

    window->surface = KZ_NULL;
    window->angle = KZS_WINDOW_ORIENTATION_ANGLE_0;

    result = kzsWindowNativeInitialize(windowNative, window);
    kzsErrorForward(result);

    window->windowNative = windowNative;
    window->state = KZS_WINDOW_STATE_ACTIVE;

    *out_window = window;
    kzsSuccess();
}

kzsError kzsWindowCreate(const struct KzsWindowProperties* windowPropertiesRequested, struct KzsDisplay* display, struct KzsWindow** out_window)
{
    kzsError result;
    struct KzsWindow* window;
    struct KzsWindowNative* windowNative;
    struct KzsWindowProperties windowProperties;

    kzsAssert(windowPropertiesRequested != KZ_NULL);

    result = kzsWindowNativeCreate(windowPropertiesRequested, display, &windowProperties, &windowNative);
    kzsErrorForward(result);

    result = kzsWindowCreateFromNative(windowNative, &windowProperties, &window);
    kzsErrorForward(result);

    *out_window = window;
    kzsSuccess();
}

kzsError kzsWindowCreateEmpty(const struct KzsWindowProperties* windowProperties, struct KzsWindow** out_window)
{
    kzsError result;
    struct KzsWindow* window;
    struct KzsWindowNative* windowNative;

    kzsAssert(windowProperties != KZ_NULL);

    result = kzsWindowNativeCreateEmpty(&windowNative);
    kzsErrorForward(result);

    window = (struct KzsWindow*)kzsMalloc(sizeof(*window));
    kzsErrorTest(window != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while trying to allocate memory");

    window->properties = *windowProperties;
    window->surface = KZ_NULL;
    window->windowNative = windowNative;
    window->angle = KZS_WINDOW_ORIENTATION_ANGLE_0;

    window->state = KZS_WINDOW_STATE_ACTIVE;

    *out_window = window;
    kzsSuccess();
}

kzsError kzsWindowDelete(struct KzsWindow* window)
{
    kzsError result;
    kzsAssert(window != KZ_NULL);

    window->state = KZS_WINDOW_STATE_UNINITIALIZED;

    result = kzsWindowNativeDelete(window->windowNative);
    kzsErrorForward(result);

    kzsInputEventQueueDelete(window->queue);
    kzsFree(window);

    kzsSuccess();
}

void kzsWindowSetSurface(struct KzsWindow* window, struct KzsSurface* surface)
{
    kzsAssert(window != KZ_NULL);
    window->surface = surface;
}

struct KzsSurface* kzsWindowGetSurface(const struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    return window->surface;
}

void kzsWindowClosed(struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    window->state = KZS_WINDOW_STATE_CLOSED;
}

struct KzsWindowNative* kzsWindowGetNative(const struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    return window->windowNative;
}

kzsError kzsWindowUpdate(const struct KzsWindow* window)
{
    kzsError result;

    kzsAssert(window != KZ_NULL);

    /* Update window surface, if one exists. */
    if(window->surface != KZ_NULL)
    {
        result = kzsSurfaceUpdate(window->surface);
        kzsErrorForward(result);
    }

    result = kzsWindowNativeUpdate(window);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzsWindowSetActive(const struct KzsWindow* window, kzBool active)
{
    kzsError result;

    kzsAssert(window != KZ_NULL);

    if (window->surface != KZ_NULL)
    {
        result = kzsSurfaceSetActive(window->surface, active);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzsWindowResize(const struct KzsWindow* window, kzUint width, kzUint height)
{
    kzsError result;

    kzsAssert(window != KZ_NULL);
    
    result = kzsWindowNativeResize(window->windowNative, width, height);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzsWindowSetPositionAbsolute(const struct KzsWindow* window, kzUint x, kzUint y)
{
    kzsError result;

    kzsAssert(window != KZ_NULL);

    result = kzsWindowNativeSetPositionAbsolute(window->windowNative, x, y);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzsWindowSetTitle(const struct KzsWindow* window, kzString titleText)
{
    kzsError result;

    kzsAssert(window != KZ_NULL);

    result = kzsWindowNativeSetTitle(window->windowNative, titleText);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzsInputEventQueue* kzsWindowGetInputEventQueue(const struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    return window->queue;
}

void kzsWindowSetInputEventHandler(const struct KzsWindow* window, kzsInputEventHandleFunction handler)
{
    kzsAssert(window != KZ_NULL);
    kzsInputEventQueueSetHandleCallback(window->queue, handler);
}

void kzsWindowSetEventHandler(struct KzsWindow* window, kzsWindowEventHandlerFunction handler)
{
    kzsAssert(window != KZ_NULL);
    window->eventHandler = handler;
}

void kzsWindowSetUserData(struct KzsWindow* window, void* userData)
{
    kzsAssert(window != KZ_NULL);
    window->userData = userData;
}

void* kzsWindowGetUserData(const struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    return(window->userData);
}

enum KzsWindowState kzsWindowGetState(const struct KzsWindow* window)
{
    enum KzsWindowState state;

    if(window != KZ_NULL)
    {
        state = window->state;
    }
    else
    {
        state = KZS_WINDOW_STATE_UNINITIALIZED;
    }

    return state;
}

void kzsWindowSetState(struct KzsWindow* window, enum KzsWindowState state)
{
    kzsAssert(window != KZ_NULL);

    window->state = state;
}

kzsInputEventHandleFunction kzsWindowGetInputEventHandler(const struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    return kzsInputEventQueueGetHandleCallback(window->queue);
}

kzsWindowEventHandlerFunction kzsWindowGetEventHandler(const struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    return window->eventHandler;
}

/** TODO: refactor this and everything related to it once this works. */
struct KzsPointingDeviceInputData* kzsWindowSetActivePointer(const struct KzsWindow* window, kzInt cursorId)
{
    struct KzsInputEventQueue* queue;
    struct KzsInputEvent* inputEvent;
    struct KzsPointingDeviceInputData* pointingDeviceInputData = KZ_NULL;

    kzsAssert(window != KZ_NULL);

    queue = window->queue;

    kzsAssert(queue != KZ_NULL);

    inputEvent = kzsInputEventQueueReserveEvent(queue);
    if(inputEvent != KZ_NULL)
    {
        kzsInputEventSetAsPointingDevice(inputEvent);
        pointingDeviceInputData = kzsInputEventGetPointingDeviceData(inputEvent);

        kzsAssert(pointingDeviceInputData != KZ_NULL);

        switch(window->angle)
        {
            case KZS_WINDOW_ORIENTATION_ANGLE_0:
            {
                kzsInputSetOrientationSettings(pointingDeviceInputData, KZS_INPUT_POINTING_DEVICE_ORIENTATION_ANGLE_0, kzsWindowGetWidth(window),
                    kzsWindowGetHeight(window));
                break;
            }
            case KZS_WINDOW_ORIENTATION_ANGLE_180:
            {
                kzsInputSetOrientationSettings(pointingDeviceInputData, KZS_INPUT_POINTING_DEVICE_ORIENTATION_ANGLE_180, kzsWindowGetWidth(window),
                    kzsWindowGetHeight(window));
                break;
            }
            default:
            {
                kzsAssertText(KZ_FALSE, "Invalid orientation angle given for window");
            }
        }

        kzsInputSetActivePointingDevice(pointingDeviceInputData, cursorId);
    }

    return pointingDeviceInputData;
}

kzUint kzsWindowGetWidth(const struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    return window->properties.width;
}

void kzsWindowSetWidth_private(struct KzsWindow* window, kzUint width)
{
    kzsAssert(window != KZ_NULL);
    window->properties.width = width;
}

kzUint kzsWindowGetHeight(const struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    return window->properties.height;
}

enum KzsWindowStyle kzsWindowGetStyle(const struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    return window->properties.style;
}

void kzsWindowSetOrientationAngle(struct KzsWindow* window, enum KzsWindowOrientationAngle angle)
{
    window->angle = angle;
}

enum KzsWindowOrientationAngle kzsWindowGetOrientationAngle(const struct KzsWindow* window)
{
    return window->angle;
}

void kzsWindowSetHeight_private(struct KzsWindow* window, kzUint height)
{
    kzsAssert(window != KZ_NULL);
    window->properties.height = height;
}

kzBool kzsWindowIsClosed(const struct KzsWindow* window)
{
    kzsAssert(window != KZ_NULL);
    return (window->state == KZS_WINDOW_STATE_CLOSED) ? KZ_TRUE : KZ_FALSE;
}

kzsError kzsWindowReadPixels(const struct KzsWindow* window, void* out_pixels)
{
    kzsError result;

    kzsAssert(window != KZ_NULL);

    result = kzsSurfaceReadPixels(window->surface, out_pixels);
    kzsErrorForward(result);

    kzsSuccess();
}
