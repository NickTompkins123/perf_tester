inline CLGeometryDifferential GetGeometryDifferential(const __global float4* triverts, const __global float2* triuvs)
{
	const float du01 = triuvs[1].x - triuvs[0].x;
	const float du02 = triuvs[2].x - triuvs[0].x;

	const float dv01 = triuvs[1].y - triuvs[0].y;
	const float dv02 = triuvs[2].y - triuvs[0].y;

	const float4 dp01 = triverts[1] - triverts[0];
	const float4 dp02 = triverts[2] - triverts[0];

	const float det = du01 * dv02 - dv01 * du02;

	CLGeometryDifferential geomdiff;

	if(det != 0.0f)
	{
		geomdiff.dpdu = ( dv02 * dp01 - dv01 * dp02 ) / det;
		geomdiff.dpdv = (-du02 * dp01 + du01 * dp02 ) / det;
	}
	else
	{
		const float4 N = normalize( cross(dp01, dp02) );
		const float4 dx0 = cross( (float4)(1.0f, 0.0f, 0.0f, 0.0f), N );
		const float4 dx1 = cross( (float4)(0.0f, 1.0f, 0.0f, 0.0f), N );
		
		geomdiff.dpdu = normalize( select( dx0, dx1, (int4)(dot(dx0,dx0) < dot(dx1,dx1)) ) );
		geomdiff.dpdv = normalize( cross(N,geomdiff.dpdu) );
	}

	return geomdiff;
}
/**/

inline void SolveLinearSystem2x2(const float A[2][2], const float B[2], float *x0, float *x1)
{
	const float det = A[0][0]*A[1][1] - A[0][1]*A[1][0];
	
	if( fabs(det) > 1e-10f )
	{
		*x0 = (A[1][1]*B[0] - A[0][1]*B[1]) / det;
		*x1 = (A[0][0]*B[1] - A[1][0]*B[0]) / det;
	}
	else
	{
		*x0 = 0.0f;
		*x1 = 0.0f;
	}
}
/**/

inline CLIntersectionDifferential GetIntersectionDifferential(const CLRayDifferential* raydiff, const CLGeometryDifferential* geomdiff, const __global float4* triverts, const float4* hitPos)
{
	const float4 e01 = triverts[1] - triverts[0];
	const float4 e02 = triverts[2] - triverts[0];
	float4 triplane = normalize( cross(e01, e02) );
	triplane.w = -dot(triplane, triverts[0]);

	const float tx = -dot(triplane, raydiff->rxOrg) / dot(triplane, raydiff->rxDir);
	const float ty = -dot(triplane, raydiff->ryOrg) / dot(triplane, raydiff->ryDir);

	const float4 px = raydiff->rxOrg + tx * raydiff->rxDir;
	const float4 py = raydiff->ryOrg + ty * raydiff->ryDir;

	float A[2][2], Bx[2], By[2];
	int axes[2];
	if (fabs(triplane.x) > fabs(triplane.y) && fabs(triplane.x) > fabs(triplane.z))
	{
		axes[0] = 1; axes[1] = 2;
    }
	else if (fabs(triplane.y) > fabs(triplane.z))
	{
		axes[0] = 0; axes[1] = 2;
	}
    else
	{
		axes[0] = 0; axes[1] = 1;
	}

	// Initialize matrices for chosen projection plane
	//A[0][0] = geomdiff->dpdu[axes[0]];
	//A[0][1] = geomdiff->dpdv[axes[0]];
	//A[1][0] = geomdiff->dpdu[axes[1]];
	//A[1][1] = geomdiff->dpdv[axes[1]];

	//Bx[0] = px[axes[0]] - p[axes[0]];
	//Bx[1] = px[axes[1]] - p[axes[1]];
	//By[0] = py[axes[0]] - p[axes[0]];
	//By[1] = py[axes[1]] - p[axes[1]];

	A[0][0] = select(A[0][0], geomdiff->dpdu.x, axes[0]==0);
	A[0][0] = select(A[0][0], geomdiff->dpdu.y, axes[0]==1);
	A[0][0] = select(A[0][0], geomdiff->dpdu.z, axes[0]==2);

	A[0][1] = select(A[0][1], geomdiff->dpdv.x, axes[0]==0);
	A[0][1] = select(A[0][1], geomdiff->dpdv.y, axes[0]==1);
	A[0][1] = select(A[0][1], geomdiff->dpdv.z, axes[0]==2);

	A[1][0] = select(A[1][0], geomdiff->dpdu.x, axes[1]==0);
	A[1][0] = select(A[1][0], geomdiff->dpdu.y, axes[1]==1);
	A[1][0] = select(A[1][0], geomdiff->dpdu.z, axes[1]==2);

	A[1][1] = select(A[1][1], geomdiff->dpdv.x, axes[1]==0);
	A[1][1] = select(A[1][1], geomdiff->dpdv.y, axes[1]==1);
	A[1][1] = select(A[1][1], geomdiff->dpdv.z, axes[1]==2);

	Bx[0] = select(Bx[0], px.x-(*hitPos).x, axes[0]==0);
	Bx[0] = select(Bx[0], px.y-(*hitPos).y, axes[0]==1);
	Bx[0] = select(Bx[0], px.z-(*hitPos).z, axes[0]==2);

	Bx[1] = select(Bx[1], px.x-(*hitPos).x, axes[1]==0);
	Bx[1] = select(Bx[1], px.y-(*hitPos).y, axes[1]==1);
	Bx[1] = select(Bx[1], px.z-(*hitPos).z, axes[1]==2);

	By[0] = select(By[0], py.x-(*hitPos).x, axes[0]==0);
	By[0] = select(By[0], py.y-(*hitPos).y, axes[0]==1);
	By[0] = select(By[0], py.z-(*hitPos).z, axes[0]==2);

	By[1] = select(By[0], py.x-(*hitPos).x, axes[1]==0);
	By[1] = select(By[0], py.y-(*hitPos).y, axes[1]==1);
	By[1] = select(By[0], py.z-(*hitPos).z, axes[1]==2);

	CLIntersectionDifferential interdiff;

	SolveLinearSystem2x2(A, Bx, &(interdiff.dudx), &(interdiff.dvdx));
	SolveLinearSystem2x2(A, By, &(interdiff.dudy), &(interdiff.dvdy));

	return interdiff;
}
/**/

float4 textureBilinearLookup(__global const uchar4* restrict texture, int2 iTexSize, const float2 UV)
{
	float2 fTexSize = convert_float2(iTexSize);

	float2 fractDummy;
	float2 _UV = fract(UV, &fractDummy);

	const float2 uv_f = _UV*fTexSize;
	fTexSize -= 1.0f;

	const float2 uv_f00 = clamp(floor(uv_f), (float2)(0.0f), fTexSize);
	const float2 uv_f11 = clamp(ceil(uv_f), (float2)(0.0f), fTexSize);
	const float2 uv_f01 = (float2)(uv_f11.x, uv_f00.y);
	const float2 uv_f10 = (float2)(uv_f00.x, uv_f11.y);
	const uint2 uv_00 = convert_uint2(uv_f00);
	const uint2 uv_11 = convert_uint2(uv_f11);
	const uint2 uv_01 = (uint2)(uv_11.x, uv_00.y);
	const uint2 uv_10 = (uint2)(uv_00.x, uv_11.y);

	const float4 t00 = convert_float4(texture[uv_00.x * iTexSize.y + uv_00.y]) / 255.0f;
	const float4 t01 = convert_float4(texture[uv_01.x * iTexSize.y + uv_01.y]) / 255.0f;
	const float4 t10 = convert_float4(texture[uv_10.x * iTexSize.y + uv_10.y]) / 255.0f;
	const float4 t11 = convert_float4(texture[uv_11.x * iTexSize.y + uv_11.y]) / 255.0f;

	const float2 alpha = uv_f - uv_f00;
    
	return	mix
			(	
				mix( t00, t01, (float4)(alpha.x) ), 
				mix( t10, t11, (float4)(alpha.x) ), 
				(float4)(alpha.y)
			);
    
    //return alpha.y * (alpha.x * t00 + (1.0f-alpha.x)*t01) + (1.0f-alpha.y) * (alpha.x * t10 + (1.0f-alpha.x) * t11);
}
/**/

float4 textureBilinearLookupLevel(__global const uchar4* restrict texture, float width, int2 texSize, float2 uv)
{
	float maxLevel = log2((float)texSize.x);

	float _level = maxLevel - 1 + log2( max(width, 1e-8f) );
	_level = clamp(_level, 0.0f, maxLevel-1);

	int iLevel = (int)(_level);
	float delta= _level-iLevel;

	int offset0 = (texSize.x*texSize.y * (pown(0.25f, iLevel) - 1.0f)) / -0.75f;
	int offset1 = (texSize.x*texSize.y * (pown(0.25f, iLevel+1) - 1.0f)) / -0.75f;

	return (1.0f - delta) * textureBilinearLookup(texture+offset0, texSize / (int2)(1 << iLevel), uv) +
				   delta  * textureBilinearLookup(texture+offset1, texSize / (int2)(1 << (iLevel+1)), uv);
}
/**/

float4 sampleEnvMap(__global const CLRay* ray, __global const uchar4* restrict posx, __global const uchar4* restrict negx, __global const uchar4* restrict posy, __global const uchar4* restrict negy, __global const uchar4* restrict posz, __global const uchar4* restrict negz)
{
	float4 cubemapColor = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
    
    const uint2 minval = (uint2)(0);
    const uint2 maxval = (uint2)(511);
    
	if( (fabs(ray->dir.x) >= fabs(ray->dir.y)) && (fabs(ray->dir.x) >= fabs(ray->dir.z)) )
	{
		if (1.0f / ray->dir.x > 0.0f)
		{
			float u = (ray->dir.y * 1.0f / ray->dir.x + 1.0f) * 0.5f;	
			float v = (ray->dir.z * 1.0f / ray->dir.x + 1.0f) * 0.5f;

			uint2 t = convert_uint2( (float2)(u,v) * (512.0f - 1.0f) );
            t = min( max(t, minval), maxval);
			cubemapColor = convert_float4( posx[t.x * 512 + t.y] ) / 255.0f;
		}
		else
		{
			float u = 1.0f - (ray->dir.y * 1.0f / ray->dir.x + 1.0f) * 0.5f;	
			float v =		 (ray->dir.z * 1.0f / ray->dir.x + 1.0f) * 0.5f;

			uint2 t = convert_uint2( (float2)(u,v) * (512.0f - 1.0f) );
            t = min( max(t, minval), maxval);
			cubemapColor = convert_float4( negx[t.x * 512 + t.y] ) / 255.0f;
		}
	}
	else if( fabs(ray->dir.y) >= fabs(ray->dir.z) )
	{
		if (1.0f / ray->dir.y > 0.0f)
		{
			float u = (ray->dir.z * 1.0f / ray->dir.y + 1.0f) * 0.5f;	
			float v = (ray->dir.x * 1.0f / ray->dir.y + 1.0f) * 0.5f;

			uint2 t = convert_uint2( (float2)(u,v) * (512.0f - 1.0f) );
            t = min( max(t, minval), maxval);
			cubemapColor = convert_float4( posy[t.x * 512 + t.y] ) / 255.0f;
		}
		else
		{
			float u = (ray->dir.z * 1.0f / ray->dir.y + 1.0f) * 0.5f;
			float v = 1.0f - (ray->dir.x * 1.0f / ray->dir.y + 1.0f) * 0.5f;	

			uint2 t = convert_uint2( (float2)(u,v) * (512.0f - 1.0f) );
            t = min( max(t, minval), maxval);
			cubemapColor = convert_float4( negy[t.x * 512 + t.y] ) / 255.0f;
		}
	}
	else
	{
		if (1.0f / ray->dir.z > 0.0f)
		{
			float u = (ray->dir.y * 1.0f / ray->dir.z + 1.0f) * 0.5f;	
			float v = 1.0f - (ray->dir.x * 1.0f / ray->dir.z + 1.0f) * 0.5f;

			uint2 t = convert_uint2( (float2)(u,v) * (512.0f - 1.0f) );
            t = min( max(t, minval), maxval);
			cubemapColor = convert_float4( negz[t.x * 512 + t.y] ) / 255.0f;
		}
		else
		{
			float u = 1.0f - (ray->dir.y * 1.0f / ray->dir.z + 1.0f) * 0.5f;
			float v = 1.0f - (ray->dir.x * 1.0f / ray->dir.z + 1.0f) * 0.5f;

			uint2 t = convert_uint2( (float2)(u,v) * (512.0f - 1.0f) );
            t = min( max(t, minval), maxval);
			cubemapColor = convert_float4( posz[t.x * 512 + t.y] ) / 255.0f;
		}
	}

	return cubemapColor;
}
/**/