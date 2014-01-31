/**
* \file
* OpenCL test definitions and hashes.
* Copyright 2011 by Rightware. All rights reserved.
*/
#ifndef TEST_DEFINITIONS_H
#define TEST_DEFINITIONS_H


#if 1
/** SPH test framecount. */
#define SPH_FRAME_COUNT 800
/** Fluid test framecount. */
#define FLUID_FRAME_COUNT 750
/** Soft body test framecount. */
#define SOFT_BODY_FRAME_COUNT (150*5)
/** Wave simulation test framecount. */
#define WAVE_FRAME_COUNT 1000
/** Mandelbulb simulation test framecount. */
#define MANDELBULB_FRAME_COUNT 400
/** Julia fractal test framecount. */
#define JULIA_FRAME_COUNT 1000
/** Image test test framecount. */
#define IMAGE_FRAME_COUNT 500
#else
/** SPH test framecount. */
#define SPH_FRAME_COUNT 8
/** Fluid test framecount. */
#define FLUID_FRAME_COUNT 10
/** Soft body test framecount. */
#define SOFT_BODY_FRAME_COUNT (5)
/** Wave simulation test framecount. */
#define WAVE_FRAME_COUNT 20
/** Mandelbulb simulation test framecount. */
#define MANDELBULB_FRAME_COUNT 40
/** Julia fractal test framecount. */
#define JULIA_FRAME_COUNT 10
/** Image test test framecount. */
#define IMAGE_FRAME_COUNT 10
#endif



/** Kernels. */
#define BASIC_IMAGE_OPERATIONS_HASH  "22a29b76cc84d6a2a0f2eb0ded945f89"
#define BILATERAL_HASH               "9b907123194ce3a3be690d6e87f1b24d"
#define CONVOLUTION_HASH             "c100da7616793ab9bdd2ab91e2c4cde7"
#define FFT_HASH                     "fae106975c640e20d223f7edce7f76da"
#define FLUID_HASH                   "9ee5e2a9488bce18acbbe9c9108707e0"
#define HISTOGRAM_HASH               "03b36115e121bfd77509840909978952"
#define JULIA_HASH                   "bffdb74c8fd4b2ff95b0afaa7682aa22"
#define MANDELBULB_HASH              "050be8cb16410b1cfad7629dde09c4fa"
#define MEDIAN_HASH                  "2003783c84e5405c2037cfb2c95160ff"
#define SEPARABLE_CONVOLUTION2D_HASH "10548a6b5ae73077614cb10396ddd3d2"
#define SOFTBODY_HASH                "9d2a4e449c76d1e2dcc43892b88ac2be"
#define SPH_HASH                     "c0bf52a56e01f0154e8f545b04cba9c9"
#define WAVE_SIMULATION_HASH         "39190616006a04eb8e514bbcac977f48"


/** Images. */
#define IMAGE_TEST_HASH1             "2e6b2d8f450087f7811708bad2984e28"
#define IMAGE_TEST_HASH2             "ca848e7065e0a1a2e40c06e09b480b79"
#define IMAGE_TEST_HASH3             "cfdf07766415d1f84723a5d4f7060abe"
#define IMAGE_TEST_HASH4             "af21bc93b08d4b6f4c0c4e70127a9265"
#define IMAGE_TEST_HASH5             "b4b004d957edc906fed1b36bb14ad9d0"
#define IMAGE_JULIA_COLOR_MAP_HASH   "e62708587fbfe348f6c9ff70e7762c2b"

/** Videos. */
#define VIDEO_MOVIE1                 "316e0ae9145002c911efaf829d62f9e4"
#define VIDEO_FIRST_FRAME            "f2ba684ae8b4b90e78a71aa914f8e02c"


/** Compiler flags. */
#define COMPILER_FLAGS "-cl-fast-relaxed-math"
/*#define COMPILER_FLAGS "-cl-opt-disable"*/


/** Image test weights. Weight varies because histogram test is excluded from embedded version. */
#define IMAGE_TEST_WEIGHT_FULL 0.4f
#define IMAGE_TEST_WEIGHT_EMBEDDED 0.5f

/** Video test weights. Weight varies because histogram test is excluded from embedded version. */
#define VIDEO_TEST_WEIGHT_FULL 0.2f
#define VIDEO_TEST_WEIGHT_EMBEDDED 0.25f


#endif
