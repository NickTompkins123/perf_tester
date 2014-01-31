/**
 * \file
 * Utility functions for handling images
 * Copyright 2011 by Rightware. All rights reserved.
 */

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/scene/bf_scene.h>
#include <benchmarkutil/util/bf_util.h>

#include "clu_opencl_base.h"

#include <clutil/clu_platform.h>
#include <clutil/clu_program.h>
#include <clutil/clu_kernel.h>
#include <clutil/clu_image.h>
#include <clutil/clu_util.h>

#include <user/project/kzu_project.h>
#include <user/project/kzu_project_loader_material.h>
#include <user/properties/kzu_fixed_properties.h>
#include <user/properties/kzu_texture_property.h>
#include <user/material/kzu_material.h>

#include <core/memory/kzc_memory_manager.h>
#include <core/util/image/kzc_image.h>
#include <core/resource_manager/texture/kzc_resource_texture.h>
#include <core/resource_manager/texture/kzc_texture_descriptor.h>
#include <core/util/io/kzc_input_stream.h>
#include <core/util/io/kzc_output_stream.h>

#include <system/wrappers/kzs_memory.h>
#include <system/display/kzs_surface.h>
#include <system/display/kzs_window.h>

#include <system/wrappers/kzs_opengl.h>

/*TODO: Decide what to do if we get error from OpenCL */
kzsError cluGetImageSize(cl_mem image, size_t *sizes)
{
    cl_int clResult;
    size_t imsize[2];

    clResult = clGetImageInfo(image, CL_IMAGE_WIDTH, sizeof(size_t), &imsize[0], NULL);
    cluClErrorTest(clResult);
    clResult = clGetImageInfo(image, CL_IMAGE_HEIGHT, sizeof(size_t), &imsize[1], NULL);
    cluClErrorTest(clResult);
    sizes[0] = imsize[0];
    sizes[1] = imsize[1];
    kzsSuccess();
}


kzsError cluCreateAndBindGlImage(struct BenchmarkFramework* framework, kzUint width, kzUint height, struct KzcTexture **out_texture, cl_mem *out_image)
{
    kzsError result;
    cl_int clResult;
    struct KzcMemoryManager *memoryManager;
    /* TODO: load from config. Must be in sync with upper value. */

    kzByte* textureData;
    /* TODO get this value from the actual image format */
    kzUint dataSize = width * height * 4;
    struct KzcTextureDescriptor descriptor;
    struct KzcTexture* texture;
    cl_mem image;
    cl_context context;

    context = bfGetClContext(framework);
    memoryManager = bfGetMemoryManager(framework);

    result = kzcMemoryAllocArray(memoryManager, textureData, dataSize, "Initial black texture");
    kzsErrorForward(result);


    /* TODO: Enable user to choose OpenGL internal format */
    kzcTextureDescriptorSet(width, height, KZC_TEXTURE_FORMAT_RGBA, KZC_TEXTURE_FILTER_POINT_SAMPLE, KZC_TEXTURE_WRAP_CLAMP,
        KZC_TEXTURE_COMPRESSION_NONE, &descriptor);
    result = kzcTextureCreate(kzuProjectGetResourceManager(bfGetProject(framework)), KZC_RESOURCE_MEMORY_TYPE_GPU_ONLY, &descriptor, textureData, &texture);
    kzsErrorForward(result);
    image = clCreateFromGLTexture2D(context, CL_MEM_READ_WRITE, KZS_GL_TEXTURE_2D, 0, kzcTextureGetTextureHandle(texture), &clResult);
    cluClErrorTest(clResult);


    result = kzcMemoryFreeArray(textureData);
    kzsErrorForward(result);

    *out_texture = texture;
    *out_image = image;
    kzsSuccess();
}

kzsError cluImageLoadPNG(struct BenchmarkFramework* framework, kzString path, cl_context context,cl_mem* out_image, struct KzcImage **out_kzImage, kzUint scale)
{
    cl_image_format imformat;
    kzsError result;
    cl_int clResult;
    struct KzcInputStream* stream;
    struct KzcImage* image;
    cl_mem inputImage;
    cl_uchar* imdata;
    kzUint imageWidth;
    kzUint imageHeight;
    struct KzcMemoryManager* manager = bfGetMemoryManager(framework);

    result = kzcInputStreamCreateFromFile(manager, path, KZC_IO_STREAM_ENDIANNESS_PLATFORM, &stream);
    kzsErrorForward(result);
    result = kzcImageLoadPNG(manager, stream, &image);
    kzsErrorForward(result);
    result = kzcImageConvert(image, KZC_IMAGE_DATA_FORMAT_RGBA_8888);
    kzsErrorForward(result);
    result = kzcInputStreamDelete(stream);
    kzsErrorForward(result);

    imageWidth = kzcImageGetWidth(image);
    imageHeight = kzcImageGetHeight(image);
    if(scale > 0)
    {
        imageWidth = imageWidth / scale;
        imageHeight = imageHeight / scale;
        result = kzcImageResize(image, imageWidth, imageHeight, KZC_IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR);
        kzsErrorForward(result);
    }
    imdata = kzcImageGetData(image);
    imformat.image_channel_order = CL_RGBA;
    imformat.image_channel_data_type = CL_UNORM_INT8;
    inputImage = clCreateImage2D(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, &imformat, (size_t)imageWidth, (size_t)imageHeight, 0, imdata, &clResult); 
    cluClErrorTest(clResult);

    *out_kzImage = image;
    *out_image = inputImage;

    kzsSuccess();
}

kzsError cluSaveImagePNG(const struct KzcMemoryManager *manager, cl_command_queue queue, cl_mem image, kzString path)
{
    kzsError result;
    cl_int clResult;
    struct KzcImage *kzImage;
    struct KzcOutputStream *out_stream;
    cl_uchar *imdata;
    size_t imsize[3] = {1,1,1};
    size_t read_origin[3] = {0,0,0};

    result = cluGetImageSize(image, imsize);
    kzsErrorForward(result);

    result = kzcMemoryAllocArray(manager, imdata, imsize[0]*imsize[1]*4, "Buffer for saving an image");
    kzsErrorForward(result);

    clResult = clEnqueueReadImage(queue, image, CL_TRUE, read_origin, imsize, 0, 0, imdata, 0, NULL, NULL);
    cluClErrorTest(clResult);

    result = kzcImageLoadMemory(manager, imsize[0], imsize[1], KZC_IMAGE_DATA_FORMAT_RGBA_8888, imdata, &kzImage);
    kzsErrorForward(result);
    result = kzcMemoryFreeArray(imdata);
    kzsErrorForward(result);

    result = kzcImageConvert(kzImage, KZC_IMAGE_DATA_FORMAT_RGB_888);
    kzsErrorForward(result);

    result = kzcOutputStreamCreateToFile(manager, path, KZC_IO_STREAM_ENDIANNESS_PLATFORM, &out_stream);
    kzsErrorForward(result);
    result = kzcImageSavePNG(kzImage, out_stream);
    kzsErrorForward(result);
    result = kzcOutputStreamDelete(out_stream);
    kzsErrorForward(result);

    result = kzcImageDelete(kzImage);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError cluCreateBufferForImage(cl_context context, cl_mem image, cl_mem *out_image)
{
    kzsError result;
    cl_int clResult;
    size_t imsize[2];
    cl_image_format imformat;
    cl_mem outImage;

    result = cluGetImageSize(image, imsize);
    kzsErrorForward(result);
    clResult = clGetImageInfo(image, CL_IMAGE_FORMAT, sizeof(cl_image_format), &imformat, NULL);
    cluClErrorTest(clResult);

    outImage = clCreateImage2D(context, CL_MEM_READ_WRITE, &imformat, imsize[0], imsize[1], 0, NULL, &clResult);
    cluClErrorTest(clResult);

    *out_image = outImage;

    kzsSuccess();
}
