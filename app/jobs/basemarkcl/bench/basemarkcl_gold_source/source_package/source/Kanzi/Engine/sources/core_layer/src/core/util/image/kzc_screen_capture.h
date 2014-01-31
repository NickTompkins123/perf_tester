/**
* \file
* Screen capturing utility.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_SCREEN_CAPTURE_H
#define KZC_SCREEN_CAPTURE_H


#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzcMemoryManager;
struct KzcImage;
struct KzsWindow;


/** Creates image from color buffer attached to window */
kzsError kzcScreenCaptureToImage(const struct KzcMemoryManager* memoryManager, const struct KzsWindow* window,
                                 struct KzcImage** out_image);


#endif
