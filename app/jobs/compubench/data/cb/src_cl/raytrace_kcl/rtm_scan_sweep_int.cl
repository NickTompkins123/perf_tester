__kernel
__attribute__((reqd_work_group_size(WGSIZE, 1, 1)))
void scanTraverse_INT(__global int* data, __global int* sum, const int n, __local int* sData)
{	
	int wiGId = get_global_id(0);
	int wiLId = get_local_id(0);
	
	// read input data into local memory
	sData[wiLId] = (wiGId < n) ? data[wiGId] : 0;

	int offset = 1;

	for(int d = WGSIZE >> 1; d > 0; d >>= 1)
	{
		barrier(CLK_LOCAL_MEM_FENCE);

		if(wiLId < d)
		{
			int ai = offset*(2*wiLId + 1) - 1;
			int bi = offset*(2*wiLId + 2) - 1;
			
			sData[bi] += sData[ai];
		}
		
		offset *= 2;
	}

	if(wiLId == 0)
	{
		sum[get_group_id(0)] = sData[WGSIZE - 1];
		sData[WGSIZE - 1] = 0;
	}

	for(int d = 1; d < WGSIZE; d *= 2)
	{
		offset >>= 1;

		barrier(CLK_LOCAL_MEM_FENCE);
		
		if(wiLId < d)
		{
			int ai = offset*(2*wiLId + 1) - 1;
			int bi = offset*(2*wiLId + 2) - 1;
			
			int t = sData[ai];
			sData[ai] = sData[bi];
			sData[bi] += t;
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// write result for this block to global mem
	if(wiGId < n)
	{
		data[wiGId] = sData[wiLId];
	}
}

__kernel
void scanBlockAdd_INT(__global const int* input, __global int* output, const int n)
{	
	int wiIdG = get_global_id(0);
	int wiIdL = get_local_id(0);
	int wgId = get_group_id(0);

	__local int sdata[1];

	if(wiIdL == 0)
	{
		sdata[0] = input[wgId];
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	if(wiIdG < n)
	{
		output[wiIdG] += sdata[0];
	}
}