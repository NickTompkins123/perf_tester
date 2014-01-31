__kernel void sine_wave(__global float4* pos, const float time)
{
    unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);

	unsigned int width = get_global_size(0);
	unsigned int height = get_global_size(1);

    // calculate uv coordinates
    float u = x / (float) width;
    float v = y / (float) height;
    u = u*2.0f - 1.0f;
    v = v*2.0f - 1.0f;

    // calculate simple sine wave pattern
    float freq = 4.0f;
    float w = sin(u*freq + time) * cos(v*freq + time) * 0.5f;

    // write output vertex
    pos[y*width+x] = (float4)(u, w, v, 1.0f);
}

__kernel void generateSpectrum( __global const float2* h0,
								__global const float* omega,
								__global float2* htOut,
								__global float2* slopeXOut,
								__global float2* slopeYOut,
								unsigned int in_width,
								const float t )
{
	unsigned int out_width = get_global_size(0);
	unsigned int out_height = get_global_size(1);

	unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);
    unsigned int in_index = y*in_width+x;
    unsigned int in_mindex = (out_height - y)*in_width + (out_width - x); // mirrored
    unsigned int out_index = y*out_width+x;

	float2 h0_k = h0[in_index];
	float2 h0_mk = h0[in_mindex];

	float sin_v, cos_v;
	sin_v = sincos(omega[in_index] * t, &cos_v);

	float2 ht;
	ht.x = (h0_k.x + h0_mk.x) * cos_v - (h0_k.y + h0_mk.y) * sin_v;
	ht.y = (h0_k.x - h0_mk.x) * sin_v + (h0_k.y - h0_mk.y) * cos_v;

	// H(t) -> Dx(t), Dy(t)
	float kx = x - out_width * 0.5f;
	float ky = y - out_height * 0.5f;
	float sqr_k = kx * kx + ky * ky;
	float rsqr_k = 0;
	if (sqr_k > 1e-12f)
		rsqr_k = 1 / sqrt(sqr_k);
	kx *= rsqr_k;
	ky *= rsqr_k;

	float2 dt_x = (float2)(ht.y * kx, -ht.x * kx);
	float2 dt_y = (float2)(ht.y * ky, -ht.x * ky);

	htOut[out_index] = ht;
	slopeXOut[out_index] = dt_x;
	slopeYOut[out_index] = dt_y;
}

__kernel void updateHeightMap(	__global const float2* htOut,
								__global const float2* slopeXOut,
								__global const float2* slopeYOut,
								__global float4* hMapOut)
{
	unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);
	unsigned int width = get_global_size(0);

    unsigned int id = y*width+x;

    float sign_correction = ((x + y) & 0x01) ? -1.0f : 1.0f;
	
	float dx = slopeXOut[id].x * sign_correction;
	float dy = slopeYOut[id].x * sign_correction;
	float dz = htOut[id].x * sign_correction;

	//hMapOut[id] = (float4)(x/512.0f, 100.0f * dz, y/512.0f, 1.0f);
	hMapOut[id] = (float4)(x/512.0f + 200.0f * dx, 100.0f * dz, y/512.0f + 200.0f * dx, 1.0f);
}

__kernel void updateNormalMap(	__global const float4* hMap,
								__global float4* normal)
{
	unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);
	unsigned int width = get_global_size(0);

	unsigned int xP = (y*width+((x+1)%width));
	unsigned int xN = (y*width+((x-1)%width));

	unsigned int yP = (((y+1)%width)*width+x);
	unsigned int yN = (((y-1)%width)*width+x);

	float4 xDiff = hMap[xP] - hMap[xN];
	float4 yDiff = hMap[yP] - hMap[yN];

    unsigned int id = y*width+x;
	//normal[id] = fabs( normalize( cross(xDiff, yDiff) ) );
	normal[id] = ( normalize( cross(yDiff, xDiff) ) );
}