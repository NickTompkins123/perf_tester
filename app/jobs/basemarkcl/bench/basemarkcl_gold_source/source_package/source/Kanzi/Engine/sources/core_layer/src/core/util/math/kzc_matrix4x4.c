/**
* \file
* Matrix4x4 structure and operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_matrix4x4.h"


#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector4.h>
#include <core/util/math/kzc_quaternion.h>

#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_math.h>
#include <system/debug/kzs_log.h>
#include <system/debug/kzs_counter.h>


const struct KzcMatrix4x4 KZC_MATRIX4X4_IDENTITY = {{1.0f, 0.0f, 0.0f, 0.0f, 
                                                     0.0f, 1.0f, 0.0f, 0.0f,
                                                     0.0f, 0.0f, 1.0f, 0.0f,
                                                     0.0f, 0.0f, 0.0f, 1.0f}};

const struct KzcMatrix4x4 KZC_MATRIX4X4_SHADOW_MAP_BIAS = {{0.5f, 0.0f, 0.0f, 0.0f, 
                                                            0.0f, 0.5f, 0.0f, 0.0f,
                                                            0.0f, 0.0f, 1.0f, 0.0f,
                                                            0.5f, 0.5f, 0.0f, 1.0f}};

/** Helper function for matrix determinant. Returns a determinant of 3x3 part of a matrix */
static kzFloat kzcMatrixDeterminant3x3_internal(const struct KzcMatrix4x4* matrix)
{
    kzFloat const* matrixData = matrix->data;

    kzFloat determinant1 = matrixData[5] * matrixData[10] - matrixData[9] * matrixData[6];
    kzFloat determinant2 = matrixData[4] * matrixData[10] - matrixData[8] * matrixData[6];
    kzFloat determinant3 = matrixData[4] * matrixData[9]  - matrixData[8] * matrixData[5];

    return  (matrixData[0] * determinant1 - matrixData[1] * determinant2 + matrixData[2] * determinant3);
}

/** Sets an identity matrix for 4th column & row only (helper function to optimized operations) */
static void kzcMatrixSetIdentityRow4Column4_internal(struct KzcMatrix4x4* matrix)
{
    kzFloat* matrixData = matrix->data;

    matrixData[3] = 0.0f;
    matrixData[7] = 0.0f;
    matrixData[11] = 0.0f;
    matrixData[12] = 0.0f;
    matrixData[13] = 0.0f;
    matrixData[14] = 0.0f;
    matrixData[15] = 1.0f;
}

void kzcMatrix4x4SetIdentity(struct KzcMatrix4x4* matrix)
{
    *matrix = KZC_MATRIX4X4_IDENTITY;
}

void kzcMatrix4x4SetInvalid(struct KzcMatrix4x4* matrix)
{
    matrix->data[0] = KZS_NAN;
    matrix->data[1] = KZS_NAN;
    matrix->data[2] = KZS_NAN;
    matrix->data[3] = KZS_NAN;
    matrix->data[4] = KZS_NAN;
    matrix->data[5] = KZS_NAN;
    matrix->data[6] = KZS_NAN;
    matrix->data[7] = KZS_NAN;
    matrix->data[8] = KZS_NAN;
    matrix->data[9] = KZS_NAN;
    matrix->data[10] = KZS_NAN;
    matrix->data[11] = KZS_NAN;
    matrix->data[12] = KZS_NAN;
    matrix->data[13] = KZS_NAN;
    matrix->data[14] = KZS_NAN;
    matrix->data[15] = KZS_NAN;
}

void kzcMatrix4x4SetIdentity3x3(struct KzcMatrix4x4* matrix)
{
    matrix->data[0] = 1.0f;
    matrix->data[1] = 0.0f;
    matrix->data[2] = 0.0f;
    matrix->data[4] = 0.0f;
    matrix->data[5] = 1.0f;
    matrix->data[6] = 0.0f;
    matrix->data[8] = 0.0f;
    matrix->data[9] = 0.0f;
    matrix->data[10] = 1.0f;
}

void kzcMatrix4x4ResetTranslation(struct KzcMatrix4x4* matrix)
{
    matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] = 0.0f;
    matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] = 0.0f;
    matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] = 0.0f;
}

void kzcMatrix4x4Transpose(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_matrix)
{
    const kzFloat *inData = matrix->data;
    kzFloat *outData = out_matrix->data;

    outData[0] = inData[0];     outData[1] = inData[4];     outData[2] = inData[8];     outData[3] = inData[12];
    outData[4] = inData[1];     outData[5] = inData[5];     outData[6] = inData[9];     outData[7] = inData[13];
    outData[8] = inData[2];     outData[9] = inData[6];     outData[10] = inData[10];   outData[11] = inData[14];
    outData[12] = inData[3];    outData[13] = inData[7];    outData[14] = inData[11];   outData[15] = inData[15];
}

void kzcMatrix4x4Inverse(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_matrix)
{
    const kzFloat determinant = kzcMatrixDeterminant3x3_internal(matrix);

    /* Check if determinant for this matrix is zero */
    if(determinant < KZ_FLOAT_EPSILON && determinant > -KZ_FLOAT_EPSILON)
    {
        /*lint -e{956} Ignored warning for not multi thread safeness. for debug printing it is ok. */
        static kzBool warningPrinted = KZ_FALSE;        
        if(!warningPrinted)
        {
            warningPrinted = KZ_TRUE;
            kzsLog(KZS_LOG_LEVEL_WARNING, "Matrix with determinant = 0 does not have inverse! This warning is only printed once.");
        }
        *out_matrix = KZC_MATRIX4X4_IDENTITY;
    }
    else
    {
        struct KzcMatrix4x4 temp;
        struct KzcMatrix4x4 transpose;
        kzFloat const* matrixData = matrix->data;
        /* 2x2 square determinants */
        kzFloat determinant11, determinant12, determinant13;
        kzFloat determinant21, determinant22, determinant23;
        kzFloat determinant31, determinant32, determinant33;
        kzFloat detInv;
        kzFloat const* transposeData;
        /* Transpose of matrix */
        kzcMatrix4x4Transpose(matrix, &transpose);
        transposeData = &transpose.data[0];

        /* Actual inverse of 3x3 matrix is defined: 1 / det(M) * (adjugate(M)) */
        determinant11 = transposeData[5] * transposeData[10] - transposeData[9] * transposeData[6];
        determinant12 = transposeData[4] * transposeData[10] - transposeData[8] * transposeData[6];
        determinant13 = transposeData[4] * transposeData[9] - transposeData[8] * transposeData[5];

        determinant21 = transposeData[1] * transposeData[10] - transposeData[9] * transposeData[2];
        determinant22 = transposeData[0] * transposeData[10] - transposeData[8] * transposeData[2];
        determinant23 = transposeData[0] * transposeData[9] - transposeData[8] * transposeData[1];

        determinant31 = transposeData[1] * transposeData[6] - transposeData[5] * transposeData[2];
        determinant32 = transposeData[0] * transposeData[6] - transposeData[2] * transposeData[4];
        determinant33 = transposeData[0] * transposeData[5] - transposeData[4] * transposeData[1];

        /* 4x4 part of matrix can be safely be identity only, since rest of the matrix content is filled with sub determinants */
        kzcMatrixSetIdentityRow4Column4_internal(&temp);
        detInv = 1.0f / determinant;
        temp.data[0] = determinant11 * detInv;
        temp.data[1] = -determinant12 * detInv;
        temp.data[2] = determinant13 * detInv;
        temp.data[4] = -determinant21 * detInv;
        temp.data[5] = determinant22 * detInv;
        temp.data[6] = -determinant23 * detInv;
        temp.data[8] = determinant31 * detInv;
        temp.data[9] = -determinant32 * detInv;
        temp.data[10] = determinant33 * detInv;

        kzcMatrix4x4Translate(&temp, -matrixData[12], -matrixData[13], -matrixData[14]);
        *out_matrix = temp;
    }

    kzsCounterIncrease("kzcMatrix4x4Inverse");
}

void kzcMatrix4x4Translate(struct KzcMatrix4x4* matrix, kzFloat x, kzFloat y, kzFloat z)
{
    kzFloat* matrixData  = matrix->data;

    matrixData[12] += matrixData[0] * x + matrixData[4] * y + matrixData[8] * z;
    matrixData[13] += matrixData[1] * x + matrixData[5] * y + matrixData[9] * z;
    matrixData[14] += matrixData[2] * x + matrixData[6] * y + matrixData[10] * z;
}

void kzcMatrix4x4TranslateVector3(struct KzcMatrix4x4* matrix, const struct KzcVector3* direction)
{
    kzcMatrix4x4Translate(matrix, kzcVector3GetX(direction), kzcVector3GetY(direction), kzcVector3GetZ(direction));
}

void kzcMatrix4x4RotateX(struct KzcMatrix4x4* matrix, kzFloat angleInRadians)
{
    struct KzcMatrix4x4 rotationMatrix;
    struct KzcMatrix4x4 result;

    const kzFloat sinTerm = kzsSinf(angleInRadians);
    const kzFloat cosTerm = kzsCosf(angleInRadians);

    kzcMatrix4x4SetIdentity(&rotationMatrix);

    rotationMatrix.data[5] = cosTerm;    rotationMatrix.data[6] = -sinTerm;
    rotationMatrix.data[9] = sinTerm;    rotationMatrix.data[10] = cosTerm;

    kzcMatrix4x4Multiply(&rotationMatrix, matrix, &result);
    *matrix = result;
}

void kzcMatrix4x4RotateY(struct KzcMatrix4x4* matrix, kzFloat angleInRadians)
{
    struct KzcMatrix4x4 rotationMatrix;
    struct KzcMatrix4x4 result;

    const kzFloat sinTerm = kzsSinf(angleInRadians);
    const kzFloat cosTerm = kzsCosf(angleInRadians);

    kzcMatrix4x4SetIdentity(&rotationMatrix);

    rotationMatrix.data[0] = cosTerm;    rotationMatrix.data[2] = sinTerm;
    rotationMatrix.data[8] = -sinTerm;    rotationMatrix.data[10] = cosTerm;

    kzcMatrix4x4Multiply(&rotationMatrix, matrix, &result);
    *matrix = result;
}

void kzcMatrix4x4RotateZ(struct KzcMatrix4x4* matrix, kzFloat angleInRadians)
{
    struct KzcMatrix4x4 rotationMatrix;
    struct KzcMatrix4x4 result;

    const kzFloat sinTerm = kzsSinf(angleInRadians);
    const kzFloat cosTerm = kzsCosf(angleInRadians);

    kzcMatrix4x4SetIdentity(&rotationMatrix);

    rotationMatrix.data[0] = cosTerm;    rotationMatrix.data[1] = -sinTerm;
    rotationMatrix.data[4] = sinTerm;    rotationMatrix.data[5] = cosTerm;

    kzcMatrix4x4Multiply(&rotationMatrix, matrix, &result);
    *matrix = result;
}

void kzcMatrix4x4Rotate(struct KzcMatrix4x4* matrix, kzFloat angleInRadians, kzFloat x, kzFloat y, kzFloat z)
{
    struct KzcMatrix4x4 rotationMatrix;
    struct KzcMatrix4x4 result;
    kzFloat cosMinusTerm;
    kzFloat axisLengthInverse, axisLengthSquared;
    kzFloat sinTerm, cosTerm;
    const kzFloat epsilon = 0.0001f;

    axisLengthSquared = x * x + y * y + z * z;

    /* Check if determinant for this matrix is zero */
    if(axisLengthSquared < epsilon)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Can not rotate by zero axis!");
    }
    else
    {
        sinTerm = kzsSinf(angleInRadians);
        cosTerm = kzsCosf(angleInRadians);
        cosMinusTerm = 1.0f - cosTerm;

        axisLengthInverse = 1.0f / kzsSqrtf(axisLengthSquared);
        x *= axisLengthInverse;
        y *= axisLengthInverse;
        z *= axisLengthInverse;

        kzcMatrix4x4SetIdentity(&rotationMatrix);

        /*  OpenGL specification defines glRotatef as: 
        ( xx(1-c)+c     xy(1-c)-zs  xz(1-c)+ys  0  )
        | yx(1-c)+zs    yy(1-c)+c   yz(1-c)-xs  0  |
        | xz(1-c)-ys    yz(1-c)+xs  zz(1-c)+c   0  |
        (   0                 0         0       1)*/
        rotationMatrix.data[0] = x * x * cosMinusTerm + cosTerm;
        rotationMatrix.data[1] = x * y * cosMinusTerm - z * sinTerm;
        rotationMatrix.data[2] = x * z * cosMinusTerm + y * sinTerm;

        rotationMatrix.data[4] = y * x * cosMinusTerm + z * sinTerm;
        rotationMatrix.data[5] = y * y * cosMinusTerm + cosTerm;
        rotationMatrix.data[6] = y * z * cosMinusTerm - x * sinTerm;

        rotationMatrix.data[8] = z * x * cosMinusTerm - y * sinTerm;
        rotationMatrix.data[9] = z * y * cosMinusTerm + x * sinTerm;
        rotationMatrix.data[10] = z * z * cosMinusTerm + cosTerm;

        kzcMatrix4x4Multiply(&rotationMatrix, matrix, &result);
        *matrix = result;
    }
}

void kzcMatrix4x4SetToViewCoordinates(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_viewMatrix)
{
    kzFloat positionX, positionY, positionZ;
    struct KzcMatrix4x4 viewMatrix = *matrix;

    positionX = matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_X];
    positionY = matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y];
    positionZ = matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z];

    viewMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] = 0.0f;
    viewMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] = 0.0f;
    viewMatrix.data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] = 0.0f;

    kzcMatrix4x4Inverse(&viewMatrix, out_viewMatrix);
    kzcMatrix4x4Translate(out_viewMatrix, -positionX, -positionY, -positionZ);
}

void kzcMatrix4x4ScaleUniform(struct KzcMatrix4x4* matrix, kzFloat scale)
{
    kzFloat *matrixData = matrix->data;

    matrixData[0] *= scale; matrixData[1] *= scale; matrixData[2] *= scale;
    matrixData[4] *= scale; matrixData[5] *= scale; matrixData[6] *= scale;
    matrixData[8] *= scale; matrixData[9] *= scale; matrixData[10] *= scale;
}

void kzcMatrix4x4Scale(struct KzcMatrix4x4* matrix, kzFloat scaleX, kzFloat scaleY, kzFloat scaleZ)
{
    kzFloat *matrixData = matrix->data;

    matrixData[0] *= scaleX; matrixData[1] *= scaleY; matrixData[2] *= scaleZ;
    matrixData[4] *= scaleX; matrixData[5] *= scaleY; matrixData[6] *= scaleZ;
    matrixData[8] *= scaleX; matrixData[9] *= scaleY; matrixData[10] *= scaleZ;
}

kzFloat kzcMatrix4x4GetScaleX(const struct KzcMatrix4x4* matrix)
{
    const kzFloat *matrixData = matrix->data;
    return kzsSqrtf(matrixData[0] * matrixData[0] + matrixData[1] * matrixData[1] + matrixData[2] * matrixData[2] + matrixData[3] * matrixData[3]);
}

kzFloat kzcMatrix4x4GetScaleY(const struct KzcMatrix4x4* matrix)
{
    const kzFloat *matrixData = matrix->data;
    return kzsSqrtf(matrixData[4] * matrixData[4] + matrixData[5] * matrixData[5] + matrixData[6] * matrixData[6] + matrixData[7] * matrixData[7]);
}

kzFloat kzcMatrix4x4GetScaleZ(const struct KzcMatrix4x4* matrix)
{
    const kzFloat *matrixData = matrix->data;
    return kzsSqrtf(matrixData[8] * matrixData[8] + matrixData[9] * matrixData[9] + matrixData[10] * matrixData[10] + matrixData[11] * matrixData[11]);
}

void kzcMatrix4x4ScaleX(struct KzcMatrix4x4* matrix, kzFloat scale)
{
    kzFloat *matrixData = matrix->data;
    matrixData[0] *= scale; 
    matrixData[4] *= scale; 
    matrixData[8] *= scale; 
}

void kzcMatrix4x4ScaleY(struct KzcMatrix4x4* matrix, kzFloat scale)
{
    kzFloat *matrixData = matrix->data;
    matrixData[1] *= scale; 
    matrixData[5] *= scale; 
    matrixData[9] *= scale; 
}

void kzcMatrix4x4ScaleZ(struct KzcMatrix4x4* matrix, kzFloat scale)
{
    kzFloat *matrixData = matrix->data;
    matrixData[2] *= scale; 
    matrixData[6] *= scale; 
    matrixData[10] *= scale; 
}

void kzcMatrix4x4Multiply(const struct KzcMatrix4x4* matrix1, const struct KzcMatrix4x4* matrix2, struct KzcMatrix4x4* out_matrix)
{
    kzFloat* matrixData          = out_matrix->data;
    kzFloat const* matrixData1   = matrix1->data;
    kzFloat const* matrixData2   = matrix2->data;

    /* 1st row */
    matrixData[0]    = matrixData1[0] * matrixData2[0]     +   matrixData1[1] * matrixData2[4]   +   matrixData1[2] * matrixData2[8]   +     matrixData1[3] * matrixData2[12];
    matrixData[1]    = matrixData1[0] * matrixData2[1]     +   matrixData1[1] * matrixData2[5]   +   matrixData1[2] * matrixData2[9]   +     matrixData1[3] * matrixData2[13];
    matrixData[2]    = matrixData1[0] * matrixData2[2]     +   matrixData1[1] * matrixData2[6]   +   matrixData1[2] * matrixData2[10]  +     matrixData1[3] * matrixData2[14];
    matrixData[3]    = matrixData1[0] * matrixData2[3]     +   matrixData1[1] * matrixData2[7]   +   matrixData1[2] * matrixData2[11]  +     matrixData1[3] * matrixData2[15];

    /* 2nd row */
    matrixData[4]    = matrixData1[4] * matrixData2[0]     +   matrixData1[5] * matrixData2[4]   +   matrixData1[6] * matrixData2[8]   +     matrixData1[7] * matrixData2[12];
    matrixData[5]    = matrixData1[4] * matrixData2[1]     +   matrixData1[5] * matrixData2[5]   +   matrixData1[6] * matrixData2[9]   +     matrixData1[7] * matrixData2[13];
    matrixData[6]    = matrixData1[4] * matrixData2[2]     +   matrixData1[5] * matrixData2[6]   +   matrixData1[6] * matrixData2[10]  +     matrixData1[7] * matrixData2[14];
    matrixData[7]    = matrixData1[4] * matrixData2[3]     +   matrixData1[5] * matrixData2[7]   +   matrixData1[6] * matrixData2[11]  +     matrixData1[7] * matrixData2[15];

    /* 3rd row */
    matrixData[8]    = matrixData1[8] * matrixData2[0]     +   matrixData1[9] * matrixData2[4]   +   matrixData1[10] * matrixData2[8]  +     matrixData1[11] * matrixData2[12];
    matrixData[9]    = matrixData1[8] * matrixData2[1]     +   matrixData1[9] * matrixData2[5]   +   matrixData1[10] * matrixData2[9]  +     matrixData1[11] * matrixData2[13];
    matrixData[10]   = matrixData1[8] * matrixData2[2]     +   matrixData1[9] * matrixData2[6]   +   matrixData1[10] * matrixData2[10] +     matrixData1[11] * matrixData2[14];
    matrixData[11]   = matrixData1[8] * matrixData2[3]     +   matrixData1[9] * matrixData2[7]   +   matrixData1[10] * matrixData2[11] +     matrixData1[11] * matrixData2[15];

    /* 4th row */
    matrixData[12]   = matrixData1[12] * matrixData2[0]    +   matrixData1[13] * matrixData2[4]  +   matrixData1[14] * matrixData2[8] +     matrixData1[15] * matrixData2[12];
    matrixData[13]   = matrixData1[12] * matrixData2[1]    +   matrixData1[13] * matrixData2[5]  +   matrixData1[14] * matrixData2[9] +     matrixData1[15] * matrixData2[13];
    matrixData[14]   = matrixData1[12] * matrixData2[2]    +   matrixData1[13] * matrixData2[6]  +   matrixData1[14] * matrixData2[10]+     matrixData1[15] * matrixData2[14];
    matrixData[15]   = matrixData1[12] * matrixData2[3]    +   matrixData1[13] * matrixData2[7]  +   matrixData1[14] * matrixData2[11]+     matrixData1[15] * matrixData2[15];

    kzsCounterIncrease("kzcMatrix4x4Multiply");
}

void kzcMatrix4x4Normalize(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_matrix)
{
    kzFloat scaleXInverse = 1.0f / kzcMatrix4x4GetScaleX(matrix);
    kzFloat scaleYInverse = 1.0f / kzcMatrix4x4GetScaleY(matrix);
    kzFloat scaleZInverse = 1.0f / kzcMatrix4x4GetScaleZ(matrix);

    *out_matrix = *matrix;
    out_matrix->data[0] *= scaleXInverse;
    out_matrix->data[1] *= scaleXInverse;
    out_matrix->data[2] *= scaleXInverse;
    out_matrix->data[4] *= scaleYInverse;
    out_matrix->data[5] *= scaleYInverse;
    out_matrix->data[6] *= scaleYInverse;
    out_matrix->data[8] *= scaleZInverse;
    out_matrix->data[9] *= scaleZInverse;
    out_matrix->data[10] *= scaleZInverse;
}

void kzcMatrix4x4OrthoNormalize(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_matrix)
{
    struct KzcVector3 vecX = kzcVector3(matrix->data[0], matrix->data[4], matrix->data[8]);
    struct KzcVector3 vecY;
    struct KzcVector3 vecZ = kzcVector3(matrix->data[2], matrix->data[6], matrix->data[10]);

    kzcVector3Normalize(&vecX);
    kzcVector3Cross(&vecX, &vecZ, &vecY);
    kzcVector3Normalize(&vecY);
    kzcVector3Cross(&vecX, &vecY, &vecZ);

    out_matrix->data[0] = vecX.data[0];
    out_matrix->data[1] = vecY.data[0];
    out_matrix->data[2] = vecZ.data[0];
    out_matrix->data[3] = matrix->data[3];
    out_matrix->data[4] = vecX.data[1];
    out_matrix->data[5] = vecY.data[1];
    out_matrix->data[6] = vecZ.data[1];
    out_matrix->data[7] = matrix->data[7];
    out_matrix->data[8] = vecX.data[2];
    out_matrix->data[9] = vecY.data[2];
    out_matrix->data[10] = vecZ.data[2];
    out_matrix->data[11] = matrix->data[11];
    out_matrix->data[12] = matrix->data[12];
    out_matrix->data[13] = matrix->data[13];
    out_matrix->data[14] = matrix->data[14];
    out_matrix->data[15] = matrix->data[15];
}

void kzcMatrix4x4OrthoNormalizePreserveScale(const struct KzcMatrix4x4* matrix, struct KzcMatrix4x4* out_matrix)
{
    kzFloat scaleY = kzcMatrix4x4GetScaleY(matrix);
    kzFloat scaleZ = kzcMatrix4x4GetScaleZ(matrix);
    struct KzcVector3 vecX = kzcVector3(matrix->data[0], matrix->data[4], matrix->data[8]);
    struct KzcVector3 vecY;
    struct KzcVector3 vecZ = kzcVector3(matrix->data[2], matrix->data[6], matrix->data[10]);

    kzcVector3Cross(&vecX, &vecZ, &vecY);
    kzcVector3Normalize(&vecY);
    kzcVector3Cross(&vecX, &vecY, &vecZ);
    kzcVector3Normalize(&vecZ);

    out_matrix->data[0] = vecX.data[0];
    out_matrix->data[1] = vecY.data[0] * scaleY;
    out_matrix->data[2] = vecZ.data[0] * scaleZ;
    out_matrix->data[3] = matrix->data[3];
    out_matrix->data[4] = vecX.data[1];
    out_matrix->data[5] = vecY.data[1] * scaleY;
    out_matrix->data[6] = vecZ.data[1] * scaleZ;
    out_matrix->data[7] = matrix->data[7];
    out_matrix->data[8] = vecX.data[2];
    out_matrix->data[9] = vecY.data[2] * scaleY;
    out_matrix->data[10] = vecZ.data[2] * scaleZ;
    out_matrix->data[11] = matrix->data[11];
    out_matrix->data[12] = matrix->data[12];
    out_matrix->data[13] = matrix->data[13];
    out_matrix->data[14] = matrix->data[14];
    out_matrix->data[15] = matrix->data[15];
}

void kzcMatrix4x4MultiplyAffine(const struct KzcMatrix4x4* matrix1, const struct KzcMatrix4x4* matrix2, struct KzcMatrix4x4* out_matrix)
{
    kzFloat* matrixData          = out_matrix->data;
    kzFloat const* matrixData1   = matrix1->data;
    kzFloat const* matrixData2   = matrix2->data;

    /* 1st row */
    matrixData[0]    = matrixData1[0] * matrixData2[0]     +   matrixData1[1] * matrixData2[4]   +   matrixData1[2] * matrixData2[8];
    matrixData[1]    = matrixData1[0] * matrixData2[1]     +   matrixData1[1] * matrixData2[5]   +   matrixData1[2] * matrixData2[9];
    matrixData[2]    = matrixData1[0] * matrixData2[2]     +   matrixData1[1] * matrixData2[6]   +   matrixData1[2] * matrixData2[10];
    matrixData[3]    = 0.0f;

    /* 2nd row */
    matrixData[4]    = matrixData1[4] * matrixData2[0]     +   matrixData1[5] * matrixData2[4]   +   matrixData1[6] * matrixData2[8];
    matrixData[5]    = matrixData1[4] * matrixData2[1]     +   matrixData1[5] * matrixData2[5]   +   matrixData1[6] * matrixData2[9];
    matrixData[6]    = matrixData1[4] * matrixData2[2]     +   matrixData1[5] * matrixData2[6]   +   matrixData1[6] * matrixData2[10];
    matrixData[7]    = 0.0f;

    /* 3rd row */
    matrixData[8]    = matrixData1[8] * matrixData2[0]     +   matrixData1[9] * matrixData2[4]   +   matrixData1[10] * matrixData2[8];
    matrixData[9]    = matrixData1[8] * matrixData2[1]     +   matrixData1[9] * matrixData2[5]   +   matrixData1[10] * matrixData2[9];
    matrixData[10]   = matrixData1[8] * matrixData2[2]     +   matrixData1[9] * matrixData2[6]   +   matrixData1[10] * matrixData2[10];
    matrixData[11]   = 0.0f;

    /* 4th row */
    matrixData[12]   = matrixData1[12] * matrixData2[0]    +   matrixData1[13] * matrixData2[4]  +   matrixData1[14] * matrixData2[8] + matrixData2[12];
    matrixData[13]   = matrixData1[12] * matrixData2[1]    +   matrixData1[13] * matrixData2[5]  +   matrixData1[14] * matrixData2[9] + matrixData2[13];
    matrixData[14]   = matrixData1[12] * matrixData2[2]    +   matrixData1[13] * matrixData2[6]  +   matrixData1[14] * matrixData2[10] + matrixData2[14];
    matrixData[15]   = 1.0f;

    kzsCounterIncrease("kzcMatrix4x4MultiplyAffine");
}

kzFloat kzcMatrix4x4MultiplyAffineGetTranslationZ(const struct KzcMatrix4x4* matrix1, const struct KzcMatrix4x4* matrix2)
{
    kzFloat const* matrixData1   = matrix1->data;
    kzFloat const* matrixData2   = matrix2->data;

    return matrixData1[12] * matrixData2[2]    +   matrixData1[13] * matrixData2[6]  +   matrixData1[14] * matrixData2[10] + matrixData2[14];
}

void kzcMatrix4x4MultiplyVector3(const struct KzcMatrix4x4* matrix, const struct KzcVector3* vector, struct KzcVector3* out_vector)
{
    kzcVector3Set(out_vector,
                  matrix->data[0] * kzcVector3GetX(vector) + matrix->data[4] * kzcVector3GetY(vector) + matrix->data[8] * kzcVector3GetZ(vector) + matrix->data[12],
                  matrix->data[1] * kzcVector3GetX(vector) + matrix->data[5] * kzcVector3GetY(vector) + matrix->data[9] * kzcVector3GetZ(vector) + matrix->data[13],
                  matrix->data[2] * kzcVector3GetX(vector) + matrix->data[6] * kzcVector3GetY(vector) + matrix->data[10] * kzcVector3GetZ(vector) + matrix->data[14]);
}

void kzcMatrix4x4MultiplyVector3By3x3(const struct KzcMatrix4x4* matrix, const struct KzcVector3* vector, struct KzcVector3* out_vector)
{
    kzcVector3Set(out_vector,
                  matrix->data[0] * kzcVector3GetX(vector) + matrix->data[4] * kzcVector3GetY(vector) + matrix->data[8] * kzcVector3GetZ(vector),
                  matrix->data[1] * kzcVector3GetX(vector) + matrix->data[5] * kzcVector3GetY(vector) + matrix->data[9] * kzcVector3GetZ(vector),
                  matrix->data[2] * kzcVector3GetX(vector) + matrix->data[6] * kzcVector3GetY(vector) + matrix->data[10] * kzcVector3GetZ(vector));
}

void kzcMatrix4x4MultiplyVector4(const struct KzcMatrix4x4* matrix, const struct KzcVector4* vector, struct KzcVector4* out_vector)
{
    out_vector->data[0] = matrix->data[ 0] * vector->data[0] +
                          matrix->data[ 4] * vector->data[1] +
                          matrix->data[ 8] * vector->data[2] +
                          matrix->data[12] * vector->data[3];

    out_vector->data[1] = matrix->data[ 1] * vector->data[0] +
                          matrix->data[ 5] * vector->data[1] +
                          matrix->data[ 9] * vector->data[2] +
                          matrix->data[13] * vector->data[3];

    out_vector->data[2] = matrix->data[ 2] * vector->data[0] +
                          matrix->data[ 6] * vector->data[1] +
                          matrix->data[10] * vector->data[2] +
                          matrix->data[14] * vector->data[3];

    out_vector->data[3] = matrix->data[ 3] * vector->data[0] +
                          matrix->data[ 7] * vector->data[1] +
                          matrix->data[11] * vector->data[2] +
                          matrix->data[15] * vector->data[3];
}

void kzcMatrix4x4GetPosition(const struct KzcMatrix4x4* matrix, struct KzcVector3* out_position)
{
    kzcVector3Set(out_position, matrix->data[12], matrix->data[13], matrix->data[14]);
}

void kzcMatrix4x4SetPositionVector3(struct KzcMatrix4x4* matrix, const struct KzcVector3* position)
{
    matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] = kzcVector3GetX(position);
    matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] = kzcVector3GetY(position);
    matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] = kzcVector3GetZ(position);
}

kzFloat kzcMatrix4x4Determinant(const struct KzcMatrix4x4* matrix)
{
    kzFloat const* data = matrix->data;
    kzFloat determinant = data[3] * data[6] * data[9]  * data[12] - data[2] * data[7] * data[9]  * data[12] - 
                          data[3] * data[5] * data[10] * data[12] + data[1] * data[7] * data[01] * data[12] +
                          data[2] * data[5] * data[11] * data[12] - data[1] * data[6] * data[11] * data[12] - 
                          data[3] * data[6] * data[8]  * data[13] + data[2] * data[7] * data[8]  * data[13] +
                          data[3] * data[4] * data[10] * data[13] - data[0] * data[7] * data[10] * data[13] - 
                          data[2] * data[4] * data[11] * data[13] + data[0] * data[6] * data[11] * data[13] +
                          data[3] * data[5] * data[8]  * data[14] - data[1] * data[7] * data[8]  * data[14] -
                          data[3] * data[4] * data[9]  * data[14] + data[0] * data[7] * data[9]  * data[14] +
                          data[1] * data[4] * data[11] * data[14] - data[0] * data[5] * data[11] * data[14] - 
                          data[2] * data[5] * data[8]  * data[15] + data[1] * data[6] * data[8]  * data[15] +
                          data[2] * data[4] * data[9]  * data[15] - data[0] * data[6] * data[9]  * data[15] - 
                          data[1] * data[4] * data[10] * data[15] + data[0] * data[5] * data[10] * data[15];
    return determinant;
}

kzFloat kzcMatrix4x4Trace(const struct KzcMatrix4x4* matrix)
{
    return (matrix->data[0] + matrix->data[5] + matrix->data[10] + 1.0f);
}

void kzcMatrix4x4CreateOrthogonalProjection(struct KzcMatrix4x4* matrix, kzFloat left, kzFloat right, kzFloat bottom,
                                            kzFloat top, kzFloat near, kzFloat far)
{
    kzcMatrix4x4SetIdentity(matrix);

    matrix->data[0] = 2.0f / (right - left);
    matrix->data[5] = 2.0f / (top - bottom);
    matrix->data[10] = -2.0f / (far - near);

    matrix->data[12] = -(right + left) / (right - left);
    matrix->data[13] = -(top + bottom) / (top - bottom);
    matrix->data[14] = -(far + near) / (far - near);
    matrix->data[15] = 1.0f;
}

void kzcMatrix4x4Interpolate(const struct KzcMatrix4x4* matrix1, const struct KzcMatrix4x4* matrix2, kzFloat t, struct KzcMatrix4x4* out_matrix)
{
    kzFloat tClamped = kzsClampf(t, 0.0f, 1.0f);
    struct KzcQuaternion rotation1;
    struct KzcQuaternion rotation2;
    struct KzcQuaternion rotation3;

    kzcQuaternionInitializeFromMatrix4x4(matrix1, &rotation1);
    kzcQuaternionInitializeFromMatrix4x4(matrix2, &rotation2);

    kzcQuaternionSlerp(&rotation1, &rotation2, tClamped, &rotation3);
    kzcQuaternionConvertToMatrix4x4(&rotation3, out_matrix);

    out_matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] = matrix1->data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] * (1.0f - t) +
        matrix2->data[KZC_MATRIX4X4_INDEX_TRANSLATION_X] * tClamped;
    out_matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] = matrix1->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] * (1.0f - t) +
        matrix2->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Y] * tClamped;
    out_matrix->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] = matrix1->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] * (1.0f - t) +
        matrix2->data[KZC_MATRIX4X4_INDEX_TRANSLATION_Z] * tClamped;
}

void kzcMatrix4x4GetEulerAngles(const struct KzcMatrix4x4* matrix, struct KzcVector3* out_anglesInRadians)
{
    struct KzcVector3 eulerAngles;

    /* TODO: Use engine provided functionality for comparing float values. but do not use float compare which uses a == b!*/
    if(!(kzsFabsf(matrix->data[2] - 1.0f) < KZ_FLOAT_EPSILON))
    {
        kzFloat cosTheta;
        eulerAngles.data[1] = -kzsAsinf(matrix->data[2]);
        cosTheta = kzsCosf(eulerAngles.data[1]);
        eulerAngles.data[0] = kzsAtan2f(matrix->data[6] / cosTheta, matrix->data[10] / cosTheta);
        eulerAngles.data[2] = kzsAtan2f(matrix->data[1] / cosTheta, matrix->data[0] / cosTheta);

    }
    else
    {
        kzFloat delta;
        eulerAngles.data[2] = 0.0f;
        delta = kzsAtan2f(matrix->data[4], matrix->data[8]);
        if(!(kzsFabsf(matrix->data[8] + 1.0f) < KZ_FLOAT_EPSILON))
        {
            eulerAngles.data[1] = KZS_PI * 0.5f;
            eulerAngles.data[0] = eulerAngles.data[2] + delta;
        }
        else
        {
            eulerAngles.data[1] = -KZS_PI * 0.5f;
            eulerAngles.data[0] = -eulerAngles.data[2] + delta;
        }
    }
    *out_anglesInRadians = eulerAngles;
}

kzBool kzcMatrix4x4IsRightAngleRotation(const struct KzcMatrix4x4* matrix, kzFloat epsilon)
{
    kzBool isRightAngle = KZ_FALSE;
    kzFloat const* data = matrix->data;
    struct KzcVector3 basisVectorA = kzcVector3(data[0], data[4], data[8]);
    struct KzcVector3 basisVectorB = kzcVector3(data[1], data[5], data[9]);
    struct KzcVector3 basisVectorC = kzcVector3(data[2], data[6], data[10]);
    kzUint countA = 0;
    kzUint countB = 0;

    /* Find the number of non-zeroes in both vectors. */
    if (kzsFabsf(basisVectorA.data[0]) > epsilon) ++countA;
    if (kzsFabsf(basisVectorA.data[1]) > epsilon) ++countA;
    if (kzsFabsf(basisVectorA.data[2]) > epsilon) ++countA;

    if (kzsFabsf(basisVectorB.data[0]) > epsilon) ++countB;
    if (kzsFabsf(basisVectorB.data[1]) > epsilon) ++countB;
    if (kzsFabsf(basisVectorB.data[2]) > epsilon) ++countB;

    if (countA == 1 && countB == 1)
    {
        struct KzcVector3 cross;
        kzcVector3Cross(&basisVectorA, &basisVectorB, &cross);
        if (kzsFabsf(basisVectorC.data[0] - cross.data[0]) < epsilon &&
            kzsFabsf(basisVectorC.data[1] - cross.data[1]) < epsilon &&
            kzsFabsf(basisVectorC.data[2] - cross.data[2]) < epsilon)
        {
            isRightAngle = KZ_TRUE;
        }
    }

    return isRightAngle;
}

kzBool kzcMatrix4x4IsSimpleLayout(const struct KzcMatrix4x4* matrix, kzFloat epsilon)
{
    struct KzcMatrix4x4 normalizedMatrix;

    kzcMatrix4x4Normalize(matrix, &normalizedMatrix);

    return kzcMatrix4x4IsRightAngleRotation(&normalizedMatrix, epsilon);
}
