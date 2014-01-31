__kernel void clearBuffers(__global CLHitRecord* restrict hitrecs, __global float4* restrict pixelsFloat4)
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

__kernel
void generateHitDataPrim(__global const CLHitRecord* restrict hitrecs,
					     __global const CLTriangle* restrict tris, __global const float4* restrict points, __global const float4* restrict normals, __global const float4* restrict tangents, __global const float2* restrict uvs,
					     __global const CLMaterial* restrict materials, __global const int* restrict matIds,
					     __global const uchar4* restrict texturesDiffuseMap, __global const uchar4* restrict texturesMaskMap, __global const uchar4* restrict texturesNormalMap,
					     __global const float* restrict rayweights, __global const CLRay* restrict rays, __global const CLRayDifferential* restrict raydiffs,
					     const unsigned int gtime,
					     __global CLHitData* restrict hitdata)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;
	
	const CLHitRecord currHitRec = hitrecs[currIndex];
	CLHitData currHitData;

	if( currHitRec.triId != INT_MAX )
	{
		currHitData.matId = matIds[currHitRec.triId];
		currHitData.tHit = currHitRec.tHit;

		float b0 = 1.0f - currHitRec.b1 - currHitRec.b2;
		float2 uv = uvs[3*currHitRec.triId    ] * b0 +
						  uvs[3*currHitRec.triId + 1] * currHitRec.b1 +
						  uvs[3*currHitRec.triId + 2] * currHitRec.b2;

		float4 p = rays[currIndex].org + rays[currIndex].dir * currHitRec.tHit;

		float4 n = normalize(normals[3*currHitRec.triId    ] * b0 +
							 normals[3*currHitRec.triId + 1] * currHitRec.b1 +
							 normals[3*currHitRec.triId + 2] * currHitRec.b2);
		if (dot(n,-1.0f*rays[currIndex].dir) < -0.25f)
			n*=-1.0f;

		float4 t = normalize(tangents[3*currHitRec.triId    ] * b0 +
							 tangents[3*currHitRec.triId + 1] * currHitRec.b1 +
							 tangents[3*currHitRec.triId + 2] * currHitRec.b2);

		// mipmapping util
		CLRayDifferential raydiff = raydiffs[currIndex];
		CLGeometryDifferential geomdiff = GetGeometryDifferential(points+currHitRec.triId*3, uvs+currHitRec.triId*3);
		CLIntersectionDifferential isectdiff = GetIntersectionDifferential(&raydiff, &geomdiff, points+currHitRec.triId*3, &p);
		const float width = 1.0f * max( max(fabs(isectdiff.dudx), fabs(isectdiff.dvdx)), max(fabs(isectdiff.dudy), fabs(isectdiff.dvdy)) );

		if((materials + currHitData.matId)->texMaskFlag == 999)
		{
			uv += (float2)(-0.0001f, 0.0f) * gtime;
		}

		if((materials + currHitData.matId)->texNormalFlag != 0)
		{
			float4 b = cross(n, t);
			//float4 nTEX = textureBilinearLookup(texturesNormalMap + (materials+currHitData.matId)->texNormalOffset, (int2)((materials+currHitData.matId)->texNormalWidth, (materials+currHitData.matId)->texNormalHeight), uv.yx);
			float4 nTEX = textureBilinearLookupLevel(texturesNormalMap + (materials+currHitData.matId)->texNormalOffset, width, (int2)((materials+currHitData.matId)->texNormalWidth, (materials+currHitData.matId)->texNormalHeight), uv.yx);
			nTEX = nTEX*2.0f - 1.0f;
			nTEX.s3 = 0.0f;
			nTEX = normalize( nTEX );

			float16 tangenframeMAT;

			tangenframeMAT.s0123 = t;
			tangenframeMAT.s4567 = b;
			tangenframeMAT.s89AB = n;
			tangenframeMAT.sCDEF = (float4)(0.0f,0.0f,0.0f,1.0f);

			n = MulMat4Vec4(&tangenframeMAT, &nTEX);
		}

		currHitData.hitPosition = p;
		currHitData.hitNormal = n;

		//currHitData.hitKD = textureBilinearLookup(texturesDiffuseMap + (materials+currHitData.matId)->texOffset, (int2)((materials+currHitData.matId)->texWidth, (materials+currHitData.matId)->texHeight), uv.yx) / M_PI_F;
		currHitData.hitKD = textureBilinearLookupLevel(texturesDiffuseMap + (materials+currHitData.matId)->texOffset, width, (int2)((materials+currHitData.matId)->texWidth, (materials+currHitData.matId)->texHeight), uv.yx) / M_PI_F;

		if ((materials + currHitData.matId)->texMaskFlag != 0)
		{
			//currHitData.hitKR = textureBilinearLookup(texturesMaskMap + (materials+currHitData.matId)->texMaskOffset, (int2)((materials+currHitData.matId)->texMaskWidth, (materials+currHitData.matId)->texMaskHeight), uv.yx);
			currHitData.hitKR = textureBilinearLookupLevel(texturesMaskMap + (materials+currHitData.matId)->texMaskOffset, width, (int2)((materials+currHitData.matId)->texMaskWidth, (materials+currHitData.matId)->texMaskHeight), uv.yx);
			//currHitData.hitKR = (float4)(0.0f);
		}
	}
	else
	{
		currHitData.matId = INT_MAX;
		currHitData.hitKD = (float4)(1.0f);
	}

	hitdata[currIndex] = currHitData;
}

__kernel
void generateHitData(__global const CLHitRecord* restrict hitrecs,
					 __global const CLTriangle* restrict tris, __global const float4* restrict points, __global const float4* restrict normals, __global const float4* restrict tangents, __global const float2* restrict uvs,
					 __global const CLMaterial* restrict materials, __global const int* restrict matIds,
					 __global const uchar4* restrict texturesDiffuseMap, __global const uchar4* restrict texturesMaskMap, __global const uchar4* restrict texturesNormalMap,
					 __global const float* restrict rayweights, __global const CLRay* restrict rays,
					const unsigned int gtime,
					 __global CLHitData* restrict hitdata)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;
	
	const CLHitRecord currHitRec = hitrecs[currIndex];
	CLHitData currHitData;

	if( currHitRec.triId != INT_MAX )
	{
		currHitData.matId = matIds[currHitRec.triId];
		currHitData.tHit = currHitRec.tHit;

		float b0 = 1.0f - currHitRec.b1 - currHitRec.b2;
		float2 uv = uvs[3*currHitRec.triId    ] * b0 +
						  uvs[3*currHitRec.triId + 1] * currHitRec.b1 +
						  uvs[3*currHitRec.triId + 2] * currHitRec.b2;

		float4 n = normalize(normals[3*currHitRec.triId    ] * b0 +
							 normals[3*currHitRec.triId + 1] * currHitRec.b1 +
							 normals[3*currHitRec.triId + 2] * currHitRec.b2);
		if (dot(n,-1.0f*rays[currIndex].dir) < -0.25f)
			n*=-1.0f;

		float4 t = normalize(tangents[3*currHitRec.triId    ] * b0 +
							 tangents[3*currHitRec.triId + 1] * currHitRec.b1 +
							 tangents[3*currHitRec.triId + 2] * currHitRec.b2);

		if((materials + currHitData.matId)->texMaskFlag == 999)
		{
			uv += (float2)(-0.0001f, 0.0f) * gtime;
		}

		if((materials + currHitData.matId)->texNormalFlag != 0)
		{
			float4 b = cross(n, t);
			float4 nTEX = textureBilinearLookup(texturesNormalMap + (materials+currHitData.matId)->texNormalOffset, (int2)((materials+currHitData.matId)->texNormalWidth, (materials+currHitData.matId)->texNormalHeight), uv.yx);
			//float4 nTEX = textureBilinearLookupLevel(texturesNormalMap + (materials+currHitData.matId)->texNormalOffset, width, (int2)((materials+currHitData.matId)->texNormalWidth, (materials+currHitData.matId)->texNormalHeight), uv.yx);
			nTEX = nTEX*2.0f - 1.0f;
			nTEX.s3 = 0.0f;
			nTEX = normalize( nTEX );

			float16 tangenframeMAT;

			tangenframeMAT.s0123 = t;
			tangenframeMAT.s4567 = b;
			tangenframeMAT.s89AB = n;
			tangenframeMAT.sCDEF = (float4)(0.0f,0.0f,0.0f,1.0f);

			n = MulMat4Vec4(&tangenframeMAT, &nTEX);
		}

		currHitData.hitPosition = rays[currIndex].org + rays[currIndex].dir * currHitRec.tHit;
		currHitData.hitNormal = n;

		currHitData.hitKD = textureBilinearLookup(texturesDiffuseMap + (materials+currHitData.matId)->texOffset, (int2)((materials+currHitData.matId)->texWidth, (materials+currHitData.matId)->texHeight), uv.yx) / M_PI_F;
		
		if ((materials + currHitData.matId)->texMaskFlag != 0)
			currHitData.hitKR = textureBilinearLookup(texturesMaskMap + (materials+currHitData.matId)->texMaskOffset, (int2)((materials+currHitData.matId)->texMaskWidth, (materials+currHitData.matId)->texMaskHeight), uv.yx);
		//currHitData.hitKR = (float4)(0.0f);
	}
	else
	{
		currHitData.matId = INT_MAX;
		currHitData.hitKD = (float4)(1.0f);
	}

	hitdata[currIndex] = currHitData;
}

__kernel void generateHitDataShadow(__global const CLHitRecord* restrict hitrecs, __global float* filterShadow)
{
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;

	float shadowParameter = 10.0f;
	filterShadow[currIndex] = hitrecs[currIndex].triId == INT_MAX ? 0.0f : -(clamp(shadowParameter/hitrecs[currIndex].tHit, 0.0f, shadowParameter) );
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