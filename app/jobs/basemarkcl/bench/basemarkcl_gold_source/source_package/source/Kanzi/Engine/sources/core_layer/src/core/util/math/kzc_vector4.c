/**
* \file
* Vector4D operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_vector4.h"


const struct KzcVector4 KZC_VECTOR4_ZERO = {{0.0f, 0.0f, 0.0f, 0.0f}};
const struct KzcVector4 KZC_VECTOR4_X_AXIS = {{1.0f, 0.0f, 0.0f, 1.0f}};
const struct KzcVector4 KZC_VECTOR4_Y_AXIS = {{0.0f, 1.0f, 0.0f, 1.0f}};
const struct KzcVector4 KZC_VECTOR4_Z_AXIS = {{0.0f, 0.0f, 1.0f, 1.0f}};

/* TODO: Check how each vector4 operation should treat W component */

void kzcVector4SetInvalid(struct KzcVector4* vector)
{
    vector->data[0] = KZS_NAN;
    vector->data[1] = KZS_NAN;
    vector->data[2] = KZS_NAN;
    vector->data[3] = KZS_NAN;
}
