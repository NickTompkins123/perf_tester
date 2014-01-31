// Inverse of Part1By1 - "delete" all odd-indexed bits
uint Compact1By1(uint x)
{
  x &= 0x55555555;                  // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
  x = (x ^ (x >>  1)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
  x = (x ^ (x >>  2)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
  x = (x ^ (x >>  4)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
  x = (x ^ (x >>  8)) & 0x0000ffff; // x = ---- ---- ---- ---- fedc ba98 7654 3210
  return x;
}

uint DecodeMorton2X(uint code)
{
  return Compact1By1(code >> 0);
}

uint DecodeMorton2Y(uint code)
{
  return Compact1By1(code >> 1);
}

__kernel
void convertUchar4(const float iterScale, __global const float4* imageFloat4, __global uchar4* imageUchar4)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);

	const size_t sizeX = get_global_size(0);
	//const size_t sizeY = get_global_size(1);

	const size_t currIndex = (idY * sizeX) + idX;

	float4 shadedPixel = imageFloat4[currIndex] * iterScale;
	shadedPixel.w = 1.0f;

	shadedPixel = min( shadedPixel, (float4)(1.0f, 1.0f, 1.0f, 1.0f) );

	//currIndex = (DecodeMorton2Y(currIndex) * sizeX) + DecodeMorton2X(currIndex);
	imageUchar4[currIndex] = convert_uchar4( 255.0f * shadedPixel + 0.5f );
}