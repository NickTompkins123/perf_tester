/**
* \file
* Drawable surface.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_SURFACE_H
#define KZS_SURFACE_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>
#include <system/input/kzs_input.h>


/* Forward declarations. */
struct KzsDisplay;
struct KzsWindow;
struct KzsSurfaceNative;
struct KzsDisplayNative;
struct KzsWindowNative;


/** Indicates that the implementation can choose any value for a surface property. */
#define KZS_SURFACE_PROPERTY_DONT_CARE -1


/**
 * \struct KzsSurface
 * Surface for graphics output.
 */
struct KzsSurface;


/** Properties for a surface. */
struct KzsSurfaceProperties
{
    kzInt bitsColorR;          /**< Size of red color channel in bits. Set to -1 for automatic. */
    kzInt bitsColorG;          /**< Size of green color channel in bits. Set to -1 for automatic. */
    kzInt bitsColorB;          /**< Size of blue color channel in bits. Set to -1 for automatic. */
    kzInt bitsAlpha;           /**< Size of alpha bits. */

    kzInt bitsStencil;         /**< Size of stencil buffer in bits. Set to -1 for automatic. */
    kzInt bitsDepthBuffer;     /**< Size of depth buffer in bits. Set to -1 for automatic. */

    kzInt antiAliasing;        /**< How many anti-aliasing samples used. Use 0 to turn AA off. */
    kzInt externalSurfaceID;   /**< External surface ID. */
};


/** Creates a drawable surface for a window. */
kzsError kzsSurfaceCreate(const struct KzsSurfaceProperties* surfaceProperties, const struct KzsDisplay* display,
                          const struct KzsWindow* window, struct KzsSurface** out_surface);

/** Deletes a surface and frees all memory allocated for it. */
kzsError kzsSurfaceDelete(struct KzsSurface* surface);

/** Makes a surface active for drawing or deactivates it. Only one surface can be active at a time. */
kzsError kzsSurfaceSetActive(const struct KzsSurface* surface, kzBool active);

/** Updates the buffer to the screen. */
kzsError kzsSurfaceUpdate(const struct KzsSurface* surface);

/** Reads pixels from the surface. */
kzsError kzsSurfaceReadPixels(const struct KzsSurface* surface, void* out_pixels);

/** Creates a native surface. */
kzsError kzsSurfaceNativeCreate(const struct KzsSurfaceProperties* surfaceProperties,struct KzsDisplayNative* displayNative,
                                struct KzsWindowNative* windowNative, struct KzsSurfaceNative** out_surfaceNative);

                                /** Deletes a native surface and frees all memory allocated for it. */
kzsError kzsSurfaceNativeDelete(struct KzsSurfaceNative* surfaceNative);

/** Sets surface active. */
kzsError kzsSurfaceNativeSetActive(struct KzsSurfaceNative* surfaceNative, kzBool active);

/** Updates the surface. */
kzsError kzsSurfaceNativeUpdate(struct KzsSurfaceNative* surfaceNative);

/** Copy color buffer data to the preallocated buffer out_pixels. */
kzsError kzsSurfaceNativeReadPixels(struct KzsSurfaceNative* nativeSurface, void* out_pixels);

void* kzsSurfaceNativeGetDisplay(struct KzsSurfaceNative* nativeSurface);

void* kzsSurfaceNativeGetContext(struct KzsSurfaceNative* nativeSurface);

/** Get native surface from surface. */
struct KzsSurfaceNative* kzsSurfaceGetNativeSurface(const struct KzsSurface* surface);

/** Sets used surface settings to the given surface properties struct. Does not get antialias or externalSurfaceID. */
void kzsSurfaceGetUsedSurfaceProperties(const struct KzsSurface* surface, struct KzsSurfaceProperties* surfaceProperties);
/** Sets used surface settings to the given surface properties struct. */
void kzsSurfaceGetUsedSurfaceNativeProperties(const struct KzsSurfaceNative* surfaceNative, struct KzsSurfaceProperties* surfaceProperties);


#endif
