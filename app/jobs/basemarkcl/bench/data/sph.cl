/**
* OpenCL smoothed particle hydrodynamics implementation.
*
* Copyright 2011 by Rightware. All rights reserved.
*/


#define H (0.04f)


inline float W(float4 difference, float h)
{
    return pown(H*H - dot(difference, difference), 3);
}

inline float Wconst(float h)
{
    return (315.0f / (64.0f * M_PI_F * pown(H, 9)));
}

inline float delWconst(float h)
{
    return -45.0f / (M_PI_F * pown(H, 6));
}

inline float4 delW(float4 difference, float h)
{
    return  pown(H - fast_length(difference), 2) * fast_normalize(difference);
}

inline float laplaceWconst(float h)
{
    return (45.0f / (M_PI_F * pown(H, 6)));
}


inline float laplaceW(float4 difference, float h)
{
    return (H - fast_length(difference));
}


/** Returns the coordinates of the voxel where particle resides */
int4 getPosCoord(float4 position, uint4 voxelCount, float h)
{
    int4 posCoord;
    posCoord.x = (int)clamp((int)floor(position.x / h), 0, (int)voxelCount.x-1) ;
    posCoord.y = (int)clamp((int)floor(position.y / h), 0, (int)voxelCount.y-1) ; 
    posCoord.z = (int)clamp((int)floor(position.z / h), 0, (int)voxelCount.z-1); 
    posCoord.w = 0;
    return posCoord;
}

/** Returns the voxel id where particle resides */
int getPosHash(float4 position, uint4 voxelCount, float h)
{
    int4 posCoord = getPosCoord(position, voxelCount, h);
    return voxelCount.x * voxelCount.y * posCoord.z + voxelCount.x * posCoord.y + posCoord.x;
}

inline int getPosHashI(int4 posCoord, uint4 voxelCount)
{    
    return voxelCount.x * voxelCount.y * posCoord.z + voxelCount.x * posCoord.y + posCoord.x;
}

/** Invocated with global size == amount of particles */
__kernel void calculateDensity(__global const uint2* restrict keys, __global const float4* restrict sortedPosition, __global const int* restrict voxelParticles,  __global float* restrict sortedDensity, uint4 voxelCount, float h, float m)
{
    const size_t i = get_global_id(0);

    /* This way of clamping the coordinate means that the boundary voxels read 3 unnecessary voxels each
    However we avoid conditionals inside loop and no need to make different loops for the special cases */
    int4 ownPosition = clamp(getPosCoord(sortedPosition[i], voxelCount, h),(int4)(1,1,1,0),(int4)(voxelCount.x - 2, voxelCount.y - 2, voxelCount.z - 2, 0));
    const float4 ownPositionf = sortedPosition[i];
    float totalDensity = 0.0f;
    const int size = get_global_size(0);
    for(int z  = -1; z <= 1; z++)
    {
        for(int y  = -1; y <= 1; y++)
        {
            for(int x  = -1; x <= 1; x++)
            {
                int index = getPosHashI(ownPosition + (int4)(x, y, z, 0) , voxelCount);
                int pId = voxelParticles[index];
              
                /* Skip if the examined voxel is empty */
                if(pId == -1) continue;

                while( pId < size && keys[pId].x == index)
                {
                    const float4 diff = ownPositionf - sortedPosition[pId];
                    /* Ignore particles outside distance h */
                    if(dot(diff,diff) < h*h)
                    {
                        totalDensity += W(diff, h);         
                    }
                    pId++;
                }                
            }
        }
    }
    sortedDensity[i] = totalDensity * m * Wconst(h);
}

/** Invocated with global size == amount of particles */
__kernel void calculateAcceleration(__global const uint2* restrict keys, __global const float4* restrict sortedPosition, __global const int* voxelParticles,  __global const float* restrict sortedDensity, __global const float4* restrict sortedVelocity, __global float4* restrict acceleration, uint4 voxelCount, float h, float m, float u)
{
    const size_t i = get_global_id(0);

    /* This way of clamping the coordinate means that the boundary voxels read 3 unnecessary voxels each
    However we avoid conditionals inside loop and no need to make different loops for the special cases */
    int4 ownPosition = clamp(getPosCoord(sortedPosition[i], voxelCount, h),(int4)(1,1,1,0),(int4)(voxelCount.x - 2, voxelCount.y - 2, voxelCount.z - 2, 0));
    const float4 ownPositionf = sortedPosition[i];
    const float ownDensity = sortedDensity[i];
    const float4 ownVelocity = sortedVelocity[i];
    const float h2 = h*h;
    const int size = get_global_size(0);
    float4 pressureAccel = 0.0f;
    float4 gravityAccel = (float4)(0.0f, -9.81f, 0.0f, 0.0f);
    float4 viscousAccel = 0.0f;
    const float ownPressureTerm = (ownDensity - 1000.0f)/(ownDensity*ownDensity);

    for(int z  = -1; z <= 1; z++)
    {
        for(int y  = -1; y <= 1; y++)
        {
            for(int x  = -1; x <= 1; x++)
            {
                int index = getPosHashI(ownPosition + (int4)(x, y, z, 0) , voxelCount);
                int pId = voxelParticles[index];
            
                /* Skip if the examined voxel is empty */
                if(pId == -1) continue;

                while(pId < size && keys[pId].x == index)
                {

                    const float4 diff = ownPositionf - sortedPosition[pId];

                    /* Ignore particles outside distance h */
                    if(dot(diff,diff) < h2)
                    {
                        const float otherDensity = sortedDensity[pId];
                        const float4 otherVelocity = sortedVelocity[pId];
                        pressureAccel += (ownPressureTerm + (otherDensity - 1000.0f)/(otherDensity*otherDensity)) * delW(diff, h);
                        viscousAccel += ((otherVelocity-ownVelocity)/otherDensity)*laplaceW(diff,h);               
                    }
                    pId++;
                }                
            }
        }
    }
    const float k = 5.0f;
    acceleration[keys[i].y] = gravityAccel - (pressureAccel * m * k * delWconst(h)) + (viscousAccel * m * laplaceWconst(h) * (u/ownDensity));
}


/** Integrates the particle count for each voxels. Not required with the sorting based approach. TODO: Make a real parallel cumulative sum*/
__kernel void cumSumVoxels(__global int* particleCount, int voxels)
{
    int cs = 0;
    for(int i = 0; i < voxels; i++)
    {
        int tmp = particleCount[i];
        particleCount[i] = cs;
        cs += tmp;        
    }
}

/** Integrate the position and velocity based on current acceleration */
__kernel void integrate(__global float4* restrict position, __global float4* restrict velocity, __global const float4* restrict acceleration, uint4 voxelCount, float h, float dt)
{
    const size_t i = get_global_id(0);
    float4 newVel = velocity[i] + acceleration[i]*dt;
    /* Due to the voxel update mechanism the velocity is clamped to h/(dt)*/
    newVel = clamp(newVel, (float4)(-h/(dt), -h/(dt), -h/(dt), 0), (float4)(h/(dt), h/(dt), h/(dt), 0));

    position[i] += newVel*dt;
    
    if(position[i].x < 0.0f || position[i].x > (float)(voxelCount.x - 1)*h)
    {
        position[i].x = clamp(position[i].x, 0.0f, (float)(voxelCount.x - 1)*h);
        newVel.x = -newVel.x;
    }
    if(position[i].y < 0.0f || position[i].y > (float)(voxelCount.y - 1)*h)
    {
        position[i].y = clamp(position[i].y, 0.0f, (float)(voxelCount.y - 1)*h);
        newVel.y = -newVel.y;
    }
    if(position[i].z < 0.0f || position[i].z > (float)(voxelCount.z - 1)*h)
    {
        position[i].z = clamp(position[i].z, 0.0f, (float)(voxelCount.z - 1)*h);
        newVel.z = -newVel.z;
    }
    
    velocity[i] = newVel;
}


/** Calculate the keys array from updated position values */
__kernel void updateKeys(__global uint2* restrict keys, __global const float4* restrict position, uint4 voxelCount, float h)
{
    const size_t i = get_global_id(0);
    keys[i] = (uint2)(getPosHash(position[i], voxelCount, h), (uint)i);
}


/** Invocated with global size == amount of particles. Set the cell start into voxel array */
__kernel void setVoxels(__global int* voxelParticles, __global const uint2* keys)
{
    const size_t i = get_global_id(0);
    if(i == 0) /* The first particle on the list */
    {
        voxelParticles[keys[0].x] = 0;
        return;
    }
    if(keys[i-1].x != keys[i].x)
    {
        voxelParticles[keys[i].x] = i;
    }
}


/** Invocated with global size == amount of voxels */
__kernel void clearVoxels(__global int* voxelParticles)
{
    voxelParticles[get_global_id(0)] = -1;
}

/** Ran only once at start to set the acceleration and velocity buffers as zero */
__kernel void clearVelocityAcceleration(__global float4* velocity, __global float4* acceleration)
{
    velocity[get_global_id(0)] = 0.0f;
    acceleration[get_global_id(0)] = 0.0f;
}


/** Invocated with global size == particle count */
__kernel void sortPostProcess(__global const uint2* restrict keys, __global float4* restrict position, __global float4* restrict velocity, __global float4* restrict sortedPosition, __global float4* restrict sortedVelocity)
{
    sortedPosition[get_global_id(0)] = position[keys[get_global_id(0)].y];
    sortedVelocity[get_global_id(0)] = velocity[keys[get_global_id(0)].y];
}


/**  
* Bitonic sort
* The array will always be sorted in ascending order
* The first dimension of global size has to be exactly half of the length of the keys
* Stage goes from 0 to floor(log2(element_count)) and pass goes from 0 to stage on each invocation
* keys points to 2d vector where the first element is the cell id where the particle resides and second element is the particle id.
*/
__kernel void bitonicSort(__global uint2* keys, uint stage, uint pass)
{
    const uint thread = get_global_id(0);
    
    const uint pairDistance = 1 << (stage - pass);

    /* The purpose of this is to introduce an additional zero at stage - pass bit*/    
    const uint leftID = (thread & (pairDistance - 1) ) | ((thread  & ~(pairDistance - 1) ) << 1); /* Is the same as below */    
    
    const uint direction = ((thread >> stage) & 1) == 1 ? 0 : 1;

    const uint rightID = leftID + pairDistance;
    const uint2 left = keys[leftID];
    const uint2 right = keys[rightID];

    const uint2 larger  = left.x > right.x ? left : right;
    const uint2 smaller = left.x > right.x ? right : left;

    keys[leftID] = direction ? smaller : larger;
    keys[rightID] = direction ? larger : smaller;
}

#define VERTEX_DATA_SIZE 8
__kernel void updateVBO(__global float4* positions, __global float* vbo, __global float4* billboard, float yoffset, float offset, float scale)
{
    unsigned int i = get_global_id(0);
    unsigned int index = i * 4 * VERTEX_DATA_SIZE;
    
    vbo[index + 0] = (positions[i].x - offset) * scale + billboard[0].x;
    vbo[index + 1] = (positions[i].y - offset - yoffset) * scale + billboard[0].y;
    vbo[index + 2] = (positions[i].z - offset) * scale + billboard[0].z;
    vbo[index + 3] = 0.0f;
    
    vbo[index + 4] = 0.0f;
    vbo[index + 5] = 0.0f;
    vbo[index + 6] = 1.0f;
    vbo[index + 7] = 0.0f;
    
    index += VERTEX_DATA_SIZE;
    vbo[index + 0] = (positions[i].x - offset) * scale + billboard[1].x;
    vbo[index + 1] = (positions[i].y - offset - yoffset) * scale + billboard[1].y;
    vbo[index + 2] = (positions[i].z - offset) * scale + billboard[1].z;
    vbo[index + 3] = 0.0f;
    
    vbo[index + 4] = 1.0f;
    vbo[index + 5] = 0.0f;
    vbo[index + 6] = 0.0f;
    vbo[index + 7] = 0.0f;
    
    index += VERTEX_DATA_SIZE;
    vbo[index + 0] = (positions[i].x - offset) * scale + billboard[2].x;
    vbo[index + 1] = (positions[i].y - offset - yoffset) * scale + billboard[2].y;
    vbo[index + 2] = (positions[i].z - offset) * scale + billboard[2].z;
    vbo[index + 3] = 0.0f;
    
    vbo[index + 4] = 0.0f;
    vbo[index + 5] = 1.0f;
    vbo[index + 6] = 0.0f;
    vbo[index + 7] = 0.0f;
    
    index += VERTEX_DATA_SIZE;
    vbo[index + 0] = (positions[i].x - offset) * scale + billboard[3].x;
    vbo[index + 1] = (positions[i].y - offset - yoffset) * scale + billboard[3].y;
    vbo[index + 2] = (positions[i].z - offset) * scale + billboard[3].z;
    vbo[index + 3] = 0.0f;
    
    vbo[index + 4] = 1.0f;
    vbo[index + 5] = 1.0f;
    vbo[index + 6] = 0.0f;
    vbo[index + 7] = 0.0f;
}
