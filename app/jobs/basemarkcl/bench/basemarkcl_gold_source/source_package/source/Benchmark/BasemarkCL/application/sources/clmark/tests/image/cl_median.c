/* 
* OpenCL median filter 
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


struct MedianFilter
{
    cl_kernel medianKernel;
    cl_program medianProgram;

    size_t* localWorkSize; /**< Local work size. Read from configuration file. */
    struct CluProfiler *profiler; /**< Clu profiler */
};


kzsError initMedian(struct BenchmarkFramework* framework, struct BfScene* scene, cl_context context, struct MedianFilter **out_filter)
{
    kzsError result;
    struct MedianFilter *filter;    
    cl_int clResult;

    result = kzcMemoryAllocVariable(bfGetMemoryManager(framework),filter,"Median filter program and kernels");
    kzsErrorForward(result);

    result = bfUtilReadLocalWorkgroupSize(framework, "ImageTestLocalWorkSize", 2, &filter->localWorkSize);
    kzsErrorForward(result);

    {
        kzString compilerSettings = COMPILER_FLAGS;
        kzBool binary;
        kzBool isValid;
        result = bfValidateFile(framework, "data/median.cl", MEDIAN_HASH, &isValid);
        kzsErrorForward(result);
        if(bfSceneGetValidConfigurationData(scene))
        {
            bfSceneSetValidConfigurationData(scene, isValid);
        }
        result = cluGetBuiltProgramFromFileWithOptions(bfGetMemoryManager(framework), "data/median.cl", KZ_FALSE, compilerSettings, context, &filter->medianProgram, &binary);
        kzsErrorForward(result);
        bfSceneSetUseProgramBinary(scene, binary);
    }
    filter->medianKernel = clCreateKernel(filter->medianProgram, "medfilt3x3", &clResult);
    cluClErrorTest(clResult);
    filter->profiler = bfSceneGetProfiler(scene);
    *out_filter = filter;
    kzsSuccess();
}

kzsError applyMedian(cl_command_queue queue, struct MedianFilter *filter, cl_mem inputImage, cl_mem outputImage)
{
    kzsError result;
    size_t imsize[2];
    cl_int clResult;
    cl_event curEvent;

    result = cluGetImageSize(inputImage, imsize);
    kzsErrorForward(result);
    result = cluSetKernelArguments(filter->medianKernel, sizeof(cl_mem), &outputImage, sizeof(cl_mem), &inputImage);
    kzsErrorForward(result);

    clResult = clEnqueueNDRangeKernel(queue, filter->medianKernel, 2, NULL, imsize, filter->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "medianKernel", curEvent);

    kzsSuccess();
}

kzsError deleteMedian(struct MedianFilter *filter)
{
    kzsError result;
    cl_int clResult;

    result = bfUtilFreeLocalWorkgroupSize(filter->localWorkSize);
    kzsErrorForward(result);

    clResult = clReleaseKernel(filter->medianKernel);
    cluClErrorTest(clResult);
    clResult = clReleaseProgram(filter->medianProgram);
    cluClErrorTest(clResult);

    result = kzcMemoryFreeVariable(filter);
    kzsErrorForward(result);

    kzsSuccess();
}


