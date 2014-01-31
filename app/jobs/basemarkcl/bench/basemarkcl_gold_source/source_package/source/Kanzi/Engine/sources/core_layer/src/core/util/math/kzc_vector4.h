/**
* \file
* 4D vector.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_VECTOR4_H
#define KZC_VECTOR4_H


#include <system/kzs_types.h>
#include <system/wrappers/kzs_math.h>


/** Structure of 4D vector. */
struct KzcVector4
{
    kzFloat data[4]; /**< [X,Y,Z,W] components. */
};


/** Constant zero 4D vector. */
extern const struct KzcVector4 KZC_VECTOR4_ZERO;
/** Constant unit 4D vector for X axis. */
extern const struct KzcVector4 KZC_VECTOR4_X_AXIS;
/** Constant unit 4D vector for Y axis. */
extern const struct KzcVector4 KZC_VECTOR4_Y_AXIS;
/** Constant unit 4D vector for Z axis. */
extern const struct KzcVector4 KZC_VECTOR4_Z_AXIS;


/** Creates a 4D vector. */
KZ_INLINE struct KzcVector4 kzcVector4(kzFloat x, kzFloat y, kzFloat z, kzFloat w)
{
    struct KzcVector4 vector;

    vector.data[0] = x;
    vector.data[1] = y;
    vector.data[2] = z;
    vector.data[3] = w;

    return vector;
}

/** Gets the X component of a 4D vector. */
KZ_INLINE kzFloat kzcVector4GetX(const struct KzcVector4* vector)
{
    return vector->data[0];
}

/** Gets the Y component of a 4D vector. */
KZ_INLINE kzFloat kzcVector4GetY(const struct KzcVector4* vector)
{
    return vector->data[1];
}

/** Gets the Z component of a 4D vector. */
KZ_INLINE kzFloat kzcVector4GetZ(const struct KzcVector4* vector)
{
    return vector->data[2];
}

/** Gets the W component of a 4D vector. */
KZ_INLINE kzFloat kzcVector4GetW(const struct KzcVector4* vector)
{
    return vector->data[3];
}

/** Gets the underlying component array of a 4D vector. */
KZ_INLINE kzFloat* kzcVector4GetComponents(struct KzcVector4* vector)
{
    return vector->data;
}

/** Sets values for 4D vector. */
KZ_INLINE void kzcVector4Set(struct KzcVector4* vector, kzFloat x, kzFloat y, kzFloat z, kzFloat a)
{
    vector->data[0] = x;
    vector->data[1] = y;
    vector->data[2] = z;
    vector->data[3] = a;
}

/** Sets X component for 4D vector. */
KZ_INLINE void kzcVector4SetX(struct KzcVector4* vector, kzFloat x)
{
    vector->data[0] = x;
}

/** Sets Y component for 4D vector. */
KZ_INLINE void kzcVector4SetY(struct KzcVector4* vector, kzFloat y)
{
    vector->data[1] = y;
}

/** Sets Z component for 4D vector. */
KZ_INLINE void kzcVector4SetZ(struct KzcVector4* vector, kzFloat z)
{
    vector->data[2] = z;
}

/** Sets W component for 4D vector. */
KZ_INLINE void kzcVector4SetW(struct KzcVector4* vector, kzFloat w)
{
    vector->data[3] = w;
}

/** Adds two 4D vectors. */
KZ_INLINE void kzcVector4Add(const struct KzcVector4* first, const struct KzcVector4* second, struct KzcVector4* targetVector)
{
    targetVector->data[0] = first->data[0] + second->data[0];
    targetVector->data[1] = first->data[1] + second->data[1];
    targetVector->data[2] = first->data[2] + second->data[2];
    targetVector->data[3] = first->data[3] + second->data[3];
}

/** Subtracts two 4D vectors. */
KZ_INLINE void kzcVector4Subtract(const struct KzcVector4* first, const struct KzcVector4* second, struct KzcVector4* targetVector)
{
    targetVector->data[0] = first->data[0] - second->data[0];
    targetVector->data[1] = first->data[1] - second->data[1];
    targetVector->data[2] = first->data[2] - second->data[2];
    targetVector->data[3] = first->data[3] - second->data[3];
}

/** Scales 4D vector. */
KZ_INLINE void kzcVector4Scale(const struct KzcVector4* vector, kzFloat factor, struct KzcVector4* targetVector)
{
    targetVector->data[0] = vector->data[0] * factor;
    targetVector->data[1] = vector->data[1] * factor;
    targetVector->data[2] = vector->data[2] * factor;
    targetVector->data[3] = vector->data[3] * factor;
}

/** Negates 4D vector. */
KZ_INLINE void kzcVector4Negate(const struct KzcVector4* vector, struct KzcVector4* targetVector)
{
    targetVector->data[0] = -vector->data[0];
    targetVector->data[1] = -vector->data[1];
    targetVector->data[2] = -vector->data[2];
    targetVector->data[3] = -vector->data[3];
}

/** Calculates dot product of two 4D vectors. */
KZ_INLINE kzFloat kzcVector4Dot(const struct KzcVector4* left, const struct KzcVector4* right)
{
    return (left->data[0] * right->data[0]) +
           (left->data[1] * right->data[1]) +
           (left->data[2] * right->data[2]) +
           (left->data[3] * right->data[3]);
}

/** Returns length squared of a 4D vector. */
KZ_INLINE kzFloat kzcVector4GetLengthSquared(const struct KzcVector4* vector)
{
    return kzcVector4Dot(vector, vector);
}

/** Returns the length of 4D vector. */
KZ_INLINE kzFloat kzcVector4GetLength(const struct KzcVector4* vector)
{
    return kzsSqrtf(kzcVector4GetLengthSquared(vector));
}

/** Returns the distance between two 4D point vectors. */
KZ_INLINE kzFloat kzcVector4Distance(const struct KzcVector4* vector1, const struct KzcVector4* vector2)
{
    struct KzcVector4 delta;
    kzcVector4Subtract(vector2, vector1, &delta);
    return kzcVector4GetLength(&delta);
}

/** Normalizes a 4D vector. */
KZ_INLINE void kzcVector4Normalize(struct KzcVector4* vector)
{
    kzFloat vectorLength = kzcVector4GetLength(vector);
    /* Zero-vectors can not be normalized. */
    if(kzsFloatIsEqual(vectorLength, 0.0f))
    {
        kzcVector4Set(vector, KZS_NAN, KZS_NAN, KZS_NAN, KZS_NAN);
    }
    else
    {
        /* Normalize the components */
        kzcVector4Scale(vector, 1.0f  / vectorLength, vector);
    }
}

/** Reflects a 4D vector according to given axis vector. Axis has to be normalized. */
KZ_INLINE void kzcVector4Reflect(const struct KzcVector4* vector,  const struct KzcVector4* axis, struct KzcVector4* targetVector)
{
    kzFloat dot = 2.0f * kzcVector4Dot(vector, axis);

    kzcVector4Set(targetVector,
                  axis->data[0] * dot - vector->data[0],
                  axis->data[1] * dot - vector->data[1],
                  axis->data[2] * dot - vector->data[2],
                  axis->data[3] * dot - vector->data[3]);
}

/** Checks if two 4D vectors are equal. */
KZ_INLINE kzBool kzcVector4IsEqual(const struct KzcVector4* first, const struct KzcVector4* second)
{
    return kzsFloatIsEqual(first->data[0], second->data[0]) &&
           kzsFloatIsEqual(first->data[1], second->data[1]) &&
           kzsFloatIsEqual(first->data[2], second->data[2]) &&
           kzsFloatIsEqual(first->data[3], second->data[3]);
}

/** Calculates cross product of two vectors. Uses only the first 3 parts of the vectors and sets last component to zero. */
KZ_INLINE void kzcVector4Cross(const struct KzcVector4* left, const struct KzcVector4* right, struct KzcVector4* targetVector)
{
    kzcVector4Set(targetVector,
        left->data[1] * right->data[2] - left->data[2] * right->data[1],
        left->data[2] * right->data[0] - left->data[0] * right->data[2],
        left->data[0] * right->data[1] - left->data[1] * right->data[0],
        0.0f);
}


/** Returns the component maximum of two vectors. */
KZ_INLINE struct KzcVector4 kzcVector4ComponentMax(const struct KzcVector4* vectorA, const struct KzcVector4* vectorB)
{
    return kzcVector4(kzsMaxf(kzcVector4GetX(vectorA), kzcVector4GetX(vectorB)),
                      kzsMaxf(kzcVector4GetY(vectorA), kzcVector4GetY(vectorB)),
                      kzsMaxf(kzcVector4GetZ(vectorA), kzcVector4GetZ(vectorB)),
                      kzsMaxf(kzcVector4GetW(vectorA), kzcVector4GetW(vectorB)));
}

/** Returns the component minimum of two vectors. */
KZ_INLINE struct KzcVector4 kzcVector4ComponentMin(const struct KzcVector4* vectorA, const struct KzcVector4* vectorB)
{
    return kzcVector4(kzsMinf(kzcVector4GetX(vectorA), kzcVector4GetX(vectorB)),
                      kzsMinf(kzcVector4GetY(vectorA), kzcVector4GetY(vectorB)),
                      kzsMinf(kzcVector4GetZ(vectorA), kzcVector4GetZ(vectorB)),
                      kzsMinf(kzcVector4GetW(vectorA), kzcVector4GetW(vectorB)));
}

/** Sets the vector components to NaN. */
void kzcVector4SetInvalid(struct KzcVector4* vector);


#endif
