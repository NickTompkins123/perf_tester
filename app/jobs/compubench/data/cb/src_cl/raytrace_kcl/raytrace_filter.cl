constant int RADIUS = 10;

__kernel void guidedCreateInputData( __global  float4* normals, __global  float4* image_in, __global float4* GFInput0, __global float4* GFInput1)
{
	const size_t currIndex = get_global_id(1) * get_global_size(0) + get_global_id(0);
	const size_t offset = get_global_size(0) * get_global_size(1);

	float4 input0p0, input0p1, input0p2, input0p3;
	float4 input1p0, input1p1, input1p2, input1p3;
	float4 image = image_in[currIndex];
	float4 normal = normals[currIndex];

	//image = (float4)(clamp(image.x, 0.0f, 10.0f), clamp(image.y, 0.0f, 10.0f), clamp(image.z, 0.0f, 10.0f), 0.0f);

	input0p0.x = normal.x; 
	input0p0.y = normal.y;            
	input0p0.z = normal.z;              
	input0p0.w = normal.w;
	input0p1.x = image.x;                        
	input0p1.y = image.y;          
	input0p1.z = image.z;            
	input0p1.w = 1.0f;
	input0p2.x = normal.x * normal.y;   
	input0p2.y = normal.x * normal.z;              
	input0p2.z = normal.x * normal.w;   
	input0p2.w = normal.y * normal.z;
	input0p3.x = normal.y * normal.w; 
	input0p3.y = normal.z * normal.w;          
	input0p3.z = 0.0f;                 
	input0p3.w = 0.0f;

	input1p0.x = normal.x * image.x;
	input1p0.y = normal.x * image.y;
	input1p0.z = normal.x * image.z;
	input1p0.w = normal.x * normal.x;
	input1p1.x = normal.y * image.x;
	input1p1.y = normal.y * image.y;
	input1p1.z = normal.y * image.z;
	input1p1.w = normal.y * normal.y;
	input1p2.x = normal.z * image.x;
	input1p2.y = normal.z * image.y;
	input1p2.z = normal.z * image.z;
	input1p2.w = normal.z * normal.z;
	input1p3.x = normal.w * image.x;
	input1p3.y = normal.w * image.y;
	input1p3.z = normal.w * image.z;
	input1p3.w = normal.w * normal.w;

	GFInput0[0*offset + currIndex] = input0p0;
	GFInput0[1*offset + currIndex] = input0p1;
	GFInput0[2*offset + currIndex] = input0p2;
	GFInput0[3*offset + currIndex] = input0p3;
			  			 
	GFInput1[0*offset + currIndex] = input1p0;
	GFInput1[1*offset + currIndex] = input1p1;
	GFInput1[2*offset + currIndex] = input1p2;
	GFInput1[3*offset + currIndex] = input1p3;

}

__kernel void guidedFilter(__global float4* GFInput0, __global float4* GFInput1, __global  float4* Akf4)
{
	const size_t currIndex = get_global_id(1) * get_global_size(0) + get_global_id(0);
	const size_t x = get_global_id(0);
	const size_t y = get_global_id(1);
	const size_t xSize = get_global_size(0);
	const size_t offset = get_global_size(0) * get_global_size(1);

	int radius = RADIUS;
	float epsilon = 0.01f;
	
	int2 UR = clamp( (int2)(x+radius, y-radius-1),   (int2)(0,0), (int2)(get_global_size(0)-1,get_global_size(1)-1) );
	int2 LR = clamp( (int2)(x+radius, y+radius),     (int2)(0,0), (int2)(get_global_size(0)-1,get_global_size(1)-1) );
	int2 UL = clamp( (int2)(x-radius-1, y-radius-1), (int2)(0,0), (int2)(get_global_size(0)-1,get_global_size(1)-1) );
	int2 LL = clamp( (int2)(x-radius-1, y+radius),   (int2)(0,0), (int2)(get_global_size(0)-1,get_global_size(1)-1) );

	int num = x<=radius ? (y<=radius ? (LR.x+1)*(LR.y+1) : (LR.x+1)*(LR.y-UR.y)) : (y>radius ? (LR.x-LL.x)*(LR.y-UR.y) : (LR.x-LL.x)*(LR.y+1));

	float4 GFI00UR =  GFInput0[0*offset + UR.y*xSize + UR.x];
	float4 GFI01UR =  GFInput0[1*offset + UR.y*xSize + UR.x];
	float4 GFI02UR =  GFInput0[2*offset + UR.y*xSize + UR.x];
	float4 GFI03UR =  GFInput0[3*offset + UR.y*xSize + UR.x];
	float4 GFI00LR =  GFInput0[0*offset + LR.y*xSize + LR.x];
	float4 GFI01LR =  GFInput0[1*offset + LR.y*xSize + LR.x];
	float4 GFI02LR =  GFInput0[2*offset + LR.y*xSize + LR.x];
	float4 GFI03LR =  GFInput0[3*offset + LR.y*xSize + LR.x];
	float4 GFI00UL =  GFInput0[0*offset + UL.y*xSize + UL.x];
	float4 GFI01UL =  GFInput0[1*offset + UL.y*xSize + UL.x];
	float4 GFI02UL =  GFInput0[2*offset + UL.y*xSize + UL.x];
	float4 GFI03UL =  GFInput0[3*offset + UL.y*xSize + UL.x];
	float4 GFI00LL =  GFInput0[0*offset + LL.y*xSize + LL.x];
	float4 GFI01LL =  GFInput0[1*offset + LL.y*xSize + LL.x];
	float4 GFI02LL =  GFInput0[2*offset + LL.y*xSize + LL.x];
	float4 GFI03LL =  GFInput0[3*offset + LL.y*xSize + LL.x];

	float4 GFInput02 = (x<=radius ? (y<=radius ? GFI02LR : GFI02LR-GFI02UR) : (y>radius ? GFI02UL+GFI02LR-GFI02LL-GFI02UR : GFI02LR-GFI02LL))/num;
	float4 GFInput03 = (x<=radius ? (y<=radius ? GFI03LR : GFI03LR-GFI03UR) : (y>radius ? GFI03UL+GFI03LR-GFI03LL-GFI03UR : GFI03LR-GFI03LL))/num;
	float4 mean_I =    (x<=radius ? (y<=radius ? GFI00LR : GFI00LR-GFI00UR) : (y>radius ? GFI00UL+GFI00LR-GFI00LL-GFI00UR : GFI00LR-GFI00LL))/num;
	float4 mean_P =    (x<=radius ? (y<=radius ? GFI01LR : GFI01LR-GFI01UR) : (y>radius ? GFI01UL+GFI01LR-GFI01LL-GFI01UR : GFI01LR-GFI01LL))/num;	
	float xy = GFInput02.x;
	float xz = GFInput02.y;
	float xw = GFInput02.z;
	float yz = GFInput02.w;
	float yw = GFInput03.x;
	float zw = GFInput03.y;

	GFI00UR =  GFInput1[0*offset + UR.y*xSize + UR.x];
	GFI01UR =  GFInput1[1*offset + UR.y*xSize + UR.x];
	GFI02UR =  GFInput1[2*offset + UR.y*xSize + UR.x];
	GFI03UR =  GFInput1[3*offset + UR.y*xSize + UR.x];
	GFI00LR =  GFInput1[0*offset + LR.y*xSize + LR.x];
	GFI01LR =  GFInput1[1*offset + LR.y*xSize + LR.x];
	GFI02LR =  GFInput1[2*offset + LR.y*xSize + LR.x];
	GFI03LR =  GFInput1[3*offset + LR.y*xSize + LR.x];
	GFI00UL =  GFInput1[0*offset + UL.y*xSize + UL.x];
	GFI01UL =  GFInput1[1*offset + UL.y*xSize + UL.x];
	GFI02UL =  GFInput1[2*offset + UL.y*xSize + UL.x];
	GFI03UL =  GFInput1[3*offset + UL.y*xSize + UL.x];	
	GFI00LL =  GFInput1[0*offset + LL.y*xSize + LL.x];
	GFI01LL =  GFInput1[1*offset + LL.y*xSize + LL.x];
	GFI02LL =  GFInput1[2*offset + LL.y*xSize + LL.x];
	GFI03LL =  GFInput1[3*offset + LL.y*xSize + LL.x];

	float4 mean_IP_x = (x<=radius ? (y<=radius ? GFI00LR : GFI00LR-GFI00UR) : (y>radius ? GFI00UL+GFI00LR-GFI00LL-GFI00UR : GFI00LR-GFI00LL))/num;
	float4 mean_IP_y = (x<=radius ? (y<=radius ? GFI01LR : GFI01LR-GFI01UR) : (y>radius ? GFI01UL+GFI01LR-GFI01LL-GFI01UR : GFI01LR-GFI01LL))/num;
	float4 mean_IP_z = (x<=radius ? (y<=radius ? GFI02LR : GFI02LR-GFI02UR) : (y>radius ? GFI02UL+GFI02LR-GFI02LL-GFI02UR : GFI02LR-GFI02LL))/num;
	float4 mean_IP_w = (x<=radius ? (y<=radius ? GFI03LR : GFI03LR-GFI03UR) : (y>radius ? GFI03UL+GFI03LR-GFI03LL-GFI03UR : GFI03LR-GFI03LL))/num;
	float xx = mean_IP_x.w;
	float yy = mean_IP_y.w;
	float zz = mean_IP_z.w;
	float ww = mean_IP_w.w;

	mean_P.w    = 0.0f;
	mean_IP_x.w = 0.0f;
	mean_IP_y.w = 0.0f;
	mean_IP_z.w = 0.0f;
	mean_IP_w.w = 0.0f;


	xx = xx - mean_I.x * mean_I.x + epsilon;
	xy-= mean_I.x * mean_I.y;
	xz-= mean_I.x * mean_I.z;
	xw-= mean_I.x * mean_I.w;
	yy = yy - mean_I.y * mean_I.y + epsilon;
	yz-= mean_I.y * mean_I.z;
	yw-= mean_I.y * mean_I.w;
	zz = zz - mean_I.z * mean_I.z + epsilon;
	zw-= mean_I.z * mean_I.w;
	ww = ww - mean_I.w * mean_I.w + epsilon;

	float yx = xy, zx = xz, wx = xw, zy = yz, wy = yw, wz = zw;

	float invdet = 1.0f/(xw*yz*zy*wx - xz*yw*zy*wx - xw*yy*zz*wx + xy*yw*zz*wx+
						 xz*yy*zw*wx - xy*yz*zw*wx - xw*yz*zx*wy + xz*yw*zx*wy+
						 xw*yx*zz*wy - xx*yw*zz*wy - xz*yx*zw*wy + xx*yz*zw*wy+
						 xw*yy*zx*wz - xy*yw*zx*wz - xw*yx*zy*wz + xx*yw*zy*wz+
						 xy*yx*zw*wz - xx*yy*zw*wz - xz*yy*zx*ww + xy*yz*zx*ww+
						 xz*yx*zy*ww - xx*yz*zy*ww - xy*yx*zz*ww + xx*yy*zz*ww );
    
    float mxx = (yz*zw*wy - yw*zz*wy + yw*zy*wz - yy*zw*wz - yz*zy*ww + yy*zz*ww) * invdet;
    float mxy = (xw*zz*wy - xz*zw*wy - xw*zy*wz + xy*zw*wz + xz*zy*ww - xy*zz*ww) * invdet;
    float mxz = (xz*yw*wy - xw*yz*wy + xw*yy*wz - xy*yw*wz - xz*yy*ww + xy*yz*ww) * invdet;
    float mxw = (xw*yz*zy - xz*yw*zy - xw*yy*zz + xy*yw*zz + xz*yy*zw - xy*yz*zw) * invdet;
    float myx = (yw*zz*wx - yz*zw*wx - yw*zx*wz + yx*zw*wz + yz*zx*ww - yx*zz*ww) * invdet;
    float myy = (xz*zw*wx - xw*zz*wx + xw*zx*wz - xx*zw*wz - xz*zx*ww + xx*zz*ww) * invdet;
    float myz = (xw*yz*wx - xz*yw*wx - xw*yx*wz + xx*yw*wz + xz*yx*ww - xx*yz*ww) * invdet;
    float myw = (xz*yw*zx - xw*yz*zx + xw*yx*zz - xx*yw*zz - xz*yx*zw + xx*yz*zw) * invdet;
    float mzx = (yy*zw*wx - yw*zy*wx + yw*zx*wy - yx*zw*wy - yy*zx*ww + yx*zy*ww) * invdet;
    float mzy = (xw*zy*wx - xy*zw*wx - xw*zx*wy + xx*zw*wy + xy*zx*ww - xx*zy*ww) * invdet;
    float mzz = (xy*yw*wx - xw*yy*wx + xw*yx*wy - xx*yw*wy - xy*yx*ww + xx*yy*ww) * invdet;
    float mzw = (xw*yy*zx - xy*yw*zx - xw*yx*zy + xx*yw*zy + xy*yx*zw - xx*yy*zw) * invdet;
    float mwx = (yz*zy*wx - yy*zz*wx - yz*zx*wy + yx*zz*wy + yy*zx*wz - yx*zy*wz) * invdet;
    float mwy = (xy*zz*wx - xz*zy*wx + xz*zx*wy - xx*zz*wy - xy*zx*wz + xx*zy*wz) * invdet;
    float mwz = (xz*yy*wx - xy*yz*wx - xz*yx*wy + xx*yz*wy + xy*yx*wz - xx*yy*wz) * invdet;
    float mww = (xy*yz*zx - xz*yy*zx + xz*yx*zy - xx*yz*zy - xy*yx*zz + xx*yy*zz) * invdet;

	float4 cov_IP_x = mean_IP_x - mean_I.x*mean_P;
	float4 cov_IP_y = mean_IP_y - mean_I.y*mean_P;
	float4 cov_IP_z = mean_IP_z - mean_I.z*mean_P;
	float4 cov_IP_w = mean_IP_w - mean_I.w*mean_P;

	float4 Akx = (float4) (cov_IP_x.x*mxx + cov_IP_y.x*myx + cov_IP_z.x*mzx + cov_IP_w.x*mwx, cov_IP_x.y*mxx + cov_IP_y.y*mxy + cov_IP_z.y*mxz + cov_IP_w.y*mxw, cov_IP_x.z*mxx + cov_IP_y.z*mxy + cov_IP_z.z*mxz + cov_IP_w.z*mxw, 0.0f);
	float4 Aky = (float4) (cov_IP_x.x*mxy + cov_IP_y.x*myy + cov_IP_z.x*mzy + cov_IP_w.x*mwy, cov_IP_x.y*mxy + cov_IP_y.y*myy + cov_IP_z.y*myz + cov_IP_w.y*myw, cov_IP_x.z*mxy + cov_IP_y.z*myy + cov_IP_z.z*myz + cov_IP_w.z*myw, 0.0f);
	float4 Akz = (float4) (cov_IP_x.x*mxz + cov_IP_y.x*myz + cov_IP_z.x*mzz + cov_IP_w.x*mwz, cov_IP_x.y*mxz + cov_IP_y.y*mzy + cov_IP_z.y*mzz + cov_IP_w.y*mzw, cov_IP_x.z*mxz + cov_IP_y.z*mzy + cov_IP_z.z*mzz + cov_IP_w.z*mzw, 0.0f);
	float4 Akw = (float4) (cov_IP_x.x*mxw + cov_IP_y.x*myw + cov_IP_z.x*mzw + cov_IP_w.x*mww, cov_IP_x.y*mxw + cov_IP_y.y*mwy + cov_IP_z.y*mwz + cov_IP_w.y*mww, cov_IP_x.z*mxw + cov_IP_y.z*mwy + cov_IP_z.z*mwz + cov_IP_w.z*mww, 0.0f);
	float4 Bk0 = (float4)(mean_P.x - Akx.x * mean_I.x - Aky.x * mean_I.y - Akz.x * mean_I.z - Akw.x * mean_I.w,
						  mean_P.y - Akx.y * mean_I.x - Aky.y * mean_I.y - Akz.y * mean_I.z - Akw.y * mean_I.w,
						  mean_P.z - Akx.z * mean_I.x - Aky.z * mean_I.y - Akz.z * mean_I.z - Akw.z * mean_I.w,
						  0.0f);
	Akx.w = Bk0.x;
	Aky.w = Bk0.y;
	Akz.w = Bk0.z;
	Akw.w = Bk0.w;

	Akf4[0*offset + currIndex] = Akx;
	Akf4[1*offset + currIndex] = Aky;
	Akf4[2*offset + currIndex] = Akz;
	Akf4[3*offset + currIndex] = Akw;
}

__kernel void guidedFilterSAT (const int w, const int h ,__global const float4* in, __global  float4* out)
{
	const int globaloffset = h * w;
	size_t currIndex = get_global_id(0);
	int shift = get_global_id(1);

	float4 sum = (float4)(0.0f);
	for(int i = 0; i<h; i++ ){
		sum +=in[shift*globaloffset + currIndex];
		out[shift*globaloffset + currIndex] = sum;
		currIndex += w;
	}
}

__kernel void guidedFilterTranspose(const int w, const int h ,__global const float4* in, __global  float4* out)
{
	const int globaloffset = h * w;

	local float4 aux[256]; 
	int bx = get_group_id(0), 
		by = get_group_id(1);
	int ix = get_local_id(0), 
		iy = get_local_id(1);

	by = (by+bx)%get_num_groups(1);

	in += (bx*16)+(by*16)*w; 
	out += (by*16)+(bx*16)*h;

	for (int i=0; i<4; i++){
		aux[iy+ix*16] = in[i*globaloffset + ix+w*iy]; 
		barrier(CLK_LOCAL_MEM_FENCE);
		out[i*globaloffset + ix+h*iy] = aux[ix+iy*16];
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	/*const size_t x = get_global_id(0);
	const size_t y = get_global_id(1);
	for (int i=0; i<4; i++)
		out[i*globaloffset +  y+x*h] = in[i*globaloffset +  x+y*w];*/

	
}

__kernel void guidedFilterComp( __global  float4* normals, __global  float4* image_in, __global  float4* Ak, __global float4* out)
{

	const size_t x = get_global_id(0);
	const size_t y = get_global_id(1);
	const size_t xSize = get_global_size(0);
	const size_t offset = get_global_size(0) * get_global_size(1);
	const size_t currIndex = get_global_id(1) * get_global_size(0) + get_global_id(0);

	int radius = RADIUS;

	int2 UR = clamp( (int2)(x+radius,   y-radius-1), (int2)(0,0), (int2)(get_global_size(0)-1,get_global_size(1)-1) );
	int2 LR = clamp( (int2)(x+radius,   y+radius  ), (int2)(0,0), (int2)(get_global_size(0)-1,get_global_size(1)-1) );
	int2 UL = clamp( (int2)(x-radius-1, y-radius-1), (int2)(0,0), (int2)(get_global_size(0)-1,get_global_size(1)-1) );
	int2 LL = clamp( (int2)(x-radius-1, y+radius  ), (int2)(0,0), (int2)(get_global_size(0)-1,get_global_size(1)-1) );

	int num = x<=radius ? (y<=radius ? (LR.x+1)*(LR.y+1) : (LR.x+1)*(LR.y-UR.y)) : (y>radius ? (LR.x-LL.x)*(LR.y-UR.y) : (LR.x-LL.x)*(LR.y+1));

	float4 GFI00UR =  Ak[0*offset + UR.y*xSize + UR.x];
	float4 GFI01UR =  Ak[1*offset + UR.y*xSize + UR.x];
	float4 GFI02UR =  Ak[2*offset + UR.y*xSize + UR.x];
	float4 GFI03UR =  Ak[3*offset + UR.y*xSize + UR.x];
	float4 GFI00LR =  Ak[0*offset + LR.y*xSize + LR.x];
	float4 GFI01LR =  Ak[1*offset + LR.y*xSize + LR.x];
	float4 GFI02LR =  Ak[2*offset + LR.y*xSize + LR.x];
	float4 GFI03LR =  Ak[3*offset + LR.y*xSize + LR.x];
	float4 GFI00UL =  Ak[0*offset + UL.y*xSize + UL.x];
	float4 GFI01UL =  Ak[1*offset + UL.y*xSize + UL.x];
	float4 GFI02UL =  Ak[2*offset + UL.y*xSize + UL.x];
	float4 GFI03UL =  Ak[3*offset + UL.y*xSize + UL.x];
	float4 GFI00LL =  Ak[0*offset + LL.y*xSize + LL.x];
	float4 GFI01LL =  Ak[1*offset + LL.y*xSize + LL.x];
	float4 GFI02LL =  Ak[2*offset + LL.y*xSize + LL.x];
	float4 GFI03LL =  Ak[3*offset + LL.y*xSize + LL.x];
	
	float4 Akx = (x<=radius ? (y<=radius ? GFI00LR : GFI00LR-GFI00UR) : (y>radius ? GFI00UL+GFI00LR-GFI00LL-GFI00UR : GFI00LR-GFI00LL))/num;
	float4 Aky = (x<=radius ? (y<=radius ? GFI01LR : GFI01LR-GFI01UR) : (y>radius ? GFI01UL+GFI01LR-GFI01LL-GFI01UR : GFI01LR-GFI01LL))/num;
	float4 Akz = (x<=radius ? (y<=radius ? GFI02LR : GFI02LR-GFI02UR) : (y>radius ? GFI02UL+GFI02LR-GFI02LL-GFI02UR : GFI02LR-GFI02LL))/num;
	float4 Akw = (x<=radius ? (y<=radius ? GFI03LR : GFI03LR-GFI03UR) : (y>radius ? GFI03UL+GFI03LR-GFI03LL-GFI03UR : GFI03LR-GFI03LL))/num;	

	float4 Bk = (float4)(Akx.w, Aky.w, Akz.w, Akw.w);


	out[currIndex] =(Akx*normals[currIndex].x + Aky*normals[currIndex].y + Akz*normals[currIndex].z + Akw*normals[currIndex].w + Bk);
}