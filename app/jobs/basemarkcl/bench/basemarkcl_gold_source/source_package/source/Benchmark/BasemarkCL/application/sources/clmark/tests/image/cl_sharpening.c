/* 
* OpenCL sharpening filter 
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

#define KERNEL_SIZE 9


struct SharpeningFilter 
{
    cl_kernel sharpeningKernel;
    cl_program sharpeningProgram;
    cl_mem convolutionKernel;
    cl_float convKernelBuffer[KERNEL_SIZE];

    /* The final image is sum of the laplacian of gaussian and the original image scaled with constant factor */
    cl_program basicImageOperationsProgram;
    cl_kernel addConstantMultiplyKernel;

    size_t* localWorkSize; /**< Local work size. Read from configuration file. */
    struct CluProfiler *profiler; /**< Clu profiler */
};


kzsError initSharpening(struct BenchmarkFramework* framework, struct BfScene* scene, cl_context context, struct SharpeningFilter **out_filter)
{
    kzsError result;
    struct SharpeningFilter *filter;   
    cl_int clResult;

    result = kzcMemoryAllocVariable(bfGetMemoryManager(framework),filter,"Image sharpening kernels and programs");
    kzsErrorForward(result);

    result = bfUtilReadLocalWorkgroupSize(framework, "ImageTestLocalWorkSize", 2, &filter->localWorkSize);
    kzsErrorForward(result);

    {
        kzBool binaryA, binaryB;
        kzString compilerSettings = COMPILER_FLAGS;
        kzBool isValid;
        result = bfValidateFile(framework, "data/convolution2d.cl", CONVOLUTION_HASH, &isValid);
        kzsErrorForward(result);
        if(bfSceneGetValidConfigurationData(scene))
        {
            bfSceneSetValidConfigurationData(scene, isValid);
        }
        result = cluGetBuiltProgramFromFileWithOptions(bfGetMemoryManager(framework), "data/convolution2d.cl", KZ_FALSE, compilerSettings, context, &filter->sharpeningProgram, &binaryA);
        kzsErrorForward(result);
        filter->sharpeningKernel = clCreateKernel(filter->sharpeningProgram, "convolutionNaive2dFloat", &clResult);
        cluClErrorTest(clResult);
        result = bfValidateFile(framework, "data/basic_image_operations.cl", BASIC_IMAGE_OPERATIONS_HASH, &isValid);
        kzsErrorForward(result);
        if(bfSceneGetValidConfigurationData(scene))
        {
            bfSceneSetValidConfigurationData(scene, isValid);
        }
        result = cluGetBuiltProgramFromFileWithOptions(bfGetMemoryManager(framework), "data/basic_image_operations.cl", KZ_FALSE, compilerSettings, context, &filter->basicImageOperationsProgram, &binaryB);
        kzsErrorForward(result);
        filter->addConstantMultiplyKernel = clCreateKernel(filter->basicImageOperationsProgram, "addConstantMultiply", &clResult);
        cluClErrorTest(clResult);
        bfSceneSetUseProgramBinary(scene, binaryB | binaryB);
    }


    /* Allocate the convolution kernel buffer*/
    filter->convolutionKernel = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_float)*KERNEL_SIZE ,NULL, &clResult);
    cluClErrorTest(clResult);
    {
        cl_float hcKernel[9] = {-1,-1,-1,-1,8,-1,-1,-1,-1};
        kzsMemcpy(filter->convKernelBuffer,hcKernel, sizeof(cl_float)*KERNEL_SIZE);

    }
    filter->profiler = bfSceneGetProfiler(scene);

    *out_filter = filter;
    kzsSuccess();
}

kzsError applySharpening(cl_command_queue queue, struct SharpeningFilter *filter, cl_mem summationImage, cl_mem inputImage, cl_mem outputImage)
{
    kzsError result;
    size_t imsize[3] = {0,0,1};
    size_t origin[3] = {0,0,0};
    cl_int clResult;
    cl_event curEvent;


    cl_float k = 0.7f;

    result = cluGetImageSize(inputImage, imsize);
    kzsErrorForward(result);



    clResult = clEnqueueWriteBuffer(queue, filter->convolutionKernel, 0, 0, sizeof(cl_float)*KERNEL_SIZE, filter->convKernelBuffer, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "write convolution buffer", curEvent);
    result = cluSetKernelArguments(filter->sharpeningKernel, sizeof(cl_mem), &outputImage, sizeof(cl_mem), &inputImage, sizeof(cl_mem), &filter->convolutionKernel);
    kzsErrorForward(result);

    clResult = clEnqueueNDRangeKernel(queue, filter->sharpeningKernel, 2, NULL, imsize, filter->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "sharpeningKernel", curEvent);
    clResult = clEnqueueCopyImage(queue, outputImage, inputImage, origin, origin, imsize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "copy image", curEvent);
    result = cluSetKernelArguments(filter->addConstantMultiplyKernel, sizeof(cl_mem), &outputImage, sizeof(cl_mem), &summationImage, sizeof(cl_mem), &inputImage,  sizeof(cl_float), &k);
    kzsErrorForward(result);
    clResult = clEnqueueNDRangeKernel(queue, filter->addConstantMultiplyKernel, 2, NULL, imsize, filter->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(filter->profiler, "addConstantMultiplyKernel", curEvent);

    kzsSuccess();
}

kzsError deleteSharpening(struct SharpeningFilter *filter)
{
    kzsError result;
    cl_int clResult;
    result = bfUtilFreeLocalWorkgroupSize(filter->localWorkSize);
    kzsErrorForward(result);

    clResult = clReleaseMemObject(filter->convolutionKernel);
    cluClErrorTest(clResult);

    clResult = clReleaseKernel(filter->sharpeningKernel);
    cluClErrorTest(clResult);

    clResult = clReleaseProgram(filter->sharpeningProgram);
    cluClErrorTest(clResult);

    clResult = clReleaseKernel(filter->addConstantMultiplyKernel);
    cluClErrorTest(clResult);

    clResult = clReleaseProgram(filter->basicImageOperationsProgram);
    cluClErrorTest(clResult);

    result = kzcMemoryFreeVariable(filter);
    kzsErrorForward(result);

    kzsSuccess();
}
