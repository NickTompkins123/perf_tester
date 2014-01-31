/**
* \file
* Resource manager memory type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_RESOURCE_MEMORY_TYPE_H
#define KZC_RESOURCE_MEMORY_TYPE_H


/** Enumeration for memory type. If resource exists in ram, it can be restored. */
enum KzcResourceMemoryType
{
    KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY,      /**< Resource is only held on GPU. */
    KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM,   /**< Resource is held both on GPU and RAM. This types of resources can be restored. */
    KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY,       /**< Resource is only held on RAM. */
    KZC_RESOURCE_MEMORY_TYPE_NONE           /**< Resource is neither in RAM or GPU. */
};


#endif
