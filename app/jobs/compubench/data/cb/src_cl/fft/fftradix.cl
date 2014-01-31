float2 cplxMul(const float2 cmplxA, const float2 cmplxB)
{
	return (float2)(cmplxA.x*cmplxB.x - cmplxA.y*cmplxB.y, cmplxA.x*cmplxB.y + cmplxA.y*cmplxB.x);
}

float2 twiddle(const float2 a, const int k, const float alpha)
{
  float sn = sin(k*alpha);
  float cs = cos(k*alpha);
  return cplxMul(a, (float2)(cs,sn));
}

__constant float SQRT_1_2 = 0.707106781188f;

float2 mul_p0q2(float2 a) { return a; }
float2 mul_p1q2(float2 a) { return (float2)(a.y,-a.x); }

float2 mul_p0q4(float2 a) { return a; }
float2 mul_p1q4(float2 a) { return (float2)(SQRT_1_2)*(float2)(a.x+a.y,-a.x+a.y); }
float2 mul_p2q4(float2 a) { return (float2)(a.y,-a.x); }
float2 mul_p3q4(float2 a) { return (float2)(SQRT_1_2)*(float2)(-a.x+a.y,-a.x-a.y); }

__kernel void fftRadix2( __global const float2* dataIn, __global float2* dataOut, const int subSeqLength )
{
	unsigned int iThread = get_global_id(0);
	unsigned int nThread = get_global_size(0);

	int inIdx = iThread&(subSeqLength-1);
	int outIdx = ((iThread-inIdx)<<1) + inIdx;
	float alpha = -3.14159265359f * inIdx / subSeqLength;

	// read and twiddle input
	float2 u0 = dataIn[iThread];
	float2 u1 = twiddle(dataIn[iThread+nThread], 1, alpha);

	// 1x DFT-2 and write
	dataOut[outIdx] = u0 + u1;
	dataOut[outIdx+subSeqLength] = u0 - u1;
}

__kernel void fftRadix4( __global const float2* dataIn, __global float2* dataOut, const int subSeqLength )
{
	unsigned int iThread = get_global_id(0);
	unsigned int nThread = get_global_size(0);

	int inIdx = iThread&(subSeqLength-1);
	int outIdx = ((iThread-inIdx)<<2) + inIdx;
	float alpha = -M_PI_F * inIdx / (2.0f*subSeqLength);

	// read and twiddle input
	float2 u0 = dataIn[iThread];
	float2 u1 = twiddle(dataIn[iThread+1*nThread], 1, alpha);
	float2 u2 = twiddle(dataIn[iThread+2*nThread], 2, alpha);
	float2 u3 = twiddle(dataIn[iThread+3*nThread], 3, alpha);

	// 2x DFT2 and twiddle
	float2 v0 = u0 + u2;
	float2 v1 = u1 + u3;
	float2 v2 = u0 - u2;
	float2 v3 = (float2)(u1.y-u3.y,u3.x-u1.x);

	// 2x DFT2 and write
	dataOut[outIdx] = v0 + v1;
	dataOut[outIdx+1*subSeqLength] = v2 + v3;
	dataOut[outIdx+2*subSeqLength] = v0 - v1;
	dataOut[outIdx+3*subSeqLength] = v2 - v3;
}

__kernel void fftRadix8( __global const float2* dataIn, __global float2* dataOut, const int subSeqLength )
{
	unsigned int iThread = get_global_id(0);
	unsigned int nThread = get_global_size(0);

	int inIdx = iThread&(subSeqLength-1);
	int outIdx = ((iThread-inIdx)<<3) + inIdx;
	float alpha = -M_PI_F * inIdx / (4.0f*subSeqLength);

	// read and twiddle input
	float2 u0 = dataIn[iThread];
	float2 u1 = twiddle(dataIn[iThread+1*nThread], 1, alpha);
	float2 u2 = twiddle(dataIn[iThread+2*nThread], 2, alpha);
	float2 u3 = twiddle(dataIn[iThread+3*nThread], 3, alpha);
	float2 u4 = twiddle(dataIn[iThread+4*nThread], 4, alpha);
	float2 u5 = twiddle(dataIn[iThread+5*nThread], 5, alpha);
	float2 u6 = twiddle(dataIn[iThread+6*nThread], 6, alpha);
	float2 u7 = twiddle(dataIn[iThread+7*nThread], 7, alpha);

	// 4x DFT2 and twiddle
	float2 v0 = u0 + u4;
	float2 v1 = u1 + u5;
	float2 v2 = u2 + u6;
	float2 v3 = u3 + u7;
	float2 v4 = mul_p0q4(u0 - u4);
	float2 v5 = mul_p1q4(u1 - u5);
	float2 v6 = mul_p2q4(u2 - u6);
	float2 v7 = mul_p3q4(u3 - u7);

	// 4x DFT2 and twiddle
	u0 = v0 + v2;
	u2 = mul_p0q2(v0 - v2);
	u1 = v1 + v3;
	u3 = mul_p1q2(v1 - v3);
	u4 = v4 + v6;
	u6 = mul_p0q2(v4 - v6);
	u5 = v5 + v7;
	u7 = mul_p1q2(v5 - v7);

	// 4x DFT2 and write
	dataOut[outIdx] = u0 + u1;
	dataOut[outIdx+1*subSeqLength] = u4 + u5;
	dataOut[outIdx+2*subSeqLength] = u2 + u3;
	dataOut[outIdx+3*subSeqLength] = u6 + u7;
	dataOut[outIdx+4*subSeqLength] = u0 - u1;
	dataOut[outIdx+5*subSeqLength] = u4 - u5;
	dataOut[outIdx+6*subSeqLength] = u2 - u3;
	dataOut[outIdx+7*subSeqLength] = u6 - u7;
}

__constant float COS_8 = 0.923879532511f; // cos(Pi/8)

__constant float SIN_8 = 0.382683432365f; // sin(Pi/8)

#define MUL_RE(a,b) (a.even*b.even - a.odd*b.odd)

#define MUL_IM(a,b) (a.even*b.odd + a.odd*b.even)

float2 mul_1(float2 a,float2 b)
{ 
	float2 x; x.even = MUL_RE(a,b); x.odd = MUL_IM(a,b); return x;
}

float2 exp_alpha_1(float alpha)
{
  float cs,sn;

  // sn = sincos(alpha,&cs);  // sincos

  cs = native_cos(alpha); sn = native_sin(alpha);  // native sin+cos

  // cs = cos(alpha); sn = sin(alpha); // sin+cos

  return (float2)(cs,sn);
}

#define mul_p0q8 mul_p0q4
float2  mul_p1q8(float2 a) { return mul_1((float2)(COS_8,-SIN_8),a); }
#define mul_p2q8 mul_p1q4
float2  mul_p3q8(float2 a) { return mul_1((float2)(SIN_8,-COS_8),a); }
#define mul_p4q8 mul_p2q4
float2  mul_p5q8(float2 a) { return mul_1((float2)(-SIN_8,-COS_8),a); }
#define mul_p6q8 mul_p3q4
float2  mul_p7q8(float2 a) { return mul_1((float2)(-COS_8,-SIN_8),a); }

// Compute in-place DFT2 and twiddle
#define DFT2_TWIDDLE(a,b,t) { float2 tmp = t(a-b); a += b; b = tmp; }

__kernel void fftRadix16( __global const float2* x, __global float2* y, const int p )
{

  int t = get_global_size(0); // number of threads

  int i = get_global_id(0); // current thread

  int k = i & (p-1); // index in input sequence, in 0..P-1

  // Inputs indices are I+{0,..,15}*T

  x += i;

  // Output indices are J+{0,..,15}*P, where

  // J is I with four 0 bits inserted at bit log2(P)

  y += ((i-k)<<4) + k;



  // Load

  float2 u[16];

  for (int m=0;m<16;m++) u[m] = x[m*t];



  // Twiddle, twiddling factors are exp(_I*PI*{0,..,15}*K/4P)

  float alpha = -M_PI*(float)k/(float)(8*p);

  for (int m=1;m<16;m++) u[m] = mul_1(exp_alpha_1(m*alpha),u[m]);



  // 8x in-place DFT2 and twiddle (1)

  DFT2_TWIDDLE(u[0],u[8],mul_p0q8);

  DFT2_TWIDDLE(u[1],u[9],mul_p1q8);

  DFT2_TWIDDLE(u[2],u[10],mul_p2q8);

  DFT2_TWIDDLE(u[3],u[11],mul_p3q8);

  DFT2_TWIDDLE(u[4],u[12],mul_p4q8);

  DFT2_TWIDDLE(u[5],u[13],mul_p5q8);

  DFT2_TWIDDLE(u[6],u[14],mul_p6q8);

  DFT2_TWIDDLE(u[7],u[15],mul_p7q8);



  // 8x in-place DFT2 and twiddle (2)

  DFT2_TWIDDLE(u[0],u[4],mul_p0q4);

  DFT2_TWIDDLE(u[1],u[5],mul_p1q4);

  DFT2_TWIDDLE(u[2],u[6],mul_p2q4);

  DFT2_TWIDDLE(u[3],u[7],mul_p3q4);

  DFT2_TWIDDLE(u[8],u[12],mul_p0q4);

  DFT2_TWIDDLE(u[9],u[13],mul_p1q4);

  DFT2_TWIDDLE(u[10],u[14],mul_p2q4);

  DFT2_TWIDDLE(u[11],u[15],mul_p3q4);



  // 8x in-place DFT2 and twiddle (3)

  DFT2_TWIDDLE(u[0],u[2],mul_p0q2);

  DFT2_TWIDDLE(u[1],u[3],mul_p1q2);

  DFT2_TWIDDLE(u[4],u[6],mul_p0q2);

  DFT2_TWIDDLE(u[5],u[7],mul_p1q2);

  DFT2_TWIDDLE(u[8],u[10],mul_p0q2);

  DFT2_TWIDDLE(u[9],u[11],mul_p1q2);

  DFT2_TWIDDLE(u[12],u[14],mul_p0q2);

  DFT2_TWIDDLE(u[13],u[15],mul_p1q2);



  // 8x DFT2 and store (reverse binary permutation)

  y[0]    = u[0]  + u[1];

  y[p]    = u[8]  + u[9];

  y[2*p]  = u[4]  + u[5];

  y[3*p]  = u[12] + u[13];

  y[4*p]  = u[2]  + u[3];

  y[5*p]  = u[10] + u[11];

  y[6*p]  = u[6]  + u[7];

  y[7*p]  = u[14] + u[15];

  y[8*p]  = u[0]  - u[1];

  y[9*p]  = u[8]  - u[9];

  y[10*p] = u[4]  - u[5];

  y[11*p] = u[12] - u[13];

  y[12*p] = u[2]  - u[3];

  y[13*p] = u[10] - u[11];

  y[14*p] = u[6]  - u[7];

  y[15*p] = u[14] - u[15];

}