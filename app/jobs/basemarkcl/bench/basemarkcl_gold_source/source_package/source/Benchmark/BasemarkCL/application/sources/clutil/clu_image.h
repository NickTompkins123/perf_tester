/**
 * \file
 * Utility functions for handling images
 * Copyright 2011 by Rightware. All rights reserved.
 */

#ifndef CLU_IMAGE_H
#define CLU_IMAGE_H


#include <clutil/clu_opencl_base.h>

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

/* Forward declarations. */
struct KzcImage;
struct KzcTexture;
struct KzcMemoryManager;
struct BenchmarkFramework;


/* Gets the image width and height and places them into sizes[0] and [1] respectively */
kzsError cluGetImageSize(cl_mem image, size_t *sizes);

/* Load a png, convert it into RGBA-8888 and generate an OpenCL Image of it. If scale > 0 resize the image by dividing the size by scale */
kzsError cluImageLoadPNG(struct BenchmarkFramework* framework, kzString path, cl_context context,cl_mem* out_image, struct KzcImage **out_kzImage, kzUint scale);

/* Save a RGBA-8888 OpenCL image object into RGB-888 png file */
kzsError cluSaveImagePNG(const struct KzcMemoryManager *manager, cl_command_queue queue, cl_mem image, kzString path);

/* Create an image buffer with identical size of the supplied image */
kzsError cluCreateBufferForImage(cl_context context, cl_mem image, cl_mem *out_image);
/* Create an opencl texture, bind it into kanzi material and create a cl image from it */
kzsError cluCreateAndBindGlImage(struct BenchmarkFramework* framework, kzUint width, kzUint height, struct KzcTexture **out_texture, cl_mem *out_image);


#endif
