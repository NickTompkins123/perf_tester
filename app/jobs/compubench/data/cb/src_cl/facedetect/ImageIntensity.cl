__kernel void createIntensityImage(read_only image2d_t source, write_only image2d_t result)
{
	uint x = get_global_id(0);
	uint y = get_global_id(1);
	int2 coords = (int2)(x, y);
	
	const sampler_t smp = CLK_FILTER_NEAREST;
	
	uint4 color = read_imageui(source, smp, coords);
	color.w = color.x * 0.2989 + color.y * 0.5870 + color.z * 0.1140;
	write_imageui(result, coords, color);
}
