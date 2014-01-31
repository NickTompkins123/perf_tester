/**
 * \file
 * Global storage. This module can be used for storing global data.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzu_globals.h"

#include <core/util/collection/kzc_hash_map.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/wrappers/kzs_string.h>
#include <system/wrappers/kzs_math.h>


/** Identifier for fixed properties. */
const kzString KZU_GLOBALS_STANDARD_MESSAGE_TYPES = "StandardMessageTypes";
const kzString KZU_GLOBALS_LUA_STATE = "LuaState"; 


/** Hash table of globals. */ /*lint -e{956} Ignored warning for not multi thread safeness. */
static struct KzcHashMap* g_kzuGlobals;


kzsError kzuGlobalsInitialize(const struct KzcMemoryManager* memoryManager)
{
    kzsError result;

    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &g_kzuGlobals);
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzsError kzuGlobalsUninitialize(void)
{
    kzsError result;

    result = kzcHashMapDelete(g_kzuGlobals);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError kzuGlobalsPut(kzString name, void* data)
{
    kzsError result;

    result = kzcHashMapPut(g_kzuGlobals, name, data);
    kzsErrorForward(result);

    kzsSuccess();
}

void* kzuGlobalsGet(kzString name)
{
    void* returnValue;
    kzBool found;

    found = kzcHashMapGet(g_kzuGlobals, name, &returnValue);
    if (!found)
    {
        returnValue = KZ_NULL;
    }

    return returnValue;
}
