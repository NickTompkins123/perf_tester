/**
* \file
* Bounding volume structure, that is, volume that bounds some area, typically geometry.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_BOUNDING_VOLUME_H
#define KZU_BOUNDING_VOLUME_H


#include <system/debug/kzs_error.h>

#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_plane.h>


/** Type of bounding volume. */
enum KzuBoundingVolumeType
{
    KZU_BOUNDING_VOLUME_TYPE_AXIS_ALIGNED_BOX, /**< Axis aligned bounding box. */
    KZU_BOUNDING_VOLUME_TYPE_SPHERE /**< Sphere bounding volume. */
};


/* Forward declaration structures. */
struct KzcMemoryManager;
struct KzcPlane;
struct KzcVertexList;
struct KzcMatrix4x4;
struct KzcVector3;
struct KzcVolume;


/** Bounding box corners. */
#define KZU_BOUNDING_BOX_CORNER_BACK_BOTTOM_LEFT    0       /**< Bottom left. */
#define KZU_BOUNDING_BOX_CORNER_BACK_TOP_LEFT      1        /**< Top left. */
#define KZU_BOUNDING_BOX_CORNER_BACK_TOP_RIGHT      2       /**< Top right. */
#define KZU_BOUNDING_BOX_CORNER_BACK_BOTTOM_RIGHT    3      /**< Bottom right. */
#define KZU_BOUNDING_BOX_CORNER_FRONT_BOTTOM_LEFT   4       /**< Bottom left. */
#define KZU_BOUNDING_BOX_CORNER_FRONT_TOP_LEFT     5        /**< Top left. */
#define KZU_BOUNDING_BOX_CORNER_FRONT_TOP_RIGHT     6       /**< Top right. */
#define KZU_BOUNDING_BOX_CORNER_FRONT_BOTTOM_RIGHT   7      /**< Bottom right. */


/**
* \struct KzuBoundingVolume
* Base structure for bounding volume.
*/
struct KzuBoundingVolume;

/**
* \struct KzuBoundingVolumeAxisAlignedBox
* Axis aligned bounding box, inherited from KzuBoundingVolume.
*/
struct KzuBoundingVolumeAxisAlignedBox;

/**
* \struct KzuBoundingVolumeSphere
* Sphere volume, inherited from KzuBoundingVolume.
*/
struct KzuBoundingVolumeSphere;

/**
* \struct KzuTransformedBoundingVolume
* Bounding volume in world coordinates.
*/
struct KzuTransformedBoundingVolume
{    
    struct KzcVector3 cornerPoints[8];   /**< Transformed corner points in world coordinates. */
    struct KzcVector3 transformedCenter; /**< Transformed center position in world coordinates. */
};


kzsError kzuBoundingVolumeCreateEmpty(const struct KzcMemoryManager* memoryManager,
                                      struct KzuBoundingVolume** out_volume);
/** Creates new bounding volume. */
kzsError kzuBoundingVolumeCreate(const struct KzcMemoryManager* memoryManager, const struct KzcVertexList* vertexList,
                                 struct KzuBoundingVolume** out_volume);
/** Creates copy of bounding volume. */
kzsError kzuBoundingVolumeCopy(const struct KzcMemoryManager* memoryManager, const struct KzuBoundingVolume* sourceBoundingVolume, struct KzuBoundingVolume** out_targetBoundingVolume);
/** Deletes a bounding volume. */
kzsError kzuBoundingVolumeDelete(struct KzuBoundingVolume* boundingVolume);


/** Calculates world transformed bounding volume from bounding volume and world transformation. */
void kzuBoundingVolumeCalculateTransformedBoundingVolume(struct KzuBoundingVolume* boundingVolume, const struct KzcMatrix4x4* worldTransformation,
                                                         struct KzuTransformedBoundingVolume* out_transformedBoundingVolume);
/** Transforms an AABB and calculates a new AABB. */
void kzuAxisAlignedBoundingBoxFromTransformedAABB(const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax, const struct KzcMatrix4x4* transform,
                                                  struct KzcVector3* out_aabbMin, struct KzcVector3* out_aabbMax);
/** Creates a transformed bounding volume from an axis aligned bounding box. */
void kzuTransformedBoundingVolumeCreateFromAABB(const struct KzcVector3* aabbMin, const struct KzcVector3* aabbMax, const struct KzcMatrix4x4* worldTransformation,
                                                struct KzuTransformedBoundingVolume* out_transformedBoundingVolume);
/** Gets axis-aligned bounding box values (minimum & maximum) from transformed bounding volume. */
void kzuTransformedBoundingVolumeGetAABB(const struct KzuTransformedBoundingVolume* transformedBoundingVolume, struct KzcVector3* out_minimum,
                                         struct KzcVector3* out_maximum);
/** Checks if transformed bounding volume is above plane. Used in frustum culling. */
kzBool kzuTransformedBoundingVolumeIsAbovePlane(const struct KzuTransformedBoundingVolume* transformedBoundingVolume, const struct KzcPlane* plane);
/** Gets corner point from transformed bounding box. */
struct KzcVector3 kzuTransformedBoundingVolumeBoxGetCornerPoint(const struct KzuTransformedBoundingVolume* boundingVolume, kzUint cornerIndex);


/** Sets bounding volume from volume. */
void kzuBoundingVolumeSetVolume(struct KzuBoundingVolume* boundingVolume, const struct KzcVolume* volume);


/** Gets axis aligned box from bounding volume. */
struct KzuBoundingVolumeAxisAlignedBox* kzuBoundingVolumeGetAxisAlignedBox(struct KzuBoundingVolume* boundingVolume);
/** Gets sphere from bounding volume. */
struct KzuBoundingVolumeSphere* kzuBoundingVolumeGetSphere(struct KzuBoundingVolume* boundingVolume);


/** Gets minimum corner from axis aligned box. */
struct KzcVector3 kzuBoundingVolumeAxisAlignedBoxGetMinimumCorner(const struct KzuBoundingVolumeAxisAlignedBox* boundingVolume);
/** Gets maximum corner from axis aligned box. */
struct KzcVector3 kzuBoundingVolumeAxisAlignedBoxGetMaximumCorner(const struct KzuBoundingVolumeAxisAlignedBox* boundingVolume);


/** Gets radius of bounding volume sphere. */
kzFloat kzuBoundingVolumeSphereGetRadius(const struct KzuBoundingVolumeSphere* boundingVolume);


#endif
