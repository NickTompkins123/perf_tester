/**
* \file
* Screen capturing utility.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzc_screen_capture.h"

#include "kzc_image.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/memory/kzc_memory_quick.h>

#include <system/wrappers/kzs_memory.h>
#include <system/display/kzs_window.h>
#include <system/wrappers/kzs_opengl.h> /* Needed for ReadPixels */


/* Does not work effectively.*/
kzsError kzcScreenCaptureToImage(const struct KzcMemoryManager* memoryManager, const struct KzsWindow* window,
                                 struct KzcImage** out_image)
{
    kzsError result;

    kzUint width;
    kzUint height;
    kzUint sizeBytes;
    kzUint size;
    void* pixels;

    struct KzcImage* image;

    kzsAssert(memoryManager != KZ_NULL);
    kzsAssert(window != KZ_NULL);

    width = kzsWindowGetWidth(window);
    height = kzsWindowGetHeight(window);
    size = width * height;

    sizeBytes = size * 3;

    /* Allocate pixels. */
    result = kzcMemoryAllocPointer(memoryManager, &pixels, sizeBytes, "Temporary screen capture buffer");
    kzsErrorForward(result);

    /* Use either GL or VG read pixel. TODO: find out which to use */
    kzsGlReadPixels(0, 0, (kzInt)width, (kzInt)height, KZS_GL_RGB, KZS_GL_UNSIGNED_BYTE, pixels);

    result = kzcImageLoadMemory(memoryManager, width, height, KZC_IMAGE_DATA_FORMAT_RGB_888, pixels, &image);
    kzsErrorForward(result);

    result = kzcImageFlipVertically(image);
    kzsErrorForward(result);

    result = kzcMemoryFreePointer(pixels);
    kzsErrorForward(result);

    *out_image = image;
    kzsSuccess();
}
