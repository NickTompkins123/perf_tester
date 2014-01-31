/**
 * \file
 * Comparator function prototypes
 * 
 * Comparator functions of the type kzInt compare(const void* first, const void* second) 
 * which returns 0 if first == second, negative number if first < second and positive if first > second.
 * For integer values the comparator can be implemented simply as return first - second; to get natural ordering from
 * small to large.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZC_COMPARATOR_H
#define KZC_COMPARATOR_H


#include <system/kzs_types.h>


/**
 * Comparator function. 
 * Returns negative integer if first < second, 0 if they are equal and positive integer if first > second.
 */
typedef kzInt (*KzcComparatorFunction)(const void* first, const void* second);

/**
 * Comparator function with arbitrary context.
 * Same as KzcComparatorFunction with additional context parameter.
 */
typedef kzInt (*KzcComparatorWithContextFunction)(const void* first, const void* second, const void* context);


/* Comparators for fundamental Kanzi datatypes.*/

/** Comparator function for signed integers. */
kzInt kzcCompareInts(const void* first, const void* second);
/** Comparator function for unsigned 32 bit integers. */
kzInt kzcCompareU32s(const void* first, const void* second);
/** Comparator function for floating points. */
kzInt kzcCompareFloats(const void* first, const void* second);
/** Comparator function for pointers. */
kzInt kzcComparePointers(const void* first, const void* second);
/** Comparator function for strings. */
kzInt kzcCompareStrings(const void* first, const void* second);


#endif
