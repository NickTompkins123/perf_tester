
inline float4 quaternionConjugate(const float4 v)
{
	return (float4)(-v.s0, -v.s1, -v.s2, v.s3);
}

inline float4 quaternionMultiply(const float4 a, const float4 b)
{
	return (float4)
	(
		a.s3*b.s0 +a.s0*b.s3 +a.s1*b.s2 -a.s2*b.s1,
		a.s3*b.s1 -a.s0*b.s2 +a.s1*b.s3 +a.s2*b.s0,
		a.s3*b.s2 +a.s0*b.s1 -a.s1*b.s0 +a.s2*b.s3,
		a.s3*b.s3 -a.s0*b.s0 -a.s1*b.s1 -a.s2*b.s2
	);
}

inline float4 quaternionMultiplyVector(const float4 q, const float4 v)
{
	return (float4)
	(
		q.s3*v.s0 +q.s0*v.s3 +q.s1*v.s2 -q.s2*v.s1,
		q.s3*v.s1 -q.s0*v.s2 +q.s1*v.s3 +q.s2*v.s0,
		q.s3*v.s2 +q.s0*v.s1 -q.s1*v.s0 +q.s2*v.s3,
		0.0f
	);
}

inline float4 quaternionAngleRotator(const float angle, const float4 axis)
{
	float cosAngle;
	float sinAngle = sincos(angle*0.5f, &cosAngle);

	float4 r = axis;
	r.s0 *= sinAngle;
	r.s1 *= sinAngle;
	r.s2 *= sinAngle;
	r.s3  = cosAngle;

	return r;
}

inline float4 rotateVectorAxisAngle(const float4 v, const float angle, const float4 axis)
{
	float4 q = quaternionAngleRotator(angle, axis);
	return quaternionMultiplyVector(q, quaternionMultiply(v, quaternionConjugate(q)));
}

