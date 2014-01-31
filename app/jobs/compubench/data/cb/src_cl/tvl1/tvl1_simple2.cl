__kernel void v_u(
  __read_only image2d_t p,
  __read_only image2d_t dw,
  __global vfloat2 *u,
  __global const float2 * restrict grad_rho_c,
  const float lambda,
  const float theta,
  __global uchar2 *mask,
  const int nx )
{
  const int x = get_global_id(0);
  const int y = get_global_id(1);

  if (x>=nx) return;
  const int idx = y * nx + x;

  //border (0.0f, 0.0f, 0.0f, 0.0f);
  const sampler_t srcSampler = CLK_NORMALIZED_COORDS_FALSE | 
    CLK_ADDRESS_CLAMP |
    CLK_FILTER_NEAREST;

  const float4 pxy = read_imagefs(p, srcSampler, (int2)(x, y));

  const float2 px1_1y = x != 0 ? read_imagefs(p, srcSampler, (int2)(x-1, y)).xz : 0.0f;
   
  const float2 pxy_1 = y != 0 ? read_imagefs(p, srcSampler, (int2)(x, y-1)).yw : (float2)0.0f;

  const float2 div_p = pxy.xz - px1_1y + pxy.yw - pxy_1;
    
  const float l_t = lambda * theta;
  const float2 u0  = VLOAD_FLOAT2(idx, u);

  if(!mask[idx].y)
  {
    VSTORE_FLOAT2(u0 + theta * div_p, idx, u);
    return;
  }


  const float2 Ixy  = read_imagefs(dw, srcSampler, (int2)(x,   y)).xy * 255.f;

  const float2 grc = grad_rho_c[idx];
  const float rho = grc.y + (Ixy.x * u0.x + Ixy.y * u0.y);

  const float2 d_p = d(rho, l_t, grc.x, Ixy);
  const float2 up = u0 + d_p + theta * div_p;
	
  VSTORE_FLOAT2(up, idx, u);
}

__kernel void p(
  const float tau,
  const float theta, 
  __global vfloat2 *u,
  __read_only image2d_t p_in,
  __write_only image2d_t p_out,
  const int nx )
{
  const int x = get_global_id(0);
  const int y = get_global_id(1);
  const int ny = get_global_size(1);

  if (x>=nx) return;
  const int idx = x + y * nx;

  //border (0.0f, 0.0f, 0.0f, 0.0f);
  const sampler_t srcSampler = CLK_NORMALIZED_COORDS_FALSE | 
    CLK_ADDRESS_CLAMP_TO_EDGE |
    CLK_FILTER_NEAREST;

  const float2 ux1y   = VLOAD_FLOAT2(idx, u);
  const float2 ux1y_1 = y != (ny - 1) ? VLOAD_FLOAT2(nx * (y + 1) + x, u) : ux1y;
  const float2 ux2_1y = x != (nx - 1) ? VLOAD_FLOAT2(nx * y + x + 1, u) : ux1y;

  const float4 u_der = (float4)(ux2_1y.x - ux1y.x, ux1y_1.x - ux1y.x, ux2_1y.y - ux1y.y, ux1y_1.y - ux1y.y);

  const float taut = tau / theta;
  const float2 ng  = 1.0f + taut * sqrt(u_der.xz * u_der.xz + u_der.yw * u_der.yw);
 
  const float4 pt = (read_imagefs(p_in, srcSampler, (int2)(x, y)) + taut * u_der) / ng.s0011;
  write_imagefs(p_out, (int2)(x, y), pt);
}
