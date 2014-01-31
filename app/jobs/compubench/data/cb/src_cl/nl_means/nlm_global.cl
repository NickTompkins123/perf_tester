__kernel
void nlm_to_intensity
(
	__global const uchar4 *restrict input,
	__global int *restrict output
)
{
	const int x = convert_int(get_global_id(0));
	const int y = convert_int(get_global_id(1));
	const int width = convert_int(get_global_size(0));

	const int4 rgba = convert_int4(input[y*width + x]);
	const int intensity = (rgba.s0 + rgba.s1 + rgba.s2) / 3;

	output[y*width + x] = intensity;
}

__kernel
void nlm_pack_neighborhood
(
	__global const int *restrict img_intensity,
	__global uchar8 *restrict img_neighborhood
)
{
	const int width = convert_int(get_global_size(0));
	const int height = convert_int(get_global_size(1));

	const int2 center = (int2)((int)get_global_id(0), (int)get_global_id(1));

	uchar8 neighborhood;

	#pragma unroll 3
	for(int y = -K; y <= K; ++y)
	{
		#pragma unroll 3
		for(int x = -K; x <= K; ++x)
		{
			if(x != 0 && y != 0)
			{
				const int2 p = clamp(center + (int2)(x, y), (int2)(0, 0), (int2)(width - 1, height - 1));

				neighborhood.s0 = img_intensity[p.s1 * width + p.s0];
				neighborhood = neighborhood.s12345670;
			}
		}
	}

	img_neighborhood[center.s1 * width + center.s0] = neighborhood;
}

__kernel
void nlm_filter
(
	__global const uchar4 *restrict input,
	__global const int *restrict input_intensity,
	__global const uchar8 *restrict input_neighborhood,
	__global uchar4 *restrict output,
	const int2 sizes
)
{
	const int2 i = (int2)(convert_int(get_global_id(0)), convert_int(get_global_id(1)));
	if(i.s0 >= sizes.s0 || i.s1 >= sizes.s1) return;

	const int i_index = i.s1 * sizes.s0 + i.s0;

	float4 mean = (float4)(0.0f);
	float z = 0.0f;

	const int8 nbh_i = convert_int8(input_neighborhood[i_index]);
	const int c_i = input_intensity[i_index];

	//#pragma unroll 15
	for(int y = -J; y <= J; ++y)
	{
		//#pragma unroll 15
		for(int x = -J; x <= J; ++x)
		{
			const int2 j = clamp(i + (int2)(x, y), (int2)(0, 0), sizes - 1);
			const int j_index = j.s1 * sizes.s0 + j.s0;

			/*int8 nbh = convert_int8(input_neighborhood[j_index]) - nbh_i;
			nbh *= nbh;
			nbh.s0123 += nbh.s4567;
			nbh.s01 += nbh.s34;
			int c = input_intensity[j_index] - c_i;
			c *= c;

			float seor = convert_float(nbh.s0 + nbh.s1 + c);*/

			int8 nbh = convert_int8(input_neighborhood[j_index]) - nbh_i;
			nbh *= nbh;
			int c = input_intensity[j_index] - c_i;
			c *= c;

			const float seor = (float)(nbh.s0 + nbh.s1 + nbh.s2 + nbh.s3 + nbh.s4 + nbh.s5 + nbh.s6 + nbh.s7 + c);

			const float w = exp(-seor/(h*h));
			z += w;
			mean += convert_float4(input[j_index]) * w;
		}
	}

	mean /= z;

	output[i_index] = convert_uchar4_sat(mean);
}
