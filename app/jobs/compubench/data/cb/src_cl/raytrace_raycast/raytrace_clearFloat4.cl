__kernel
void clearFloat4(__global float4* imageFloat4)
{
	// workitem/worksize info
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);

	const size_t sizeX = get_global_size(0);
	//const size_t sizeY = get_global_size(1);

	const size_t currIndex = (idY * sizeX) + idX;

	imageFloat4[currIndex] = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
}