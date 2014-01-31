/**
* \file
* Benchmark framework get screenshot.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/

#include "bf_screenshot.h"

#include <benchmarkutil/bf_benchmark_framework.h>
#include <benchmarkutil/util/bf_util.h>

#include <core/util/image/kzc_image.h>
#include <core/util/image/kzc_screen_capture.h>
#include <core/util/io/kzc_output_stream.h>
#include <core/util/string/kzc_string.h>
#include <core/memory/kzc_memory_manager.h>

#include <system/debug/kzs_error.h>
#include <system/file/kzs_file.h>


kzsError bfWriteImageToFile( const struct KzcMemoryManager* manager, kzString path,const struct KzcImage* image )
{
    kzsError result;
    struct KzcOutputStream* outputStream;

    /* Open output stream */
    result = kzcOutputStreamCreateToFile(manager, path, KZC_IO_STREAM_ENDIANNESS_PLATFORM, &outputStream);
    kzsErrorForward(result);

    /* Write */
    result = kzcImageSavePNG(image, outputStream);
    kzsErrorForward(result);

    result = kzcOutputStreamDelete(outputStream);
    kzsErrorForward(result);

    kzsSuccess();
}

kzsError bfSaveScreenshot( const struct BenchmarkFramework* bf )
{
    kzsError result;
    struct KzcImage* image;
    struct KzcMemoryManager* memoryManager;
    struct KzsWindow* window;

    memoryManager = bfGetQuickManager(bf);

    /* Take pixels */
    window = bfGetWindow(bf);

    result = kzcScreenCaptureToImage(memoryManager, window, &image);
    kzsErrorForward(result);

    /* Compose output name. */
    {
        /*
        struct BfTestLogNameCollection* logNameCollection;
        struct BfSceneLogName* logName;
        kzUint frameIndex = bfSceneGetFrameIndex(scene);
        kzMutableString outName;
        logNameCollection = bfSceneGetLogFileCollection(scene);
        logName = bfSceneLogNameCollectionGetScreenshot(logNameCollection,frameIndex);

        kzsErrorTest(logName != KZ_NULL, KZS_ERROR_ILLEGAL_OPERATION, "Could not find specified indexed filename for screenshot");

        result = bfComposeReportfileName(bf, scene, logName, &outName);
        kzsErrorForward(result);

        result = bfComposeCompleteReportFilePath(bf, outName, &outPath);
        kzsErrorForward(result);

        result = bfFileIndexAdd_private(bf,logName->parameters , outName);
        kzsErrorForward(result);

        result = kzcStringDelete(outName);
        kzsErrorForward(result);
        */
    }

    /*
    result = bfWriteImageToFile(memoryManager, outPath, image);
    kzsErrorForward(result);

    result = kzcStringDelete(outPath);
    kzsErrorForward(result);
    */

    kzsSuccess();
}

kzsError writeTga(kzString path, const struct KzcImage* image)
{
    kzsError result;
    kzUint height = kzcImageGetHeight(image);
    kzUint width = kzcImageGetWidth(image);

    kzByte* pixels = kzcImageGetData(image);
    kzUint imageSize = height*width*3;

    #define WLOW 0
    #define WHI 0
    #define HLOW 0
    #define HHI 0

    kzByte header[] = 
    {
            0, /* ID length */
            0, /* no color map */
            2, /* uncompressed, true color */
            0, 0, 0, 0,
            0,
            0, 0, 0, 0, /* x and y origin */
            WLOW, /*12*/
            WHI, /*13*/
            HLOW, /*14*/
            HHI, /*15*/
            24, /* 32|24 bit bitmap */
            0 
    };

    kzsFile* outFile;
    kzUint i;
    kzUint pixelCount = width * height;
    kzByte* pixPtr = pixels;

    result = kzcImageFlipVertically(image);
    kzsErrorForward(result);

    kzsAssert(pixPtr != KZ_NULL);

    header[12] = (kzByte)(width & 0x00FF);
    header[13] = (kzByte)((width & 0xFF00) >> 8);
    header[14] = (kzByte)(height & 0x00FF);
    header[15] = (kzByte)((height & 0xFF00) >> 8);

    /* Swap bgr -> rgb */
    for(i = 0; i < pixelCount; ++i)
    {
        kzByte tmp = pixPtr[0];
        pixPtr[0] = pixPtr[2];
        pixPtr[2] = tmp;
        pixPtr +=3 ;
    }
    outFile = kzsFopen(path, "wb");

    kzsErrorTest(kzsFwrite(header, sizeof(header), 1, outFile)== 1,KZS_ERROR_ASSERTION_FAILED,"Could not write to file") ;
    kzsErrorTest(kzsFwrite(pixels, imageSize, 1, outFile) == 1, KZS_ERROR_ASSERTION_FAILED, "Could not write to file");

    kzsErrorTest(kzsFclose(outFile) == 0, KZS_ERROR_ASSERTION_FAILED, "Could not close file handle");

    kzsSuccess();
}

kzsError bfSaveScreenshotToFile( const struct BenchmarkFramework* bf, kzString fileName )
{
    kzsError result;
    struct KzcImage* image;
    struct KzcMemoryManager* memoryManager;
    struct KzsWindow* window;

    memoryManager = bfGetQuickManager(bf);

    window = bfGetWindow(bf);
    result = kzcScreenCaptureToImage(memoryManager, window, &image);
    kzsErrorForward(result);

    result = writeTga(fileName, image);
    kzsErrorForward(result);

    kzsSuccess();
}

