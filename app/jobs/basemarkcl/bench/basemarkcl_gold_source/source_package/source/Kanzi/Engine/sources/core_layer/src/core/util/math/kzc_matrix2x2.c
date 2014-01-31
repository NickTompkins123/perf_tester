/**
* \file
* Matrix2x2 structure and operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_matrix2x2.h"


#include <core/util/math/kzc_vector2.h>

#include <system/debug/kzs_log.h>


const struct KzcMatrix2x2 KZC_MATRIX2X2_IDENTITY = { { 1.0f, 0.0f,
                                                       0.0f, 1.0f } };


void kzcMatrix2x2SetIdentity(struct KzcMatrix2x2* matrix)
{
    *matrix = KZC_MATRIX2X2_IDENTITY;
}

void kzcMatrix2x2SetInvalid(struct KzcMatrix2x2* matrix)
{
    matrix->data[0] = KZS_NAN;
    matrix->data[1] = KZS_NAN;
    matrix->data[2] = KZS_NAN;
    matrix->data[3] = KZS_NAN;
}

void kzcMatrix2x2Transpose(const struct KzcMatrix2x2* matrix, struct KzcMatrix2x2* out_matrix)
{
    const kzFloat *inData = matrix->data;
    kzFloat *outData = out_matrix->data;

    outData[0] = inData[0];     outData[1] = inData[2];
    outData[2] = inData[1];     outData[3] = inData[3];
}

void kzcMatrix2x2Inverse(const struct KzcMatrix2x2* matrix, struct KzcMatrix2x2* out_matrix)
{
    kzFloat const* matrixData = matrix->data;
    kzFloat determinant = kzcMatrix2x2Determinant(matrix);

    if(determinant < KZ_FLOAT_EPSILON && determinant > -KZ_FLOAT_EPSILON)
    {
        kzsLog(KZS_LOG_LEVEL_WARNING, "Matrix with determinant = 0 does not have inverse!");
        *out_matrix = KZC_MATRIX2X2_IDENTITY;
    }
    else
    {
        struct KzcMatrix2x2 temp;
        kzFloat detInv = 1.0f / determinant;

        temp.data[0] = detInv * matrixData[3];
        temp.data[1] = -detInv * matrixData[1];
        temp.data[2] = -detInv * matrixData[2];
        temp.data[3] = detInv * matrixData[0];

        *out_matrix = temp;
    }
}

void kzcMatrix2x2Multiply(const struct KzcMatrix2x2* matrix1, const struct KzcMatrix2x2* matrix2, struct KzcMatrix2x2* out_matrix)
{
    kzFloat* matrixData          = out_matrix->data;
    kzFloat const* matrixData1   = matrix1->data;
    kzFloat const* matrixData2   = matrix2->data;

    matrixData[0]    = matrixData1[0] * matrixData2[0]     +   matrixData1[1] * matrixData2[2];
    matrixData[1]    = matrixData1[0] * matrixData2[1]     +   matrixData1[1] * matrixData2[3];

    matrixData[2]    = matrixData1[2] * matrixData2[0]     +   matrixData1[3] * matrixData2[2];
    matrixData[3]    = matrixData1[2] * matrixData2[1]     +   matrixData1[3] * matrixData2[3];
}

void kzcMatrix2x2MultiplyVector2(const struct KzcMatrix2x2* matrix, const struct KzcVector2* vector, struct KzcVector2* out_vector)
{
    kzFloat x = kzcVector2GetX(vector);
    kzFloat y = kzcVector2GetY(vector);
    kzcVector2Set(out_vector,
        matrix->data[0] * x + matrix->data[2] * y,
        matrix->data[1] * x + matrix->data[3] * y);
}

void kzcMatrix2x2Rotate(struct KzcMatrix2x2* matrix, kzFloat angleInRadians)
{
    struct KzcMatrix2x2 rotationMatrix;
    struct KzcMatrix2x2 result;

    const kzFloat sinTerm = kzsSinf(angleInRadians);
    const kzFloat cosTerm = kzsCosf(angleInRadians);

    kzcMatrix2x2SetIdentity(&rotationMatrix);

    rotationMatrix.data[0] = cosTerm;    rotationMatrix.data[1] = -sinTerm;
    rotationMatrix.data[2] = sinTerm;    rotationMatrix.data[3] = cosTerm;

    kzcMatrix2x2Multiply(&rotationMatrix, matrix, &result);
    *matrix = result;
}

void kzcMatrix2x2ScaleUniform(struct KzcMatrix2x2* matrix, kzFloat scale)
{
    kzFloat *matrixData = matrix->data;

    matrixData[0] *= scale; matrixData[1] *= scale;
    matrixData[2] *= scale; matrixData[3] *= scale;
}

void kzcMatrix2x2Scale(struct KzcMatrix2x2* matrix, kzFloat scaleX, kzFloat scaleY)
{
    kzFloat *matrixData = matrix->data;

    matrixData[0] *= scaleX; matrixData[1] *= scaleY;
    matrixData[2] *= scaleX; matrixData[3] *= scaleY;
}

kzFloat kzcMatrix2x2GetScaleX(struct KzcMatrix2x2* matrix)
{
    kzFloat *matrixData = matrix->data;
    return kzsSqrtf(matrixData[0] * matrixData[0] + matrixData[1] * matrixData[1]);
}

kzFloat kzcMatrix2x2GetScaleY(struct KzcMatrix2x2* matrix)
{
    kzFloat *matrixData = matrix->data;
    return kzsSqrtf(matrixData[2] * matrixData[2] + matrixData[3] * matrixData[3]);
}

void kzcMatrix2x2ScaleX(struct KzcMatrix2x2* matrix, kzFloat scale)
{
    kzFloat *matrixData = matrix->data;
    matrixData[0] *= scale; 
    matrixData[2] *= scale; 
}

void kzcMatrix2x2ScaleY(struct KzcMatrix2x2* matrix, kzFloat scale)
{
    kzFloat *matrixData = matrix->data;
    matrixData[1] *= scale; 
    matrixData[3] *= scale; 
}

kzFloat kzcMatrix2x2Determinant(const struct KzcMatrix2x2* matrix)
{
    return matrix->data[0] * matrix->data[3] - matrix->data[1] * matrix->data[2];
}

kzFloat kzcMatrix2x2Trace(const struct KzcMatrix2x2* matrix)
{
    return (matrix->data[0] + matrix->data[3]);
}
