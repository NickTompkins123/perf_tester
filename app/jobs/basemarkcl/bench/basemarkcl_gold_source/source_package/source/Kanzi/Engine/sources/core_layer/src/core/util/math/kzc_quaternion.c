/**
* \file
* Quaternion operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_quaternion.h"

#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/math/kzc_vector3.h>

#include <system/wrappers/kzs_math.h>


void kzcQuaternionSetIdentity(struct KzcQuaternion* quaternion)
{
    quaternion->w = 1.0f;
    quaternion->x = 0.0f;
    quaternion->y = 0.0f;
    quaternion->z = 0.0f;
}

void kzcQuaternionInitializeFromVector(const struct KzcVector3* axis, kzFloat angleInRadians, struct KzcQuaternion* out_quaternion)
{
    /* Conversion from vector to quaternion is defined as: q = cos(theta / 2) + v*sin(theta / 2). */
    kzFloat angle = angleInRadians * 0.5f;
    kzFloat sinAngle = kzsSinf(angle);
    struct KzcVector3 axisNormalized = *axis;
    kzcVector3Normalize(&axisNormalized);

    out_quaternion->w = kzsCosf(angle);
    out_quaternion->x = kzcVector3GetX(&axisNormalized) * sinAngle;
    out_quaternion->y = kzcVector3GetY(&axisNormalized) * sinAngle;
    out_quaternion->z = kzcVector3GetZ(&axisNormalized) * sinAngle;
}

void kzcQuaternionInitializeFromMatrix4x4(const struct KzcMatrix4x4* matrix, struct KzcQuaternion* out_quaternion)
{
    kzFloat matrixTrace;
    matrixTrace = kzcMatrix4x4Trace(matrix);

    /* Quaternion is initialized from matrix by following manner:
        MatToQuat @ http://www.cs.indiana.edu/~hanson/quatvis/code/quatcodekit.txt 1.6.2009.
    */

    /* Positive matrix trace. */
    if(matrixTrace - 1.0f > 0.0f)
    {
        kzFloat sqrtTraceDouble;
        sqrtTraceDouble = kzsSqrtf(matrixTrace) * 2.0f;

        out_quaternion->w = 0.25f * sqrtTraceDouble;
        out_quaternion->x = (matrix->data[9] - matrix->data[6]) / sqrtTraceDouble;
        out_quaternion->y = (matrix->data[2] - matrix->data[8]) / sqrtTraceDouble;
        out_quaternion->z = (matrix->data[4] - matrix->data[1]) / sqrtTraceDouble;
    }
    /* Negative or zero trace */
    else
    {
        /* See which column has the biggest value in diagonal. */
        if (matrix->data[0] > matrix->data[5] && matrix->data[0] > matrix->data[10])
        {
            kzFloat divisor = kzsSqrtf(1.0f + matrix->data[0] - matrix->data[5] - matrix->data[10]) * 2.0f;
            out_quaternion->w = (matrix->data[9] - matrix->data[6]) / divisor;
            out_quaternion->x = 0.25f * divisor;
            out_quaternion->y = (matrix->data[4] + matrix->data[1]) / divisor;
            out_quaternion->z = (matrix->data[2] + matrix->data[8]) / divisor;
            
        }
        else if (matrix->data[5] > matrix->data[10])
        {
            kzFloat divisor = kzsSqrtf(1.0f + matrix->data[5] - matrix->data[0] - matrix->data[10]) * 2.0f;
            out_quaternion->w = (matrix->data[2] - matrix->data[8]) / divisor;
            out_quaternion->x = (matrix->data[4] + matrix->data[1]) / divisor;
            out_quaternion->y = 0.25f * divisor;
            out_quaternion->z = (matrix->data[9] + matrix->data[6]) / divisor;
        }
        else 
        {
            kzFloat divisor = kzsSqrtf(1.0f + matrix->data[10] - matrix->data[0] - matrix->data[5]) * 2.0f;
            out_quaternion->w = (matrix->data[4] - matrix->data[1]) / divisor;
            out_quaternion->x = (matrix->data[2] + matrix->data[8]) / divisor;
            out_quaternion->y = (matrix->data[9] + matrix->data[6]) / divisor;
            out_quaternion->z = 0.25f * divisor;
        }
    }

    kzcQuaternionNormalize(out_quaternion);
}

void kzcQuaternionConvertToMatrix4x4(const struct KzcQuaternion* quaternion, struct KzcMatrix4x4* out_matrix)
{
    /* Quaternion -> matrix3x3 is defined as:
    | 1 - 2*qy2 - 2*qz2      2*qx*qy - 2*qz*qw       2*qx*qz + 2*qy*qw |
    | 2*qx*qy + 2*qz*qw      1 - 2*qx2 - 2*qz2       2*qy*qz - 2*qx*qw |
    | 2*qx*qz - 2*qy*qw      2*qy*qz + 2*qx*qw       1 - 2*qx2 - 2*qy2 |
    
    Source: http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm 1.6.2009
    */

    /* Exponential and multiplication precalculations. */
    kzFloat x2 = quaternion->x * quaternion->x;
    kzFloat y2 = quaternion->y * quaternion->y;
    kzFloat z2 = quaternion->z * quaternion->z;
    kzFloat xy = quaternion->x * quaternion->y;
    kzFloat xz = quaternion->x * quaternion->z;
    kzFloat yz = quaternion->y * quaternion->z;
    kzFloat wx = quaternion->w * quaternion->x;
    kzFloat wy = quaternion->w * quaternion->y;
    kzFloat wz = quaternion->w * quaternion->z;
    kzcMatrix4x4SetIdentity(out_matrix);
    
    out_matrix->data[0] = 1.0f - 2.0f * y2 - 2.0f * z2; /*  1 - 2*qy2 - 2*qz2 */
    out_matrix->data[1] = 2.0f * (xy - wz); /* 2*qx*qy - 2*qz*qw */
    out_matrix->data[2] = 2.0f * (xz + wy); /* 2*qx*qz + 2*qy*qw */

    out_matrix->data[4] = 2.0f * (xy + wz); /* 2*qx*qy + 2*qz*qw */
    out_matrix->data[5] = 1.0f - 2.0f * x2 - 2.0f * z2; /* 1 - 2*qx2 - 2*qz2 */
    out_matrix->data[6] = 2.0f * (yz - wx); /* 2*qy*qz - 2*qx*qw */

    out_matrix->data[8] = 2.0f * (xz - wy); /* 2*qx*qz - 2*qy*qw */
    out_matrix->data[9] = 2.0f * (yz + wx); /* 2*qy*qz + 2*qx*qw */
    out_matrix->data[10] = 1.0f - 2.0f * x2 - 2.0f * y2; /* 1 - 2*qx2 - 2*qy2*/
}

void kzcQuaternionMultiply(const struct KzcQuaternion* quaternion1, const struct KzcQuaternion* quaternion2, struct KzcQuaternion* out_quaternion)
{
    out_quaternion->w = (quaternion1->w * quaternion2->w) - (quaternion1->x * quaternion2->x) -
                        (quaternion1->y * quaternion2->y) - (quaternion1->z * quaternion2->z);
    out_quaternion->x = (quaternion1->w * quaternion2->x) + (quaternion1->x * quaternion2->w) +
                        (quaternion1->y * quaternion2->z) - (quaternion1->z * quaternion2->y);
    out_quaternion->y = (quaternion1->w * quaternion2->y) + (quaternion1->y * quaternion2->w) +
                        (quaternion1->z * quaternion2->x) - (quaternion1->x * quaternion2->z);
    out_quaternion->z = (quaternion1->w * quaternion2->z) + (quaternion1->z * quaternion2->w) +
                        (quaternion1->x * quaternion2->y) - (quaternion1->y * quaternion2->x);
}

void kzcQuaternionMultiplyVector3(const struct KzcQuaternion* quaternion, const struct KzcVector3* vector3, struct KzcVector3* out_vector3)
{
    struct KzcQuaternion quaternionFromVector;
    struct KzcQuaternion inverse;
    struct KzcQuaternion resultQuaternion;
    struct KzcVector3 vectorNormalized = *vector3;
    kzcVector3Normalize(&vectorNormalized);

    quaternionFromVector.w = 0.0f;
    quaternionFromVector.x = kzcVector3GetX(&vectorNormalized);
    quaternionFromVector.y = kzcVector3GetY(&vectorNormalized);
    quaternionFromVector.z = kzcVector3GetZ(&vectorNormalized);
    inverse = *quaternion;

    kzcQuaternionInverse(&inverse);
    kzcQuaternionMultiply(&quaternionFromVector, &inverse, &resultQuaternion);
    kzcQuaternionMultiply(quaternion, &resultQuaternion, &quaternionFromVector);

    *out_vector3 = kzcVector3(quaternionFromVector.x, quaternionFromVector.y, quaternionFromVector.z);
}

void kzcQuaternionNormalize(struct KzcQuaternion* quaternion)
{
    kzFloat magnitude = quaternion->w * quaternion->w + quaternion->x * quaternion->x + quaternion->y * quaternion->y
        + quaternion->z * quaternion->z;

    kzFloat length = kzsSqrtf(magnitude);
    quaternion->w /= length;
    quaternion->x /= length;
    quaternion->y /= length;
    quaternion->z /= length;
}

void kzcQuaternionInverse(struct KzcQuaternion* quaternion)
{
    quaternion->x = -quaternion->x;
    quaternion->y = -quaternion->y;
    quaternion->z = -quaternion->z;
}

void kzcQuaternionSlerp(const struct KzcQuaternion* quaternion1, const struct KzcQuaternion* quaternion2, kzFloat t, struct KzcQuaternion* out_quaternion)
{
    kzFloat t1, t2;
    kzFloat sinTheta;
    kzFloat cosineTheta;
    kzFloat theta, phi;
    kzBool flip;

    /* 
       SLERP is defined as Slerp(q1, q2, t) = sin((1 - t) * theta)             sin(t * theta)
                                              -------------------- * q1 +      -------------- * q2
                                                  sin(theta)                      sin(theta)
    */

    /* Cosine theta can be achieved from quaternion dot product. */
    cosineTheta = quaternion1->x * quaternion2->x 
                + quaternion1->y * quaternion2->y
                + quaternion1->z * quaternion2->z
                + quaternion1->w * quaternion2->w;

    /* quaternion1 on opposite hemisphere from quaternion2, -quaternion2. */
    if (cosineTheta < 0.0f)
    {
        cosineTheta = -cosineTheta;
        flip = KZ_TRUE;
    }
    /* Same side */
    else
    {
        flip = KZ_FALSE;
    }

    /* quaternion1 equals quaternion2, return quaternion1. */
    if(1.0f - cosineTheta < KZ_FLOAT_EPSILON)
    {
        *out_quaternion = *quaternion1;
    }
    else
    {
        theta = kzsAcosf(cosineTheta);
        phi   = theta;
        sinTheta = kzsSinf(theta);
        t2  = kzsSinf(theta - t * phi) / sinTheta;
        t1 = kzsSinf(t * phi) / sinTheta;

        if (flip)
        {
            t1 = -t1;
        }

        /* Interpolate */
        out_quaternion->x = t2 * quaternion1->x + t1 * quaternion2->x;
        out_quaternion->y = t2 * quaternion1->y + t1 * quaternion2->y;
        out_quaternion->z = t2 * quaternion1->z + t1 * quaternion2->z;
        out_quaternion->w = t2 * quaternion1->w + t1 * quaternion2->w;
    }
}
