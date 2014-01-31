/**
* \file
* 2D vector.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_VECTOR2_H
#define KZC_VECTOR2_H


#include "kzc_vector3.h"

#include <system/kzs_types.h>
#include <system/wrappers/kzs_math.h>


/** Structure of 2D vector. */
struct KzcVector2
{
    kzFloat data[2]; /**< [X,Y] components. */
};


/** Constant zero 2D vector. */
extern const struct KzcVector2 KZC_VECTOR2_ZERO;
/** Constant unit 2D vector for X axis. */
extern const struct KzcVector2 KZC_VECTOR2_X_AXIS;
/** Constant unit 2D vector for Y axis. */
extern const struct KzcVector2 KZC_VECTOR2_Y_AXIS;


/** Creates a 2D vector. */
KZ_INLINE struct KzcVector2 kzcVector2(kzFloat x, kzFloat y)
{
    struct KzcVector2 vector;

    vector.data[0] = x;
    vector.data[1] = y;

    return vector;
}

/** Gets the X component of a 2D vector. */
KZ_INLINE kzFloat kzcVector2GetX(const struct KzcVector2* vector)
{
    return vector->data[0];
}

/** Gets the Y component of a 2D vector. */
KZ_INLINE kzFloat kzcVector2GetY(const struct KzcVector2* vector)
{
    return vector->data[1];
}

/** Gets the underlying component array of a 2D vector. */
KZ_INLINE kzFloat* kzcVector2GetComponents(struct KzcVector2* vector)
{
    return vector->data;
}

/** Sets all components for 2D vector. */
KZ_INLINE void kzcVector2Set(struct KzcVector2* vector, kzFloat x, kzFloat y)
{
    vector->data[0] = x;
    vector->data[1] = y;
}

/** Sets X component for 2D vector. */
KZ_INLINE void kzcVector2SetX(struct KzcVector2* vector, kzFloat x)
{
    vector->data[0] = x;
}

/** Sets Y component for 2D vector. */
KZ_INLINE void kzcVector2SetY(struct KzcVector2* vector, kzFloat y)
{
    vector->data[1] = y;
}

/** Adds two 2D vectors. */
KZ_INLINE void kzcVector2Add(const struct KzcVector2* first, const struct KzcVector2* second, struct KzcVector2* targetVector)
{
    targetVector->data[0] = first->data[0] + second->data[0];
    targetVector->data[1] = first->data[1] + second->data[1];
}

/** Subtracts two 2D vectors. */
KZ_INLINE void kzcVector2Subtract(const struct KzcVector2* first, const struct KzcVector2* second, struct KzcVector2* targetVector)
{
    targetVector->data[0] = first->data[0] - second->data[0];
    targetVector->data[1] = first->data[1] - second->data[1];
}

/** Scales 2D vector. */
KZ_INLINE void kzcVector2Scale(const struct KzcVector2* vector, kzFloat factor, struct KzcVector2* out_vector)
{
    out_vector->data[0] = vector->data[0] * factor;
    out_vector->data[1] = vector->data[1] * factor;
}

/** Negates 2D vector. */
KZ_INLINE void kzcVector2Negate(const struct KzcVector2* vector, struct KzcVector2* targetVector)
{
    targetVector->data[0] = -vector->data[0];
    targetVector->data[1] = -vector->data[1];
}

/** Calculates dot product of two 2D vectors. */
KZ_INLINE kzFloat kzcVector2Dot(const struct KzcVector2* left, const struct KzcVector2* right)
{
    return (left->data[0] * right->data[0]) +
           (left->data[1] * right->data[1]);
}

/** Returns length squared of a 2D vector. */
KZ_INLINE kzFloat kzcVector2GetLengthSquared(const struct KzcVector2* vector)
{
    return kzcVector2Dot(vector, vector);
}

/** Returns the length of 2D vector. */
KZ_INLINE kzFloat kzcVector2GetLength(const struct KzcVector2* vector)
{
    return kzsSqrtf(kzcVector2GetLengthSquared(vector));
}

/** Returns the distance between two 2D point vectors. */
KZ_INLINE kzFloat kzcVector2Distance(const struct KzcVector2* vector1, const struct KzcVector2* vector2)
{
    struct KzcVector2 delta;
    kzcVector2Subtract(vector2, vector1, &delta);
    return kzcVector2GetLength(&delta);
}

/** Normalizes a 2D vector. */
KZ_INLINE void kzcVector2Normalize(struct KzcVector2* vector)
{
    kzFloat vectorLength = kzcVector2GetLength(vector);
    /* Zero-vectors can not be normalized. */
    if(kzsFloatIsEqual(vectorLength, 0.0f))
    {
        kzcVector2Set(vector, KZS_NAN, KZS_NAN);
    }
    else
    {
        /* Normalize the components */
        kzcVector2Scale(vector, 1.0f  / vectorLength, vector);
    }
}

/** Reflects a 2D vector according to given axis vector. Axis has to be normalized. */
KZ_INLINE void kzcVector2Reflect(const struct KzcVector2* vector,  const struct KzcVector2* axis, struct KzcVector2* targetVector)
{
    kzFloat dot = 2.0f * kzcVector2Dot(vector, axis);

    kzcVector2Set(targetVector,
                  axis->data[0] * dot - vector->data[0],
                  axis->data[1] * dot - vector->data[1]);
}

/** Checks if two 2D vectors are equal. */
KZ_INLINE kzBool kzcVector2IsEqual(const struct KzcVector2* first, const struct KzcVector2* second)
{
    return kzsFloatIsEqual(first->data[0], second->data[0]) &&
           kzsFloatIsEqual(first->data[1], second->data[1]);
}

/** Converts a 2D vector to a 3D vector. Z will be zero. */
KZ_INLINE struct KzcVector3 kzcVector2ToVector3(const struct KzcVector2* vector)
{
    return kzcVector3(vector->data[0], vector->data[1], 0.0f);
}

/** Converts a 3D vector to a 2D vector. Z will be discarded. */
KZ_INLINE struct KzcVector2 kzcVector2FromVector3(const struct KzcVector3* vector)
{
    return kzcVector2(vector->data[0], vector->data[1]);
}

/** Returns the component maximum of two vectors. */
KZ_INLINE struct KzcVector2 kzcVector2ComponentMax(const struct KzcVector2* vectorA, const struct KzcVector2* vectorB)
{
    return kzcVector2(kzsMaxf(kzcVector2GetX(vectorA), kzcVector2GetX(vectorB)),
                      kzsMaxf(kzcVector2GetY(vectorA), kzcVector2GetY(vectorB)));
}

/** Returns the component minimum of two vectors. */
KZ_INLINE struct KzcVector2 kzcVector2ComponentMin(const struct KzcVector2* vectorA, const struct KzcVector2* vectorB)
{
    return kzcVector2(kzsMinf(kzcVector2GetX(vectorA), kzcVector2GetX(vectorB)),
                      kzsMinf(kzcVector2GetY(vectorA), kzcVector2GetY(vectorB)));
}

/** Sets the vector components to NaN. */
void kzcVector2SetInvalid(struct KzcVector2* vector);


#endif
