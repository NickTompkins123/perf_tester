__constant int PALETTENUM = 7;
__constant float4 colorPalette[7] =
{
	(float4)(0.0f, 0.0f, 0.0f, 1.0f),		// black
	(float4)(1.0f, 0.0f, 0.0f, 1.0f),		// red
	(float4)(1.0f, 1.0f, 0.0f, 1.0f),		// yellow
	(float4)(1.0f, 1.0f, 1.0f, 1.0f),		// white
	(float4)(1.0f, 1.0f, 0.0f, 1.0f),		// yellow
	(float4)(1.0f, 0.0f, 0.0f, 1.0f),		// red
	(float4)(0.0f, 0.0f, 0.0f, 1.0f)		// black
};

__kernel
void root(const float currIter, const float iterCount,
		  __global uchar4* gImage)
{
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);

	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);
	
	const int maxIter = 100;

	float4 fractalFloat4 = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
	
	float real0 = -0.63f;
	float imag0 = -0.407f;

	float real1 = -0.835f;
	float imag1 = 0.2321f;

	float x0 = real0 + currIter * ( (real1 - real0) / (iterCount) );
	float y0 = imag0 + currIter * ( (imag1 - imag0) / (iterCount) );

	for(int j = 0; j < SAMPLEITER; ++j)
	{
		for(int i = 0; i < SAMPLEITER; ++i)
		{
			float xOffset = i * SAMPLEIDIVINV;
			float yOffset = j * SAMPLEIDIVINV;

			float x = ((idX+xOffset) / sizeX) * 4.0f - 2.0f;
			float y = ((idY+yOffset) / sizeY) * 4.0f - 2.0f;
	
			bool isInside = true;

			int iter;
			for(iter = 0; iter < maxIter; ++iter)
			{
				float xtemp = x*x - y*y + x0;
				float ytemp = 2.0f*x*y + y0;

				if((x * x + y * y) > 4.0f)
				{
					isInside = false;
					break;
				}

				x = xtemp;
				y = ytemp;
			}
	
			if(!isInside)
			{	
				float colorSelectorF = (iter/1.05f) / PALETTENUM;
		
				int colorSelectorIL = (int)(colorSelectorF);
				colorSelectorF -= colorSelectorIL;

				colorSelectorIL = min(colorSelectorIL, PALETTENUM-1);
				int colorSelectorIU = min(colorSelectorIL+1, PALETTENUM-1);
		
				fractalFloat4 += colorPalette[ colorSelectorIL ] * (1.0f-colorSelectorF) + colorPalette[ colorSelectorIU ] * colorSelectorF;
			}
		}
	}

	fractalFloat4 *= SAMPLESCALE;
	
	gImage[idY * sizeX + idX] = convert_uchar4( 255.0f * fractalFloat4 + 0.5f );
}