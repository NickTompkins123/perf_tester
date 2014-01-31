/**
* \file
* Matrix2x2 structure and operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_MATRIX2X2_H
#define KZC_MATRIX2X2_H


#include <system/wrappers/kzs_math.h>
#include <system/kzs_types.h>


/* Forward declarations */
struct KzcVector2;


/** Structure for 2x2 matrix. Uses column-major notation. */
struct KzcMatrix2x2
{
    kzFloat data[4]; /**< Matrix data */
};


/** Constant for identity matrix. */
extern const struct KzcMatrix2x2 KZC_MATRIX2X2_IDENTITY;


/** 
 * Creates a matrix with given values. 
 */
KZ_INLINE struct KzcMatrix2x2 kzcMatrix2x2(kzFloat a1, kzFloat b1, 
                                           kzFloat a2, kzFloat b2)
{
    struct KzcMatrix2x2 matrix;
    matrix.data[0] = a1;
    matrix.data[1] = b1;
    matrix.data[2] = a2;
    matrix.data[3] = b2;
    return matrix;
}

/** Sets an identity matrix. */
void kzcMatrix2x2SetIdentity(struct KzcMatrix2x2* matrix);
/** Sets an invalid matrix filled with NaNs. */
void kzcMatrix2x2SetInvalid(struct KzcMatrix2x2* matrix);


/** Applies a transpose for a matrix. out_matrix cannot be same as input matrix. */
void kzcMatrix2x2Transpose(const struct KzcMatrix2x2* matrix, struct KzcMatrix2x2* out_matrix);
/** Applies an inversion for an affine matrix. out_matrix cannot be same as input matrix. */
void kzcMatrix2x2Inverse(const struct KzcMatrix2x2* matrix, struct KzcMatrix2x2* out_matrix);
/** Applies rotation for a matrix. */
void kzcMatrix2x2Rotate(struct KzcMatrix2x2* matrix, kzFloat angleInRadians);


/** Applies an uniform scaling for a matrix */
void kzcMatrix2x2ScaleUniform(struct KzcMatrix2x2* matrix, kzFloat scale);
/** Applies a scaling for a matrix. */
void kzcMatrix2x2Scale(struct KzcMatrix2x2* matrix, kzFloat scaleX, kzFloat scaleY);
/** Gets scaling factor from matrix for X axis. */
kzFloat kzcMatrix2x2GetScaleX(struct KzcMatrix2x2* matrix);
/** Gets scaling factor from matrix for Y axis. */
kzFloat kzcMatrix2x2GetScaleY(struct KzcMatrix2x2* matrix);
/** Applies a scaling for a matrix on axis X. */
void kzcMatrix2x2ScaleX(struct KzcMatrix2x2* matrix, kzFloat scale);
/** Applies a scaling for a matrix on axis Y. */
void kzcMatrix2x2ScaleY(struct KzcMatrix2x2* matrix, kzFloat scale);


/** Multiplies 2 matrices, result in out_matrix. out_matrix cannot be same as any of the input matrices. */
void kzcMatrix2x2Multiply(const struct KzcMatrix2x2* matrix1, const struct KzcMatrix2x2* matrix2, struct KzcMatrix2x2* out_matrix);
/** Multiplies matrix and vector2. out_vector cannot be same as input vector. */
void kzcMatrix2x2MultiplyVector2(const struct KzcMatrix2x2* matrix, const struct KzcVector2* vector, struct KzcVector2* out_vector);


/** Returns matrix's trace. */
kzFloat kzcMatrix2x2Trace(const struct KzcMatrix2x2* matrix);

/** Returns the determinant of the matrix. */
kzFloat kzcMatrix2x2Determinant(const struct KzcMatrix2x2* matrix);

/** Compares 2 matrices if they are equal. */
KZ_INLINE kzBool kzcMatrix2x2IsEqual(const struct KzcMatrix2x2* matrixA, const struct KzcMatrix2x2* matrixB)
{
    kzInt i;
    kzBool isEqualMatrix = KZ_TRUE;

    for(i = 0; i < 4; ++i)
    {
        if(!kzsFloatIsEqual(matrixA->data[i], matrixB->data[i]))
        {
            isEqualMatrix = KZ_FALSE;
            break;
        }
    }

    return isEqualMatrix;
}


#endif
