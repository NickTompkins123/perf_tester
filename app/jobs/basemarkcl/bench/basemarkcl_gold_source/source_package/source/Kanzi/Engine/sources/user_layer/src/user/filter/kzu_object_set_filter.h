/**
* \file
* Specifies object set filter.
* Object set filter accepts or rejects all specified objects and all their ancestors.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_OBJECT_SET_FILTER_H
#define KZU_OBJECT_SET_FILTER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzuObjectNode;
struct KzuObjectSource;


/** Creates object set filter. */
kzsError kzuObjectSetFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                  kzUint objectCount, struct KzuObjectNode* const* objects, kzBool isExclusive,
                                  struct KzuObjectSource** out_objectSource);

/** Adds new object to object set filter. */
kzsError kzuObjectSetFilterAddObject(const struct KzuObjectSource* objectSource, const struct KzuObjectNode* objectNode);


#endif
