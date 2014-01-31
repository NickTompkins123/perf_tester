/**
 * \file
 * Property accessors for KzuCameraNode.
 */
#ifndef KZU_CAMERA_PROPERTIES_H
#define KZU_CAMERA_PROPERTIES_H


#include "kzu_camera.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuCameraNode;


/** Gets the value of property DisableAspectRatio in cameraNode. */
kzBool kzuCameraNodeGetDisableAspectRatio(const struct KzuCameraNode* cameraNode);
/** Sets the value of property DisableAspectRatio in cameraNode. */
kzsError kzuCameraNodeSetDisableAspectRatio(const struct KzuCameraNode* cameraNode, kzBool value);

/** Gets the value of property ZNear in cameraNode. */
kzFloat kzuCameraNodeGetZNear(const struct KzuCameraNode* cameraNode);
/** Sets the value of property ZNear in cameraNode. */
kzsError kzuCameraNodeSetZNear(const struct KzuCameraNode* cameraNode, kzFloat value);

/** Gets the value of property ZFar in cameraNode. */
kzFloat kzuCameraNodeGetZFar(const struct KzuCameraNode* cameraNode);
/** Sets the value of property ZFar in cameraNode. */
kzsError kzuCameraNodeSetZFar(const struct KzuCameraNode* cameraNode, kzFloat value);

/** Gets the value of property ProjectionType in cameraNode. */
enum KzuProjectionType kzuCameraNodeGetProjectionType(const struct KzuCameraNode* cameraNode);
/** Sets the value of property ProjectionType in cameraNode. */
kzsError kzuCameraNodeSetProjectionType(const struct KzuCameraNode* cameraNode, enum KzuProjectionType value);

/** Gets the value of property OrthogonalCoordinateSystemType in cameraNode. */
enum KzuOrthogonalCoordinateSystem kzuCameraNodeGetOrthogonalCoordinateSystemType(const struct KzuCameraNode* cameraNode);
/** Sets the value of property OrthogonalCoordinateSystemType in cameraNode. */
kzsError kzuCameraNodeSetOrthogonalCoordinateSystemType(const struct KzuCameraNode* cameraNode, enum KzuOrthogonalCoordinateSystem value);

/** Gets the value of property OrthogonalPlaneHeight in cameraNode. */
kzFloat kzuCameraNodeGetOrthogonalPlaneHeight(const struct KzuCameraNode* cameraNode);
/** Sets the value of property OrthogonalPlaneHeight in cameraNode. */
kzsError kzuCameraNodeSetOrthogonalPlaneHeight(const struct KzuCameraNode* cameraNode, kzFloat value);

/** Gets the value of property FovType in cameraNode. */
enum KzuFieldOfViewType kzuCameraNodeGetFovType(const struct KzuCameraNode* cameraNode);
/** Sets the value of property FovType in cameraNode. */
kzsError kzuCameraNodeSetFovType(const struct KzuCameraNode* cameraNode, enum KzuFieldOfViewType value);

/** Gets the value of property Fov in cameraNode. Value in degrees. */
kzFloat kzuCameraNodeGetFov(const struct KzuCameraNode* cameraNode);
/** Sets the value of property Fov in cameraNode. Value in degrees. */
kzsError kzuCameraNodeSetFov(const struct KzuCameraNode* cameraNode, kzFloat value);

/** Gets the value of property OrientationAngle in cameraNode. */
kzFloat kzuCameraNodeGetOrientationAngle(const struct KzuCameraNode* cameraNode);
/** Sets the value of property OrientationAngle in cameraNode. */
kzsError kzuCameraNodeSetOrientationAngle(const struct KzuCameraNode* cameraNode, kzFloat value);


#endif
