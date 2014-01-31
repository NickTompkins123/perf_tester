/**
* \file
* Drawable surface.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_surface.h"

#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/kzs_types.h>
#include <system/display/kzs_display.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_opengl_base.h>

/* Forward declarations. */
struct KzsDisplay;
struct KzsWindow;


struct KzsSurface
{
    struct KzsSurfaceNative*    surfaceNative;  /**< Platform specific data. */
};


kzsError kzsSurfaceCreate(const struct KzsSurfaceProperties* surfaceProperties, const struct KzsDisplay* display,
                          const struct KzsWindow* window, struct KzsSurface** out_surface)
{
    kzsError result;
    struct KzsSurface* surface;

    kzsAssert(display != KZ_NULL);
    kzsAssert(window != KZ_NULL);

    surface = (struct KzsSurface*)kzsMalloc(sizeof(*surface));
    kzsErrorTest(surface != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Unable to allocate memory for surface.");

    result = kzsSurfaceNativeCreate(surfaceProperties, kzsDisplayGetNative(display), kzsWindowGetNative(window), &surface->surfaceNative);
    kzsErrorForward(result);

    *out_surface = surface;
    kzsSuccess();
}

kzsError kzsSurfaceSetActive(const struct KzsSurface* surface, kzBool active)
{
    kzsError result;
    kzsAssert(surface != KZ_NULL);

    result = kzsSurfaceNativeSetActive(surface->surfaceNative, active);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzsSurfaceUpdate(const struct KzsSurface* surface)
{
    kzsError result;
    kzsAssert(surface != KZ_NULL);
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    result = kzsSurfaceNativeUpdate(surface->surfaceNative);
    kzsErrorForward(result);
#endif

    kzsSuccess();
}

kzsError kzsSurfaceDelete(struct KzsSurface* surface)
{
    kzsError result;
    kzsAssert(surface != KZ_NULL);

    result = kzsSurfaceNativeDelete(surface->surfaceNative);
    kzsErrorForward(result);
    kzsFree(surface);

    kzsSuccess();
}

kzsError kzsSurfaceReadPixels(const struct KzsSurface* surface, void* out_pixels)
{
    kzsError result;

    result =  kzsSurfaceNativeReadPixels(surface->surfaceNative, out_pixels);
    kzsErrorForward(result);

    kzsSuccess();
}

struct KzsSurfaceNative* kzsSurfaceGetNativeSurface(const struct KzsSurface* surface)
{
    kzsAssert(surface != KZ_NULL);
    return surface->surfaceNative;
}

void kzsSurfaceGetUsedSurfaceProperties(const struct KzsSurface* surface, struct KzsSurfaceProperties* surfaceProperties)
{
    kzsSurfaceGetUsedSurfaceNativeProperties(surface->surfaceNative, surfaceProperties);
}
