typedef struct
{
	int x;
	int y;
	int width;
	int height;
	float scale;
} HaarRect;


__kernel void violaJones(
	__global HaarFeatureNode* stageNodes,
	int stagesCount,
	__global int* stageNodeCounts,
	__global float* stageThresholds,
	read_only image2d_t integralImage,  
	read_only image2d_t integral2Image, 
	__global int* resRectangles,
	const int rectCount,
	__global const HaarRect* haarRects)
{
	int id = get_global_id(0);

	if (id >= rectCount)
		return;

	HaarRect rect = haarRects[id];

	const sampler_t smp = CLK_ADDRESS_CLAMP;

	// try to detect an object inside the window
	
	resRectangles[id] = compute(stageNodes, stagesCount, stageNodeCounts,
		stageThresholds, integralImage, smp, integral2Image, rect.scale,
		rect.x, rect.y, rect.width, rect.height);
}
