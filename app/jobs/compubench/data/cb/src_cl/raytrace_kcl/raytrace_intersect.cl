// ((unsigned int)(-1)) >> 2
#define KDTRIEMPTY 1073741823

//#define USESTACK_SIZE 4
//#define USESTACK_LOCALARR
//#define USESTACK_REGVEC4

#define USESTACK_ARRAY2

// KD helpers
bool KDIsLeaf(__global const CLKDNode* node)
{
	return (node->flags & 3) == 3;
}
float KDGetSplitPosition(__global const CLKDNode* node)
{
	return node->splitPosition;
}
uint KDGetSplitAxis(__global const CLKDNode* node)
{
	return node->flags & 3;
}
uint KDGetChildId(__global const CLKDNode* node)
{
	return node->childId >> 2;
}
uint KDGetTriId(__global const CLKDNode* node)
{
	return node->triId >> 2;
}

bool intersectTriangle(const CLTriangle* tri, const CLRay* ray, float* rayT, float* b1, float* b2)
{
	const float denom = dot(tri->p, ray->dir);

	*rayT = (-1.0f * dot(tri->p, ray->org)) / denom;

	const float4 intPt = ray->org + (*rayT) * ray->dir; 

	*b1 = dot(intPt, tri->ep20);
	*b2 = dot(intPt, tri->ep01);

	return *b1 >= 0.0f && *b2 >= 0.0f && *b1 + *b2 <= 1.0f && *rayT > 0.0f;
}
/**/

bool intersectAABB(__global const CLAABB* aabb, const CLRay* ray, float* tnear, float* tfar)
{	
	const float4 dirInv= (float4)(1.0f) / ray->dir;

	const float4 tnear4 = dirInv * (aabb->min - ray->org);
    const float4 tfar4 = dirInv * (aabb->max - ray->org);

    const float4 t0 = min(tnear4, tfar4);
    const float4 t1 = max(tnear4, tfar4);

    const float _tnear = max(max(t0.x, t0.y), t0.z);
    const float _tfar = min(min(t1.x, t1.y), t1.z);

    *tnear = _tnear;
    *tfar = _tfar;

    return (_tfar >= _tnear) && (_tfar >= 0.0f);
}
/**/

inline void intersectKDTree(const CLRay* restrict ray, __global const CLTriangle* restrict tris,
							__global const CLAABB* restrict aabb, __global const CLKDNode* restrict kdnodePool, __global const int* restrict kdtriRegistry,
							__global const int* restrict kdnodetriIDs, __global const float* restrict kdnodeAABBpp, __global const int* restrict kdnodeRope,
							CLHitRecord* hitrec)
{

//#define PDandSS	
#define ROPEPP

#ifdef ROPEPP
	float gtNear = 0.0f;
	int rope;
	__global const CLKDNode* restrict node = kdnodePool;

	int xSign = ray->dir.x >= 0.0f;
	int ySign = ray->dir.y >= 0.0f;
	int zSign = ray->dir.z >= 0.0f;

	bool hasHit = false;

	do
	{
		float4 Pentry = ray->org + ray->dir * gtNear;	
		float org_axis, splitPos;
		uint splitAxis;

		while( !KDIsLeaf(node) )
		{	
			splitAxis = KDGetSplitAxis(node);
			splitPos = KDGetSplitPosition(node);

			org_axis = Pentry.x;
			org_axis = select(org_axis, Pentry.y, splitAxis==1);
			org_axis = select(org_axis, Pentry.z, splitAxis==2);

			int nodeoffset = KDGetChildId(node);
			nodeoffset = (org_axis <= splitPos) ? nodeoffset : nodeoffset+1;
			node = kdnodePool + nodeoffset;
		}

		int currLeafIndex = KDGetTriId(node);

		float localFarX = (kdnodeAABBpp[6*currLeafIndex + xSign*3    ] - ray->org.x) / ray->dir.x;
		float localFarY = (kdnodeAABBpp[6*currLeafIndex + ySign*3 + 1] - ray->org.y) / ray->dir.y;
		float localFarZ = (kdnodeAABBpp[6*currLeafIndex + zSign*3 + 2] - ray->org.z) / ray->dir.z;
		gtNear = min(localFarX, min(localFarY, localFarZ));

		//int exit_plane = localFarX < localFarY ? (localFarX < localFarZ ? 0 : 2) : (localFarY < localFarZ ? 1 : 2);
		int exit_plane = 0;
		exit_plane = (gtNear == localFarY) ? 1 : exit_plane;
		exit_plane = (gtNear == localFarZ) ? 2 : exit_plane;

		int lu = xSign;
		lu = select(lu, ySign, exit_plane==1);
		lu = select(lu, zSign, exit_plane==2);

		rope = kdnodeRope[currLeafIndex*6 + (exit_plane << 1) + lu];
		node = kdnodePool + rope;

		float _tHit, _b1, _b2;

		int kdTri = kdnodetriIDs[currLeafIndex];
		int currTri = kdtriRegistry[kdTri];

		while(currTri != -1)
		{
			const CLTriangle myTri = tris[currTri];
			const bool hit = intersectTriangle( &myTri, ray, &_tHit, &_b1, &_b2 );
		
			if( hit && _tHit < hitrec->tHit && _tHit <= gtNear )
			{				
				hitrec->triId = currTri;
				hitrec->tHit = _tHit;
				hitrec->b1 = _b1;
				hitrec->b2 = _b2;

				hasHit = true;
			}

			++kdTri;
			currTri = kdtriRegistry[kdTri];
		}

		if(hasHit)
			return;
	}
	while (rope != -1);
	return;
#endif
	
	
#ifdef PDandSS	
	// find global range for the ray
	float gtNear, gtFar;
	bool hasHit = false;

	if( !intersectAABB( aabb, ray, &gtNear, &gtFar ) )
		return;

	uint offsetRoot = 0;
	bool pushDown;

	__global const CLKDNode* restrict node;
	uint offsetNode;

	float tNear, tFar;
	tNear = tFar = gtNear;

#ifdef USESTACK_VECTOR4
	uint4 stackNode;
	float4 stackFar;
#endif
#ifdef USESTACK_ARRAY4
	uint stackNode[4];
	float stackFar[4];
#endif
#ifdef USESTACK_ARRAY2
	uint stackNode[2];
	float stackFar[2];
#endif

#if (defined USESTACK_VECTOR4) || (defined USESTACK_ARRAY4) || (defined USESTACK_ARRAY2)
	uint stackSize = 0;
#endif

	while( tFar < gtFar )
	{
#if (defined USESTACK_VECTOR4) || (defined USESTACK_ARRAY4) || (defined USESTACK_ARRAY2)
		if(stackSize == 0)
		{
#endif
			node = kdnodePool+offsetRoot;
			offsetNode = offsetRoot;
			tNear = tFar;
			tFar = gtFar;
			pushDown = true;
#if (defined USESTACK_VECTOR4) || (defined USESTACK_ARRAY4) || (defined USESTACK_ARRAY2)
		}
		else
		{
			--stackSize;
			pushDown = false;
			
#ifdef USESTACK_VECTOR4
			node = kdnodePool+stackNode.s0;
			offsetNode = stackNode.s0;
			tNear = tFar;
			tFar = stackFar.s0;

			stackNode.s012 = stackNode.s123;
			stackFar.s012 = stackFar.s123;
#endif
#ifdef USESTACK_ARRAY4
			node = kdnodePool+stackNode[0];
			offsetNode = stackNode[0];
			tNear = tFar;
			tFar = stackFar[0];

			stackNode[0] = stackNode[1];
			stackNode[1] = stackNode[2];
			stackNode[2] = stackNode[3];

			stackFar[0] = stackFar[1];
			stackFar[1] = stackFar[2];
			stackFar[2] = stackFar[3];
#endif
#ifdef USESTACK_ARRAY2
			node = kdnodePool+stackNode[0];
			offsetNode = stackNode[0];
			tNear = tFar;
			tFar = stackFar[0];

			stackNode[0] = stackNode[1];
			stackFar[0] = stackFar[1];
#endif
		}
#endif

		float tSplit, dir_axis, org_axis, splitPos;
		uint offsetFirst, offsetSecond, splitAxis;

		while( !KDIsLeaf(node) )
		{	
			splitAxis = KDGetSplitAxis(node);
			splitPos = KDGetSplitPosition(node);

			dir_axis = ray->dir.x;
			org_axis = ray->org.x;

			dir_axis = splitAxis==1 ? ray->dir.y : dir_axis;
			org_axis = splitAxis==1 ? ray->org.y : org_axis;

			dir_axis = splitAxis==2 ? ray->dir.z : dir_axis;
			org_axis = splitAxis==2 ? ray->org.z : org_axis;

			//switch ( splitAxis )
			//{
			//	case 1: 
			//		dir_axis = ray->dir.y;
			//		org_axis = ray->org.y;
			//		break;
			//
			//	case 2: 
			//		dir_axis = ray->dir.z;
			//		org_axis = ray->org.z;
			//		break;
			//}

			//dir_axis = select(dir_axis, ray->dir.y, splitAxis==1);
			//org_axis = select(org_axis, ray->org.y, splitAxis==1);

			//dir_axis = select(dir_axis, ray->dir.z, splitAxis==2);
			//org_axis = select(org_axis, ray->org.z, splitAxis==2);

			//dir_axis = ((float*)&(ray->dir))[splitAxis];
			//org_axis = ((float*)&(ray->org))[splitAxis];

			tSplit = (splitPos - org_axis) / dir_axis;
			offsetNode = (org_axis < splitPos) || (org_axis == splitPos && dir_axis < 0.0f);

			offsetFirst = KDGetChildId(node) + 1 - offsetNode;
			offsetSecond = KDGetChildId(node) + offsetNode;
						
			if( tSplit > tFar || tSplit <= 0.0f)
			{
				offsetNode = offsetFirst;
			}
			else if( tSplit <= tNear )
			{
				offsetNode = offsetSecond;
			}
			else
			{
#ifdef USESTACK_VECTOR4
				stackNode = (uint4)(offsetSecond, stackNode.s012);
				stackFar = (float4)(tFar, stackFar.s012);
				
				//stackSize = min(stackSize+1, (uint)(4));
				stackSize = (stackSize < 3) ? stackSize+1 : 4;
#endif
#ifdef USESTACK_ARRAY4
				stackNode[3] = stackNode[2];
				stackNode[2] = stackNode[1];
				stackNode[1] = stackNode[0];
				stackNode[0] = offsetSecond;

				stackFar[3] = stackFar[2];	
				stackFar[2] = stackFar[1];	
				stackFar[1] = stackFar[0];				
				stackFar[0] = tFar;

				//stackSize = min(stackSize+1, (uint)(4));
				stackSize = (stackSize < 3) ? stackSize+1 : 4;
#endif
#ifdef USESTACK_ARRAY2
				stackNode[1] = stackNode[0];
				stackNode[0] = offsetSecond;

				stackFar[1] = stackFar[0];				
				stackFar[0] = tFar;

				//stackSize = min(stackSize+1, (uint)(2));
				stackSize = (stackSize == 0) ? 1 : 2;
#endif
				offsetNode = offsetFirst;
				tFar = tSplit;

				pushDown = false;
			}

			if(pushDown)
				offsetRoot = offsetNode;

			node = kdnodePool+offsetNode;
		}

		float _tHit, _b1, _b2;

		//int kdTri = KDGetTriId(node);
		int kdTri = kdnodetriIDs[KDGetTriId(node)]; //rope indirection
		int currTri = kdtriRegistry[kdTri];

		while(currTri != -1)
		{
			const CLTriangle myTri = tris[currTri];
			const bool hit = intersectTriangle( &myTri, ray, &_tHit, &_b1, &_b2 );
		
			if( hit && _tHit < hitrec->tHit && _tHit <= (tFar + 0.0001f) )
			{				
				hitrec->triId = currTri;
				hitrec->tHit = _tHit;
				hitrec->b1 = _b1;
				hitrec->b2 = _b2;

				hasHit = true;
			}

			++kdTri;
			currTri = kdtriRegistry[kdTri];
		}

		if(hasHit)
			return;
	}

#endif
}

inline void intersectBVH(const CLRay* restrict ray, __global const CLTriangle* restrict tris, __global const int2* mortonPairs,
						__global const CLAABB* bvhNodeAABBs, __global const CLBVHNode* bvhNodeLinks, const int leafOffset, const int dynamicStart,
						 CLHitRecord* hitrec)
{
	int levelIndex = 0;
	int currNode = 0;

	do
	{
		if( currNode < leafOffset )
		{
			const int childL = bvhNodeLinks[currNode].nodeData1;
			const int childU = bvhNodeLinks[currNode].nodeData2;

			float tnearL, tfarL;
			float tnearU, tfarU;

			intersectAABB( bvhNodeAABBs+childL, ray, &tnearL, &tfarL );
			intersectAABB( bvhNodeAABBs+childU, ray, &tnearU, &tfarU );

			const int wantLL = (tnearL <= tfarL) && (tnearL < hitrec->tHit) && (tfarL >= 0.0f);
			const int wantUU = (tnearU <= tfarU) && (tnearU < hitrec->tHit) && (tfarU >= 0.0f);

			if(wantLL | wantUU)
			{
				levelIndex <<= 1;

				if(wantLL ^ wantUU)
				{
					++levelIndex;
					currNode = wantLL * childL + wantUU * childU;
				}
				else
				{
					currNode = tnearL < tnearU ? childL : childU;
				}

				continue;
			}
		}
		else
		{
			// leaf node, intersect tris
			float _tHit, _b1, _b2;

			const int from = bvhNodeLinks[currNode].nodeData1;
			const int count = bvhNodeLinks[currNode].nodeData2;

			for(int currTri = from; currTri < from+count; ++currTri)
			{
				const CLTriangle myTri = tris[currTri + dynamicStart];
				const bool triHit = intersectTriangle( &myTri, ray, &_tHit, &_b1, &_b2 );
					
				if( triHit && _tHit < hitrec->tHit )
				{
					hitrec->triId = mortonPairs[currTri].s0 + dynamicStart;
					//hitrec->triId = currTri + dynamicStart;
					hitrec->tHit = _tHit;
					hitrec->b1 = _b1;
					hitrec->b2 = _b2;
				}		
			}
		}

		++levelIndex;

		while( (levelIndex & 1) == 0 )
		{
			// get parent
			currNode = bvhNodeLinks[currNode].nodeData3;
			levelIndex >>= 1;
		}

		// get sibling
		currNode = bvhNodeLinks[currNode].nodeData0 >> 1;
	}
	while(currNode);
}

//__kernel __attribute__((reqd_work_group_size(8,8,1)))
__kernel
void intersectScene_KDTree(__global const float* restrict rayweights, __global const CLRay* restrict rays,
						   __global const CLTriangle*  tris, __global const CLAABB*  aabb, __global const CLKDNode*  kdnodePool, __global const int*  kdtriRegistry,
						   __global const int* restrict kdnodetriIDs, __global const float* restrict kdnodeAABBpp, __global const int* restrict kdnodeRope,
						   __global CLHitRecord* hitrecs)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;

	if( rayweights[currIndex] == 0.0f )
		return;

	CLHitRecord currHitRec = hitrecs[currIndex];

	const CLRay currRay = rays[currIndex];

	intersectKDTree(&currRay, tris, aabb, kdnodePool, kdtriRegistry, kdnodetriIDs, kdnodeAABBpp, kdnodeRope, &currHitRec);

	hitrecs[currIndex] = currHitRec;
}

//__kernel __attribute__((reqd_work_group_size(8,8,1)))
__kernel
void intersectScene_KDTreeEX(__global const float* restrict rayweights, __global const CLRay* restrict rays,
							 __global const CLTriangle* restrict tris, __global const float2*  uvs, __global const int*  matIds, __global const CLMaterial*  materials, __global const uchar4* texturesMaskMap,
							 __global const CLAABB* restrict aabb, __global const CLKDNode* restrict kdnodePool, __global const int* restrict kdtriRegistry,
							 __global const int* restrict kdnodetriIDs, __global const float* restrict kdnodeAABBpp, __global const int* restrict kdnodeRope,
							 __global CLHitRecord* hitrecs)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;

	if( rayweights[currIndex] == 0.0f )
		return;

	CLHitRecord currHitRec = hitrecs[currIndex];

	CLRay currRay = rays[currIndex];
	float rayOrgX = currRay.org.x;

	while(true)
	{
		currHitRec.triId = INT_MAX;
		currHitRec.tHit = FLT_MAX;

		intersectKDTree(&currRay, tris, aabb, kdnodePool, kdtriRegistry, kdnodetriIDs, kdnodeAABBpp, kdnodeRope, &currHitRec);

		if(currHitRec.triId != INT_MAX)
		{
			int matId = matIds[currHitRec.triId];
			
			float2 uv = uvs[3*currHitRec.triId    ] * (1.0f - currHitRec.b1 - currHitRec.b2) +
						uvs[3*currHitRec.triId + 1] * currHitRec.b1 +
						uvs[3*currHitRec.triId + 2] * currHitRec.b2;

			float4 maskColor = (float4)(1.0f, 0.0f, 0.0f, 9999.0f);
		
			if((materials+matId)->texMaskFlag == 1)
			{
				maskColor = textureBilinearLookup(texturesMaskMap + (materials+matId)->texMaskOffset, (int2)((materials+matId)->texMaskWidth, (materials+matId)->texMaskHeight), uv.yx);
			}

			currHitRec.tHit += 0.0001f;
            currRay.org += currRay.dir * currHitRec.tHit;

			currHitRec.tHit = (currRay.org.x - rayOrgX) / currRay.dir.x;
           
			if(maskColor.x >= 0.3f)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	hitrecs[currIndex] = currHitRec;
}
/**/

//__kernel __attribute__((reqd_work_group_size(8,8,1)))
__kernel
void intersectScene_BVH(__global const float* rayweights, __global const CLRay* rays,  __global const CLTriangle* tris, __global const int2* mortonPairs,
						__global const CLAABB* bvhNodeAABBs, __global const CLBVHNode* bvhNodeLinks, const int leafOffset, const int dynamicStart,
						__global CLHitRecord* hitrecs)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;

	if( rayweights[currIndex] == 0.0f )
		return;

	CLHitRecord currHitRec = hitrecs[currIndex];

	const CLRay currRay = rays[currIndex];

	intersectBVH(&currRay, tris, mortonPairs, bvhNodeAABBs, bvhNodeLinks, leafOffset, dynamicStart, &currHitRec);

	hitrecs[currIndex] = currHitRec;
}