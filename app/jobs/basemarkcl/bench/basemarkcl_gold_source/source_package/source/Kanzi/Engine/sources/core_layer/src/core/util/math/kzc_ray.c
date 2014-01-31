/**
* \file
* Ray utility.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_ray.h"

#include <system/debug/kzs_error.h>

#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector4.h>


kzBool kzcRaySphereIntersection(const struct KzcRay* ray, const struct KzcVector3* spherePosition, const kzFloat sphereRadius, 
                                kzFloat* out_distance)
{
    kzBool result; 
    kzFloat distance;
    struct KzcVector3 rayToSphereCenter;
    kzFloat rayToSphereCenterLengthSquared;
    kzFloat rayToSphereClosestApproach; /* Closest point on ray to sphere center */
    kzFloat sphereRadiusSquared = sphereRadius * sphereRadius;

    kzcVector3Subtract(spherePosition, &ray->origin, &rayToSphereCenter);
    rayToSphereCenterLengthSquared = kzcVector3Dot(&rayToSphereCenter, &rayToSphereCenter);
    rayToSphereClosestApproach = kzcVector3Dot(&rayToSphereCenter, &ray->direction);

    /* Check if ray starts inside the sphere. */
    if(rayToSphereCenterLengthSquared < sphereRadiusSquared)
    {
        distance = 0.0f;
        result = KZ_TRUE;
    }
    /* Check if ray direction going away from the ball. */
    else if(rayToSphereClosestApproach < 0)
    {
        distance = KZ_FLOAT_MAXIMUM;
        result = KZ_FALSE;
    }
    else
    {
        kzFloat sphereDistance = sphereRadiusSquared - rayToSphereCenterLengthSquared + (rayToSphereClosestApproach * rayToSphereClosestApproach);

        /* Ray will not hit the sphere */
        if(sphereDistance < 0)
        {
            distance = KZ_FLOAT_MAXIMUM;
            result = KZ_FALSE;
        }
        else
        {
            distance = rayToSphereClosestApproach - kzsSqrtf(sphereDistance);
            result = KZ_TRUE;
        }
    }

    *out_distance = distance;
    return result;
}

kzBool kzcRayBoundingBoxIntersection(const struct KzcRay* ray, const struct KzcVector3* boundingBoxMinimum, 
                                     const struct KzcVector3* boundingBoxMaximum, 
                                     const struct KzcMatrix4x4* boxOrientation, kzFloat* out_distance)
{
    kzBool result = KZ_FALSE;
    kzFloat distance = KZ_FLOAT_MAXIMUM;
    const kzInt ORIGIN_RIGHT  = 0;
    const kzInt ORIGIN_LEFT   = 1;
    const kzInt ORIGIN_MIDDLE = 2;
    kzBool insideEveryAxis = KZ_TRUE; 
    struct KzcVector3 rayOrigin;
    struct KzcVector3 rayDirection;
    kzInt originAlignment[3]; /* Side of cube where ray origin is */
    struct KzcVector3 candidatePlanes;
    struct KzcVector3 candidatePlaneDistances = KZC_VECTOR3_ZERO;
    kzInt whichPlane = 0;
    kzInt axis;
    
    kzsAssert(kzcVector3GetX(boundingBoxMinimum) <= kzcVector3GetX(boundingBoxMaximum));
    kzsAssert(kzcVector3GetY(boundingBoxMinimum) <= kzcVector3GetY(boundingBoxMaximum));
    kzsAssert(kzcVector3GetZ(boundingBoxMinimum) <= kzcVector3GetZ(boundingBoxMaximum));

    /* Rotate ray to box space */
    {
        struct KzcVector3 rotatedRayOrigin;
        struct KzcVector4 rotatedRayDirection;
        struct KzcMatrix4x4 inverseWorld;
        kzcMatrix4x4Inverse(boxOrientation, &inverseWorld);
        {
            struct KzcVector4 direction;
            kzcMatrix4x4MultiplyVector3(&inverseWorld, &ray->origin, &rotatedRayOrigin);
            direction = kzcVector3ToVector4(&ray->direction);
            kzcMatrix4x4MultiplyVector4(&inverseWorld, &direction, &rotatedRayDirection);
    
            rayOrigin = rotatedRayOrigin;

            rayDirection = kzcVector3FromVector4(&rotatedRayDirection);
        }
    }

    /* Find candidate planes */
    for(axis = 0; axis < 3; ++axis)
    {
        /* If starting position is smaller than box's minimum coordinate on current axis, we can possibly hit it */
        if(rayOrigin.data[axis] < boundingBoxMinimum->data[axis]) 
        {
            originAlignment[axis] = ORIGIN_LEFT;
            candidatePlanes.data[axis] = boundingBoxMinimum->data[axis];
            insideEveryAxis = KZ_FALSE;
        }
        /* If starting position is smaller than box's maximum coordinate on current axis, we can possibly hit it */
        else if(rayOrigin.data[axis] > boundingBoxMaximum->data[axis]) 
        {
            originAlignment[axis] = ORIGIN_RIGHT;
            candidatePlanes.data[axis] = boundingBoxMaximum->data[axis];
            insideEveryAxis = KZ_FALSE;
        }
        /* Starting position lies between box's faces for this axis */
        else
        {
            candidatePlanes.data[axis] = 0.0f;
            originAlignment[axis] = ORIGIN_MIDDLE;
        }
    }

    /* Point is inside box on all separate axes */
    if(insideEveryAxis)
    {
        distance = 0.0f;
        result = KZ_TRUE;
    }

    if(!result)
    {
        /* Calculate distances to candidate planes */
        for(axis = 0; axis < 3; ++axis)
        {
            if(originAlignment[axis] == ORIGIN_MIDDLE || kzsFloatIsEqual(rayDirection.data[axis], 0.0f))
            {
                candidatePlaneDistances.data[axis] = -1.0f;
            }
            else
            {
                candidatePlaneDistances.data[axis] = (candidatePlanes.data[axis] - rayOrigin.data[axis]) / rayDirection.data[axis];
            }
        }

        /* Get largest of the distances for final choice of intersection */
        for(axis = 1; axis < 3; ++axis)
        {
            if(candidatePlaneDistances.data[whichPlane] < candidatePlaneDistances.data[axis])
            {
                whichPlane = axis;
            }
        }

        /* Check if candidate actually inside box */
        if(candidatePlaneDistances.data[whichPlane] < 0.0f) 
        {
            distance = 0.0f;
            result = KZ_TRUE;
        }
    }

    if(!result)
    {
        /* Find hit position */
        struct KzcVector3 hitPosition;
        struct KzcVector3 rayDistance;

        for(axis = 0; axis < 3; ++axis)
        {
            if(whichPlane != axis) 
            {
                hitPosition.data[axis] = rayOrigin.data[axis] + candidatePlaneDistances.data[whichPlane] * rayDirection.data[axis];
                if(hitPosition.data[axis] < boundingBoxMinimum->data[axis] || hitPosition.data[axis] > boundingBoxMaximum->data[axis])
                {
                    distance = KZ_FLOAT_MAXIMUM;
                    result = KZ_FALSE;
                    break;
                }
            }
            else
            {
                hitPosition.data[axis] = candidatePlanes.data[axis];
            }

            rayDistance.data[axis] = rayOrigin.data[axis] - hitPosition.data[axis];

            /* Calculate the distance from origin to hit position */
            if(axis == 2)
            {
                /* Transform back to world space to get correct distance. */
                kzcMatrix4x4MultiplyVector3By3x3(boxOrientation, &rayDistance, &rayDistance);
                distance = kzcVector3GetLength(&rayDistance);
                result = KZ_TRUE;
            }
        }
    }

    *out_distance = distance;
    return result;
}

void kzcRayTransform(struct KzcRay* ray, const struct KzcMatrix4x4* transform)
{
    kzcMatrix4x4MultiplyVector3(transform, &ray->origin, &ray->origin);
    kzcMatrix4x4MultiplyVector3By3x3(transform, &ray->direction, &ray->direction);
    kzcVector3Normalize(&ray->direction);
}
