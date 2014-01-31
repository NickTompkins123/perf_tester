__kernel
void radixKeyWrite(__global const int* data, __global int* keys, const int bitselect)
{
	int wiIdG = get_global_id(0);

	keys[wiIdG] = !(bitselect & data[wiIdG]);
}

__kernel
void radixOffsetCalc(__global int* offsets, __global const int* keys, __global const int* totalfalses)
{
	int wiIdG = get_global_id(0);

	if( keys[wiIdG] == 0 )
	{
		offsets[wiIdG] = wiIdG - offsets[wiIdG] + totalfalses[0];
	}
}

__kernel
void radixScatter(__global const int* data0, __global int* data1, __global const int* offsets)
{
	int wiIdG = get_global_id(0);

	data1[ offsets[wiIdG] ] = data0[ wiIdG ];
}