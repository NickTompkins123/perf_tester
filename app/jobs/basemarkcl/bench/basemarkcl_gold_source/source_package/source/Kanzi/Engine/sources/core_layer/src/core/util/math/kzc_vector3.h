/**
* \file
* 3D Vector.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_VECTOR3_H
#define KZC_VECTOR3_H


#include "kzc_vector4.h"

#include <system/kzs_types.h>
#include <system/wrappers/kzs_math.h>


/** Structure of 3D vector. */
struct KzcVector3
{
    kzFloat data[3]; /**< [X,Y,Z] components. */
};


/** Constant zero 3D vector. */
extern const struct KzcVector3 KZC_VECTOR3_ZERO;
/** Constant unit 3D vector for X axis. */
extern const struct KzcVector3 KZC_VECTOR3_X_AXIS;
/** Constant unit 3D vector for Y axis. */
extern const struct KzcVector3 KZC_VECTOR3_Y_AXIS;
/** Constant unit 3D vector for Z axis. */
extern const struct KzcVector3 KZC_VECTOR3_Z_AXIS;
/** Constant unit 3D vector for negative X axis. */
extern const struct KzcVector3 KZC_VECTOR3_X_AXIS_NEGATIVE;
/** Constant unit 3D vector for negative Y axis. */
extern const struct KzcVector3 KZC_VECTOR3_Y_AXIS_NEGATIVE;
/** Constant unit 3D vector for negative Z axis. */
extern const struct KzcVector3 KZC_VECTOR3_Z_AXIS_NEGATIVE;
/** Constant 3D vector with each field at maximum. */
extern const struct KzcVector3 KZC_VECTOR3_MAXIMUM;
/** Constant 3D vector with each field at minimum. */
extern const struct KzcVector3 KZC_VECTOR3_MINIMUM;

/** Creates a 3D vector. */
KZ_INLINE struct KzcVector3 kzcVector3(kzFloat x, kzFloat y, kzFloat z)
{
    struct KzcVector3 vector;

    vector.data[0] = x;
    vector.data[1] = y;
    vector.data[2] = z;

    return vector;
}

/** Gets the X component of a 3D vector. */
KZ_INLINE kzFloat kzcVector3GetX(const struct KzcVector3* vector)
{
    return vector->data[0];
}

/** Gets the Y component of a 3D vector. */
KZ_INLINE kzFloat kzcVector3GetY(const struct KzcVector3* vector)
{
    return vector->data[1];
}

/** Gets the Z component of a 3D vector. */
KZ_INLINE kzFloat kzcVector3GetZ(const struct KzcVector3* vector)
{
    return vector->data[2];
}

/** Gets the underlying component array of a 3D vector. */
KZ_INLINE kzFloat* kzcVector3GetComponents(struct KzcVector3* vector)
{
    return vector->data;
}

/** Sets values for 3D vector. */
KZ_INLINE void kzcVector3Set(struct KzcVector3* vector, kzFloat x, kzFloat y, kzFloat z)
{
    vector->data[0] = x;
    vector->data[1] = y;
    vector->data[2] = z;
}

/** Sets X component for 3D vector. */
KZ_INLINE void kzcVector3SetX(struct KzcVector3* vector, kzFloat x)
{
    vector->data[0] = x;
}

/** Sets Y component for 3D vector. */
KZ_INLINE void kzcVector3SetY(struct KzcVector3* vector, kzFloat y)
{
    vector->data[1] = y;
}

/** Sets Z component for 3D vector. */
KZ_INLINE void kzcVector3SetZ(struct KzcVector3* vector, kzFloat z)
{
    vector->data[2] = z;
}

/** Adds two 3D vectors. */
KZ_INLINE void kzcVector3Add(const struct KzcVector3* first, const struct KzcVector3* second, struct KzcVector3* targetVector)
{
    kzcVector3Set(targetVector,
                  first->data[0] + second->data[0],
                  first->data[1] + second->data[1],
                  first->data[2] + second->data[2]);
}

/** Subtracts two 3D vectors. */
KZ_INLINE void kzcVector3Subtract(const struct KzcVector3* first, const struct KzcVector3* second, struct KzcVector3* targetVector)
{
    kzcVector3Set(targetVector,
                  first->data[0] - second->data[0],
                  first->data[1] - second->data[1],
                  first->data[2] - second->data[2]);
}

/** Scales 3D vector. */
KZ_INLINE void kzcVector3Scale(const struct KzcVector3* vector, kzFloat factor, struct KzcVector3* targetVector)
{
    kzcVector3Set(targetVector,
                  vector->data[0] * factor,
                  vector->data[1] * factor,
                  vector->data[2] * factor);
}

/** Negates 3D vector. */
KZ_INLINE void kzcVector3Negate(const struct KzcVector3* vector, struct KzcVector3* targetVector)
{
    kzcVector3Set(targetVector,
                  -vector->data[0],
                  -vector->data[1],
                  -vector->data[2]);
}

/** Calculates dot product of two 3D vectors. */
KZ_INLINE kzFloat kzcVector3Dot(const struct KzcVector3* first, const struct KzcVector3* second)
{
    return (first->data[0] * second->data[0]) +
           (first->data[1] * second->data[1]) +
           (first->data[2] * second->data[2]);
}

/** Returns length squared of a 3D vector. */
KZ_INLINE kzFloat kzcVector3GetLengthSquared(const struct KzcVector3* vector)
{
    return kzcVector3Dot(vector, vector);
}

/** Returns length of a 3D vector. */
KZ_INLINE kzFloat kzcVector3GetLength(const struct KzcVector3* vector)
{
    return kzsSqrtf(kzcVector3GetLengthSquared(vector));
}

/** Returns the distance between two 3D point vectors. */
KZ_INLINE kzFloat kzcVector3Distance(const struct KzcVector3* vector1, const struct KzcVector3* vector2)
{
    struct KzcVector3 delta;
    kzcVector3Subtract(vector2, vector1, &delta);
    return kzcVector3GetLength(&delta);
}

/** Normalizes a 3D vector. */
KZ_INLINE void kzcVector3Normalize(struct KzcVector3* vector)
{
    kzFloat vectorLength = kzcVector3GetLength(vector);
    /* Zero-vectors can not be normalized. */
    if(kzsFloatIsEqual(vectorLength, 0.0f))
    {
        kzcVector3Set(vector, KZS_NAN, KZS_NAN, KZS_NAN);
    }
    else
    {
        /* Normalize the components */
        kzcVector3Scale(vector, 1.0f / vectorLength, vector);
    }
}

/** Projects vector B onto vector A. */
KZ_INLINE void kzcVector3ProjectVector3(const struct KzcVector3* vectorA, const struct KzcVector3* vectorB, struct KzcVector3* result)
{
    kzFloat dot = kzcVector3Dot(vectorA, vectorB);
    kzcVector3Scale(vectorA, dot, result);
}

/** Reflects a 3D vector according to given axis vector. Axis has to be normalized. */
KZ_INLINE void kzcVector3Reflect(const struct KzcVector3* vector,  const struct KzcVector3* axis, struct KzcVector3* targetVector)
{
    kzFloat dot = 2.0f * kzcVector3Dot(vector, axis);

    kzcVector3Set(targetVector,
                  axis->data[0] * dot - vector->data[0],
                  axis->data[1] * dot - vector->data[1],
                  axis->data[2] * dot - vector->data[2]);
}

/** Checks if two vectors are equal. */
KZ_INLINE kzBool kzcVector3IsEqual(const struct KzcVector3* first, const struct KzcVector3* second)
{
    return kzsFloatIsEqual(first->data[0], second->data[0]) &&
           kzsFloatIsEqual(first->data[1], second->data[1]) &&
           kzsFloatIsEqual(first->data[2], second->data[2]);
}

/** Converts a 3D vector to a 4D vector. W will be zero. */
KZ_INLINE struct KzcVector4 kzcVector3ToVector4(const struct KzcVector3* vector)
{
    return kzcVector4(vector->data[0], vector->data[1], vector->data[2], 0.0f);
}

/** Converts a 4D vector to a 3D vector. W will be discarded. */
KZ_INLINE struct KzcVector3 kzcVector3FromVector4(const struct KzcVector4* vector)
{
    return kzcVector3(vector->data[0], vector->data[1], vector->data[2]);
}

/** Calculates cross product of two 3D vectors. */
KZ_INLINE void kzcVector3Cross(const struct KzcVector3* left, const struct KzcVector3* right, struct KzcVector3* targetVector)
{
    kzcVector3Set(targetVector,
                  left->data[1] * right->data[2] - left->data[2] * right->data[1],
                  left->data[2] * right->data[0] - left->data[0] * right->data[2],
                  left->data[0] * right->data[1] - left->data[1] * right->data[0]);
}

/** Returns the component maximum of two vectors. */
KZ_INLINE struct KzcVector3 kzcVector3ComponentMax(const struct KzcVector3* vectorA, const struct KzcVector3* vectorB)
{
    return kzcVector3(kzsMaxf(kzcVector3GetX(vectorA), kzcVector3GetX(vectorB)),
        kzsMaxf(kzcVector3GetY(vectorA), kzcVector3GetY(vectorB)),
        kzsMaxf(kzcVector3GetZ(vectorA), kzcVector3GetZ(vectorB)));
}

/** Returns the component minimum of two vectors. */
KZ_INLINE struct KzcVector3 kzcVector3ComponentMin(const struct KzcVector3* vectorA, const struct KzcVector3* vectorB)
{
    return kzcVector3(kzsMinf(kzcVector3GetX(vectorA), kzcVector3GetX(vectorB)),
        kzsMinf(kzcVector3GetY(vectorA), kzcVector3GetY(vectorB)),
        kzsMinf(kzcVector3GetZ(vectorA), kzcVector3GetZ(vectorB)));
}

/** Sets the vector components to NaN. */
void kzcVector3SetInvalid(struct KzcVector3* vector);


#endif
