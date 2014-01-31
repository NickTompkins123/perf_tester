/* \file
*OpenCL platform utility functions. Simple helper functions for handling the platform and runtime level tasks.
*
*Copyright 2011 by Rightware. All rights reserved.
*/

#ifndef CLU_PLATFORM_H
#define CLU_PLATFORM_H

#include "clu_opencl_base.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzsSurfaceNative;
struct KzcMemoryManager;
struct KzcDynamicArray;
struct kzcHashMap;


/**
 * \struct ClDeviceInfo
 * Cl device information.
 */
struct CluDeviceInfo
{
    kzUint id; /**< Internal ID for the device. */
    kzMutableString name; /**< Device name. */
    kzMutableString vendor; /**< Device vendor name. */
    kzMutableString vendorId; /**< Device vendor id. */
    kzMutableString deviceVersion; /**< Device version. */
    kzMutableString driverVersion; /**< Driver version. */
    kzMutableString deviceType; /**< Type of the device. */
    struct KzcHashMap* deviceConfig; /**< Device information <kzString, kzInt*>. */
};

/**
 * \struct ClPlatformInfo
 * Cl Platform information.
 */
struct CluPlatformInfo
{
    kzUint id; /**< Internal ID for the platform. */
    kzMutableString name; /**< Name of the context. */
    kzMutableString version; /**< Version of the context. */
    kzMutableString vendor; /**< Vendor of the context. */
    kzMutableString profile; /**< Profile of the context. */

    struct KzcDynamicArray* devices; /**< Devices on this platform. <CluDeviceInfo> */
};

/**
 * \struct ClDeviceInfo
 * Contains information of available platforms and devices.
 */
struct CluInfo
{
    struct KzcDynamicArray* platforms; /**< Availalbe platforms. <CluPlatformInfo> */
    kzUint usedPlatform; /**< Id of used platform. */
    kzUint usedDevice; /**< Id of used platform. */
};


/** Create ClInfo structure and fills it with information from OpenCL. */
kzsError cluInfoCreate(const struct KzcMemoryManager* manager, struct CluInfo** out_info);

/** Delete ClInfo structure. */
kzsError cluInfoDelete(struct CluInfo* info);

/** Create an OpenCL context with default GPU device and bind to Kanzi surface if available. */
kzsError cluCreateGPUcontext(const struct KzcMemoryManager* memoryManager, const struct KzsSurfaceNative* nativeSurface, 
                             kzUint platformIndex, kzInt *deviceIndex, cl_context* outcontext);


#endif
