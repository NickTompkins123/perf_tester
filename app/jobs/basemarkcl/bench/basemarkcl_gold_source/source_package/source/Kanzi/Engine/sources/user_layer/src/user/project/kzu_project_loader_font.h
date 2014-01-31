/**
* \file
* Project loader for font files.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_FONT_H
#define KZU_PROJECT_LOADER_FONT_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzcBitmapFont;
struct KzcFont;


/** Loads bitmap font from binary. */
kzsException kzuProjectLoaderLoadBitmapFont(struct KzuProject* project, kzString path, struct KzcBitmapFont** out_bitmapFont);

/** Loads TrueType font from binary. */
kzsException kzuProjectLoaderLoadTruetypeFont(struct KzuProject* project, kzString path, struct KzcFont** out_font);

/** Loads any supported font type from binary. */
kzsException kzuProjectLoaderLoadFont(struct KzuProject* project, kzString path, struct KzcFont** out_font);

/** Unloads font from project. */
kzsException kzuProjectLoaderUnloadFont(struct KzuProject* project, const struct KzcFont* font);


#endif
