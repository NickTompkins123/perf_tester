/**
 * \file
 * Global storage. This module can be used for storing user level global data.
 * Warning: Such global data is not thread safe unless explicitly synchronized.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZU_GLOBALS_H
#define KZU_GLOBALS_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


extern const kzString KZU_GLOBALS_STANDARD_MESSAGE_TYPES; /**< Identifier for standard message types. */
extern const kzString KZU_GLOBALS_LUA_STATE; /**< Identifier for lua state. */


/* Forward declarations. */
struct KzcMemoryManager;


/** Initializes globals. */
kzsError kzuGlobalsInitialize(const struct KzcMemoryManager* memoryManager);

/** Uninitializes globals. */
kzsError kzuGlobalsUninitialize(void);

/** Put data to global storage. */
kzsError kzuGlobalsPut(kzString name, void* data);

/** Get data from global storage. Returns KZ_NULL if no data is associated with the given name. */
void* kzuGlobalsGet(kzString name);


#endif
