#ifndef KANZIWINDOW_H
#define KANZIWINDOW_H

#include "predefinitions.h"

struct KzsWindow;

namespace Kanzi {
namespace System {

class KANZI_API Window
{
public:

    enum PointingDeviceState
    {
        KZS_POINTING_DEVICE_STATE_DOWN,         /**< Pressed. */
        KZS_POINTING_DEVICE_STATE_UP,           /**< Released. */
        KZS_POINTING_DEVICE_STATE_MOVE,         /**< Pointer moved. Not all platforms support this, for example many touch screens. */
        KZS_POINTING_DEVICE_STATE_DRAG          /**< Pointer dragged. */
    };

	// Constructor
	Window(KzsWindow* window);
	
	// Destructor
	~Window();

	/*
	* \brief Injects an input event to window.
	*/
    void inject(kzInt cursor, kzInt x, kzInt y, kzFloat pressure, kzU32 buttons, PointingDeviceState buttonState);

	/*
	* \brief Updates window.
	*/
    kzBool update();

	/*
	* \brief Returns Kanzi window handle.
	*/
    KzsWindow* handle() const;

private:
    class WindowPrivate* d;
};

} // Kanzi::System
} // Kanzi

#endif
