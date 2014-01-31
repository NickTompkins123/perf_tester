__kernel
void radixKeyWrite(__global const int2* data, __global int* keys, const int bitselect, const int n)
{
	int wiIdG = get_global_id(0);

	if(wiIdG < n)
		keys[wiIdG] = !(bitselect & data[wiIdG].s0);
}

__kernel
void radixOffsetCalc(__global int* offsets, __global const int* keys, __global const int* totalfalses, const int n)
{
	int wiIdG = get_global_id(0);

	if( wiIdG < n && keys[wiIdG] == 0 )
	{
		offsets[wiIdG] = wiIdG - offsets[wiIdG] + totalfalses[0];
	}
}

__kernel
void radixScatter(__global const int2* data0, __global int2* data1, __global const int* offsets, const int n)
{
	int wiIdG = get_global_id(0);

	if(wiIdG < n)
		data1[ offsets[wiIdG] ] = data0[ wiIdG ];
}