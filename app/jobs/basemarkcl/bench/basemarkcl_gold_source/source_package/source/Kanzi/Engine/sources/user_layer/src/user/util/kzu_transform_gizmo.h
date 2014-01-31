/**
* \file
* Transform gizmo utility.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_TRANSFORM_GIZMO_H
#define KZU_TRANSFORM_GIZMO_H


#include <system/input/kzs_input.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzuEngine;
struct KzuProject;
struct KzuPropertyManager;
struct KzuMessageDispatcher;
struct KzcMemoryManager;
struct KzuPick;
struct KzuTransformedScene;


/** Enumeration for transform gizmo modes. */
enum KzuTransformGizmoMode
{
    KZU_TRANSFORM_GIZMO_MODE_NONE,          /** No mode. */
    KZU_TRANSFORM_GIZMO_MODE_TRANSLATE,     /** Translate mode. */
    KZU_TRANSFORM_GIZMO_MODE_ROTATE,        /** Rotate mode. */
    KZU_TRANSFORM_GIZMO_MODE_SCALE          /** Scale mode. */
};

/** Enumeration for transform gizmo coordinate system reference modes. */
enum KzuTransformGizmoReferenceMode
{
    KZU_TRANSFORM_GIZMO_REFERENCE_LOCAL,    /** Local reference. */
    KZU_TRANSFORM_GIZMO_REFERENCE_GLOBAL    /** Global reference. */
};

/** Enumeration for transform gizmo grid snap mode. */
enum KzuTransformGizmoGridSnapMode
{
    KZU_TRANSFORM_GIZMO_GRID_SNAP_NONE,     /**< Used when mode is not known. */
    KZU_TRANSFORM_GIZMO_GRID_SNAP_RELATIVE, /**< Snap to relative motion. */
    KZU_TRANSFORM_GIZMO_GRID_SNAP_ABSOLUTE, /**< Snap to absolute coordinates. */
    KZU_TRANSFORM_GIZMO_GRID_SNAP_OBJECT    /**< Snap to other objects. */
};


/**
* \struct KzuTransformGizmo
* Structure to hold transform gizmo context.
*/
struct KzuTransformGizmo;


/** Create a transform gizmo context. */
kzsError kzuTransformGizmoCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                                 kzString gizmoResourcePath, struct KzuTransformGizmo** out_gizmo);
/** Delete a transform gizmo context. */
kzsError kzuTransformGizmoDelete(struct KzuTransformGizmo* gizmo);

/** Sets the transform gizmo mode. */
kzsError kzuTransformGizmoSetMode(struct KzuTransformGizmo* gizmo, enum KzuTransformGizmoMode mode);
/** Gets the transform gizmo mode. */
enum KzuTransformGizmoMode kzuTransformGizmoGetMode(const struct KzuTransformGizmo* gizmo);

/** Sets the target object for the transform gizmo. */
kzsError kzuTransformGizmoSetTarget(struct KzuTransformGizmo* gizmo, kzString objectPath);
/** Gets the target object for the transform gizmo. */
kzString kzuTransformGizmoGetTarget(const struct KzuTransformGizmo* gizmo);

/** Tell whether or not grid snap is enabled. */
kzBool kzuTransformGizmoIsGridSnapEnabled(const struct KzuTransformGizmo* gizmo);
/** Sets the grid snap on or off in the transform gizmo. */
void kzuTransformGizmoSetGridSnapEnabled(struct KzuTransformGizmo* gizmo, kzBool enabled);
/** Get the size of the grid in the transform gizmo. */
kzFloat kzuTransformGizmoGetGridSize(const struct KzuTransformGizmo* gizmo);
/** Set the grid size for the transform gizmo.
*
* If the user is trying to set the grid size to a value <= 0.0f, it is left unchanged. */
void kzuTransformGizmoSetGridSize(struct KzuTransformGizmo* gizmo, kzFloat size);
/** Get the grid snap mode. */
enum KzuTransformGizmoGridSnapMode kzuTransformGizmoGetGridSnapMode(const struct KzuTransformGizmo* gizmo);
/** Set the grid snap mode. */
void kzuTransformGizmoSetGridSnapMode(struct KzuTransformGizmo* gizmo, enum KzuTransformGizmoGridSnapMode mode);

/** Render the transform gizmo. */
kzsError kzuTransformGizmoRender(const struct KzuTransformGizmo* gizmo, const struct KzuEngine* engine, const struct KzuProject* project,
                                 const struct KzuTransformedScene* transformedScene);
/** Process key events for the transform gizmo. */
kzsError kzuTransformGizmoProcessInput(struct KzuTransformGizmo* gizmo, const struct KzuEngine* engine, const struct KzuProject* project,
                                       struct KzuTransformedScene* transformedScene,
                                       struct KzuPick* pick, kzInt x, kzInt y, enum KzsInputPointingDeviceState state, kzBool* out_handled);

/** Transform gizmo callback prototype. */
typedef kzsError (*KzuTransformGizmoCallback)(kzString objectPath, void* userData);
/** Sets a callback for transform gizmo. Pass KZ_NULL to disable. */
void kzuTransformGizmoSetCallback(struct KzuTransformGizmo* gizmo, KzuTransformGizmoCallback callback, void* userData);


#endif
