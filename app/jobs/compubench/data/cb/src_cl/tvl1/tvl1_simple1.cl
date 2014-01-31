__kernel void warping(
  __read_only image2d_t d,
  __global vfloat2 *u,
  __write_only image2d_t dw,
  __global uchar2 *mask,
  const int nx )
{
  const int x = get_global_id(0);
  const int y = get_global_id(1);
  const int ny = get_global_size(1);

  if (x>=nx) return;

  const sampler_t sampler_linear = CLK_NORMALIZED_COORDS_FALSE | 
    CLK_ADDRESS_CLAMP_TO_EDGE |
    CLK_FILTER_LINEAR;

  const float2 us = VLOAD_FLOAT2(nx * y + x, u);
   
  uchar2 m = mask[y * nx + x];

  float2 t = us + ((float2)(x, y));
  m.y = 255;
  if (t.x < 0 || t.x >= nx || t.y < 0 || t.y >= ny)
    m.y = 0;

  mask[y * nx + x] = m;
  write_imagef(dw, (int2)(x, y), read_imagef(d, sampler_linear, (float2)(x, y) + us.xy + 0.5f).xyxy);
}

__kernel void grad_rho_c(
  __read_only image2d_t I0,
  __read_only image2d_t I1,
  __read_only image2d_t dw,
  __global vfloat2 *u,
  __global float2 *grad_rho_c,
  const int nx )
{
  const int x = get_global_id(0);
  const int y = get_global_id(1);

  if (x>=nx) return;
  const int idx = y * nx + x;

  const sampler_t sampler_linear = CLK_NORMALIZED_COORDS_FALSE | 
    CLK_ADDRESS_CLAMP_TO_EDGE |
    CLK_FILTER_LINEAR;

  const sampler_t sampler_nearest = CLK_NORMALIZED_COORDS_FALSE | 
    CLK_ADDRESS_CLAMP_TO_EDGE |
    CLK_FILTER_NEAREST;

  const float2 Ixy = read_imagefs(dw, sampler_nearest, (int2)(x,  y)).xy * 255.f;

  const float2 up = VLOAD_FLOAT2(idx, u);

  //store the |Grad(I1)|^2
  //compute the constant part of the rho function
  const float fid = img_diff(read_imagef(I1, sampler_linear, (float2)(x, y) + up.xy + 0.5f),
			     read_imagef(I0, sampler_nearest, (int2)(x, y))) * 255.f;
  grad_rho_c[idx] = (float2)(Ixy.x * Ixy.x + Ixy.y * Ixy.y, fid - Ixy.x * up.x - Ixy.y * up.y);
}

inline float2 d(const float rho, const float l_t, const float grad, const float2 Ixy)
{
  if(rho < - l_t * grad)
  {
    return l_t * Ixy;
  }
  else
  {
    if(rho > l_t * grad)
    {
      return -l_t * Ixy;
    }
    else
    {
      if(grad < 1E-10f)
	return (float2)0.0f;
      else
      {
	return - rho * Ixy / grad;
      }
    }
  }
}
