/**
* \file
* System memory manager.
* System memory manager allocates memory with the system layer malloc function.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_MEMORY_SYSTEM_H
#define KZC_MEMORY_SYSTEM_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;


/** Creates a new memory manager, which allocates the memory with system layer memory allocation functions. */
kzsError kzcMemoryManagerCreateSystemManager(struct KzcMemoryManager** out_manager);


#endif
