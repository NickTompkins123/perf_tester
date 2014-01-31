__kernel void init_u(__global vfloat4 *u)
{
	VSTORE_FLOAT4((float4)0.0f, get_global_id(0), u);
}

__kernel void crop_u(__global vfloat2 *u_in,
					 __global vfloat2 *u_out,
					 const int width_in)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int width_out = get_global_size(0);
	VSTORE_FLOAT2(VLOAD_FLOAT2(y * width_in + x, u_in), y * width_out + x, u_out);
}

__kernel void gauss_filter( __read_only image2d_t input,
						   __write_only image2d_t intermediate )
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
							  CLK_ADDRESS_CLAMP_TO_EDGE |
							  CLK_FILTER_NEAREST;

	const int x = get_global_id(0);
	const int y = get_global_id(1);

	float4 sum = (float4)(0.0f);
	sum += (2.0f/583.0f)  * read_imagef( input, sampler, (int2)(x - 2    , y - 2 ) );
	sum += (7.0f/583.0f)  * read_imagef( input, sampler, (int2)(x - 1  , y - 2 ) );
	sum += (15.0f/583.0f)  * read_imagef( input, sampler, (int2)(x    , y - 2  ) );
	sum += (7.0f/583.0f)  * read_imagef( input, sampler, (int2)(x + 1  , y - 2  ) );
	sum += (2.0f/583.0f)  * read_imagef( input, sampler, (int2)(x + 2 , y - 2  ) );

	sum += (7.0f/583.0f)  * read_imagef( input, sampler, (int2)(x - 2  , y - 1  ) );
	sum += (31.0f/583.0f)  * read_imagef( input, sampler, (int2)(x - 1 , y - 1  ) );
	sum += (52.0f/583.0f)  * read_imagef( input, sampler, (int2)(x    , y - 1  ) );
	sum += (31.0f/583.0f)  * read_imagef( input, sampler, (int2)(x + 1 , y - 1 ) );
	sum += (7.0f/583.0f)  * read_imagef( input, sampler, (int2)(x + 2 , y - 1 ) );

	sum += (15.0f/583.0f)  * read_imagef( input, sampler, (int2)(x - 2, y    ) );
	sum += (52.0f/583.0f)  * read_imagef( input, sampler, (int2)(x - 1, y    ) );
	sum += (127.0f/583.0f)  * read_imagef( input, sampler, (int2)(x    , y    ) );
	sum += (52.0f/583.0f)  * read_imagef( input, sampler, (int2)(x + 1, y    ) );
	sum += (15.0f/583.0f)  * read_imagef( input, sampler, (int2)(x + 2, y    ) );

	sum += (7.0f/583.0f)  * read_imagef( input, sampler, (int2)(x - 2, y + 1 ) );
	sum += (31.0f/583.0f)  * read_imagef( input, sampler, (int2)(x - 1, y + 1 ) );
	sum += (52.0f/583.0f)  * read_imagef( input, sampler, (int2)(x    , y + 1 ) );
	sum += (31.0f/583.0f)  * read_imagef( input, sampler, (int2)(x + 1, y + 1 ) );
	sum += (7.0f/583.0f)  * read_imagef( input, sampler, (int2)(x + 2, y + 1 ) );

	sum += (2.0f/583.0f)  * read_imagef( input, sampler, (int2)(x - 2, y + 2  ) );
	sum += (7.0f/583.0f)  * read_imagef( input, sampler, (int2)(x - 1, y + 2  ) );
	sum += (15.0f/583.0f)  * read_imagef( input, sampler, (int2)(x    , y + 2 ) );
	sum += (7.0f/583.0f)  * read_imagef( input, sampler, (int2)(x + 1, y + 2 ) );
	sum += (2.0f/583.0f)  * read_imagef( input, sampler, (int2)(x + 2, y + 2 ) );


	write_imagef( intermediate, (int2)(x, y), sum );
}

__kernel void down_sample( __read_only image2d_t input,
						   __write_only image2d_t downSampled)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
								CLK_ADDRESS_CLAMP_TO_EDGE |
								CLK_FILTER_LINEAR; //linear?
	
	const int x = get_global_id(0);
	const int y = get_global_id(1);

	float4 avg = read_imagef(input, sampler, (float2)(x*2.0f + 1.0f, y*2.0f + 1.0f));

	write_imagef(downSampled, (int2)(x,y), avg);
}

__kernel void zoom_out(
    __global vfloat2 *u0,      //input vector field
    __global vfloat2 *u1,      //output vector field
    const int nx,                   //width of the original image
    const int ny                   //height of the original image
    )
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int nxx = get_global_size(0); //width of the zoomed image

    VSTORE_FLOAT2(VLOAD_FLOAT2(nx * (y / 2) + (x / 2), u0) * 2, nxx * y + x, u1);
}
