//#define WGSIZE 256

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

	int pout = 0;
	int pin = 1;

	#pragma unroll
	for(int offset = 1; offset < WGSIZE; offset *= 2)
	{
		// swap double buffer indices 
		pout = 1 - pout; 
		pin = 1 - pout;

		if(wiLId >= offset)
		{
			sData[pout*WGSIZE + wiLId] = sData[pin*WGSIZE + wiLId] + sData[pin*WGSIZE + wiLId-offset];
		}
		else
		{
			sData[pout*WGSIZE + wiLId] = sData[pin*WGSIZE + wiLId];
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	
	// write block sum result to global mem
	if(wiLId == 0)
	{
		sum[get_group_id(0)] = sData[(1+pout)*WGSIZE - 1];
	}
	
	// write result for this block to global mem
	if(wiGId < n)
	{
		data[wiGId] = sData[pout*WGSIZE + wiLId] - x;
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