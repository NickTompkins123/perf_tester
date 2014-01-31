/**
 * \file
 * Comparator function prototypes
 * 
 * Comparator functions of the type kzInt compare(const void* first, const void* second) 
 * which returns 0 if first == second, -1 if first < second and 1 if first > second
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_comparator.h"

#include <core/util/string/kzc_string.h>


#define kzcNumberCompare_internal(a_param, b_param) ((a_param < b_param) ? -1 : ((a_param > b_param) ? 1 : 0))

kzInt kzcCompareInts(const void* first, const void* second)
{
    kzInt a = *((kzInt*)first);
    kzInt b = *((kzInt*)second);
    return kzcNumberCompare_internal(a, b);
}

kzInt kzcCompareU32s(const void* first, const void* second)
{
    kzU32 a = *((kzU32*)first);
    kzU32 b = *((kzU32*)second);
    return kzcNumberCompare_internal(a, b);
}

kzInt kzcCompareFloats(const void* first, const void* second)
{
    kzFloat a = *((kzFloat*)first);
    kzFloat b = *((kzFloat*)second);
    return kzcNumberCompare_internal(a, b);
}

kzInt kzcComparePointers(const void* first, const void* second)
{
    return kzcNumberCompare_internal(first, second);
}

#undef kzcNumberCompare_internal

kzInt kzcCompareStrings(const void* first, const void* second)
{
    return kzcStringCompare((kzString)first, (kzString)second);
}
