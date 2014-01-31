/**
* \file
* Binary reader utilities.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_BINARY_UTIL_H
#define KZC_BINARY_UTIL_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzcInputStream;


/** Reads a kzb string from the given input stream. A new string object is allocated and must be freed by the caller. */
kzsException kzuBinaryReadString(const struct KzcMemoryManager* memoryManager, struct KzcInputStream* inputStream, kzMutableString* out_string);


#endif
