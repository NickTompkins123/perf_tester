/**
 * \file
 * General purpose hash code functions.
 * Hash code and comparator is related such that for two objects a and b if compare(a,b) == 0 then hashCode(a) == hashCode(b).
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_HASH_CODE_H
#define KZC_HASH_CODE_H


#include <system/kzs_types.h>


/** Hash code function. Returns an unsigned 32-bit hash code for the given argument. */
typedef kzU32 (*KzcHashFunction)(const void* pointer);


/* Hash code functions for fundamental Kanzi datatypes.*/

/** Hash code function for integers. */
kzU32 kzcHashCodeFromInt(const void*);
/** Hash code function for unsigned 32 bit integers. */
kzU32 kzcHashCodeFromU32(const void*);
/** Hash code function for arbitrary pointers. */
kzU32 kzcHashCodeFromPointer(const void*);
/** Hash code function for strings. */
kzU32 kzcHashCodeFromString(const void*);


#endif

