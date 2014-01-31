/**
* \file
* OpenCL benchmark fluid test.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "cl_fluid.h"

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>
#include <benchmarkutil/util/bf_file_validator.h>
#include <benchmarkutil/settings/bf_settings.h>

#include <clutil/clu_floatbuffer.h>
#include <clutil/clu_platform.h>
#include <clutil/clu_program.h>
#include <clutil/clu_kernel.h>
#include <clutil/clu_util.h>
#include <clutil/clu_profiler.h>

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
#include <core/util/io/kzc_file.h>
#include <core/util/string/kzc_string.h>

#include <system/display/kzs_window.h>
#include <system/display/kzs_surface.h>
#include <system/wrappers/kzs_memory.h>
#include <system/wrappers/kzs_opengl.h>

#include <clutil/clu_opencl_base.h>

#include <clmark/test_definitions.h>

/** Use CPU to calculate the fluid. */
/*#define CPU 1*/

/** Size of the fluid grid. This value is read from configuration file. */
kzInt FLUID_GRID_SIZE;
#define N (FLUID_GRID_SIZE - 2)


#ifdef CPU
/** Finds index for given x,y coordinates */
#define INDEX(i,j) ((i)+(N+2)*(j))
/** Helper for looping the buffers. */
#define FOR_EACH_CELL for(i = 1; i <= N; ++i) { for(j=1; j <= N; ++j) {
/** Helper for looping the buffers. */
#define END_FOR }}
#endif
/** Swaps buffers. */
#define SWAP(x0,x) {struct FloatBuffer* tmp = x0; x0 = x; x = tmp;}


const kzUint NEW_DENSITIES_BUFFER_X = 0; /**< X coordinate in new densities buffer. */
const kzUint NEW_DENSITIES_BUFFER_Y = 1; /**< Y coordinate in new densities buffer. */
const kzUint NEW_DENSITIES_BUFFER_VALUE = 2; /**< Y coordinate in new densities buffer. */
const kzUint NEW_DENSITIES_BUFFER_ITEM_SIZE = 3; /**< Size of one item in new densities buffer. */
const kzUint NEW_DENSITIES_BUFFER_ITEMS = 20; /**< Maximum number of items in new densities buffer. */

const kzUint NEW_VELOCITIES_BUFFER_X = 0; /**< X coordinate in new velocities buffer. */
const kzUint NEW_VELOCITIES_BUFFER_Y = 1; /**< Y coordinate in new velocities buffer. */
const kzUint NEW_VELOCITIES_BUFFER_VEL_X = 2; /**< X velocity in new velocities buffer. */
const kzUint NEW_VELOCITIES_BUFFER_VEL_Y = 3; /**< Y velocity in new velocities buffer. */
const kzUint NEW_VELOCITIES_BUFFER_ITEM_SIZE = 4; /**< Size of item in new velocities buffer. */
const kzUint NEW_VELOCITIES_BUFFER_ITEMS = 20; /**< Maximum number of items velocities buffer. */


/** Amount of kernels in the application. */
const kzUint kernelCount = 9;


/* Internal data of fluid test */
struct FluidTestState
{
    cl_program fluidProgram; /**< CL program containing the kernels. */
    cl_kernel* fluidKernels; /**< CL kernels. */

    cl_mem outputBuffer; /**< Buffer used to output to gl texture. */
    cl_command_queue queue; /**< Command queue. */
    cl_device_id device; /**< Device id. */

    struct KzcTexture* texture; /**< Texture used to render results. */
    kzByte* textureData; /**< Texture data of the texture. */

    struct FloatBuffer* u; /**< Velocity buffer for horizontal movement. */
    struct FloatBuffer* v; /**< Velocity buffer for vertical movement. */
    struct FloatBuffer* uPrevious; /**< Velocity buffer for last horizontal movement. */
    struct FloatBuffer* vPrevious; /**< Velocity buffer for last vertical movement. */
    struct FloatBuffer* density; /**< Density buffer. */
    struct FloatBuffer* densityPrevious; /**< Previous density buffer. */

    struct FloatBuffer* linTempBuffer;

    struct FloatBuffer* newVelocities; /**< Buffer used to add new forces to the buffers. */
    struct FloatBuffer* newDensities; /**< Buffer used to add new densities to the buffers. */

    kzUint bufferSize; /**< Size of the buffers. */

    size_t image_size[3]; /**< Image size for buffers. */
    size_t* localWorkSize; /**< Local work size. Read from configuration file. */

    struct CluProfiler *profiler; /**< Clu profiler */    

    kzFloat addedDensity; /**< Amount of density added for each addition step. */
};


kzsError fluidSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene);
kzsError fluidSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene);
kzsError fluidSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene);
static kzsError diffuse(const struct FluidTestState *testData, kzInt b, struct FloatBuffer* x, struct FloatBuffer* x0, kzFloat diff, kzFloat dt);
static kzsError diffuseSolve(const struct FluidTestState* testData, kzInt b, struct FloatBuffer* x, struct FloatBuffer* x0, kzFloat a, kzFloat c);
static kzsError advect(const struct FluidTestState *testData, kzInt b, struct FloatBuffer* d, struct FloatBuffer* d0, struct FloatBuffer* u, struct FloatBuffer* v, kzFloat dt);
static kzsError project(const struct FluidTestState *testData, struct FloatBuffer* u, struct FloatBuffer* v, struct FloatBuffer* p, struct FloatBuffer* div);
static kzsError densityStep(const struct FluidTestState *testData, struct FloatBuffer* density, struct FloatBuffer* x0, struct FloatBuffer* u, struct FloatBuffer* v, kzFloat diff, kzFloat dt);
static kzsError velocityStep(const struct FluidTestState *testData, struct FloatBuffer* u, struct FloatBuffer* v, struct FloatBuffer* u0, struct FloatBuffer* v0, kzFloat visc, kzFloat dt);
static kzsError applyForces(const struct FluidTestState *testData, kzUint frame, kzUint frameCountTotal, struct FloatBuffer* d, struct FloatBuffer* u, struct FloatBuffer* v);
static kzsError addSource(const struct FluidTestState *testData, struct FloatBuffer* x, struct FloatBuffer* s, kzFloat dt);
static kzsError setBounds(const struct FluidTestState *testData, kzInt b, struct FloatBuffer* x);


static kzsError addSource(const struct FluidTestState *testData, struct FloatBuffer* x, struct FloatBuffer* s, kzFloat dt)
{

#ifdef CPU
    kzInt i;
    kzInt size = (N + 2) * (N + 2);
    for(i = 0; i < size; ++i)
    {
        x->data[i] += dt * s->data[i];
    }
#else

    kzsError result;
    cl_int clResult;
    cl_event curEvent;

    result = cluSetKernelArguments(testData->fluidKernels[5],
        sizeof(cl_mem), &x->buffer,
        sizeof(cl_mem), &s->buffer,
        sizeof(cl_float), &dt);
    kzsErrorForward(result);

    clResult = clEnqueueNDRangeKernel(testData->queue, testData->fluidKernels[5], 2, NULL, testData->image_size, testData->localWorkSize, 0, NULL, &curEvent);
    cluClErrorTest(clResult);
    cluProfilerAddEvent(testData->profiler, "addSource", curEvent);

#endif

    kzsSuccess();
}

static kzsError setBounds(const struct FluidTestState *testData, kzInt b, struct FloatBuffer* x)
{
#ifdef CPU
    kzUint i;

    for(i=1 ; i<=N ; i++)
    {
        x->data[INDEX(0, i)] = b==1 ? -x->data[INDEX(1,i)] : x->data[INDEX(1,i)];
        x->data[INDEX(N+1, i)] = b==1 ? -x->data[INDEX(N,i)] : x->data[INDEX(N,i)];
        x->data[INDEX(i, 0)] = b==2 ? -x->data[INDEX(i,1)] : x->data[INDEX(i,1)];
        x->data[INDEX(i, N+1)] = b==2 ? -x->data[INDEX(i,N)] : x->data[INDEX(i,N)];
    }
    x->data[INDEX(0, 0)] = 0.5f * (x->data[INDEX(1,0 )]+x->data[INDEX(0  ,1)]);
    x->data[INDEX(0, N+1)] = 0.5f * (x->data[INDEX(1,N+1)]+x->data[INDEX(0  ,N)]);
    x->data[INDEX(N+1, 0)] = 0.5f * (x->data[INDEX(N,0 )]+x->data[INDEX(N+1,1)]);
    x->data[INDEX(N+1, N+1)] = 0.5f * (x->data[INDEX(N,N+1)]+x->data[INDEX(N+1,N)]);

#else
    {
        cl_int clResult;
        kzsError result;
        size_t n = N;
        size_t n2 = 1;
        cl_event curEvent;
        result = cluSetKernelArguments(testData->fluidKernels[6], 
            sizeof(cl_mem), &x->buffer,
            sizeof(cl_int), &b
            );
        kzsErrorForward(result);
        result = cluSetKernelArguments(testData->fluidKernels[8], 
            sizeof(cl_mem), &x->buffer,
            sizeof(cl_int), &b
            );
        kzsErrorForward(result);

        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fluidKernels[6], 1, NULL, &n, NULL, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "setBounds", curEvent);
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fluidKernels[8], 1, NULL, &n2, NULL, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "setCornerBounds", curEvent);
    }
#endif

    kzsSuccess();
}

static kzsError diffuseSolve(const struct FluidTestState* testData, kzInt b, struct FloatBuffer* x, struct FloatBuffer* x0, kzFloat a, kzFloat c)
{
    kzsError result;
    kzInt k;

    for(k=0; k < 4; k++)
    {
#ifdef CPU
        kzUint i, j;
        FOR_EACH_CELL
            x->data[INDEX(i,j)] = (x0->data[INDEX(i,j)] + a*(x->data[INDEX(i-1,j)]+x->data[INDEX(i+1,j)]+x->data[INDEX(i,j-1)]+x->data[INDEX(i,j+1)]))/c;
        END_FOR
#else
        {
            cl_int clResult;

            size_t n2 [2] = {0};
            size_t offset[2] = {1, 1};
            cl_event curEvent;
            n2[0] = N;
            n2[1] = N;

            result = cluSetKernelArguments(testData->fluidKernels[1], 
                sizeof(cl_mem), &x->buffer,
                sizeof(cl_mem), &x0->buffer,
                sizeof(cl_mem), &testData->linTempBuffer->buffer,
                sizeof(cl_float), &a,
                sizeof(cl_float), &c
                );
            kzsErrorForward(result);

            clResult = clEnqueueNDRangeKernel(testData->queue, testData->fluidKernels[1], 2, offset, n2, NULL, 0, NULL, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "linSolve", curEvent);
            result = setBounds(testData, b, testData->linTempBuffer); 
            kzsErrorForward(result);
            result = cluSetKernelArguments(testData->fluidKernels[1], 
                sizeof(cl_mem), &testData->linTempBuffer->buffer,
                sizeof(cl_mem), &x0->buffer,
                sizeof(cl_mem), &x->buffer,
                sizeof(cl_float), &a,
                sizeof(cl_float), &c
                );
            kzsErrorForward(result);

            clResult = clEnqueueNDRangeKernel(testData->queue, testData->fluidKernels[1], 2, offset, n2, NULL, 0, NULL, &curEvent);
            cluClErrorTest(clResult);
            cluProfilerAddEvent(testData->profiler, "linSolve", curEvent);
        }
#endif

        result = setBounds(testData, b, x); 
        kzsErrorForward(result);
    }
    kzsSuccess();
}

static kzsError diffuse(const struct FluidTestState *testData, kzInt b, struct FloatBuffer* x, struct FloatBuffer* x0, kzFloat diff, kzFloat dt)
{
    kzsError result;
    
    kzFloat a = dt * diff * N * N;
    result = diffuseSolve(testData, b, x, x0, a, 1 + 4 * a);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError advect(const struct FluidTestState *testData, kzInt b, struct FloatBuffer* d, struct FloatBuffer* d0, struct FloatBuffer* u, struct FloatBuffer* v, kzFloat dt)
{
    kzsError result;
    kzFloat dt0 = dt*N;

#ifdef CPU
    {
        kzUint i, j;
        kzInt i0, j0, i1, j1;
        kzFloat x, y, s0, t0, s1, t1;

        FOR_EACH_CELL
            x = i-dt0*u->data[INDEX(i,j)];
            y = j-dt0*v->data[INDEX(i,j)];
            if(x < 0.5f) x=0.5f; 
            if(x > N+0.5f) x=N+0.5f; 
            i0=(int)x;
            i1=i0+1;
            if(y < 0.5f) y=0.5f; 
            if(y > N+0.5f) y=N+0.5f; 
            j0=(int)y; 
            j1=j0+1;
            s1 = x-i0; 
            s0 = 1-s1; 
            t1 = y-j0; 
            t0 = 1-t1;
            d->data[INDEX(i,j)] = s0*(t0*d0->data[INDEX(i0,j0)] + t1*d0->data[INDEX(i0,j1)]) + s1*(t0*d0->data[INDEX(i1,j0)] + t1*d0->data[INDEX(i1,j1)]);
        END_FOR
    }
#else
    {  
        cl_int clResult;
        cl_event curEvent;

        result = cluSetKernelArguments(testData->fluidKernels[2],
            sizeof(cl_mem), &d->buffer,
            sizeof(cl_mem), &d0->buffer,
            sizeof(cl_mem), &u->buffer,
            sizeof(cl_mem), &v->buffer,
            sizeof(cl_float), &dt0
            );
        kzsErrorForward(result);

        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fluidKernels[2], 2, NULL, testData->image_size, testData->localWorkSize, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "advect", curEvent);
    }
#endif
        
    result = setBounds(testData, b, d);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError project(const struct FluidTestState *testData, struct FloatBuffer* u, struct FloatBuffer* v, struct FloatBuffer* p, struct FloatBuffer* d)
{
    kzsError result;
#ifdef CPU
    kzUint i, j;

    FOR_EACH_CELL
        d->data[INDEX(i,j)] = -0.5f*(u->data[INDEX(i+1,j)]-u->data[INDEX(i-1,j)]+v->data[INDEX(i,j+1)]-v->data[INDEX(i,j-1)])/N;
        p->data[INDEX(i,j)] = 0;
    END_FOR	
        
    setBounds(testData, 0, d);
    setBounds(testData, 0, p);

    diffuseSolve(testData, 0, p, d, 1, 4);

    FOR_EACH_CELL
        u->data[INDEX(i,j)] -= 0.5f*N*(p->data[INDEX(i+1,j)]-p->data[INDEX(i-1,j)]);
        v->data[INDEX(i,j)] -= 0.5f*N*(p->data[INDEX(i,j+1)]-p->data[INDEX(i,j-1)]);
    END_FOR
        
    setBounds(testData, 1, u);
    setBounds(testData, 2, v);

#else

    {
        cl_int clResult;
        size_t n2 [2] = {0};
        size_t offset[2] = {1, 1};
        cl_event curEvent;
        
        n2[0] = testData->image_size[0]-2;
        n2[1] = testData->image_size[1]-2;

        result = cluSetKernelArguments(testData->fluidKernels[3],
            sizeof(cl_mem), &d->buffer,
            sizeof(cl_mem), &u->buffer,
            sizeof(cl_mem), &v->buffer,
            sizeof(cl_mem), &p->buffer);
        kzsErrorForward(result);

        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fluidKernels[3], 2, offset, n2, NULL, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "project1", curEvent);
    }

    result = setBounds(testData, 0, d);
    kzsErrorForward(result);
    result = setBounds(testData, 0, p);
    kzsErrorForward(result);

    result = diffuseSolve(testData, 0, p, d, 1.0f, 4.0f);
    kzsErrorForward(result);

    {
        cl_int clResult;
        size_t n2 [2] = {0};
        size_t offset[2] = {1, 1};
        cl_event curEvent;
        n2[0] = testData->image_size[0]-2;
        n2[1] = testData->image_size[1]-2;

        result = cluSetKernelArguments(testData->fluidKernels[4],
            sizeof(cl_mem), &u->buffer,
            sizeof(cl_mem), &v->buffer,
            sizeof(cl_mem), &p->buffer);
        kzsErrorForward(result);

        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fluidKernels[4], 2, offset, n2, NULL, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "project2" , curEvent);
    }

    result = setBounds(testData, 1, u);
    kzsErrorForward(result);
    result = setBounds(testData, 2, v);
    kzsErrorForward(result);
#endif

    kzsSuccess();
}

static kzsError densityStep(const struct FluidTestState *testData, struct FloatBuffer* density, struct FloatBuffer* x0, struct FloatBuffer* u, struct FloatBuffer* v, kzFloat diff, kzFloat dt)
{
    kzsError result;
    
    result = addSource(testData, density, x0, dt);
    kzsErrorForward(result);
    SWAP(x0, density);
    result = diffuse(testData, 0, density, x0, diff, dt);
    kzsErrorForward(result);
    SWAP(x0, density);
    result = advect(testData, 0, density, x0, u, v, dt);
    kzsErrorForward(result);

    kzsSuccess();
}

static kzsError velocityStep(const struct FluidTestState *testData, struct FloatBuffer* u, struct FloatBuffer* v, struct FloatBuffer* u0, struct FloatBuffer* v0, kzFloat visc, kzFloat dt)
{
    kzsError result;

    result = addSource(testData, u, u0, dt); 
    kzsErrorForward(result);
    result = addSource(testData, v, v0, dt);
    kzsErrorForward(result);

    SWAP(u0, u);

    result = diffuse(testData, 1, u, u0, visc, dt);
    kzsErrorForward(result);

    SWAP(v0, v);

    result = diffuse(testData, 2, v, v0, visc, dt);
    kzsErrorForward(result);
    result = project(testData, u, v, u0, v0);
    kzsErrorForward(result);

    SWAP(u0, u);
    SWAP(v0, v);

    result = advect(testData, 1, u, u0, u0, v0, dt);
    kzsErrorForward(result);
    result = advect(testData, 2, v, v0, u0, v0, dt);
    kzsErrorForward(result);
    result = project(testData, u, v, u0, v0);
    kzsErrorForward(result);
    
    kzsSuccess();
}

static kzsError applyForces(const struct FluidTestState *testData, kzUint frame, kzUint frameCountTotal, struct FloatBuffer* d, struct FloatBuffer* u, struct FloatBuffer* v)
{
    kzUint i, j, n;
    kzUint maxCount;
    kzFloat forceModifier = 1.0f;
    const kzUint boundaryFrame = 100;

    srand(frame * 7);

    for(i = 0; i < NEW_VELOCITIES_BUFFER_ITEMS; i++)
    {
        const kzUint offset = i * NEW_VELOCITIES_BUFFER_ITEM_SIZE;
        testData->newVelocities->data[offset/*+ NEW_VELOCITIES_BUFFER_X*/] = -1.0f;
        testData->newVelocities->data[offset  + NEW_VELOCITIES_BUFFER_Y  ] = -1.0f;
    }

    for(i = 0; i < NEW_DENSITIES_BUFFER_ITEMS; i++)
    {
        const kzUint offset = i * NEW_DENSITIES_BUFFER_ITEM_SIZE;
        testData->newDensities->data[offset/*+ NEW_VELOCITIES_BUFFER_X*/] = -1.0f;
        testData->newDensities->data[offset  + NEW_VELOCITIES_BUFFER_Y  ] = -1.0f;
    }


    maxCount = 10;
    if(frame > boundaryFrame)
    {       
        kzUint set = kzsMinU(9, 1 + (frameCountTotal - frame) / 40);
        kzUint seed = 1 + (frameCountTotal - frame) / 20;
        srand(75 + seed);
        maxCount = set;
        forceModifier = 0.1f;
    }
    for(n = 0; n < maxCount; n++)
    {
        const kzUint offsetDensity = n * NEW_DENSITIES_BUFFER_ITEM_SIZE;
        const kzUint offsetVelocity = n * NEW_VELOCITIES_BUFFER_ITEM_SIZE;
        i = (kzUint)(kzInt)(rand() % FLUID_GRID_SIZE);
        j = (kzUint)(kzInt)(rand() % FLUID_GRID_SIZE);

        if(!(i<1 || i>(kzUint)N || j<1 || j>(kzUint)N))
        {
            kzFloat force = ((frame % 250) + 150) + (kzFloat)(rand()%15) * 50.f;
            force *= forceModifier;
            testData->newDensities->data[offsetDensity/*+NEW_DENSITIES_BUFFER_X*/] = (kzFloat)i;
            testData->newDensities->data[offsetDensity + NEW_DENSITIES_BUFFER_Y] = (kzFloat)j;
            testData->newDensities->data[offsetDensity + NEW_DENSITIES_BUFFER_VALUE] = testData->addedDensity;

            if(n < 6 || frame > boundaryFrame)
            {
                testData->newVelocities->data[offsetVelocity/*+NEW_VELOCITIES_BUFFER_X*/] = (kzFloat)i;
                testData->newVelocities->data[offsetVelocity + NEW_VELOCITIES_BUFFER_Y] = (kzFloat)j;
                testData->newVelocities->data[offsetVelocity + NEW_VELOCITIES_BUFFER_VEL_X] = (kzFloat)(force * (-5+rand()%10));
                testData->newVelocities->data[offsetVelocity + NEW_VELOCITIES_BUFFER_VEL_Y] = (kzFloat)(force * (-5+rand()%10));
            }
        }
    }

#ifdef CPU
    {
        kzUint size = (N+2)*(N+2);
        for(i = 0; i < size; ++i) 
        {
            u->data[i] = 0.0f;
            v->data[i] = 0.0f;
            d->data[i] = 0.0f;
        }

        for(i = 0; i < NEW_VELOCITIES_BUFFER_ITEMS; i++)
        {
            kzUint offset = i * NEW_VELOCITIES_BUFFER_ITEM_SIZE;
            kzUint x = testData->newVelocities->data[offset + NEW_VELOCITIES_BUFFER_X];
            kzUint y = testData->newVelocities->data[offset + NEW_VELOCITIES_BUFFER_Y];
            if(x > 0 && y > 0)
            {
                kzInt id = INDEX(x, y);
                u->data[id] = testData->newVelocities->data[offset + NEW_VELOCITIES_BUFFER_VEL_X];
                v->data[id] = testData->newVelocities->data[offset + NEW_VELOCITIES_BUFFER_VEL_Y];
            }
        }

        for(i = 0; i < NEW_DENSITIES_BUFFER_ITEMS; i++)
        {
            kzUint offset = i * NEW_DENSITIES_BUFFER_ITEM_SIZE;
            kzUint x = testData->newDensities->data[offset + NEW_DENSITIES_BUFFER_X];
            kzUint y = testData->newDensities->data[offset + NEW_DENSITIES_BUFFER_Y];
            if(x > 0 && y > 0)
            {
                kzInt id = INDEX(x, y);
                d->data[id] = testData->newDensities->data[offset + NEW_DENSITIES_BUFFER_VALUE];
            }
        }
    }
#else

#if 1
    {
        kzsError result;
        cl_int clResult;
        cl_event curEvent;

        clResult = clEnqueueWriteBuffer(testData->queue, testData->newDensities->buffer, CL_FALSE, 0, testData->newDensities->size, testData->newDensities->data, 0, KZ_NULL, KZ_NULL);
        cluClErrorTest(clResult);
        clResult = clEnqueueWriteBuffer(testData->queue, testData->newVelocities->buffer, CL_FALSE, 0, testData->newVelocities->size, testData->newVelocities->data, 0, KZ_NULL, KZ_NULL);
        cluClErrorTest(clResult);

        result = cluSetKernelArguments(testData->fluidKernels[7], 
            sizeof(cl_mem), &d->buffer,
            sizeof(cl_mem), &u->buffer,
            sizeof(cl_mem), &v->buffer,
            sizeof(cl_mem), &testData->newDensities->buffer,
            sizeof(cl_mem), &testData->newVelocities->buffer
            );
        kzsErrorForward(result);

        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fluidKernels[7], 2, NULL, testData->image_size, testData->localWorkSize, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "addForcesAndDensity", curEvent);
    }
#else
    {
        cl_int result;

        result = clEnqueueWriteBuffer(testData->queue, testData->uPrevious->buffer, CL_TRUE, 0, testData->uPrevious->size, testData->uPrevious->data, 0, KZ_NULL, KZ_NULL);
        cluClErrorTest(result);
        result = clEnqueueWriteBuffer(testData->queue, testData->vPrevious->buffer, CL_TRUE, 0, testData->vPrevious->size, testData->vPrevious->data, 0, KZ_NULL, KZ_NULL);
        cluClErrorTest(result);
        result = clEnqueueWriteBuffer(testData->queue, testData->densityPrevious->buffer, CL_TRUE, 0, testData->densityPrevious->size, testData->densityPrevious->data, 0, KZ_NULL, KZ_NULL);
        cluClErrorTest(result);
    }
#endif

#endif


    kzsSuccess();
}

KZ_CALLBACK kzsError fluidSceneLoad(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct FluidTestState *testData;
    struct KzcMemoryManager* memoryManager = kzcMemoryGetManager(framework);
    kzsError result;
    cl_int clResult;
    cl_context context;

    result = kzaApplicationSetScenePath(bfGetApplication(framework), "Scenes/Fluid");
    kzsErrorForward(result);

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));
    bfSceneSetFrameCounter(scene, FLUID_FRAME_COUNT);

    {
        kzBool fullProfile;
        result = bfGetCLFullProfile(framework, &fullProfile);
        kzsErrorForward(result);

        if(fullProfile)
        {
            FLUID_GRID_SIZE = 1024;
            testData->addedDensity = 250.0f;
        }
        else
        {
            FLUID_GRID_SIZE = 256;
            testData->addedDensity = 50.0f;
        }
    }

    testData->image_size[0] = (kzUint)FLUID_GRID_SIZE;
    testData->image_size[1] = (kzUint)FLUID_GRID_SIZE;
    testData->image_size[2] = 1;

    context = bfGetClContext(framework);
    
    clResult = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &testData->device, NULL);
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

        testData->queue = clCreateCommandQueue(context, testData->device, cqProperties, &clResult);
        cluClErrorTest(clResult);
    }


    result = bfUtilReadLocalWorkgroupSize(framework, "FluidLocalWorkSize", 2, &testData->localWorkSize);
    kzsErrorForward(result);
    testData->profiler = bfSceneGetProfiler(scene);

    {
        kzString kernelPath =  "data/fluid.cl";
        kzString compilerSettings = COMPILER_FLAGS;
        kzBool binaryProgramExists;
        
        result = cluGetProgramBinaryExists(memoryManager, kernelPath, &binaryProgramExists);
        kzsErrorForward(result);

        if(binaryProgramExists)
        {
            kzBool binary;
            result = cluLoadProgramFromFile(memoryManager, kernelPath, KZ_FALSE, context, &testData->fluidProgram, &binary);
            kzsErrorForward(result);
            bfSceneSetUseProgramBinary(scene, KZ_TRUE);
        }
        else
        {        
            /* Append define for GRID_SIZE to fluid.cl. */
            kzMutableString kernelSourceFromFile;
            kzMutableString kernelSource;
            kzBool isValid;
            result = bfValidateFile(framework, kernelPath, FLUID_HASH, &isValid);
            kzsErrorForward(result);
            bfSceneSetValidConfigurationData(scene, isValid);

            result = kzcFileReadTextFile(bfGetMemoryManager(framework), kernelPath, &kernelSourceFromFile);
            kzsErrorForward(result);
            result = kzcStringFormat(bfGetMemoryManager(framework), "#define GRID_SIZE %d\n%s", &kernelSource, FLUID_GRID_SIZE, kernelSourceFromFile);
            kzsErrorForward(result);
            
            result = cluGetBuiltProgramFromStringWithOptions(memoryManager, kernelSource, compilerSettings, context, &testData->fluidProgram);
            kzsErrorForward(result);

            result = kzcStringDelete(kernelSourceFromFile);
            kzsErrorForward(result);
            result = kzcStringDelete(kernelSource);
            kzsErrorForward(result);
        }
    }

    result = kzcMemoryAllocArray(memoryManager, testData->fluidKernels, kernelCount, "Kernel objects");
    kzsErrorForward(result);
    testData->fluidKernels[0] = clCreateKernel(testData->fluidProgram, "visualizeDensity", &clResult);
    cluClErrorTest(clResult);
    testData->fluidKernels[1] = clCreateKernel(testData->fluidProgram, "linSolve", &clResult);
    cluClErrorTest(clResult);
    testData->fluidKernels[2] = clCreateKernel(testData->fluidProgram, "advect", &clResult);
    cluClErrorTest(clResult);
    testData->fluidKernels[3] = clCreateKernel(testData->fluidProgram, "project1", &clResult);
    cluClErrorTest(clResult);
    testData->fluidKernels[4] = clCreateKernel(testData->fluidProgram, "project2", &clResult);
    cluClErrorTest(clResult);
    testData->fluidKernels[5] = clCreateKernel(testData->fluidProgram, "addSource", &clResult);
    cluClErrorTest(clResult);
    testData->fluidKernels[6] = clCreateKernel(testData->fluidProgram, "setBounds", &clResult);
    cluClErrorTest(clResult);
    testData->fluidKernels[7] = clCreateKernel(testData->fluidProgram, "addForcesAndDensity", &clResult);
    cluClErrorTest(clResult);
    testData->fluidKernels[8] = clCreateKernel(testData->fluidProgram, "setCornerBounds", &clResult);
    cluClErrorTest(clResult);

    {
        cl_context bufferContext = context;
#ifdef CPU
        bufferContext = KZ_NULL;
#endif
        testData->bufferSize = FLUID_GRID_SIZE * FLUID_GRID_SIZE;
        result = cluFloatBufferCreate(memoryManager, testData->bufferSize, bufferContext, &testData->u);
        kzsErrorForward(result);
        result = cluFloatBufferCreate(memoryManager, testData->bufferSize, bufferContext, &testData->v);
        kzsErrorForward(result);
        result = cluFloatBufferCreate(memoryManager, testData->bufferSize, bufferContext, &testData->linTempBuffer);
        kzsErrorForward(result);
        result = cluFloatBufferCreate(memoryManager, testData->bufferSize, bufferContext, &testData->uPrevious);
        kzsErrorForward(result);
        result = cluFloatBufferCreate(memoryManager, testData->bufferSize, bufferContext, &testData->vPrevious);
        kzsErrorForward(result);
        result = cluFloatBufferCreate(memoryManager, testData->bufferSize, bufferContext, &testData->density);
        kzsErrorForward(result);
        result = cluFloatBufferCreate(memoryManager, testData->bufferSize, bufferContext, &testData->densityPrevious);
        kzsErrorForward(result);
        result = cluFloatBufferCreate(memoryManager, NEW_DENSITIES_BUFFER_ITEMS * NEW_DENSITIES_BUFFER_ITEM_SIZE, bufferContext, &testData->newDensities);
        kzsErrorForward(result);
        result = cluFloatBufferCreate(memoryManager, NEW_VELOCITIES_BUFFER_ITEMS * NEW_VELOCITIES_BUFFER_ITEM_SIZE, bufferContext, &testData->newVelocities);
        kzsErrorForward(result);
    }

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    {
        const kzUint width = FLUID_GRID_SIZE;
        const kzUint height = FLUID_GRID_SIZE;
        kzUint dataSize = width * height * 4;
        struct KzcTextureDescriptor descriptor;

        result = kzcMemoryAllocArray(memoryManager, testData->textureData, dataSize, "Fluid test texture");
        kzsErrorForward(result);
        kzsMemset(testData->textureData, 0, dataSize);

        kzcTextureDescriptorSet(width, height, KZC_TEXTURE_FORMAT_RGBA, KZC_TEXTURE_FILTER_POINT_SAMPLE, KZC_TEXTURE_WRAP_CLAMP, KZC_TEXTURE_COMPRESSION_NONE, &descriptor);
        result = kzcTextureCreate(kzuProjectGetResourceManager(bfGetProject(framework)), KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, &descriptor, testData->textureData, &testData->texture);
        kzsErrorForward(result);

        testData->outputBuffer = clCreateFromGLTexture2D(context, (cl_mem_flags)CL_MEM_READ_WRITE, KZS_GL_TEXTURE_2D, 0, (GLuint)kzcTextureGetTextureHandle(testData->texture), 
            &clResult);
        cluClErrorTest(clResult);
    }
#else 
    {
        const kzUint width = (kzUint)FLUID_GRID_SIZE;
        const kzUint height = (kzUint)FLUID_GRID_SIZE;
        cl_image_format format;
        format.image_channel_order = CL_RGBA;
        format.image_channel_data_type = CL_UNORM_INT8;
        testData->outputBuffer = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &format, width, height, 0, KZ_NULL, &clResult);
        cluClErrorTest(clResult);
    }
#endif

    {
        struct KzuMaterial* material;       
        result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), "Materials/Fluid_Textured/Fluid_TexturedMaterial", &material);
        kzsErrorForward(result);

        {
            struct KzuPropertyManager* propertyManager = kzuMaterialGetPropertyManager(material);
            result = kzuPropertyManagerSetVoid(propertyManager, material, KZU_PROPERTY_TYPE_TEXTURE, testData->texture);
            kzsErrorForward(result);
        }
    }
    
    kzsSuccess();
}


KZ_CALLBACK kzsError fluidSceneUpdate(struct BenchmarkFramework* framework, kzUint deltaTime, struct BfScene* scene)
{
    struct FluidTestState *testData;
    kzsError result;
    cl_int clResult;
    cl_event curEvent;

    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    {
        kzFloat visc = 0.0f;
        kzFloat dt = 0.25f;
        kzFloat diff = 0.0f;

        result = applyForces(testData, bfSceneGetFrameCounterValue(scene), bfSceneGetFrameInitialCounterValue(scene), testData->densityPrevious, 
            testData->uPrevious, testData->vPrevious);
        kzsErrorForward(result);        
        result = velocityStep(testData, testData->u, testData->v, testData->uPrevious, testData->vPrevious, visc, dt) ;
        kzsErrorForward(result);
        result = densityStep(testData, testData->density, testData->densityPrevious, testData->u, testData->v, diff, dt);
        kzsErrorForward(result);
    }

#ifndef CPU
    {  


#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
        clResult = clEnqueueAcquireGLObjects(testData->queue, 1, &testData->outputBuffer, 0, KZ_NULL, KZ_NULL);
        cluClErrorTest(clResult);
#endif

        result = cluSetKernelArguments(testData->fluidKernels[0], 
            sizeof(cl_mem), &testData->outputBuffer,
            sizeof(cl_mem), &testData->density->buffer
            );
        kzsErrorForward(result);
        
        clResult = clEnqueueNDRangeKernel(testData->queue, testData->fluidKernels[0], 2, NULL, testData->image_size, testData->localWorkSize, 0, NULL, &curEvent);
        cluClErrorTest(clResult);
        cluProfilerAddEvent(testData->profiler, "visualizeDensity", curEvent);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE        
        clResult = clEnqueueReleaseGLObjects(testData->queue, 1, &testData->outputBuffer, 0, KZ_NULL, KZ_NULL);
        cluClErrorTest(clResult);
#endif

    }
#else
    {
        kzUint offset = 0;
        kzUint i;
        for(i = 0; i < testData->bufferSize; ++i)
        {
            kzByte density = (kzByte)kzsClampui((kzUint)(testData->density->data[i] * 255.f), 0, 255);
            testData->textureData[offset++] = density;
            testData->textureData[offset++] = density;
            testData->textureData[offset++] = density;
            testData->textureData[offset++] = density;
        }
        kzcTextureUpdateData(testData->texture, testData->textureData);
    }
#endif 

    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

    kzsSuccess();
}

KZ_CALLBACK kzsError fluidSceneUninitialize(struct BenchmarkFramework* framework, struct BfScene* scene)
{
    struct FluidTestState *testData;
    kzsError result;
    cl_int clResult;
    cl_uint i;
    testData = bfSceneGetUserData(scene);
    kzsAssert(kzcIsValidPointer(testData));

    result = bfUtilFreeLocalWorkgroupSize(testData->localWorkSize);
    kzsErrorForward(result);

    clResult = clFinish(testData->queue);
    cluClErrorTest(clResult);

    result = cluFloatBufferDelete(testData->newDensities);
    kzsErrorForward(result);
    result = cluFloatBufferDelete(testData->newVelocities);
    kzsErrorForward(result);

#if KZ_OPENGL_VERSION != KZ_OPENGL_NONE
    result = kzcMemoryFreeArray(testData->textureData);
    kzsErrorForward(result);
#endif

    {
        result = cluFloatBufferDelete(testData->u);
        kzsErrorForward(result);
        result = cluFloatBufferDelete(testData->v);
        kzsErrorForward(result);
        result = cluFloatBufferDelete(testData->linTempBuffer);
        kzsErrorForward(result);
        result = cluFloatBufferDelete(testData->uPrevious);
        kzsErrorForward(result);
        result = cluFloatBufferDelete(testData->vPrevious);
        kzsErrorForward(result);
        result = cluFloatBufferDelete(testData->density);
        kzsErrorForward(result);
        result = cluFloatBufferDelete(testData->densityPrevious);
        kzsErrorForward(result);
    }
    for(i = 0; i < (cl_uint)kernelCount; ++i)
    {  
        clResult = clReleaseKernel(testData->fluidKernels[i]);      
        cluClErrorTest(clResult);
    }

    clResult = clReleaseMemObject(testData->outputBuffer);
    cluClErrorTest(clResult);

    clResult = clReleaseProgram(testData->fluidProgram);      
    cluClErrorTest(clResult);
    clResult = clReleaseCommandQueue(testData->queue);      
    cluClErrorTest(clResult);
    result = kzcMemoryFreeArray(testData->fluidKernels);
    kzsErrorForward(result);
    kzsSuccess();
}

kzsError fluidCreate(const struct BenchmarkFramework* framework, struct BfScene** out_scene)
{
    kzsError result;
    struct KzcMemoryManager* memoryManager = bfGetMemoryManager(framework);
    struct BfScene* scene;
    struct BfSceneConfiguration* configuration;
    struct FluidTestState *testData = NULL;

    result = bfTestConfigurationInitialize(memoryManager, fluidSceneLoad, fluidSceneUpdate, KZ_NULL,
        fluidSceneUninitialize, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, KZ_NULL, &configuration);
    kzsErrorForward(result);

    result = kzcMemoryAllocVariable(memoryManager,testData,"Fluid test internal data");
    kzsErrorForward(result);
    result = bfSceneCreate(framework, configuration, "Fluid Operations Test", "Physics", testData, &scene);
    kzsErrorForward(result);

    *out_scene = scene;
    kzsSuccess();
}
