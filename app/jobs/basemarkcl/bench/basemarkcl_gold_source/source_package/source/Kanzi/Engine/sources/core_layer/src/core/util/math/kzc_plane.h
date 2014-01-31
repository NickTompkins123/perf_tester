/**
* \file
* Plane structure and operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_PLANE_H
#define KZC_PLANE_H


#include <system/kzs_types.h>


/* Forward declarations */
struct KzcVector3;
struct KzcRay;


/** Structure for plane. Mathematically, plane is defined as: a*x + b*y + c*z + d = 0. */
struct KzcPlane
{
    kzFloat a; /**< x component. */
    kzFloat b; /**< y component. */
    kzFloat c; /**< z component. */
    kzFloat d; /**< distance component. */
};


/** Creates a plane structure from a point (x,y,z) on the plane and distance from origin. */
KZ_INLINE struct KzcPlane kzcPlane(kzFloat x, kzFloat y, kzFloat z, kzFloat distance)
{
    struct KzcPlane plane;
    plane.a = x;
    plane.b = y;
    plane.c = z;
    plane.d = distance;

    return plane;
}

/** Constructs a plane from point and normal. */
void kzcPlaneCreateFromPointAndNormal(struct KzcPlane* plane, const struct KzcVector3* point, const struct KzcVector3* normal);

/** Gets point's distance to plane. */
kzFloat kzcPlaneDistanceToPoint(const struct KzcPlane* plane, const struct KzcVector3* point);

/**
* Gets ray and plane intersection point.
* The plane and the ray are tested for parallelness using the tolerance value.
* If the tolerance test does not pass, output values are filled with NaNs and KZ_FALSE is returned.
*/
kzBool kzcPlaneRayIntersection(const struct KzcPlane* plane, const struct KzcRay* ray, kzFloat parallelTolerance, kzFloat* out_distance, struct KzcVector3* out_intersectionPoint);

/** Project a vector onto a plane. */
void kzcPlaneProjectVector3(const struct KzcPlane* plane, const struct KzcVector3* vector, struct KzcVector3* out_vector);


#endif
