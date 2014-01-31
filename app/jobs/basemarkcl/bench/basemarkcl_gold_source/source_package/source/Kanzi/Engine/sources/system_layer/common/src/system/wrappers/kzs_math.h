/**
* \file
* Math wrappers.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_MATH_H
#define KZS_MATH_H


/* System */
#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

/* Standard library */
#include <math.h>
#include <float.h> /*lint -efile(766,float.h) This file is referenced by macros. */
#include <limits.h> /*lint -efile(766,limits.h) This file is referenced by macros. */
#include <stdlib.h>

/* Disable warnings of using same local variable name as math.h might declare as global on at least win32 platform. */
/*lint -esym(578,y1) */

/** Pi. */
#define KZS_PI 3.1415926536f

/** Largest value for floating point number */
#define KZ_FLOAT_MAXIMUM FLT_MAX
/** Largest value for floating point number */
#define KZ_FLOAT_MINIMUM (-FLT_MAX)
/** Smallest value larger than positive zero. */
#define KZ_FLOAT_SMALLEST_POSITIVE FLT_MIN
/** Smallest delta between 2 floating point numbers. */
#define KZ_FLOAT_EPSILON 1.0e-20

/** Largest value for integer. */
#define KZ_INT_MAXIMUM INT_MAX
/** Smallest value for integer. */
#define KZ_INT_MINIMUM INT_MIN

/** Largest value for unsigned integer. */
#define KZ_UINT_MAXIMUM UINT_MAX
/** Smallest value for unsigned integer. */
#define KZ_UINT_MINIMUM 0


/** \see KZS_NAN */
extern const kzUint KZS_NAN_PRIVATE;
/** Floating point value for invalid number. */
#define KZS_NAN /*lint -e(740)*/ (*(kzFloat*)&KZS_NAN_PRIVATE)


/** Returns sine of given angle. */
KZ_INLINE kzFloat kzsSinf(kzFloat angle)
{
    return (kzFloat)sin((kzDouble)angle);
}

/** Returns cosine of given angle. */
KZ_INLINE kzFloat kzsCosf(kzFloat angle)
{
    return (kzFloat)cos((kzDouble)angle);
}

/** Returns inverse sine from floating point number between -1 and 1. */
KZ_INLINE kzFloat kzsAsinf(kzFloat x)
{
    return (kzFloat)asin((kzDouble)x);
}

/** Returns inverse cosine from floating point number between -1 and 1. */
KZ_INLINE kzFloat kzsAcosf(kzFloat x)
{
    return (kzFloat)acos((kzDouble)x);
}

/** Returns tangent of given angle. */
KZ_INLINE kzFloat kzsTanf(kzFloat angle)
{
    return (kzFloat)tan((kzDouble)angle);
}

/** Returns atan from floating point number. */
KZ_INLINE kzFloat kzsAtanf(kzFloat x)
{
    return (kzFloat)atan((kzDouble)x);
}

/** Returns atan2 from floating point number. */
KZ_INLINE kzFloat kzsAtan2f(kzFloat y, kzFloat x)
{
    return (kzFloat)atan2((kzDouble)y, (kzDouble)x);
}


/** Returns square root of given value. */
KZ_INLINE kzFloat kzsSqrtf(kzFloat value)
{
    return (kzFloat)sqrt((kzDouble)value);
}

/** Returns the signum of given value. */
KZ_INLINE kzFloat kzsSignf(kzFloat value)
{
    return (value < 0.0f) ? -1.0f : 1.0f;
}

/** Floating point remainder of value divided by divisor. */
KZ_INLINE kzFloat kzsFmod(kzFloat value, kzFloat divisor)
{
    return (kzFloat)fmod((kzDouble)value, (kzDouble)divisor);
}

/** Breaks floating point number to integral and decimal part. Returns integral part as return value and sets intPart to decimal part. */
KZ_INLINE kzFloat kzsModf(kzFloat value, kzFloat* intPart)
{
    kzDouble intPartTemp;
    kzFloat result = (kzFloat)modf((kzDouble)value, &intPartTemp);
    *intPart = (kzFloat)intPartTemp;
    return result;
}

/**
 * Returns modulo dividend % divisor for signed numbers in a way that negative numbers are
 * treated as an extension instead of mirroring compared to positive numbers.
 * For example kzsSModf(-16, 5) = 4
 */
KZ_INLINE kzUint kzsSMod(kzInt dividend, kzUint divisor)
{
    kzInt remainder = dividend % (kzInt)divisor;
    return (kzUint)((dividend >= 0 || remainder == 0) ?
                    remainder :
                    (dividend - (dividend / (kzInt)divisor - 1) * (kzInt)divisor)); /* SMod(-16, 5) = (-16 - (-16 / 5 - 1) * 5) = (-16 - (-4) * 5) = -16 + 20 = 4 */
}

/** Powers value to given exponent. */
KZ_INLINE kzFloat kzsPowf(kzFloat value, kzFloat exponent)
{
    return (kzFloat)pow((kzDouble)value, (kzDouble)exponent);
}

/** Takes 10-base logarithm from floating point value. */
KZ_INLINE kzFloat kzsLog10f(kzFloat value)
{
    return (kzFloat)log10((kzDouble)value);
}

/** Takes e (neper) -base logarithm from double value. */
KZ_INLINE kzDouble kzsLogE(kzDouble value)
{
    return (kzDouble)log(value);
}

/** Takes e (neper) -base logarithm from floating piont value. */
KZ_INLINE kzFloat kzsLogEf(kzFloat value)
{
    return (kzFloat)log((kzDouble)value);
}

/** Returns the largest integer not greater than value as float. */
KZ_INLINE kzFloat kzsFloorf(kzFloat value)
{
    return (kzFloat)floor((kzDouble)value);
}

/** Returns the largest integer not greater than value as integer. */
KZ_INLINE kzInt kzsIFloorf(kzFloat value)
{
    kzInt intValue = (kzInt)value;
    return intValue <= value ? intValue : intValue - 1;
}

/** Returns the smallest integer no less than value in float. */
KZ_INLINE kzFloat kzsCeilf(kzFloat value)
{
    return (kzFloat)ceil((kzDouble)value);
}

/** Returns the smallest integer no less than value in integer. */
KZ_INLINE kzInt kzsICeilf(kzFloat value)
{
    kzInt intValue = (kzInt)value;
    return intValue >= value ? intValue : intValue + 1;
}

/** Returns absolute value from integer value. */
KZ_INLINE kzUint kzsAbs(kzInt value)
{
    return (kzUint)(value >= 0 ? value : -value);
}

/** Returns absolute value from floating point number. */
KZ_INLINE kzFloat kzsFabsf(kzFloat value)
{
    return (kzFloat)fabs((kzDouble)value);
}

/** Returns the exponential value from floating point number. */
KZ_INLINE kzFloat kzsExp(kzFloat value)
{
    return (kzFloat)exp((kzDouble)value);
}

/** Clamps float value to given range */
KZ_INLINE kzFloat kzsClampf(kzFloat value, kzFloat minimum, kzFloat maximum)
{
    kzsAssert(maximum >= minimum);
    return (value <= minimum) ? minimum : ((value >= maximum) ? maximum : value);
}

/** Clamps integer value to given range */
KZ_INLINE kzInt kzsClampi(kzInt value, kzInt minimum, kzInt maximum)
{
    kzsAssert(maximum >= minimum);
    return (value <= minimum) ? minimum : ((value >= maximum) ? maximum : value);
}

/** Clamps unsigned integer value to given range */
KZ_INLINE kzUint kzsClampui(kzUint value, kzUint minimum, kzUint maximum)
{
    kzsAssert(maximum >= minimum);
    return (value <= minimum) ? minimum : ((value >= maximum) ? maximum : value);
}

/** Linearly interpolate between [a,b] where t should be in range [0,1]. */
KZ_INLINE kzFloat kzsLerpf(kzFloat minimum, kzFloat maximum, kzFloat t)
{
    kzsAssert(maximum >= minimum);
    return minimum + t * (maximum - minimum);
}

/** Converts degrees to radians */
KZ_INLINE kzFloat kzsDegreesToRadians(kzFloat angle)
{
    return angle * KZS_PI / 180.0f;
}

/* Non-standard library functions */

/** Check whether value is in [min, max] (an interval inclusive of the limit points) */
KZ_INLINE kzBool kzsIsInClosedInterval(kzFloat value, kzFloat minimum, kzFloat maximum)
{
    kzsAssert(maximum >= minimum);
    return (value >= minimum) && (value <= maximum);
}

/** Check if two floats are equal. */
KZ_INLINE kzBool kzsFloatIsEqual(kzFloat valueA, kzFloat valueB)
{
    /* The following comparison ensures that kzsFloatIsEqual(-0.0f, 0.0f) == KZ_TRUE. */
    return valueA == valueB; /*lint !e777 */

    /* To check if two floats are almost equal, implement the function described in
       http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm */
}

/** Check whether value in (min, max) (an interval range exclusive of the limit points)  */
KZ_INLINE kzBool kzsIsInOpenInterval(kzFloat value, kzFloat minimum, kzFloat maximum)
{
    kzsAssert(maximum >= minimum);
    return (value > minimum) && (value < maximum);
}

/** Returns the smaller of two integers. */
KZ_INLINE kzInt kzsMin(kzInt a, kzInt b)
{
    return (a < b) ? a : b;
}

/** Returns the greater of two integers. */
KZ_INLINE kzInt kzsMax(kzInt a, kzInt b)
{
    return (a > b) ? a : b;
}

/** Returns the smaller of two unsigned integers. */
KZ_INLINE kzUint kzsMinU(kzUint a, kzUint b)
{
    return (a < b) ? a : b;
}

/** Returns the greater of two unsigned integers. */
KZ_INLINE kzUint kzsMaxU(kzUint a, kzUint b)
{
    return (a > b) ? a : b;
}

/** Returns the smaller of two floats. */
KZ_INLINE kzFloat kzsMinf(kzFloat a, kzFloat b)
{
    return (a < b) ? a : b;
}

/** Returns the greater of two floats. */
KZ_INLINE kzFloat kzsMaxf(kzFloat a, kzFloat b)
{
    return (a > b) ? a : b;
}

/** Sorts values in buffer using quick sort. */
KZ_INLINE void kzsQsort(void *buffer, kzUint itemCount, kzUint itemSize, kzInt (*comparator)(const void*, const void *))
{
    qsort(buffer, itemCount, itemSize, comparator);
}

/** Casts a float to unsigned int. */
KZ_INLINE kzUint kzsFloatToUint(kzFloat value)
{
    return (kzUint)(kzInt)value; /* Direct casting of float to unsigned integer is problematic on some platforms. */
}

/** Casts a float to unsigned int. */
KZ_INLINE kzFloat kzsUintToFloat(kzUint value)
{
    return (kzFloat)(kzInt)value; /* Direct casting of float to unsigned integer is problematic on some platforms. */
}

/** Swaps two arbitrary elements of arbitrary size. */
KZ_INLINE void kzsSwap(void* element1, void* element2, kzUint elementSize)
{
    kzUint bytesLeft = elementSize;
    kzUint offset = 0;

    while (bytesLeft >= sizeof(void*))
    {
        void* temp = *((void**)element1 + offset);
        *((void**)element1 + offset) = *((void**)element2 + offset);
        *((void**)element2 + offset) = temp;
        ++offset;
        bytesLeft -= sizeof(void*);
    }

    offset *= sizeof(void*);

    while (bytesLeft > 0)
    {
        kzU8 temp = *((kzU8*)element1 + offset);
        *((kzU8*)element1 + offset) = *((kzU8*)element2 + offset);
        *((kzU8*)element2 + offset) = temp;
        ++offset;
        --bytesLeft;
    }
}


#endif
