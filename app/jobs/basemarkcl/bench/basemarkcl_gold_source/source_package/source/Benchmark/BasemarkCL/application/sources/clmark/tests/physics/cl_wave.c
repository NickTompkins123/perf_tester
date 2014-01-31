/**
* \file
* OpenCL FFT wave simulation.
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "dynamic_mesh.h"

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>
#include <benchmarkutil/util/bf_file_validator.h>
#include <benchmarkutil/settings/bf_settings.h>

#include <application/kza_application.h>

#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader_material.h>
#include <user/project/kzu_project_loader_object_node.h>
#include <user/project/kzu_project_loader_camera.h>
#include <user/material/kzu_material_type.h>
#include <user/material/kzu_material.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/renderer/kzu_renderer.h>
#include <user/animation/kzu_animation_player.h>

#include <core/util/math/kzc_random.h>
#include <core/util/io/kzc_file.h>
#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/renderer/kzc_renderer.h>
#include <core/resource_manager/shader/kzc_resource_shader.h>
#include <core/debug/kzc_log.h>

#include <system/display/kzs_window.h>
#include <system/display/kzs_surface.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_opengl.h>

#include <clutil/clu_platform.h>
#include <clutil/clu_program.h>
#include <clutil/clu_kernel.h>
#include <clutil/clu_util.h>
#include <clutil/clu_profiler.h>
#include <clutil/clu_opencl_base.h>

#include <clmark/test_definitions.h>



/* We limit ourselves to power of two grid sizes */
kzUint SIDE_LOG2;
#define SIDE_LENGTH (1u << SIDE_LOG2)
#define GRID_SIZE (SIDE_LENGTH*SIDE_LENGTH)
kzUint VBO_SIDE;
#define VBO_SIZE (VBO_SIDE*VBO_SIDE)
#define INDEX_COUNT ((VBO_SIDE-1)*(VBO_SIDE-1)* 2 * 3)


/** Amount of data in one vertex in VBO. */
#define VERTEX_DATA_SIZE 8


struct WaveTestData
{
    cl_context context; /**< OpenCL context. */
    cl_command_queue queue; /**< OpenCL command queue. */
    cl_device_id device; /**< OpenCL device. */

    cl_program waveProgram; /**< Wave simulation program object. */
    cl_program fftProgram; /**< FFT program object. */

    cl_kernel fft2dRow;
    cl_kernel fft2dColumn;
    cl_kernel ifft2dRow;
    cl_kernel ifft2dColumn;
    cl_kernel dispersion;
    cl_kernel updateVbo;
    cl_kernel calculateNormals[3];
    cl_kernel addFields;
    cl_kernel addDrop;
    cl_kernel zeroBoundaries;

    cl_float *tempbuffer; /**< Temp buffer for transferring data. */

    cl_mem frequencyDomainHeightField; /* Holds the initial unpropagated heightfield as complex_t*GRID_SIZE array */
    cl_mem heightField1; /* The two buffers for inverse transforming the frequencyDomainHeightField. */
    cl_mem heightField2;

    kzUint vboHandle;    

    cl_float t; /* Simulation time */
    
    struct KzcRandom* random; /* Random generator for the drop locations */

    kzUint particleFaceBufferSize; /**< Size of buffer containing indexes to faces. */
    kzInt* particleFaceIndexes; /**< Indices indicating faces where each particle belongs to. */
    cl_mem particleFaceIndexBuffer; /**< Cl buffer for per particle face normal. */

    kzUint vertexBufferObjectHandle; /**< Vertex Buffer Object OpenGL handle. */
    cl_mem vertexBufferObjectCLBuffer; /**< Cl buffer for vbo. */

    kzUint vboIndexBufferHandle; /** Index Buffer Object OpenGL handle. */
    cl_mem vboIndexBufferCl; /**< Cl buffer for IBO. */
    kzUint* indices; /**< Triangle indices for rendering. Same as IBO. */

    struct KzcVector4* faceNormal; /**< Unaveraged face normals. */
    cl_mem faceNormalBuffer; /**< Cl buffer for face normals. */
    
    struct KzuMaterial* material; /**< Material used to render wave. */
    struct DynamicMeshNode* dynamicMesh; /**< Kanzi scene graph object for water surface. */

    struct KzuScene* scene; /**< Scene for rendering cloth. */
    struct KzuCameraNode* cameras[2]; /**< Camera nodes used to render scene from different angles. */

    struct CluProfiler *profiler; /**< Clu profiler */

    size_t* localWorkSize; /**< Local work size. Read from configuration file. */
    size_t* localWorkSizeFFT; /**< Local work size. Read from configuration file. */

    cl_float dt; /**< deltaT which is applied at every iteration */
    kzUint dropRate; /**< How many frames are skipped between adding drops */
};


kzsError waveSceneRender(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError waveSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError waveSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError waveSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);

/** Applies dispersion to the frequency domain heightfield. */
kzsError applyDispersion(struct WaveTestData* testData, cl_float time);


kzsError renderWaterSurfaceCallback(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode, void* userData)
{
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    kzsError result;
    struct WaveTestData* testData = userData;
    struct KzcMatrix4x4 transformation = kzuTransformedObjectNodeGetMatrix(transformedObjectNode);

    struct KzcRenderer* coreRenderer;
    kzcRendererSetCullMode(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_CULL_MODE_NONE);

    kzsAssert(kzcIsValidPointer(renderer));
    coreRenderer = kzuRendererGetCoreRenderer(renderer);
    kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD, &transformation);

    {
        result = kzuRendererApplyMaterial(renderer, testData->material);
        kzsErrorForward(result);
    }

#define BUFFER_OFFSET(i) ((char *)NULL + (i) * sizeof(float))
    {
        kzInt* positionIndex;
        kzInt* normalIndex;
        struct KzuMaterialType* materialType = kzuMaterialGetMaterialType(testData->material);
        struct KzcShader* shader = kzuMaterialTypeGetShaderProgram(materialType);
        struct KzcHashMap* hashMap = kzcShaderGetAttributes(shader);
        kzBool found;
        found = kzcHashMapGet(hashMap, "kzPosition", (void**)&positionIndex);
        kzsAssertText(found, "Material must have position attribute.");
        found = kzcHashMapGet(hashMap, "kzNormal", (void**)&normalIndex);
        kzsAssertText(found, "Material must have normal attribute.");

        {
            kzsGlBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, testData->vboIndexBufferHandle);
            kzsGlBindBuffer(KZS_GL_ARRAY_BUFFER, testData->vertexBufferObjectHandle);

            kzsGlVertexAttribPointer(*positionIndex, 3, KZS_GL_FLOAT, KZ_FALSE, VERTEX_DATA_SIZE * sizeof(float), 0);
            kzsGlEnableVertexAttribArray(*positionIndex);
            kzsGlVertexAttribPointer(*normalIndex, 3, KZS_GL_FLOAT, KZ_FALSE, VERTEX_DATA_SIZE * sizeof(float), BUFFER_OFFSET(4));
            kzsGlEnableVertexAttribArray(*normalIndex);

            kzsGlDrawElements(KZS_GL_TRIANGLES, INDEX_COUNT, KZS_GL_UNSIGNED_INT, 0);

            kzsGlDisableVertexAttribArray(*positionIndex);
            kzsGlDisableVertexAttribArray(*normalIndex);

            kzsGlBindBuffer(KZS_GL_ARRAY_BUFFER, 0);
            kzsGlBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
#endif
    kzsSuccess();
}

/* Fourier transforms the targetbuffer using tempbuffer as temporary storage for the fft */
kzsError fft(struct WaveTestData * testData, cl_mem target, cl_mem tempbuffer)
{
    size_t grsize[2];
    size_t gcsize[2];
    cl_uint i;
    cl_event curEvent;
    cl_int clResult;
    kzsError result;
    grsize[0] = SIDE_LENGTH/2;
    grsize[1] = SIDE_LENGTH;
    gcsize[0] = SIDE_LENGTH;
    gcsize[1] = SIDE_LENGTH/2;

    for(i = 1; i <= (SIDE_LENGTH/2); i = i << 2)
    {
        cl_int inputint = i;
        result = cluSetKernelArguments(testData->fft2dRow ,sizeof(cl_mem), &target, sizeof(cl_mem), &tempbuffer, sizeof(cl_int), &inputint);
        kzsErrorForward(result);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fft2dRow, 2, NULL, grsize, testData->localWorkSizeFFT, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        inputint = inputint << 1;
        cluProfilerAddEvent(testData->profiler, "fft2dRow", curEvent);
        result = cluSetKernelArguments(testData->fft2dRow, sizeof(cl_mem), &tempbuffer, sizeof(cl_mem), &target, sizeof(cl_int), &inputint);
        kzsErrorForward(result);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fft2dRow, 2, NULL, grsize, testData->localWorkSizeFFT, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "fft2dRow", curEvent);
    }
    for(i = 1; i <= (SIDE_LENGTH/2); i = i << 2)
    {
        cl_int inputint = i;
        result = cluSetKernelArguments(testData->fft2dColumn, sizeof(cl_mem), &target, sizeof(cl_mem), &tempbuffer, sizeof(cl_int), &inputint);
        kzsErrorForward(result);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fft2dColumn, 2, NULL, gcsize, testData->localWorkSizeFFT, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "fft2dColumn", curEvent);
        inputint = inputint << 1;
        result = cluSetKernelArguments(testData->fft2dColumn, sizeof(cl_mem), &tempbuffer, sizeof(cl_mem), &target, sizeof(cl_int), &inputint);
        kzsErrorForward(result);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fft2dColumn, 2, NULL, gcsize, testData->localWorkSizeFFT, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "fft2dColumn", curEvent);
    }
    kzsSuccess();
}

kzsError ifft(struct WaveTestData * testData, cl_mem target, cl_mem tempbuffer)
{
    size_t grsize[2];
    size_t gcsize[2];
    cl_uint i;
    cl_event curEvent;
    cl_int clResult;
    kzsError result;
    grsize[0] = SIDE_LENGTH/2;
    grsize[1] = SIDE_LENGTH;
    gcsize[0] = SIDE_LENGTH;
    gcsize[1] = SIDE_LENGTH/2;

    for(i = 1; i <= (SIDE_LENGTH/2); i = i << 2)
    {
        cl_int inputint = i;
        result = cluSetKernelArguments(testData->ifft2dColumn, sizeof(cl_mem), &target, sizeof(cl_mem), &tempbuffer, sizeof(cl_int), &inputint);
        kzsErrorForward(result);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->ifft2dColumn, 2, NULL, gcsize, testData->localWorkSizeFFT, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "ifft2dColumn", curEvent);
        inputint = inputint << 1;
        result = cluSetKernelArguments(testData->ifft2dColumn, sizeof(cl_mem), &tempbuffer, sizeof(cl_mem), &target, sizeof(cl_int), &inputint);
        kzsErrorForward(result);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->ifft2dColumn, 2, NULL, gcsize, testData->localWorkSizeFFT, 0, NULL, &curEvent);
        cluProfilerAddEvent(testData->profiler, "ifft2dColumn", curEvent);
    }
    for(i = 1; i <= (SIDE_LENGTH/2); i = i << 2)
    {
        cl_int inputint = i;
        result = cluSetKernelArguments(testData->ifft2dRow ,sizeof(cl_mem), &target, sizeof(cl_mem), &tempbuffer, sizeof(cl_int), &inputint);
        kzsErrorForward(result);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->ifft2dRow, 2, NULL, grsize, testData->localWorkSizeFFT, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "ifft2dColumn", curEvent);
        inputint = inputint << 1;
        result = cluSetKernelArguments(testData->ifft2dRow, sizeof(cl_mem), &tempbuffer, sizeof(cl_mem), &target, sizeof(cl_int), &inputint);
        kzsErrorForward(result);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->ifft2dRow, 2, NULL, grsize, testData->localWorkSizeFFT, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "ifft2dColumn", curEvent);
    }

    kzsSuccess();
}

/* Adds a drop to a random location in the height field. */
kzsError addDrop(struct WaveTestData * testData, cl_mem target, cl_mem tempbuffer1, cl_mem tempbuffer2)
{

    cl_int x, y;
    cl_float fx, fy;
    cl_int clResult;
    size_t gsize[2];
    kzsError result;
    cl_event curEvent;
    gsize[0] = SIDE_LENGTH;
    gsize[1] = SIDE_LENGTH;

    x = kzcRandomInteger(testData->random, 0, VBO_SIDE-1);
    y = kzcRandomInteger(testData->random, 0, VBO_SIDE-1);
    fx = (cl_float) x;
    fy = (cl_float) y;
    result = cluSetKernelArguments(testData->addDrop, sizeof(cl_mem), &target, sizeof(cl_float), &fx, sizeof(cl_float), &fy);
    kzsErrorForward(result);
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->addDrop, 2, NULL, gsize, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "addDrop", curEvent);

    kzsSuccess();
}

/* Generate the initial frequency domain height field. Uses heightField1 as a temporary buffer for the fft */
kzsError generateInitialHeightfield(struct BenchmarkFramework* framework, struct WaveTestData * testData)
{
    cl_uint i;
    struct KzcMemoryManager *memoryManager;

    kzsError result;
    cl_int clResult;    

    memoryManager = bfGetMemoryManager(framework);
    result = kzcMemoryAllocPointer(memoryManager, &testData->tempbuffer, sizeof(cl_float)*GRID_SIZE*2, "tempbuffer for heightfield initalization");
    kzsErrorForward(result);
    for(i = 0; i < GRID_SIZE; i++)
    {
        testData->tempbuffer[i*2] = 0.0f;
        testData->tempbuffer[i*2 + 1] = 0.0f;
    }

    clResult = clEnqueueWriteBuffer(testData->queue, testData->frequencyDomainHeightField, 1, 0, sizeof(cl_float2)*GRID_SIZE, testData->tempbuffer, 0, NULL, NULL);
    cluClErrorTest(clResult);
    result = fft(testData, testData->frequencyDomainHeightField, testData->heightField1);
    kzsErrorForward(result);

    for( i = 0; i < 16; i++)
    {
        cl_float t = 0.0100f*30.0f;
        result = addDrop(testData, testData->frequencyDomainHeightField, testData->heightField1, testData->heightField2);
        kzsErrorForward(result);
        result = applyDispersion(testData, t);
        kzsErrorForward(result);
    }
    

    kzsSuccess();
}


kzsError applyDispersion(struct WaveTestData * testData, cl_float time)
{
    size_t dsize[2];
    cl_int clResult;
    cl_event curEvent;
    dsize[0] = SIDE_LENGTH;
    dsize[1] = SIDE_LENGTH;
    cluSetKernelArguments(testData->dispersion,
        sizeof(cl_mem), &testData->frequencyDomainHeightField, 
        sizeof(cl_mem), &testData->frequencyDomainHeightField, 
        sizeof(cl_float), &time);
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->dispersion, 2, NULL, dsize, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "dispersion", curEvent);
    kzsSuccess();
}

/* Generate the spatial domain heightfield */
kzsError generateSpatialDomainField(struct BenchmarkFramework* framework, struct BfScene* scene, struct WaveTestData * testData)
{
    size_t dsize[2];
    kzsError result;

    cl_int clResult;
    dsize[0] = SIDE_LENGTH;
    dsize[1] = SIDE_LENGTH;

    

    result = applyDispersion(testData, testData->dt);
    kzsErrorForward(result);

    result = ifft(testData, testData->frequencyDomainHeightField, testData->heightField1);

    /* Zero the values outside vbo thus giving us reflecting edges. TODO: Use global offset and not conditional expressions inside the kernel when more drivers are compliant. */
    {
        cl_int boundarySize = VBO_SIDE;
        cl_event curEvent;
        result = cluSetKernelArguments(testData->zeroBoundaries,
                    sizeof(cl_mem), &testData->frequencyDomainHeightField, 
                    sizeof(cl_int), &boundarySize);
        kzsErrorForward(result);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->zeroBoundaries, 2, NULL, dsize, testData->localWorkSize, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "zeroBoundaries", curEvent);
    }
  

    kzsSuccess();
}

void setFace(struct WaveTestData* testData, kzUint faceIndex, kzUint particleIndex)
{
    kzUint i;
    kzUint offset = particleIndex * 8;

    kzsAssert(kzcIsValidPointer(testData));

    for(i = 0; i < 8; ++i)
    {
        if(testData->particleFaceIndexes[i + offset] == -1)
        {
            testData->particleFaceIndexes[i + offset] = faceIndex;
            return;
        }
    }

    kzsAssert(KZ_FALSE);
}

kzsError waveUpdateCamera(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct WaveTestData *testData;
    kzsError result;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    {
        const kzUint framesPerCut = WAVE_FRAME_COUNT / 2;
        kzUint frame = bfSceneGetFrameInitialCounterValue(scene) - bfSceneGetFrameCounterValue(scene);
        kzFloat time = (frame % framesPerCut) / (kzFloat)framesPerCut;
        kzUint camera = (frame / framesPerCut) % 2;
        struct KzuAnimationPlayer* player = kzuSceneGetAnimationPlayer(testData->scene);
        kzuAnimationPlayerSetPaused(player, KZ_TRUE);
        kzuAnimationPlayerSetTime(player, time * 6.0f);
        result = kzuAnimationPlayerUpdate(player, 0.0f);
        kzsErrorForward(result);
        kzuSceneSetViewCamera(testData->scene, testData->cameras[camera]);
    }

    kzsSuccess();
}

kzsError waveSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct WaveTestData *testData = NULL;
    kzsError result;
    cl_int clResult;
    struct KzcMemoryManager *memoryManager;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    memoryManager = bfGetMemoryManager(framework);
    testData->context = bfGetClContext(framework);

    result = bfUtilReadLocalWorkgroupSize(framework, "WaveSimulationLocalWorkSize", 2, &testData->localWorkSize);
    kzsErrorForward(result);
    result = bfUtilReadLocalWorkgroupSize(framework, "WaveSimulationLocalWorkSizeFFT", 2, &testData->localWorkSizeFFT);
    kzsErrorForward(result);

    {
        kzBool fullProfile;
        result = bfGetCLFullProfile(framework, &fullProfile);
        kzsErrorForward(result);

        if(fullProfile)
        {
            testData->dt = 0.03f;
            testData->dropRate = 6;
            SIDE_LOG2 = 10;            
            VBO_SIDE = 1000;
        }
        else
        {
            testData->dt = 0.01f;
            testData->dropRate = 32;
            SIDE_LOG2 = 8;
            VBO_SIDE = 250;
        }
    }
    

    clResult = clGetContextInfo(testData->context,CL_CONTEXT_DEVICES,sizeof(cl_device_id),&testData->device,NULL);
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

        testData->queue = clCreateCommandQueue(testData->context, testData->device, cqProperties, &clResult);
        cluClErrorTest(clResult);
    }


    testData->profiler = bfSceneGetProfiler(scene);

    bfSceneSetFrameCounter(scene, WAVE_FRAME_COUNT);

    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/wave");
    kzsErrorForward(result);
    result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), "Materials/wave_material/wave", &testData->material);
    kzsErrorForward(result);
    testData->scene = kzaApplicationGetScene(bfGetApplication(framework));
    result = kzuProjectLoaderLoadCameraNode(bfGetProject(framework), "Scenes/wave/Root Node/wave_camera_1", &testData->cameras[0]);
    kzsErrorForward(result);
    result = kzuProjectLoaderLoadCameraNode(bfGetProject(framework), "Scenes/wave/Root Node/wave_camera_2", &testData->cameras[1]);
    kzsErrorForward(result);

    result = waveUpdateCamera(framework, scene);
    kzsErrorForward(result);

    {
        kzBool isWaveValid;
        kzBool isFftValid;
        kzBool binaryA, binaryB;
        {
            kzString compilerSettings = COMPILER_FLAGS;

            result = bfValidateFile(framework, "data/wave_simulation.cl", WAVE_SIMULATION_HASH, &isWaveValid);
            kzsErrorForward(result);

            result = cluGetBuiltProgramFromFileWithOptions(bfGetMemoryManager(framework), "data/wave_simulation.cl", KZ_FALSE, compilerSettings, testData->context, &testData->waveProgram, &binaryA);
            kzsErrorForward(result);

            result = bfValidateFile(framework, "data/fft.cl", FFT_HASH, &isFftValid);
            kzsErrorForward(result);
            result = cluGetBuiltProgramFromFileWithOptions(bfGetMemoryManager(framework), "data/fft.cl", KZ_FALSE, compilerSettings, testData->context, &testData->fftProgram, &binaryB);
            kzsErrorForward(result);
        }
        bfSceneSetValidConfigurationData(scene, isFftValid & isWaveValid);
        bfSceneSetUseProgramBinary(scene, binaryA | binaryB);
    }


    testData->dispersion = clCreateKernel(testData->waveProgram, "deepWaterDispersion", &clResult);
    cluClErrorTest(clResult);
    testData->addFields = clCreateKernel(testData->waveProgram, "addFields", &clResult);
    cluClErrorTest(clResult);
    testData->addDrop = clCreateKernel(testData->waveProgram, "addDrop", &clResult);
    cluClErrorTest(clResult);
    testData->zeroBoundaries = clCreateKernel(testData->waveProgram, "zeroBoundaries", &clResult);
    cluClErrorTest(clResult);
    testData->updateVbo = clCreateKernel(testData->waveProgram, "generateHeightFieldVbo", &clResult);
    cluClErrorTest(clResult);

    testData->calculateNormals[0] = clCreateKernel(testData->waveProgram, "resetFaceNormals", &clResult);
    cluClErrorTest(clResult);
    testData->calculateNormals[1] = clCreateKernel(testData->waveProgram, "calculateFaceNormals", &clResult);
    cluClErrorTest(clResult);
    testData->calculateNormals[2] = clCreateKernel(testData->waveProgram, "averageFaceNormals", &clResult);
    cluClErrorTest(clResult);

    /* Fourier transform kernels */
    testData->fft2dRow = clCreateKernel(testData->fftProgram, "radix2fft2dRow", &clResult);
    cluClErrorTest(clResult);
    testData->fft2dColumn = clCreateKernel(testData->fftProgram, "radix2fft2dColumn", &clResult);
    cluClErrorTest(clResult);
    testData->ifft2dRow = clCreateKernel(testData->fftProgram, "radix2ifft2dRow", &clResult);
    cluClErrorTest(clResult);
    testData->ifft2dColumn = clCreateKernel(testData->fftProgram, "radix2ifft2dColumn", &clResult);
    cluClErrorTest(clResult);

    /* Create the required buffers */
    testData->frequencyDomainHeightField = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_float2)*GRID_SIZE ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->heightField1 = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_float2)*GRID_SIZE ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->heightField2 = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_float2)*GRID_SIZE ,NULL, &clResult);
    cluClErrorTest(clResult);

    result = kzcRandomCreate(memoryManager, 34123, &testData->random);
    kzsErrorForward(result);

    result = generateInitialHeightfield(framework, testData);
    kzsErrorForward(result);

    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    testData->particleFaceBufferSize = INDEX_COUNT * 8 * sizeof(kzInt);
    result = kzcMemoryAllocPointer(memoryManager, &testData->particleFaceIndexes, testData->particleFaceBufferSize, "Face index buffer data");
    kzsErrorForward(result);
    kzsMemset(testData->particleFaceIndexes, -1, testData->particleFaceBufferSize);

    result = kzcMemoryAllocPointer(memoryManager, &testData->faceNormal, INDEX_COUNT / 3 * sizeof(struct KzcVector4), "Face Normal Buffer data");
    kzsErrorForward(result);

    /* Create index buffer for rendering water. */
    {
        cl_int errorCode;
        kzUint dataSize;
        dataSize = INDEX_COUNT * sizeof(kzUint);

        {
            result = kzcMemoryAllocPointer(memoryManager, &testData->indices, dataSize, "Indices");
            kzsErrorForward(result);

            /* Build faces. */
            {
                kzUint offset = 0;
                kzUint startingIndex = 0;                
                kzUint i;
                for(i = 0; i < INDEX_COUNT; i += 6)
                {
                    testData->indices[i    ] = startingIndex;
                    testData->indices[i + 1] = startingIndex + 1;
                    testData->indices[i + 2] = startingIndex + VBO_SIDE;
                    testData->indices[i + 3] = startingIndex + VBO_SIDE;
                    testData->indices[i + 4] = startingIndex + 1;
                    testData->indices[i + 5] = startingIndex + VBO_SIDE + 1;

                    setFace(testData, (i+0)/3, startingIndex);
                    setFace(testData, (i+1)/3, startingIndex + 1);
                    setFace(testData, (i+2)/3, startingIndex + VBO_SIDE);
                    setFace(testData, (i+3)/3, startingIndex + VBO_SIDE);
                    setFace(testData, (i+4)/3, startingIndex + 1);
                    setFace(testData, (i+5)/3, startingIndex + VBO_SIDE + 1);

                    offset++;
                    if(offset >= VBO_SIDE - 1)
                    {
                        startingIndex += 2;
                        offset = 0;
                    }
                    else
                    {
                        startingIndex++;
                    }
                }
            }

            glGenBuffers(1, &testData->vboIndexBufferHandle);
            glBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, testData->vboIndexBufferHandle);
            glBufferData(KZS_GL_ELEMENT_ARRAY_BUFFER, dataSize, testData->indices, KZS_GL_STATIC_DRAW);
            glBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, 0);
            testData->vboIndexBufferCl = clCreateBuffer(bfGetClContext(framework), (cl_mem_flags)CL_MEM_COPY_HOST_PTR, dataSize, 
                testData->indices, &errorCode);
            cluClErrorTest(errorCode);
        }
    }

    /* Array buffer. */
    {
        kzUint vboSize = VBO_SIZE * sizeof(float) * VERTEX_DATA_SIZE; 
        glGenBuffers(1, &testData->vertexBufferObjectHandle);
        glBindBuffer(KZS_GL_ARRAY_BUFFER, testData->vertexBufferObjectHandle);
        glBufferData(KZS_GL_ARRAY_BUFFER, vboSize, 0, KZS_GL_DYNAMIC_DRAW);
        testData->vertexBufferObjectCLBuffer = clCreateFromGLBuffer(bfGetClContext(framework), CL_MEM_WRITE_ONLY, testData->vertexBufferObjectHandle, NULL);
    }

    /* Create mesh for water surface. */
    {
        struct KzuObjectNode* node;
        result = dynamicMeshNodeCreate(memoryManager, kzuProjectGetPropertyManager(bfGetProject(framework)), kzuProjectGetMessageDispatcher(bfGetProject(framework)), 
            &testData->dynamicMesh);
        kzsErrorForward(result);
        dynamicMeshSetCallback(testData->dynamicMesh, renderWaterSurfaceCallback, testData);
        result = kzuProjectLoaderLoadObjectNode(bfGetProject(framework), "Scenes/wave/Root Node/WaterSurface", &node);
        kzsErrorForward(result);
        result = kzuObjectNodeRemoveAllChildren(node); /* Removes helper nodes which are used to fine tune the visuals. */
        kzsErrorForward(result);
        result = kzuObjectNodeAddChild(node, (struct KzuObjectNode*)testData->dynamicMesh);
        kzsErrorForward(result);
    }

    testData->faceNormalBuffer = clCreateBuffer(testData->context, (cl_mem_flags)CL_MEM_COPY_HOST_PTR, INDEX_COUNT / 3 * sizeof(struct KzcVector4), 
        testData->faceNormal, &clResult);
    cluClErrorTest(clResult);
    testData->particleFaceIndexBuffer = clCreateBuffer(testData->context, (cl_mem_flags)CL_MEM_COPY_HOST_PTR, testData->particleFaceBufferSize,
        testData->particleFaceIndexes, &clResult);
    cluClErrorTest(clResult);
#endif

    kzsSuccess();
}

kzsError waveSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct WaveTestData *testData;
    kzsError result;
    cl_int clResult;
    size_t vsize[2];
    cl_int instride = SIDE_LENGTH;
    kzUint tick;
    cl_event curEvent;
    vsize[0] = VBO_SIDE;
    vsize[1] = VBO_SIDE;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE

    
    clResult = clEnqueueAcquireGLObjects(testData->queue, 1, &testData->vertexBufferObjectCLBuffer, 0, 0, 0);
    cluClErrorTest(clResult);
#endif

    tick = bfSceneGetFrameCounterValue(scene);
    if(tick % testData->dropRate == 0)
    {
        result = addDrop(testData, testData->frequencyDomainHeightField, testData->heightField1, testData->heightField2);
        kzsErrorForward(result);
    }

    result = generateSpatialDomainField(framework, scene, testData);
    kzsErrorForward(result);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    result = cluSetKernelArguments(testData->updateVbo, 
        sizeof(cl_mem), &testData->frequencyDomainHeightField, 
        sizeof(cl_mem), &testData->vertexBufferObjectCLBuffer,
        sizeof(cl_int), &instride);
    kzsErrorForward(result);
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->updateVbo, 2, NULL, vsize, testData->localWorkSize, 0, NULL, &curEvent );
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "updateVbo", curEvent);
#endif

    result = fft(testData, testData->frequencyDomainHeightField, testData->heightField1);
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    /* Normal calculation. */
    {
        {  
            size_t sizeFaces[2];
            size_t sizeVBO[2];
            sizeFaces[0] = INDEX_COUNT/3;
            sizeFaces[1] = 1;
            sizeVBO[0] = VBO_SIZE;
            sizeVBO[1] = 1;


            /* Reset face normals. */
            result = cluSetKernelArguments(testData->calculateNormals[0], 
                sizeof(cl_mem), &testData->faceNormalBuffer);
            kzsErrorForward(result);
            clResult = clEnqueueNDRangeKernel(testData->queue, testData->calculateNormals[0], 2, NULL, sizeFaces, NULL, 0, 0, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "calculateNormals", curEvent);
            /* Calculate face normals. */
            result = cluSetKernelArguments(testData->calculateNormals[1], 
                sizeof(cl_mem), &testData->vboIndexBufferCl,
                sizeof(cl_mem), &testData->vertexBufferObjectCLBuffer,
                sizeof(cl_mem), &testData->faceNormalBuffer);
            kzsErrorForward(result);            
            clResult = clEnqueueNDRangeKernel(testData->queue, testData->calculateNormals[1], 2, NULL, sizeFaces, NULL, 0, 0, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "calculateNormals", curEvent);
            /* Average face normals to vertex's. */
            result = cluSetKernelArguments(testData->calculateNormals[2],
                sizeof(cl_mem), &testData->vertexBufferObjectCLBuffer,
                sizeof(cl_mem), &testData->faceNormalBuffer,
                sizeof(cl_mem), &testData->particleFaceIndexBuffer);
            kzsErrorForward(result);
            clResult = clEnqueueNDRangeKernel(testData->queue, testData->calculateNormals[2], 2, NULL, sizeVBO, NULL, 0, 0, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "calculateNormals", curEvent);
            clResult = clEnqueueReleaseGLObjects(testData->queue, 1, &testData->vertexBufferObjectCLBuffer, 0, KZ_NULL, KZ_NULL);
            cluClErrorTest(clResult);

        }
    }
#endif
    
    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

    kzsSuccess();
}

kzsError waveSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct WaveTestData *testData = NULL;
    kzsError result;
    cl_int clResult;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    result = bfUtilFreeLocalWorkgroupSize(testData->localWorkSize);
    kzsErrorForward(result);
    result = bfUtilFreeLocalWorkgroupSize(testData->localWorkSizeFFT);
    kzsErrorForward(result);
    
    kzsErrorForward(result);
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    /*result = kzuSceneRemoveObject(kzaApplicationGetScene(bfGetApplication(framework)), (struct KzuObjectNode*)testData->dynamicMesh);
    kzsErrorForward(result);*/
    result = dynamicMeshNodeDelete(testData->dynamicMesh);
    kzsErrorForward(result);

    result = kzcMemoryFreePointer(testData->particleFaceIndexes);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(testData->faceNormal);
    kzsErrorForward(result);
#endif

    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

    clResult = clReleaseKernel(testData->fft2dColumn);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->fft2dRow);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->ifft2dColumn);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->ifft2dRow);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->updateVbo);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->dispersion);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->addFields);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->addDrop);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->calculateNormals[0]);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->calculateNormals[1]);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->calculateNormals[2]);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->zeroBoundaries);
    cluClErrorTest(clResult);
    clResult = clReleaseProgram(testData->fftProgram);
    cluClErrorTest(clResult);
    clResult = clReleaseProgram(testData->waveProgram);
    cluClErrorTest(clResult);

    clResult = clReleaseMemObject(testData->frequencyDomainHeightField);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->heightField1);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->heightField2);
    cluClErrorTest(clResult);
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    clResult = clReleaseMemObject(testData->faceNormalBuffer);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->particleFaceIndexBuffer);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->vertexBufferObjectCLBuffer);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->vboIndexBufferCl);
    cluClErrorTest(clResult);
    
    glDeleteBuffers(1, &testData->vertexBufferObjectHandle);
    glDeleteBuffers(1, &testData->vboIndexBufferHandle);
    result = kzcMemoryFreeVariable(testData->indices);
    kzsErrorForward(result);
#endif
    clResult = clReleaseCommandQueue(testData->queue);
    cluClErrorTest(clResult);

    result = kzcMemoryFreePointer(testData->tempbuffer);
    kzsErrorForward(result);
    result = kzcRandomDelete(testData->random);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError waveCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct WaveTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, waveSceneLoad, waveSceneUpdate, waveSceneRender,
        waveSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager, testData,"FFT wave simulation test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Wave Simulation Test", "Physics", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;
    kzsSuccess();
}

kzsError waveSceneRender(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct WaveTestData *testData;
    kzsError result;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    result = waveUpdateCamera(framework, scene);
    kzsErrorForward(result);

    kzsSuccess();

}
