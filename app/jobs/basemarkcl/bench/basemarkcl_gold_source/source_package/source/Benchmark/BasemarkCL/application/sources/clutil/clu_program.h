/* \file
* Utility functions for loading OpenCL kernels and building them
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef CLU_PROGRAM_H
#define CLU_PROGRAM_H

#include "clu_opencl_base.h"

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Predeclarations. */
struct KzcMemoryManager;



/** Checks if binary program exists for the given path. */
kzsError cluGetProgramBinaryExists(const struct KzcMemoryManager* manager, kzString path, kzBool* out_exists);

/** Load an OpenCL kernel from a file and return a built program. The program is by default built for all devices bound to the supplied context. */
kzsError cluLoadProgramFromFile(const struct KzcMemoryManager* manager, kzString path, kzBool onlyOnline, cl_context context, cl_program* out_program, kzBool* out_binary);

/** Load a program source from file and build it at all devices defined for the context */
kzsError cluGetBuiltProgramFromFile(const struct KzcMemoryManager* manager, kzString path, kzBool onlyOnline, cl_context context, cl_program* out_program, kzBool* out_binary);

/** Load a program source from file and build it at all devices defined for the context using the supplied build options */
kzsError cluGetBuiltProgramFromFileWithOptions(const struct KzcMemoryManager* manager, kzString path, kzBool onlyOnline, kzString options, cl_context context, cl_program* out_program, kzBool* out_binary);

/** Build an OpenCL kernel from a given string and returns a built program. The program is by default built for all devices bound to the supplied context. */
kzsError cluGetBuiltProgramFromStringWithOptions(const struct KzcMemoryManager* manager, kzString kernelSource, kzString options, cl_context context, cl_program *out_program);

/** Print a build log of a program which has failed to build. */
kzsError cluPrintBuildLog(cl_program program);


#endif
