/**
 * \file
 * OpenCL Image Tests
 * Copyright 2011 by Rightware. All rights reserved.
 */
#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>
#include <benchmarkutil/settings/bf_settings.h>
#include <benchmarkutil/util/bf_file_validator.h>

#include <application/kza_application.h>

#include <user/project/kzu_project.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/properties/kzu_property_manager_void.h>
#include <user/properties/kzu_texture_property.h>
#include <user/material/kzu_material.h>
#include <user/project/kzu_project_loader_material.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/texture/kzc_texture_descriptor.h>
#include <core/util/image/kzc_image.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/debug/kzc_log.h>
#include <core/util/math/kzc_vector2.h>
#include <user/properties/kzu_property_manager_vector2.h>
#include <user/properties/kzu_property_manager_float.h>

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

#include "cl_bilateral.h"
#include "cl_histogram.h"
#include "cl_blur.h"
#include "cl_median.h"
#include "cl_sharpening.h"
#include "cl_image_common.h"



/** Frame count of the test. */
#define TEST_DURATION IMAGE_FRAME_COUNT

/** Number of images. */
#define IMAGE_TEST_IMAGE_COUNT 5

/** Image test image paths. */
const kzString imageTestImagePaths[IMAGE_TEST_IMAGE_COUNT] = 
{
    "data/photo1.png",
    "data/photo2.png",
    "data/photo3.png",
    "data/photo4.png",
    "data/photo5.png"
};


struct ImageTestData
{
    cl_context context; /**< OpenCL context. */
    
    cl_command_queue queue; /**< OpenCL command queue. */
    cl_device_id device; /**< OpenCL device. */
    
    cl_mem originalImage; /**< Unmodified image for tests which do not iterate the used filter */
    cl_mem testImage; /**< Original image where the picture is initially loaded */
    struct KzcImage* kzTestImage;
    cl_mem outputImage; /**< Filtered image which is actually displayed */

    kzBool useImagePreloading; /**< Is image preloading enabled. */
    struct KzcImage* preloadedImages[IMAGE_TEST_IMAGE_COUNT]; /**< Preloaded images. */
    cl_mem preloadedImageBuffers[IMAGE_TEST_IMAGE_COUNT];  /*< Preloaded image buffers. */

    struct MedianFilter *bilateralFilter; /**< Bilateral filter */
    struct HistogramFilter *histogramFilter; /**< Histogram equalization */
    struct BlurFilter *blurFilter; /**< Gaussian blur via separable convolution */
    struct MedianFilter *medianFilter; /**< Median filter. */    
    struct SharpeningFilter *sharpeningFilter; /**< Image sharpening via laplace filter */

    struct KzcTexture* textureFiltered; /**< Processed video frame */
    struct KzcTexture* textureOriginal; /**< Unprocessed video frame */

    kzUint activeImage; /**< Currently shown image index. */

    kzUint scaling; /** Scaling factor for embedded profile */

};


kzsError imageSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene, enum ImageTestType type);
kzsError imageSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError imageSceneRender(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError imageSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError blurSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError blurSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError blurSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError histogramSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError histogramSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError histogramSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError bilateralSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError bilateralSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError bilateralSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError medianSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError medianSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError medianSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

kzsError sharpeningSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError sharpeningSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError sharpeningSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);


/* Basic initialization every image test frame needs */
void cleanAndAcquire( struct ImageTestData ** testData, struct BfScene* scene, cl_int *clResult ) 
{
    *testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(*testData));

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    *clResult = clEnqueueAcquireGLObjects((*testData)->queue, 1, &((*testData)->outputImage), 0, KZ_NULL, KZ_NULL);
    cluClErrorTest(*clResult);
    *clResult = clEnqueueAcquireGLObjects((*testData)->queue, 1, &((*testData)->originalImage), 0, KZ_NULL, KZ_NULL);
    cluClErrorTest(*clResult);
#endif
}

void releaseAndFinish( struct ImageTestData * testData, cl_int *clResult) 
{
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    *clResult = clEnqueueReleaseGLObjects(testData->queue, 1, &testData->outputImage, 0, KZ_NULL, KZ_NULL);
    cluClErrorTest(*clResult);
    *clResult = clEnqueueReleaseGLObjects(testData->queue, 1, &testData->originalImage, 0, KZ_NULL, KZ_NULL);
    cluClErrorTest(*clResult);
#endif
    *clResult = clFinish(testData->queue);
    cluClErrorTest(*clResult);
}

kzsError bilateralSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    result = imageSceneLoad(framework, scene, IMAGE_TEST_TYPE_SURFACE_SMOOTHING);
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = initBilateral(framework, scene, testData->context, &testData->bilateralFilter);
    kzsErrorForward(result);
    result = bilateralSceneUpdate(framework, 0, scene);
    kzsErrorForward(result);
    result = imageSceneRender(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError bilateralSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cl_int clResult;

    cleanAndAcquire(&testData, scene, &clResult);
    
    result = applyBilateral(testData->queue, testData->bilateralFilter, testData->testImage, testData->outputImage);
    kzsErrorForward(result);
    result = applyBilateral(testData->queue, testData->bilateralFilter, testData->outputImage, testData->testImage);
    kzsErrorForward(result);

    releaseAndFinish(testData, &clResult);
    cluClErrorTest(clResult);
    kzsSuccess();
}

kzsError bilateralSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
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
    result = imageSceneUninitialize(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError medianSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    result = imageSceneLoad(framework, scene, IMAGE_TEST_TYPE_NOISE_REDUCTION);
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = initMedian(framework, scene, testData->context, &testData->medianFilter);
    kzsErrorForward(result);
    result = medianSceneUpdate(framework, 0, scene);
    kzsErrorForward(result);
    result = imageSceneRender(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError medianSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cl_int clResult;
    cleanAndAcquire(&testData, scene, &clResult);

    result = applyMedian(testData->queue, testData->medianFilter, testData->testImage, testData->outputImage );
    kzsErrorForward(result);
    result = applyMedian(testData->queue, testData->medianFilter, testData->outputImage, testData->testImage);
    kzsErrorForward(result);

    releaseAndFinish(testData, &clResult);
    
    kzsSuccess();
}

kzsError medianSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
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
    result = imageSceneUninitialize(framework, scene);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError histogramSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    result = imageSceneLoad(framework, scene, IMAGE_TEST_TYPE_COLOR_CORRECTION);
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = initHistogram(framework, scene, testData->context, &testData->histogramFilter);
    kzsErrorForward(result);
    result = histogramSceneUpdate(framework, 0, scene);
    kzsErrorForward(result);
    result = imageSceneRender(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError histogramSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cl_int clResult;
    cleanAndAcquire(&testData, scene, &clResult);

    result = applyHistogram(testData->queue, testData->histogramFilter, testData->originalImage, testData->outputImage);
    kzsErrorForward(result);

    releaseAndFinish(testData, &clResult);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError histogramSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    cl_int clResult;
    struct ImageTestData *testData = NULL;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);
    result = deleteHistogram(testData->histogramFilter);
    kzsErrorForward(result);
    result = imageSceneUninitialize(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError blurSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    result = imageSceneLoad(framework, scene, IMAGE_TEST_TYPE_SMOOTHING);
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = initBlur(framework, scene, testData->context, testData->testImage, &testData->blurFilter);
    kzsErrorForward(result);
    result = blurSceneUpdate(framework, 0, scene);
    kzsErrorForward(result);
    result = imageSceneRender(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError blurSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cleanAndAcquire(&testData, scene, &result);

    result = applyBlur(testData->queue, testData->blurFilter, testData->originalImage, testData->outputImage);
    kzsErrorForward(result);

    releaseAndFinish(testData, &result);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError blurSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = deleteBlur(testData->blurFilter);
    kzsErrorForward(result);
    result = imageSceneUninitialize(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError sharpeningSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    struct ImageTestData *testData = NULL;
    result = imageSceneLoad(framework, scene, IMAGE_TEST_TYPE_SHARPENING);
    kzsErrorForward(result);
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    result = initSharpening(framework, scene, testData->context, &testData->sharpeningFilter);
    kzsErrorForward(result);
    /* Sharpening test uses Gaussian blur to reduce high frequency noise */
    result = initBlur(framework, scene, testData->context, testData->testImage, &testData->blurFilter);
    kzsErrorForward(result);
    result = sharpeningSceneUpdate(framework, 0, scene);
    kzsErrorForward(result);
    result = imageSceneRender(framework, scene);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError sharpeningSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cleanAndAcquire(&testData, scene, &result);

    result = applyBlur(testData->queue, testData->blurFilter, testData->originalImage, testData->testImage );
    kzsErrorForward(result);
    result = applySharpening(testData->queue, testData->sharpeningFilter, testData->originalImage, testData->testImage, testData->outputImage);
    kzsErrorForward(result);

    releaseAndFinish(testData, &result);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError sharpeningSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
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
    result = imageSceneUninitialize(framework, scene);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError imageSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene, enum ImageTestType type)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cl_int clResult;
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

    {
        kzBool fullProfile;
        result = bfGetCLFullProfile(framework, &fullProfile);
        kzsErrorForward(result);

        if(fullProfile)
        {
            testData->scaling = 0;
        }
        else
        {
            testData->scaling = 2;
        }
    }
    /* Create command queue based on configuration */
    {
        cl_command_queue_properties cqProperties = 0;
        struct CluProfiler *profiler;
        profiler = bfSceneGetProfiler(scene);
    
        if(profiler->profilingEnabled)
        {
            cqProperties = CL_QUEUE_PROFILING_ENABLE;
        }

        testData->queue = clCreateCommandQueue(testData->context, testData->device, cqProperties, &clResult);
        kzsErrorForward(clResult);
    }

    {
        kzBool fullProfile;
        result = bfGetCLFullProfile(framework, &fullProfile);
        kzsErrorForward(result);
        bfSceneSetScoreWeightFactor(scene, (kzFloat)(fullProfile ? IMAGE_TEST_WEIGHT_FULL : IMAGE_TEST_WEIGHT_EMBEDDED));
    }

    bfSceneSetFrameCounter(scene, TEST_DURATION);

    testData->activeImage = 0;

    {
        kzBool validImages = KZ_TRUE;
        kzInt preload;
        struct KzcSettingContainer* settings = bfGetSettings(framework);
        result = settingGetInt(settings, "ImageTestPreloadImages", &preload);
        kzsErrorForward(result);
        testData->useImagePreloading = (preload == 0 ? KZ_FALSE : KZ_TRUE);
        if(testData->useImagePreloading)
        {
            kzUint i;
            for(i = 0; i < IMAGE_TEST_IMAGE_COUNT; ++i)
            {
                result = cluImageLoadPNG(framework, imageTestImagePaths[i], testData->context, &testData->preloadedImageBuffers[i], &testData->preloadedImages[i], testData->scaling);
                kzsErrorForward(result);
            }
        }

        /* If previous ones are valid. Validate rest. */
        if(bfSceneGetValidConfigurationData(scene))
        {
            kzUint i;
            for(i = 0; i < IMAGE_TEST_IMAGE_COUNT; ++i)
            {
                kzBool isValid;
                kzString hash;
                switch(i)
                {
                    case 0: hash = IMAGE_TEST_HASH1; break;
                    case 1: hash = IMAGE_TEST_HASH2; break;
                    case 2: hash = IMAGE_TEST_HASH3; break;
                    case 3: hash = IMAGE_TEST_HASH4; break;
                    default: hash = IMAGE_TEST_HASH5; break;
                }
                result = bfValidateFile(framework, imageTestImagePaths[i], hash, &isValid);
                kzsErrorForward(result); 
                validImages &= isValid;
            }
            bfSceneSetValidConfigurationData(scene, validImages);
        }
    }

    if(testData->useImagePreloading)
    {
        testData->kzTestImage= testData->preloadedImages[0];
        testData->testImage = testData->preloadedImageBuffers[0];
    }
    else
    { 
        result = cluImageLoadPNG(framework, imageTestImagePaths[0], testData->context, &testData->testImage, &testData->kzTestImage, testData->scaling);
        kzsErrorForward(result);
    }

    result = cluGetImageSize(testData->testImage, imsize);
    kzsErrorForward(result);
    width = imsize[0];
    height = imsize[1];
    {
        struct KzcTextureDescriptor descriptor;

        kzcTextureDescriptorSet(width, height, KZC_TEXTURE_FORMAT_RGBA, KZC_TEXTURE_FILTER_POINT_SAMPLE, KZC_TEXTURE_WRAP_REPEAT,
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
        testData->outputImage = clCreateFromGLTexture2D(testData->context, CL_MEM_READ_WRITE, KZS_GL_TEXTURE_2D, 0, kzcTextureGetTextureHandle(testData->textureFiltered), &clResult);
        cluClErrorTest(clResult);
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
            KZC_TEXTURE_FILTER_BILINEAR, KZC_TEXTURE_WRAP_REPEAT, KZC_TEXTURE_COMPRESSION_NONE, testData->kzTestImage, &testData->textureOriginal);
        kzsErrorForward(result);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
        testData->originalImage = clCreateFromGLTexture2D(testData->context, CL_MEM_READ_ONLY, KZS_GL_TEXTURE_2D, 0, kzcTextureGetTextureHandle(testData->textureOriginal), &clResult);
        cluClErrorTest(clResult);
#else
        {
            cl_int clResult;
            cl_image_format format;
            format.image_channel_order = CL_RGBA;
            format.image_channel_data_type = CL_UNORM_INT8;
            testData->originalImage = clCreateImage2D(testData->context, CL_MEM_COPY_HOST_PTR|CL_MEM_READ_ONLY, &format, width, height, width * 4, kzcImageGetData(testData->kzTestImage), &clResult);
            cluClErrorTest(clResult);
        }
#endif
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
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    /* Make sure that all pending gl operations from engine side have been flushed as some test initializations call cl code */
    glFinish();
#endif

    kzsSuccess();
}

kzsError imageSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct ImageTestData *testData = NULL;
    kzsError result;
    cl_int clResult;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

    if(testData->useImagePreloading)
    {
        kzUint i;
        for(i = testData->activeImage; i < IMAGE_TEST_IMAGE_COUNT; ++i)
        {
            result = kzcImageDelete(testData->preloadedImages[i]);
            kzsErrorForward(result);
            
            
            result = clReleaseMemObject(testData->preloadedImageBuffers[i]);
            kzsErrorForward(result);
            
        }
    }
    else
    {
        result = kzcImageDelete(testData->kzTestImage);
        kzsErrorForward(result);
        result = clReleaseMemObject(testData->testImage);
        kzsErrorForward(result);
    }
    
    result = clReleaseMemObject(testData->outputImage);
    kzsErrorForward(result);
    result = clReleaseMemObject(testData->originalImage);
    kzsErrorForward(result);
    
    result = kzcTextureDelete(testData->textureFiltered);
    kzsErrorForward(result);
    result = kzcTextureDelete(testData->textureOriginal);
    kzsErrorForward(result);
    
    clResult = clReleaseCommandQueue(testData->queue);
    cluClErrorTest(clResult);

    kzsSuccess();
}

kzsError sharpeningCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{    
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct ImageTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, sharpeningSceneLoad, sharpeningSceneUpdate, imageSceneRender,
        sharpeningSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData, "Sharpnening test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Image Sharpening Test", "Image manipulation", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;    
    kzsSuccess();
}

kzsError blurCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct ImageTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, blurSceneLoad, blurSceneUpdate, imageSceneRender,
        blurSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager,testData,"Blur test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Image Blur Test", "Image manipulation", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;    
    kzsSuccess();
}

kzsError histogramCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{   
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct ImageTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, histogramSceneLoad, histogramSceneUpdate, imageSceneRender,
        histogramSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData, "Histogram test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Image Color Correction Test", "Image manipulation", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;    
    kzsSuccess();
}

kzsError bilateralCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct ImageTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, bilateralSceneLoad, bilateralSceneUpdate, imageSceneRender,
        bilateralSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData, "Bilateral test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Image Surface Smoothing Test", "Image manipulation", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;    
    kzsSuccess();
}

kzsError medianCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct ImageTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, medianSceneLoad, medianSceneUpdate, imageSceneRender,
        medianSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData, "Median test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Image Noise Reduction Test", "Image manipulation", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;   
    kzsSuccess();
}

kzsError imageSceneRender(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsError result;
    cl_int clResult;
    kzUint frameIndex;
    kzUint imageIndex;
    struct ImageTestData *testData = NULL;
    struct KzuPropertyType* scale;
    struct KzuPropertyType* offset;
    kzFloat scaleValue;
    struct KzcVector2 offsetValue;
    kzsAssert(kzcIsValidPointer(scene));
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    scaleValue = 0.4f;

    frameIndex = bfSceneGetFrameInitialCounterValue(scene) - bfSceneGetFrameCounterValue(scene);
    kzsAssertText((TEST_DURATION / IMAGE_TEST_IMAGE_COUNT) >= 1.0f, "Increase test duration");
    imageIndex = (frameIndex / (TEST_DURATION / IMAGE_TEST_IMAGE_COUNT)) % IMAGE_TEST_IMAGE_COUNT;

    scale = kzuProjectGetPropertyType(bfGetProject(framework), "scale");
    offset = kzuProjectGetPropertyType(bfGetProject(framework), "offset");
    {
        kzFloat tval = ((kzFloat)frameIndex)/80.0f;
    
        offsetValue.data[0] = kzsSinf(tval)*0.3f + 0.5f;
        offsetValue.data[1] = kzsCosf(tval)*0.3f + 0.5f;
    }

    {
        struct KzuMaterial* material;
        result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), "Materials/image_test_textured_view/image_test_TestImageFilteredMat", &material);
        kzsErrorForward(result);

        {
            struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
            result = kzuPropertyManagerSetFloat(propertyManager, material, scale, scaleValue);
            kzsErrorForward(result);
            result = kzuPropertyManagerSetVector2(propertyManager, material, offset, offsetValue);
            kzsErrorForward(result);
        }
    }

    {
        struct KzuMaterial* material;
        result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), "Materials/image_test_textured_view/image_test_TestImageOriginalMat", &material);
        kzsErrorForward(result);

        {
            struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
            result = kzuPropertyManagerSetFloat(propertyManager, material, scale, scaleValue);
            kzsErrorForward(result);
            result = kzuPropertyManagerSetVector2(propertyManager, material, offset, offsetValue);
            kzsErrorForward(result);
        }
    }



    if(testData->activeImage != imageIndex)
    {
        testData->activeImage = imageIndex;

        /* Release old image. */
        {
            result = kzcImageDelete(testData->kzTestImage);
            kzsErrorForward(result);
            result = clReleaseMemObject(testData->testImage);
            kzsErrorForward(result);
        }

        /* Change the image in render function as this is not timed. */
        {
            if(testData->useImagePreloading)
            {
                testData->testImage = testData->preloadedImageBuffers[imageIndex];
                testData->kzTestImage = testData->preloadedImages[imageIndex];
            }
            else
            {
                result = cluImageLoadPNG(framework, imageTestImagePaths[imageIndex], testData->context, &testData->testImage, &testData->kzTestImage, testData->scaling);
                kzsErrorForward(result); 
            }
            {
                clResult = clReleaseMemObject(testData->originalImage);
                cluClErrorTest(clResult);
                result = kzcTextureDelete(testData->textureOriginal);
                kzsErrorForward(result);
                result = kzcTextureCreateFromImage(kzuProjectGetResourceManager(bfGetProject(framework)), KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, 
                    KZC_TEXTURE_FILTER_BILINEAR, KZC_TEXTURE_WRAP_REPEAT, KZC_TEXTURE_COMPRESSION_NONE, testData->kzTestImage, &testData->textureOriginal);
                kzsErrorForward(result);
                testData->originalImage = clCreateFromGLTexture2D(testData->context, CL_MEM_READ_ONLY, KZS_GL_TEXTURE_2D, 0, kzcTextureGetTextureHandle(testData->textureOriginal), &result);
                cluClErrorTest(result);
            }
#if 1
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
#endif
        }
    }

    kzsSuccess();
}
