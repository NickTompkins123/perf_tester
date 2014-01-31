/**
* \file
* Project loader gpu memory type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_GPU_MEMORY_TYPE
#define KZU_PROJECT_GPU_MEMORY_TYPE


#include <core/resource_manager/kzc_resource_memory_type.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/** Type of memory handling for object. */
enum KzuBinaryGpuMemoryType
{
    KZU_BINARY_GPU_MEMORY_TYPE_GPU_ONLY = 0, /**< Object is stored only in GPU memory. */
    KZU_BINARY_GPU_MEMORY_TYPE_GPU_AND_RAM = 1, /**< Object is stored in RAM and GPU memory. */
    KZU_BINARY_GPU_MEMORY_TYPE_RAM_ONLY = 2 /**< Object is stored in RAM only. */
};


/** Returns the project gpu memory type from binary memory type. */
kzsError kzuProjectLoaderGetMemoryType_private(enum KzuBinaryGpuMemoryType memoryType, enum KzcResourceMemoryType* out_resourceMemoryType);


#endif
