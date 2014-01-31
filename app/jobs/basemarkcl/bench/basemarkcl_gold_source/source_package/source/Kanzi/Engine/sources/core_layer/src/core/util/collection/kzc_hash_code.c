/**
 * \file
 * General purpose hash code functions.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzc_hash_code.h"

#include <core/util/string/kzc_string.h>


kzU32 kzcHashCodeFromInt(const void* pointer)
{
    return (kzU32)*((kzInt*)pointer);
}

kzU32 kzcHashCodeFromU32(const void* pointer)
{
    return *(kzU32*)pointer;
}

kzU32 kzcHashCodeFromPointer(const void* pointer)
{
    /* Get 32 bit part of the pointer address. */
    return *(kzU32*)(void*)&pointer;
}

kzU32 kzcHashCodeFromString(const void* pointer)
{
    return kzcStringGetHashCode((kzString)pointer);
}
