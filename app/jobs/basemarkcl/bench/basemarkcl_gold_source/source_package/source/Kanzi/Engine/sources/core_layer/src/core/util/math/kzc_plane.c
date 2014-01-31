/**
* \file
* Plane operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_plane.h"

#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_ray.h>


void kzcPlaneCreateFromPointAndNormal(struct KzcPlane* plane, const struct KzcVector3* point, const struct KzcVector3* normal)
{
    plane->a = kzcVector3GetX(normal);
    plane->b = kzcVector3GetY(normal);
    plane->c = kzcVector3GetZ(normal);
    plane->d = -kzcVector3Dot(point, normal);
}

kzFloat kzcPlaneDistanceToPoint(const struct KzcPlane* plane, const struct KzcVector3* vector)
{
    return (plane->a * kzcVector3GetX(vector)) + 
           (plane->b * kzcVector3GetY(vector)) + 
           (plane->c * kzcVector3GetZ(vector)) +
           plane->d;
}


kzBool kzcPlaneRayIntersection(const struct KzcPlane* plane, const struct KzcRay* ray, kzFloat parallelTolerance, kzFloat* out_distance, struct KzcVector3* out_intersectionPoint)
{
    kzBool intersects = KZ_FALSE;
    struct KzcVector3 result = kzcVector3(KZS_NAN, KZS_NAN, KZS_NAN);
    struct KzcVector3 planeNormal = kzcVector3(plane->a, plane->b, plane->c);
    kzFloat distance = KZS_NAN;
    kzFloat dot = kzcVector3Dot(&ray->direction, &planeNormal);

    if (kzsFabsf(dot) > parallelTolerance)
    {
        distance = -(kzcVector3Dot(&ray->origin, &planeNormal) + plane->d) / dot;
        if (distance > 0.0f)
        {
            intersects = KZ_TRUE;
            kzcVector3Scale(&ray->direction, distance, &result);
            kzcVector3Add(&result, &ray->origin, &result);
        }
    }
    
    *out_distance = distance;
    *out_intersectionPoint = result;

    return intersects;
}

void kzcPlaneProjectVector3(const struct KzcPlane* plane, const struct KzcVector3* vector, struct KzcVector3* out_vector)
{
    struct KzcVector3 result;
    struct KzcVector3 scaledVector;
    struct KzcVector3 planeNormal = kzcVector3(plane->a, plane->b, plane->c);

    kzcVector3Scale(&planeNormal, kzcVector3Dot(vector, &planeNormal), &scaledVector);
    kzcVector3Subtract(vector, &scaledVector, &result);

    *out_vector = result;
}
