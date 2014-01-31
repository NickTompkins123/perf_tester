/* OpenCl program utility library
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "clu_program.h"

#include <core/util/io/kzc_file.h>
#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/debug/kzc_log.h>

#include <system/wrappers/kzs_memory.h>
#include <system/debug/kzs_log.h>
#include <system/debug/kzs_error.h>

#include <clutil/clu_util.h>


kzsError cluGetProgramBinaryExists(const struct KzcMemoryManager* manager, kzString path, kzBool* out_exists)
{
    kzBool exists;
    kzsError result;
    kzMutableString binaryPath;

    result = kzcStringFormat(manager, "%s.bin", &binaryPath, path);
    kzsErrorForward(result);

    exists = kzcFileExists(binaryPath);

    result = kzcStringDelete(binaryPath);
    kzsErrorForward(result);

    *out_exists = exists;
    kzsSuccess();
}

kzsError cluLoadProgramFromFile(const struct KzcMemoryManager* manager, kzString path, kzBool onlyOnline, cl_context context, cl_program *out_program, kzBool* out_binary)
{
    cl_int clResult;
    kzsError result;
    kzMutableString strings;
    cl_program program;
    kzMutableString binaryPath;
    kzBool binary = KZ_FALSE;

    result = kzcStringFormat(manager, "%s.bin", &binaryPath, path);
    kzsErrorForward(result);

    if(!onlyOnline && kzcFileExists(binaryPath))
    {
        kzUint size;
        kzByte* data;
        cl_device_id deviceIds[10];
        size_t returnSize;
        cl_int clBinaryStatus;
        
        binary = KZ_TRUE;

        kzcLogDebug("Using binary kernel '%s'", binaryPath);

        clResult = clGetContextInfo(context, CL_CONTEXT_DEVICES, 10 * sizeof(cl_device_id), &deviceIds, &returnSize);
        cluClErrorTest(clResult);
        returnSize = returnSize / sizeof(cl_device_id);
        
        result = kzcFileReadBinaryFile(manager, binaryPath, &size, &data);
        kzsErrorForward(result);

        {
            size_t lengths[1] = { size };
            program = clCreateProgramWithBinary(context, returnSize, deviceIds, lengths, (const unsigned char**)&data, &clBinaryStatus, &clResult);
            cluClErrorTest(clBinaryStatus);
            cluClErrorTest(clResult);
        }

        result = kzcMemoryFreePointer(data);
        kzsErrorForward(result);
    }
    else
    {
        result = kzcFileReadTextFile(manager, path, &strings);
        kzsErrorForward(result);
        program = clCreateProgramWithSource(context, 1, (const kzChar**)&strings, NULL, &clResult);    
        cluClErrorTest(clResult);
        result = kzcStringDelete(strings);
        kzsErrorForward(result);
    }

    result = kzcStringDelete(binaryPath);
    kzsErrorForward(result);

    if(out_binary != KZ_NULL) *out_binary = binary;
    *out_program = program;
    kzsSuccess();
}


kzsError cluGetBuiltProgramFromFile(const struct KzcMemoryManager* manager, kzString path, kzBool onlyOnline, cl_context context, cl_program *out_program, kzBool* out_binary)
{
    kzsError result;
    kzString options = "";
    kzBool binary;
    result = cluGetBuiltProgramFromFileWithOptions(manager, path, onlyOnline, options, context, out_program, &binary);
    kzsErrorForward(result);
    if(out_binary != KZ_NULL) *out_binary = binary;
    kzsSuccess();
}

kzsError cluGetBuiltProgramFromFileWithOptions(const struct KzcMemoryManager* manager, kzString path, kzBool onlyOnline, kzString options, cl_context context, cl_program *out_program, kzBool* out_binary)
{
    kzsError result;
    cl_int clResult;
    cl_program program;
    kzBool binary;

    result = cluLoadProgramFromFile(manager, path, onlyOnline, context, &program, &binary);
    kzsErrorForward(result);
    clResult = clBuildProgram(program, 0, NULL, options, NULL, NULL);
    if(clResult == CL_BUILD_PROGRAM_FAILURE)
    {
        kzsError prResult;
        prResult = cluPrintBuildLog(program);
        kzsErrorForward(prResult);
    }
    cluClErrorTest(clResult);

    if(out_binary != KZ_NULL) *out_binary = binary;
    *out_program = program;
    kzsSuccess();
}

kzsError cluGetBuiltProgramFromStringWithOptions(const struct KzcMemoryManager* manager, kzString kernelSource, kzString options, cl_context context, cl_program *out_program)
{
    cl_int clResult;
    cl_program program;

    program = clCreateProgramWithSource(context, 1, &kernelSource, KZ_NULL, &clResult);    
    cluClErrorTest(clResult);

    clResult = clBuildProgram(program, 0, NULL, options, NULL, NULL);
    if(clResult == CL_BUILD_PROGRAM_FAILURE)
    {
        kzsError prResult; 
        prResult = cluPrintBuildLog(program);
        kzsErrorForward(prResult);
    }
    cluClErrorTest(clResult);

    *out_program = program;
    kzsSuccess();
}

kzsError cluPrintBuildLog(cl_program program)
{
    /* TODO: Use cl return value checking here. */
    cl_int clResult;
    cl_device_id device;
    cl_uint num; /* TODO: No short names - always write the whole name. */
    cl_int logStatus;
    char * buildLog = NULL; /* TODO: use kzMutableString */
    size_t buildLogSize = 0;
    
    clResult = clGetProgramInfo(program,CL_PROGRAM_NUM_DEVICES,sizeof(cl_uint),&num,NULL);
    cluClErrorTest(clResult);
    clResult = clGetProgramInfo(program,CL_PROGRAM_DEVICES,sizeof(cl_device_id),&device,NULL);
    cluClErrorTest(clResult);

    logStatus = clGetProgramBuildInfo (program, 
        device, 
        CL_PROGRAM_BUILD_LOG, 
        buildLogSize, 
        buildLog, 
        &buildLogSize);
    kzsErrorTest(logStatus == CL_SUCCESS, KZS_ERROR_ASSERTION_FAILED, "clGetProgramBuildInfo failed.");

    buildLog = (char*)kzsMalloc(buildLogSize);
    if(buildLog == NULL)
    {
        kzsErrorThrow(KZS_ERROR_ASSERTION_FAILED, "Failed to allocate host memory. (buildLog)");
    }
    kzsMemset(buildLog, 0, buildLogSize);

    logStatus = clGetProgramBuildInfo (program, 
        device, 
        CL_PROGRAM_BUILD_LOG, 
        buildLogSize, 
        buildLog, 
        NULL);
    kzsErrorTest(logStatus == CL_SUCCESS, KZS_ERROR_ASSERTION_FAILED, "clGetProgramBuildInfo failed.");
    
    if(logStatus != CL_SUCCESS)
    {
        kzsFree(buildLog);
        kzsErrorThrow(KZS_ERROR_ASSERTION_FAILED, "clGetProgramBuildInfo failed.");
    }

    kzsLog(KZS_LOG_LEVEL_WARNING, "\n\t\t\tBUILD LOG\n ************************************************\n");
    kzsLog(KZS_LOG_LEVEL_WARNING, (kzString)buildLog);
    kzsLog(KZS_LOG_LEVEL_WARNING, "************************************************\n");
    kzsFree(buildLog);

    kzsSuccess();
}
