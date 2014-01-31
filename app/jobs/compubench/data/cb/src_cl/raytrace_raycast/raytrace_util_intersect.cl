// ((unsigned int)(-1)) >> 2
#define KDTRIEMPTY 1073741823

#define USESTACK_VECTOR4
//#define USESTACK_ARRAY4
//#define USESTACK_ARRAY2

inline bool intersectTriangle(const CLRay* ray, const float4 plane, const float4 edgePlane20, const float4 edgePlane01, float* rayT, float* b1, float* b2)
{
	const float denom = dot(plane, ray->dir);

	*rayT = (-1.0f * dot(plane, ray->org)) / denom;

	const float4 intPt = ray->org + (*rayT) * ray->dir; 

	*b1 = dot(intPt, edgePlane20);
	*b2 = dot(intPt, edgePlane01);

	bool cond = *b1 >= 0.0f && *b2 >= 0.0f && *b1 + *b2 <= 1.0f && *rayT > 0.0f;
	return cond;
}

inline bool IntersectAABB3x2(__global const CLAABB3x2* aabb, const CLRay* ray, float* tnear, float* tfar)
{
	*tnear = -FLT_MAX;
	*tfar = FLT_MAX;

	float2 tx = (aabb->x - ray->org.x) / ray->dir.x;
	float2 ty = (aabb->y - ray->org.y) / ray->dir.y;
	float2 tz = (aabb->z - ray->org.z) / ray->dir.z;

	*tnear = min(tx.s0, tx.s1);
	*tfar = max(tx.s0, tx.s1);

	*tnear = max(*tnear, min(ty.s0, ty.s1));
	*tfar = min(*tfar, max(ty.s0, ty.s1));
	
	*tnear = max(*tnear, min(tz.s0, tz.s1));
	*tfar = min(*tfar, max(tz.s0, tz.s1));

	return (*tfar >= 0.0f) && (*tfar >= *tnear);
}

bool intersectKDTree(const CLRay* ray, __global const CLTriangle* tris, __global const CLAABB3x2* aabb, __global const CLKDNode* kdnodePool, __global const int* kdtriRegistry,
					 CLHitRecord* hitrec)
{
	bool hasHit = false;

	// find global range for the ray
	float gtNear, gtFar;

	if( !IntersectAABB3x2( aabb, ray, &gtNear, &gtFar ) )
		return false;

	uint offsetRoot = 0;
	bool pushDown;

	__global const CLKDNode* node;
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

			//dir_axis = select(dir_axis, ray->dir.x, splitAxis==0);
			//org_axis = select(org_axis, ray->org.x, splitAxis==0);

			//dir_axis = select(dir_axis, ray->dir.y, splitAxis==1);
			//org_axis = select(org_axis, ray->org.y, splitAxis==1);

			//dir_axis = select(dir_axis, ray->dir.z, splitAxis==2);
			//org_axis = select(org_axis, ray->org.z, splitAxis==2);

			dir_axis = (splitAxis==0) ? ray->dir.x : dir_axis;
			org_axis = (splitAxis==0) ? ray->org.x : org_axis;

			dir_axis = (splitAxis==1) ? ray->dir.y : dir_axis;
			org_axis = (splitAxis==1) ? ray->org.y : org_axis;

			dir_axis = (splitAxis==2) ? ray->dir.z : dir_axis;
			org_axis = (splitAxis==2) ? ray->org.z : org_axis;

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

		if( KDGetTriId(node) != KDTRIEMPTY )
		{
			int triListId = KDGetTriId(node);
			int currTri = kdtriRegistry[triListId];

			while(currTri != -1)
			{
				float _tHit, _b1, _b2;
				/**/
				bool hit = intersectTriangle( ray, tris[currTri].plane, tris[currTri].edgePlane20, tris[currTri].edgePlane01, &_tHit, &_b1, &_b2 );

				if( hit && _tHit < hitrec->tHit && _tHit <= (tFar + 0.0001f) )
				{
					hasHit = true;
					
					hitrec->triId = currTri;
					hitrec->tHit = _tHit;
					hitrec->b1 = _b1;
					hitrec->b2 = _b2;
				}

				++triListId;
				currTri = kdtriRegistry[triListId];
			}
		}

		if( hasHit )
			return true;
	}
	
	return hasHit;
}