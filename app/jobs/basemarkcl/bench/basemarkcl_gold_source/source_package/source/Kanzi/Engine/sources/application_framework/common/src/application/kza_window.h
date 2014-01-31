/**
 * \file
 * Application framework windows handling.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZA_WINDOW_H
#define KZA_WINDOW_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzaApplication;
struct KzsWindow;
struct KzsSurface;
struct KzsWindowProperties;
struct KzsSurfaceProperties;


/** Create a new application windows. */
kzsError kzaWindowCreate(struct KzaApplication* application, struct KzsWindowProperties* windowProperties, const struct KzsSurfaceProperties* surfaceProperties, struct KzsWindow** out_window);

/** Create a new empty application windows. The window does not contain drawing surface. It is expected to be available when using this. */
kzsError kzaWindowCreateEmpty(const struct KzaApplication* application, const struct KzsWindowProperties* windowProperties, struct KzsWindow** out_window);

/** Delete all windows and the dynamic array they are in. */
kzsError kzaWindowDeleteAll(const struct KzaApplication* application);

/** Delete all surfaces and the dynamic array they are in. */
kzsError kzaSurfaceDeleteAll(const struct KzaApplication* application);

/** Delete all displays and the dynamic array they are in. */
kzsError kzaDisplayDeleteAll(const struct KzaApplication* application);


#endif
