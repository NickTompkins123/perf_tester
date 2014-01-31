//#define FLT_MAX 	0x1.fffffep127f
//#define FLT_MIN 	0x1.0p-126f
//#define INT_MAX 	2147483647
//#define INT_MIN 	(-2147483647- 1)

typedef struct
{
	float4	pos;
	float4	dirTL, dirTR, dirBL, dirBR;
} CLCamera;
/**/

typedef struct
{
	float4	org;
	float4	dir;
} CLRay;
/**/

typedef struct
{
	float4	plane, edgePlane20, edgePlane01;
} CLTriangle;
/**/

typedef struct
{
	int	triId;			// if nohit - triId = INT_MAX
	float	tHit, b1, b2;
} CLHitRecord;
/**/

typedef struct
{
	float4	hitPosition;
	float4	hitNormal;
	float4	hitColor;
} CLHitData;
/**/

typedef struct
{
	float2	x, y, z;
} CLAABB3x2;
/**/

typedef struct
{
	float4	diffColor;		// currently unused
	float4	specColor;		// currently unused

	float	shininess;		// currently unused
	float	kr;
	float	kt;
	float	ior;			// currently unused

	int		texOffset;
	int		texWidth;
	int		texHeight;
	int		pad1;
} CLMaterial;
/**/

typedef struct
{
	float splitPosition;
	unsigned int data;
} CLKDNode;
/**/

// KD helpers
inline float KDGetSplitPosition(__global const CLKDNode* node)
{
	return node->splitPosition;
}
inline bool KDIsLeaf(__global const CLKDNode* node)
{
	return (node->data & 3) == 3;
}
inline uint KDGetSplitAxis(__global const CLKDNode* node)
{
	return node->data & 3;
}
inline uint KDGetChildId(__global const CLKDNode* node)
{
	return node->data >> 2;
}
inline uint KDGetTriId(__global const CLKDNode* node)
{
	return node->data >> 2;
}