#define FLOAT_PRECISION 1

#if FLOAT_PRECISION == 1

#define vfloat float
#define vfloat2 float2
#define vfloat4 float4

#define VSTORE_FLOAT(v, i, a) a[i] = v
#define VSTORE_FLOAT2(v, i, a) a[i] = v
#define VSTORE_FLOAT4(v, i, a) a[i] = v

#define VLOAD_FLOAT(i, a) a[i]
#define VLOAD_FLOAT2(i, a) a[i]
#define VLOAD_FLOAT4(i, a) a[i]

#else

#define vfloat half
#define vfloat2 half
#define vfloat4 half

#define VSTORE_FLOAT(v, i, a) vstore_half(v, i, a)
#define VSTORE_FLOAT2(v, i, a) vstore_half2(v, i, a)
#define VSTORE_FLOAT4(v, i, a) vstore_half4(v, i, a)

#define VLOAD_FLOAT(i, a) vload_half(i, a)
#define VLOAD_FLOAT2(i, a) vload_half2(i, a)
#define VLOAD_FLOAT4(i, a) vload_half4(i, a)

#endif
