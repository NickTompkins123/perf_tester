/**
* \file
* Specifies contains property filter.
* Contains property filter accepts or rejects an object if it contains a property of specified type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_CONTAINS_PROPERTY_FILTER_H
#define KZU_CONTAINS_PROPERTY_FILTER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzuPropertyType;
struct KzuObjectSource;


/** Creates property filter. */
kzsError kzuContainsPropertyFilterCreate(const struct KzcMemoryManager* memoryManager, struct KzuObjectSource* input,
                                         const struct KzuPropertyType* propertyType, kzBool isExclusive,
                                         struct KzuObjectSource** out_objectSource);


#endif
