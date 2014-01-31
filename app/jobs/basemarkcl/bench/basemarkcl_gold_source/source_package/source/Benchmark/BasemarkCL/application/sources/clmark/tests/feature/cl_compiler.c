/**
 * \file
 * OpenCL Compiler speed tests
 * Copyright 2011 by Rightware. All rights reserved.
 */

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>
#include <benchmarkutil/report/bf_report.h>

#include <clutil/clu_platform.h>
#include <clutil/clu_program.h>
#include <clutil/clu_kernel.h>
#include <clutil/clu_floatbuffer.h>
#include <clutil/clu_util.h>

#include <application/kza_application.h>

#include <user/project/kzu_project_loader_material.h>
#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_float_property.h>
#include <user/material/kzu_material.h>
#include <user/material/kzu_material_type.h>

#include <core/debug/kzc_log.h>
#include <core/util/math/kzc_random.h>
#include <core/memory/kzc_memory_manager.h>

#include <clutil/clu_opencl_base.h>
#define PROGRAM_COUNT 13
/** Vector test state. */


struct CompilerTestState
{
    cl_command_queue queue; /**< Command queue. */
    cl_device_id device; /**< Device id. */

    kzUint numPrograms;
    cl_program* testPrograms;

    struct KzuMaterial* loadingMaterial; /**< Material used to render progress bar. */
    struct KzuPropertyType* loadingPropertyType; /**< Property driving progress bar position. */


};


kzsError compilerSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError compilerSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError compilerSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);


kzsError compilerSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    cl_context context;
    kzsError result;
    cl_int clResult;
    struct CompilerTestState *testData;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    context = bfGetClContext(framework);
    clResult = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &testData->device, KZ_NULL);
    cluClErrorTest(clResult);
    testData->queue = clCreateCommandQueue(context, testData->device, 0, &clResult);
    cluClErrorTest(clResult);
    bfSceneSetFrameCounter(scene, PROGRAM_COUNT);
    bfSceneDisableFromOverallScore(scene);
    
    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/ProgressBar");
    kzsErrorForward(result);

    {
        struct KzuMaterialType* materialType;
        result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), "Materials/LoadBarTexture/IntervalSceneLoadBar", &testData->loadingMaterial);
        kzsErrorForward(result);
        materialType = kzuMaterialGetMaterialType(testData->loadingMaterial);
        testData->loadingPropertyType = kzuMaterialTypeGetPropertyTypeByName(materialType, "LoadAmount");
    }

    kzsSuccess();
}

kzsError compilerSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    kzsError result;
    cl_int clResult;
    cl_context context;

    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(framework);
    struct CompilerTestState *testData = NULL;
    cl_program testProgram;
    kzString programNames[PROGRAM_COUNT] = 
    {
        "data/fluid.cl",
        "data/sph.cl",
        "data/median.cl",
        "data/wave_simulation.cl",
        "data/basic_image_operations.cl",
        "data/fft.cl",
        "data/histogram.cl",
        "data/julia.cl",
        "data/mandelbulb.cl",   
        "data/convolution2d.cl",
        "data/separable_convolution2d.cl",
        "data/softbody.cl",
        "data/bilateral.cl"
    };

    context = bfGetClContext(framework);

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    
    result = cluGetBuiltProgramFromFile(memoryManager, programNames[bfSceneGetFrameCounterValue(scene) - 1], KZ_TRUE, context, &testProgram, KZ_NULL);
    kzsErrorForward(result);

    clResult = clReleaseProgram(testProgram);
    cluClErrorTest(clResult);


    {   
        kzUint framesElapsed = bfSceneGetFrameCounterValue(scene);
        struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(testData->loadingMaterial);
        kzFloat loadingAmount = 1.0f - framesElapsed / ((kzFloat)bfSceneGetFrameInitialCounterValue(scene));
        result = kzuPropertyManagerSetFloat(propertyManager, testData->loadingMaterial, testData->loadingPropertyType, loadingAmount);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError compilerSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct CompilerTestState *testData;
    cl_int clResult;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    clResult = clReleaseCommandQueue(testData->queue);
    cluClErrorTest(clResult);

    kzsSuccess();
}


kzsError compilerTestCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct CompilerTestState *testData = KZ_NULL;

    result = bfTestConfigurationInitialize(memoryManager, compilerSceneLoad, compilerSceneUpdate, KZ_NULL,
        compilerSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData, "OpenCL compiler test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Online Compiler Test", "General", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;
    kzsSuccess();
}
