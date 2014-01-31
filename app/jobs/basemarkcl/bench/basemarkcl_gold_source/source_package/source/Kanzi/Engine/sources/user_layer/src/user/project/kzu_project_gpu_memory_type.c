/**
* \file
* Project loader gpu memory type.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzu_project_gpu_memory_type.h"

#include <core/resource_manager/kzc_resource_memory_type.h>


kzsError kzuProjectLoaderGetMemoryType_private(enum KzuBinaryGpuMemoryType memoryType, 
                                                       enum KzcResourceMemoryType* out_resourceMemoryType)
{
    enum KzcResourceMemoryType resourceMemoryType;

    switch (memoryType)
    {
        case KZU_BINARY_GPU_MEMORY_TYPE_GPU_ONLY: resourceMemoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY; break;
        case KZU_BINARY_GPU_MEMORY_TYPE_GPU_AND_RAM: resourceMemoryType = KZC_RESOURCE_MEMORY_TYPE_GPU_AND_RAM; break;
        case KZU_BINARY_GPU_MEMORY_TYPE_RAM_ONLY: resourceMemoryType = KZC_RESOURCE_MEMORY_TYPE_RAM_ONLY; break;
        default: kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid memory type in binary.");
    }

    *out_resourceMemoryType = resourceMemoryType;
    kzsSuccess();    
}
