/**
* \file
* Quaternion operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_QUATERNION_H
#define KZC_QUATERNION_H


#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcVector3;
struct KzcMatrix4x4;


/**
* Structure of quaternion. Quaternion defines 3D orientation. 
*/
struct KzcQuaternion
{
    kzFloat w; /**< Angle. */
    kzFloat x; /**< X axis. */
    kzFloat y; /**< Y axis. */
    kzFloat z; /**< Z axis. */
};


/** Sets identity quaternion. */
void kzcQuaternionSetIdentity(struct KzcQuaternion* quaternion);
/** Creates quaternion from vector and and angle. */
void kzcQuaternionInitializeFromVector(const struct KzcVector3* axis, kzFloat angleInRadians, struct KzcQuaternion* out_quaternion);
/** Creates quaternion from matrix. */
void kzcQuaternionInitializeFromMatrix4x4(const struct KzcMatrix4x4* matrix, struct KzcQuaternion* out_quaternion);
/** Converts quaternion from 4x4 matrix. */
void kzcQuaternionConvertToMatrix4x4(const struct KzcQuaternion* quaternion, struct KzcMatrix4x4* out_matrix);
/** Multiplies 2 quaternions, resulting rotation. */
void kzcQuaternionMultiply(const struct KzcQuaternion* quaternion1, const struct KzcQuaternion* quaternion2, struct KzcQuaternion* out_quaternion);
/** Multiplies quaternion with vector, i.e. applies rotation to vector. */
void kzcQuaternionMultiplyVector3(const struct KzcQuaternion* quaternion, const struct KzcVector3* vector3, struct KzcVector3* out_vector3);
/** Normalizes a quaternion. */
void kzcQuaternionNormalize(struct KzcQuaternion* quaternion);
/** Inverses a quaternion. */
void kzcQuaternionInverse(struct KzcQuaternion* quaternion);
/** Spherical interpolation of 2 quaternions. t as interpolation value ranging [0, 1]. */
void kzcQuaternionSlerp(const struct KzcQuaternion* quaternion1, const struct KzcQuaternion* quaternion2, kzFloat t, struct KzcQuaternion* out_quaternion);


#endif
