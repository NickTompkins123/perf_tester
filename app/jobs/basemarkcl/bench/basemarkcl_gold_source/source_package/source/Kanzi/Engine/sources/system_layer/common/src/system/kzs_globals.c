/**
 * \file
 * Global storage. This module can be used for storing global data.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzs_globals.h"

#include <system/wrappers/kzs_string.h>
#include <system/wrappers/kzs_math.h>


/** Maximum number of globals. Must be power of two. */
#define KZS_MAXIMUM_GLOBALS_COUNT 16


/** Single global hash table entry. */
struct KzsGlobalHashEntry
{
    kzString name; /**< Name of the global. */
    kzU32 hashCode; /**< Hash value calculated from the name. */
    void* data; /**< Global data. */
};

/** Hash table of globals. */
static struct KzsGlobalHashEntry g_kzsGlobals[KZS_MAXIMUM_GLOBALS_COUNT];


/** Calculates hash code from given string. */
static kzU32 kzsGlobalsGetHashCode_private(kzString string)
{
    kzU32 hash = 5381;
    kzUint i;

    /* djb2 hash algorithm */
    for (i = 0; string[i] != '\0'; ++i)
    {
        hash = ((hash << 5) + hash) + (kzByte)string[i]; /* hash * 33 + c */
    }

    return hash;
}

kzsError kzsGlobalsInitialize()
{
    kzUint i;

    for (i = 0; i < KZS_MAXIMUM_GLOBALS_COUNT; ++i) {
        g_kzsGlobals[i].name = KZ_NULL;
    }

    kzsSuccess();
}

kzsError kzsGlobalsUninitialize()
{
    kzsSuccess();
}

/** Find hash table index for given global name. */
static kzUint kzsGlobalsFindIndex_private(kzString name)
{
    kzU32 hashCode = kzsGlobalsGetHashCode_private(name);
    kzUint i = 0;
    kzBool found = KZ_FALSE;
    kzUint index;

    do
    {
        kzString existingName;
        index = ((kzUint)hashCode + i) & (KZS_MAXIMUM_GLOBALS_COUNT - 1);
        existingName = g_kzsGlobals[index].name;

        /* Check if the slot is empty. */
        if (existingName == KZ_NULL)
        {
            found = KZ_TRUE;
            /* Initialize the slot. */
            g_kzsGlobals[index].name = name;
            g_kzsGlobals[index].hashCode = hashCode;
            g_kzsGlobals[index].data = KZ_NULL;
        }
        /* If not, check if the slot matches the given name. */
        else if (g_kzsGlobals[index].hashCode == hashCode &&
                 kzsStrcmp(existingName, name) == 0)
        {
            found = KZ_TRUE;
        }

        /* Otherwise move to the next slot. */
        if (!found)
        {
            ++i;
            kzsAssert(i < KZS_MAXIMUM_GLOBALS_COUNT);
        }
    } while (!found);

    return index;
}

void kzsGlobalsPut(kzString name, void* data)
{
    kzUint index = kzsGlobalsFindIndex_private(name);
    g_kzsGlobals[index].data = data;
}

void* kzsGlobalsGet(kzString name)
{
    kzUint index = kzsGlobalsFindIndex_private(name);
    return g_kzsGlobals[index].data;
}
