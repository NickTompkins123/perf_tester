/**
* \file
* Specifies sort object source for material type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_SORT_BY_MATERIAL_TYPE_OBJECT_SOURCE_H
#define KZU_SORT_BY_MATERIAL_TYPE_OBJECT_SOURCE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuObjectSource;
struct KzcMemoryManager;


/** Creates an object source which performs sorting by material type. */
kzsError kzuSortByMaterialTypeObjectSourceCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                                 struct KzuObjectSource** out_objectSource);


#endif
