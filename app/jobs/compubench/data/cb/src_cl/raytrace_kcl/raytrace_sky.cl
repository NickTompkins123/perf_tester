inline float Perez (float A, float B, float C, float D, float E, float theta, float gamma){
	return ( 1 + A*exp(B/cos(theta)) ) * ( 1 + C*exp(D*gamma) + E*cos(gamma)*cos(gamma) );
}

float4 GetSkyColor ( float4 sun_dir, float4 view_dir, float T, float exposure, float sun_size, float sunIntensity, float2 xyYcolor){

	float4 zenit = (float4) (0.0f, 1.0f, 0.0f, 0.0f);
	float theta_sun =acos(clamp(dot(zenit,sun_dir), -0.99999f, 0.99999f));
	float theta = acos(clamp(dot(zenit,view_dir), -0.99999f, 0.99999f));
	float gamma = acos(clamp(dot(sun_dir,view_dir), -0.99999f, 0.99999f));

	float3 A, B, C, D, E, zenith, xyY, XYZ;
	float4 rgb;

	if (theta > M_PI_F/2-0.00001f) theta = 0.00001f;

	A.x = -0.0193f*T - 0.2592f; A.y = -0.0167f*T - 0.2608f; A.z =   0.1787f*T - 1.4630f;
	B.x = -0.0665f*T + 0.0008f; B.y = -0.0950f*T + 0.0092f; B.z =  -0.3554f*T + 0.4275f;
	C.x = -0.0004f*T + 0.2125f; C.y = -0.0079f*T + 0.2102f; C.z =  -0.0227f*T + 5.3251f;
	D.x = -0.0641f*T - 0.8989f; D.y = -0.0441f*T - 1.6537f; D.z =   0.1206f*T - 2.5771f;
	E.x = -0.0033f*T + 0.0452f; E.y = -0.0109f*T + 0.0529f; E.z =  -0.0670f*T + 0.3703f;

	float chi = (4.0f/9.0f - T/120.0f)*(M_PI_F - 2.0f*theta_sun);
	zenith.z = ((4.0453f*T - 4.9710f)*tan(chi) - 0.2155f*T + 2.4192f) * 1000.0f;
	zenith.x = ( 0.00166f*theta_sun*theta_sun*theta_sun - 0.00375f*theta_sun*theta_sun + 0.00209f*theta_sun + 0.00000f )*T*T +
	           (-0.02903f*theta_sun*theta_sun*theta_sun + 0.06377f*theta_sun*theta_sun - 0.03202f*theta_sun + 0.00394f )*T   +
			   ( 0.11693f*theta_sun*theta_sun*theta_sun - 0.21196f*theta_sun*theta_sun + 0.06052f*theta_sun + 0.25886f );
	zenith.y = ( 0.00275f*theta_sun*theta_sun*theta_sun - 0.00610f*theta_sun*theta_sun + 0.00317f*theta_sun + 0.00000f )*T*T +
	           (-0.04214f*theta_sun*theta_sun*theta_sun + 0.08970f*theta_sun*theta_sun - 0.04153f*theta_sun + 0.00516f )*T   +
			   ( 0.15346f*theta_sun*theta_sun*theta_sun - 0.26756f*theta_sun*theta_sun + 0.06670f*theta_sun + 0.26688f );
	
	xyY.x = zenith.x * (Perez(A.x,B.x,C.x,D.x,E.x,theta,gamma)/Perez(A.x,B.x,C.x,D.x,E.x,0.0f,theta_sun));
	xyY.y = zenith.y * (Perez(A.y,B.y,C.y,D.y,E.y,theta,gamma)/Perez(A.y,B.y,C.y,D.y,E.y,0.0f,theta_sun));
	xyY.z = zenith.z * (Perez(A.z,B.z,C.z,D.z,E.z,theta,gamma)/Perez(A.z,B.z,C.z,D.z,E.z,0.0f,theta_sun));
	float Kn = zenith.z * (Perez(A.z,B.z,C.z,D.z,E.z,theta_sun,0.0f)/Perez(A.z,B.z,C.z,D.z,E.z,0.0f,theta_sun));
	xyY.z /= Kn;
	
	
    //Sun highlighting
	if ( degrees(gamma) < sun_size && exposure==0.0f){
		float sigma = 0.5f;
		float x = degrees(gamma)/sun_size*3.0f; 
		xyY.z += sunIntensity * exp( -(x*x)/(2*sigma*sigma) ) * xyY.z;
		//xyY.x+=0.04f;
		//xyY.y+=0.04f;
		//xyY.x=0.323f+xyYcolor.x;
		//xyY.y=0.343f+xyYcolor.y;
		//xyY.x=0.332f+xyYcolor.x;
		//xyY.y=0.349f+xyYcolor.y;
		xyY.x=0.341f;//+xyYcolor.x;
		xyY.y=0.355f+xyYcolor.y;
	}

	//convert xyY to XYZ
	XYZ.x = xyY.x/xyY.y*xyY.z;
	XYZ.y = xyY.z;
	XYZ.z = ((1.0f - xyY.x - xyY.y) / xyY.y) * xyY.z;
	
	//convert XYZ to rgb
	rgb.x =  3.240479f*XYZ.x - 1.537150f*XYZ.y - 0.498535f*XYZ.z;
	rgb.y = -0.969256f*XYZ.x + 1.875992f*XYZ.y + 0.041556f*XYZ.z;
	rgb.z =  0.055648f*XYZ.x - 0.204043f*XYZ.y + 1.057311f*XYZ.z;
	rgb.w = XYZ.y;

	if (exposure!=0.0f){
		rgb = HDR(rgb,exposure, 0);
		if ( degrees(gamma) < sun_size){
			float sigma = 0.5f;
			float x = degrees(gamma)/sun_size*3.0f; 
			rgb += sunIntensity * exp( -(x*x)/(2*sigma*sigma) ) * rgb;
		}
	
	}
	return rgb;
}

__kernel void generateSkyMap ( global CLSun* sun, __global const CLLight* light, __global float* skyMap){

	const size_t x = get_global_id(0);
	const size_t y = get_global_id(1);
	const size_t x_size = get_global_size(0);
	const size_t y_size = get_global_size(1);

	float phi = M_PI_F* 2.0f * ( ((float)(x) + 0.5f) / (float)(x_size) );
	float theta = M_PI_F * 0.5f * ( ((float)(y) + 0.5f)/ (float)(y_size) );

	float4 view_dir = normalize( (float4)(sin(phi)*sin(theta), cos(theta), cos(phi)*sin(theta), 0.0f) );
	float4 data = GetSkyColor ((*light).v, view_dir, (*sun).turbidity, 0.0f, (*sun).size, (*sun).intensity, (*sun).xyYcolor);
	skyMap[y*x_size + x] = data.w;
}

__kernel void conditionalDensity (const uint skySizeX, __global float* skyMap, __global float* conditional, __global float* conditionalAvg){
	const int x = get_global_id(0);
	const int x_size = get_global_size(0);

	uint n=skySizeX;
	conditional[x*(skySizeX+1)] =0.0f;
	for (int i=1; i<n+1; i++)
		conditional[x*(n+1) + i] = conditional[x*(n+1) + i-1] + skyMap[x*n + i-1]/(float)(n);
	
	conditionalAvg[x] = conditional[x*(n+1) + n];

	for (int i=1; i<n+1; i++)
		conditional[x*(n+1) + i]/=conditionalAvg[x];
}

__kernel void marginalDensity  (const uint skySizeY, __global float* conditionalAvg, __global float* marginal, __global float* marginalAvg){
	const int x = get_global_id(0);
	const int x_size = get_global_size(0);

	uint n=skySizeY;
	marginal[0] = 0.0f;
	for (int i=1; i<n+1; i++)
		marginal[i] = marginal[i-1] + conditionalAvg[i-1]/n;
	marginalAvg[0] = marginal[n];
	for (int i=1; i<n+1; i++)
		marginal[i] /=marginalAvg[0];
}

__kernel void generateSkyRay (const uint skySizeX, const uint skySizeY, __global const CLLight* light, __global const float* restrict skyrayweights, __global float2* randomNumbers,
							  __global float* skyMap, __global float* conditional, __global float* conditionalAvg, __global float* marginal, __global float* marginalAvg,
							  __global const CLHitData* restrict hitdata, __global CLRay* ray, __global CLRay* shadowRay, __global CLRay* skyRay, __global float* prob){

	const size_t currIndex = get_global_id(1)*get_global_size(0) + get_global_id(0);

	if (skyrayweights[currIndex] == 0.0f) return;
	float u = randomNumbers[currIndex].x;
	float v = randomNumbers[currIndex].y;


	int first = 0;
	int last = skySizeY+1;
	int middle = (first + last)/2;

	while((last-first) > 1 ){
		if (marginal[middle] > u)
			last = middle;
		else
			first = middle;
		middle = (first + last)/2;
	}
	int i = max(0,first);

	first = 0;
	last = skySizeX+1;
	middle = (first + last)/2;

	while((last-first) > 1 ){
		if (conditional[(i)*(skySizeX+1) + middle] > v)
			last = middle;
		else
			first = middle;
		middle = (first + last)/2;
	}
	int j = max(0,first);


	/*int i=0;
	while (u>marginal[i] && i<skySizeY+1)
		i++;
	i = max(0,i-1);

	int j=0;
	while (v>conditional[(i)*(skySizeX+1) + j] && j<skySizeX+1)
		j++;
	j =max(0,j-1);*/

	float theta = M_PI_F * 0.5f * (i + 0.5f + (u - marginal[i]) / (marginal[i+1] - marginal[i]))/(float)(skySizeY);
	float phi = M_PI_F * 2.0f * (j + 0.5f + (v - conditional[(i)*(skySizeX+1) + j]) / (conditional[(i)*(skySizeX+1) + j+1] - conditional[(i)*(skySizeX+1) + j]))/(float)(skySizeX);
	prob[currIndex] = sin(theta) < 0.00001f ? 0.0f : ((conditionalAvg[i]/marginalAvg[0])*(skyMap[i*skySizeX+j]/conditionalAvg[i]))/(M_PI_F*M_PI_F*sin(theta));

	skyRay[currIndex].dir = normalize( (float4)(sin(phi)*sin(theta), cos(theta), cos(phi)*sin(theta), 0.0f) );
	skyRay[currIndex].org = hitdata[currIndex].hitPosition + 0.001f*hitdata[currIndex].hitNormal;
	
	shadowRay[currIndex].dir = (*light).v;
	shadowRay[currIndex].org = hitdata[currIndex].hitPosition + 0.001f*hitdata[currIndex].hitNormal;


}
__kernel void clearSkyColor (__global const CLHitData* restrict hitdata, __global float4* skyColor, __global float* skyrayweights){
	const size_t currIndex = get_global_id(1)*get_global_size(0) + get_global_id(0);

	skyrayweights[currIndex] = hitdata[currIndex].matId == INT_MAX ? 0.0f : 1.0f;
	skyColor[currIndex] = (float4)(0.0f);
}

__kernel void computeSkyColor(const uint iteration, const uint depth, const uint actualIterNum, global CLSun* sun, __global CLRay* skyRay, __global float* prob, __global const CLHitData* restrict hitdata, __global const CLHitRecord* restrict hitrecs, __global const CLLight* light, __global CLRay* ray, 
	                          __global const uchar4* restrict posx, __global const uchar4* restrict negx, __global const uchar4* restrict posy, __global const uchar4* restrict negy, __global const uchar4* restrict posz, __global const uchar4* restrict negz,__global float4* skyColor, __global float4* skyMask){
	const size_t currIndex = get_global_id(1)*get_global_size(0) + get_global_id(0);
	
	if (hitrecs[currIndex].triId == INT_MAX && hitdata[currIndex].matId != INT_MAX){
		float cost = dot(skyRay[currIndex].dir, hitdata[currIndex].hitNormal);
		skyColor[currIndex] += cost > 0.0f ? (GetSkyColor ((*light).v, skyRay[currIndex].dir, (*sun).turbidity, 0.0f, (*sun).size, (*sun).intensity, (*sun).xyYcolor)*cost)/prob[currIndex] : (float4)(0.0f);
	}
	if (hitdata[currIndex].matId==INT_MAX && depth==0 && iteration==1/* && actualIterNum==0*/){
		skyMask[currIndex] = sampleEnvMap(ray+currIndex, posx, negx, posy, negy, posz, negz);
		//skyColor[currIndex] += GetSkyColor ((*light).v, ray[currIndex].dir, (*sun).turbidity, (*sun).exposure, (*sun).size, 5.0f, (*sun).xyYcolor);
		//skyColor[currIndex] += sampleEnvMapSky(ray+currIndex, posx, negx, posy, negy, posz, negz);
	}
	if (hitdata[currIndex].matId==INT_MAX && depth!=0){
		skyColor[currIndex] += GetSkyColor ((*light).v, ray[currIndex].dir, (*sun).turbidity, 0.0f, (*sun).size, 5.0f, (*sun).xyYcolor) + 0.1f;
	}
}