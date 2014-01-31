%{
#include <system/display/kzs_window.h>
#include <system/display/kzs_display.h>
#include <system/display/kzs_surface.h>
%}

checkError(kzsWindowUpdate)

%nodefaultctor KzsWindow;
%nodefaultdtor KzsWindow;

struct KzsWindow {};

checkErrorOut(kzsWindowCreate_temp)
%rename kzsWindowCreate_temp kzsWindowCreate;
%ignore kzsWindowCreate;
%inline %{
struct KzsWindow* kzsWindowCreate_temp(kzUint width, kzUint height, kzsError* out_error) {
    kzsError result;
    struct KzsWindow* window;
    struct KzsSurface* surface;
    struct KzsDisplay* display;
    struct KzsDisplayConfiguration displayConfiguration;
    struct KzsWindowProperties windowProperties;
    struct KzsSurfaceProperties surfaceProperties;
    displayConfiguration.dummy = 0;
            
    windowProperties.width = width;
    windowProperties.height = height;
    windowProperties.style = KZS_WINDOW_STYLE_RESIZABLE;
    
    surfaceProperties.antiAliasing = 4;
    surfaceProperties.bitsColorR = 5;
    surfaceProperties.bitsColorG = 6;
    surfaceProperties.bitsColorB = 5;

    surfaceProperties.bitsDepthBuffer = 16;
    surfaceProperties.bitsStencil = 0;

    result = kzsDisplayCreate(0, &displayConfiguration, &display);
    kzsErrorIf(result) {
        *out_error = result;
        return KZ_NULL;
    }
    
    result = kzsWindowCreate(&windowProperties, display, &window);
    kzsErrorIf(result) {
        *out_error = result;
        return KZ_NULL;
    }
    
    result = kzsSurfaceCreate(&surfaceProperties, display, window, &surface);
    kzsErrorIf(result) {
        *out_error = result;
        return KZ_NULL;
    }

    kzsWindowSetSurface(window, surface);

    *out_error = KZS_SUCCESS;
    return window;
}
%}

%include <system/input/kzs_input.h>
%include <system/display/kzs_window.h>
