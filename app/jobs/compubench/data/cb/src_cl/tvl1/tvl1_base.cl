inline void write_imagefs(__write_only image2d_t i, const int2 xy, const float4 d)
{
  write_imagef(i, xy, (d + 1.0f) / 2.0f);
}

inline float4 read_imagefs(__read_only image2d_t i, sampler_t srcSampler, const int2 xy)
{
  return read_imagef(i, srcSampler, xy) * 2.0f - 1.0f;
}

inline float img_diff(float4 pix1, float4 pix2)
{
  return ((pix1.x + pix1.y + pix1.z) - (pix2.x + pix2.y + pix2.z)) / 3.0f;
}

__kernel void gradient_init_p(
  __read_only image2d_t input,  //input image
  __write_only image2d_t d,     //computed derivative
  __write_only image2d_t p
  )
{
  const sampler_t srcSampler = CLK_NORMALIZED_COORDS_FALSE | 
    CLK_ADDRESS_CLAMP_TO_EDGE |
    CLK_FILTER_NEAREST;

  const int x = get_global_id(0);
  const int y = get_global_id(1);
   
  const float pdx = img_diff(read_imagef(input, srcSampler, (int2)(x + 1, y)), read_imagef(input, srcSampler, (int2)(x - 1, y))) / 2.0f;
  const float pdy = img_diff(read_imagef(input, srcSampler, (int2)(x, y + 1)), read_imagef(input, srcSampler, (int2)(x, y - 1))) / 2.0f;

  write_imagefs(d, (int2)(x, y), ((float2)(pdx, pdy)).xyxy);
  write_imagef(p, (int2)(x, y), (float4)(0.5f));
}
