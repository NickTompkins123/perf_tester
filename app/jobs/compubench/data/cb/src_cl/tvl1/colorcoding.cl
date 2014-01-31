#define EXTR 340282346638528859811704183484516925440.0f
#define POS(X) X = X > 0 ? X : 0
#define SQR(x) ((x)*(x))

inline uint4 vectorToColor(float2 xy, float max_vector_length)
{
  float x0 = xy.x / max_vector_length;
  float y0 = xy.y / max_vector_length;

  float distM = 1.4142f - sqrt(SQR(x0 - 1.f) + SQR(y0 + 1.f));
  float distO = 1.4142f - sqrt(SQR(x0 - 1.f) + SQR(y0 - 1.f));
  float distG = 1.4142f - sqrt(SQR(x0 + 1.f) + SQR(y0 - 1.f));
  float distB = 1.4142f - sqrt(SQR(x0 + 1.f) + SQR(y0 + 1.f));
  float distW = 1.4142f - sqrt(SQR(x0) + SQR(y0));
  POS(distM); POS(distO); POS(distG); POS(distB); POS(distW);


  float weight = distM + distO + distG + distB + distW;
  distM /= weight; distO /= weight; distG /= weight; distB /= weight; distW /= weight;

  uint4 result;

  result.x = distW * 255 + distO * 255 + distM * 255;
  result.y = distW * 255 + distO * 126.5f + distG * 255;
  result.z = distW * 255 + distB * 255 + distM * 255;
  result.w = 255;

  return result;
}

__kernel void colorCoding(
  __global const vfloat2 * restrict u,
  __write_only image2d_t c,
  float max_vector_length)
{
  const int x = get_global_id(0);
  const int y = get_global_id(1);
  const int width = get_global_size(0);
  
  if ( x < max_vector_length * 2 && y < max_vector_length * 2)
  {		
    write_imageui(c, (int2)(x, y), vectorToColor((float2)(x, y) - max_vector_length, max_vector_length));
  } else
  {
    const float2 up = VLOAD_FLOAT2(y * width + x, u);
	//uint4 cc = {up.x*1000, up.y,up.y*1000,255};
	//write_imageui(c, (int2)(x,y), cc);
    write_imageui(c, (int2)(x,y), (up.x != EXTR) ? vectorToColor(up, max_vector_length) : (uint4)0);
  }
}

