__kernel void generatePrimaryRays(global CLParameters* parameters, global CLSun* sun, __global const CLCamera* restrict camera, __global const float2* randomNumbers, __global int* rayNumbers,
								  __global float* rayweights, __global CLRay* rays, __global CLRay* primaryRays,
								  __global CLRayDifferential* rayDiffs)
{
	const size_t idX = get_global_id(0);
	const size_t idY = get_global_id(1);
	const size_t sizeX = get_global_size(0);
	const size_t sizeY = get_global_size(1);

	const size_t currIndex = idY * sizeX + idX;

	rayNumbers[currIndex] = rayweights[currIndex] == 1.0f ? rayNumbers[currIndex] + 1 : rayNumbers[currIndex];

	float4 camDir = (camera->dirTL + camera->dirTR + camera->dirBL + camera->dirBR) * 0.25f;
	float4 camI = (camDir - ((camera->dirTL + camera->dirBL) * 0.5f)) / (sizeX*0.5f);
	float4 camJ = (camDir - ((camera->dirTL + camera->dirTR) * 0.5f)) / (sizeY*0.5f);

	CLRay primaryRay;
	primaryRay.org = camera->pos;
	primaryRay.dir = camera->dirTL + (idX) * camI + (idY) * camJ;
	//primaryRay.dir = camera->dirTL + (idX + (randomNumbers[currIndex].x)) * camI + (idY + (randomNumbers[currIndex].y)) * camJ;
	primaryRay.dir = normalize(primaryRay.dir);

	if ((*parameters).DoF){
		float focal = 3.0f + 1000.0f*((*sun).xyYcolor.x);
		float aperture = 1000.0f;
		float4 focalPoint = primaryRay.org + focal*primaryRay.dir;

		float x = sqrt(aperture*randomNumbers[currIndex].x) * cos(2.0f*M_PI_F*randomNumbers[currIndex].y);
		float y = sqrt(aperture*randomNumbers[currIndex].x) * sin(2.0f*M_PI_F*randomNumbers[currIndex].y);

		primaryRay.org += camI*x + camJ*y;
		primaryRay.dir = normalize(focalPoint - primaryRay.org) ;
	}


	CLRayDifferential raydiff;
	raydiff.rxOrg = camera->pos;
	raydiff.ryOrg = camera->pos;
	raydiff.rxDir = camera->dirTL + (idX+1) * camI + (idY  ) * camJ;
	raydiff.ryDir = camera->dirTL + (idX  ) * camI + (idY+1) * camJ;
	raydiff.rxDir = normalize(raydiff.rxDir);
	raydiff.ryDir = normalize(raydiff.ryDir);

	rays[currIndex] = primaryRay;
	primaryRays[currIndex] = primaryRay;
	rayDiffs[currIndex] = raydiff;
}

__kernel void generateRandomNumber (__global const float* restrict mask, __global uint2* randomSeed, __global float2* randomNumbers)
{
	const size_t currIndex = get_global_id(1)*get_global_size(0) + get_global_id(0);

	if (mask[currIndex] == 0.0f) return;
	randomSeed[2*currIndex] = 36969 * (randomSeed[2*currIndex] & 65535) + (randomSeed[2*currIndex] >> 16);
	randomSeed[2*currIndex+1] = 18000 * (randomSeed[2*currIndex+1] & 65535) + (randomSeed[2*currIndex+1] >> 16);
	uint2 ires = (randomSeed[2*currIndex] << 16) + randomSeed[2*currIndex+1];
	uint2 ui = (ires & 0x007fffff) | 0x40000000;
	randomNumbers[currIndex] = (as_float2(ui) - 2.0f) / 2.0f;
}

__kernel void generateCosRay(__global float2* randomNumbers, __global CLRay* ray,  __global CLRay* primaryRay, __global const CLHitData* restrict hitdata, __global const CLMaterial* restrict materials, __global float* prob, __global float* clMRayWeights)
{
	const size_t currIndex = get_global_id(1)*get_global_size(0) + get_global_id(0);
	primaryRay[currIndex] = ray[currIndex];

	if (hitdata[currIndex].matId!= INT_MAX){
		float phi = 2.0f * M_PI_F * randomNumbers[currIndex].x;
		float theta =  asin(sqrt(randomNumbers[currIndex].y));
		float4 normal = hitdata[currIndex].hitNormal;

		float4 dx0 = cross( (float4)(1.0f, 0.0f, 0.0f, 0.0f),normal );
		float4 dx1 = cross( (float4)(0.0f, 1.0f, 0.0f, 0.0f),normal );

		float4 dx = normalize(dot(dx0,dx0) > dot(dx1,dx1) ? dx0 : dx1);
		float4 dy = normalize(cross(normal,dx));

		ray[currIndex].org = hitdata[currIndex].hitPosition + 0.001f*normal;
		if ((materials + hitdata[currIndex].matId)->texMaskFlag != 0 && hitdata[currIndex].hitKR.z > 0.3f){
			ray[currIndex].dir = Reflect(ray[currIndex].dir, normal);
			prob[currIndex] = 1.0f;
		}
		else{
			prob[currIndex] = cos(theta)/M_PI_F;
			ray[currIndex].dir = normalize(dx*sin(theta)*cos(phi) + dy*sin(theta)*sin(phi) + normal*cos(theta));
		}
	}

	clMRayWeights[currIndex] = hitdata[currIndex].matId == INT_MAX ? 0.0f : 1.0f;
}

__kernel void clearAccumColorsIterative(__global float4* accumLight0StackIterative, __global float4* accumLight1StackIterative, __global float4* accummMaterialIterative, __global float4* accummColorIterative,
										__global float* clMRayWeights, __global int* rayNumbers, __global float4* skyMask, __global float4* filterKD, __global float4* filterKDIterative)
{
	const size_t currIndex = get_global_id(1)*get_global_size(0) + get_global_id(0);

	accumLight0StackIterative[currIndex] = (float4)(0.0f);
	accumLight1StackIterative[currIndex] = (float4)(0.0f);
	accummMaterialIterative[currIndex] = (float4)(0.0f);
	accummColorIterative[currIndex]=(float4)(0.0f);

	clMRayWeights[currIndex] = 1.0f;
	rayNumbers[currIndex] = 0;
	skyMask[currIndex] = (float4)(0.0f);
	filterKD[currIndex]=(float4)(1.0f);
	filterKDIterative[currIndex]=(float4)(0.0f);
}

__kernel void clearAccumColors(__global float4* accummColor, __global  float4* accumLight0Stack, __global  float4* accumLight1Stack, __global  float4* accumMaterial,
							   __global float4* filterNormals)
{
	const size_t currIndex = get_global_id(1)*get_global_size(0) + get_global_id(0);

	accummColor[currIndex]= (float4)(0.0f);
	accumLight0Stack[currIndex] = (float4)(0.0f);
	accumLight1Stack[currIndex] = (float4)(0.0f);
	accumMaterial[currIndex] = (float4)(0.0f);

	filterNormals[currIndex] = (float4)(0.0f);
	filterNormals[currIndex].w = 10.0f;	
}

__kernel void writeStack(const uint depth, const int actualIterNum, global CLParameters* parameters, __global const CLHitData* restrict hitdata, __global const CLMaterial* restrict materials, __global CLRay* ray,  __global CLRay* primaryRay, __global float* prob,
					     __global float4* skyColor, __global float4* skyMask, __global const CLLight* light, __global float4* filterNormals,  __global float4* filterKD, __global float4* filterKDIterative,
						 __global float4* lightStack, __global float4* materialStack)
{
	const size_t currIndex = get_global_id(1)*get_global_size(0) + get_global_id(0);
	const size_t offset = depth * get_global_size(0) * get_global_size(1);

	if (hitdata[currIndex].matId != INT_MAX){
		if ((materials + hitdata[currIndex].matId)->texMaskFlag != 0 && hitdata[currIndex].hitKR.z > 0.3f){
		//if ((materials + hitdata[currIndex].matId)->texMaskFlag != 0 && hitdata[currIndex].hitKR.x < 0.2f){
			lightStack[offset + currIndex] = (float4)(0.0f);
			materialStack[offset + currIndex] = (float4)(0.4f) + hitdata[currIndex].hitKD;

		} 
		else
		if ((materials + hitdata[currIndex].matId)->texMaskFlag != 0 && hitdata[currIndex].hitKR.y > 0.01f && depth==0){
			lightStack[offset + currIndex] = (*parameters).filter == 1 && depth==0 ? skyColor[currIndex] : skyColor[currIndex] * hitdata[currIndex].hitKD;
			materialStack[offset + currIndex] = (dot(ray[currIndex].dir, hitdata[currIndex].hitNormal) * (hitdata[currIndex].hitKD + Specular(-1.0f*primaryRay[currIndex].dir, hitdata[currIndex].hitNormal, (*light).v, (float4)(hitdata[currIndex].hitKR.y), 10))) / prob[currIndex];
		}
		else{
			lightStack[offset + currIndex] = (*parameters).filter == 1 && depth==0 ? skyColor[currIndex] : skyColor[currIndex] * hitdata[currIndex].hitKD ;
			materialStack[offset + currIndex] = dot(ray[currIndex].dir, hitdata[currIndex].hitNormal) * hitdata[currIndex].hitKD / prob[currIndex];
		}
		if (depth==0 && actualIterNum==0){
			if ((*parameters).filter == 1){
				filterKD[currIndex] = hitdata[currIndex].hitKD;
				filterKDIterative[currIndex] +=filterKD[currIndex];

			}
			float4 normalTemp = hitdata[currIndex].hitNormal;
			normalTemp.w = hitdata[currIndex].tHit* 1.0f;

			filterNormals[currIndex] = normalTemp;
		}
			
	}
	else{
		lightStack[offset + currIndex] = skyColor[currIndex];
		materialStack[offset + currIndex] = (float4)(0.0f);
	}

	filterKDIterative[currIndex] = skyMask[currIndex].x !=0.0f ?  1.0f : filterKDIterative[currIndex];
}

__kernel void upStack(const uint depth, __global float4* lightStack, __global float4* materialStack)
{
	const size_t currIndex = get_global_id(1)*get_global_size(0) + get_global_id(0);
	const size_t offset = depth * get_global_size(0) * get_global_size(1);
	const size_t offsetB = (depth-1) * get_global_size(0) * get_global_size(1);

	lightStack[offsetB + currIndex] += materialStack[offsetB + currIndex]*lightStack[offset + currIndex];
}

__kernel void accumColors(const int iterNum, __global float4* skyMask, __global float* rayWeights, __global float4* lightStack, __global  float4* materialStack,
						  __global  float4* accumLight0Stack, __global  float4* accumLight1Stack, __global  float4* accummMaterial, __global float4* accumColor, __global float4* filterKD
						  )
{
	const size_t currIndex = get_global_id(1)*get_global_size(0) + get_global_id(0);
	const size_t offset = get_global_size(0) * get_global_size(1);

	accumColor[currIndex] += lightStack[currIndex];
	accumLight0Stack[currIndex] += lightStack[currIndex]/iterNum;
	accumLight1Stack[currIndex] += lightStack[offset + currIndex]/iterNum;
	accummMaterial[currIndex] += materialStack[currIndex]/iterNum;

	if ( skyMask[currIndex].x == 0.0f )
		rayWeights[currIndex] = 1.0f;
}

__kernel void composite(const uint iteration, global CLParameters* parameters, __global int* rayNumbers,
					    __global float4* accummColor, __global float4* accummMaterial, __global float4* filterKD, __global float4* filterLight0Stack, __global float4* filterLight1Stack,
						__global float4* accummColorIterative, __global float4* accumLight0StackIterative, __global float4* accumLight1StackIterative, __global float4* accummMaterialIterative, __global float4* finalColor, __global float* filterShadow)
{
	const size_t currIndex = get_global_id(1) * get_global_size(0) + get_global_id(0);

	if ((*parameters).filter == 0){
		if ((*parameters).iterative == 1){
			accummColorIterative[currIndex] += accummColor[currIndex];
			finalColor[currIndex] = accummColorIterative[currIndex]/(float)rayNumbers[currIndex];
		}
		else{
			finalColor[currIndex] = accummColor[currIndex]/(float)rayNumbers[currIndex];
		}
	}

	if ((*parameters).filter == 1){
		if ((*parameters).iterative == 1){
			accumLight0StackIterative[currIndex] += filterLight0Stack[currIndex];
			accumLight1StackIterative[currIndex] += filterLight1Stack[currIndex];
			accummMaterialIterative[currIndex] += accummMaterial[currIndex];
			finalColor[currIndex] = (accumLight0StackIterative[currIndex]/(float)rayNumbers[currIndex])*(filterKD[currIndex]/(float)rayNumbers[currIndex]) + (accumLight1StackIterative[currIndex]/(float)rayNumbers[currIndex])*(accummMaterialIterative[currIndex]/(float)rayNumbers[currIndex]);
			//finalColor[currIndex] = (accumLight0StackIterative[currIndex])*(filterKD[currIndex]) + (accumLight1StackIterative[currIndex])*(accummMaterialIterative[currIndex]/(float)rayNumbers[currIndex]);
		}
		else{
			//finalColor[currIndex] = (filterLight0Stack[currIndex]/(float)rayNumbers[currIndex])*(filterKD[currIndex]/(float)rayNumbers[currIndex]) + (filterLight1Stack[currIndex]/(float)rayNumbers[currIndex])*(accummMaterial[currIndex]/(float)rayNumbers[currIndex]);
			finalColor[currIndex] = (filterLight0Stack[currIndex])*filterKD[currIndex] + (filterLight1Stack[currIndex])*(accummMaterial[currIndex]);
		}
	}

	//finalColor[currIndex] = (filterLight1Stack[currIndex])*(accummMaterial[currIndex]);
	//finalColor[currIndex] = (filterLight0Stack[currIndex])*filterKD[currIndex];
	//finalColor[currIndex] = (10.0f - filterShadow[currIndex])/10.0f;

}

__kernel void toneMapping(/*__global float* exposure,*/  __global  float4* normals, global CLParameters* parameters, __global float4* skyMask, __global uchar4* pixelsUchar4, __global float4* finalColor){
	const size_t currIndex = get_global_id(1) * get_global_size(0) + get_global_id(0);

	/*float sample_luminance = 0.0f;
	int sample_num = 10;
	for (int i=0; i<sample_num; i++)
		for (int j =0; j<sample_num; j++){
			int x =get_global_size(0) / sample_num * (i + 0.5f);
			int y =get_global_size(1) / sample_num * (j + 0.5f);
			sample_luminance +=0.2126f*(float)pixelsUchar4[y*get_global_size(0)+x].x + 0.7152f*(float)pixelsUchar4[y*get_global_size(0)+x].y + 0.0722f*(float)pixelsUchar4[y*get_global_size(0)+x].z;
		}
	sample_luminance/=sample_num*sample_num;

	float reference = (*parameters).avgLuminance;
	float error = (sample_luminance - reference)/255.0f * sqrt(fabs(sample_luminance - reference));
	exposure[0] -= error*0.001f;
	exposure[0] = clamp(exposure[0], 0.5f, 2.5f);*/
	float exposure = 1.1f;

	if(skyMask[currIndex].x == 0.0f)
		finalColor[currIndex] = HDR(finalColor[currIndex], exposure, 0);
	else
		finalColor[currIndex] = skyMask[currIndex];


	
	//finalColor[currIndex]*= 1.0f - min(sqrt( max(normals[currIndex].w - 90.0f, 0.0f)*0.002f),0.8f) * (float4)(255.0f/141.0f, 255.0f/167.0f, 255.0f/255.0f, 1.0f);
	finalColor[currIndex] = mix (finalColor[currIndex], (float4)(112.0f/255.0f, 133.0f/255.0f, 224.0f/255.0f, 1.0f), min(sqrt( max(normals[currIndex].w - 90.0f, 0.0f)*0.005f), (float4)(1.0f)));
	//finalColor[currIndex]*= 1.0f - max(normals[currIndex].w-100.0f, 0.0f)*0.0025f * (float4)(255.0f/141.0f, 255.0f/167.0f, 255.0f/255.0f, 1.0f);
	//finalColor[currIndex]+= (float4)(0.0f, 0.0f, 1.0f, 1.0f)*sqrt( max(normals[currIndex].w - 100.0f, 0.0f)*0.001f);
}