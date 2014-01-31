/**
* \file
* Base functions for file related operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_FILE_BASE_H
#define KZS_FILE_BASE_H


#include <system/kzs_types.h>


/**
 * Sets the active working directory to the given one.
 * All relative file and resource paths will be relative to the given path after this call, unless path is null.
 */
void kzsFileBaseSetResourceDirectory(kzString path);

/** Returns the active working directory. */
kzString kzsFileBaseGetResourceDirectory(void);


#endif
