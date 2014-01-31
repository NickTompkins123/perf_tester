__kernel
__attribute__((reqd_work_group_size(WGSIZE, 1, 1)))
void scanTraverse_INT(__global int* data, __global int* sum, const int n, __local int* sData)
{
	int wiGId = get_global_id(0);
	int wiLId = get_local_id(0);

	// read input data into local memory
	int x = (wiGId < n) ? data[wiGId] : 0;
	sData[wiLId] = x;
	barrier(CLK_LOCAL_MEM_FENCE);

	#pragma unroll
	for(int offset = 1; offset < WGSIZE; offset *= 2)
	{
		int t;
		if(wiLId >= offset)
		{
			t = sData[wiLId - offset];
		}
		barrier(CLK_LOCAL_MEM_FENCE);

		if(wiLId >= offset)
 		{
			sData[wiLId] += t;
 		}
 		barrier(CLK_LOCAL_MEM_FENCE);
	}
	
	// write block sum result to global mem
	if(wiLId == 0)
	{
		sum[get_group_id(0)] = sData[WGSIZE - 1];
	}
	
	// write result for this block to global mem
	if(wiGId < n)
	{
		data[wiGId] = sData[wiLId] - x;
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