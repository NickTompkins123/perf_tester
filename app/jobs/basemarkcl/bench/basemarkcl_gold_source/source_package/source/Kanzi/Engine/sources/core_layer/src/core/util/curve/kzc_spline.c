/**
* \file
* Spline curves
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_spline.h"

#include "kzc_interpolation.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector4.h>

#include <system/wrappers/kzs_math.h>


/**
 * Spline curve data.
 */
struct KzcSplineData
{
    kzBool selfOwned;

    kzUint dimension; /**< Dimension of the spline, i.e. how many components the control point vectors have. */
    kzUint degree; /**< Degree of the polynom used for the interpolation. */
    kzUint pointCount; /**< Number of control points. */
    /** Spline control points 3-dimensional array flattened to 1-dimensional: controlPoints[numPoints][degree+1][dimension] -> controlPoints[numPoints * (degree+1) * dimension]. */
    kzFloat* controlPoints;
    kzBool loop; /**< Flag to indicate whether this spline loops or not. */
    kzBool releaseControlPoints; /**< Flag to indicate whether the memory of the control points should be freed or not. The status of the flag depends on how the spline was created. */
};


/**
 * Spline curve.
 */
struct KzcSpline
{
    struct KzcSplineData* data; /**< Data for spline. */
};


/**
 * Spline curve tracker. Keeps track of location in a spline curve.
 */
struct KzcSplineTracker
{
    const struct KzcSpline* spline; /**< The spline to track. */
    kzFloat t; /**< Current position in the spline. */
};


kzsError kzcSplineCreate(const struct KzcMemoryManager* memoryManager, kzUint dimension, kzUint degree,
                         kzUint pointCount, kzFloat* controlPoints, kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    struct KzcSpline* spline;
    struct KzcSplineData* splineData;

    result = kzcMemoryAllocVariable(memoryManager, spline, "Spline");
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, splineData, "SplineData");
    kzsErrorForward(result);

    spline->data = splineData;

    splineData->dimension = dimension;
    splineData->degree = degree;
    splineData->pointCount = pointCount;
    splineData->controlPoints = controlPoints;
    splineData->loop = loop;
    splineData->releaseControlPoints = KZ_FALSE;

    splineData->selfOwned = KZ_TRUE;

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateHermite(const struct KzcMemoryManager* memoryManager, kzUint dimension, kzUint pointCount,
                                const kzFloat* const* points, const kzFloat* const* tangents, kzBool loop,
                                struct KzcSpline** out_spline)
{
    kzsError result;
    kzFloat* controlPoints;
    struct KzcSpline* spline;
    const kzUint degree = 3; /* Hermite curves are cubic polynomials. */
    kzUint polynomialCount = (loop ? pointCount : pointCount - 1);
    kzUint offset;
    kzUint i;
    kzUint j;

    result = kzcMemoryAllocArray(memoryManager, controlPoints, polynomialCount * (degree + 1) * dimension,
                                 "Spline control points as polynomials");
    kzsErrorForward(result);

    for (i = 0, offset = 0; i < polynomialCount; ++i, offset += (degree + 1) * dimension)
    {
        kzUint nextIndex = (i == pointCount - 1) ? 0 : i + 1;
        const kzFloat* point = points[i];
        const kzFloat* tangent = tangents[i];
        const kzFloat* nextPoint = points[nextIndex];
        const kzFloat* nextTangent = tangents[nextIndex];
        for (j = 0; j < dimension; j++)
        {
            kzFloat p0 = point[j];
            kzFloat d0 = tangent[j];
            kzFloat p1 = nextPoint[j];
            kzFloat d1 = nextTangent[j];
            kzFloat A =  2.0f * p0 +        d0 - 2.0f * p1 + d1;
            kzFloat B = -3.0f * p0 - 2.0f * d0 + 3.0f * p1 - d1;
            kzFloat C =                     d0                 ;
            kzFloat D =         p0                             ;
            controlPoints[offset                 + j] = A;
            controlPoints[offset + 1 * dimension + j] = B;
            controlPoints[offset + 2 * dimension + j] = C;
            controlPoints[offset + 3 * dimension + j] = D;
        }
    }

    result = kzcSplineCreate(memoryManager, dimension, degree, polynomialCount, controlPoints, loop, &spline);
    kzsErrorForward(result);

    spline->data->releaseControlPoints = KZ_TRUE;

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateHermiteVector2(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                       const struct KzcVector2* points, const struct KzcVector2* tangents,
                                       kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    const kzFloat** tangents2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, tangents2, pointCount, "Spline control tangents array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i].data;
        tangents2[i] = tangents[i].data;
    }

    result = kzcSplineCreateHermite(memoryManager, 2, pointCount, points2, tangents2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)tangents2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateHermiteVector2Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                               const struct KzcVector2* const* points, const struct KzcVector2* const* tangents,
                                               kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    const kzFloat** tangents2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, tangents2, pointCount, "Spline control tangents array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i]->data;
        tangents2[i] = tangents[i]->data;
    }

    result = kzcSplineCreateHermite(memoryManager, 2, pointCount, points2, tangents2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)tangents2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateHermiteVector3(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                       const struct KzcVector3* points, const struct KzcVector3* tangents,
                                       kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    const kzFloat** tangents2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, tangents2, pointCount, "Spline control tangents array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i].data;
        tangents2[i] = tangents[i].data;
    }

    result = kzcSplineCreateHermite(memoryManager, 3, pointCount, points2, tangents2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)tangents2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateHermiteVector3Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                               const struct KzcVector3* const* points, const struct KzcVector3* const* tangents,
                                               kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    const kzFloat** tangents2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, tangents2, pointCount, "Spline control tangents array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i]->data;
        tangents2[i] = tangents[i]->data;
    }

    result = kzcSplineCreateHermite(memoryManager, 3, pointCount, points2, tangents2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)tangents2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateHermiteVector4(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                       const struct KzcVector4* points, const struct KzcVector4* tangents,
                                       kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    const kzFloat** tangents2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, tangents2, pointCount, "Spline control tangents array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i].data;
        tangents2[i] = tangents[i].data;
    }

    result = kzcSplineCreateHermite(memoryManager, 4, pointCount, points2, tangents2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)tangents2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateHermiteVector4Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                               const struct KzcVector4* const* points, const struct KzcVector4* const* tangents,
                                               kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    const kzFloat** tangents2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, tangents2, pointCount, "Spline control tangents array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i]->data;
        tangents2[i] = tangents[i]->data;
    }

    result = kzcSplineCreateHermite(memoryManager, 4, pointCount, points2, tangents2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)tangents2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateCatmullRom(const struct KzcMemoryManager* memoryManager, kzUint dimension, kzUint pointCount,
                                   const kzFloat* const* points, kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** controlTangents;
    kzFloat* controlTangentVectors;
    struct KzcSpline* spline;
    kzUint offset;
    kzUint i;
    kzUint j;

    result = kzcMemoryAllocArray(memoryManager, controlTangents, pointCount, "Spline control tangents");
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(memoryManager, controlTangentVectors, pointCount * dimension, "Spline control tangent vectors");
    kzsErrorForward(result);

    /* Create tangents from control points. */
    for (i = 0, offset = 0; i < pointCount; ++i, offset += dimension)
    {
        /* Tangents are calculated from difference from next and previous control points. */
        const kzFloat* previousPoint;
        const kzFloat* nextPoint;
        kzFloat multiplier = 0.5f;

        /* If the spline is looping, next and previous control points are also looping. */
        if (loop)
        {
            previousPoint = points[(i + pointCount - 1) % pointCount];
            nextPoint     = points[(i + 1) % pointCount];
        }
        /* For non-looping splines handle first and last tangents as special case. */
        else
        {
            if (i == 0)
            {
                previousPoint = points[0];
                nextPoint     = points[1];
                multiplier = 1.0f;
            }
            else if (i == pointCount - 1)
            {
                previousPoint = points[i - 1];
                nextPoint     = points[i];
                multiplier = 1.0f;
            }
            else
            {
                previousPoint = points[i - 1];
                nextPoint     = points[i + 1];
            }
        }

        for (j = 0; j < dimension; ++j)
        {
            controlTangentVectors[offset + j] = (nextPoint[j] - previousPoint[j]) * multiplier;
        }
        controlTangents[i] = &controlTangentVectors[offset];
    }

    result = kzcSplineCreateHermite(memoryManager, dimension, pointCount, points, controlTangents, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray(controlTangentVectors);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)controlTangents);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateCatmullRomVector2(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                          const struct KzcVector2* points, kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i].data;
    }

    result = kzcSplineCreateCatmullRom(memoryManager, 2, pointCount, points2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateCatmullRomVector2Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                                  const struct KzcVector2* const* points, kzBool loop,
                                                  struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i]->data;
    }

    result = kzcSplineCreateCatmullRom(memoryManager, 2, pointCount, points2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateCatmullRomVector3(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                          const struct KzcVector3* points, kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i].data;
    }

    result = kzcSplineCreateCatmullRom(memoryManager, 3, pointCount, points2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateCatmullRomVector3Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                                  const struct KzcVector3* const* points, kzBool loop,
                                                  struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i]->data;
    }

    result = kzcSplineCreateCatmullRom(memoryManager, 3, pointCount, points2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateCatmullRomVector4(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                          const struct KzcVector4* points, kzBool loop, struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i].data;
    }

    result = kzcSplineCreateCatmullRom(memoryManager, 4, pointCount, points2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineCreateCatmullRomVector4Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                                  const struct KzcVector4* const* points, kzBool loop,
                                                  struct KzcSpline** out_spline)
{
    kzsError result;
    const kzFloat** points2;
    struct KzcSpline* spline;
    kzUint i;

    result = kzcMemoryAllocArray(memoryManager, points2, pointCount, "Spline control points array");
    kzsErrorForward(result);

    for (i = 0; i < pointCount; ++i)
    {
        points2[i] = points[i]->data;
    }

    result = kzcSplineCreateCatmullRom(memoryManager, 4, pointCount, points2, loop, &spline);
    kzsErrorForward(result);

    result = kzcMemoryFreeArray((void*)points2);
    kzsErrorForward(result);

    *out_spline = spline;
    kzsSuccess();
}

kzsError kzcSplineDelete(struct KzcSpline* spline)
{
    kzsError result;
    kzBool selfOwned;

    kzsAssert(kzcIsValidPointer(spline));
    selfOwned = spline->data->selfOwned;

    if (spline->data->releaseControlPoints)
    {
        result = kzcMemoryFreeArray(spline->data->controlPoints);
        kzsErrorForward(result);
    }

    result = kzcMemoryFreeVariable(spline->data);
    kzsErrorForward(result);

    if(selfOwned)
    {
        result = kzcMemoryFreeVariable(spline);
        kzsErrorForward(result);
    }
    else
    {
        spline->data = KZ_NULL;
    }

    kzsSuccess();
}

void kzcSplineTakeOwnership(const struct KzcSpline* spline)
{
    kzsAssert(kzcSplineIsValid(spline));

    spline->data->selfOwned = KZ_FALSE;
}

kzBool kzcSplineIsValid(const struct KzcSpline* spline)
{
    return spline != KZ_NULL && spline->data != KZ_NULL;
}

void kzcSplineTransferData(struct KzcSpline* targetSpline, struct KzcSpline* sourceSpline)
{
    kzsAssert(kzcIsValidPointer(targetSpline));
    kzsAssert(targetSpline->data == KZ_NULL);
    kzsAssert(kzcSplineIsValid(sourceSpline));

    targetSpline->data = sourceSpline->data;
    sourceSpline->data = KZ_NULL;
}

kzUint kzcSplineGetLength(const struct KzcSpline* spline)
{
    kzsAssert(kzcIsValidPointer(spline));

    return spline->data->pointCount;
}

void kzcSplineGetPoint(const struct KzcSpline* spline, kzFloat t, kzFloat* out_point)
{
    kzUint segmentIndex;
    kzFloat relativeT;

    kzsAssert(kzcIsValidPointer(spline));

    if (spline->data->loop)
    {
        segmentIndex = kzsSMod(kzsIFloorf(t), spline->data->pointCount);
        relativeT = t - (kzFloat)kzsIFloorf(t);
    }
    else
    {
        if (t >= (kzFloat)(kzInt)spline->data->pointCount)
        {
            segmentIndex = spline->data->pointCount - 1;
            relativeT = 1.0f;
        }
        else if (t < 0)
        {
            segmentIndex = 0;
            relativeT = 0.0f;
        }
        else
        {
            segmentIndex = kzsFloatToUint(t);
            relativeT = t - (kzFloat)(kzInt)segmentIndex;
        }
    }
    kzcInterpolatePolynomialVector(spline->data->dimension, spline->data->degree,
                                   &spline->data->controlPoints[segmentIndex * (spline->data->degree + 1) * spline->data->dimension],
                                   relativeT, out_point);
}

kzsError kzcSplineTrackerCreate(const struct KzcMemoryManager* memoryManager, const struct KzcSpline* spline,
                                struct KzcSplineTracker** out_splineTracker)
{
    kzsError result;
    struct KzcSplineTracker* splineTracker;

    result = kzcMemoryAllocVariable(memoryManager, splineTracker, "Spline tracker");
    kzsErrorForward(result);

    splineTracker->spline = spline;
    splineTracker->t = 0.0f;

    *out_splineTracker = splineTracker;
    kzsSuccess();
}

kzsError kzcSplineTrackerDelete(struct KzcSplineTracker* splineTracker)
{
    kzsError result;

    kzsAssert(kzcIsValidPointer(splineTracker));

    result = kzcMemoryFreeVariable(splineTracker);
    kzsErrorForward(result);

    kzsSuccess();
}

kzFloat kzcSplineTrackerGetT(const struct KzcSplineTracker* splineTracker)
{
    kzsAssert(kzcIsValidPointer(splineTracker));

    return splineTracker->t;
}

void kzcSplineTrackerMove(struct KzcSplineTracker* splineTracker, kzFloat deltaT)
{
    kzsAssert(kzcIsValidPointer(splineTracker));

    splineTracker->t += deltaT;
}

void kzcSplineTrackerGetPoint(const struct KzcSplineTracker* splineTracker, kzFloat* out_point)
{
    kzsAssert(kzcIsValidPointer(splineTracker));

    kzcSplineGetPoint(splineTracker->spline, splineTracker->t, out_point);
}
