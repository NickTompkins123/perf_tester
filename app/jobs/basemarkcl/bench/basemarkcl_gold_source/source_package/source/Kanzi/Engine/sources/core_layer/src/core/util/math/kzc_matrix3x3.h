/**
* \file
* Matrix3x3 structure and operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_MATRIX3X3_H
#define KZC_MATRIX3X3_H


#include <system/wrappers/kzs_math.h>
#include <system/kzs_types.h>


#define KZC_MATRIX3X3_INDEX_TRANSLATION_X    6    /**< Matrix index for X translation. */
#define KZC_MATRIX3X3_INDEX_TRANSLATION_Y    7    /**< Matrix index for Y translation. */


/* Forward declaration structures. */
struct KzcVector2;
struct KzcVector3;


/** Structure for 3x3 matrix. Uses column-major notation. */
struct KzcMatrix3x3
{
    kzFloat data[9]; /**< Matrix data */
};


/** Constant for identity matrix. */
extern const struct KzcMatrix3x3 KZC_MATRIX3X3_IDENTITY;


/** 
 * Creates a matrix with given values. 
 */
KZ_INLINE struct KzcMatrix3x3 kzcMatrix3x3(kzFloat a1, kzFloat b1, kzFloat c1, 
                                           kzFloat a2, kzFloat b2, kzFloat c2, 
                                           kzFloat a3, kzFloat b3, kzFloat c3)
{
    struct KzcMatrix3x3 matrix;
    matrix.data[0] = a1;
    matrix.data[1] = b1;
    matrix.data[2] = c1;
    matrix.data[3] = a2;
    matrix.data[4] = b2;
    matrix.data[5] = c2;
    matrix.data[6] = a3;
    matrix.data[7] = b3;
    matrix.data[8] = c3;
    return matrix;
}


/** Sets a identity matrix. */
void kzcMatrix3x3SetIdentity(struct KzcMatrix3x3* matrix);
/** Sets an invalid matrix filled with NaNs. */
void kzcMatrix3x3SetInvalid(struct KzcMatrix3x3* matrix);
/** Sets a 3x3 matrix top left 2x2 part as identity. */
void kzcMatrix3x3SetIdentity2x2(struct KzcMatrix3x3* matrix);
/** Sets a 3x3 matrix translation columns to zero. */
void kzcMatrix3x3ResetTranslation(struct KzcMatrix3x3* matrix);


/** Applies a transpose for a matrix. out_matrix cannot be same as input matrix. */
void kzcMatrix3x3Transpose(const struct KzcMatrix3x3* matrix, struct KzcMatrix3x3* out_matrix);
/** Applies an inversion for an affine matrix. out_matrix cannot be same as input matrix. */
void kzcMatrix3x3Inverse(const struct KzcMatrix3x3* matrix, struct KzcMatrix3x3* out_matrix);
/** Applies a translation for a matrix. */
void kzcMatrix3x3Translate(struct KzcMatrix3x3* matrix, kzFloat x, kzFloat y);
/** Applies a translation for a matrix. */
void kzcMatrix3x3TranslateVector2(struct KzcMatrix3x3* matrix, const struct KzcVector2* translation);
/** Applies rotation for a matrix. */
void kzcMatrix3x3Rotate(struct KzcMatrix3x3* matrix, kzFloat angleInRadians);


/** Applies an uniform scaling for a matrix */
void kzcMatrix3x3ScaleUniform(struct KzcMatrix3x3* matrix, kzFloat scale);
/** Applies a scaling for a matrix. */
void kzcMatrix3x3Scale(struct KzcMatrix3x3* matrix, kzFloat scaleX, kzFloat scaleY);
/** Gets scaling factor from matrix for X axis. */
kzFloat kzcMatrix3x3GetScaleX(const struct KzcMatrix3x3* matrix);
/** Gets scaling factor from matrix for Y axis. */
kzFloat kzcMatrix3x3GetScaleY(const struct KzcMatrix3x3* matrix);
/** Applies a scaling for a matrix on axis X. */
void kzcMatrix3x3ScaleX(struct KzcMatrix3x3* matrix, kzFloat scale);
/** Applies a scaling for a matrix on axis Y. */
void kzcMatrix3x3ScaleY(struct KzcMatrix3x3* matrix, kzFloat scale);


/** Normalizes the given matrix, result in out_matrix. The matrix must be an affine transformation. */
void kzcMatrix3x3Normalize(const struct KzcMatrix3x3* matrix, struct KzcMatrix3x3* out_matrix);
/** Multiplies 2 matrices, result in out_matrix. out_matrix cannot be same as any of the input matrices. */
void kzcMatrix3x3Multiply(const struct KzcMatrix3x3* matrix1, const struct KzcMatrix3x3* matrix2, struct KzcMatrix3x3* out_matrix);
/** Multiplies matrix and vector2 defined as vec3(vector2.xy, 1.0). out_vector cannot be same as input vector. */
void kzcMatrix3x3MultiplyVector2(const struct KzcMatrix3x3* matrix, const struct KzcVector2* vector, struct KzcVector2* out_vector);
/** Multiplies matrix and vector3. out_vector cannot be same as input vector. */
void kzcMatrix3x3MultiplyVector3(const struct KzcMatrix3x3* matrix, const struct KzcVector3* vector, struct KzcVector3* out_vector);


/** Returns a position that this transformation matrix contains. */
void kzcMatrix3x3GetPosition(const struct KzcMatrix3x3* matrix, struct KzcVector2* out_position);
/** Sets the position components of transformation matrix. */
void kzcMatrix3x3SetPositionVector3(struct KzcMatrix3x3* matrix, const struct KzcVector2* position);

/** Calculates the determinant of the matrix. */
kzFloat kzcMatrix3x3Determinant(const struct KzcMatrix3x3* matrix);

/** Returns matrix's trace. */
kzFloat kzcMatrix3x3Trace(const struct KzcMatrix3x3* matrix);


/** Compares 2 matrices if they are equal. */
KZ_INLINE kzBool kzcMatrix3x3IsEqual(const struct KzcMatrix3x3* matrixA, const struct KzcMatrix3x3* matrixB)
{
    kzInt i;
    kzBool isEqualMatrix = KZ_TRUE;

    for(i = 0; i < 9; ++i)
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
