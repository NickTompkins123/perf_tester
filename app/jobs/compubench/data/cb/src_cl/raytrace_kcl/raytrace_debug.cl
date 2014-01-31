__kernel void clearBuffersDebug(__global CLHitRecord* restrict hitrecs, __global float4* restrict pixelsFloat4)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;

	hitrecs[currIndex].triId = INT_MAX;
	hitrecs[currIndex].tHit = FLT_MAX;
	pixelsFloat4[currIndex] = (float4)(0.0f);
}

__kernel void generatePrimaryRaysDebug(__global const CLCamera* restrict camera, __global float* rayweights, __global CLRay* rays, __global CLRayDifferential* raydiffs)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;
	
	const float4 camDir = (camera->dirTL + camera->dirTR + camera->dirBL + camera->dirBR) * 0.25f;
	const float4 camI = (camDir - ((camera->dirTL + camera->dirBL) * 0.5f)) / (sizeX*0.5f);
	const float4 camJ = (camDir - ((camera->dirTL + camera->dirTR) * 0.5f)) / (sizeY*0.5f);
	
	CLRay primaryRay;
	primaryRay.org = camera->pos;
	primaryRay.dir = camera->dirTL + (idX) * camI + (idY) * camJ;
	primaryRay.dir = normalize(primaryRay.dir);

	CLRayDifferential raydiff;
	raydiff.rxOrg = camera->pos;
	raydiff.ryOrg = camera->pos;
	raydiff.rxDir = camera->dirTL + (idX+1) * camI + (idY) * camJ;
	raydiff.ryDir = camera->dirTL + (idX) * camI + (idY+1) * camJ;
	raydiff.rxDir = normalize(raydiff.rxDir);
	raydiff.ryDir = normalize(raydiff.ryDir);

	rayweights[currIndex] = 1.0f;
	rays[currIndex] = primaryRay;
	raydiffs[currIndex] = raydiff;
}

__kernel
void generateHitDataDebug_Normal(__global const CLHitRecord* restrict hitrecs, __global const CLTriangle* restrict tris,
								 __global const float* restrict rayweights, __global const CLRay* restrict rays,
								 __global float4* restrict color)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;
	
	const CLHitRecord currHitRec = hitrecs[currIndex];
	float4 hitColor = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

	if( currHitRec.triId != INT_MAX )
	{
		float4 n = tris[currHitRec.triId].p;
		hitColor = fabs( n );
	}

	color[currIndex] = hitColor;
}

__kernel
void generateHitDataDebug_Texture(__global const CLHitRecord* restrict hitrecs, __global const CLTriangle* restrict tris, __global const float4* restrict points, __global const float4* restrict normals, __global const float4* restrict tangents, __global const float2* restrict uvs,
								  __global const CLMaterial* restrict materials, __global const int* restrict matIds,
								  __global const uchar4* restrict texturesDiffuseMap, __global const uchar4* restrict texturesMaskMap, __global const uchar4* restrict texturesNormalMap,
								  __global const uchar4* restrict posx, __global const uchar4* restrict negx, __global const uchar4* restrict posy, __global const uchar4* restrict negy, __global const uchar4* restrict posz, __global const uchar4* restrict negz,
								  __global const float* restrict rayweights, __global const CLRay* restrict rays, __global const CLRayDifferential* restrict raydiffs,
								  __global float4* restrict color)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;
	
	const CLHitRecord currHitRec = hitrecs[currIndex];
	float4 hitColor = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

	hitColor = (float4)(0.25f, 0.25f, 1.0f, 1.0f);

	if( currHitRec.triId != INT_MAX )
	{
		const int matId = matIds[currHitRec.triId];

		const float b0 = 1.0f - currHitRec.b1 - currHitRec.b2;

		const float2 uv = uvs[3*currHitRec.triId    ] * b0 +
						  uvs[3*currHitRec.triId + 1] * currHitRec.b1 +
						  uvs[3*currHitRec.triId + 2] * currHitRec.b2;

		float4 p = rays[currIndex].org + rays[currIndex].dir * currHitRec.tHit;

		float4 n = normalize(normals[3*currHitRec.triId    ] * b0 +
					 		 normals[3*currHitRec.triId + 1] * currHitRec.b1 +
							 normals[3*currHitRec.triId + 2] * currHitRec.b2);

		float4 t = normalize(tangents[3*currHitRec.triId    ] * b0 +
							 tangents[3*currHitRec.triId + 1] * currHitRec.b1 +
							 tangents[3*currHitRec.triId + 2] * currHitRec.b2);

		// mipmapping util
		CLRayDifferential raydiff = raydiffs[currIndex];
		CLGeometryDifferential geomdiff = GetGeometryDifferential(points+currHitRec.triId*3, uvs+currHitRec.triId*3);
		CLIntersectionDifferential isectdiff = GetIntersectionDifferential(&raydiff, &geomdiff, points+currHitRec.triId*3, &p);
		const float width = 0.5f * max( max(fabs(isectdiff.dudx), fabs(isectdiff.dvdx)), max(fabs(isectdiff.dudy), fabs(isectdiff.dvdy)) );

		//float4 texDiff = textureBilinearLookup(texturesDiffuseMap + (materials+matId)->texOffset, (int2)((materials+matId)->texWidth, (materials+matId)->texHeight), uv.yx);
		float4 texDiff = textureBilinearLookupLevel(texturesDiffuseMap + (materials+matId)->texOffset, width, (int2)((materials+matId)->texWidth, (materials+matId)->texHeight), uv.yx);

		if((materials + matId)->texNormalFlag == 1)
		{
			const float16 tangenframeMAT = (float16)( t, cross(n, t), n, (float4)(0.0f,0.0f,0.0f,1.0f) );

			//float4 texNorm = textureBilinearLookup(texturesNormalMap + (materials+matId)->texNormalOffset, (int2)((materials+matId)->texNormalWidth, (materials+matId)->texNormalHeight), uv.yx);
			float4 texNorm = textureBilinearLookupLevel(texturesNormalMap + (materials+matId)->texNormalOffset, width, (int2)((materials+matId)->texNormalWidth, (materials+matId)->texNormalHeight), uv.yx);

			texNorm = texNorm*2.0f - 1.0f;
			texNorm.s3 = 0.0f;
			texNorm = normalize( texNorm );

			n = MulMat4Vec4(&tangenframeMAT, &texNorm);
		}

		hitColor = texDiff * n.y;
	}

	color[currIndex] = hitColor;
}

__kernel void convertToUCharDebug(__global const float4* restrict pixelsFloat4, __global uchar4* pixelsUchar4)
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