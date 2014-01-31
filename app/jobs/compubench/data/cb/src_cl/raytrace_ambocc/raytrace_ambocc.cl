#ifndef M_PI_F
    #define M_PI_F 3.14159265358979323846f
#endif

__constant int SPHNUM = 5;
__constant float4 sphPosArr[5] =
{
	(float4)(-6.0f, 0.0f, 0.0f, 1.0f),
	(float4)(-3.0f, 0.0f, 0.0f, 1.0f),
	(float4)( 0.0f, 0.0f, 0.0f, 1.0f),
	(float4)( 3.0f, 0.0f, 0.0f, 1.0f),
	(float4)( 6.0f, 0.0f, 0.0f, 1.0f)
};

typedef struct
{
	float4	origin;
	float4	view, right, up;
} CLCamera;

float nextFloat(unsigned int* seed0, unsigned int* seed1)
{
	*seed0 = 36969 * ((*seed0) & 65535) + ((*seed0) >> 16);
	*seed1 = 18000 * ((*seed1) & 65535) + ((*seed1) >> 16);
		
	return ((((*seed0) << 16) + (*seed1)) & 0xffff) * (1.0f / 0xffff);
}

float4 uniformSampleHemiSphere(float u1, float u2)
{
	float r = sqrt( max(0.0f, 1.0f - u1 * u1) );
	float phi = 2.0f * M_PI_F * u2;
    
    return (float4)(cos(phi) * r, sin(phi) * r, u1, 0.0f);
}
	
bool intersectRayPlane(float* tRay, float4 rayOrg, float4 rayDir, float4 plane)
{
	float denom = dot(plane, rayDir);
	*tRay = -1.0f * dot(plane, rayOrg) / denom;
		
	if(*tRay > 0.0f)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool intersectRaySphere(float* tRay, float4 rayOrg, float4 rayDir, float4 sphPos, float sphRad)
{
	float4 SphToRay = rayOrg - sphPos;

	float a = dot(rayDir, rayDir);
	float b = 2.0f * dot(rayDir, SphToRay);
	float c = dot(SphToRay, SphToRay) - sphRad*sphRad;

	float disc = (b*b - 4*a*c);

	if( disc < 0.0f )
	{
		return 0;
	}

	disc = sqrt(disc);

	*tRay = (-b - disc) / (2.0f*a);
	if(*tRay > 0.0f)
	{
		return 1;
	}

	*tRay = (-b + disc) / (2.0f*a);
	if(*tRay > 0.0f)
	{
		return 1;
	}

	return 0;
}

bool intersectWorldBoolean(float4 rayOrg, float4 rayDir, float4* hitPoint, float4* hitNormal, float currIter, float iterCount)
{
	float tCurr = 1e+6f;
	float tRay;
	
	for(int i = 0; i < SPHNUM; ++i)
	{
		float t = currIter * 2.0f * M_PI_F / iterCount;
		float y = 1.0f + cos((i-2.0f)*M_PI_F*0.5f + t);

		float4 sphPos = sphPosArr[i] + (float4)(0.0f, y, 0.0f, 0.0f);
		bool hasHit = intersectRaySphere(&tRay, rayOrg, rayDir, sphPos, 1.2f);
		if(hasHit && tRay < tCurr)
		{
			*hitPoint = rayOrg + rayDir * tRay;
			*hitNormal = normalize( *hitPoint - sphPos );
			
			tCurr = tRay;
		}
	}
		
	float4 plane = (float4)(0.0f, 1.0f, 0.0f, 1.2f);
	bool hasHit = intersectRayPlane(&tRay, rayOrg, rayDir, plane);
	if(hasHit && tRay < tCurr)
	{
		float4 hitPointTemp = rayOrg + rayDir * tRay;
			
		if(	hitPointTemp.x < 10.0f && hitPointTemp.x > -10.0f &&
			hitPointTemp.y < 10.0f && hitPointTemp.y > -10.0f &&
			hitPointTemp.z < 10.0f && hitPointTemp.z > -10.0f	)
		{	
			*hitPoint = hitPointTemp;
			*hitNormal = plane;
			(*hitNormal).w = 0.0f;
			
			tCurr = tRay;
		}
	}

	return (tCurr != 1e+6f);
}

__kernel
void clear(__global float4* gImageFloat4)
{
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);

	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	gImageFloat4[idY * sizeX + idX] = (float4)(0,0,0,0);
}

__kernel
void root(__global const CLCamera* gCamera, __global uint* gSeed0, __global uint* gSeed1,
		  const float currIter, const float iterCount, const float scaleIter,
		  __global float4* gImageFloat4)
{
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);

	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);
	
	float4 aoFloat4 = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
	
	CLCamera camera = gCamera[0];
	
	float4 rayOrg = camera.origin;
	float4 rayDir = normalize( camera.view + camera.right * (float)(idX) + camera.up * (float)(idY) );
	
	float4 hitPoint;
	float4 hitNormal;
	
	bool primHit = intersectWorldBoolean(rayOrg, rayDir, &hitPoint, &hitNormal, currIter, iterCount);
	
	int aoHitCount = 0;
	if(primHit)
	{
		float4 aoRayOrg = hitPoint + hitNormal * 0.001f;
		
		// load seed		
		unsigned int seed0 = gSeed0[idY * sizeX + idX];
		unsigned int seed1 = gSeed1[idY * sizeX + idX];
					
		for(int aoSamp = 0; aoSamp < 16; ++aoSamp)
		{
			float4 aoRayDir = uniformSampleHemiSphere( nextFloat(&seed0, &seed1), nextFloat(&seed0, &seed1) );
						
			// orient random dir
			if( dot(aoRayDir, hitNormal) < 0.0f )
			{
				aoRayDir = -aoRayDir;
			}
						
			bool aoHit = intersectWorldBoolean(aoRayOrg, aoRayDir, &hitPoint, &hitNormal, currIter, iterCount);
			aoHitCount += aoHit;
		}
		
		// save seed
		gSeed0[idY * sizeX + idX] = seed0;
		gSeed1[idY * sizeX + idX] = seed1;
		
		aoFloat4 = (float4)(1.0f - aoHitCount/16.0f, 1.0f - aoHitCount/16.0f, 1.0f - aoHitCount/16.0f, 1.0f);
		aoFloat4 *= scaleIter;
	}
	
	gImageFloat4[idY * sizeX + idX] += aoFloat4;
}

__kernel void convertToUChar(__global const float4* restrict pixelsFloat4, __global uchar4* pixelsUchar4)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;

	uchar4 pixelUchar4 = convert_uchar4_sat( pixelsFloat4[currIndex] * 255.0f + 0.5f );
	pixelUchar4.s3 = 255;

	pixelsUchar4[currIndex] = pixelUchar4;
}