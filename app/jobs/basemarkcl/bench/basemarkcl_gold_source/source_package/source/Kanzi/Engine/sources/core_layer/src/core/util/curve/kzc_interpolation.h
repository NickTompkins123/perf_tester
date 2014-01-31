/**
* \file
* Interpolation functions
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_INTERPOLATION_H
#define KZC_INTERPOLATION_H


#include <system/kzs_types.h>


/* Forward declarations */
struct KzcVector2;
struct KzcVector3;
struct KzcVector4;


/** Returns linearly interpolated value between given two values. t is clamped in the range [0,1]. */
kzFloat kzcInterpolateLinear(kzFloat value1, kzFloat value2, kzFloat t);

/** Returns linearly interpolated point between given two n-dimensional points. t is clamped in the range [0,1]. */
void kzcInterpolateLinearVector(kzUint dimension, const kzFloat* point1, const kzFloat* point2, kzFloat t, kzFloat* out_point);

/** Returns linearly interpolated vector between given two 2-dimensional vectors. t is clamped in the range [0,1]. */
void kzcInterpolateLinearVector2(const struct KzcVector2* const point1, const struct KzcVector2* const point2,
                                 kzFloat t, struct KzcVector2* out_vector);

/** Returns linearly interpolated vector between given two 3-dimensional vectors. t is clamped in the range [0,1]. */
void kzcInterpolateLinearVector3(const struct KzcVector3* const point1, const struct KzcVector3* const point2,
                                 kzFloat t, struct KzcVector3* out_vector);

/** Returns linearly interpolated vector between given two 4-dimensional vectors. t is clamped in the range [0,1]. */
void kzcInterpolateLinearVector4(const struct KzcVector4* const point1, const struct KzcVector4* const point2,
                                 kzFloat t, struct KzcVector4* out_vector);


/** Returns arbitrary degree bezier interpolated value from given points. First and last points are end points, all other are control points. */
kzFloat kzcInterpolateBezier(kzUint degree, kzFloat* points, kzFloat t);

/** Returns arbitrary degree bezier interpolated point from given n-dimensional points. */
void kzcInterpolateBezierVector(kzUint dimension, kzUint degree, const kzFloat** points, kzFloat t, kzFloat* out_point);

/** Returns quadratic bezier interpolated value from given points. */
kzFloat kzcInterpolateBezierQuadratic(kzFloat point1, kzFloat controlPoint, kzFloat point2, kzFloat t);

/** Returns quadratic bezier interpolated point from given n-dimensional points. */
void kzcInterpolateBezierQuadraticVector(kzUint dimension, const kzFloat* point1, const kzFloat* controlPoint,
                                         const kzFloat* point2, kzFloat t, kzFloat* out_point);

/** Returns quadratic bezier interpolated vector from given 2-dimensional vectors. */
void kzcInterpolateBezierQuadraticVector2(const struct KzcVector2* const point1, const struct KzcVector2* const controlPoint,
                                          const struct KzcVector2* const point2, kzFloat t, struct KzcVector2* out_vector);

/** Returns quadratic bezier interpolated vector from given 3-dimensional vectors. */
void kzcInterpolateBezierQuadraticVector3(const struct KzcVector3* const startPoint, const struct KzcVector3* const controlPoint,
                                          const struct KzcVector3* const endPoint, kzFloat t, struct KzcVector3* out_vector);

/** Returns quadratic bezier interpolated vector from given 4-dimensional vectors. */
void kzcInterpolateBezierQuadraticVector4(const struct KzcVector4* const point1, const struct KzcVector4* const controlPoint,
                                          const struct KzcVector4* const point2, kzFloat t, struct KzcVector4* out_vector);

/** Returns cubic bezier interpolated value from given points. */
kzFloat kzcInterpolateBezierCubic(kzFloat point1, kzFloat controlPoint1, kzFloat controlPoint2, kzFloat point2, kzFloat t);

/** Returns cubic bezier interpolated point from given n-dimensional points. */
void kzcInterpolateBezierCubicVector(kzUint dimension, const kzFloat* point1, const kzFloat* controlPoint1,
                                     const kzFloat* controlPoint2, const kzFloat* point2, kzFloat t, kzFloat* out_point);

/** Returns cubic bezier interpolated vector from given 2-dimensional vectors. */
void kzcInterpolateBezierCubicVector2(const struct KzcVector2* const point1, const struct KzcVector2* const controlPoint1,
                                      const struct KzcVector2* const controlPoint2, const struct KzcVector2* const point2,
                                      kzFloat t, struct KzcVector2* out_vector);

/** Returns cubic bezier interpolated vector from given 3-dimensional vectors. */
void kzcInterpolateBezierCubicVector3(const struct KzcVector3* const point1, const struct KzcVector3* const controlPoint1,
                                      const struct KzcVector3* const controlPoint2, const struct KzcVector3* const point2,
                                      kzFloat t, struct KzcVector3* out_vector);

/** Returns cubic bezier interpolated vector from given 4-dimensional vectors. */
void kzcInterpolateBezierCubicVector4(const struct KzcVector4* const point1, const struct KzcVector4* const controlPoint1,
                                      const struct KzcVector4* const controlPoint2, const struct KzcVector4* const point2,
                                      kzFloat t, struct KzcVector4* out_vector);


/** Returns Hermite interpolated value between given two values and their derivatives. t is clamped in the range [0,1]. */
kzFloat kzcInterpolateHermite(kzFloat value1, kzFloat derivative1, kzFloat value2, kzFloat derivative2, kzFloat t);

/** Returns Hermite interpolated point between given two n-dimensional points and their derivatives. t is clamped in the range [0,1]. */
void kzcInterpolateHermiteVector(kzUint dimension, const kzFloat* point1, const kzFloat* derivative1, const kzFloat* point2, const kzFloat* derivative2, kzFloat t, kzFloat* out_point);

/** Returns Hermite interpolated vector between given two 2-dimensional vectors and their derivatives. t is clamped in the range [0,1]. */
void kzcInterpolateHermiteVector2(const struct KzcVector2* point1, const struct KzcVector2* derivative1,
                                  const struct KzcVector2* point2, const struct KzcVector2* derivative2,
                                  kzFloat t, struct KzcVector2* out_vector);
/** Returns Hermite interpolated vector between given two 3-dimensional vectors and their derivatives. t is clamped in the range [0,1]. */
void kzcInterpolateHermiteVector3(const struct KzcVector3* point1, const struct KzcVector3* derivative1,
                                  const struct KzcVector3* point2, const struct KzcVector3* derivative2,
                                  kzFloat t, struct KzcVector3* out_vector);
/** Returns Hermite interpolated vector between given two 4-dimensional vectors and their derivatives. t is clamped in the range [0,1]. */
void kzcInterpolateHermiteVector4(const struct KzcVector4* point1, const struct KzcVector4* derivative1,
                                  const struct KzcVector4* point2, const struct KzcVector4* derivative2,
                                  kzFloat t, struct KzcVector4* out_vector);


/** Returns evaluated value from the given polynomial. t is clamped in the range [0,1]. */
kzFloat kzcInterpolatePolynomial(kzUint degree, const kzFloat* coefficients, kzFloat t);

/**
 * Returns evaluated point from the given n-dimensional polynomial. t is clamped in the range [0,1].
 * \param coefficients 2-dimensional array flattened to 1-dimensional array. coefficients[degree][dimension] -> coefficients[degree * dimension]
 */
void kzcInterpolatePolynomialVector(kzUint dimension, kzUint degree, const kzFloat* coefficients, kzFloat t, kzFloat* out_point);

/**
 * Returns smoothly interpolated value in the range [0,1] between given edges.
 * The smoothing is calculated as specified in the OpenGL Shading Language specification.
 * edge1 must be less than edge2.
 * Value of the function is 0 if t <= edge1 and 1 if t >= edge2.
 * If edge1 < t < edge2, the value is interpolated smoothly between edge1 and edge2 in a way
 * that the derivative of the function is f'(edge1) == f'(edge2) == 0.
 */
kzFloat kzcInterpolateSmoothStep(kzFloat edge1, kzFloat edge2, kzFloat t);

/** Interpolates 2 angles, t between [0, 1]. Returns interpolated angle. */
kzFloat kzcInterpolateAngle(kzFloat angle1, kzFloat angle2, kzFloat t);


#endif
