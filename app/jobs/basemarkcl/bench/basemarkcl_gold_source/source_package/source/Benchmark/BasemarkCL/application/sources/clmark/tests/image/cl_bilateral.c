/* 
* OpenCL bilateral filter 
*
* Copyright 2011 by Rightware. All rights reserved.
*/

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>

#include <benchmarkutil/settings/bf_settings.h>

#include <core/memory/kzc_memory_manager.h>

#include <system/wrappers/kzs_memory.h>

#include <clutil/clu_platform.h>
#include <clutil/clu_program.h>
#include <clutil/clu_kernel.h>
#include <clutil/clu_image.h>
#include <clutil/clu_util.h>
#include <clutil/clu_profiler.h>

#include <benchmarkutil/util/bf_file_validator.h>
#include <clmark/test_definitions.h>

struct BilateralFilter
{
    cl_kernel bilateralKernel;
    cl_program bilateralProgram;
    size_t* localWorkSize; /**< Local work size. Read from configuration file. */
    struct CluProfiler *profiler; /**< Clu profiler */
};


kzsError initBilateral(struct BenchmarkFramework* framework, struct BfScene* scene, cl_context context, struct BilateralFilter **out_filter)
{
    kzsError result;
    cl_int clResult;
    struct BilateralFilter *filter;    

    result = kzcMemoryAllocVariable(bfGetMemoryManager(framework), filter, "Bilateral filter program and kernels");
    kzsErrorForward(result);

    result = bfUtilReadLocalWorkgroupSize(framework, "ImageTestLocalWorkSize", 2, &filter->localWorkSize);
    kzsErrorForward(result);

    {
        kzBool binary;
        kzString compilerSettings = COMPILER_FLAGS;
        kzBool isValid;
        result = bfValidateFile(framework, "data/bilateral.cl", BILATERAL_HASH, &isValid);
        kzsErrorForward(result);
        if(bfSceneGetValidConfigurationData(scene))
        {
            bfSceneSetValidConfigurationData(scene, isValid);
        }
        result = cluGetBuiltProgramFromFileWithOptions(bfGetMemoryManager(framework), "data/bilateral.cl", KZ_FALSE, compilerSettings, context, &filter->bilateralProgram, &binary);
        kzsErrorForward(result);
        bfSceneSetUseProgramBinary(scene, binary);
    }

    filter->bilateralKernel = clCreateKernel(filter->bilateralProgram, "bilateralFilter", &clResult);
    cluClErrorTest(clResult);
    filter->profiler = bfSceneGetProfiler(scene);
    *out_filter = filter;
    kzsSuccess();
}

kzsError applyBilateral(cl_command_queue queue, struct BilateralFilter *filter, cl_mem inputImage, cl_mem outputImage)
{
    kzsError result;
    cl_int clResult;
    size_t imsize[2];
    cl_event curEvent;

    result = cluGetImageSize(inputImage, imsize);
    kzsErrorForward(result);
    result = cluSetKernelArguments(filter->bilateralKernel, sizeof(cl_mem), &outputImage, sizeof(cl_mem), &inputImage);
    kzsErrorForward(result);

    clResult = clEnqueueNDRangeKernel(queue, filter->bilateralKernel, 2, NULL, imsize, filter->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "bilateralKernel", curEvent);
    
    kzsSuccess();
}

kzsError deleteBilateral(struct BilateralFilter *filter)
{
    kzsError result;
    cl_int clResult;
    
    result = bfUtilFreeLocalWorkgroupSize(filter->localWorkSize);
    kzsErrorForward(result);

    clResult = clReleaseKernel(filter->bilateralKernel);
    cluClErrorTest(clResult);
    clResult = clReleaseProgram(filter->bilateralProgram);
    cluClErrorTest(clResult);

    result = kzcMemoryFreeVariable(filter);
    kzsErrorForward(result);

    kzsSuccess();
}


