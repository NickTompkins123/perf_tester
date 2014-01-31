/**
* \file
* 2D vector.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_vector2.h"


const struct KzcVector2 KZC_VECTOR2_ZERO = {{0.0f, 0.0f}};
const struct KzcVector2 KZC_VECTOR2_X_AXIS = {{1.0f, 0.0f}};
const struct KzcVector2 KZC_VECTOR2_Y_AXIS = {{0.0f, 1.0f}};

void kzcVector2SetInvalid(struct KzcVector2* vector)
{
    vector->data[0] = KZS_NAN;
    vector->data[1] = KZS_NAN;
}
