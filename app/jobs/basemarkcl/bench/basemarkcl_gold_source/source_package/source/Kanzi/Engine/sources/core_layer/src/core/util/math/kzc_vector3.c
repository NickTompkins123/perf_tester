/**
* \file
* Vector3D operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_vector3.h"


const struct KzcVector3 KZC_VECTOR3_ZERO = {{0.0f, 0.0f, 0.0f}};
const struct KzcVector3 KZC_VECTOR3_X_AXIS = {{1.0f, 0.0f, 0.0f}};
const struct KzcVector3 KZC_VECTOR3_Y_AXIS = {{0.0f, 1.0f, 0.0f}};
const struct KzcVector3 KZC_VECTOR3_Z_AXIS = {{0.0f, 0.0f, 1.0f}};
const struct KzcVector3 KZC_VECTOR3_X_AXIS_NEGATIVE = {{-1.0f, 0.0f, 0.0f}};
const struct KzcVector3 KZC_VECTOR3_Y_AXIS_NEGATIVE = {{0.0f, -1.0f, 0.0f}};
const struct KzcVector3 KZC_VECTOR3_Z_AXIS_NEGATIVE = {{0.0f, 0.0f, -1.0f}};
const struct KzcVector3 KZC_VECTOR3_MAXIMUM = {{KZ_FLOAT_MAXIMUM, KZ_FLOAT_MAXIMUM, KZ_FLOAT_MAXIMUM}};
const struct KzcVector3 KZC_VECTOR3_MINIMUM = {{KZ_FLOAT_MINIMUM, KZ_FLOAT_MINIMUM, KZ_FLOAT_MINIMUM}};

void kzcVector3SetInvalid(struct KzcVector3* vector)
{
    vector->data[0] = KZS_NAN;
    vector->data[1] = KZS_NAN;
    vector->data[2] = KZS_NAN;
}
