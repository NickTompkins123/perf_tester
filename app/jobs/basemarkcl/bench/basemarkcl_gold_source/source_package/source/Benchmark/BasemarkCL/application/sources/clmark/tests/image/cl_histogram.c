/* 
* OpenCL histogram based color correction
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

struct HistogramFilter
{
    cl_kernel histogramGenerateKernel; /**< Histogram generation kernel. */
    cl_kernel histogramApplyKernel; /**< Apply histogram kernel. */
    cl_kernel histogramGenerateCumulativeDistributionKernel; /**< Kernel for generating histogram cumulative distribution. */
    cl_kernel histogramZeroDistributionKernel; /**< Zero distribution kernel. */
    cl_program histogramProgram; /**< Histogram program. */
    cl_mem histogramBuffer; /**< Histogram CL memory buffer. */
    size_t* localWorkSize; /**< Local work size. Read from configuration file. */
    struct CluProfiler *profiler; /**< Clu profiler */
};

/* Amount of color channels */
#define CHANNEL_COUNT 4
/* Amount of histogram bins */
#define BIN_COUNT 256


kzsError initHistogram(struct BenchmarkFramework* framework, struct BfScene* scene, cl_context context, struct HistogramFilter **out_filter)
{
    kzsError result;
    struct HistogramFilter *filter;    
    cl_int clResult;


    result = kzcMemoryAllocVariable(bfGetMemoryManager(framework),filter,"Histogram filter program, kernels and buffers.");
    kzsErrorForward(result);

    result = bfUtilReadLocalWorkgroupSize(framework, "ImageTestLocalWorkSize", 2, &filter->localWorkSize);
    kzsErrorForward(result);

    {
        kzBool fullProfile;
        result = bfGetCLFullProfile(framework, &fullProfile);
        kzsErrorForward(result);

        if(!fullProfile)
        {
            bfSceneDisableFromOverallScore(scene);
        }
    }

    /* Load and compile the histogram kernels */
    {
        kzBool binary;
        kzString compilerSettings = COMPILER_FLAGS;
        kzBool isValid;
        result = bfValidateFile(framework, "data/histogram.cl", HISTOGRAM_HASH, &isValid);
        kzsErrorForward(result);
        if(bfSceneGetValidConfigurationData(scene))
        {
            bfSceneSetValidConfigurationData(scene, isValid);
        }
        result = cluGetBuiltProgramFromFileWithOptions(bfGetMemoryManager(framework), "data/histogram.cl", KZ_FALSE, compilerSettings, context, &filter->histogramProgram, &binary);
        kzsErrorForward(result);
        bfSceneSetUseProgramBinary(scene, binary);
    }
    filter->histogramApplyKernel = clCreateKernel(filter->histogramProgram, "applyHistogram", &clResult);
    cluClErrorTest(clResult);
    filter->histogramGenerateCumulativeDistributionKernel = clCreateKernel(filter->histogramProgram, "generateCumulativeDistribution", &clResult);
    cluClErrorTest(clResult);
    filter->histogramGenerateKernel = clCreateKernel(filter->histogramProgram, "generateHistogram", &clResult);
    cluClErrorTest(clResult);
    filter->histogramZeroDistributionKernel = clCreateKernel(filter->histogramProgram, "zeroDistribution", &clResult);
    cluClErrorTest(clResult);

    /* Allocate the histogram buffer */
    filter->histogramBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_uint)*CHANNEL_COUNT*BIN_COUNT ,NULL, &clResult);
    cluClErrorTest(clResult);
    filter->profiler = bfSceneGetProfiler(scene);
    *out_filter = filter;
    kzsSuccess();
}

kzsError applyHistogram(cl_command_queue queue, struct HistogramFilter *filter, cl_mem inputImage, cl_mem outputImage)
{
    kzsError result;
    cl_int clResult;
    size_t hdim[1] = {CHANNEL_COUNT};
    size_t imsize[3] = {1,1,1};
    cl_event curEvent;

    result = cluGetImageSize(inputImage, imsize);
    kzsErrorForward(result);

    clResult = cluSetKernelArguments(filter->histogramZeroDistributionKernel, sizeof(cl_mem), &filter->histogramBuffer);
    cluClErrorTest(clResult);
    clResult = cluSetKernelArguments(filter->histogramGenerateKernel, sizeof(cl_mem), &inputImage, sizeof(cl_mem), &filter->histogramBuffer);
    cluClErrorTest(clResult); 
    clResult = cluSetKernelArguments(filter->histogramGenerateCumulativeDistributionKernel, sizeof(cl_mem), &filter->histogramBuffer);
    cluClErrorTest(clResult);
    clResult = cluSetKernelArguments(filter->histogramApplyKernel, sizeof(cl_mem), &outputImage, sizeof(cl_mem), &inputImage, sizeof(cl_mem), &filter->histogramBuffer);
    cluClErrorTest(clResult);

    clResult = clEnqueueNDRangeKernel(queue, filter->histogramZeroDistributionKernel, 1, NULL, hdim, NULL, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "histogramZeroDistributionKernel", curEvent);
    clResult = clEnqueueNDRangeKernel(queue, filter->histogramGenerateKernel, 2, NULL, imsize, filter->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "histogramGenerateKernel", curEvent);
    clResult = clEnqueueNDRangeKernel(queue, filter->histogramGenerateCumulativeDistributionKernel, 1, NULL, hdim, NULL, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "histogramGenerateCumulativeDistributionKernel", curEvent);
    clResult = clEnqueueNDRangeKernel(queue, filter->histogramApplyKernel, 2, NULL, imsize, filter->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "histogramApplyKernel", curEvent);

    kzsSuccess();
}

kzsError deleteHistogram(struct HistogramFilter *filter)
{
    kzsError result;
    cl_int clResult;
    result = bfUtilFreeLocalWorkgroupSize(filter->localWorkSize);
    kzsErrorForward(result);

    clResult = clReleaseMemObject(filter->histogramBuffer);
    cluClErrorTest(clResult);
    result = clReleaseKernel(filter->histogramApplyKernel);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(filter->histogramGenerateCumulativeDistributionKernel);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(filter->histogramGenerateKernel);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(filter->histogramZeroDistributionKernel);
    cluClErrorTest(clResult);
    clResult = clReleaseProgram(filter->histogramProgram);
    cluClErrorTest(clResult);


    result = kzcMemoryFreeVariable(filter);
    kzsErrorForward(result);
    kzsSuccess();
}


