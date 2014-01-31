__kernel
void generateHitDataCutOff(__global const CLHitRecord* hitRecs, __global const CLRay* rays,
						   __global const float4* normals, __global const float2* uvs, __global const int* matIds, __global const CLMaterial* materials, __global const uchar4* textures,
						   __global CLRay* raysShadow, __global float* rayWeightsShadow,
						   __global CLHitData* hitData)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);

	const size_t sizeX = get_global_size(0);
	//const size_t sizeY = get_global_size(1);

	const size_t currIndex = (idY * sizeX) + idX;

	CLHitRecord hitrec = hitRecs[currIndex];
		
	if( hitrec.triId != INT_MAX )
	{
		CLRay currRay = rays[currIndex];

		float b0 = 1.0f - hitrec.b1 - hitrec.b2;
		
		float4 n = normalize
		(
			normals[ 3*hitrec.triId     ] * b0 +
			normals[ 3*hitrec.triId + 1 ] * hitrec.b1 +
			normals[ 3*hitrec.triId + 2 ] * hitrec.b2
		);

		n = dot(currRay.dir, n) > 0.0f ? -n : n;
		
		float2 uv =
		(
			uvs[ 3*hitrec.triId     ] * b0 +
			uvs[ 3*hitrec.triId + 1 ] * hitrec.b1 +
			uvs[ 3*hitrec.triId + 2 ] * hitrec.b2
		);
				
		int matId = matIds[ hitrec.triId ];

		CLMaterial _mat = materials[matId];
		float4 c = textureBilinearLookup(textures + _mat.texOffset, (int2)(_mat.texWidth, _mat.texHeight), uv.yx);
		
		float4 p = currRay.org + currRay.dir * hitrec.tHit + n * 0.001f;
		
		(hitData+currIndex)->hitPosition = p;		
		(hitData+currIndex)->hitNormal = n;
		(hitData+currIndex)->hitColor = c;
		
		CLRay shadowRay;
		shadowRay.org = p; 
		shadowRay.dir = normalize((float4)(-5.25f, 47.0f, 11.0f, 0.0f));
		
		raysShadow[currIndex] = shadowRay;
		rayWeightsShadow[currIndex] = 1.0f;
		//rayWeightsShadow[currIndex] = 0.0f;
	}
	else
	{
		rayWeightsShadow[currIndex] = 0.0f;
	}
}