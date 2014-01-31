/**
* \file
* OpenCL smoothed particle hydrodynamics test.
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "cl_sph.h"

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
#include <user/properties/kzu_fixed_properties.h>
#include <user/properties/kzu_property_manager_void.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/properties/kzu_texture_property.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_transformed_scene.h>
#include <user/renderer/kzu_renderer.h>
#include <user/animation/kzu_animation_player.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/resource_manager/shader/kzc_resource_shader.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/texture/kzc_texture_descriptor.h>
#include <core/renderer/kzc_renderer.h>
#include <core/util/image/kzc_image.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/debug/kzc_log.h>

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

#include <math.h>

#include "dynamic_mesh.h"


struct SphTestData;


kzsError sphSceneRender(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError sphSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError sphSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError sphSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError sphSortParticles(struct SphTestData *testData);
kzsError sphResetScene(struct SphTestData *testData);


kzInt PARTICLE_LOG2;


kzInt VOXEL_COUNT_X;
kzInt VOXEL_COUNT_Y;
kzInt VOXEL_COUNT_Z;

#define PARTICLE_COUNT (1 << PARTICLE_LOG2)
#define GRID_SIZE 512

/** Size of vertex data. */
#define VERTEX_DATA_SIZE 8
/** Amount of vertices. */
#define VERTEX_COUNT (PARTICLE_COUNT * 4)
/** Index count. */
#define INDEX_COUNT (PARTICLE_COUNT * 2 * 3)


struct SphTestData
{
    cl_context context; /**< OpenCL context. */

    cl_command_queue queue; /**< OpenCL command queue. */
    cl_device_id device; /**< OpenCL device. */

    cl_uint voxelCount[4]; /**< Voxel count. */
    cl_float m; /**< Mass of a single particle */
    cl_float h; /**< Radius of a particle and width of an individual voxel */
    cl_float u; /**< Viscosity coefficient (mu) */
    cl_float dt; /**< Time step of the simulator */

    cl_mem billboardBuffer; /**< Billboard offsets. */

    cl_mem keys; /**< Array of uint2 with first component is the vector id and second is the particle id */
    cl_mem newkeys; /**< Temporary buffer used with the O(n) voxel updating system */
 
    cl_mem sortedPosition; /**< Sorted positions. */

    kzUint vertexBufferObjectHandle; /**< Vertex Buffer Object OpenGL handle. */
    cl_mem vertexBufferObjectCLBuffer; /**< Cl buffer for VBO. */

    kzUint* indices; /**< Triangle indices for rendering. */
    kzUint vboIndexBufferHandle; /** Index Buffer Object OpenGL handle. */
    cl_mem vboIndexBufferCl; /**< Cl buffer for IBO. */

    cl_mem sortedVelocity; /**< Velocities of particles sorted by voxel they reside in */
    cl_mem sortedDensity; /**< Density of particles sorted by voxel they reside in */
    cl_mem position; /**< Position of the particles */
    cl_mem velocity; /**< Velocities of the particles */
    cl_mem acceleration; /**< Instantaneous acceleration */
    cl_mem voxelParticles; /**< List of voxel id:s where each particle currently resides */
    cl_mem particleCount;  /**< The amount of particles residing on the voxel */

    cl_program sphProgram;

    cl_kernel bitonicSort; /**< Kernel for one step within bitonic sorting network. */
    cl_kernel updateKeys; 
    cl_kernel sortPostProcess;
    cl_kernel clearVoxels;
    cl_kernel setVoxels;
    cl_kernel calculateDensity;
    cl_kernel calculateAcceleration;
    cl_kernel clearVelocityAcceleration;
    cl_kernel integrate;
    cl_kernel cumSumVoxels;
    cl_kernel renderSph;
    cl_kernel updateVBO;

    cl_float *initialPositions; /* Initial positions for uploading the data to the opencl device. */

    struct KzuMaterial* material; /**< Material used to render particles. */
    struct DynamicMeshNode* dynamicMesh; /**< Kanzi scene graph object for particles. */

    struct KzuCameraNode* cameraNode; /**< Camera node used to render 3d content. */

    struct KzuScene* scene; /**< Scene for rendering cloth. */
    struct KzuCameraNode* cameras[3]; /**< Camera nodes used to render scene from different angles. */

    struct CluProfiler *profiler; /**< Clu profiler */

    size_t* localWorkSize; /**< Local work size. Read from configuration file. */

    cl_float offset; /* Parameters to control display of the fluid */
    cl_float scale;
    cl_float yoffset;
};

/* Cubic root does not exist on certain libc implementations */
double cubicRoot(double a)
{
    double x = 1.0;
    int i;
    for(i = 0;i < 100; i++)
    {
        x = (4.0/3.0)*sqrt(sqrt(a*x)) - x/3.0;
    }
    return x;
}

kzsError renderSphCallback(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode, void* userData)
{

    kzsError result;
    struct SphTestData* testData = userData;
    struct KzcMatrix4x4 transformation = kzuTransformedObjectNodeGetMatrix(transformedObjectNode);

    struct KzcRenderer* coreRenderer;
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    kzcRendererSetCullMode(kzuRendererGetCoreRenderer(renderer), KZC_RENDERER_CULL_MODE_NONE);

    kzsAssert(kzcIsValidPointer(renderer));
    coreRenderer = kzuRendererGetCoreRenderer(renderer);
    kzcRendererSetMatrix(coreRenderer, KZC_RENDERER_MATRIX_WORLD, &transformation);

    result = kzuRendererApplyMaterial(renderer, testData->material);
    kzsErrorForward(result);

    #define BUFFER_OFFSET(i) ((char *)NULL + (i) * sizeof(float))
    {
        kzInt* positionIndex;
        kzInt* uvIndex;
        struct KzuMaterialType* materialType = kzuMaterialGetMaterialType(testData->material);
        struct KzcShader* shader = kzuMaterialTypeGetShaderProgram(materialType);
        struct KzcHashMap* hashMap = kzcShaderGetAttributes(shader);
        kzBool found;
        found = kzcHashMapGet(hashMap, "kzPosition", (void**)&positionIndex);
        kzsAssertText(found, "Material for rendering must have position attribute.");
        found = kzcHashMapGet(hashMap, "kzTextureCoordinate0", (void**)&uvIndex);
        kzsAssertText(found, "Material for rendering must have normal attribute.");

        kzsGlBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, testData->vboIndexBufferHandle);
        kzsGlBindBuffer(KZS_GL_ARRAY_BUFFER, testData->vertexBufferObjectHandle);

        kzsGlVertexAttribPointer(*positionIndex, 3, KZS_GL_FLOAT, KZ_FALSE, VERTEX_DATA_SIZE * sizeof(float), 0);
        kzsGlEnableVertexAttribArray(*positionIndex);
        kzsGlVertexAttribPointer(*uvIndex, 3, KZS_GL_FLOAT, KZ_FALSE, VERTEX_DATA_SIZE * sizeof(float), BUFFER_OFFSET(4));
        kzsGlEnableVertexAttribArray(*uvIndex);

        kzsGlDrawElements(KZS_GL_TRIANGLES, INDEX_COUNT, KZS_GL_UNSIGNED_INT, 0);

        kzsGlDisableVertexAttribArray(*positionIndex);
        kzsGlDisableVertexAttribArray(*uvIndex);

        kzsGlBindBuffer(KZS_GL_ARRAY_BUFFER, 0);
        kzsGlBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, 0);
    }
#endif
    kzsSuccess();
}

kzsError sphUpdateCamera(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct SphTestData *testData;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    {
        const kzUint framesPerCut = 200;
        kzUint frame = bfSceneGetFrameInitialCounterValue(scene) - bfSceneGetFrameCounterValue(scene);
        kzFloat time = (frame % framesPerCut) / (kzFloat)framesPerCut;
        kzUint cameraPath = (4 + (frame / framesPerCut) * 11) % 6;
        kzUint camera = cameraPath % 3;
        struct KzuAnimationPlayer* player = kzuSceneGetAnimationPlayer(testData->scene);
        kzuAnimationPlayerSetPaused(player, KZ_TRUE);
        kzuAnimationPlayerSetTime(player, (cameraPath > 3 ? 9.0f : 0.0f) + time * 9.0f);
        kzuAnimationPlayerUpdate(player, 0.0f);
        kzuSceneSetViewCamera(testData->scene, testData->cameras[camera]);
        testData->cameraNode = testData->cameras[camera];
    }

    kzsSuccess();
}

kzsError sphSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct SphTestData *testData = NULL;
    kzsError result;
    cl_int clResult;
    struct KzcMemoryManager *memoryManager;
    size_t pcount = PARTICLE_COUNT;
    cl_uint vcount;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    memoryManager = bfGetMemoryManager(framework);
    testData->context = bfGetClContext(framework);


    clResult = clGetContextInfo(testData->context ,CL_CONTEXT_DEVICES, sizeof(cl_device_id), &testData->device, NULL);
    cluClErrorTest(clResult);

    {
        kzBool fullProfile;
        result = bfGetCLFullProfile(framework, &fullProfile);
        kzsErrorForward(result);

        if(fullProfile)
        {
            const float scalefac = 0.6666666f;
            testData->yoffset = -0.05f/scalefac;
            testData->offset = 0.625f/scalefac;
            testData->scale = 5.15f*scalefac;
            PARTICLE_LOG2 = 16;
            VOXEL_COUNT_X = 48;
            VOXEL_COUNT_Y = 48;
            VOXEL_COUNT_Z = 48;
        }
        else
        {
            testData->yoffset = -0.05f;
            testData->offset = 0.625f;
            testData->scale = 5.15f;

            PARTICLE_LOG2 = 14;
            VOXEL_COUNT_X = 32;
            VOXEL_COUNT_Y = 32;
            VOXEL_COUNT_Z = 32;
        }
    }


    testData->profiler = bfSceneGetProfiler(scene);
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


    bfSceneSetFrameCounter(scene, SPH_FRAME_COUNT);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
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
                kzUint startingIndex = 0;                
                kzUint i;
                for(i = 0; i < (kzUint)INDEX_COUNT; i += 6)
                {
                    testData->indices[i    ] = startingIndex;
                    testData->indices[i + 1] = startingIndex + 1;
                    testData->indices[i + 2] = startingIndex + 2;
                    testData->indices[i + 3] = startingIndex + 1;
                    testData->indices[i + 4] = startingIndex + 3;
                    testData->indices[i + 5] = startingIndex + 2;
                    startingIndex += 4;
                }
            }

            glGenBuffers(1, &testData->vboIndexBufferHandle);
            glBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, testData->vboIndexBufferHandle);
            glBufferData(KZS_GL_ELEMENT_ARRAY_BUFFER, dataSize, testData->indices, KZS_GL_STATIC_DRAW);
            glBindBuffer(KZS_GL_ELEMENT_ARRAY_BUFFER, 0);
            testData->vboIndexBufferCl = clCreateBuffer(bfGetClContext(framework), (cl_mem_flags)CL_MEM_COPY_HOST_PTR, dataSize, 
                testData->indices, &errorCode);
            cluClErrorTest(errorCode);
            result = kzcMemoryFreePointer(testData->indices);
            kzsErrorForward(result);
        }
    }
#endif
    result = bfUtilReadLocalWorkgroupSize(framework, "SPHLocalWorkSize", 1, &testData->localWorkSize);
    kzsErrorForward(result);
    {
        kzString compilerSettings = COMPILER_FLAGS;
        kzBool binary;
        kzBool isValid;
        result = bfValidateFile(framework, "data/sph.cl", SPH_HASH, &isValid);
        kzsErrorForward(result);
        bfSceneSetValidConfigurationData(scene, isValid);
        result = cluGetBuiltProgramFromFileWithOptions(bfGetMemoryManager(framework), "data/sph.cl", KZ_FALSE, compilerSettings, testData->context, &testData->sphProgram, &binary);
        kzsErrorForward(result);
        bfSceneSetUseProgramBinary(scene, binary);
    }


    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/sph");
    kzsErrorForward(result);
    testData->scene = kzaApplicationGetScene(bfGetApplication(framework));
    result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), "Materials/sph_texture/sph_texture_material", &testData->material);
    kzsErrorForward(result);
    result = kzuProjectLoaderLoadCameraNode(bfGetProject(framework), "Scenes/sph/Root Node/sph_3d_camera", &testData->cameras[0]);
    kzsErrorForward(result);
    result = kzuProjectLoaderLoadCameraNode(bfGetProject(framework), "Scenes/sph/Root Node/sph_3d_camera2", &testData->cameras[1]);
    kzsErrorForward(result);
    result = kzuProjectLoaderLoadCameraNode(bfGetProject(framework), "Scenes/sph/Root Node/sph_3d_camera3", &testData->cameras[2]);
    kzsErrorForward(result);

    result = sphUpdateCamera(framework, scene);
    kzsErrorForward(result);

    {
        struct KzuObjectNode* node;
        result = dynamicMeshNodeCreate(memoryManager, kzuProjectGetPropertyManager(bfGetProject(framework)), kzuProjectGetMessageDispatcher(bfGetProject(framework)), 
            &testData->dynamicMesh);
        kzsErrorForward(result);
        dynamicMeshSetCallback(testData->dynamicMesh, renderSphCallback, testData);
        result = kzuProjectLoaderLoadObjectNode(bfGetProject(framework), "Scenes/sph/Root Node/SPH_3D", &node);
        kzsErrorForward(result);
        result = kzuObjectNodeAddChild(node, (struct KzuObjectNode*)testData->dynamicMesh);
        kzsErrorForward(result);
    }

    /* Create the kernels */
    testData->bitonicSort = clCreateKernel(testData->sphProgram, "bitonicSort", &clResult);
    cluClErrorTest(clResult);
    testData->updateKeys = clCreateKernel(testData->sphProgram, "updateKeys", &clResult);
    cluClErrorTest(clResult);
    testData->sortPostProcess = clCreateKernel(testData->sphProgram, "sortPostProcess", &clResult);
    cluClErrorTest(clResult);
    testData->clearVoxels = clCreateKernel(testData->sphProgram, "clearVoxels", &clResult);
    cluClErrorTest(clResult);
    testData->setVoxels = clCreateKernel(testData->sphProgram, "setVoxels", &clResult);
    cluClErrorTest(clResult);
    testData->calculateDensity = clCreateKernel(testData->sphProgram, "calculateDensity", &clResult);
    cluClErrorTest(clResult);
    testData->calculateAcceleration = clCreateKernel(testData->sphProgram, "calculateAcceleration", &clResult);
    cluClErrorTest(clResult);
    testData->clearVelocityAcceleration = clCreateKernel(testData->sphProgram, "clearVelocityAcceleration", &clResult);
    cluClErrorTest(clResult);
    testData->integrate = clCreateKernel(testData->sphProgram, "integrate", &clResult);
    cluClErrorTest(clResult);
    testData->cumSumVoxels = clCreateKernel(testData->sphProgram, "cumSumVoxels", &clResult);
    cluClErrorTest(clResult);
    testData->updateVBO = clCreateKernel(testData->sphProgram, "updateVBO", &clResult);
    cluClErrorTest(clResult);

    /* Create the required buffers */
    testData->keys = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_uint2)*PARTICLE_COUNT ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->newkeys = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_uint2)*PARTICLE_COUNT ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->sortedPosition = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_float4)*PARTICLE_COUNT ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->sortedVelocity = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_float4)*PARTICLE_COUNT ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->sortedDensity = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_float)*PARTICLE_COUNT ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->position = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_float4)*PARTICLE_COUNT ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->velocity = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_float4)*PARTICLE_COUNT ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->acceleration = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_float4)*PARTICLE_COUNT ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->voxelParticles = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_int)*VOXEL_COUNT_X*VOXEL_COUNT_Y*VOXEL_COUNT_Z ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->particleCount = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_int)*VOXEL_COUNT_X*VOXEL_COUNT_Y*VOXEL_COUNT_Z ,NULL, &clResult);
    cluClErrorTest(clResult);
    testData->billboardBuffer = clCreateBuffer(testData->context, CL_MEM_READ_WRITE, sizeof(cl_float)*4*4, NULL, &clResult);
    cluClErrorTest(clResult);


    testData->voxelCount[0] = VOXEL_COUNT_X;
    testData->voxelCount[1] = VOXEL_COUNT_Y;
    testData->voxelCount[2] = VOXEL_COUNT_Z;
    testData->voxelCount[3] = 0;
    testData->m = 0.015f;
    testData->h = 0.04f;
    testData->u = 10.0f;
    testData->dt = 0.005f;
    vcount = testData->voxelCount[0] * testData->voxelCount[1] * testData->voxelCount[2];
    result = kzcMemoryAllocArray(memoryManager, testData->initialPositions, PARTICLE_COUNT*4, "debug rendering");
    kzsErrorForward(result);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    {
        kzUint vboSize = VERTEX_COUNT * sizeof(float) * VERTEX_DATA_SIZE;
        glGenBuffers(1, &testData->vertexBufferObjectHandle);
        glBindBuffer(KZS_GL_ARRAY_BUFFER, testData->vertexBufferObjectHandle);
        glBufferData(KZS_GL_ARRAY_BUFFER, vboSize, 0, KZS_GL_DYNAMIC_DRAW);
        testData->vertexBufferObjectCLBuffer = clCreateFromGLBuffer(bfGetClContext(framework), CL_MEM_WRITE_ONLY, testData->vertexBufferObjectHandle, NULL);
    }
#endif
    

    /* Set the kernel arguments for kernels which have constant arguments */
    
    result = cluSetKernelArguments(testData->updateKeys, sizeof(cl_mem), &testData->keys, sizeof(cl_mem), &testData->position, sizeof(cl_uint4), &testData->voxelCount, sizeof(cl_float), &testData->h);
    kzsErrorForward(result);
    result = cluSetKernelArguments(testData->sortPostProcess, sizeof(cl_mem), &testData->keys, sizeof(cl_mem), &testData->position, sizeof(cl_mem), &testData->velocity, sizeof(cl_mem), &testData->sortedPosition, sizeof(cl_mem), &testData->sortedVelocity);
    kzsErrorForward(result);
    result = cluSetKernelArguments(testData->clearVoxels, sizeof(cl_mem), &testData->voxelParticles);
    kzsErrorForward(result);
    result = cluSetKernelArguments(testData->setVoxels, sizeof(cl_mem), &testData->voxelParticles, sizeof(cl_mem), &testData->keys);
    kzsErrorForward(result);

    result = cluSetKernelArguments(testData->calculateDensity, sizeof(cl_mem), &testData->keys, sizeof(cl_mem), &testData->sortedPosition, sizeof(cl_mem), &testData->voxelParticles, sizeof(cl_mem), &testData->sortedDensity,  sizeof(cl_uint4), &testData->voxelCount,  sizeof(cl_float), &testData->h, sizeof(cl_float), &testData->m);
    kzsErrorForward(result);
    result = cluSetKernelArguments(testData->calculateAcceleration, sizeof(cl_mem), &testData->keys, sizeof(cl_mem), &testData->sortedPosition, sizeof(cl_mem), &testData->voxelParticles,
                                   sizeof(cl_mem), &testData->sortedDensity, sizeof(cl_mem), &testData->sortedVelocity, sizeof(cl_mem), &testData->acceleration,  sizeof(cl_uint4), &testData->voxelCount,
                                   sizeof(cl_float), &testData->h, sizeof(cl_float), &testData->m, sizeof(cl_float), &testData->u);
    kzsErrorForward(result);
    
    result = cluSetKernelArguments(testData->integrate, sizeof(cl_mem), &testData->position, sizeof(cl_mem), &testData->velocity, sizeof(cl_mem), &testData->acceleration,
                                   sizeof(cl_uint4), &testData->voxelCount, sizeof(cl_float), &testData->h, sizeof(cl_float), &testData->dt);
    kzsErrorForward(result);
    result = cluSetKernelArguments(testData->clearVelocityAcceleration, sizeof(cl_mem), &testData->velocity, sizeof(cl_mem), &testData->acceleration);
    kzsErrorForward(result);
     
    result = cluSetKernelArguments(testData->cumSumVoxels, sizeof(cl_mem), &testData->particleCount, sizeof(cl_int), &vcount);
    kzsErrorForward(result);
    result = sphResetScene(testData);
    kzsErrorForward(result);



    kzsSuccess();
}


kzsError sphResetScene(struct SphTestData *testData)
{
    cl_int clResult;
    size_t pcount = PARTICLE_COUNT;
    kzsError result;
    size_t vcount = testData->voxelCount[0] * testData->voxelCount[1] * testData->voxelCount[2];
    cl_event curEvent;
    /* Here we set the initial positions of the particles and upload them to the OpenCL device. */
    {       

        cl_uint i;
        const cl_float h = testData->h;
        const int side = (int)ceil(cubicRoot((double)PARTICLE_COUNT));

        for(i = 0; i < (kzUint)PARTICLE_COUNT; i++)
        {
            testData->initialPositions[(i)*4] = h/2.0f * (cl_float)(i%(side)) + 0.5f*h;
            testData->initialPositions[(i)*4 + 1] = h/2.0f * (cl_float)((i/side)%side) + 0.25f*h + (i % 1000) * 0.001f * h * 20.0f;
            testData->initialPositions[(i)*4 + 2] = h/2.0f * (cl_float)( (i/(side*side))) + 0.5f*h;

            testData->initialPositions[(i)*4 + 3] = 0.0f;
        }

        clResult = clEnqueueWriteBuffer(testData->queue, testData->position, 0, 0, sizeof(cl_float4)*PARTICLE_COUNT, testData->initialPositions, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "Position buffer write", curEvent);
    }


    cluClErrorTest(clResult);
    {
        const size_t pcount = PARTICLE_COUNT;

        clResult = clEnqueueNDRangeKernel(testData->queue, testData->clearVelocityAcceleration, 1, NULL, &pcount, NULL, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "clearVelocityAcceleration", curEvent);
    }

    /* The initial sorting of the particles */
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->updateKeys, 1, NULL, &pcount, NULL, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "updateKeys", curEvent);
    result = sphSortParticles(testData);
    kzsErrorForward(result);
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->sortPostProcess, 1, NULL, &pcount, NULL, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "sortPostProcess", curEvent);
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->clearVoxels, 1, NULL, &vcount, NULL, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "clearVoxels", curEvent);
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->setVoxels, 1, NULL, &pcount, NULL, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "setVoxels", curEvent);
    clResult = clEnqueueReadBuffer(testData->queue, testData->sortedPosition, 0, 0, sizeof(cl_float4)*PARTICLE_COUNT, testData->initialPositions, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "sortedPosition", curEvent);
    clFinish(testData->queue);



    kzsSuccess();
}

kzsError sphSortParticles(struct SphTestData *testData)
{
    cl_int clResult;
    cl_uint stage;
    cl_uint pass;
    cl_event curEvent;
    size_t fsize = PARTICLE_COUNT / 2;

    clResult = clSetKernelArg(testData->bitonicSort, 0, sizeof(cl_mem), &testData->keys);
    cluClErrorTest(clResult);

    for(stage = 0; stage < (kzUint)PARTICLE_LOG2; stage++)
    {
        clResult = clSetKernelArg(testData->bitonicSort, 1, sizeof(cl_uint), &stage);
        cluClErrorTest(clResult);
        for(pass = 0; pass <= stage; pass++)
        {
            clResult = clSetKernelArg(testData->bitonicSort, 2, sizeof(cl_uint), &pass);
            cluClErrorTest(clResult);
            clResult = clEnqueueNDRangeKernel(testData->queue, testData->bitonicSort, 1, NULL, &fsize, NULL, 0, NULL, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "bitonicSort", curEvent);
        }
    }
    kzsSuccess();
}


kzsError sphSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct SphTestData *testData;
    kzsError result;
    cl_int clResult;
    const size_t pcount = PARTICLE_COUNT;
    size_t vcount;   
    cl_event curEvent;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    vcount = testData->voxelCount[0] * testData->voxelCount[1] * testData->voxelCount[2];
    
    if((bfSceneGetFrameInitialCounterValue(scene) - bfSceneGetFrameCounterValue(scene)) % 400 == 0) 
    {
        result = sphResetScene(testData);
        kzsErrorForward(result);
    }

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    clResult = clEnqueueAcquireGLObjects(testData->queue, 1, &testData->vertexBufferObjectCLBuffer, 0, 0, 0);
    cluClErrorTest(clResult);
#endif

    /* Sorting based approach for O(nlog^2n) complexity */
    
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->updateKeys, 1, NULL, &pcount, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "updateKeys", curEvent);
    result = sphSortParticles(testData);
    kzsErrorForward(result);

    clResult = clEnqueueNDRangeKernel(testData->queue, testData->sortPostProcess, 1, NULL, &pcount, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "sortPostProcess", curEvent);

    clResult = clEnqueueNDRangeKernel(testData->queue, testData->clearVoxels, 1, NULL, &vcount, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "clearVoxels ", curEvent);
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->setVoxels, 1, NULL, &pcount, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "setVoxels ", curEvent);

    clResult = clEnqueueNDRangeKernel(testData->queue, testData->calculateDensity, 1, NULL, &pcount, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "calculateDensity ", curEvent);
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->calculateAcceleration, 1, NULL, &pcount, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "calculateAcceleration ", curEvent);
    clResult = clEnqueueNDRangeKernel(testData->queue, testData->integrate, 1, NULL, &pcount, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "integrate ", curEvent);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    {
        struct KzuTransformedScene* transformedScene = kzaApplicationGetTransformedScene(bfGetApplication(framework));
        const struct KzcDynamicArray* transformedObjects = kzuTransformedSceneGetObjects(transformedScene);
        struct KzuTransformedObjectNode* transformedCameraNode = kzuTransformedObjectListFindObject(transformedObjects, 
            kzuCameraNodeToObjectNode(testData->cameraNode));
        struct KzcVector3 points[4];
        kzFloat radius = 0.085f;

        points[0] = kzcVector3(-radius, -radius, 0.0f);
        points[1] = kzcVector3(radius, -radius, 0.0f);
        points[2] = kzcVector3(-radius, radius, 0.0f);
        points[3] = kzcVector3(radius, radius, 0.0f);
        {
            struct KzcVector3 tmp;
            struct KzcMatrix4x4 matrix = kzuTransformedObjectNodeGetMatrix(transformedCameraNode);
            kzcMatrix4x4MultiplyVector3By3x3(&matrix, &points[0], &tmp);
            points[0] = tmp;
            kzcMatrix4x4MultiplyVector3By3x3(&matrix, &points[1], &tmp);
            points[1] = tmp;
            kzcMatrix4x4MultiplyVector3By3x3(&matrix, &points[2], &tmp);
            points[2] = tmp;
            kzcMatrix4x4MultiplyVector3By3x3(&matrix, &points[3], &tmp);
            points[3] = tmp;
        }

        {
            kzUint i, j;
            kzFloat pointData[4 * 4] = {0.0f};
            for(i = 0; i < 4; ++i)
            for(j = 0; j < 3; ++j)
            {
                pointData[i * 4 + j] = points[i].data[j];
            }
            clResult = clEnqueueWriteBuffer(testData->queue, testData->billboardBuffer, 0, 0, sizeof(cl_float)*4*4, pointData, 0, NULL, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "Write billboardBuffer ", curEvent);

        }

        result = cluSetKernelArguments(testData->updateVBO, 
            sizeof(cl_mem), &testData->sortedPosition, 
            sizeof(cl_mem), &testData->vertexBufferObjectCLBuffer,
            sizeof(cl_mem), &testData->billboardBuffer,
            sizeof(cl_float), &testData->yoffset,
            sizeof(cl_float), &testData->offset,
            sizeof(cl_float), &testData->scale);
        kzsErrorForward(result);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->updateVBO, 1, NULL, &pcount, testData->localWorkSize, 0, NULL, &curEvent);
        cluClErrorTest(clResult); 
        cluProfilerAddEvent(testData->profiler, "updateVBO ", curEvent);
    }
    
    clResult = clEnqueueReleaseGLObjects(testData->queue, 1, &testData->vertexBufferObjectCLBuffer, 0, KZ_NULL, KZ_NULL);
    cluClErrorTest(clResult);
#endif

    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

    kzsSuccess();
}

kzsError sphSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct SphTestData *testData = NULL;
    kzsError result;
    cl_int clResult;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    
    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

    result = bfUtilFreeLocalWorkgroupSize(testData->localWorkSize);
    kzsErrorForward(result);

    clResult = clReleaseMemObject(testData->keys);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->newkeys);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->sortedPosition);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->sortedVelocity);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->sortedDensity);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->position);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->velocity);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->acceleration);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->voxelParticles);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->particleCount);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->billboardBuffer);
    cluClErrorTest(clResult);
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    clResult = clReleaseMemObject(testData->vboIndexBufferCl);
    cluClErrorTest(clResult);
    clResult = clReleaseMemObject(testData->vertexBufferObjectCLBuffer);
    cluClErrorTest(clResult);
    glDeleteBuffers(1, &testData->vertexBufferObjectHandle);
    glDeleteBuffers(1, &testData->vboIndexBufferHandle);
#endif
    clResult = clReleaseKernel(testData->bitonicSort);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->updateKeys);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->sortPostProcess);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->clearVoxels);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->setVoxels);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->calculateDensity);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->calculateAcceleration);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->clearVelocityAcceleration);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->integrate);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->cumSumVoxels);
    cluClErrorTest(clResult);
    clResult = clReleaseKernel(testData->updateVBO);
    cluClErrorTest(clResult);

    clResult = clReleaseProgram(testData->sphProgram);
    cluClErrorTest(clResult);

    clResult = clReleaseCommandQueue(testData->queue);
    cluClErrorTest(clResult);
    result = kzcMemoryFreeArray(testData->initialPositions);
    kzsErrorForward(result);

    {
        struct KzuObjectNode* node;        
        result = kzuProjectLoaderLoadObjectNode(bfGetProject(framework), "Scenes/sph/Root Node/SPH_3D", &node);
        kzsErrorForward(result);
        result = kzuObjectNodeRemoveChild(node, (struct KzuObjectNode*)testData->dynamicMesh);
        kzsErrorForward(result);
        result = dynamicMeshNodeDelete(testData->dynamicMesh);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError sphCreate(const struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct SphTestData *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, sphSceneLoad, sphSceneUpdate, sphSceneRender,
        sphSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    /* TOOD: space after comma. */
    result = kzcMemoryAllocVariable(memoryManager, testData,"Smoothed particle hydrodynamics test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Smoothed Particle Hydrodynamics Test", "Physics", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;
    kzsSuccess();
}

KZ_CALLBACK kzsError sphSceneRender(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct SphTestData *testData;
    kzsError result;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    result = sphUpdateCamera(framework, scene);
    kzsErrorForward(result);

    kzsSuccess();
}
