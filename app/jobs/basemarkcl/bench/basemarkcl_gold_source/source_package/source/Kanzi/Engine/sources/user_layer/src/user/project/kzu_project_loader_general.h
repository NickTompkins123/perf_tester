/**
* \file
* Project loader general purpose functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_GENERAL_H
#define KZU_PROJECT_LOADER_GENERAL_H


#include "kzu_project.h"

#include <user/binary/kzu_binary_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryFileInfo;
struct KzcInputStream;


kzsException kzuProjectLoaderReadTextFile(const struct KzcMemoryManager* memoryManager,
                                          const struct KzuBinaryFileInfo* file, kzMutableString* out_text);

/**
 * Get the object type corresponding the given file type.
 * Returns KZU_PROJECT_OBJECT_TYPE_INVALID for file types, which are not managed by project.
 */
enum KzuProjectObjectType kzuProjectLoaderGetObjectType(enum KzuBinaryFileType binaryType);


#endif
