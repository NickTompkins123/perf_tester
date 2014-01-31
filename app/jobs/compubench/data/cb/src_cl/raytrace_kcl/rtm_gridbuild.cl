typedef struct
{
	float4	min, max;
} CLAABB;

typedef struct
{
	int4 dim;
	float4 size;
	float4 step;
	float4 sizeInv;
	float4 stepInv;
} CLGrid;

__kernel
void gridOverlapCalc(__global const CLAABB* triaabb, __global const CLAABB* gridaabb, __global const CLGrid* grid, __global int* overlap)
{
	int wiIdG = get_global_id(0);

	float4 lowerId = triaabb[wiIdG].min - gridaabb->min;
	int lowerX = lowerId.x * grid->stepInv.x;
	int lowerY = lowerId.y * grid->stepInv.y;
	int lowerZ = lowerId.z * grid->stepInv.z;

	float4 upperId = triaabb[wiIdG].max - gridaabb->min;
	int upperX = upperId.x * grid->stepInv.x;
	int upperY = upperId.y * grid->stepInv.y;
	int upperZ = upperId.z * grid->stepInv.z;

	int count = (upperX-lowerX + 1)*(upperY-lowerY + 1)*(upperZ-lowerZ + 1);

	overlap[wiIdG] = count;
}

//
// scan overlap
//

__kernel
void gridWritePairs(__global const CLAABB* triaabb, __global const CLAABB* gridaabb, __global const CLGrid* grid, __global const int* overlap, __global int2* gridpairs)
{
	int wiIdG = get_global_id(0);

	float4 lowerId = triaabb[wiIdG].min - gridaabb->min;
	int lowerX = lowerId.x * grid->stepInv.x;
	int lowerY = lowerId.y * grid->stepInv.y;
	int lowerZ = lowerId.z * grid->stepInv.z;

	float4 upperId = triaabb[wiIdG].max - gridaabb->min;
	int upperX = upperId.x * grid->stepInv.x;
	int upperY = upperId.y * grid->stepInv.y;
	int upperZ = upperId.z * grid->stepInv.z;

	for(int k = 0; k <= upperZ-lowerZ; ++k)
	{
		for(int j = 0; j <= upperY-lowerY; ++j)
		{
			for(int i = 0; i <= upperX-lowerX; ++i)
			{
				int offset = i + j*(upperX-lowerX+1) + k*(upperX-lowerX+1)*(upperY-lowerY+1);
				
				gridpairs[ overlap[wiIdG]+offset ].s0 = wiIdG;
				gridpairs[ overlap[wiIdG]+offset ].s1 = lowerX + i + (lowerY + j)*grid->dim.x + (lowerZ + k)*grid->dim.x*grid->dim.y;
			}
		}
	}
}

//
// sort int2 pairs by s1
//

__kernel
void gridWriteCellsInit(__global int2* gridcell)
{
	int wiIdG = get_global_id(0);

	gridcell[wiIdG] = (int2)(INT_MAX, INT_MAX);
}

__kernel
void gridWriteCells(__global const int2* gridpairs, __global int2* gridcells)
{
	int wiIdG = get_global_id(0);

	int2 currElem = gridpairs[wiIdG];
	int2 nextElem = gridpairs[wiIdG+1];

	if(currElem.s1 != nextElem.s1)
	{
		gridcells[ currElem.s1 ].s1 = wiIdG;
		gridcells[ nextElem.s1 ].s0 = wiIdG+1;
	}
}

__kernel
void gridWriteCells_Left(__global const int2* gridpairs, __global int2* gridcells)
{
	int wiIdG = get_global_id(0);

	int2 currElem = gridpairs[wiIdG];
	int2 nextElem = gridpairs[wiIdG+1];

	if(currElem.s1 != nextElem.s1)
	{
		gridcells[ currElem.s1 ].s1 = wiIdG;
		//gridcells[ nextElem.s1 ].s0 = wiIdG+1;
	}
}

__kernel
void gridWriteCells_Right(__global const int2* gridpairs, __global int2* gridcells)
{
	int wiIdG = get_global_id(0);

	int2 currElem = gridpairs[wiIdG];
	int2 nextElem = gridpairs[wiIdG+1];

	if(currElem.s1 != nextElem.s1)
	{
		//gridcells[ currElem.s1 ].s1 = wiIdG;
		gridcells[ nextElem.s1 ].s0 = wiIdG+1;
	}
}

__kernel
void gridWriteCellPad(__global const int2* gridpairs, __global int2* gridcells, const int start, const int end)
{
	gridcells[ gridpairs[start].s1 ].s0 = start;

	gridcells[ gridpairs[end].s1 ].s1 = end;
}