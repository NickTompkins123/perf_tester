__kernel void ImageUIntIntegralSquare(
		read_only image2d_t sumImage,
		write_only image2d_t dstImage,
		const int height)
{
	const sampler_t smp = CLK_ADDRESS_NONE;
	
	uint x = get_global_id(0);
	uint tempVal = 0;
	for (int y = 1; y < height; y++)
	{
		uint4 color = read_imageui(sumImage, smp, (int2)(x + 1, y));
		tempVal += color.w;
		color.w = tempVal;
 		write_imageui(dstImage, (int2)(x + 1, y), color);
	}
}
