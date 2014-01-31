/**
 * \file
 * Global storage. This module can be used for storing global data.
 * Warning: Such global data is not thread safe unless explicitly synchronized.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZS_GLOBALS_H
#define KZS_GLOBALS_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Initializes counters. */
kzsError kzsGlobalsInitialize(void);

/** Uninitializes counters. */
kzsError kzsGlobalsUninitialize(void);

/** Put data to global storage. */
void kzsGlobalsPut(kzString name, void* data);

/** Get data from global storage. Returns KZ_NULL if no data is associated with the given name. */
void* kzsGlobalsGet(kzString name);


#endif
