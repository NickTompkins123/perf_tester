/* Video utility library. Uses avcodec
* Copyright 2011 by Rightware. All rights reserved.
*/

#ifndef VIDEO_UTIL_H
#define VIDEO_UTIL_H

#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


struct VideoUtil;
struct BfScene;
struct BenchmarkFramework;


/** 
* Init the video util and open a video file. The video is currently always converted to RGBA-8888 pixel format.
* If width and height are 0 the size of the returned frame will be the same as source file
*/
kzsError videoInit(struct BenchmarkFramework *framework, kzString filename, kzUint width, kzUint height, struct VideoUtil **outVideo);

/** Grab the next frame. */
kzsError videoNext(struct BenchmarkFramework *framework, struct VideoUtil *video);

/** Returns the next real frame */
kzsError videoNextFrame(struct BenchmarkFramework *framework, struct VideoUtil *video);

/** Frees the internal data structures and closes the file handle */
kzsError videoUnitialize(struct BenchmarkFramework *framework, struct VideoUtil *video);

/** Return the pointer to the buffer which contains the decompressed and scaled frame */
kzByte* videoGetFramePointer(struct VideoUtil* video);


#endif
