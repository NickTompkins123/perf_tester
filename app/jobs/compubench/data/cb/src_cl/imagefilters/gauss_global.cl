__kernel void HorizontalGaussFilter
(
	__global const uchar4 *restrict input,
	__constant float *coefficients,
	__global uchar4 *output
)
{
	const int pX = get_global_id(0);
	const int pY = get_global_id(1);

	const int widthPrec = get_global_size(0) - 1;
	const int pY_width = pY*get_global_size(0);

	float4 aggregate = convert_float4(input[pY_width + pX]) * coefficients[samplingOffset];

	for(int o = 1; o <= samplingOffset; ++o)
	{
		aggregate += convert_float4(input[pY_width + max(pX - o, 0)]) * coefficients[samplingOffset - o];
		aggregate += convert_float4(input[pY_width + min(pX + o, widthPrec)]) * coefficients[samplingOffset + o];
	}

	output[pY_width + pX] = convert_uchar4_sat(aggregate);
}

__kernel void VerticalGaussFilter
(
	__global const uchar4 *restrict input,
	__constant float *coefficients,
	__global uchar4 *output
)
{
	const int pX = get_global_id(0);
	const int pY = get_global_id(1);

	const int width = get_global_size(0);
	const int heightPrec = get_global_size(1) - 1;

	float4 aggregate = convert_float4(input[pY*width + pX]) * coefficients[samplingOffset];

	for(int o = 1; o <= samplingOffset; ++o)
	{
		aggregate += convert_float4(input[max(pY - o, 0)*width + pX]) * coefficients[samplingOffset - o];
		aggregate += convert_float4(input[min(pY + o, heightPrec)*width + pX]) * coefficients[samplingOffset + o];
	}

	output[pY*width + pX] = convert_uchar4_sat(aggregate);
}

__kernel void TranspositionalGaussFilter
(
	__global const uchar4 *restrict input,
	__constant float *coefficients,
	__global uchar4 *output
)
{
	const int pX = get_global_id(0);
	const int pY = get_global_id(1);

	const int widthPrec = get_global_size(0) - 1;
	const int pY_width = pY*get_global_size(0);

	float4 aggregate = convert_float4(input[pY_width + pX]) * coefficients[samplingOffset];

	for(int o = 1; o <= samplingOffset; ++o)
	{
		aggregate += convert_float4(input[pY_width + max(pX - o, 0)]) * coefficients[samplingOffset - o];
		aggregate += convert_float4(input[pY_width + min(pX + o, widthPrec)]) * coefficients[samplingOffset + o];
	}

	output[pX*get_global_size(1) + pY] = convert_uchar4_sat(aggregate); //uncoalesced transpositional write
}

__kernel void TranspositionalGaussFilterLuminance ( __global const uchar *input,
													__constant float *coefficients,
													__global uchar *output )
{
	const int pX = get_global_id(0);
	const int pY = get_global_id(1);
	const int widthPrec = get_global_size(0) - 1;
	const int pY_width = pY*get_global_size(0);

	float aggregate = convert_float(input[pY_width + pX]) * coefficients[samplingOffset];

	for(int o = 1; o <= samplingOffset; ++o)
	{
		aggregate += convert_float(input[pY_width + max(pX - o, 0)]) * coefficients[samplingOffset - o];
		aggregate += convert_float(input[pY_width + min(pX + o, widthPrec)]) * coefficients[samplingOffset + o];
	}

	output[pX*get_global_size(1) + pY] = convert_uchar_sat(aggregate); //uncoalesced transpositional write
}
