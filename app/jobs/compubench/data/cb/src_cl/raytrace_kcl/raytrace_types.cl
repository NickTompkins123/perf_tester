typedef struct
{
	float4	pos;
	float4	dirTL, dirTR, dirBL, dirBR;
} CLCamera;

typedef struct
{
	float4	org;
	float4	dir;
} CLRay;

typedef struct
{
	float4	rxOrg, ryOrg;
	float4	rxDir, ryDir;
} CLRayDifferential;

typedef struct
{
	float4 dpdu, dpdv;
} CLGeometryDifferential;

typedef struct
{
	//float4	dpdx, dpdy;
	float	dudx, dvdx, dudy, dvdy; 
} CLIntersectionDifferential;

//typedef struct
//{
//	int	vertId0, vertId1, vertId2, matId;
//} CLTriangle;

typedef struct
{
	float4	p, ep20, ep01;
} CLTriangle;

typedef struct
{
	int		triId;					// if nohit - triId = INT_MAX
	float	tHit, b1, b2;
} CLHitRecord;

typedef struct
{
	int		matId;					// if nohit - matId = INT_MAX
	float   tHit;
	int		pad0, pad1;

	float4	hitPosition;
	float4	hitNormal;

	float4	hitKD;
	float4	hitKR;
} CLHitData;

typedef struct
{
	int		texOffset;
	int		texWidth;
	int		texHeight;
	int		texFlag;

	int		texMaskOffset;
	int		texMaskWidth;
	int		texMaskHeight;
	int		texMaskFlag;

	int		texLightOffset;
	int		texLightWidth;
	int		texLightHeight;
	int		texLightFlag;

	int		texNormalOffset;
	int		texNormalWidth;
	int		texNormalHeight;
	int		texNormalFlag;
} CLMaterial;

typedef struct
{
	float4	pos;
	float4	u, v, w;
} CLLight;

typedef struct
{
	float4	min, max;
} CLAABB;

typedef struct
{
	int nodeData0;	// parentNode
	int nodeData1;	// childLowerNode OR startTri
	int nodeData2;	// childUpperNode OR countTri
	int nodeData3;	// node ray mask - traversal order
} CLBVHNode;

typedef struct
{
	union
	{
		float splitPosition;	// interior
	};
	union
	{
		unsigned int flags;		// both: interior = axis - 0,1,2 | leaf = leaf indicator - 3
		unsigned int childId;	// interior
		unsigned int triId;		// leaf
	};
} CLKDNode;

typedef struct
{
	int filter;
	int iterative;
	int toneMapping;
	int depth;
	int iterationNum;
	int avgLuminance;
	int filterType;
	int DoF;

}CLParameters;

typedef struct
{
	float intensity;
	float size;
	float turbidity;
	float exposure;
	float2 xyYcolor;
}CLSun;