/**
* \file
* Specifies distance filter.
* Distance filter accepts or rejects objects that are within given range from specified object.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_DISTANCE_FILTER_H
#define KZU_DISTANCE_FILTER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzuObjectNode;
struct KzuObjectSource;


/**
 * Creates distance filter. All objects that are nearer than given maxDistance from given object are either
 * accepted or rejected depending on whether isExclusive is set or not.
 */
kzsError kzuDistanceFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                 struct KzuObjectNode* object, const kzFloat maxDistance, kzBool isExclusive,
                                 struct KzuObjectSource** out_objectSource);


#endif
