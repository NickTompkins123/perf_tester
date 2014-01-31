__kernel
void intersectScene(__global const CLRay* rays, __global const float* rayWeights, __global const CLTriangle* tris,
					__global const CLAABB3x2* aabb, __global const CLKDNode* kdnodePool, __global const int* kdtriRegistry,
					__global CLHitRecord* hitRecs)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);

	const size_t sizeX = get_global_size(0);
	//const size_t sizeY = get_global_size(1);

	const size_t currIndex = (idY * sizeX) + idX;

	CLHitRecord hitrec;
	hitrec.triId = INT_MAX;
	hitrec.tHit = FLT_MAX;
	
	if( rayWeights[currIndex] != 0.0f )
	{
		CLRay currRay = rays[currIndex];
		intersectKDTree(&currRay, tris, aabb, kdnodePool, kdtriRegistry, &hitrec);
	}
	
	hitRecs[currIndex] = hitrec;	
}