

// Copyright (C) 2010-2012 Advanced Micro Devices, Inc. All Rights Reserved.

#define HSTRIDE 512
#define VSTRIDE 512
#define DIM 16

__attribute__((reqd_work_group_size(64,1,1)))
__kernel void
fft_trans(__global float2 * restrict gcomplx)
{
    __local float2 ldsa[1024];
    __local float2 ldsb[1024];
    uint gid = get_global_id(0);
    uint me = gid & 0x3fU;
    uint k = (gid >> 6) % 136;

    // Compute location of blocks
    int l = DIM+0.5f - native_sqrt((DIM+0.5f)*(DIM+0.5f) - 2.0f * (float)as_int(k));
    int kl = ((DIM*2+1 - l) * l) >> 1;
    uint j = k - kl;
    uint i = l + j;

    uint goa, gob;
    uint go = ((me & 0x7U) << 2) + ((gid>>6)/136) * VSTRIDE * HSTRIDE;
    __global float4 *gp;
    __local float4 *lp4;
    uint lo = ((me & 0x7U) << 7) + (me >> 3);
    uint lot = (me<<2); 
    __local float2 *lp;
    float4 z00, z01, z10, z11, z20, z21, z30, z31;

    // Array offsets
    goa = go + (i << 5) + j * (HSTRIDE*32) + (me >> 3)*HSTRIDE;

    // Load A block
    gp = (__global float4 *)(gcomplx + goa);
    z00 = gp[0*HSTRIDE/4*16];
    z01 = gp[0*HSTRIDE/4*16 + 1];
    z10 = gp[1*HSTRIDE/4*16];
    z11 = gp[1*HSTRIDE/4*16 + 1];
    z20 = gp[2*HSTRIDE/4*16];
    z21 = gp[2*HSTRIDE/4*16 + 1];
    z30 = gp[3*HSTRIDE/4*16];
    z31 = gp[3*HSTRIDE/4*16 + 1];

    // Save into LDS
    lp = ldsa + lo;
    lp[0*8] = z00.xy;
    lp[1*8] = z10.xy;
    lp[2*8] = z20.xy;
    lp[3*8] = z30.xy;
    lp += 8*4;

    lp[0*8] = z00.zw;
    lp[1*8] = z10.zw;
    lp[2*8] = z20.zw;
    lp[3*8] = z30.zw;
    lp += 8*4;

    lp[0*8] = z01.xy;
    lp[1*8] = z11.xy;
    lp[2*8] = z21.xy;
    lp[3*8] = z31.xy;
    lp += 8*4;

    lp[0*8] = z01.zw;
    lp[1*8] = z11.zw;
    lp[2*8] = z21.zw;
    lp[3*8] = z31.zw;
    lp += 8*4;

    //End load A block

    //  Load B block
    gob = go + (j << 5) + i * (HSTRIDE*32) + (me >> 3)*HSTRIDE;
    gp = (__global float4 *)(gcomplx + gob);
    z00 = gp[0*HSTRIDE/4*16];
    z01 = gp[0*HSTRIDE/4*16 + 1];
    z10 = gp[1*HSTRIDE/4*16];
    z11 = gp[1*HSTRIDE/4*16 + 1];
    z20 = gp[2*HSTRIDE/4*16];
    z21 = gp[2*HSTRIDE/4*16 + 1];
    z30 = gp[3*HSTRIDE/4*16];
    z31 = gp[3*HSTRIDE/4*16 + 1];

    // Save into LDS
    lp = ldsb + lo;
    lp[0*8] = z00.xy;
    lp[1*8] = z10.xy;
    lp[2*8] = z20.xy;
    lp[3*8] = z30.xy;
    lp += 8*4;

    lp[0*8] = z00.zw;
    lp[1*8] = z10.zw;
    lp[2*8] = z20.zw;
    lp[3*8] = z30.zw;
    lp += 8*4;

    lp[0*8] = z01.xy;
    lp[1*8] = z11.xy;
    lp[2*8] = z21.xy;
    lp[3*8] = z31.xy;
    lp += 8*4;

    lp[0*8] = z01.zw;
    lp[1*8] = z11.zw;
    lp[2*8] = z21.zw;
    lp[3*8] = z31.zw;
    lp += 8*4;

    // End load B block

    barrier(CLK_LOCAL_MEM_FENCE);

    // write A block
    goa = go + (i << 5) + j * (VSTRIDE*32) + (me >> 3)*VSTRIDE;
    gp = (__global float4 *)(gcomplx + goa);
    lp4 = (__local float4 *)(ldsb + lot);
    z00 = lp4[0];
    z01 = lp4[1];
    lp4 += 32*4;

    z10 = lp4[0];
    z11 = lp4[1];
    lp4 += 32*4;

    z20 = lp4[0];
    z21 = lp4[1];
    lp4 += 32*4;

    z30 = lp4[0];
    z31 = lp4[1];

    gp[0*VSTRIDE/4*16] = z00;
    gp[0*VSTRIDE/4*16+1] = z01;
    gp[1*VSTRIDE/4*16] = z10;
    gp[1*VSTRIDE/4*16+1] = z11;
    gp[2*VSTRIDE/4*16] = z20;
    gp[2*VSTRIDE/4*16+1] = z21;
    gp[3*VSTRIDE/4*16] = z30;
    gp[3*VSTRIDE/4*16+1] = z31;

    // End write A block;

    // write B block

    gob = go + (j << 5) + i * (VSTRIDE*32) + (me >> 3)*VSTRIDE;
    gp = (__global float4 *)(gcomplx + gob);
    lp4 = (__local float4 *)(ldsa + lot);

    z00 = lp4[0];
    z01 = lp4[1];
    lp4 += 32*4;

    z10 = lp4[0];
    z11 = lp4[1];
    lp4 += 32*4;

    z20 = lp4[0];
    z21 = lp4[1];
    lp4 += 32*4;

    z30 = lp4[0];
    z31 = lp4[1];

    gp[0*VSTRIDE/4*16] = z00;
    gp[0*VSTRIDE/4*16+1] = z01;
    gp[1*VSTRIDE/4*16] = z10;
    gp[1*VSTRIDE/4*16+1] = z11;
    gp[2*VSTRIDE/4*16] = z20;
    gp[2*VSTRIDE/4*16+1] = z21;
    gp[3*VSTRIDE/4*16] = z30;
    gp[3*VSTRIDE/4*16+1] = z31;

    // End write B block;

}


