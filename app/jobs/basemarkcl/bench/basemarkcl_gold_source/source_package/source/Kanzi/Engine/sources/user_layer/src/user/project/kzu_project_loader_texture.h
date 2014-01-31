/**
* \file
* Project loader for texture.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_TEXTURE_H
#define KZU_PROJECT_LOADER_TEXTURE_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzcTexture;


/** Loads texture from binary. */
kzsException kzuProjectLoaderLoadTexture(const struct KzuProject* project, kzString path, struct KzcTexture** out_texture);

/** Unloads texture from project. */
kzsException kzuProjectLoaderUnloadTexture(struct KzuProject* project, const struct KzcTexture* texture);


#endif
