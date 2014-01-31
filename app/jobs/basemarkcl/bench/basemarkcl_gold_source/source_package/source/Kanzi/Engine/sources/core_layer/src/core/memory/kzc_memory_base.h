/**
* \file
* Core memory manager base definitions and functions.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_MEMORY_BASE_H
#define KZC_MEMORY_BASE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/**
 * If defined, the following memory debugging features are enabled:
 * <ul>
 *   <li>Memory allocations are tracked with string descriptions. This uses some extra memory.
 *   <li>Guard data is written and checked in the beginning and end of allocated memory. This also uses more memory and reduces performance slightly.
 * </ul>
 */
#ifdef _DEBUG
#define KZC_MEMORY_DEBUG
#define KZC_MEMORY_MEASURE
#endif
/**
 * If defined, all unallocated memory in pooled memory manager is written with guard data and checked while allocating.
 * This does not use any additional memory, but can severely reduce performance.
 * This flag has no effect unless KZC_MEMORY_DEBUG is enabled as well.
 */
#if 0
#define KZC_MEMORY_DEBUG_GUARD_AGGRESSIVELY
#endif


#ifdef KZC_MEMORY_DEBUG
    /** This macro is used to add description parameter for memory management functions. */
    #define MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(x) , x
#else
    /** This macro is used to add description parameter for memory management functions. */
    #define MEMORY_MANAGER_DEBUG_PARAM_PRIVATE(x)
#endif


/** Type of a memory manager. */
enum KzcMemoryManagerType {
    KZC_MEMORY_MANAGER_TYPE_SYSTEM, /**< System memory manager. */
    KZC_MEMORY_MANAGER_TYPE_POOLED, /**< Pooled memory manager. */
    KZC_MEMORY_MANAGER_TYPE_QUICK,  /**< Quick memory manager. */
    KZC_MEMORY_MANAGER_TYPE_CUSTOM  /**< Custom memory manager. */
};


#endif
