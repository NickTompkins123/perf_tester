/**
* \file
* Project loader for composer.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#ifndef KZU_PROJECT_LOADER_COMPOSER_H
#define KZU_PROJECT_LOADER_COMPOSER_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuBinaryDirectory;
struct KzuProject;
struct KzuComposer;


/** Loads composer from binary. */
kzsException kzuProjectLoaderLoadComposer(struct KzuProject* project, kzString path, struct KzuComposer** out_composer);

/** Unloads composer from project. */
kzsException kzuProjectLoaderUnloadComposer(struct KzuProject* project, const struct KzuComposer* composer);


#endif
