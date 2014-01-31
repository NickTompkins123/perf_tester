// "Insert" a 0 bit after each of the 16 low bits of x
uint Part1By1(uint x)
{
  x &= 0x0000ffff;                  // x = ---- ---- ---- ---- fedc ba98 7654 3210
  x = (x ^ (x <<  8)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
  x = (x ^ (x <<  4)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
  x = (x ^ (x <<  2)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
  x = (x ^ (x <<  1)) & 0x55555555; // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
  return x;
}

uint EncodeMorton2(uint x, uint y)
{
  return (Part1By1(y) << 1) + Part1By1(x);
}

float nextFloat(unsigned int* seed0, unsigned int* seed1)
{
	*seed0 = 36969 * ((*seed0) & 65535) + ((*seed0) >> 16);
	*seed1 = 18000 * ((*seed1) & 65535) + ((*seed1) >> 16);
		
	return ((((*seed0) << 16) + (*seed1)) & 0xffff) * (1.0f / 0xffff);
}

__kernel
void generatePrimaryRays(__global const CLCamera* camera, __global CLRay* rays, __global float* rayWeights, __global uint* gSeed0, __global uint* gSeed1)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);

	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = (idY * sizeX) + idX;
	//currIndex = EncodeMorton2(idX, idY);

	// load seed		
	unsigned int seed0 = gSeed0[currIndex];
	unsigned int seed1 = gSeed1[currIndex];

	CLCamera cam = camera[0];

	CLRay primRay;
	primRay.org = cam.pos;

	//primRay.dir = (sizeX - idX - 0.5f) * (sizeY - idY - 0.5f) * cam.dirTL +
	//	      (idX + 0.5f) * (sizeY - idY - 0.5f) * cam.dirTR +
	//	      (sizeX - idX - 0.5f) * (idY + 0.5f) * cam.dirBL +
	//	      (idX + 0.5f) * (idY + 0.5f) * cam.dirBR;
	//primRay.dir = normalize(primRay.dir);	
	
	float4 camDir = (cam.dirTL + cam.dirTR + cam.dirBL + cam.dirBR) * 0.25f;
	float4 camI = (camDir - ((cam.dirTL + cam.dirBL) * 0.5f)) / (sizeX*0.5f);
	float4 camJ = (camDir - ((cam.dirTL + cam.dirTR) * 0.5f)) / (sizeY*0.5f);

	primRay.dir = cam.dirTL + (idX + (nextFloat(&seed0, &seed1))) * camI + (idY + (nextFloat(&seed0, &seed1))) * camJ;
	primRay.dir = normalize(primRay.dir);

	// save seed
	gSeed0[idY * sizeX + idX] = seed0;
	gSeed1[idY * sizeX + idX] = seed1;

	rays[currIndex] = primRay;
	rayWeights[currIndex] = 1.0f;
}