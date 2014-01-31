/**
 * \file
 * Application framework windows handling.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kza_window.h"

#include "kza_application.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/collection/kzc_dynamic_array.h>

#include <system/display/kzs_display.h>
#include <system/display/kzs_window.h>
#include <system/display/kzs_surface.h>


kzsError kzaWindowCreate(struct KzaApplication* application, struct KzsWindowProperties* windowProperties, const struct KzsSurfaceProperties* surfaceProperties, struct KzsWindow** out_window)
{
    kzsError result;
    struct KzsWindow* window;
    KzaApplicationWindowCreatorFunction windowCreator;
    struct KzcDynamicArray* displays = kzaApplicationGetDisplayArray(application);
    struct KzsDisplay* display = kzcDynamicArrayGet(displays, 0); /* TODO: Default display used for now. Add support for using others. */
  
    /* Create a window. */
    windowCreator = kzaApplicationGetWindowCreator(application);
    if (windowCreator == KZ_NULL)
    {
        result = kzsWindowCreate(windowProperties, display, &window);
        kzsErrorForward(result);
    }
    else
    {
        windowCreator(application, windowProperties, display, &window);
    }

    /* Create a drawable surface and bind it to the window. */
    {
        struct KzsSurface* surface;
        struct KzcDynamicArray* surfaces = kzaApplicationGetSurfaceArray(application);
        
        result = kzsSurfaceCreate(surfaceProperties, display, window, &surface);
        kzsErrorForward(result);

        kzsWindowSetSurface(window, surface);

        result = kzcDynamicArrayAdd(surfaces, surface);
        kzsErrorForward(result);
    }

    /* Use default handlers. */
    kzsWindowSetInputEventHandler(window, kzaApplicationInputEventHandler);
    kzsWindowSetEventHandler(window, kzaApplicationWindowEventHandler);

    /* Store KzaApplication struct pointer into the handlers. */
    kzsWindowSetUserData(window, application);

    /* Store window in the dynamic array. */
    {
        struct KzcDynamicArray* windowArray = kzaApplicationGetWindowArray_private(application);

        result = kzcDynamicArrayAdd(windowArray, window);
        kzsErrorForward(result);
    }

    *out_window = window;
    kzsSuccess();
}

kzsError kzaWindowCreateEmpty(const struct KzaApplication* application, const struct KzsWindowProperties* windowProperties, struct KzsWindow** out_window)
{
    kzsError result;
    struct KzsWindow* window;

    result = kzsWindowCreateEmpty(windowProperties, &window);
    kzsErrorForward(result);

    /* Store window in the dynamic array. */
    {
        struct KzcDynamicArray* windowArray = kzaApplicationGetWindowArray_private(application);

        result = kzcDynamicArrayAdd(windowArray, window);
        kzsErrorForward(result);
    }

    *out_window = window;
    kzsSuccess();
}

/* TODO: move away from public interface */
kzsError kzaWindowDeleteAll(const struct KzaApplication* application)
{
    kzsError result;

    struct KzcDynamicArray* windowArray = kzaApplicationGetWindowArray_private(application);

    struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(windowArray);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzsWindow* window = (struct KzsWindow*)kzcDynamicArrayIteratorGetValue(it);
        result = kzsWindowDelete(window);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayDelete(windowArray);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzaSurfaceDeleteAll(const struct KzaApplication* application)
{
    kzsError result;

    struct KzcDynamicArray* surfaceArray = kzaApplicationGetSurfaceArray(application);

    struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(surfaceArray);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzsSurface* surface = (struct KzsSurface*)kzcDynamicArrayIteratorGetValue(it);
        result = kzsSurfaceDelete(surface);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayDelete(surfaceArray);
    kzsErrorForward(result);

    kzsSuccess();
}


kzsError kzaDisplayDeleteAll(const struct KzaApplication* application)
{
    kzsError result;

    struct KzcDynamicArray* displayArray = kzaApplicationGetDisplayArray(application);

    struct KzcDynamicArrayIterator it = kzcDynamicArrayGetIterator(displayArray);

    while (kzcDynamicArrayIterate(it))
    {
        struct KzsDisplay* display = (struct KzsDisplay*)kzcDynamicArrayIteratorGetValue(it);
        result = kzsDisplayDelete(display);
        kzsErrorForward(result);
    }

    result = kzcDynamicArrayDelete(displayArray);
    kzsErrorForward(result);

    kzsSuccess();
}
