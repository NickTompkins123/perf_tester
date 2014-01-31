/**
* \file
* Represents camera (viewer) in scene graph. Node inherited from KzuObjectNode.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_CAMERA_H
#define KZU_CAMERA_H


#include <user/scene_graph/kzu_object_base.h>

#include <core/util/math/kzc_plane.h>
#include <core/util/math/kzc_matrix4x4.h>


/** Enumeration for camera's field of view type */
enum KzuFieldOfViewType
{
    KZU_CAMERA_FIELD_OF_VIEW_HORIZONTAL,    /**< Horizontal field of view. */
    KZU_CAMERA_FIELD_OF_VIEW_VERTICAL       /**< Vertical field of view. */
};

/** Enumeration for camera's projection type */
enum KzuProjectionType
{
    KZU_PROJECTION_PERSPECTIVE, /**< Perspective projection */
    KZU_PROJECTION_ORTHOGRAPHIC /**< Orthogonal projection */
};

/** Enumeration for orthogonal coordinate system. */
enum KzuOrthogonalCoordinateSystem
{
    KZU_ORTHOGONAL_COORDINATE_SYSTEM_ABSOLUTE,  /**< Absolute orthogonal projection. */
    KZU_ORTHOGONAL_COORDINATE_SYSTEM_RELATIVE   /**< Relative orthogonal projection. */
};

/** Enumeration for defined frustum planes. */
enum KzuFrustumPlane
{
    KZU_FRUSTUM_PLANE_NEAR,     /**< Near frustum plane. */
    KZU_FRUSTUM_PLANE_FAR,      /**< Far frustum plane. */
    KZU_FRUSTUM_PLANE_TOP,      /**< Top frustum plane. */
    KZU_FRUSTUM_PLANE_BOTTOM,   /**< Bottom frustum plane. */
    KZU_FRUSTUM_PLANE_LEFT,     /**< Left frustum plane. */
    KZU_FRUSTUM_PLANE_RIGHT     /**< Right frustum plane. */
};

#define KZU_FRUSTUM_PLANE_COUNT 6 /**< Number of frustum planes. */


/** Object type identifier for Light objects. */
extern const KzuObjectType KZU_OBJECT_TYPE_CAMERA;


/* Forward declaration structures. */
struct KzuObjectNode;
struct KzcMemoryManager;
struct KzuPropertyManager;
struct KzuMessageDispatcher;
struct KzcMatrix4x4;
struct KzcVector3;


/**
 * \struct KzuCameraNode
 * Camera in scene graph, inherited from KzuObjectNode.
 */
struct KzuCameraNode;


/** Creates new camera node. */
kzsError kzuCameraNodeCreate(const struct KzcMemoryManager* memoryManager, struct KzuPropertyManager* propertyManager, struct KzuMessageDispatcher* messageDispatcher,
                             kzString name, struct KzuCameraNode** out_cameraNode);
/** Deletes a camera node. */
kzsError kzuCameraNodeDelete(struct KzuCameraNode* cameraNode);


/** Gets a camera node from object node. */
struct KzuCameraNode* kzuCameraNodeFromObjectNode(const struct KzuObjectNode* objectNode);
/** Returns an object node from camera node. */
struct KzuObjectNode* kzuCameraNodeToObjectNode(const struct KzuCameraNode* cameraNode);

/** Sets orthogonal projection for camera. */
kzsError kzuCameraNodeSetOrthogonalProjection(const struct KzuCameraNode* cameraNode, enum KzuOrthogonalCoordinateSystem coordinateSystem);
/** Sets perspective projection for camera. */
kzsError kzuCameraNodeSetPerspectiveProjection(const struct KzuCameraNode* cameraNode);
/** Sets aspect ratio for camera. */
kzsError kzuCameraNodeSetAspectRatio(const struct KzuCameraNode* cameraNode, kzFloat aspectRatio);
/** Gets aspect ratio from camera. */
kzFloat kzuCameraNodeGetAspectRatio(const struct KzuCameraNode* cameraNode);

/** Sets inherited aspect ratio value for camera. */
void kzuCameraNodeSetInheritedAspectRatio(const struct KzuCameraNode* cameraNode, kzFloat inheritedAspectRatio);

/** Creates a look at matrix for camera. */
kzsError kzuCameraNodeLookAt(const struct KzuCameraNode* cameraNode, struct KzcVector3 position, 
                             struct KzcVector3 lookAtPoint, struct KzcVector3 upVector);
/** Creates frustum planes for camera node. */
void kzuCameraNodeCalculateFrustumPlanes(const struct KzuCameraNode* cameraNode, const struct KzcMatrix4x4* cameraViewMatrix,
                                  struct KzcPlane* out_frustumPlanes);
/** Utility for calculating ortho projection from values. */
void kzuCameraNodeCalculateOrthogonalProjectionFromValues(kzFloat left, kzFloat right, kzFloat top, kzFloat bottom, 
                                                    enum KzuOrthogonalCoordinateSystem coordinateSystem, 
                                                    enum KzuFieldOfViewType fovType, kzFloat orthoHeight, kzFloat aspectRatio,
                                                    struct KzcMatrix4x4* out_projectionMatrix);
/** Creates a orthogonal projection for camera. */
void kzuCameraNodeCalculateOrthogonalProjection(const struct KzuCameraNode* cameraNode, kzFloat left, kzFloat right, kzFloat top, kzFloat bottom,
                                            struct KzcMatrix4x4* out_projectionMatrix);
/** Creates a projection for camera. */
void kzuCameraNodeCalculateProjectionMatrix(const struct KzuCameraNode* cameraNode, struct KzcMatrix4x4* out_projectionMatrix);

/** Clones a camera data from source. */
kzsError kzuCameraNodeCloneData(const struct KzuCameraNode* sourceCameraNode, const struct KzuCameraNode* destinationCameraNode);


#include "kzu_camera_properties.h"


#endif
