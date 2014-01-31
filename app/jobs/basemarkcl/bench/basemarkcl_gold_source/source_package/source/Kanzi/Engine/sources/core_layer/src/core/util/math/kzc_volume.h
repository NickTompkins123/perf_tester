/**
* \file
* 3-dimensional volume.
* The volume sizes remain non-negative for all operations as long as the input sizes are non-negative.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_VOLUME_H
#define KZC_VOLUME_H


#include "kzc_vector3.h"

#include <system/wrappers/kzs_math.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzcVector3;


/** Structure for volume. */
struct KzcVolume
{
    kzFloat x; /**< X-coordinate for volume left side. */
    kzFloat y; /**< Y-coordinate for volume bottom side. */
    kzFloat z; /**< Z-coordinate for volume back side. */
    kzFloat width; /**< Width of the volume. */
    kzFloat height; /**< Height of the volume. */
    kzFloat depth; /**< Depth of the volume. */
};


/** Constant zero volume in origin. */
extern const struct KzcVolume KZC_VOLUME_EMPTY;

/** Constant for volume which has its minimum as infinite and maximum as negative infinite. */
extern const struct KzcVolume KZC_VOLUME_INFINITE;


/** Creates a volume from a point (x,y) and size (width,height). */
KZ_INLINE struct KzcVolume kzcVolume(kzFloat x, kzFloat y, kzFloat z, kzFloat width, kzFloat height, kzFloat depth)
{
    struct KzcVolume volume;

    volume.x = x;
    volume.y = y;
    volume.z = z;
    volume.width = width;
    volume.height = height;
    volume.depth = depth;

    return volume;
}

/** Creates a volume from two points defining arbitrary opposite corners. */
KZ_INLINE struct KzcVolume kzcVolumeFromPoints(kzFloat x1, kzFloat x2, kzFloat y1, kzFloat y2, kzFloat z1, kzFloat z2)
{
    struct KzcVolume volume;

    volume.x = kzsMinf(x1, x2);
    volume.y = kzsMinf(y1, y2);
    volume.z = kzsMinf(z1, z2);
    volume.width = kzsMaxf(x1, x2) - volume.x;
    volume.height = kzsMaxf(y1, y2) - volume.y;
    volume.depth = kzsMaxf(z1, z2) - volume.z;

    return volume;
}

/** Creates a volume from two points defining arbitrary opposite corners. */
KZ_INLINE struct KzcVolume kzcVolumeFromVectors(const struct KzcVector3* point1, const struct KzcVector3* point2)
{
    struct KzcVolume volume;

    volume.x = kzsMinf(kzcVector3GetX(point1), kzcVector3GetX(point2));
    volume.y = kzsMinf(kzcVector3GetY(point1), kzcVector3GetY(point2));
    volume.z = kzsMinf(kzcVector3GetZ(point1), kzcVector3GetZ(point2));
    volume.width = kzsMaxf(kzcVector3GetX(point1), kzcVector3GetX(point2)) - volume.x;
    volume.height = kzsMaxf(kzcVector3GetY(point1), kzcVector3GetY(point2)) - volume.y;
    volume.depth = kzsMaxf(kzcVector3GetZ(point1), kzcVector3GetZ(point2)) - volume.z;

    return volume;
}

/** Creates a volume from a point vector and size vector. */
KZ_INLINE struct KzcVolume kzcVolumeFromVectorAndSize(const struct KzcVector3* point, const struct KzcVector3* size)
{
    struct KzcVolume volume;

    volume.x = kzcVector3GetX(point);
    volume.y = kzcVector3GetY(point);
    volume.z = kzcVector3GetZ(point);
    volume.width = kzcVector3GetX(size);
    volume.height = kzcVector3GetY(size);
    volume.depth = kzcVector3GetZ(size);

    return volume;
}

/** Checks if the volume is empty. */
KZ_INLINE kzBool kzcVolumeIsEmpty(const struct KzcVolume* volume)
{
    return (volume->width <= 0 || volume->height <= 0 || volume->depth <= 0);
}

/** Checks if the given point is inside the given volume. The volume is closed from left/right but open from right/bottom. */
KZ_INLINE kzBool kzcVolumeContainsPoint(const struct KzcVolume* volume, const struct KzcVector3* point)
{
    kzFloat x = kzcVector3GetX(point) - volume->x;
    kzFloat y = kzcVector3GetY(point) - volume->y;
    kzFloat z = kzcVector3GetZ(point) - volume->z;
    return (x >= 0 && x < volume->width && 
            y >= 0 && y < volume->height && 
            z >= 0 && z < volume->depth);
}

/** Checks if the given volume contains the given other volume completely. */
KZ_INLINE kzBool kzcVolumeContainsVolume(const struct KzcVolume* volume, const struct KzcVolume* otherVolume)
{
    return ((volume->x <= otherVolume->x) && (volume->x + volume->width >= otherVolume->x + otherVolume->width) &&
            (volume->y <= otherVolume->y) && (volume->y + volume->height >= otherVolume->y + otherVolume->height) &&
            (volume->z <= otherVolume->z) && (volume->z + volume->depth >= otherVolume->z + otherVolume->depth));
}

/** Checks if the two given volumes overlap at least partially. */
KZ_INLINE kzBool kzcVolumeOverlaps(const struct KzcVolume* volume1, const struct KzcVolume* volume2)
{
    return ((volume1->x < volume2->x + volume2->width) && (volume2->x < volume1->x + volume1->width) &&
            (volume1->y < volume2->y + volume2->height) && (volume2->y < volume1->y + volume1->height) &&
            (volume1->z < volume2->z + volume2->depth) && (volume2->z < volume1->z + volume1->depth));
}

/** Checks if the two given volumes are same. */
KZ_INLINE kzBool kzcVolumeIsEqual(const struct KzcVolume* volume1, const struct KzcVolume* volume2)
{
    return (kzsFloatIsEqual(volume1->x, volume2->x) && kzsFloatIsEqual(volume1->width, volume2->width) &&
            kzsFloatIsEqual(volume1->y, volume2->y) && kzsFloatIsEqual(volume1->height, volume2->height) &&
            kzsFloatIsEqual(volume1->z, volume2->z) && kzsFloatIsEqual(volume1->depth, volume2->depth));
}

/** Calculates bound volume for union of two volumes. */
KZ_INLINE void kzcVolumeUnion(const struct KzcVolume* volume1, const struct KzcVolume* volume2, struct KzcVolume* targetVolume)
{
    targetVolume->x = kzsMinf(volume1->x, volume2->x);
    targetVolume->y = kzsMinf(volume1->y, volume2->y);
    targetVolume->z = kzsMinf(volume1->z, volume2->z);
    targetVolume->width = kzsMaxf(volume1->x + volume1->width, volume2->x + volume2->width) - targetVolume->x;
    targetVolume->height = kzsMaxf(volume1->y + volume1->height, volume2->y + volume2->height) - targetVolume->y;
    targetVolume->depth = kzsMaxf(volume1->z + volume1->depth, volume2->z + volume2->depth) - targetVolume->z;
}

/** Calculates intersection of two volumes. */
KZ_INLINE void kzcVolumeIntersection(const struct KzcVolume* volume1, const struct KzcVolume* volume2, struct KzcVolume* targetVolume)
{
    targetVolume->x = kzsMaxf(volume1->x, volume2->x);
    targetVolume->y = kzsMaxf(volume1->y, volume2->y);
    targetVolume->z = kzsMaxf(volume1->z, volume2->z);
    targetVolume->width = kzsMaxf(0.0f, kzsMinf(volume1->x + volume1->width, volume2->x + volume2->width) - targetVolume->x);
    targetVolume->height = kzsMaxf(0.0f, kzsMinf(volume1->y + volume1->height, volume2->y + volume2->height) - targetVolume->y);
    targetVolume->depth = kzsMaxf(0.0f, kzsMinf(volume1->z + volume1->depth, volume2->z + volume2->depth) - targetVolume->z);
}

/** Calculates the area of a volume. */
KZ_INLINE kzFloat kzcVolumeGetVolume(const struct KzcVolume* volume)
{
    return volume->width * volume->height * volume->depth;
}

/** Translates a volume. */
KZ_INLINE void kzcVolumeTranslate(struct KzcVolume* volume, const struct KzcVector3* translation)
{
    volume->x += kzcVector3GetX(translation);
    volume->y += kzcVector3GetY(translation);
    volume->z += kzcVector3GetZ(translation);
}


#endif
