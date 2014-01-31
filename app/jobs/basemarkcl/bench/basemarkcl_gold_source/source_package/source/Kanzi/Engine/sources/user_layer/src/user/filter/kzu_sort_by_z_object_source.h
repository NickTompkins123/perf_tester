/**
* \file
* Specifies sort object sources. 
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_SORT_BY_Z_OBJECT_SOURCE_H
#define KZU_SORT_BY_Z_OBJECT_SOURCE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuObjectSource;
struct KzcMemoryManager;


/** Creates an object source which performs sorting by object view z value. */
kzsError kzuSortByZObjectSourceCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                      kzBool frontToBack, struct KzuObjectSource** out_objectSource);


#endif
