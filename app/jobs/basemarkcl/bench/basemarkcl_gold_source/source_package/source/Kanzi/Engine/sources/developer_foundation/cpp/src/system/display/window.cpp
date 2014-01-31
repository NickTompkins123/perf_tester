extern "C"
{
#include <system/display/kzs_window.h>
#include <system/display/kzs_surface.h>
}

#include "system/display/window.h"

namespace Kanzi {
namespace System {

class WindowPrivate
{
public:
    WindowPrivate(KzsWindow* window) : window(window) {}
    KzsWindow* window;
};

Window::Window(KzsWindow* window) : 
    d(new WindowPrivate(window))
{
}

Window::~Window()
{
    delete d;
}

void Window::inject(kzInt cursor, kzInt x, kzInt y, kzFloat pressure, kzU32 buttons, PointingDeviceState buttonState)
{
    kzsWindowSetActivePointer(d->window, cursor);

    KzsInputPointingDevice* inputPointingDevice = kzsWindowGetInputPointingDevice(d->window);
    KzsPointingDeviceInputData* pointingDeviceData = kzsInputGetPointingDeviceData(inputPointingDevice);
    kzsInputSetPointingDeviceInputData(pointingDeviceData, cursor, x, y, pressure, buttons, (KzsInputPointingDeviceState)buttonState);

    kzsInputPointingDeviceHandlerFunction pointingDeviceHandler = kzsWindowGetPointingDeviceHandler(d->window);
    pointingDeviceHandler(pointingDeviceData, kzsWindowGetUserData(d->window));
}

#if 0
kzBool Window::create(KanziDisplay* display)
{
    kzBool result = KZ_FALSE;

    KzsWindowProperties windowProperties;
    windowProperties.width = 640;
    windowProperties.height = 480;
    windowProperties.style = KZS_WINDOW_STYLE_RESIZABLE;
    
    if (kzsWindowCreate(&windowProperties, display->object(), &d->window) == KZS_SUCCESS)
    {
        /* Create a drawable surface and bind it to the window. */
        KzsSurfaceProperties surfaceProperties;
        surfaceProperties.antiAliasing = 4;
        surfaceProperties.bitsDepthBuffer = 16;
        surfaceProperties.bitsColorR = KZS_SURFACE_PROPERTY_DONT_CARE;
        surfaceProperties.bitsColorG = KZS_SURFACE_PROPERTY_DONT_CARE;
        surfaceProperties.bitsColorB = KZS_SURFACE_PROPERTY_DONT_CARE;
        surfaceProperties.bitsStencil = KZS_SURFACE_PROPERTY_DONT_CARE;

        if (kzsSurfaceCreate(&surfaceProperties, display->object(), d->window, &d->surface) == KZS_SUCCESS)
        {
            kzsWindowSetSurface(d->window, d->surface);
            result = KZ_TRUE;
        }
        
    }

    return result;
}
#endif

kzBool Window::update()
{
    kzBool result = KZ_FALSE;
    if (kzsWindowGetState(d->window) == KZS_WINDOW_STATE_ACTIVE)
    {
        result = kzsWindowUpdate(d->window) == KZS_SUCCESS;
    }
    return result;
}

KzsWindow* Window::handle() const
{
    return d->window;
}

} // Kanzi::System
} // Kanzi
