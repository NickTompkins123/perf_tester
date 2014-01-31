/**
* \file
* Project loader for image files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_IMAGE_H
#define KZU_PROJECT_LOADER_IMAGE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzcImage;


/** Loads png image from binary. */
kzsException kzuProjectLoaderLoadImagePng(const struct KzuProject* project, kzString path, struct KzcImage** out_image);

/** Loads jpg image from binary. */
kzsException kzuProjectLoaderLoadImageJpeg(const struct KzuProject* project, kzString path, struct KzcImage** out_image);

/** Loads ETC image from binary. */
kzsException kzuProjectLoaderLoadImageETC(const struct KzuProject* project, kzString path, struct KzcImage** out_image);

/** Loads DXT image from binary. */
kzsException kzuProjectLoaderLoadImageDXT(const struct KzuProject* project, kzString path, struct KzcImage** out_image);

/** Loads mipmap image from binary. */
kzsException kzuProjectLoaderLoadImageMipmap(const struct KzuProject* project, kzString path, struct KzcImage** out_image);

/** Loads image from binary. */
kzsException kzuProjectLoaderLoadImage(const struct KzuProject* project, kzString path, struct KzcImage** out_image);

/** Unloads image from project. */
kzsException kzuProjectLoaderUnloadImage(struct KzuProject* project, const struct KzcImage* image);


#endif
