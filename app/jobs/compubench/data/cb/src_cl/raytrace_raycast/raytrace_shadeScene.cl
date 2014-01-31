__kernel
void shadeScene(__global const CLRay* rays, __global const float* rayWeights,
				__global const CLHitRecord* hitRecs, __global const CLHitData* hitData,
				__global const CLHitRecord* hitRecsShadow,
				__global float4* imageFloat4)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);

	const size_t sizeX = get_global_size(0);
	//const size_t sizeY = get_global_size(1);

	const size_t currIndex = (idY * sizeX) + idX;
	
	float4 shadedPixel = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

	int triId = (hitRecs+currIndex)->triId;
	float weight = rayWeights[currIndex];
	
	CLHitData hitdata = hitData[currIndex];
		
	if( triId != INT_MAX )
	{
		float diffFactor = fmax (0.0f, dot( hitdata.hitNormal, normalize((float4)(-5.25f, 47.0f, 11.0f, 0.0f)) ) );
		
		int shtriId = (hitRecsShadow+currIndex)->triId;
		float shFactor = shtriId == INT_MAX ? 1.0f : 0.0f;
		
		shadedPixel = weight * shFactor * diffFactor * hitdata.hitColor + 0.1f * hitdata.hitColor;
	}
	else
	{
		CLRay currRay = rays[currIndex];
		
		shadedPixel = weight * fmax( (float4)(0.75f, 0.75f, 0.75f, 1.0f) * (1.0f - currRay.dir.y) + (float4)(0.37f, 0.63f, 1.0f, 1.0f) * currRay.dir.y, (float4)(0.0f, 0.0f, 0.0f, 1.0f) );
	}

	imageFloat4[currIndex] += shadedPixel;
}