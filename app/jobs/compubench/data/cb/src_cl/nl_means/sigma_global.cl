__kernel
void rgba_to_intensity
(
	global const uchar4 *restrict input,
	global int *restrict output
)
{
	const int k_index = (int)get_global_id(0);

	const int4 rgba = convert_int4(input[k_index]);
	const int intensity = (rgba.s0 + rgba.s1 + rgba.s2) / 3;

	output[k_index] = intensity;
}

__kernel
void sigma_filter
(
	__global const int *restrict intensities,
	__global uchar4 *restrict input,
	__global uchar4 *restrict output,
	const int2 sizes
)
{
	const int2 k = (int2)((int)get_global_id(0), (int)get_global_id(1));
	if(k.s0 >= sizes.s0 || k.s1 >= sizes.s1) return;
	const int k_index = k.s1 * sizes.s0 + k.s0;

	const int k_intensity = intensities[k_index];

	int4 mean = (int4)(0);
	int count = 0;
	for(int y = -J; y <= J; ++y)
	{
		for(int x = -J; x <= J; ++x)
		{
			const int2 k_plus_n = clamp(k + (int2)(x, y), (int2)(0, 0), sizes - 1);
			const int k_plus_n_index = k_plus_n.s1 * sizes.s0 + k_plus_n.s0;

			const int k_plus_n_intensity = intensities[k_plus_n_index];

			const int good = abs_diff(k_plus_n_intensity, k_intensity) < 2*sigma_int;

			mean += good * convert_int4(input[k_plus_n_index]);
			count += good;
		}
	}

	mean <<= 8;
	mean /= count;
	mean >>= 8;

	output[k_index] = convert_uchar4_sat(mean);
}
