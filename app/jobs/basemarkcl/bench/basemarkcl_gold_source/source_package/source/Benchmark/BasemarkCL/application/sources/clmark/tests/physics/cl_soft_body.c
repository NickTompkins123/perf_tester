/**
* \file
* OpenCL soft body test.
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "cl_soft_body.h"

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>
#include <benchmarkutil/util/bf_file_validator.h>
#include <benchmarkutil/settings/bf_settings.h>

#include <application/kza_application.h>

#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader_material.h>
#include <user/project/kzu_project_loader_camera.h>
#include <user/material/kzu_material_type.h>
#include <user/material/kzu_material.h>
#include <user/scene_graph/kzu_object.h>
#include <user/scene_graph/kzu_mesh.h>
#include <user/scene_graph/kzu_scene.h>
#include <user/scene_graph/kzu_camera.h>
#include <user/scene_graph/kzu_transformed_object.h>
#include <user/renderer/kzu_renderer.h>
#include <user/animation/kzu_animation_player.h>

#include <core/resource_manager/shader/kzc_resource_shader.h>
#include <core/memory/kzc_memory_manager.h>
#include <core/renderer/kzc_renderer.h>
#include <core/util/io/kzc_file.h>
#include <core/util/string/kzc_string.h>
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
#include <clutil/clu_profiler.h>

#include <clmark/test_definitions.h>


#include "dynamic_mesh.h"


/** Rest length of cloth spring. */
kzFloat REST_LENGTH;
/** Y position of the cloth holding hooks. */
kzFloat TOP_Y;


/** Use CPU to calculate the cloth. Currently implementation is outdated. */
/*#define CPU 1*/

/** Copy data after calls from GPU memory to CPU memory. Useful for debugging */
/*#define USE_DEBUG_COPY_TO_HOST_CALLS 1*/

/** Particles per side in the particle grid. */
kzInt PARTICLES_PER_SIDE;
/** Amount of particles in the simulation. */
#define PARTICLE_COUNT (PARTICLES_PER_SIDE * PARTICLES_PER_SIDE)

#define INDEX_COUNT ((PARTICLES_PER_SIDE - 1) * (PARTICLES_PER_SIDE - 1) * 2 * 3)

/** Helper for accessing particle in grid at index x, y. */
#define INDEX(x, y) ((x) + ((PARTICLES_PER_SIDE) * (y)))

/** Amount of neighbours for each particle. The springs count for each. */
#define NEIGHBOUR_COUNT 8

/** Amount of data in one vertex in VBO. */
#define VERTEX_DATA_SIZE 8


#define COLLISION_OBJECT_COUNT 9
kzFloat collisionObjectPositionArray[COLLISION_OBJECT_COUNT * 4] =
{
    2.0f, 0.0f, 0.0f, 0.0f,
    -4.1f, 0.0f, 0.0f, 0.0f,
    -1.7f, 3.5f, -0.2f, 0.0f,
    -1.7f, 0.0f, 3.8f, 0.0f,
    -1.7f, 0.0f, -3.77f, 0.0f,
    -4.1f, 0.0f, -7.5f, 0.0f,
    -4.1f, 0.0f, -12.5f, 0.0f,
    -4.85f, 3.5f, 3.02f, 0.0f,
    -4.85f, 6.42f, -0.30f, 0.0f
};


struct SoftBodyTestState
{    
    kzUint particlePositionBufferSize; /**< Size of particle position buffer. */
    struct KzcVector4* particlePositions; /**< Particle positions. */
    struct KzcVector4* particleOldPositions; /**< Particle old positions. */
    struct KzcVector4* particleForces; /**< Forces affecting the particles. */
    struct KzcVector4* faceNormal; /**< Unaveraged face normals. */
    kzUint particleNeighbourBufferSize; /**< Size of buffer containing indexes to cells neighboring each other. */
    kzInt* particleNeighbourIndexes; /**< Indices to the neighbors of particles. Neighbor I'th element are stored in area [NEIGHBOUR_COUNT * i, NEIGHBOUR_COUNT * (i+1)]. */
    kzInt* particleFaceIndexes; /**< Indices indicating faces where each particle belongs to. */
    kzInt* particleSatisfyConstraintOffsets; /**< Buffer indicating the offsets used to calculate the cloth. The offsets are there to stop multiple kernels writing same data. */
    kzUint particleFaceBufferSize; /**< Size of buffer containing indexes to vertices creating a triangle. */

#ifndef CPU
    cl_mem particlePositionsBuffer; /**< Cl memory mapped buffer positions. */
    cl_mem particleOldPositionsBuffer; /**< Cl buffer old positions. */
    cl_mem particleForcesBuffer; /**< Cl buffer forces. */
    cl_mem particleNeighbourIndexesBuffer; /**< Cl buffer neighbors. */
    cl_mem faceNormalBuffer; /**< Cl buffer for face normals. */
    cl_mem particleFaceIndexBuffer; /**< Cl buffer for per particle face normal. */
    cl_mem particleSatisfyConstraintOffsetBuffer; /**< Cl buffer for satisfy constraints offsets. */
#endif

    cl_program softBodyProgram; /**< CL program containing the kernels. */

    cl_device_id device; /**< Device id. */
    kzUint softBodyKernelCount; /**< Amount of CL kernels. */
    cl_kernel* softBodyKernels; /**< CL kernels. */
    cl_command_queue queue; /**< Command queue. */

    kzUint vertexBufferObjectHandle; /**< Vertex Buffer Object OpenGL handle. */
    cl_mem vertexBufferObjectCLBuffer; /**< Cl buffer for vbo. */

    kzUint vboIndexBufferHandle; /** Index Buffer Object OpenGL handle. */
    cl_mem vboIndexBufferCl; /**< Cl buffer for IBO. */
    kzUint* indices; /**< Triangle indices for rendering. Same as IBO. */

#ifdef CPU
    kzFloat* vboBuffer; /**< Float buffer for uploading the cloth VBO to GPU. Used when running without OpenCL. */
#endif

    struct KzuMaterial* material; /**< Material used to render cloth. */
    struct DynamicMeshNode* dynamicMesh; /**< Kanzi scenegraph object for cloth. */

    struct KzuScene* scene; /**< Scene for rendering cloth. */
    struct KzuCameraNode* cameras[3]; /**< Camera nodes used to render scene from different angles. */

    size_t* localWorkSize; /**< Local work size. Read from configuration file. */
    struct CluProfiler *profiler; /**< Clu profiler */

    size_t workGroupSize[2]; /**< Workgroup size. */
};


kzsError softBodySceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError softBodySceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError softBodyScenePreUpdate(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError softBodyScenePostUpdate(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError softBodySceneRender(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError softBodySceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError softBodyAccumulateForces(struct BenchmarkFramework* framework, struct SoftBodyTestState* testData);
kzsError softBodyIntegrate(struct BenchmarkFramework* framework, struct SoftBodyTestState* testData);
kzsError softBodySatisfyConstraints(struct BenchmarkFramework* framework, struct BfScene* scene, struct SoftBodyTestState* testData);
kzsError softBodyUpdateCamera(struct BenchmarkFramework* framework, struct BfScene* scene);


kzsError renderCallback(struct KzuRenderer* renderer, const struct KzuTransformedObjectNode* transformedObjectNode, void* userData)
{
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    kzsError result;
    struct SoftBodyTestState* testData = userData;
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
        kzsAssertText(found, "Material for rendering cloth must have position attribute.");
        found = kzcHashMapGet(hashMap, "kzNormal", (void**)&normalIndex);
        kzsAssertText(found, "Material for rendering cloth must have normal attribute.");

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


kzsError softBodyAccumulateForces(struct BenchmarkFramework* framework, struct SoftBodyTestState* testData)
{
    kzsAssert(kzcIsValidPointer(testData));

#ifdef CPU
    {
        kzUint i;
        for(i = 0; i < PARTICLE_COUNT; ++i)
        {
            testData->particleForces[i].data[1] = -200.f;
        }
    }
#else
    {
        kzsError result;
        cl_int clResult;
        cl_event curEvent;
        
        testData->workGroupSize[0] = PARTICLE_COUNT;
        testData->workGroupSize[1] = 1;
#ifdef USE_DEBUG_COPY_TO_HOST_CALLS
        clResult = clEnqueueWriteBuffer(testData->queue, testData->particleForcesBuffer, KZ_TRUE, 0, testData->particlePositionBufferSize,
            testData->particleForces, 0, NULL, NULL);
        cluClErrorTest(clResult);
#endif

        result = cluSetKernelArguments(testData->softBodyKernels[3], 
            sizeof(cl_mem), &testData->particleForcesBuffer); 
        kzsErrorForward(result);

        clResult = clEnqueueNDRangeKernel(testData->queue, testData->softBodyKernels[3], 2, NULL, testData->workGroupSize, 0, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "applyForces", curEvent);

#ifdef USE_DEBUG_COPY_TO_HOST_CALLS        
        clResult = clEnqueueReadBuffer(testData->queue, testData->particleForcesBuffer, KZ_TRUE, 0, testData->particlePositionBufferSize,
            testData->particleForces, 0, NULL, NULL);
        cluClErrorTest(clResult);
#endif
    }
#endif

    kzsSuccess();
}

kzsError softBodyIntegrate(struct BenchmarkFramework* framework, struct SoftBodyTestState* testData)
{
    kzsAssert(kzcIsValidPointer(testData));

#ifdef CPU
    {
        const kzFloat dt = 0.01f;
        kzUint i;
        for(i = 0; i < PARTICLE_COUNT; ++i)
        {
            struct KzcVector4 temp;
            struct KzcVector4 f;
            struct KzcVector4 addPos;
            struct KzcVector4 addPos2;

            temp = testData->particlePositions[i];
            f = testData->particleForces[i];

            /* Equals = positions[i] += (positions[i] - oldPositions[i]) * dampen + forces[i] * dt * dt; */
            kzcVector4Subtract(&testData->particlePositions[i], &testData->particleOldPositions[i], &addPos);
            kzcVector4Scale(&addPos, 0.97f, &addPos);
            kzcVector4Scale(&f, dt * dt, &addPos2);

            kzcVector4Add(&testData->particlePositions[i], &addPos, &testData->particlePositions[i]);
            kzcVector4Add(&testData->particlePositions[i], &addPos2, &testData->particlePositions[i]);
            testData->particleOldPositions[i] = temp;
        }
    }
#else
    {
        kzsError result;
        cl_int clResult;
        cl_event curEvent;

        testData->workGroupSize[0] = PARTICLE_COUNT;
        testData->workGroupSize[1] = 1;

#ifdef USE_DEBUG_COPY_TO_HOST_CALLS
        clResult = clEnqueueWriteBuffer(testData->queue, testData->particlePositionsBuffer, KZ_TRUE, 0, testData->particlePositionBufferSize,
            testData->particlePositions, 0, NULL, NULL);
        cluClErrorTest(clResult);
        clResult = clEnqueueWriteBuffer(testData->queue, testData->particleOldPositionsBuffer, KZ_TRUE, 0, testData->particlePositionBufferSize,
            testData->particleOldPositions, 0, NULL, NULL);
        cluClErrorTest(clResult);
        clResult = clEnqueueWriteBuffer(testData->queue, testData->particleForcesBuffer, KZ_TRUE, 0, testData->particlePositionBufferSize,
            testData->particleForces, 0, NULL, NULL);
        cluClErrorTest(clResult);
#endif
        
        
        result = cluSetKernelArguments(testData->softBodyKernels[1],
            sizeof(cl_mem), &testData->particlePositionsBuffer,
            sizeof(cl_mem), &testData->particleOldPositionsBuffer,
            sizeof(cl_mem), &testData->particleForcesBuffer
            );
        kzsErrorForward(result);
        
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->softBodyKernels[1], 2, NULL, testData->workGroupSize, 0, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "integrateSoftBody", curEvent);


#ifdef USE_DEBUG_COPY_TO_HOST_CALLS
        clResult = clEnqueueReadBuffer(testData->queue, testData->particlePositionsBuffer, KZ_TRUE, 0, testData->particlePositionBufferSize,
            testData->particlePositions, 0, NULL, NULL);
        cluClErrorTest(clResult);
        clResult = clEnqueueReadBuffer(testData->queue, testData->particleOldPositionsBuffer, KZ_TRUE, 0, testData->particlePositionBufferSize,
            testData->particleOldPositions, 0, NULL, NULL);
        cluClErrorTest(clResult);
        clResult = clEnqueueReadBuffer(testData->queue, testData->particleForcesBuffer, KZ_TRUE, 0, testData->particlePositionBufferSize,
            testData->particleForces, 0, NULL, NULL);
        cluClErrorTest(clResult);
#endif


    }
#endif

    kzsSuccess();
}

kzsError softBodySatisfyConstraints(struct BenchmarkFramework* framework, struct BfScene* scene, struct SoftBodyTestState* testData)
{
    kzUint n;
    const kzUint iterations = 20;
    kzsAssert(kzcIsValidPointer(testData));

    for(n = 0; n < iterations; ++n)
    {

#ifdef CPU
        kzUint i, j;

        for(i = 0; i < PARTICLE_COUNT; ++i)
        {

            for(j = 0; j < NEIGHBOUR_COUNT; ++j)
            {
                kzFloat rest = REST_LENGTH;
                kzInt index = testData->particleNeighbourIndexes[i * NEIGHBOUR_COUNT + j];
                if(j>1)
                {
                    rest = kzsSqrtf(rest*rest + rest*rest);
                }
                if(j>3)
                {
                    rest = kzsSqrtf((2.f*rest)*(2.f*rest)+(2.f*rest)*(2.f*rest));
                }
                if(index >= 0)
                {
                    struct KzcVector4 x1 = testData->particlePositions[i];
                    struct KzcVector4 x2 = testData->particlePositions[index];
                    struct KzcVector4 delta;
                    kzFloat deltaDot;
                    struct KzcVector4 deltaProjected;
                    float deltalength;
                    float diff;
                    kzFloat separate;

                    kzcVector4Subtract(&x1, &x2, &delta);
                    deltaDot = kzcVector4Dot(&delta, &delta);
                    
                    {
                        const kzFloat softness = 0.95f; /* 1.0f stiff, 0.f totally relaxed */
                        deltalength = kzsSqrtf(deltaDot);
                        diff = (deltalength - rest) / deltalength;
                        separate = 0.5f * diff * softness;
                    }

                    kzcVector4Scale(&delta, separate, &deltaProjected);

                    {
                        kzcVector4Subtract(&testData->particlePositions[i], &deltaProjected, &testData->particlePositions[i]);
                        kzcVector4Add(&testData->particlePositions[index], &deltaProjected, &testData->particlePositions[index]);
                    }
                }
            }
        }

        {
            kzUint collisionObject;
            for(collisionObject = 0; collisionObject < COLLISION_OBJECT_COUNT; ++collisionObject)
            {
                const float ballSize = 5.4f;
                int offset = collisionObject * 4;
                const float offsetZ = 7.0f;
                struct KzcVector4 ballPos = kzcVector4(collisionObjectPositionArray[offset], collisionObjectPositionArray[offset + 1], 
                    collisionObjectPositionArray[offset + 2] + offsetZ, collisionObjectPositionArray[offset + 3]);
                struct KzcVector4 dif;
                kzFloat length;
                kzFloat separation = ballSize;
                kzcVector4Subtract(&testData->particlePositions[i], &ballPos, &dif);
                length = kzcVector4GetLength(&dif);
                if(length < separation)
                {
                    kzcVector4Scale(&dif, 1.0f / length, &dif);
                    kzcVector4Scale(&dif, length - separation, &dif);
                    kzcVector4Subtract(&testData->particlePositions[i], &dif, &testData->particlePositions[i]);
                }
            }
        }

        {
            kzInt tick = bfSceneGetFrameCounterValue(scene);
            kzFloat distance = (PARTICLES_PER_SIDE - 1) * REST_LENGTH * 1.f;
            kzFloat offset = sin(tick * 0.04f) * distance * 1.0f;
            kzFloat offset2 = sin(tick * 0.036f) * distance * 0.95f;
            testData->particlePositions[INDEX(0, 0)] = kzcVector4(-distance * 0.5 + offset, TOP_Y, offset2 + distance, 0.0f);
            testData->particlePositions[INDEX(PARTICLES_PER_SIDE - 1, 0)] = kzcVector4(distance + offset, TOP_Y, offset2, 0.0f);
        }

        {
            struct KzcVector4 min = kzcVector4(-250.f,    1.0f, -250.0f, 0.0f);
            struct KzcVector4 max = kzcVector4( 250.f,  150.0f,  250.0f, 0.0f);
            testData->particlePositions[i] = kzcVector4ComponentMax(&testData->particlePositions[i], &min);
            testData->particlePositions[i] = kzcVector4ComponentMin(&testData->particlePositions[i], &max);
        }
#else
        {
            kzsError result;
            cl_int clResult;

#ifdef USE_DEBUG_COPY_TO_HOST_CALLS
            clResult = clEnqueueWriteBuffer(testData->queue, testData->particlePositionsBuffer, KZ_TRUE, 0, testData->particlePositionBufferSize,
                testData->particlePositions, 0, NULL, NULL);
            cluClErrorTest(clResult);
            /*clResult = clEnqueueWriteBuffer(testData->queue, testData->particleNeighbourIndexesBuffer, KZ_TRUE, 0, testData->particleNeighbourBufferSize,
                testData->particleNeighbourIndexes, 0, NULL, NULL);
            cluClErrorTest(clResult);*/
#endif

            {
                kzUint j;
                cl_event curEvent;
                cl_float tick = (cl_float)(625.0f - (bfSceneGetFrameInitialCounterValue(scene) - bfSceneGetFrameCounterValue(scene)));
                cl_int stOffset[8];
                stOffset[0] = 0; 
                stOffset[1] = 1;
                stOffset[2] = 2;
                stOffset[3] = 3;
                stOffset[4] = PARTICLES_PER_SIDE + 0;
                stOffset[5] = PARTICLES_PER_SIDE + 1;
                stOffset[6] = PARTICLES_PER_SIDE + 2;
                stOffset[7] = PARTICLES_PER_SIDE + 3;

                testData->workGroupSize[0] = PARTICLE_COUNT / 8;
                testData->workGroupSize[1] = 0;
                for(j = 0; j < 8; ++j)
                {
                    result = cluSetKernelArguments(testData->softBodyKernels[2],
                        sizeof(cl_mem), &testData->particlePositionsBuffer,
                        sizeof(cl_mem), &testData->particleNeighbourIndexesBuffer,
                        sizeof(cl_mem), &testData->particleSatisfyConstraintOffsetBuffer,
                        sizeof(cl_float), &tick,
                        sizeof(cl_int), &stOffset[j]
                        );
                    kzsErrorForward(result);
                    clResult = clEnqueueNDRangeKernel(testData->queue, testData->softBodyKernels[2], 1, NULL, testData->workGroupSize, 0, 0, NULL, &curEvent);
                    cluClErrorTest(clResult);
                    cluProfilerAddEvent(testData->profiler, "satisfyConstraints", curEvent);

                }
            }


#ifdef USE_DEBUG_COPY_TO_HOST_CALLS
            clResult = clEnqueueReadBuffer(testData->queue, testData->particlePositionsBuffer, KZ_TRUE, 0, testData->particlePositionBufferSize,
                testData->particlePositions, 0, NULL, NULL);
            cluClErrorTest(clResult);
            /*clResult = clEnqueueReadBuffer(testData->queue, testData->particleNeighbourIndexesBuffer, KZ_TRUE, 0, testData->particleNeighbourBufferSize,
                testData->particleNeighbourIndexes, 0, NULL, NULL);
            cluClErrorTest(clResult);*/
#endif
        }
#endif
    }
    
    kzsSuccess();
}

void addFaceToParticle(struct SoftBodyTestState* testData, kzUint faceIndex, kzUint particleIndex)
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

KZ_CALLBACK kzsError softBodySceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct SoftBodyTestState *testData;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(framework);
    kzsError result;
    cl_int clResult;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    bfSceneSetFrameCounter(scene, SOFT_BODY_FRAME_COUNT);


    {
        kzBool fullProfile;
        result = bfGetCLFullProfile(framework, &fullProfile);
        kzsErrorForward(result);

        if(fullProfile)
        {
            PARTICLES_PER_SIDE = 128;
            TOP_Y = 28.5f;
            REST_LENGTH = 0.1f;
        }
        else
        {
            PARTICLES_PER_SIDE = 64;
            TOP_Y = 20.5f;
            REST_LENGTH = 0.2f;
        }
    }

    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/soft_body");
    kzsErrorForward(result);
    testData->scene = kzaApplicationGetScene(bfGetApplication(framework));
    result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), "Materials/SPCloth/ClothMaterial", &testData->material);
    kzsErrorForward(result);

    result = kzuProjectLoaderLoadCameraNode(bfGetProject(framework), "Scenes/soft_body/Root Node/soft_body_Camera1", &testData->cameras[0]);
    kzsErrorForward(result);
    result = kzuProjectLoaderLoadCameraNode(bfGetProject(framework), "Scenes/soft_body/Root Node/soft_body_Camera2", &testData->cameras[1]);
    kzsErrorForward(result);
    result = kzuProjectLoaderLoadCameraNode(bfGetProject(framework), "Scenes/soft_body/Root Node/soft_body_Camera3", &testData->cameras[2]);
    kzsErrorForward(result);

    testData->profiler = bfSceneGetProfiler(scene);

    softBodyUpdateCamera(framework, scene);

    /* Init data and CL buffers. */
    {
        kzUint size = PARTICLE_COUNT * sizeof(struct KzcVector4);
        testData->particlePositionBufferSize = size;
        result = kzcMemoryAllocPointer(memoryManager, &testData->particlePositions, size, "Buffer data");
        kzsErrorForward(result);
        result = kzcMemoryAllocPointer(memoryManager, &testData->particleOldPositions, size, "Buffer data");
        kzsErrorForward(result);
        result = kzcMemoryAllocPointer(memoryManager, &testData->particleForces, size, "Buffer data");
        kzsErrorForward(result);
        result = kzcMemoryAllocPointer(memoryManager, &testData->faceNormal, INDEX_COUNT / 3 * sizeof(struct KzcVector4), "Face Normal Buffer data");
        kzsErrorForward(result);
        

        testData->particleNeighbourBufferSize = PARTICLE_COUNT * NEIGHBOUR_COUNT * sizeof(kzInt);
        result = kzcMemoryAllocPointer(memoryManager, &testData->particleNeighbourIndexes, testData->particleNeighbourBufferSize, "Buffer data");
        kzsErrorForward(result);
        kzsMemset(testData->particleNeighbourIndexes, -1, testData->particleNeighbourBufferSize);

        testData->particleFaceBufferSize = PARTICLE_COUNT * 8 * sizeof(kzInt);
        result = kzcMemoryAllocPointer(memoryManager, &testData->particleFaceIndexes, testData->particleFaceBufferSize, "Particle Face Index Buffer data");
        kzsErrorForward(result);
        kzsMemset(testData->particleFaceIndexes, -1, testData->particleFaceBufferSize);

        result = kzcMemoryAllocPointer(memoryManager, &testData->particleSatisfyConstraintOffsets, PARTICLE_COUNT / 8 * sizeof(kzInt), "Satisfy constraints offset buffer data");
        kzsErrorForward(result);
    }

    /* Initialize particle positions. */
    {
        kzUint i = 0;
        kzInt x, y;
        srand(23);
        for(x = 0; x < PARTICLES_PER_SIDE; ++x)
        for(y = 0; y < PARTICLES_PER_SIDE; ++y)
        {
            kzFloat xx = (y - PARTICLES_PER_SIDE) * REST_LENGTH;
            kzFloat yy = (-25 + x) * REST_LENGTH;
            testData->particlePositions[i] = kzcVector4(0.0f, TOP_Y, 0.0f, 0.0f);
            testData->particlePositions[i].data[0] += xx;
            testData->particlePositions[i].data[1] += yy;
            testData->particlePositions[i].data[2] += xx;
            testData->particleOldPositions[i] = testData->particlePositions[i];
            testData->particleForces[i] = kzcVector4(0.0f, 0.0f, 0.0f, 0.0f);
            i++;
        }
    }

    {
        kzInt x, y;
        for(x = 0; x < PARTICLES_PER_SIDE; ++x)
        for(y = 0; y < PARTICLES_PER_SIDE; ++y)
        {
            kzUint offset = INDEX(x, y) * NEIGHBOUR_COUNT;
            {
                kzUint j;
                for(j = 0; j < NEIGHBOUR_COUNT; ++j)
                {
                    testData->particleNeighbourIndexes[offset + j] = -1;
                }
            }
            if(x > 0)
            {
                testData->particleNeighbourIndexes[offset + 0] = INDEX(x - 1, y);
            }
            if(x < PARTICLES_PER_SIDE - 2)
            {
                testData->particleNeighbourIndexes[offset + 1] = INDEX(x + 1, y);
            }
            if(y > 0)
            {
                testData->particleNeighbourIndexes[offset + 2] = INDEX(x, y - 1);
            }
            if(x > 0 && y > 0)
            {
                testData->particleNeighbourIndexes[offset + 3] = INDEX(x - 1, y - 1);
            }
            if(x < (PARTICLES_PER_SIDE - 2) && y > 0)
            {
                testData->particleNeighbourIndexes[offset + 4] = INDEX(x + 1, y - 1);
            }
            if(x > 1 && y > 0)
            {
                testData->particleNeighbourIndexes[offset + 6] = INDEX(x - 2, y - 1);
            }
            if(x < (PARTICLES_PER_SIDE - 3) && y > 0)
            {
                testData->particleNeighbourIndexes[offset + 5] = INDEX(x + 2, y - 1);
            }
        }
    }


    /* Build satisfy constraints offset table. */
    {
        kzUint offset = 0;
        kzUint i;
        for(i = 0; i < (kzUint)PARTICLE_COUNT/8; ++i)
        {
            kzUint index = i * 4;
            kzUint row = index / (PARTICLES_PER_SIDE);
            index = (index % PARTICLES_PER_SIDE) + PARTICLES_PER_SIDE * row * 2;
            testData->particleSatisfyConstraintOffsets[offset] = index;
            offset++;
        }
    }

    result = bfUtilReadLocalWorkgroupSize(framework, "SoftBodyLocalWorkSize", 2, &testData->localWorkSize);
    kzsErrorForward(result);
    {
        kzString kernelPath =  "data/softbody.cl";
        kzString compilerSettings = COMPILER_FLAGS;
        cl_context context = bfGetClContext(framework);
        kzBool binaryProgramExists;
        
        result = cluGetProgramBinaryExists(memoryManager, kernelPath, &binaryProgramExists);
        kzsErrorForward(result);
        
        /* If binary kernel is available use it. */
        if(binaryProgramExists)
        {
            kzBool binary;
            result = cluLoadProgramFromFile(memoryManager, kernelPath, KZ_FALSE, context, &testData->softBodyProgram, &binary);
            kzsErrorForward(result);
            bfSceneSetUseProgramBinary(scene, KZ_TRUE);
        }
        else
        {
            /* Append required defines to softbody.cl. */
            kzMutableString kernelSourceFromFile;
            kzMutableString kernelSource;        
            kzBool isValid;
            result = bfValidateFile(framework, kernelPath, SOFTBODY_HASH, &isValid);
            kzsErrorForward(result);
            bfSceneSetValidConfigurationData(scene, isValid);

            result = kzcFileReadTextFile(bfGetMemoryManager(framework), kernelPath, &kernelSourceFromFile);        
            kzsErrorForward(result);
            result = kzcStringFormat(bfGetMemoryManager(framework), "#define PARTICLES_PER_SIDE %d\n#define TOP_Y %f\n#define REST_LENGTH %f\n#define NEIGHBOUR_COUNT %d\n%s", 
                &kernelSource, PARTICLES_PER_SIDE, TOP_Y, REST_LENGTH, NEIGHBOUR_COUNT, kernelSourceFromFile);
            kzsErrorForward(result);

            result = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &testData->device, NULL);
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

                testData->queue = clCreateCommandQueue(context, testData->device, cqProperties, &clResult);
                cluClErrorTest(clResult);
            }

            result = cluGetBuiltProgramFromStringWithOptions(memoryManager, kernelSource, compilerSettings, context, &testData->softBodyProgram);        
            kzsErrorForward(result);        
            result = kzcStringDelete(kernelSourceFromFile);        
            kzsErrorForward(result);        
            result = kzcStringDelete(kernelSource);        
            kzsErrorForward(result);
        }
    }
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    {
        kzUint vboSize = PARTICLE_COUNT * sizeof(float) * VERTEX_DATA_SIZE;
        glGenBuffers(1, &testData->vertexBufferObjectHandle);
        glBindBuffer(KZS_GL_ARRAY_BUFFER, testData->vertexBufferObjectHandle);
        glBufferData(KZS_GL_ARRAY_BUFFER, vboSize, 0, KZS_GL_DYNAMIC_DRAW);
        testData->vertexBufferObjectCLBuffer = clCreateFromGLBuffer(bfGetClContext(framework), CL_MEM_WRITE_ONLY, testData->vertexBufferObjectHandle, NULL);
    }
#endif

    {
        testData->softBodyKernelCount = 7;
        result = kzcMemoryAllocArray(memoryManager, testData->softBodyKernels, testData->softBodyKernelCount, "Kernel objects");
        kzsErrorForward(result);
        testData->softBodyKernels[0] = clCreateKernel(testData->softBodyProgram, "updateVBO", &clResult);
        cluClErrorTest(clResult);
        testData->softBodyKernels[1] = clCreateKernel(testData->softBodyProgram, "integrateSoftBody", &clResult);
        cluClErrorTest(clResult);
        testData->softBodyKernels[2] = clCreateKernel(testData->softBodyProgram, "satisfyConstraints", &clResult);
        cluClErrorTest(clResult);
        testData->softBodyKernels[3] = clCreateKernel(testData->softBodyProgram, "applyForces", &clResult);
        cluClErrorTest(clResult);
        testData->softBodyKernels[4] = clCreateKernel(testData->softBodyProgram, "calculateFaceNormals", &clResult);
        cluClErrorTest(clResult);
        testData->softBodyKernels[5] = clCreateKernel(testData->softBodyProgram, "averageFaceNormals", &clResult);
        cluClErrorTest(clResult);
        testData->softBodyKernels[6] = clCreateKernel(testData->softBodyProgram, "resetFaceNormals", &clResult);
        cluClErrorTest(clResult);
    }

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
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
                for(i = 0; i < (kzUint)INDEX_COUNT; i += 6)
                {
                    testData->indices[i    ] = startingIndex;
                    testData->indices[i + 1] = startingIndex + 1;
                    testData->indices[i + 2] = startingIndex + PARTICLES_PER_SIDE;
                    testData->indices[i + 3] = startingIndex + PARTICLES_PER_SIDE;
                    testData->indices[i + 4] = startingIndex + 1;
                    testData->indices[i + 5] = startingIndex + PARTICLES_PER_SIDE + 1;

                    addFaceToParticle(testData, (i+0)/3, startingIndex);
                    addFaceToParticle(testData, (i+1)/3, startingIndex + 1);
                    addFaceToParticle(testData, (i+2)/3, startingIndex + PARTICLES_PER_SIDE);
                    addFaceToParticle(testData, (i+3)/3, startingIndex + PARTICLES_PER_SIDE);
                    addFaceToParticle(testData, (i+4)/3, startingIndex + 1);
                    addFaceToParticle(testData, (i+5)/3, startingIndex + PARTICLES_PER_SIDE + 1);

                    offset++;
                    if(offset >= (kzUint)PARTICLES_PER_SIDE - 1)
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
#endif
    result = dynamicMeshNodeCreate(memoryManager, kzuProjectGetPropertyManager(bfGetProject(framework)), kzuProjectGetMessageDispatcher(bfGetProject(framework)), 
        &testData->dynamicMesh);
    kzsErrorForward(result);
    dynamicMeshSetCallback(testData->dynamicMesh, renderCallback, testData);
    result = kzuSceneAddObject(kzaApplicationGetScene(bfGetApplication(framework)), (struct KzuObjectNode*)testData->dynamicMesh);
    kzsErrorForward(result);

    {
        kzUint size = PARTICLE_COUNT * sizeof(struct KzcVector4);
#ifndef CPU
        {
            cl_int errorCode;
            cl_context context = bfGetClContext(framework);
            testData->particlePositionsBuffer = clCreateBuffer(context, (cl_mem_flags)CL_MEM_COPY_HOST_PTR, size, testData->particlePositions, &errorCode);
            cluClErrorTest(errorCode);
            testData->particleOldPositionsBuffer = clCreateBuffer(context, (cl_mem_flags)CL_MEM_COPY_HOST_PTR, size, testData->particleOldPositions, &errorCode);
            cluClErrorTest(errorCode);
            testData->particleForcesBuffer = clCreateBuffer(context, (cl_mem_flags)CL_MEM_COPY_HOST_PTR, size, testData->particleForces, &errorCode);
            cluClErrorTest(errorCode);
            testData->particleNeighbourIndexesBuffer = clCreateBuffer(context, (cl_mem_flags)CL_MEM_COPY_HOST_PTR, testData->particleNeighbourBufferSize, 
                testData->particleNeighbourIndexes, &errorCode);
            cluClErrorTest(errorCode);
            testData->faceNormalBuffer = clCreateBuffer(context, (cl_mem_flags)CL_MEM_COPY_HOST_PTR, INDEX_COUNT / 3 * sizeof(struct KzcVector4), 
                testData->faceNormal, &errorCode);
            cluClErrorTest(errorCode);
            testData->particleFaceIndexBuffer = clCreateBuffer(context, (cl_mem_flags)CL_MEM_COPY_HOST_PTR, PARTICLE_COUNT * 8 * sizeof(kzUint), 
                testData->particleFaceIndexes, &errorCode);
            cluClErrorTest(errorCode);
            testData->particleSatisfyConstraintOffsetBuffer = clCreateBuffer(context, (cl_mem_flags)CL_MEM_COPY_HOST_PTR, PARTICLE_COUNT / 8 * sizeof(kzUint), 
                testData->particleSatisfyConstraintOffsets, &errorCode);
            cluClErrorTest(errorCode);
        }
#else
        {
            kzUint size = PARTICLE_COUNT * VERTEX_DATA_SIZE * sizeof(kzFloat);
            result = kzcMemoryAllocPointer(memoryManager, &testData->vboBuffer, size, "VBO Positions");
            kzsErrorForward(result);            
        }
#endif
    }

    testData->workGroupSize[0] = 1;
    testData->workGroupSize[1] = 1;

    /* Run simulation couple of steps to relax sim. */
    {
        kzUint i;
        for(i = 0; i < 10; ++i)
        {
            softBodySceneUpdate(framework, 1, scene);
        }
    }

    kzsSuccess();
}

kzsError softBodyUpdateCamera(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct SoftBodyTestState *testData;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    /* Cameras. */
    {
        const kzUint framesPerCut = 300;
        kzUint frame = bfSceneGetFrameInitialCounterValue(scene) - bfSceneGetFrameCounterValue(scene);
        kzFloat time = (frame % framesPerCut) / (kzFloat)framesPerCut;
        kzUint cameraPath = (3 + (frame / framesPerCut) * 11) % 6;
        kzUint camera = cameraPath % 3;
        struct KzuAnimationPlayer* player = kzuSceneGetAnimationPlayer(testData->scene);
        kzuAnimationPlayerSetPaused(player, KZ_TRUE);
        kzuAnimationPlayerSetTime(player, (cameraPath > 3 ? 9.0f : 0.0f) + time * 9.0f);
        kzuAnimationPlayerUpdate(player, 0.0f);
        kzuSceneSetViewCamera(testData->scene, testData->cameras[camera]);
    }
    kzsSuccess();
}

KZ_CALLBACK kzsError softBodySceneRender(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct SoftBodyTestState *testData;
    kzsError result;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    result = softBodyUpdateCamera(framework, scene);
    kzsErrorForward(result);

    kzsSuccess();
}

KZ_CALLBACK kzsError softBodySceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct SoftBodyTestState *testData;
    kzsError result;
    cl_event curEvent;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    /* Step simulation. */
    {
        result = softBodyAccumulateForces(framework, testData);
        kzsErrorForward(result);
        result = softBodySatisfyConstraints(framework, scene, testData);
        kzsErrorForward(result);
        result = softBodyIntegrate(framework, testData);
        kzsErrorForward(result);
    }

    {
        static kzUint timer = 0;
        timer += deltaTime;

        result = kzuObjectNodeSetIdentityTransformation((struct KzuObjectNode*)testData->dynamicMesh);
        kzsErrorForward(result);
        dynamicMeshSetTime(testData->dynamicMesh, kzsSinf(timer * 0.001f) * 2.0f + 2.0f);
    }

#ifdef CPU    
    {
        kzUint i;

        for(i = 0; i < INDEX_COUNT / 3; ++i)
        {
            unsigned int index1 = testData->indices[i * 3 + 0];
            struct KzcVector4 p1 = testData->particlePositions[index1];
            unsigned int index2 = testData->indices[i * 3 + 1];
            struct KzcVector4 p2 = testData->particlePositions[index2];
            unsigned int index3 = testData->indices[i * 3 + 2];
            struct KzcVector4 p3 = testData->particlePositions[index3];

            struct KzcVector4 v1;
            struct KzcVector4 v2;
            struct KzcVector4 normal;
            kzcVector4Subtract(&p1, &p3, &v1);
            kzcVector4Subtract(&p1, &p2, &v1);
            kzcVector4Cross(&v1, &v2, &normal);
            kzcVector4Normalize(&normal);

            testData->faceNormal[i] = normal;
        }

        for(i = 0; i < PARTICLE_COUNT; ++i)
        {
            kzUint n;
            kzUint offset = i * 8;
            struct KzcVector4 normal = kzcVector4(0.0f, 0.0f, 0.0f, 0.0f);
            for(n = 0; n < 8; ++n)
            {
                kzUint faceIndex = testData->particleFaceIndexes[offset + n];
                if(faceIndex != -1)
                {
                    struct KzcVector4 faceNormal = testData->faceNormal[faceIndex];
                    kzcVector4Add(&normal, &faceNormal, &normal);
                }
            }

            kzcVector4Normalize(&normal);
            
            /* Set the position to VBO. */
            {
                kzUint vboOffset = i * VERTEX_DATA_SIZE;
                testData->vboBuffer[vboOffset + 0] = testData->particlePositions[i].data[0];
                testData->vboBuffer[vboOffset + 1] = testData->particlePositions[i].data[1];
                testData->vboBuffer[vboOffset + 2] = testData->particlePositions[i].data[2];
                testData->vboBuffer[vboOffset + 3] = 1.0f;
                testData->vboBuffer[vboOffset + 4] = normal.data[0];
                testData->vboBuffer[vboOffset + 5] = normal.data[1];
                testData->vboBuffer[vboOffset + 6] = normal.data[2];
                testData->vboBuffer[vboOffset + 7] = 0.0f;
            }
        }

#if 0
        for(i = 0; i < PARTICLE_COUNT; ++i)
        {
            kzUint offset = i * VERTEX_DATA_SIZE;
            testData->vboBuffer[offset + 0] = testData->particlePositions[i].data[0];
            testData->vboBuffer[offset + 1] = testData->particlePositions[i].data[1];
            testData->vboBuffer[offset + 2] = testData->particlePositions[i].data[2];
            testData->vboBuffer[offset + 3] = 1.0f;
            /* Reset normals. */
            testData->vboBuffer[offset + 4] = 0.0f;
            testData->vboBuffer[offset + 5] = 0.0f;
            testData->vboBuffer[offset + 6] = 0.0f;
            testData->vboBuffer[offset + 7] = 0.0f;
        }

        for(i = 0; i < INDEX_COUNT / 3; ++i)
        { 
            unsigned int index1 = testData->indices[i * 3 + 0];
            struct KzcVector4 p1 = testData->particlePositions[index1];
            unsigned int index2 = testData->indices[i * 3 + 1];
            struct KzcVector4 p2 = testData->particlePositions[index2];
            unsigned int index3 = testData->indices[i * 3 + 2];
            struct KzcVector4 p3 = testData->particlePositions[index3];

            struct KzcVector4 v1;
            struct KzcVector4 v2;
            struct KzcVector4 normal;
            kzcVector4Subtract(&p1, &p3, &v1);
            kzcVector4Subtract(&p1, &p2, &v1);
            kzcVector4Cross(&v1, &v2, &normal);
            kzcVector4Normalize(&normal);

            {
                unsigned int i1 = index1 * VERTEX_DATA_SIZE;
                testData->vboBuffer[i1 + 4] += normal.data[0];
                testData->vboBuffer[i1 + 5] += normal.data[1];
                testData->vboBuffer[i1 + 6] += normal.data[2];
                testData->vboBuffer[i1 + 7] += 1.0f;
            }

            {
                unsigned int i2 = index2 * VERTEX_DATA_SIZE;
                testData->vboBuffer[i2 + 4] += normal.data[0];
                testData->vboBuffer[i2 + 5] += normal.data[1];
                testData->vboBuffer[i2 + 6] += normal.data[2];
                testData->vboBuffer[i2 + 7] += 1.0f;
            }

            {
                unsigned int i3 = index3 * VERTEX_DATA_SIZE;    
                testData->vboBuffer[i3 + 4] += normal.data[0];
                testData->vboBuffer[i3 + 5] += normal.data[1];
                testData->vboBuffer[i3 + 6] += normal.data[2];
                testData->vboBuffer[i3 + 7] += 1.0f;
            }
        }
#endif
        /* Average normals. */
        for(i = 0; i < PARTICLE_COUNT; ++i)
        {
            kzUint offset = i * VERTEX_DATA_SIZE;
            struct KzcVector3 normal = kzcVector3(testData->vboBuffer[offset + 4], 
                testData->vboBuffer[offset + 5], testData->vboBuffer[offset + 6]);
            kzcVector3Normalize(&normal);

            testData->vboBuffer[offset + 4] = normal.data[0];
            testData->vboBuffer[offset + 5] = normal.data[1];
            testData->vboBuffer[offset + 6] = normal.data[2];
            testData->vboBuffer[offset + 7] = 0.0f;
        }

        glBindBuffer(GL_ARRAY_BUFFER, testData->vertexBufferObjectHandle);
        glBufferSubData(GL_ARRAY_BUFFER, 0, PARTICLE_COUNT * VERTEX_DATA_SIZE * sizeof(float), testData->vboBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
#else
    {
        cl_int clResult;
        {  
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
            size_t sizeFaces[2];
            size_t sizeVBO[2];
            
            sizeFaces[0] = INDEX_COUNT/3;
            sizeFaces[1] = 1;
            sizeVBO[0] = PARTICLE_COUNT;
            sizeVBO[1] = 1;

            clResult = clEnqueueAcquireGLObjects(testData->queue, 1, &testData->vertexBufferObjectCLBuffer, 0, 0, 0);
            cluClErrorTest(clResult);

            /* Reset face normals. */
            result = cluSetKernelArguments(testData->softBodyKernels[6], 
                sizeof(cl_mem), &testData->faceNormalBuffer);
            kzsErrorForward(result);
            clResult = clEnqueueNDRangeKernel(testData->queue, testData->softBodyKernels[6], 2, NULL, sizeFaces, NULL, 0, 0, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "resetFaceNormals", curEvent);

            /* Calculate face normals. */
            result = cluSetKernelArguments(testData->softBodyKernels[4], 
                sizeof(cl_mem), &testData->vboIndexBufferCl,
                sizeof(cl_mem), &testData->particlePositionsBuffer,
                sizeof(cl_mem), &testData->faceNormalBuffer);
            kzsErrorForward(result);            
            clResult = clEnqueueNDRangeKernel(testData->queue, testData->softBodyKernels[4], 2, NULL, sizeFaces, NULL, 0, 0, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "applyForces", curEvent);

            /* Average face normals to vertex's. */
            result = cluSetKernelArguments(testData->softBodyKernels[5],
                sizeof(cl_mem), &testData->vertexBufferObjectCLBuffer,
                sizeof(cl_mem), &testData->faceNormalBuffer,
                sizeof(cl_mem), &testData->particleFaceIndexBuffer);
            kzsErrorForward(result);
            clResult = clEnqueueNDRangeKernel(testData->queue, testData->softBodyKernels[5], 2, NULL, sizeVBO, NULL, 0, 0, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "averageFaceNormals", curEvent);

            result = cluSetKernelArguments(testData->softBodyKernels[0],
                sizeof(cl_mem), &testData->particlePositionsBuffer,
                sizeof(cl_mem), &testData->vertexBufferObjectCLBuffer);
            kzsErrorForward(result); 
            
            clResult = clEnqueueNDRangeKernel(testData->queue, testData->softBodyKernels[0], 2, NULL, sizeVBO, NULL, 0, 0, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "updateVBO", curEvent);

            clResult = clEnqueueReleaseGLObjects(testData->queue, 1, &testData->vertexBufferObjectCLBuffer, 0, KZ_NULL, KZ_NULL);
            cluClErrorTest(clResult);
#endif
            clResult = clFinish(testData->queue);
            cluClErrorTest(clResult);
        }
    }
#endif

    kzsSuccess();
}

KZ_CALLBACK kzsError softBodySceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct SoftBodyTestState *testData;
    kzsError result;
    cl_int clResult;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    result = bfUtilFreeLocalWorkgroupSize(testData->localWorkSize);
    kzsErrorForward(result);

    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

#ifndef CPU    /* Free CL buffers. */
    {
        kzUint i;
        for(i = 0; i < testData->softBodyKernelCount; ++i)
        {
            clResult = clReleaseKernel(testData->softBodyKernels[i]);
            cluClErrorTest(clResult);
        }
        clResult = clReleaseProgram(testData->softBodyProgram);
        cluClErrorTest(clResult);
        clResult = clReleaseMemObject(testData->particlePositionsBuffer);
        cluClErrorTest(clResult);
        clResult = clReleaseMemObject(testData->particleOldPositionsBuffer);
        cluClErrorTest(clResult);
        clResult = clReleaseMemObject(testData->particleForcesBuffer);
        cluClErrorTest(clResult);
        clResult = clReleaseMemObject(testData->particleNeighbourIndexesBuffer);
        cluClErrorTest(clResult);
        clResult = clReleaseMemObject(testData->faceNormalBuffer);
        cluClErrorTest(clResult);
        clResult = clReleaseMemObject(testData->particleFaceIndexBuffer);
        cluClErrorTest(clResult);
        clResult = clReleaseMemObject(testData->particleSatisfyConstraintOffsetBuffer);
        cluClErrorTest(clResult);
#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
        clResult = clReleaseMemObject(testData->vboIndexBufferCl);
        cluClErrorTest(clResult);
        clResult = clReleaseMemObject(testData->vertexBufferObjectCLBuffer);
        cluClErrorTest(clResult);
        glDeleteBuffers(1, &testData->vertexBufferObjectHandle);
        glDeleteBuffers(1, &testData->vboIndexBufferHandle);
#endif

        clResult = clReleaseCommandQueue(testData->queue);
        cluClErrorTest(clResult);
    }
#endif

    result = kzcMemoryFreeArray(testData->softBodyKernels);
    kzsErrorForward(result);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    result = kzcMemoryFreePointer(testData->indices);
    kzsErrorForward(result);
#endif

    result = kzcMemoryFreePointer(testData->particlePositions);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(testData->particleOldPositions);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(testData->particleForces);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(testData->faceNormal);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(testData->particleNeighbourIndexes);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(testData->particleFaceIndexes);
    kzsErrorForward(result);
    result = kzcMemoryFreePointer(testData->particleSatisfyConstraintOffsets);
    kzsErrorForward(result);    

    result = kzuSceneRemoveObject(kzaApplicationGetScene(bfGetApplication(framework)), (struct KzuObjectNode*)testData->dynamicMesh);
    kzsErrorForward(result);
    result = dynamicMeshNodeDelete(testData->dynamicMesh);
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzsError softBodyCreate(struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct SoftBodyTestState *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, softBodySceneLoad, softBodySceneUpdate, softBodySceneRender,
        softBodySceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, softBodyScenePreUpdate, softBodyScenePostUpdate, &configuration);
    kzsErrorForward(result);
    
    result = kzcMemoryAllocVariable(memoryManager, testData, "Soft body test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Soft Body Test", "Physics", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;
    kzsSuccess();
}

kzsError softBodyScenePreUpdate(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsSuccess();
}

kzsError softBodyScenePostUpdate(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    kzsSuccess();
}
