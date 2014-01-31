/* 
* OpenCL Gaussian blur filter 
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

#include <math.h>


#define RANGE_DEVIATION 3
#define KERNEL_SIZE (RANGE_DEVIATION*16)


struct BlurFilter
{
    cl_kernel blurKernelHorizontal;
    cl_kernel blurKernelVertical;
    cl_program blurProgram;
    cl_mem convolutionKernel;
    cl_mem bufferImage;

    cl_float convKernelBuffer[KERNEL_SIZE];

    size_t* localWorkSize; /**< Local work size. Read from configuration file. */
    struct CluProfiler *profiler; /**< Clu profiler */
};


kzsError initBlur( struct BenchmarkFramework* framework, struct BfScene* scene, cl_context context, cl_mem inputImage, struct BlurFilter **out_filter )
{
    kzsError result;
    cl_int clResult;
    struct BlurFilter *filter;   
    cl_float normval = 0.0f;
    cl_int i = 0;

    result = kzcMemoryAllocVariable(bfGetMemoryManager(framework),filter,"Image blur kernels and programs");
    kzsErrorForward(result);

    result = bfUtilReadLocalWorkgroupSize(framework, "ImageTestLocalWorkSize", 2, &filter->localWorkSize);
    kzsErrorForward(result);

    {
        kzBool binary;
        kzString compilerSettings = COMPILER_FLAGS;
        kzBool isValid;
        result = bfValidateFile(framework, "data/separable_convolution2d.cl", SEPARABLE_CONVOLUTION2D_HASH, &isValid);
        kzsErrorForward(result);
        if(bfSceneGetValidConfigurationData(scene))
        {
            bfSceneSetValidConfigurationData(scene, isValid);
        }
        result = cluGetBuiltProgramFromFileWithOptions(bfGetMemoryManager(framework), "data/separable_convolution2d.cl", KZ_FALSE, compilerSettings, context, &filter->blurProgram, &binary);
        kzsErrorForward(result);
        bfSceneSetUseProgramBinary(scene, binary);
    }
    filter->blurKernelHorizontal = clCreateKernel(filter->blurProgram, "convolutionNaiveRowsFloat", &clResult);
    cluClErrorTest(clResult);
    filter->blurKernelVertical = clCreateKernel(filter->blurProgram, "convolutionNaiveColumnsFloat", &clResult);
    cluClErrorTest(clResult);

    /* Calculate the used filter kernel here. */
    for(i = 0; i < KERNEL_SIZE; ++i)
    {    
        filter->convKernelBuffer[i] = (cl_float)expf(((cl_float)-((i - KERNEL_SIZE/2)*(i - KERNEL_SIZE/2))) / (2.0f*(cl_float)RANGE_DEVIATION*(cl_float)RANGE_DEVIATION));
        normval += filter->convKernelBuffer[i];
    }

    for(i = 0; i < KERNEL_SIZE; ++i)
    {    
        filter->convKernelBuffer[i] = filter->convKernelBuffer[i]/normval;        
    }

    /* Allocate the convolution kernel buffer*/
    filter->convolutionKernel = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_float)*KERNEL_SIZE ,NULL, &clResult);
    cluClErrorTest(clResult);
    result = cluCreateBufferForImage(context, inputImage, &filter->bufferImage);
    kzsErrorForward(result);
    filter->profiler = bfSceneGetProfiler(scene);
    *out_filter = filter;
    kzsSuccess();
}

kzsError applyBlur(cl_command_queue queue, struct BlurFilter *filter, cl_mem inputImage, cl_mem outputImage)
{
    kzsError result;
    cl_int clResult;
    size_t imsize[2];
    cl_event curEvent;

    result = cluGetImageSize(inputImage, imsize);
    kzsErrorForward(result);

    clResult = clEnqueueWriteBuffer(queue, filter->convolutionKernel, 0, 0, sizeof(cl_float)*KERNEL_SIZE, filter->convKernelBuffer, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "write kernel buffer", curEvent);
    result = cluSetKernelArguments(filter->blurKernelHorizontal, sizeof(cl_mem), &filter->bufferImage, sizeof(cl_mem), &inputImage, sizeof(cl_mem), &filter->convolutionKernel);
    kzsErrorForward(result);
    result = cluSetKernelArguments(filter->blurKernelVertical,  sizeof(cl_mem), &outputImage, sizeof(cl_mem), &filter->bufferImage, sizeof(cl_mem), &filter->convolutionKernel);
    kzsErrorForward(result);

    clResult = clEnqueueNDRangeKernel(queue, filter->blurKernelHorizontal, 2, NULL, imsize, filter->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "blurKernelHorizontal", curEvent);
    clResult = clEnqueueNDRangeKernel(queue, filter->blurKernelVertical, 2, NULL, imsize, filter->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "blurKernelVertical", curEvent);

    kzsSuccess();
}

kzsError deleteBlur(struct BlurFilter *filter)
{
    kzsError result;
    cl_int clResult;
    result = bfUtilFreeLocalWorkgroupSize(filter->localWorkSize);
    kzsErrorForward(result);

    clResult = clReleaseMemObject(filter->convolutionKernel);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(filter->bufferImage);
    cluClErrorTest(clResult);

    clResult = clReleaseKernel(filter->blurKernelHorizontal);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(filter->blurKernelVertical);
    cluClErrorTest(clResult);
    clResult = clReleaseProgram(filter->blurProgram);
    cluClErrorTest(clResult);

    result = kzcMemoryFreeVariable(filter);
    kzsErrorForward(result);

    kzsSuccess();
}
