/**
* \file
* Spline curves
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_SPLINE_H
#define KZC_SPLINE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzcVector2;
struct KzcVector3;
struct KzcVector4;


/**
 * \struct KzcSpline
 * Spline curve. Used for smooth interpolation between arbitrary control points.
 */
struct KzcSpline;

/**
 * \struct KzcSplineTracker
 * Spline curve tracker. Keeps track of location in a spline curve.
 */
struct KzcSplineTracker;


/**
 * Creates an arbitrary spline with given control points.
 *
 * \param degree Degree of the polynomial.
 * \param dimension Dimension of the point vectors, i.e. the number of components in the point array.
 * \param pointCount Number of control points.
 * \param controlPoints Control points as polynomial coefficients, where the highest order coefficient is the first element.
 *        This is an array of polynomials, where each polynomial is an array of coefficients, where each coefficient is a vector.
 *        This 3-dimensional array is given as flattened to 1-dimensional: controlPoints[numPoints][degree][dimension] -> controlPoints[numPoints * degree * dimension].
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreate(const struct KzcMemoryManager* memoryManager, kzUint dimension, kzUint degree, kzUint pointCount,
                         kzFloat* controlPoints, kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Hermite spline with given control points.
 * Hermite spline is a cubic spline where each control point is defined with position and tangent vector.
 *
 * \param dimension Dimension of the point vectors, i.e. the number of components in the point array.
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as an array of arrays, where each inner array represents a vector.
 * \param tangents Tangent vectors of control points as an array of arrays, where each inner array represents a vector.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateHermite(const struct KzcMemoryManager* memoryManager, kzUint dimension, kzUint pointCount,
                                const kzFloat* const* points, const kzFloat* const* tangents, kzBool loop,
                                struct KzcSpline** out_spline);

/**
 * Creates a Hermite spline with given control points of 2-dimensional vectors.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vectors.
 * \param tangents Tangent vectors of control points as array of vectors.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateHermiteVector2(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                       const struct KzcVector2* points, const struct KzcVector2* tangents,
                                       kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Hermite spline with given control points of 2-dimensional vector pointers.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vector pointers.
 * \param tangents Tangent vectors of control points as array of vector pointers.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateHermiteVector2Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                               const struct KzcVector2* const* points, const struct KzcVector2* const* tangents,
                                               kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Hermite spline with given control points of 3-dimensional vectors.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vectors.
 * \param tangents Tangent vectors of control points as array of vectors.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateHermiteVector3(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                       const struct KzcVector3* points, const struct KzcVector3* tangents,
                                       kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Hermite spline with given control points of 3-dimensional vector pointers.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vector pointers.
 * \param tangents Tangent vectors of control points as array of vector pointers.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateHermiteVector3Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                               const struct KzcVector3* const* points, const struct KzcVector3* const* tangents,
                                               kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Hermite spline with given control points of 4-dimensional vectors.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vectors.
 * \param tangents Tangent vectors of control points as array of vectors.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateHermiteVector4(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                       const struct KzcVector4* points, const struct KzcVector4* tangents,
                                       kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Hermite spline with given control points of 4-dimensional vector pointers.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vector pointers.
 * \param tangents Tangent vectors of control points as array of vector pointers.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateHermiteVector4Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                               const struct KzcVector4* const* points, const struct KzcVector4* const* tangents,
                                               kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Catmull-Rom spline with given control points.
 * Catmull-Rom spline is a Hermite spline where tangents are calculated automatically as t_n = (p_n+1 - p_n-1) / 2.
 * If looping is disabled, first and last tangents are calculated simply t_0 = p_1 - p_0 and t_N = p_N - p_N-1.
 *
 * \param dimension Dimension of the point vectors, i.e. the number of components in the point array.
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as an array of arrays, where each inner array represents a vector.
 * \param loop Controls whether the spline is looping as an array of arrays, where each inner array represents a vector.
 */
kzsError kzcSplineCreateCatmullRom(const struct KzcMemoryManager* memoryManager, kzUint dimension, kzUint pointCount,
                                   const kzFloat* const* points, kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Catmull-Rom spline with given control points of 2-dimensional vectors.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vectors.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateCatmullRomVector2(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                          const struct KzcVector2* points, kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Catmull-Rom spline with given control points of 2-dimensional vector pointers.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vector pointers.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateCatmullRomVector2Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                                  const struct KzcVector2* const* points, kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Catmull-Rom spline with given control points of 3-dimensional vectors.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vectors.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateCatmullRomVector3(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                          const struct KzcVector3* points, kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Catmull-Rom spline with given control points of 3-dimensional vector pointers.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vector pointers.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateCatmullRomVector3Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                                  const struct KzcVector3* const* points, kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Catmull-Rom spline with given control points of 4-dimensional vectors.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vectors.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateCatmullRomVector4(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                          const struct KzcVector4* points, kzBool loop, struct KzcSpline** out_spline);

/**
 * Creates a Catmull-Rom spline with given control points of 4-dimensional vector pointers.
 *
 * \param pointCount Number of control points.
 * \param points Location vectors of control points as array of vector pointers.
 * \param loop Controls whether the spline is looping.
 */
kzsError kzcSplineCreateCatmullRomVector4Pointers(const struct KzcMemoryManager* memoryManager, kzUint pointCount,
                                                  const struct KzcVector4* const* points, kzBool loop, struct KzcSpline** out_spline);


/** Frees the memory allocated for spline. */
kzsError kzcSplineDelete(struct KzcSpline* spline);


/**
 * Takes ownership of the spline container deletion. After this, when a spline is deleted,
 * only the data is actually deleted and the container remains alive.
 */
void kzcSplineTakeOwnership(const struct KzcSpline* spline);
/** Checks if the spline is valid (pointer and the data are non-null). */
kzBool kzcSplineIsValid(const struct KzcSpline* spline);
/** Moves the data from source spline to the target spline. */
void kzcSplineTransferData(struct KzcSpline* targetSpline, struct KzcSpline* sourceSpline);


/**
 * Returns the length of the spline. This is the number of control points or possibly one less if looping is disabled.
 * Each spline segment (part between two consecutive control points) has a length of one.
 */
kzUint kzcSplineGetLength(const struct KzcSpline* spline);

/**
 * Evaluates the value of the spline in the given location t.
 * Integer part of t defines the segment index and decimal part of t defines the location in that segment.
 */
void kzcSplineGetPoint(const struct KzcSpline* spline, kzFloat t, kzFloat* out_point);


/** Creates a new spline tracker with the given spline curve. */
kzsError kzcSplineTrackerCreate(const struct KzcMemoryManager* memoryManager, const struct KzcSpline* spline,
                                struct KzcSplineTracker** out_splineTracker);

/** Frees the memory allocated for spline tracker. */
kzsError kzcSplineTrackerDelete(struct KzcSplineTracker* splineTracker);

/** Returns the t location of the given spline tracker. */
kzFloat kzcSplineTrackerGetT(const struct KzcSplineTracker* splineTracker);

/** Moves given amount (forward or backwards) in the given spline tracker. */
void kzcSplineTrackerMove(struct KzcSplineTracker* splineTracker, kzFloat deltaT);

/** Evaluates the value of the spline tracker in the current location. */
void kzcSplineTrackerGetPoint(const struct KzcSplineTracker* splineTracker, kzFloat* out_point);


#endif
