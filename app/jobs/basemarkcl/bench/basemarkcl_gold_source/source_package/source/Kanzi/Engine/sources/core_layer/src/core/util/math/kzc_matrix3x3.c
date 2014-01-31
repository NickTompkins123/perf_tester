/**
* \file
* Matrix3x3 structure and operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_matrix3x3.h"


#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_vector3.h>

#include <system/debug/kzs_counter.h>
#include <system/debug/kzs_log.h>


const struct KzcMatrix3x3 KZC_MATRIX3X3_IDENTITY = { { 1.0f, 0.0f, 0.0f,
                                                       0.0f, 1.0f, 0.0f,
                                                       0.0f, 0.0f, 1.0f } };


/** Helper function for matrix determinant. Returns a determinant of 2x2 part of a matrix. */
static kzFloat kzcMatrixDeterminant2x2_internal(const struct KzcMatrix3x3* matrix)
{
    kzFloat const* matrixData = matrix->data;
    kzFloat determinant = matrixData[0] * matrixData[4] - matrixData[1] * matrixData[3];

    return determinant;
}


void kzcMatrix3x3SetIdentity(struct KzcMatrix3x3* matrix)
{
    *matrix = KZC_MATRIX3X3_IDENTITY;
}

void kzcMatrix3x3SetInvalid(struct KzcMatrix3x3* matrix)
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
}

void kzcMatrix3x3SetIdentity2x2(struct KzcMatrix3x3* matrix)
{
    matrix->data[0] = 1.0f;
    matrix->data[1] = 0.0f;
    matrix->data[3] = 0.0f;
    matrix->data[4] = 1.0f;
}

void kzcMatrix3x3ResetTranslation(struct KzcMatrix3x3* matrix)
{
    matrix->data[KZC_MATRIX3X3_INDEX_TRANSLATION_X] = 0.0f;
    matrix->data[KZC_MATRIX3X3_INDEX_TRANSLATION_Y] = 0.0f;
}

void kzcMatrix3x3Transpose(const struct KzcMatrix3x3* matrix, struct KzcMatrix3x3* out_matrix)
{
    const kzFloat *inData = matrix->data;
    kzFloat *outData = out_matrix->data;

    outData[0] = inData[0];     outData[1] = inData[3];     outData[2] = inData[6];
    outData[3] = inData[1];     outData[4] = inData[4];     outData[5] = inData[7];
    outData[6] = inData[2];     outData[7] = inData[5];     outData[8] = inData[8];
}

void kzcMatrix3x3Inverse(const struct KzcMatrix3x3* matrix, struct KzcMatrix3x3* out_matrix)
{
    const kzFloat determinant = kzcMatrixDeterminant2x2_internal(matrix);

    if(determinant < KZ_FLOAT_EPSILON && determinant > -KZ_FLOAT_EPSILON)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Matrix with determinant = 0 does not have inverse!");
        *out_matrix = KZC_MATRIX3X3_IDENTITY;
    }
    else
    {
        struct KzcMatrix3x3 temp;
        struct KzcMatrix3x3 transpose;
        kzFloat const* matrixData = matrix->data;
        kzFloat detInv;
        kzFloat det11;
        kzFloat det12;
        kzFloat det21;
        kzFloat det22;
        /* Transpose of matrix */
        kzcMatrix3x3Transpose(matrix, &transpose);

        /* Actual inverse of 3x3 matrix is defined: 1 / det(M) * (adjugate(M)) */
        det11 = transpose.data[4] * transpose.data[8] - transpose.data[5] * transpose.data[7];
        det12 = transpose.data[5] * transpose.data[6] - transpose.data[3] * transpose.data[8];
        det21 = transpose.data[7] * transpose.data[2] - transpose.data[8] * transpose.data[1];
        det22 = transpose.data[8] * transpose.data[0] - transpose.data[6] * transpose.data[2];

        /* Last row & column of matrix can be safely be identity only, since rest of the matrix content is filled with sub determinants */
        detInv = 1.0f / determinant;
        temp.data[0] = det11 * detInv;
        temp.data[1] = -det12 * detInv;
        temp.data[2] = 0.0f;
        temp.data[3] = -det21 * detInv;
        temp.data[4] = det22 * detInv;
        temp.data[5] = 0.0f;
        temp.data[6] = 0.0f;
        temp.data[7] = 0.0f;
        temp.data[8] = 1.0f;

        kzcMatrix3x3Translate(&temp, -matrixData[KZC_MATRIX3X3_INDEX_TRANSLATION_X], -matrixData[KZC_MATRIX3X3_INDEX_TRANSLATION_Y]);
        *out_matrix = temp;
    }
}

void kzcMatrix3x3Translate(struct KzcMatrix3x3* matrix, kzFloat x, kzFloat y)
{
    kzFloat* matrixData  = matrix->data;

    matrixData[KZC_MATRIX3X3_INDEX_TRANSLATION_X] += matrixData[0] * x + matrixData[3] * y;
    matrixData[KZC_MATRIX3X3_INDEX_TRANSLATION_Y] += matrixData[1] * x + matrixData[4] * y;
}

void kzcMatrix3x3TranslateVector2(struct KzcMatrix3x3* matrix, const struct KzcVector2* translation)
{
    kzcMatrix3x3Translate(matrix, kzcVector2GetX(translation), kzcVector2GetY(translation));
}

void kzcMatrix3x3Rotate(struct KzcMatrix3x3* matrix, kzFloat angleInRadians)
{
    struct KzcMatrix3x3 rotationMatrix;
    struct KzcMatrix3x3 result;

    const kzFloat sinTerm = kzsSinf(angleInRadians);
    const kzFloat cosTerm = kzsCosf(angleInRadians);

    kzcMatrix3x3SetIdentity(&rotationMatrix);

    rotationMatrix.data[0] = cosTerm;    rotationMatrix.data[1] = -sinTerm;
    rotationMatrix.data[3] = sinTerm;    rotationMatrix.data[4] = cosTerm;

    kzcMatrix3x3Multiply(&rotationMatrix, matrix, &result);
    *matrix = result;
}

void kzcMatrix3x3ScaleUniform(struct KzcMatrix3x3* matrix, kzFloat scale)
{
    kzFloat *matrixData = matrix->data;

    matrixData[0] *= scale; matrixData[1] *= scale;
    matrixData[3] *= scale; matrixData[4] *= scale;
}

void kzcMatrix3x3Scale(struct KzcMatrix3x3* matrix, kzFloat scaleX, kzFloat scaleY)
{
    kzFloat *matrixData = matrix->data;

    matrixData[0] *= scaleX; matrixData[1] *= scaleY;
    matrixData[3] *= scaleX; matrixData[4] *= scaleY;
}

kzFloat kzcMatrix3x3GetScaleX(const struct KzcMatrix3x3* matrix)
{
    const kzFloat *matrixData = matrix->data;
    return kzsSqrtf(matrixData[0] * matrixData[0] + matrixData[1] * matrixData[1] + matrixData[2] * matrixData[2]);
}

kzFloat kzcMatrix3x3GetScaleY(const struct KzcMatrix3x3* matrix)
{
    const kzFloat *matrixData = matrix->data;
    return kzsSqrtf(matrixData[3] * matrixData[3] + matrixData[4] * matrixData[4] + matrixData[5] * matrixData[5]);
}

void kzcMatrix3x3ScaleX(struct KzcMatrix3x3* matrix, kzFloat scale)
{
    kzFloat *matrixData = matrix->data;
    matrixData[0] *= scale; 
    matrixData[3] *= scale; 
    matrixData[6] *= scale; 
}

void kzcMatrix3x3ScaleY(struct KzcMatrix3x3* matrix, kzFloat scale)
{
    kzFloat *matrixData = matrix->data;
    matrixData[1] *= scale; 
    matrixData[4] *= scale; 
    matrixData[7] *= scale; 
}

void kzcMatrix3x3Normalize(const struct KzcMatrix3x3* matrix, struct KzcMatrix3x3* out_matrix)
{
    kzFloat scaleXInverse = 1.0f / kzcMatrix3x3GetScaleX(matrix);
    kzFloat scaleYInverse = 1.0f / kzcMatrix3x3GetScaleY(matrix);

    *out_matrix = *matrix;
    out_matrix->data[0] *= scaleXInverse;
    out_matrix->data[1] *= scaleXInverse;
    out_matrix->data[3] *= scaleYInverse;
    out_matrix->data[4] *= scaleYInverse;
}

void kzcMatrix3x3Multiply(const struct KzcMatrix3x3* matrix1, const struct KzcMatrix3x3* matrix2, struct KzcMatrix3x3* out_matrix)
{
    kzFloat* matrixData          = out_matrix->data;
    kzFloat const* matrixData1   = matrix1->data;
    kzFloat const* matrixData2   = matrix2->data;

    /* 1st row */
    matrixData[0]    = matrixData1[0] * matrixData2[0]     +   matrixData1[1] * matrixData2[3]   +   matrixData1[2] * matrixData2[6];
    matrixData[1]    = matrixData1[0] * matrixData2[1]     +   matrixData1[1] * matrixData2[4]   +   matrixData1[2] * matrixData2[7];
    matrixData[2]    = matrixData1[0] * matrixData2[2]     +   matrixData1[1] * matrixData2[5]   +   matrixData1[2] * matrixData2[8];
    
    /* 2nd row */
    matrixData[3]    = matrixData1[3] * matrixData2[0]     +   matrixData1[4] * matrixData2[3]   +   matrixData1[5] * matrixData2[6];
    matrixData[4]    = matrixData1[3] * matrixData2[1]     +   matrixData1[4] * matrixData2[4]   +   matrixData1[5] * matrixData2[7];
    matrixData[5]    = matrixData1[3] * matrixData2[2]     +   matrixData1[4] * matrixData2[5]   +   matrixData1[5] * matrixData2[8];

    /* 3rd row */
    matrixData[6]    = matrixData1[6] * matrixData2[0]     +   matrixData1[7] * matrixData2[3]   +   matrixData1[8] * matrixData2[6];
    matrixData[7]    = matrixData1[6] * matrixData2[1]     +   matrixData1[7] * matrixData2[4]   +   matrixData1[8] * matrixData2[7];
    matrixData[8]    = matrixData1[6] * matrixData2[2]     +   matrixData1[7] * matrixData2[5]   +   matrixData1[8] * matrixData2[8];

    kzsCounterIncrease("kzcMatrix3x3Multiply");
}

void kzcMatrix3x3MultiplyVector2(const struct KzcMatrix3x3* matrix, const struct KzcVector2* vector, struct KzcVector2* out_vector)
{
    kzcVector2Set(out_vector,
        matrix->data[0] * kzcVector2GetX(vector) + matrix->data[3] * kzcVector2GetY(vector) + matrix->data[6],
        matrix->data[1] * kzcVector2GetX(vector) + matrix->data[4] * kzcVector2GetY(vector) + matrix->data[7]);
}

void kzcMatrix3x3MultiplyVector3(const struct KzcMatrix3x3* matrix, const struct KzcVector3* vector, struct KzcVector3* out_vector)
{
    kzcVector3Set(out_vector,
        matrix->data[0] * kzcVector3GetX(vector) + matrix->data[3] * kzcVector3GetY(vector) + matrix->data[6],
        matrix->data[1] * kzcVector3GetX(vector) + matrix->data[4] * kzcVector3GetY(vector) + matrix->data[7],
        1.0f);
}


void kzcMatrix3x3GetPosition(const struct KzcMatrix3x3* matrix, struct KzcVector2* out_position)
{
    kzcVector2Set(out_position, matrix->data[KZC_MATRIX3X3_INDEX_TRANSLATION_X], matrix->data[KZC_MATRIX3X3_INDEX_TRANSLATION_Y]);
}

void kzcMatrix3x3SetPositionVector3(struct KzcMatrix3x3* matrix, const struct KzcVector2* position)
{
    matrix->data[KZC_MATRIX3X3_INDEX_TRANSLATION_X] = kzcVector2GetX(position);
    matrix->data[KZC_MATRIX3X3_INDEX_TRANSLATION_Y] = kzcVector2GetY(position);
}

kzFloat kzcMatrix3x3Determinant(const struct KzcMatrix3x3* matrix)
{
    kzFloat const* matrixData = matrix->data;

    kzFloat determinant1 = matrixData[4] * matrixData[8] - matrixData[7] * matrixData[5];
    kzFloat determinant2 = matrixData[3] * matrixData[8] - matrixData[6] * matrixData[5];
    kzFloat determinant3 = matrixData[3] * matrixData[7] - matrixData[6] * matrixData[4];

    return  (matrixData[0] * determinant1 - matrixData[1] * determinant2 + matrixData[2] * determinant3);
}

kzFloat kzcMatrix3x3Trace(const struct KzcMatrix3x3* matrix)
{
    return (matrix->data[0] + matrix->data[4] + 1.0f);
}
