/**
* \file
* Window handling.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_WINDOW_H
#define KZS_WINDOW_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>


/* Forward declarations. */
struct KzsDisplay;
struct KzsSurface;
struct KzsPointingDeviceInputData;
struct KzsKeyDeviceInputData;
struct KzsInputEvent;
struct KzsWindowNative;


/** Kanzi window structure. */
struct KzsWindow;


/** Enumeration for different window states. */
enum KzsWindowState
{
    KZS_WINDOW_STATE_UNINITIALIZED,             /**< Window has not yet been initialized or was destroyed. */
    KZS_WINDOW_STATE_ACTIVE,                    /**< Window is created, initialized and ready to use. */
    KZS_WINDOW_STATE_INACTIVE,                  /**< Window can be used but it might not be visible. */
    KZS_WINDOW_STATE_CLOSED                     /**< Window has been closed but it's resources have not been freed yet. */
};

/** List of window orientation angles. */
enum KzsWindowOrientationAngle
{
    KZS_WINDOW_ORIENTATION_ANGLE_0, /**< 0 degrees. */
    KZS_WINDOW_ORIENTATION_ANGLE_180 /**< 180 degrees. */
};

/** Enumeration for different event types. */
enum KzsWindowEventType
{
    KZS_WINDOW_EVENT_WINDOW_RESIZED,   /**< Window has been resized. */
    KZS_WINDOW_EVENT_FOCUS_LOST,       /**< Window has lost input focus but it may still be completely or partially visible. */
    KZS_WINDOW_EVENT_FOCUS_GAINED,     /**< Window has gained input focus and is fully visible. */
    KZS_WINDOW_EVENT_HIDDEN,           /**< Window has been minimized or completely hidden and/or a power saver or screen saver is enabled. Rendering should be disabled. */
    KZS_WINDOW_EVENT_RESOURCES_LOST    /**< Window has lost the resources it has used. Application may need to be restarted or reinitialized. */
};

/** Enumeration for different window styles. */
enum KzsWindowStyle
{
    KZS_WINDOW_STYLE_NORMAL,                    /**< A window that cannot be resized. */
    KZS_WINDOW_STYLE_RESIZABLE,                 /**< A window that can be resized. */
    KZS_WINDOW_STYLE_FULL_SCREEN,               /**< Window occupies the whole screen. */
    KZS_WINDOW_STYLE_STUDIO_PREVIEW             /**< Docked window with external message handling. Should only be used by Kanzi Studio. */
};

/** Properties for a window. */
struct KzsWindowProperties
{
    kzUint width;                   /**< Width of the window. Use 0 to use the maximum width available on the display. */
    kzUint height;                  /**< Height of the window. Use 0 to use the maximum height available on the display. */
    enum KzsWindowStyle style;      /**< Style properties of the window. */
};

/** Callback function type for event handling. */ 
typedef kzsError (*kzsWindowEventHandlerFunction)(struct KzsWindow* window, enum KzsWindowEventType type, void* userData);


/** Creates a window with a previously created native window. Takes ownership of the native window. */
kzsError kzsWindowCreateFromNative(struct KzsWindowNative* windowNative, const struct KzsWindowProperties* windowProperties, 
                                   struct KzsWindow** out_window);
/** Creates a window and returns it. */
kzsError kzsWindowCreate(const struct KzsWindowProperties* windowProperties, struct KzsDisplay* display, struct KzsWindow** out_window);
/** Creates a window and returns it. */
kzsError kzsWindowCreateEmpty(const struct KzsWindowProperties* windowProperties, struct KzsWindow** out_window);
/** Creates a native window. */
kzsError kzsWindowNativeCreate(const struct KzsWindowProperties* windowPropertiesRequested, struct KzsDisplay* display, struct KzsWindowProperties* out_windowProperties, struct KzsWindowNative** out_windowNative);
/** Creates an empty native window. Used in special cases where the native window cannot be initialized at once. */
kzsError kzsWindowNativeCreateEmpty(struct KzsWindowNative** out_windowNative);

/** Deletes a window. */
kzsError kzsWindowDelete(struct KzsWindow* window);

/** Initializes window. Called after creating window and native window. */
kzsError kzsWindowNativeInitialize(struct KzsWindowNative* windowNative, struct KzsWindow* window);
/** Deletes a native window. */
kzsError kzsWindowNativeDelete(struct KzsWindowNative* windowNative);

/** Binds a surface to a window. */
void kzsWindowSetSurface(struct KzsWindow* window, struct KzsSurface* surface);

/** Gets current surface from window. */
struct KzsSurface* kzsWindowGetSurface(const struct KzsWindow* window);

/** Called when the window has been closed outside the application and the engine. */
void kzsWindowClosed(struct KzsWindow* window);

/** Update contents of a window. */
kzsError kzsWindowUpdate(const struct KzsWindow* window);
/** Update contents of a window. Native implementation. */
kzsError kzsWindowNativeUpdate(const struct KzsWindow* window);

/** Set window as active window. */
kzsError kzsWindowSetActive(const struct KzsWindow* window, kzBool active);

/** Resizes the window. */
kzsError kzsWindowResize(const struct KzsWindow* window, kzUint width, kzUint height);

/** Resizes the window. Native implementation. */
kzsError kzsWindowNativeResize(const struct KzsWindowNative* windowNative, kzUint width, kzUint height);

/** Sets the absolute position the window, if supported by the platform. */
kzsError kzsWindowSetPositionAbsolute(const struct KzsWindow* window, kzUint x, kzUint y);

/** Sets the absolute position the window, if supported by the platform. Native implementation. */
kzsError kzsWindowNativeSetPositionAbsolute(const struct KzsWindowNative* windowNative, kzUint x, kzUint y);

/** Sets the title bar string for the window, if supported by the platform. */
kzsError kzsWindowSetTitle(const struct KzsWindow* window, kzString titleText);

/** Sets the title bar string for the window, if supported by the platform. Native implementation. */
kzsError kzsWindowNativeSetTitle(const struct KzsWindowNative* windowNative, kzString titleText);

/** Set function pointer for handling of input devices. */
void kzsWindowSetInputEventHandler(const struct KzsWindow* window, kzsInputEventHandleFunction handler);

/** Set function pointer for handling of window events. */
void kzsWindowSetEventHandler(struct KzsWindow* window, kzsWindowEventHandlerFunction handler);

/** Sets user data for window. User data can then be accessed from the callback functions. */
void kzsWindowSetUserData(struct KzsWindow* window, void* userData);
/** Gets user data for window. */
void* kzsWindowGetUserData(const struct KzsWindow* window);

/** Set user's custom pointer. */
void kzsWindowSetInputUserData(struct KzsWindow* window, void* data);

/** Store the input data read from the keyboard device. */
void kzsWindowSetButtonData(const struct KzsWindow* window, kzUint button, enum KzsInputKeyDeviceState state);

/** TODO: refactor this and everything related to it once this works. */
/** Sets the active pointing device id. Only one in non-multi touch systems. */
struct KzsPointingDeviceInputData* kzsWindowSetActivePointer(const struct KzsWindow* window, kzInt cursorId);

/** Returns the input pointing device. */
struct KzsInputEventQueue* kzsWindowGetInputEventQueue(const struct KzsWindow* window);

/** Returns the function pointer for pointing devices handling. */
kzsInputEventHandleFunction kzsWindowGetInputEventHandler(const struct KzsWindow* window);

/** Returns the function pointer for window event handling. */
kzsWindowEventHandlerFunction kzsWindowGetEventHandler(const struct KzsWindow* window);

/** Returns the current state of a window. */
enum KzsWindowState kzsWindowGetState(const struct KzsWindow* window);

/** Sets a new state for a window. */
void kzsWindowSetState(struct KzsWindow* window, enum KzsWindowState state);

/** Sets orientation angle for window. */
void kzsWindowSetOrientationAngle(struct KzsWindow* window, enum KzsWindowOrientationAngle angle);
/** Gets orientation angle from window. */
enum KzsWindowOrientationAngle kzsWindowGetOrientationAngle(const struct KzsWindow* window);

/** Gets the native window. */
struct KzsWindowNative* kzsWindowGetNative(const struct KzsWindow* window);

/** Gets the width of the window. */
kzUint kzsWindowGetWidth(const struct KzsWindow* window);
/** Gets the height of the window. */
kzUint kzsWindowGetHeight(const struct KzsWindow* window);
/** Gets the style of the window. */
enum KzsWindowStyle kzsWindowGetStyle(const struct KzsWindow* window);

/** Sets the width information of the window. Does not resize the actual window, only updates the internal information. */
void kzsWindowSetWidth_private(struct KzsWindow* window, kzUint width);
/** Sets the height information of the window. Does not resize the actual window, only updates the internal information. */
void kzsWindowSetHeight_private(struct KzsWindow* window, kzUint height);

/** Check if the window has been closed and needs to be cleaned up. */
kzBool kzsWindowIsClosed(const struct KzsWindow* window);

/** Reads pixel data from bound surface to out_pixels. out_pixels must be preallocated to correct size.*/
kzsError kzsWindowReadPixels(const struct KzsWindow* window, void* out_pixels);


#endif
