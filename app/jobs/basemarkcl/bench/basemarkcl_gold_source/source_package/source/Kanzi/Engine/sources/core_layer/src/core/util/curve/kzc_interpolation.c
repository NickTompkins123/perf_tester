/**
* \file
* Interpolation functions
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_interpolation.h"

#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector4.h>


/** Clamps the given value in the range [0,1]. */
static kzFloat kzcInterpolateClamp_internal(kzFloat t)
{
    return (t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t));
}

kzFloat kzcInterpolateLinear(kzFloat value1, kzFloat value2, kzFloat t)
{
    kzFloat clampedT = kzcInterpolateClamp_internal(t);
    return value1 * (1.0f - clampedT) + value2 * clampedT;
}

void kzcInterpolateLinearVector(kzUint dimension, const kzFloat* point1, const kzFloat* point2, kzFloat t, kzFloat* out_point)
{
    kzFloat clampedT = kzcInterpolateClamp_internal(t);
    kzUint i;
    for (i = 0; i < dimension; ++i)
    {
        out_point[i] = point1[i] * (1.0f - clampedT) + point2[i] * clampedT;
    }
}

void kzcInterpolateLinearVector2(const struct KzcVector2* const point1, const struct KzcVector2* const point2,
                                 kzFloat t, struct KzcVector2* out_vector)
{
    kzcInterpolateLinearVector(2, point1->data, point2->data, t, out_vector->data);
}

void kzcInterpolateLinearVector3(const struct KzcVector3* const point1, const struct KzcVector3* const point2,
                                 kzFloat t, struct KzcVector3* out_vector)
{
    kzcInterpolateLinearVector(3, point1->data, point2->data, t, out_vector->data);
}

void kzcInterpolateLinearVector4(const struct KzcVector4* const point1, const struct KzcVector4* const point2,
                                 kzFloat t, struct KzcVector4* out_vector)
{
    kzcInterpolateLinearVector(4, point1->data, point2->data, t, out_vector->data);
}

kzFloat kzcInterpolateBezier(kzUint degree, kzFloat* points, kzFloat t)
{
    kzFloat result;

    if (degree == 0)
    {
        result = points[0];
    }
    else if (degree == 1)
    {
        result = kzcInterpolateLinear(points[0], points[1], t);
    }
    else
    {
        result = kzcInterpolateLinear(kzcInterpolateBezier(degree - 1, points, t),
                                      kzcInterpolateBezier(degree - 1, points + 1, t),
                                      t);
    }

    return result;
}

static kzFloat kzcInterpolateBezierVectorHelper_internal(kzUint currentDimension, kzUint degree, const kzFloat** points, kzFloat t)
{
    kzFloat result;

    if (degree == 0)
    {
        result = points[0][currentDimension];
    }
    else if (degree == 1)
    {
        result = kzcInterpolateLinear(points[0][currentDimension], points[1][currentDimension], t);
    }
    else
    {
        result = kzcInterpolateLinear(kzcInterpolateBezierVectorHelper_internal(currentDimension, degree - 1, points, t),
                                      kzcInterpolateBezierVectorHelper_internal(currentDimension, degree - 1, points + 1, t),
                                      t);
    }

    return result;
}

void kzcInterpolateBezierVector(kzUint dimension, kzUint degree, const kzFloat** points, kzFloat t, kzFloat* out_point)
{
    kzUint i;
    for (i = 0; i < dimension; ++i)
    {
        out_point[i] = kzcInterpolateBezierVectorHelper_internal(i, degree, points, t);
    }
}

kzFloat kzcInterpolateBezierQuadratic(kzFloat point1, kzFloat controlPoint, kzFloat point2, kzFloat t)
{
    /* B(t) = A*P0 + B*P1 + C*P2, where
         P0 = point1
         P1 = controlPoint
         P2 = point2
          A = (1 - t)^2
          B = 2*(1 - t)*t
          C = t^2 */
    kzFloat clampedT = kzcInterpolateClamp_internal(t);
    kzFloat oneMinusT = (1.0f - clampedT);
    kzFloat A = oneMinusT * oneMinusT;
    kzFloat B = 2 * oneMinusT * clampedT;
    kzFloat C = clampedT * clampedT;

    return A * point1 + B * controlPoint + C * point2;
}

void kzcInterpolateBezierQuadraticVector(kzUint dimension, const kzFloat* point1, const kzFloat* controlPoint,
                                         const kzFloat* point2, kzFloat t, kzFloat* out_point)
{
    kzFloat clampedT = kzcInterpolateClamp_internal(t);
    kzFloat oneMinusT = (1.0f - clampedT);
    kzFloat A = oneMinusT * oneMinusT;
    kzFloat B = 2 * oneMinusT * clampedT;
    kzFloat C = clampedT * clampedT;
    kzUint i;

    for (i = 0; i < dimension; ++i)
    {
        out_point[i] = A * point1[i] + B * controlPoint[i] + C * point2[i];
    }
}

void kzcInterpolateBezierQuadraticVector2(const struct KzcVector2* const point1, const struct KzcVector2* const controlPoint,
                                          const struct KzcVector2* const point2, kzFloat t, struct KzcVector2* out_vector)
{
    kzcInterpolateBezierQuadraticVector(2, point1->data, controlPoint->data, point2->data, t, out_vector->data);
}

void kzcInterpolateBezierQuadraticVector3(const struct KzcVector3* const point1, const struct KzcVector3* const controlPoint,
                                          const struct KzcVector3* const point2, kzFloat t, struct KzcVector3* out_vector)
{
    kzcInterpolateBezierQuadraticVector(3, point1->data, controlPoint->data, point2->data, t, out_vector->data);
}

void kzcInterpolateBezierQuadraticVector4(const struct KzcVector4* const point1, const struct KzcVector4* const controlPoint,
                                          const struct KzcVector4* const point2, kzFloat t, struct KzcVector4* out_vector)
{
    kzcInterpolateBezierQuadraticVector(4, point1->data, controlPoint->data, point2->data, t, out_vector->data);
}

kzFloat kzcInterpolateBezierCubic(kzFloat point1, kzFloat controlPoint1, kzFloat controlPoint2, kzFloat point2, kzFloat t)
{
    /* B(t) = A*P0 + B*P1 + C*P2 + D*P3, where
         P0 = point1
         P1 = controlPoint1
         P2 = controlPoint2
         P2 = point2
          A = (1 - t)^3
          B = 3 * (1 - t)^2 * t
          C = 3 * (1 - t) * t^2
          C = t^3 */
    kzFloat clampedT = kzcInterpolateClamp_internal(t);
    kzFloat oneMinusT = (1.0f - clampedT);
    kzFloat A = oneMinusT * oneMinusT * oneMinusT;
    kzFloat B = 3 * oneMinusT * oneMinusT * clampedT;
    kzFloat C = 3 * oneMinusT * clampedT * clampedT;
    kzFloat D = clampedT * clampedT * clampedT;

    return A * point1 + B * controlPoint1 + C * controlPoint2 + D * point2;
}

void kzcInterpolateBezierCubicVector(kzUint dimension, const kzFloat* point1, const kzFloat* controlPoint1,
                                     const kzFloat* controlPoint2, const kzFloat* point2, kzFloat t, kzFloat* out_point)
{
    kzFloat clampedT = kzcInterpolateClamp_internal(t);
    kzFloat oneMinusT = (1.0f - clampedT);
    kzFloat A = oneMinusT * oneMinusT * oneMinusT;
    kzFloat B = 3 * oneMinusT * oneMinusT * clampedT;
    kzFloat C = 3 * oneMinusT * clampedT * clampedT;
    kzFloat D = clampedT * clampedT * clampedT;
    kzUint i;

    for (i = 0; i < dimension; ++i)
    {
        out_point[i] = A * point1[i] + B * controlPoint1[i] + C * controlPoint2[i] + D * point2[i];
    }
}

void kzcInterpolateBezierCubicVector2(const struct KzcVector2* const point1, const struct KzcVector2* const controlPoint1,
                                      const struct KzcVector2* const controlPoint2, const struct KzcVector2* const point2,
                                      kzFloat t, struct KzcVector2* out_vector)
{
    kzcInterpolateBezierCubicVector(2, point1->data, controlPoint1->data, controlPoint2->data, point2->data, t, out_vector->data);
}

void kzcInterpolateBezierCubicVector3(const struct KzcVector3* const point1, const struct KzcVector3* const controlPoint1,
                                      const struct KzcVector3* const controlPoint2, const struct KzcVector3* const point2,
                                      kzFloat t, struct KzcVector3* out_vector)
{
    kzcInterpolateBezierCubicVector(3, point1->data, controlPoint1->data, controlPoint2->data, point2->data, t, out_vector->data);
}

void kzcInterpolateBezierCubicVector4(const struct KzcVector4* const point1, const struct KzcVector4* const controlPoint1,
                                      const struct KzcVector4* const controlPoint2, const struct KzcVector4* const point2,
                                      kzFloat t, struct KzcVector4* out_vector)
{
    kzcInterpolateBezierCubicVector(4, point1->data, controlPoint1->data, controlPoint2->data, point2->data, t, out_vector->data);
}

kzFloat kzcInterpolateHermite(kzFloat value1, kzFloat derivative1, kzFloat value2, kzFloat derivative2, kzFloat t)
{
    kzFloat clampedT = kzcInterpolateClamp_internal(t);
    kzFloat clampedT2 = clampedT * clampedT;
    kzFloat clampedT3 = clampedT2 * clampedT;
    kzFloat h00 = 2.0f * clampedT3 - 3.0f * clampedT2 + 1.0f;
    kzFloat h10 = clampedT3 - 2.0f * clampedT2 + clampedT;
    kzFloat h01 = -2.0f * clampedT3 + 3.0f * clampedT2;
    kzFloat h11 = clampedT3 - clampedT2;
    return h00 * value1 + h10 * derivative1 + h01 * value2 + h11 * derivative2;
}

void kzcInterpolateHermiteVector(kzUint dimension, const kzFloat* point1, const kzFloat* derivative1,
                                 const kzFloat* point2, const kzFloat* derivative2, kzFloat t, kzFloat* out_point)
{
    kzFloat clampedT = kzcInterpolateClamp_internal(t);
    kzFloat clampedT2 = clampedT * clampedT;
    kzFloat clampedT3 = clampedT2 * clampedT;
    kzFloat h00 = 2.0f * clampedT3 - 3.0f * clampedT2 + 1;
    kzFloat h10 = clampedT3 - 2.0f * clampedT2 + clampedT;
    kzFloat h01 = -2.0f * clampedT3 + 3.0f * clampedT2;
    kzFloat h11 = clampedT3 - clampedT2;
    kzUint i;
    for (i = 0; i < dimension; ++i)
    {
        out_point[i] = h00 * point1[i] + h10 * derivative1[i] + h01 * point2[i] + h11 * derivative2[i];
    }
}

void kzcInterpolateHermiteVector2(const struct KzcVector2* point1, const struct KzcVector2* derivative1,
                                  const struct KzcVector2* point2, const struct KzcVector2* derivative2,
                                  kzFloat t, struct KzcVector2* out_vector)
{
    kzcInterpolateHermiteVector(2, point1->data, derivative1->data, point2->data, derivative2->data, t, out_vector->data);
}

void kzcInterpolateHermiteVector3(const struct KzcVector3* point1, const struct KzcVector3* derivative1,
                                  const struct KzcVector3* point2, const struct KzcVector3* derivative2,
                                  kzFloat t, struct KzcVector3* out_vector)
{
    kzcInterpolateHermiteVector(3, point1->data, derivative1->data, point2->data, derivative2->data, t, out_vector->data);
}

void kzcInterpolateHermiteVector4(const struct KzcVector4* point1, const struct KzcVector4* derivative1,
                                  const struct KzcVector4* point2, const struct KzcVector4* derivative2,
                                  kzFloat t, struct KzcVector4* out_vector)
{
    kzcInterpolateHermiteVector(4, point1->data, derivative1->data, point2->data, derivative2->data, t, out_vector->data);
}

kzFloat kzcInterpolatePolynomial(kzUint degree, const kzFloat* coefficients, kzFloat t)
{
    kzFloat clampedT = kzcInterpolateClamp_internal(t);
    kzFloat clampedTn = 1.0f;
    kzFloat sum = 0.0f;
    kzUint i;
    for (i = degree + 1; i > 0; --i)
    {
        sum += coefficients[i - 1] * clampedTn;
        clampedTn *= clampedT;
    }
    return sum;
}

void kzcInterpolatePolynomialVector(kzUint dimension, kzUint degree, const kzFloat* coefficients, kzFloat t, kzFloat* out_point)
{
    kzFloat clampedT = kzcInterpolateClamp_internal(t);
    kzFloat clampedTn = 1.0f;
    kzUint offset;
    kzUint i;
    kzUint j;
    for (j = 0; j < dimension; ++j)
    {
        out_point[j] = 0.0f;
    }
    for (i = degree + 1, offset = (degree + 1) * dimension - 1; i > 0; --i)
    {
        for (j = dimension; j > 0; --j, --offset)
        {
            out_point[j - 1] += coefficients[offset] * clampedTn;
        }
        clampedTn *= clampedT;
    }
}

kzFloat kzcInterpolateSmoothStep(kzFloat edge1, kzFloat edge2, kzFloat t)
{
    kzFloat clampedT = kzcInterpolateClamp_internal((t - edge1) / (edge2 - edge1));
    return clampedT * clampedT * (3.0f - 2.0f * clampedT);
}

kzFloat kzcInterpolateAngle(kzFloat angle1, kzFloat angle2, kzFloat t)
{
    kzFloat result;
    kzFloat tClamped = kzsClampf(t, 0.0f, 1.0f);

    if ((angle1 > angle2 && (angle1 - angle2) < 0.5f) ||
        (angle1 < angle2 && ((angle1 + 0.5f - angle2)) < 0.5f))
    {
        result = angle1 + (angle2 - angle1) * tClamped;
    }
    else
    {
        if(angle1 > angle2)
        {
            result = angle1 + (1.0f + (angle2 - angle1)) * tClamped;
        }
        else
        {
            result = angle1 - (1.0f - (angle2 - angle1)) * tClamped;
        }
    }
    if(result > 1.0f)
    {
        result -= 1.0f;
    }
    if(result < 0.0f)
    {
        result += 1.0f;
    }

    return result;
}
