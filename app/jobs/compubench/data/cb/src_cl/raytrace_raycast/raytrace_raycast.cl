/*
#include "raytrace_util_types.cl"
#include "raytrace_util_intersect.cl"
#include "raytrace_util_texture.cl"

#include "raytrace_clearFloat4.cl"
#include "raytrace_convertUchar4.cl"

#include "raytrace_generatePrimaryRays.cl"
#include "raytrace_intersectScene.cl"
#include "raytrace_generateHitData.cl"
#include "raytrace_generateHitDataCutOff.cl"
#include "raytrace_shadeScene.cl"
*/

/*
__kernel
void raytrace_raycast(__global const CLCamera* camera, __global const CLTriangle* tris, __global const float4* normals, __global const float2* uvs,
					  __global const int* matIds, __global const CLMaterial* materials, __global const uchar4* textures,
					  __global const CLAABB3x2* aabb, __global const CLKDNode* kdnodePool, __global const int* kdtriRegistry,
					  __global uchar4* imageUchar4)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);

	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = (idY * sizeX) + idX;

	float4 camDir = (camera->dirTL + camera->dirTR + camera->dirBL + camera->dirBR) * 0.25f;
	float4 camI = (camDir - ((camera->dirTL + camera->dirBL) * 0.5f)) / (sizeX*0.5f);
	float4 camJ = (camDir - ((camera->dirTL + camera->dirTR) * 0.5f)) / (sizeY*0.5f);

	CLRay currRay;
	currRay.org = camera->pos;
	currRay.dir = camera->dirTL + (idX) * camI + (idY) * camJ;
	currRay.dir = normalize(currRay.dir);

	CLHitRecord hitrec;
	hitrec.triId = INT_MAX;
	hitrec.tHit = FLT_MAX;

	intersectKDTree(&currRay, tris, aabb, kdnodePool, kdtriRegistry, &hitrec);

	float4 shadedPixel = (float4)(0.0f);

	if( hitrec.triId != INT_MAX )
	{
		const int matId = matIds[hitrec.triId];	

		const float b0 = 1.0f - hitrec.b1 - hitrec.b2;

		float4 n = normalize(normals[3*hitrec.triId    ] * b0 +
							 normals[3*hitrec.triId + 1] * hitrec.b1 +
							 normals[3*hitrec.triId + 2] * hitrec.b2);

		float2 uv = uvs[3*hitrec.triId    ] * b0 +
					uvs[3*hitrec.triId + 1] * hitrec.b1 +
					uvs[3*hitrec.triId + 2] * hitrec.b2;
		
		float diffFactor = max (0.0f, dot( n, normalize((float4)(-5.25f, 47.0f, 11.0f, 0.0f)) ) );
		float4 diffColor = textureBilinearLookup(textures + (materials+matId)->texOffset, (int2)((materials+matId)->texWidth, (materials+matId)->texHeight), uv.yx);

		float4 shColor = (float4)(0.0f);

		CLRay shadowRay;
		shadowRay.org = currRay.org + hitrec.tHit * currRay.dir + n * 0.001f;
		shadowRay.dir = normalize((float4)(-5.25f, 47.0f, 11.0f, 0.0f));

		CLHitRecord shhitrec;
		shhitrec.triId = INT_MAX;
		shhitrec.tHit = FLT_MAX;

		intersectKDTree(&shadowRay, tris, aabb, kdnodePool, kdtriRegistry, &shhitrec);

		if( shhitrec.triId == INT_MAX )
		{
			shColor = (float4)(1.0f);
		}

		//shadedPixel = fabs(n);
		//shadedPixel = (float4)(uv, 0.0f, 0.0f);
		shadedPixel = shColor * diffFactor * diffColor + 0.1f * diffColor;
	}
	else
	{
		shadedPixel = max( (float4)(0.75f, 0.75f, 0.75f, 1.0f) * (1.0f - currRay.dir.y) + (float4)(0.37f, 0.63f, 1.0f, 1.0f) * currRay.dir.y, (float4)(0.0f, 0.0f, 0.0f, 1.0f) );
	}

	uchar4 pixelUchar4;
	pixelUchar4.x = shadedPixel.x * 255.0f;
	pixelUchar4.y = shadedPixel.y * 255.0f;
	pixelUchar4.z = shadedPixel.z * 255.0f;
	pixelUchar4.w = 255;

	imageUchar4[currIndex] = pixelUchar4;
}
*/