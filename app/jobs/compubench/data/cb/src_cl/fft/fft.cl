__kernel __attribute__((reqd_work_group_size (16,16,1)))
void transpose( __global const float2* Data_In, __global float2* Data_Out )
{
	unsigned int x = get_global_id(0);
	unsigned int y = get_global_id(1);

	unsigned int width = get_global_size(0);
	unsigned int height = get_global_size(1);
 
	unsigned int srcId = y * width + x;
	unsigned int dstId = x * height + y;
 
	Data_Out[dstId] = Data_In[srcId];
}

__kernel __attribute__((reqd_work_group_size (16,16,1)))
void transposeLocal( __global const float2* Data_In, __global float2* Data_Out )
{
	local float2 aux[256]; 
	int bx = get_group_id(0), 
		by = get_group_id(1);
	int ix = get_local_id(0), 
		iy = get_local_id(1);

	int w = get_global_size(0);
	int h = get_global_size(1);

	by = (by+bx)%get_num_groups(1);

	Data_In += (bx*16)+(by*16)*w; 
	Data_Out += (by*16)+(bx*16)*h;

	aux[iy+ix*16] = Data_In[ix+w*iy]; 
	barrier(CLK_LOCAL_MEM_FENCE);
	Data_Out[ix+h*iy] = aux[ix+iy*16];
}

__kernel void FFTBitRev( __global float2* Data_In, __global float2* Data_Out )
{
	int gId = get_global_id(0)-get_global_offset(0);

	int gSize = get_global_size(0);

	int numBits = log2((float)gSize);

	int reverse_num = 0;
	for(int i = 0; i < numBits; ++i)
	{
		int currBit = !((gId & (1 << i)) == 0);
		reverse_num |= (currBit << ((numBits - 1) - i));
	}
  
    Data_Out[get_global_offset(0)+reverse_num] = Data_In[get_global_offset(0)+gId];
}

__kernel void FFT2( __global float2* data, const int stage, const int stagePow, const int dir ) 
{   	
    int gId = get_global_id(0)-get_global_offset(0);

	int distance = 1 << stage;
	int lmask = distance - 1;

	int leftId = ((gId >> stage) << (stage+1)) + (gId & lmask);
	int rightId = leftId + distance;

    float2 in0 = data[get_global_offset(0)+leftId];
	float2 in1 = data[get_global_offset(0)+rightId];

	float angle = dir * (-M_PI_F*(rightId%stagePow)) / stagePow;

	float2 tw, bf;
	tw.x = cos(angle);
	tw.y = sin(angle);
	bf.x = tw.x*in1.x - tw.y*in1.y;
	bf.y = tw.x*in1.y + tw.y*in1.x;
	in1.x = bf.x;
	in1.y = bf.y;

	data[get_global_offset(0)+leftId] = in0 + in1;
	data[get_global_offset(0)+rightId] = in0 - in1;
}

__kernel void FFTScale( __global float2* Data_In )
{
	unsigned int x = get_global_id(0);
	unsigned int y = get_global_id(1);

	unsigned int size = get_global_size(0) * get_global_size(1);
 
	unsigned int id = y * get_global_size(0) + x;
 
	Data_In[id] = Data_In[id] / (float2)(size,size);
}