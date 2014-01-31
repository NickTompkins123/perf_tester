/**
* \file
* Specifies object type filter. This filter accepts or rejects specific types of objects.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_OBJECT_TYPE_FILTER_H
#define KZU_OBJECT_TYPE_FILTER_H


#include <user/scene_graph/kzu_object_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declaration structures & enumerations */
struct KzcMemoryManager;
struct KzuObjectSource;


/** Creates an object type filter. */
kzsError kzuObjectTypeFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                   KzuObjectType objectType, kzBool isExclusive, struct KzuObjectSource** out_objectSource);


#endif
