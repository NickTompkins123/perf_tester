/**
* \file
* OpenCL julia fractal.
* Copyright 2011 by Rightware. All rights reserved.
*/
#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>
#include <benchmarkutil/util/bf_file_validator.h>
#include <benchmarkutil/settings/bf_settings.h>

#include <application/kza_application.h>

#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader_material.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/properties/kzu_property_manager_void.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/properties/kzu_texture_property.h>
#include <user/material/kzu_material.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/texture/kzc_texture_descriptor.h>
#include <core/util/image/kzc_image.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/io/kzc_output_stream.h>

#include <system/display/kzs_window.h>
#include <system/display/kzs_surface.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_opengl.h>

#include <clutil/clu_opencl_base.h>
#include <clutil/clu_platform.h>
#include <clutil/clu_program.h>
#include <clutil/clu_kernel.h>
#include <clutil/clu_util.h>
#include <clutil/clu_image.h>
#include <clutil/clu_profiler.h>

#include <clmark/test_definitions.h>

#include "cl_julia.h"


kzsError juliaSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError juliaSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError juliaSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);


/** Size of the texture. */
kzUint GRID_SIZE;


struct JuliaTestData
{
    cl_context context; /**< OpenCL Context. */

    cl_command_queue queue; /**< OpenCL Command queue. */
    cl_device_id device;    /**< Device id. */

    cl_program juliaProgram; /**< OpenCL Program for fractal calculation. */
    cl_kernel juliaKernel; /**< OpenCL kernel. */
 
    cl_mem outputImage; /**< Image to write output to. */

    cl_mem colourTexture; /**< Texture used as color map for the fractal. */
    struct KzcImage *kzColourTexture; /**< Kanzi texture used for color map. */

    cl_float scale; /**< Scale factor. */
 
    size_t gridSize[2]; /**< Global work size. */
    size_t* localWorkSize; /**< Local work size. Read from configuration file. */

    struct KzcTexture* outputTexture; /**< Output texture for engine. */
    struct CluProfiler *profiler; /**< Clu profiler */

};


kzsError juliaSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct JuliaTestData *testData = NULL;
    kzsError result;
    cl_int clResult;

    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/Julia");
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    bfSceneSetScoreWeightFactor(scene, 0.5f);

    testData->context = bfGetClContext(framework);
    {
        kzBool fullProfile;
        result = bfGetCLFullProfile(framework, &fullProfile);
        kzsErrorForward(result);

        if(fullProfile)
        {
            GRID_SIZE = 1024;
        }
        else
        {
            GRID_SIZE = 512;
        }
    }
    clResult = clGetContextInfo(testData->context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &testData->device,NULL);
    cluClErrorTest(clResult);

    /* Create command queue based on configuration */
    {
        cl_command_queue_properties cqProperties = 0;
        struct CluProfiler *profiler;
        profiler = bfSceneGetProfiler(scene);

        if(profiler->profilingEnabled)
        {
            cqProperties = CL_QUEUE_PROFILING_ENABLE;
        }

        testData->queue = clCreateCommandQueue(testData->context, testData->device, cqProperties, &result);
        kzsErrorForward(result);
    }


    testData->profiler = bfSceneGetProfiler(scene);

    result = bfUtilReadLocalWorkgroupSize(framework, "JuliaFractalLocalWorkSize", 2, &testData->localWorkSize);
    kzsErrorForward(result);

    bfSceneSetFrameCounter(scene, JULIA_FRAME_COUNT);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    {
        const kzUint width = GRID_SIZE;
        const kzUint height = GRID_SIZE;
        struct KzcTextureDescriptor descriptor;

        kzcTextureDescriptorSet(width, height, KZC_TEXTURE_FORMAT_RGBA, KZC_TEXTURE_FILTER_BILINEAR, KZC_TEXTURE_WRAP_CLAMP, KZC_TEXTURE_COMPRESSION_NONE, &descriptor);
        {
            kzUint size = width * height * 4;
            void* data;
            result = kzcMemoryAllocPointer(bfGetMemoryManager(framework), &data, size, "Texture data");
            kzsErrorForward(result);
            kzsMemset(data, 0, size);
            result = kzcTextureCreate(kzuProjectGetResourceManager(bfGetProject(framework)), KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, &descriptor, data, &testData->outputTexture);
            kzsErrorForward(result);
            result = kzcMemoryFreePointer(data);
            kzsErrorForward(result);
        }

	    kzsGlFinish();
        testData->outputImage = clCreateFromGLTexture2D(testData->context, CL_MEM_WRITE_ONLY, KZS_GL_TEXTURE_2D, 0, (GLuint) kzcTextureGetTextureHandle(testData->outputTexture), 
            &clResult);
        cluClErrorTest(clResult);
    }
#else 
    {
        const kzUint width = GRID_SIZE;
        const kzUint height = GRID_SIZE;
        cl_image_format format;
        format.image_channel_order = CL_RGBA;
        format.image_channel_data_type = CL_UNORM_INT8;
        testData->outputImage = clCreateImage2D(testData->context, CL_MEM_WRITE_ONLY, &format, width, height, 0, KZ_NULL, &clResult);
        cluClErrorTest(clResult);
    }
#endif

    {
        struct KzuMaterial* material;
        result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), "Materials/Julia_Textured/Julia_TexturedMaterial", &material);
        kzsErrorForward(result);

        {
            struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
            result = kzuPropertyManagerSetVoid(propertyManager, material, KZU_PROPERTY_TYPE_TEXTURE, testData->outputTexture);
            kzsErrorForward(result);
        }
    }

    {
        kzBool resourcesAreValid;

        result = cluImageLoadPNG(framework, "data/juliacolourmap.png", testData->context, &testData->colourTexture, &testData->kzColourTexture, 0);
        kzsErrorForward(result);

        result = bfValidateFile(framework, "data/juliacolourmap.png", IMAGE_JULIA_COLOR_MAP_HASH, &resourcesAreValid);
        kzsErrorForward(result);

        {
            kzBool binary;
            kzBool isValid;
            kzString compilerSettings = COMPILER_FLAGS;
            result = bfValidateFile(framework, "data/julia.cl", JULIA_HASH, &isValid);
            kzsErrorForward(result);
            resourcesAreValid &= isValid;
            result = cluGetBuiltProgramFromFileWithOptions(bfGetMemoryManager(framework), "data/julia.cl", KZ_FALSE, compilerSettings, testData->context, &testData->juliaProgram, &binary);
            kzsErrorForward(result);
            bfSceneSetUseProgramBinary(scene, binary);
        }
        bfSceneSetValidConfigurationData(scene, resourcesAreValid);
    }

    testData->juliaKernel = clCreateKernel(testData->juliaProgram, "juliaImage", &clResult);
    cluClErrorTest(clResult);
    testData->scale = 0.2f;
    testData->gridSize[0] = GRID_SIZE;
    testData->gridSize[1] = GRID_SIZE;

    kzsSuccess();
}

kzsError juliaSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct JuliaTestData *testData;
    kzsError result;
    cl_int clResult;
    cl_event curEvent;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));


#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    clResult = clEnqueueAcquireGLObjects(testData->queue, 1, &testData->outputImage, 0, KZ_NULL, KZ_NULL);
    cluClErrorTest(clResult);
#endif

    testData->scale += 0.015f;

    result = cluSetKernelArguments(testData->juliaKernel, sizeof(cl_mem), &testData->outputImage, sizeof(cl_mem), &testData->colourTexture, sizeof(cl_float), &testData->scale);
    kzsErrorForward(result);

    clResult = clEnqueueNDRangeKernel(testData->queue, testData->juliaKernel, 2, NULL, testData->gridSize, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "juliaKernel", curEvent);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    clResult = clEnqueueReleaseGLObjects(testData->queue, 1, &testData->outputImage, 0, KZ_NULL, KZ_NULL);
    cluClErrorTest(clResult);
#endif
    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

    kzsSuccess();
}

kzsError juliaSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct JuliaTestData *testData = NULL;
    kzsError result;
    cl_int clResult;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    result = bfUtilFreeLocalWorkgroupSize(testData->localWorkSize);
    kzsErrorForward(result);

    /* Free the images */
    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

    clResult = clReleaseKernel(testData->juliaKernel);
    cluClErrorTest(clResult);
    clResult = clReleaseProgram(testData->juliaProgram);
    cluClErrorTest(clResult);


    result = clReleaseMemObject(testData->outputImage);
    kzsErrorForward(result);
    clResult = clReleaseMemObject(testData->colourTexture);
    cluClErrorTest(clResult);

    clResult = clReleaseCommandQueue(testData->queue);
    cluClErrorTest(clResult);
    result = kzcImageDelete(testData->kzColourTexture);
    kzsErrorForward(result);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    result = kzcTextureDelete(testData->outputTexture);
    kzsErrorForward(result);
#endif

    kzsSuccess();
}


kzsError juliaCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct JuliaTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, juliaSceneLoad, juliaSceneUpdate, KZ_NULL,
        juliaSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData, "Julia test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Julia Rendering Test", "General", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;
    kzsSuccess();
}
