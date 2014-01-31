/**
* \file
* OpenCL benchmark video tests.
* Copyright 2011 by Rightware. All rights reserved.
*/
#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>
#include <benchmarkutil/util/bf_file_validator.h>

#include <application/kza_application.h>

#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader_material.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/properties/kzu_property_manager_void.h>
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
#include <clutil/video.h>
#include <clutil/clu_profiler.h>

#include <clmark/test_definitions.h>

#include "cl_bilateral.h"
#include "cl_histogram.h"
#include "cl_blur.h"
#include "cl_median.h"
#include "cl_sharpening.h"
#include "cl_image_common.h"


struct ImageTestData
{
    cl_context context; /**< OpenCL context. */
    
    cl_command_queue queue; /**< OpenCL command queue. */
    cl_device_id device; /**< OpencL device. */
    
    cl_mem intermediateImage; /**< Temp image for tests which require double buffering */ 

    cl_mem testImage; /** Initial frame from PNG image and every frame gets updated with new video frame */
    struct KzcImage *kzTestImage;
    cl_mem outputImage;
    
    struct MedianFilter *bilateralFilter; /**< Bilateral filter */ 

    struct HistogramFilter *histogramFilter; /**< Histogram equalization */

    struct BlurFilter *blurFilter; /**< Gaussian blur via separable convolution */

    struct MedianFilter *medianFilter; /**< Median filter. */

    struct SharpeningFilter *sharpeningFilter; /**< Image sharpening via Laplace filter */

    struct KzcTexture *textureFiltered; /**< Processed video frame */
    struct KzcTexture *textureOriginal; /**< Unprocessed video frame */
    struct VideoUtil *video; /**< Video utils state */

};


kzsError imageVSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError blurVideoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError blurVideoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError blurVideoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError histogramVideoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError histogramVideoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError histogramVideoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError bilateralVideoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError bilateralVideoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError bilateralVideoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError medianVideoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError medianVideoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError medianVideoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError sharpeningVideoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError sharpeningVideoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError sharpeningVideoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError videoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene, enum ImageTestType type);
kzsError videoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError videoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);


/** Basic initialization every image test frame needs */
KZ_INLINE void cleanAndAcquire( struct ImageTestData ** testData, struct BfScene* scene, cl_int *clResult ) 
{
    *testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(*testData));


#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    *clResult = clEnqueueAcquireGLObjects((*testData)->queue, 1, &((*testData)->outputImage), 0, KZ_NULL, KZ_NULL);
    cluClErrorTest(*clResult);
#endif
}

KZ_INLINE void releaseAndFinish( struct ImageTestData * testData, cl_int *clResult) 
{
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    *clResult = clEnqueueReleaseGLObjects(testData->queue, 1, &testData->outputImage, 0, KZ_NULL, KZ_NULL);
    cluClErrorTest(*clResult);
#endif

    *clResult = clFinish(testData->queue);
    cluClErrorTest(*clResult);
}

kzsError bilateralVideoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;

    struct ImageTestData *testData = NULL;
    result = videoSceneLoad(framework, scene, IMAGE_TEST_TYPE_SURFACE_SMOOTHING);
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = initBilateral(framework, scene, testData->context, &testData->bilateralFilter);
    kzsErrorForward(result);
    result = bilateralVideoSceneUpdate(framework, 0, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError bilateralVideoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cl_int clResult;

    result = videoSceneUpdate(framework, deltaTime, scene);
    kzsErrorForward(result);

    cleanAndAcquire(&testData, scene, &clResult);
    
    result = applyBilateral(testData->queue, testData->bilateralFilter, testData->testImage, testData->outputImage);
    kzsErrorForward(result);

    releaseAndFinish(testData, &clResult);
    cluClErrorTest(clResult);
    kzsSuccess();
}

kzsError bilateralVideoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    cl_int clResult;
    struct ImageTestData *testData = NULL;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);
    result = deleteBilateral(testData->bilateralFilter);
    kzsErrorForward(result);
    result = videoSceneUninitialize(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError medianVideoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    result = videoSceneLoad(framework, scene, IMAGE_TEST_TYPE_NOISE_REDUCTION);
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = initMedian(framework, scene, testData->context, &testData->medianFilter);
    kzsErrorForward(result);
    result = medianVideoSceneUpdate(framework, 0, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError medianVideoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cl_int clResult;

    result = videoSceneUpdate(framework, deltaTime, scene);
    kzsErrorForward(result);
    cleanAndAcquire(&testData, scene, &clResult);

    result = applyMedian(testData->queue, testData->medianFilter, testData->testImage, testData->outputImage );
    kzsErrorForward(result);

    releaseAndFinish(testData, &clResult);
    
    kzsSuccess();
}

kzsError medianVideoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    cl_int clResult;
    struct ImageTestData *testData = NULL;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);
    result = deleteMedian(testData->medianFilter);
    kzsErrorForward(result);
    result = videoSceneUninitialize(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}



kzsError histogramVideoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    result = videoSceneLoad(framework, scene, IMAGE_TEST_TYPE_COLOR_CORRECTION);
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = initHistogram(framework, scene, testData->context, &testData->histogramFilter);
    kzsErrorForward(result);
    result = histogramVideoSceneUpdate(framework, 0, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError histogramVideoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;

    result = videoSceneUpdate(framework, deltaTime, scene);
    kzsErrorForward(result);
    cleanAndAcquire(&testData, scene, &result);

    result = applyHistogram(testData->queue, testData->histogramFilter, testData->testImage, testData->outputImage );
    kzsErrorForward(result);

    releaseAndFinish(testData, &result);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError histogramVideoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = clFinish(testData->queue);
    kzsErrorForward(result);
    result = deleteHistogram(testData->histogramFilter);
    kzsErrorForward(result);
    result = videoSceneUninitialize(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}



kzsError blurVideoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    result = videoSceneLoad(framework, scene, IMAGE_TEST_TYPE_SMOOTHING);
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = initBlur(framework, scene, testData->context, testData->testImage, &testData->blurFilter);
    kzsErrorForward(result);
    result = blurVideoSceneUpdate(framework, 0, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError blurVideoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;

    result = videoSceneUpdate(framework, deltaTime, scene);
    kzsErrorForward(result);
    cleanAndAcquire(&testData, scene, &result);

    result = applyBlur(testData->queue, testData->blurFilter, testData->testImage, testData->outputImage );
    kzsErrorForward(result);

    releaseAndFinish(testData, &result);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError blurVideoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = clFinish(testData->queue);
    kzsErrorForward(result);
    result = deleteBlur(testData->blurFilter);
    kzsErrorForward(result);
    result = videoSceneUninitialize(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError sharpeningVideoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    result = videoSceneLoad(framework, scene, IMAGE_TEST_TYPE_SHARPENING);
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = initSharpening(framework, scene, testData->context, &testData->sharpeningFilter);
    kzsErrorForward(result);
    /* Sharpening test uses Gaussian blur to reduce high frequency noise */
    result = initBlur(framework, scene, testData->context, testData->testImage, &testData->blurFilter);
    kzsErrorForward(result);
    result = sharpeningVideoSceneUpdate(framework, 0, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError sharpeningVideoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cl_int clResult;
    size_t imorigin[3] = {0, 0, 0};
    size_t imrect[3] = {512, 512, 1};
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    
    result = videoSceneUpdate(framework, deltaTime, scene);
    kzsErrorForward(result);
    cleanAndAcquire(&testData, scene, &result);
    clResult = clEnqueueCopyImage(testData->queue, testData->testImage, testData->intermediateImage, imorigin, imorigin, imrect, 0, NULL, NULL);
    cluClErrorTest(clResult);


    result = applyBlur(testData->queue, testData->blurFilter, testData->intermediateImage, testData->testImage );
    kzsErrorForward(result);
    result = applySharpening(testData->queue, testData->sharpeningFilter, testData->intermediateImage, testData->testImage, testData->outputImage);
    kzsErrorForward(result);


    releaseAndFinish(testData, &result);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError sharpeningVideoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    cl_int clResult;
    struct ImageTestData *testData = NULL;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);
    result = deleteSharpening(testData->sharpeningFilter);
    kzsErrorForward(result);
    result = deleteBlur(testData->blurFilter);
    kzsErrorForward(result);

    result = videoSceneUninitialize(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError videoSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene, enum ImageTestType type)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    struct KzcMemoryManager *memoryManager;
    kzUint width = 512;
    kzUint height = 512;
    size_t imsize[2];

    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/image_test_scene");
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    memoryManager = bfGetMemoryManager(framework);
    result = clImageCommonAssignTexture(framework, type);
    kzsErrorForward(result);

    testData->context = bfGetClContext(framework);
    kzsErrorForward(result);
    result = clGetContextInfo(testData->context,CL_CONTEXT_DEVICES,sizeof(cl_device_id),&testData->device,NULL);
    kzsErrorForward(result);
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

    {
        kzBool fullProfile;
        result = bfGetCLFullProfile(framework, &fullProfile);
        kzsErrorForward(result);
        bfSceneSetScoreWeightFactor(scene, (kzFloat)(fullProfile ? VIDEO_TEST_WEIGHT_FULL : VIDEO_TEST_WEIGHT_EMBEDDED));
    }

    bfSceneSetFrameCounter(scene, 387);
    videoInit(framework, "data/movie1.mpeg", 512, 512, &testData->video);

    result = cluImageLoadPNG(framework, "data/movie1_first_frame.png", testData->context, &testData->testImage, &testData->kzTestImage, 0);
    kzsErrorForward(result); 
    result = cluGetImageSize(testData->testImage, imsize);
    kzsErrorForward(result);
    width = imsize[0];
    height = imsize[1];
    
    {
        kzBool isValid1;
        kzBool isValid2;
        result = bfValidateFile(framework, "data/movie1.mpeg", VIDEO_MOVIE1, &isValid1);
        kzsErrorForward(result);
        result = bfValidateFile(framework, "data/movie1_first_frame.png", VIDEO_FIRST_FRAME, &isValid2);
        kzsErrorForward(result);
        
        bfSceneSetValidConfigurationData(scene, isValid1 == KZ_TRUE && isValid2 == KZ_TRUE);
    }

    {    
        struct KzcTextureDescriptor descriptor;

        kzcTextureDescriptorSet(width, height, KZC_TEXTURE_FORMAT_RGBA, KZC_TEXTURE_FILTER_POINT_SAMPLE, KZC_TEXTURE_WRAP_CLAMP,
            KZC_TEXTURE_COMPRESSION_NONE, &descriptor);

        {
            kzUint size = width * height * 4;
            void* data;
            result = kzcMemoryAllocPointer(bfGetMemoryManager(framework), &data, size, "Texture data");
            kzsErrorForward(result);
            kzsMemset(data, 0, size);
            result = kzcTextureCreate(kzuProjectGetResourceManager(bfGetProject(framework)), KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, &descriptor, data, &testData->textureFiltered);
            kzsErrorForward(result);
            result = kzcMemoryFreePointer(data);
            kzsErrorForward(result);
        }

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
        testData->outputImage = clCreateFromGLTexture2D(testData->context, CL_MEM_READ_WRITE, KZS_GL_TEXTURE_2D, 0, kzcTextureGetTextureHandle(testData->textureFiltered), &result);
        cluClErrorTest(result);
#else
        {
            cl_int clResult;
            cl_image_format format;
            format.image_channel_order = CL_RGBA;
            format.image_channel_data_type = CL_UNORM_INT8;
            testData->outputImage = clCreateImage2D(testData->context, CL_MEM_WRITE_ONLY, &format, width, height, 0, KZ_NULL, &clResult);
            cluClErrorTest(clResult);
        }
#endif

        result = kzcTextureCreateFromImage(kzuProjectGetResourceManager(bfGetProject(framework)), KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, 
            KZC_TEXTURE_FILTER_BILINEAR, KZC_TEXTURE_WRAP_CLAMP, KZC_TEXTURE_COMPRESSION_NONE, testData->kzTestImage, &testData->textureOriginal);
        kzsErrorForward(result);

        {
            cl_int clResult;
            cl_image_format format;
            format.image_channel_order = CL_RGBA;
            format.image_channel_data_type = CL_UNORM_INT8;
            testData->intermediateImage = clCreateImage2D(testData->context, CL_MEM_COPY_HOST_PTR|CL_MEM_READ_WRITE, &format, width, height, width * 4, kzcImageGetData(testData->kzTestImage), &clResult);
            cluClErrorTest(clResult);
        }


    }

    /* Set texture to material. */
    {
        struct KzuMaterial* material;
        result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), "Materials/image_test_textured_view/image_test_TestImageFilteredMat", &material);
        kzsErrorForward(result);

        {
            struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
            result = kzuPropertyManagerSetVoid(propertyManager, material, KZU_PROPERTY_TYPE_TEXTURE, testData->textureFiltered);
            kzsErrorForward(result);
        }
    }

    /* Set texture to 'original' material. */
    {
        struct KzuMaterial* material;
        result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), "Materials/image_test_textured_view/image_test_TestImageOriginalMat", &material);
        kzsErrorForward(result);

        {
            struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
            result = kzuPropertyManagerSetVoid(propertyManager, material, KZU_PROPERTY_TYPE_TEXTURE, testData->textureOriginal);
            kzsErrorForward(result);
        }
    }
    /* Grab the first frame of the video */
    {
        void *data; 
        result = videoNextFrame(framework, testData->video);
        kzsErrorForward(result);
        data = videoGetFramePointer(testData->video);

        result = kzcTextureUpdateData(testData->textureOriginal, data);
        kzsErrorForward(result);
    }
    kzsSuccess();
}

kzsError videoSceneRender(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    void *data; 
    kzsError result;
    testData = bfSceneGetUserData(scene);
    result = videoNextFrame(framework, testData->video);
    kzsErrorForward(result);
    data = videoGetFramePointer(testData->video);

    result = kzcTextureUpdateData(testData->textureOriginal, data);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError videoSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    cl_int clResult;
    struct KzcMemoryManager *memoryManager;

    size_t imorigin[3] = {0, 0, 0};
    size_t imrect[3] = {512, 512, 1};
    void *data;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    memoryManager = bfGetMemoryManager(framework);
   
    data = videoGetFramePointer(testData->video);
    
    clResult = clEnqueueWriteImage(testData->queue, testData->testImage, 0, imorigin, imrect, 0, 0, data, 0, NULL, NULL);
    cluClErrorTest(clResult);

    kzsSuccess();
}

kzsError videoSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = clFinish(testData->queue);
    kzsErrorForward(result);
    videoUnitialize(framework, testData->video);
    result = imageVSceneUninitialize(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}




kzsError imageVSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cl_int clResult;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    /* Free the images */
    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);


    clResult = clReleaseMemObject(testData->testImage);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->outputImage);
    cluClErrorTest(clResult);

    clResult = clReleaseMemObject(testData->intermediateImage);
    cluClErrorTest(clResult);

    clResult = clReleaseCommandQueue(testData->queue);
    cluClErrorTest(clResult);
    result = kzcImageDelete(testData->kzTestImage);
    kzsErrorForward(result);
    result = kzcTextureDelete(testData->textureFiltered);
    kzsErrorForward(result);
    result = kzcTextureDelete(testData->textureOriginal);
    kzsErrorForward(result);

    kzsSuccess();
}


kzsError sharpeningVideoCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{    
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct ImageTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, sharpeningVideoSceneLoad, sharpeningVideoSceneUpdate, videoSceneRender,
        sharpeningVideoSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData, "Sharpnening test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Video Sharpening Test", "Video manipulation", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;    
    kzsSuccess();
}

kzsError blurVideoCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct ImageTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, blurVideoSceneLoad, blurVideoSceneUpdate, videoSceneRender,
        blurVideoSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager,testData,"Blur test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Video Blur Test", "Video manipulation", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;    
    kzsSuccess();
}

kzsError histogramVideoCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{   
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct ImageTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, histogramVideoSceneLoad, histogramVideoSceneUpdate, videoSceneRender,
        histogramVideoSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData, "Histogram test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Video Color Correction Test", "Video manipulation", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;    
    kzsSuccess();
}

kzsError bilateralVideoCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct ImageTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, bilateralVideoSceneLoad, bilateralVideoSceneUpdate, videoSceneRender,
        bilateralVideoSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData, "Bilateral test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Video Surface Smoothing Test", "Video manipulation", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;    
    kzsSuccess();
}

kzsError medianVideoCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct ImageTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, medianVideoSceneLoad, medianVideoSceneUpdate, videoSceneRender,
        medianVideoSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData, "Video noise reduction test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Video Noise Reduction Test", "Video manipulation", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;   
    kzsSuccess();
}
