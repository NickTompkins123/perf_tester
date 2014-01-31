/**
* \file
* Matrix4x4 structure and operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_MATRIX4X4_H
#define KZC_MATRIX4X4_H


#include <system/wrappers/kzs_math.h>
#include <system/kzs_types.h>


#define KZC_MATRIX4X4_INDEX_TRANSLATION_X    12    /**< Matrix index for X translation. */
#define KZC_MATRIX4X4_INDEX_TRANSLATION_Y    13    /**< Matrix index for Y translation. */
#define KZC_MATRIX4X4_INDEX_TRANSLATION_Z    14    /**< Matrix index for Z translation. */


/* Forward declaration structures. */
struct KzcVector3;
struct KzcVector4;


/** 
* Structure for 4x4 matrix. Uses column-major notation.
*
* The translation components occupy the 13th, 14th, and 15th elements of the 16-element matrix,
* where indices are numbered from 1 to 16 as described in section 2.11.2 of the OpenGL 2.1 Specification.
*/
struct KzcMatrix4x4
{
    kzFloat data[16]; /**< Matrix data. */
};


/** Constant for identity matrix. */
extern const struct KzcMatrix4x4 KZC_MATRIX4X4_IDENTITY;
/** Constant for NaN matrix. */
extern const struct KzcMatrix4x4 KZC_MATRIX4X4_NAN;
/** Constant for shadow map bias matrix. */
extern const struct KzcMatrix4x4 KZC_MATRIX4X4_SHADOW_MAP_BIAS;


/** 
 * Creates a matrix with given values. 
 * The translation components occupy the 13th, 14th, and 15th elements of the 16-element matrix,
 * where indices are numbered from 1 to 16 as described in section 2.11.2 of the OpenGL 2.1 Specification.
 */
KZ_INLINE struct KzcMatrix4x4 kzcMatrix4x4(kzFloat a1, kzFloat b1, kzFloat c1, kzFloat d1,
                                           kzFloat a2, kzFloat b2, kzFloat c2, kzFloat d2,
                                           kzFloat a3, kzFloat b3, kzFloat c3, kzFloat d3,
                                           kzFloat a4, kzFloat b4, kzFloat c4, kzFloat d4)
{
    struct KzcMatrix4x4 matrix;
    matrix.data[0] = a1;
    matrix.data[1] = b1;
    matrix.data[2] = c1;
    matrix.data[3] = d1;
    matrix.data[4] = a2;
    matrix.data[5] = b2;
    matrix.data[6] = c2;
    matrix.data[7] = d2;
    matrix.data[8] = a3;
    matrix.data[9] = b3;
    matrix.data[10] = c3;
    matrix.data[11] = d3;
    matrix.data[12] = a4;
    matrix.data[13] = b4;
    matrix.data[14] = c4;
    matrix.data[15] = d4;
    return matrix;
}

/** Sets a identity matrix. */
void kzcMatrix4x4SetIdentity(struct KzcMatrix4x4* matrix);
/** Sets an invalid matrix filled with NaNs. */
void kzcMatrix4x4SetInvalid(struct KzcMatrix4x4* matrix);
/** Sets a 4x4 matrix top left 3x3 part as identity. */
void kzcMatrix4x4SetIdentity3x3(struct KzcMatrix4x4* matrix);
/** Sets a 4x4 matrix translation columns to zero. */
void kzcMatrix4x4ResetTranslation(struct KzcMatrix4x4* matrix);


/** Applies a transpose for a matrix. out_matrix cannot be same as input matrix. */
void kzcMatrix4x4Transpose(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_matrix);
/** Applies an inversion for an affine matrix. out_matrix cannot be same as input matrix. */
void kzcMatrix4x4Inverse(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_matrix);
/** Applies a translation for a matrix. */
void kzcMatrix4x4Translate(struct KzcMatrix4x4* matrix, kzFloat x, kzFloat y, kzFloat z);
/** Translates matrix by vector3. */
void kzcMatrix4x4TranslateVector3(struct KzcMatrix4x4* matrix, const struct KzcVector3* translation);
/** Applies a rotation X for a matrix, angle in radians. */
void kzcMatrix4x4RotateX(struct KzcMatrix4x4* matrix, kzFloat angleInRadians);
/** Applies a rotation Y for a matrix, angle in radians. */
void kzcMatrix4x4RotateY(struct KzcMatrix4x4* matrix, kzFloat angleInRadians);
/** Applies a rotation Z for a matrix, angle in radians. */
void kzcMatrix4x4RotateZ(struct KzcMatrix4x4* matrix, kzFloat angleInRadians);
/** Applies a rotation around arbitrary axis for a matrix, angle in radians. Similar to glRotatef. */
void kzcMatrix4x4Rotate(struct KzcMatrix4x4* matrix, kzFloat angleInRadians, kzFloat x, kzFloat y, kzFloat z);
/** Sets 4x4 matrix to view coordinates ("viewer"). out_viewMatrix cannot be same as input matrix. */
void kzcMatrix4x4SetToViewCoordinates(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_viewMatrix);


/** Applies an uniform scaling for a matrix */
void kzcMatrix4x4ScaleUniform(struct KzcMatrix4x4* matrix, kzFloat scale);
/** Applies a scaling for a matrix */
void kzcMatrix4x4Scale(struct KzcMatrix4x4* matrix, kzFloat scaleX, kzFloat scaleY, kzFloat scaleZ);
/** Gets scaling factor from matrix for X axis. */
kzFloat kzcMatrix4x4GetScaleX(const struct KzcMatrix4x4* matrix);
/** Gets scaling factor from matrix for Y axis. */
kzFloat kzcMatrix4x4GetScaleY(const struct KzcMatrix4x4* matrix);
/** Gets scaling factor from matrix for Z axis. */
kzFloat kzcMatrix4x4GetScaleZ(const struct KzcMatrix4x4* matrix);
/** Applies a scaling for a matrix on axis X. */
void kzcMatrix4x4ScaleX(struct KzcMatrix4x4* matrix, kzFloat scale);
/** Applies a scaling for a matrix on axis Y. */
void kzcMatrix4x4ScaleY(struct KzcMatrix4x4* matrix, kzFloat scale);
/** Applies a scaling for a matrix on axis Z. */
void kzcMatrix4x4ScaleZ(struct KzcMatrix4x4* matrix, kzFloat scale);


/** Normalizes the given matrix, result in out_matrix. The matrix must be an affine transformation. */
void kzcMatrix4x4Normalize(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_matrix);
/** OrthoNormalizes a matrix with cross products, result in out_matrix.
*
* Axises are normalized fully, including their lengths. This function can be used to prevent value drift. */
void kzcMatrix4x4OrthoNormalize(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_matrix);
/** OrthoNormalizes a matrix with cross products, result in out_matrix.
*
* This function preserves the scale of the axises, so it won't help in preventing potential drift. */
void kzcMatrix4x4OrthoNormalizePreserveScale(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_matrix);
/** Multiplies 2 matrices, result in out_matrix. out_matrix cannot be same as any of the input matrices. */
void kzcMatrix4x4Multiply(const struct KzcMatrix4x4* matrix1, const struct KzcMatrix4x4* matrix2, struct KzcMatrix4x4* out_matrix);
/** Multiplies 2 affine matrices, result in out_matrix. out_matrix cannot be same as any of the input matrices. */
void kzcMatrix4x4MultiplyAffine(const struct KzcMatrix4x4* matrix1, const struct KzcMatrix4x4* matrix2, struct KzcMatrix4x4* out_matrix);
/** Gets translation Z value from matrix1 * matrix2, where both are affine transformations. */
kzFloat kzcMatrix4x4MultiplyAffineGetTranslationZ(const struct KzcMatrix4x4* matrix1, const struct KzcMatrix4x4* matrix2);
/** Multiplies matrix and vector3 defined as vec4(vector3.xyz, 1.0). */
void kzcMatrix4x4MultiplyVector3(const struct KzcMatrix4x4* matrix, const struct KzcVector3* vector, struct KzcVector3* out_vector);
/** Multiplies matrix and vector3 by using 3x3 top left corner of matrix (orientation & scale only). */
void kzcMatrix4x4MultiplyVector3By3x3(const struct KzcMatrix4x4* matrix, const struct KzcVector3* vector, struct KzcVector3* out_vector);
/** Multiplies matrix and vector4. out_vector cannot be same as input vector. */
void kzcMatrix4x4MultiplyVector4(const struct KzcMatrix4x4* matrix, const struct KzcVector4* vector, struct KzcVector4* out_vector);
/** Creates projection matrix for orthogonal projection. */
void kzcMatrix4x4CreateOrthogonalProjection(struct KzcMatrix4x4* matrix, kzFloat left, kzFloat right, kzFloat bottom, kzFloat top, kzFloat near, kzFloat far);
/** Interpolates 2 SRT matrices, t clamped to range [0, 1]. */
void kzcMatrix4x4Interpolate(const struct KzcMatrix4x4* matrix1, const struct KzcMatrix4x4* matrix2, kzFloat t, struct KzcMatrix4x4* out_matrix);


/** Returns a position that this transformation matrix contains. */
void kzcMatrix4x4GetPosition(const struct KzcMatrix4x4* matrix, struct KzcVector3* out_position);
/** Sets the position components of transformation matrix. */
void kzcMatrix4x4SetPositionVector3(struct KzcMatrix4x4* matrix, const struct KzcVector3* position);


/** Calculates Euler angles (in radians) from given matrix. */
void kzcMatrix4x4GetEulerAngles(const struct KzcMatrix4x4* matrix, struct KzcVector3* out_anglesInRadians);

/** Calculates the determinant of the matrix. */
kzFloat kzcMatrix4x4Determinant(const struct KzcMatrix4x4* matrix);

/** Returns matrix's trace. */
kzFloat kzcMatrix4x4Trace(const struct KzcMatrix4x4* matrix);

/** Determines if a matrix has only right angle rotations. */
kzBool kzcMatrix4x4IsRightAngleRotation(const struct KzcMatrix4x4* matrix, kzFloat epsilon);

/** Determines if a matrix is a simple layout matrix - has only scale and right-angle rotations. */
kzBool kzcMatrix4x4IsSimpleLayout(const struct KzcMatrix4x4* matrix, kzFloat epsilon);


/** Compares 2 matrices if they are equal. */
KZ_INLINE kzBool kzcMatrix4x4IsEqual(const struct KzcMatrix4x4* matrixA, const struct KzcMatrix4x4* matrixB)
{
    kzInt i;
    kzBool isEqualMatrix = KZ_TRUE;

    for(i = 0; i < 16; ++i)
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
